#include "OpcUaServer.h"
#include "infra/EcatClient.h"

#include <open62541.h>

#include <QDebug>
#include <QJsonObject>
#include <QThread>
#include <thread>

// ── OPC UA node IDs for our EtherCAT address space ──────────────────
static constexpr UA_UInt16 NS_ID = 1;

// ── Constructor / Destructor ─────────────────────────────────────────
OpcUaServer::OpcUaServer(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {}

OpcUaServer::~OpcUaServer() { stop(); }

// ── Start ────────────────────────────────────────────────────────────
bool OpcUaServer::start(quint16 port) {
    if (running_) return true;
    port_ = port;

    server_ = UA_Server_new();
    if (!server_) {
        qWarning() << "OPC UA: failed to create server";
        return false;
    }

    config_ = UA_Server_getConfig(server_);
    UA_ServerConfig_setMinimal(config_, port_, nullptr);

    // Add the EtherCAT namespace
    UA_StatusCode retval = UA_Server_addNamespace(server_, "http://nekoecat.local/opcua/");
    if (retval != UA_STATUSCODE_GOOD) {
        qWarning() << "OPC UA: failed to add namespace:" << retval;
        UA_Server_delete(server_);
        server_ = nullptr;
        return false;
    }

    retval = UA_Server_run_startup(server_);
    if (retval != UA_STATUSCODE_GOOD) {
        qWarning() << "OPC UA: server startup failed:" << retval;
        UA_Server_delete(server_);
        server_ = nullptr;
        return false;
    }

    running_ = true;
    setupObjectTypes();

    // Start the server in a background thread
    std::thread([this]() {
        UA_Server_run(server_, &running_);
    }).detach();

    qInfo().noquote() << QString("OPC UA server started on port %1").arg(port_);
    emit serverStarted(port_);
    return true;
}

// ── Stop ─────────────────────────────────────────────────────────────
void OpcUaServer::stop() {
    if (!running_) return;
    running_ = false;
    // Give the server thread time to exit
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

// ── Create the EtherCAT address space ────────────────────────────────
void OpcUaServer::setupObjectTypes() {
    if (!server_) return;

    // Helper: add a folder node
    UA_NodeId slavesFolderId;
    auto addFolder = [&](UA_UInt32 id, const UA_NodeId &parent, const char *name) {
        UA_ObjectAttributes attr = UA_ObjectAttributes_default;
        attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US"), (char *)name);
        UA_QualifiedName qn = UA_QUALIFIEDNAME_ALLOC(NS_ID, name);
        UA_NodeId nodeId = UA_NODEID_NUMERIC(NS_ID, id);
        UA_Server_addObjectNode(server_, nodeId, parent,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                qn,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
                                attr, nullptr, &nodeId);
        UA_QualifiedName_clear(&qn);
        if (strcmp(name, "Slaves") == 0) slavesFolderId = nodeId;
        return nodeId;
    };

    UA_NodeId objectsFolder = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId ecatRoot = addFolder(1000, objectsFolder, "EtherCAT");
    UA_NodeId master0 = addFolder(2000, ecatRoot, "Master0");
    /* slavesFolderId_ = */ addFolder(3000, master0, "Slaves");
}

// ── Add slave variable nodes ─────────────────────────────────────────
void OpcUaServer::addSlaveNodes() {
    if (!server_) return;
    QMutexLocker lock(&mutex_);

    UA_NodeId slavesFolderId = UA_NODEID_NUMERIC(NS_ID, 3000);

    for (int i = 0; i < slaves_.size(); ++i) {
        const auto &sl = slaves_[i];
        UA_UInt32 nodeBase = 4000 + i * 10;

        // Slave object node
        UA_ObjectAttributes slaveAttr = UA_ObjectAttributes_default;
        QByteArray nameBytes = sl.name.toUtf8();
        slaveAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US"), nameBytes.constData());
        UA_NodeId slaveNodeId = UA_NODEID_NUMERIC(NS_ID, nodeBase);
        UA_QualifiedName qn = UA_QUALIFIEDNAME_ALLOC(NS_ID, nameBytes.constData());
        UA_Server_addObjectNode(server_, slaveNodeId, slavesFolderId,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                qn,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                slaveAttr, nullptr, &slaveNodeId);
        UA_QualifiedName_clear(&qn);

        // Helper to add a variable node under a slave
        auto addVariable = [&](UA_UInt32 subId, const char *name,
                               const QString &value) {
            UA_VariableAttributes va = UA_VariableAttributes_default;
            va.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US"), (char *)name);
            QByteArray valBytes = value.toUtf8();
            UA_String uaVal = UA_STRING_ALLOC(valBytes.constData());
            UA_Variant_setScalar(&va.value, &uaVal, &UA_TYPES[UA_TYPES_STRING]);
            UA_NodeId varNodeId = UA_NODEID_NUMERIC(NS_ID, nodeBase + subId);
            UA_QualifiedName varQn = UA_QUALIFIEDNAME_ALLOC(NS_ID, name);
            UA_Server_addVariableNode(server_, varNodeId, slaveNodeId,
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                      varQn,
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                      va, nullptr, nullptr);
            UA_String_clear(&uaVal);
            UA_QualifiedName_clear(&varQn);
        };

        addVariable(1, "Position", QByteArray::number(sl.position));
        addVariable(2, "Name", sl.name);
        addVariable(3, "State", sl.state);
        addVariable(4, "Flags", sl.flags);
    }
}

// ── Remove slave nodes ───────────────────────────────────────────────
void OpcUaServer::removeSlaveNodes() {
    if (!server_) return;
    QMutexLocker lock(&mutex_);
    // Phase 1: simply re-add; a production version would track node IDs.
}

// ── Update slaves ────────────────────────────────────────────────────
void OpcUaServer::updateSlaves(const QVector<SlaveInfo> &slaves) {
    {
        QMutexLocker lock(&mutex_);
        slaves_ = slaves;
    }
    removeSlaveNodes();
    addSlaveNodes();
}