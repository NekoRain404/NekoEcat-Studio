#include "OpcUaServer.h"
#include "infra/EcatClient.h"

#include <open62541.h>

#include <QDebug>
#include <QThread>

// ── OPC UA node IDs ──────────────────────────────────────────────────
static constexpr UA_UInt16 NS_ID = 1;
static constexpr int kPollIntervalMs = 1000;

// ── Constructor ──────────────────────────────────────────────────────
OpcUaServer::OpcUaServer(EcatClient* client, QObject* parent) : QObject(parent), client_(client) {}

OpcUaServer::~OpcUaServer() {
    stop();
}

// ── Start ────────────────────────────────────────────────────────────
bool OpcUaServer::start(quint16 port) {
    if (running_)
        return true;
    port_ = port;

    server_ = UA_Server_new();
    if (!server_) {
        qWarning() << "OPC UA: failed to create server";
        return false;
    }

    config_ = UA_Server_getConfig(server_);
    UA_ServerConfig_setMinimal(config_, port_, nullptr);
    config_->logging = UA_Log_Stdout_new(UA_LOGLEVEL_ERROR);

    UA_StatusCode retval = UA_Server_addNamespace(server_, "http://nekoecat.local/opcua/");
    if (retval != UA_STATUSCODE_GOOD) {
        qWarning() << "OPC UA: namespace failed:" << retval;
        UA_Server_delete(server_);
        server_ = nullptr;
        return false;
    }

    retval = UA_Server_run_startup(server_);
    if (retval != UA_STATUSCODE_GOOD) {
        qWarning() << "OPC UA: startup failed:" << retval;
        UA_Server_delete(server_);
        server_ = nullptr;
        return false;
    }

    running_ = true;
    setupObjectTypes();
    setupSdoPolling();

    std::thread([this]() { UA_Server_run(server_, &running_); }).detach();

    qInfo().noquote() << QString("OPC UA server started on port %1").arg(port_);
    emit serverStarted(port_);
    return true;
}

// ── Stop ─────────────────────────────────────────────────────────────
void OpcUaServer::stop() {
    if (!running_)
        return;
    running_ = false;
    if (sdoPollTimer_)
        sdoPollTimer_->stop();
    QThread::msleep(200);
    if (server_) {
        UA_Server_run_shutdown(server_);
        UA_Server_delete(server_);
        server_ = nullptr;
        config_ = nullptr;
    }
    qInfo() << "OPC UA server stopped";
    emit serverStopped();
}

// ── Create address space ─────────────────────────────────────────────
void OpcUaServer::setupObjectTypes() {
    if (!server_)
        return;
    auto addFolder = [&](UA_UInt32 id, const UA_NodeId& parent, const char* name) {
        UA_ObjectAttributes attr = UA_ObjectAttributes_default;
        attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", (char*)name);
        UA_QualifiedName qn = UA_QUALIFIEDNAME_ALLOC(NS_ID, name);
        UA_NodeId nodeId = UA_NODEID_NUMERIC(NS_ID, id);
        UA_Server_addObjectNode(server_, nodeId, parent, UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), qn,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), attr, nullptr, &nodeId);
        UA_QualifiedName_clear(&qn);
        return nodeId;
    };
    UA_NodeId objectsFolder = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId ecatRoot = addFolder(1000, objectsFolder, "EtherCAT");
    UA_NodeId master0 = addFolder(2000, ecatRoot, "Master0");
    addFolder(3000, master0, "Slaves");
}

// ── Add slave nodes ──────────────────────────────────────────────────
void OpcUaServer::addSlaveNodes() {
    if (!server_)
        return;
    QMutexLocker lock(&mutex_);
    UA_NodeId slavesFolderId = UA_NODEID_NUMERIC(NS_ID, 3000);

    for (int i = 0; i < slaves_.size(); ++i) {
        const auto& sl = slaves_[i];
        UA_UInt32 nodeBase = 4000 + i * 10;

        UA_ObjectAttributes slaveAttr = UA_ObjectAttributes_default;
        QByteArray nameBytes = sl.name.toUtf8();
        slaveAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", nameBytes.constData());
        UA_NodeId slaveNodeId = UA_NODEID_NUMERIC(NS_ID, nodeBase);
        UA_QualifiedName qn = UA_QUALIFIEDNAME_ALLOC(NS_ID, nameBytes.constData());
        UA_Server_addObjectNode(server_, slaveNodeId, slavesFolderId, UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), qn,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), slaveAttr, nullptr, &slaveNodeId);
        UA_QualifiedName_clear(&qn);

        auto addStatic = [&](UA_UInt32 sub, const char* n, const QString& v) {
            UA_VariableAttributes va = UA_VariableAttributes_default;
            va.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", (char*)n);
            va.accessLevel = UA_ACCESSLEVELMASK_READ;
            QByteArray vb = v.toUtf8();
            UA_String us = UA_STRING_ALLOC(vb.constData());
            UA_Variant_setScalar(&va.value, &us, &UA_TYPES[UA_TYPES_STRING]);
            UA_NodeId nid = UA_NODEID_NUMERIC(NS_ID, nodeBase + sub);
            UA_QualifiedName q = UA_QUALIFIEDNAME_ALLOC(NS_ID, n);
            UA_Server_addVariableNode(server_, nid, slaveNodeId, UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), q,
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), va, nullptr, nullptr);
            UA_String_clear(&us);
            UA_QualifiedName_clear(&q);
        };
        addStatic(1, "Position", QByteArray::number(sl.position));
        addStatic(2, "Name", sl.name);
        addStatic(3, "State", sl.state);
        addStatic(4, "Flags", sl.flags);

        addSdoNodes(i, nodeBase, slaveNodeId);
    }
}

