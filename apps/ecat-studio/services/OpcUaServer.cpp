#include "OpcUaServer.h"
#include "infra/EcatClient.h"

#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/log_stdout.h>

#include <QDebug>
#include <QJsonObject>

// ── OPC UA node IDs for our EtherCAT address space ──────────────────
// Root object ID for EtherCAT data
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

    // Disable logging
    config_->logger = UA_Log_Stdout_new(UA_LOGLEVEL_ERROR);

    // Add the EtherCAT namespace
    UA_StatusCode retval = UA_Server_addNamespace(config_, "http://nekoecat.local/opcua/");
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

    // Start the server thread (open62541 background loop)
    UA_Server_run(server_, [](UA_Server *) { return true; });

    qInfo().noquote() << QString("OPC UA server started on port %1").arg(port_);
    emit serverStarted(port_);
    return true;
}

// ── Stop ─────────────────────────────────────────────────────────────
void OpcUaServer::stop() {
    if (!running_) return;
    running_ = false;
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

    // Create folder node for "EtherCAT" under Objects folder
    UA_ObjectAttributes attr = UA_ObjectAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT((char *)"en-US", (char *)"EtherCAT");
    attr.description = UA_LOCALIZEDTEXT((char *)"en-US",
                                         (char *)"EtherCAT bus data from NekoEcat Studio");

    UA_NodeId ethercatFolderId = UA_NODEID_NUMERIC(NS_ID, 1000);
    UA_NodeId parentFolderId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentFolderNodeClass = UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE);
    UA_Server_addObjectNode(server_, ethercatFolderId, parentFolderId,
                            parentFolderNodeClass,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            attr, nullptr, &ethercatFolderId);

    // Create folder for Master0
    UA_ObjectAttributes masterAttr = UA_ObjectAttributes_default;
    masterAttr.displayName = UA_LOCALIZEDTEXT((char *)"en-US", (char *)"Master0");
    UA_NodeId masterFolderId = UA_NODEID_NUMERIC(NS_ID, 2000);
    UA_Server_addObjectNode(server_, masterFolderId, ethercatFolderId,
                            parentFolderNodeClass,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            masterAttr, nullptr, &masterFolderId);

    // Create folder for Slaves
    UA_ObjectAttributes slavesAttr = UA_ObjectAttributes_default;
    slavesAttr.displayName = UA_LOCALIZEDTEXT((char *)"en-US", (char *)"Slaves");
    UA_NodeId slavesFolderId = UA_NODEID_NUMERIC(NS_ID, 3000);
    UA_Server_addObjectNode(server_, slavesFolderId, masterFolderId,
                            parentFolderNodeClass,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            slavesAttr, nullptr, &slavesFolderId);
}

// ── Add slave nodes ──────────────────────────────────────────────────
void OpcUaServer::addSlaveNodes() {
    if (!server_) return;
    QMutexLocker lock(&mutex_);

    // Slaves folder is NS_ID:3000
    UA_NodeId slavesFolderId = UA_NODEID_NUMERIC(NS_ID, 3000);
    UA_NodeId folderTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE);

    for (int i = 0; i < slaves_.size(); ++i) {
        const auto &sl = slaves_[i];
        UA_UInt32 nodeBase = 4000 + i * 10; // Each slave gets 10 IDs

        // Slave object node
        UA_ObjectAttributes slaveAttr = UA_ObjectAttributes_default;
        QByteArray nameBytes = sl.name.toUtf8();
        slaveAttr.displayName = UA_LOCALIZEDTEXT((char *)"en-US", nameBytes.constData());
        UA_NodeId slaveNodeId = UA_NODEID_NUMERIC(NS_ID, nodeBase);
        UA_Server_addObjectNode(server_, slaveNodeId, slavesFolderId,
                                folderTypeId,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                slaveAttr, nullptr, &slaveNodeId);

        // Helper to add a variable node under a slave
        auto addVariable = [&](UA_UInt32 subId, const char *name,
                               const QString &value) {
            UA_VariableAttributes va = UA_VariableAttributes_default;
            va.displayName = UA_LOCALIZEDTEXT((char *)"en-US", (char *)name);
            QByteArray valBytes = value.toUtf8();
            UA_String uaVal = UA_STRING_ALLOC(valBytes.constData());
            UA_Variant_setScalar(&va.value, &uaVal, &UA_TYPES[UA_TYPES_STRING]);
            UA_NodeId varNodeId = UA_NODEID_NUMERIC(NS_ID, nodeBase + subId);
            UA_Server_addVariableNode(server_, varNodeId, slaveNodeId,
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                      va, nullptr, nullptr);
            UA_String_clear(&uaVal);
        };

        addVariable(1, "Position", QByteArray::number(sl.position));
        addVariable(2, "Name", sl.name);
        addVariable(3, "State", sl.state);
        addVariable(4, "VendorID", sl.vendorId);
        addVariable(5, "ProductCode", sl.productCode);
        addVariable(6, "Revision", sl.revision);
    }
}

// ── Remove slave nodes ───────────────────────────────────────────────
void OpcUaServer::removeSlaveNodes() {
    if (!server_) return;
    QMutexLocker lock(&mutex_);
    // Deleting the Slaves folder children by iterating.
    // In Phase 1 we simply re-add; a production version would track node IDs.
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