// ── Add SDO variable nodes ───────────────────────────────────────────
void OpcUaServer::addSdoNodes(int slaveIndex, UA_UInt32 nodeBase, const UA_NodeId&) {
    struct SdoDef {
        const char* name;
        const char* index;
        const char* sub;
    };
    static const SdoDef sdos[] = {
        {"DeviceType", "0x1000", "0"}, {"ErrorRegister", "0x1001", "0"}, {"DeviceName", "0x1008", "0"},
        {"VendorID", "0x1018", "1"},   {"ProductCode", "0x1018", "2"},   {"Revision", "0x1018", "3"},
        {"SerialNo", "0x1018", "4"},
    };

    // SDO folder (nodeBase + 9)
    UA_ObjectAttributes sdoAttr = UA_ObjectAttributes_default;
    sdoAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", "SDO");
    UA_QualifiedName sdoQn = UA_QUALIFIEDNAME_ALLOC(NS_ID, "SDO");
    UA_Server_addObjectNode(server_, UA_NODEID_NUMERIC(NS_ID, nodeBase + 9), UA_NODEID_NUMERIC(NS_ID, nodeBase),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), sdoQn, UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
                            sdoAttr, nullptr, nullptr);
    UA_QualifiedName_clear(&sdoQn);

    for (int s = 0; s < 7; ++s) {
        const auto& def = sdos[s];
        QString key = QStringLiteral("%1:%2:%3").arg(slaveIndex).arg(def.index).arg(def.sub);
        {
            QMutexLocker lock(&sdoCacheMutex_);
            sdoCache_.insert(key, QStringLiteral("—"));
        }

        MonitoredSdo ms;
        ms.slavePos = slaveIndex;
        ms.index = def.index;
        ms.subIndex = def.sub;
        monitoredSdos_.append(ms);

        // Store mapping: key → (nodeBase, sdoIndex)
        sdoNodeMap_.insert(key, {nodeBase, s});

        UA_VariableAttributes va = UA_VariableAttributes_default;
        va.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", (char*)def.name);
        va.accessLevel = UA_ACCESSLEVELMASK_READ;
        UA_String init = UA_STRING_ALLOC("—");
        UA_Variant_setScalar(&va.value, &init, &UA_TYPES[UA_TYPES_STRING]);
        UA_QualifiedName varQn = UA_QUALIFIEDNAME_ALLOC(NS_ID, def.name);
        UA_Server_addVariableNode(server_, UA_NODEID_NUMERIC(NS_ID, nodeBase + 20 + s),
                                  UA_NODEID_NUMERIC(NS_ID, nodeBase + 9), UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                  varQn, UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), va, new QString(key),
                                  nullptr);
        UA_QualifiedName_clear(&varQn);
        UA_String_clear(&init);
    }
}

// ── Remove slave nodes ───────────────────────────────────────────────
void OpcUaServer::removeSlaveNodes() {
    if (!server_)
        return;
    QMutexLocker lock(&mutex_);
    monitoredSdos_.clear();
    sdoNodeMap_.clear();
}

// ── Setup polling timer ──────────────────────────────────────────────
void OpcUaServer::setupSdoPolling() {
    if (!client_)
        return;
    sdoPollTimer_ = new QTimer(this);
    connect(sdoPollTimer_, &QTimer::timeout, this, &OpcUaServer::pollSdoValues);
    sdoPollTimer_->start(kPollIntervalMs);

    connect(client_, &EcatClient::sdoValue, this,
            [this](int pos, const QString& idx, const QString& sub, const QString& val) {
                QString key = QStringLiteral("%1:%2:%3").arg(pos).arg(idx).arg(sub);
                QMutexLocker lock(&sdoCacheMutex_);
                if (!sdoCache_.contains(key))
                    return;
                sdoCache_[key] = val;

                // Update the OPC UA variable node
                auto it = sdoNodeMap_.constFind(key);
                if (it == sdoNodeMap_.constEnd() || !server_)
                    return;
                auto [nodeBase, sdoIdx] = it.value();
                UA_NodeId nodeId = UA_NODEID_NUMERIC(NS_ID, nodeBase + 20 + sdoIdx);

                UA_Variant uaVal;
                UA_Variant_init(&uaVal);
                QByteArray vb = val.toUtf8();
                UA_String us = UA_STRING_ALLOC(vb.constData());
                UA_Variant_setScalar(&uaVal, &us, &UA_TYPES[UA_TYPES_STRING]);
                UA_Server_writeValue(server_, nodeId, uaVal);
                UA_String_clear(&us);
                UA_Variant_clear(&uaVal);
            });
}

// ── Poll SDO values ──────────────────────────────────────────────────
void OpcUaServer::pollSdoValues() {
    if (!client_ || !client_->isConnected())
        return;
    for (const auto& ms : monitoredSdos_)
        client_->upload(ms.slavePos, ms.index, ms.subIndex);
}

// ── Update slaves ────────────────────────────────────────────────────
void OpcUaServer::updateSlaves(const QVector<SlaveInfo>& slaves) {
    {
        QMutexLocker lock(&mutex_);
        slaves_ = slaves;
    }
    removeSlaveNodes();
    addSlaveNodes();
}