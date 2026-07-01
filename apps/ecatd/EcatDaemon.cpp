// ecatd runtime daemon: TCP server, command dispatch, and master lifecycle.
#include "EcatDaemon.h"

#include "EthercatNativeBackend.h"
#include "JsonProtocol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include <QTimer>
namespace {

// Extract the master identifier from request params; defaults to "0" for single-master setups.
QString requestedMaster(const QJsonObject &params)
{
    return params.value("master").toString("0").trimmed();
}

// Validate that the master param is a numeric IgH index, required by the ecrt API.
bool requestedMasterIndex(const QJsonObject &params, uint32_t *index, QString *error)
{
    bool ok = false;
    const uint value = requestedMaster(params).toUInt(&ok);
    if (!ok) {
        if (error) {
            *error = "Free Run requires a numeric IgH master index.";
        }
        return false;
    }
    *index = value;
    return true;
}

}

// Wire incoming TCP connections to the per-client read handler.
EcatDaemon::EcatDaemon(QObject *parent)
    : QObject(parent), eoeHandler_(nullptr), redundancyHandler_(nullptr),
      onlineChangeHandler_(nullptr)
{
    uptimeTimer_.start();
    backend_ = new EthercatCliBackend(this);
    dcSyncHandler_.setBackend(backend_);
    eoeHandler_ = EoEHandler(backend_);
    redundancyHandler_ = RedundancyHandler(backend_);
    onlineChangeHandler_ = OnlineChangeHandler(backend_);
    connect(&server_, &QTcpServer::newConnection, this, &EcatDaemon::acceptClient);
    setupHandlers();

    // Poll AL status every second for event tracking.
    alPollTimer_ = new QTimer(this);
    connect(alPollTimer_, &QTimer::timeout, this, [this]() { alEventHandler_.poll(); });
    alPollTimer_->start(1000);

    // Enrich DC sync data from ecrt every 500ms when Free Run is active.
    dcPollTimer_ = new QTimer(this);
    connect(dcPollTimer_, &QTimer::timeout, this, [this]() {
        if (freeRun_.running()) {
            dcSyncHandler_.update(freeRun_.masterHandle(), 0);
        }
    });
    dcPollTimer_->start(500);
}


// Bind to localhost only — the daemon is a local IPC service, not network-exposed.
bool EcatDaemon::listen(quint16 port)
{
    return server_.listen(QHostAddress::LocalHost, port);
}

void EcatDaemon::acceptClient()
{
    // Drain all pending connections (edge-triggered) and allocate a per-socket
    // line buffer for reassembly across TCP fragmentation boundaries.
    while (auto *socket = server_.nextPendingConnection()) {
        buffers_.insert(socket, {});
        ++activeConnections_;
        connect(socket, &QTcpSocket::readyRead, this, &EcatDaemon::readClient);
        connect(socket, &QTcpSocket::disconnected, socket, [this, socket] {
            buffers_.remove(socket);
            --activeConnections_;
            socket->deleteLater();
        });
    }
}

void EcatDaemon::readClient()
{
    // Accumulate bytes and extract complete newline-delimited JSON frames;
    // partial frames remain buffered until the next readyRead signal.
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    auto &buffer = buffers_[socket];
    buffer += socket->readAll();

    int newline = -1;
    while ((newline = buffer.indexOf('\n')) >= 0) {
        const auto line = buffer.left(newline);
        buffer.remove(0, newline + 1);
        const auto document = QJsonDocument::fromJson(line);
        if (!document.isObject()) {
            send(socket, JsonProtocol::failure({}, "Invalid JSON request"));
            continue;
        }
        handle(socket, document.object());
    }
}

// Dispatch a parsed JSON request through the CommandDispatcher and send the response.
void EcatDaemon::handle(QTcpSocket *socket, const QJsonObject &request) {
    ++requestCount_;
    const QJsonObject response = dispatcher_.dispatch(request);
    if (!response.value("ok").toBool()) {
        ++errorCount_;
    }
    send(socket, response);
}

// Register all 20+ command handlers as lambdas.  Each handler receives (id, params) and returns a JSON response.
void EcatDaemon::setupHandlers() {
    dispatcher_.registerHandler("ping", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, {
            {"name", "ecatd"},
            {"version", "0.1.0"},
            {"multiMaster", true},
            {"uptimeMs", uptimeTimer_.elapsed()},
            {"requestCount", static_cast<qint64>(requestCount_)},
            {"errorCount", static_cast<qint64>(errorCount_)},
            {"activeConnections", activeConnections_}
        });
    });

    dispatcher_.registerHandler("hostDiagnostics", [this](const QString &id, const QJsonObject &) {
        QString error;
        const auto checks = backend_->hostDiagnostics(&error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"checks", checks}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("master", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->masterText(requestedMaster(params), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("scan", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const auto slaves = backend_->scanSlaves(requestedMaster(params), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"slaves", toJson(slaves)}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("rescan", [this](const QString &id, const QJsonObject &params) {
        QString error;
        if (backend_->rescan(requestedMaster(params), &error)) {
            QJsonObject resp = CommandDispatcher::success(id);
            if (backend_->isNative() && backend_->lastOperationWasFallback()) {
                resp["backend"] = QStringLiteral("cli_fallback");
                resp["reason"] = backend_->lastFallbackReason();
            }
            return resp;
        }
        return CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("slaveInfo", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->slaveInfo(requestedMaster(params), params.value("position").toInt(), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("pdos", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->pdos(requestedMaster(params), params.value("position").toInt(), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("sdos", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->sdos(requestedMaster(params), params.value("position").toInt(), &error);
        if (error.isEmpty()) {
            QJsonObject resp = CommandDispatcher::success(id, {{"text", text}});
            if (backend_->isNative() && backend_->lastOperationWasFallback()) {
                resp["backend"] = QStringLiteral("cli_fallback");
                resp["reason"] = backend_->lastFallbackReason();
            }
            return resp;
        }
        return CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("xml", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->slaveXml(requestedMaster(params), params.value("position").toInt(), &error);
        if (error.isEmpty()) {
            QJsonObject resp = CommandDispatcher::success(id, {{"text", text}});
            if (backend_->isNative() && backend_->lastOperationWasFallback()) {
                resp["backend"] = QStringLiteral("cli_fallback");
                resp["reason"] = backend_->lastFallbackReason();
            }
            return resp;
        }
        return CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("upload", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->upload(requestedMaster(params),
                                             params.value("position").toInt(),
                                             params.value("index").toString(),
                                             params.value("subIndex").toString(),
                                             params.value("type").toString(),
                                             &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"value", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("download", [this](const QString &id, const QJsonObject &params) {
        QString error;
        return backend_->download(requestedMaster(params),
                                 params.value("position").toInt(),
                                 params.value("index").toString(),
                                 params.value("subIndex").toString(),
                                 params.value("value").toString(),
                                 params.value("type").toString(),
                                 &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("applyStartupSdos", [this](const QString &id, const QJsonObject &params) {
        const QString master = requestedMaster(params);
        int applied = 0, failed = 0;
        QJsonArray failures, results;
        int row = 0;
        for (const auto &value : params.value("items").toArray()) {
            const auto item = value.toObject();
            QString itemError;
            if (backend_->download(master,
                                  item.value("position").toInt(),
                                  item.value("index").toString(),
                                  item.value("subIndex").toString(),
                                  item.value("value").toString(),
                                  item.value("type").toString(),
                                  &itemError)) {
                ++applied;
                results.append(QJsonObject{{"row", row}, {"ok", true},
                    {"position", item.value("position").toInt()},
                    {"index", item.value("index").toString()},
                    {"subIndex", item.value("subIndex").toString()}});
            } else {
                ++failed;
                QJsonObject fail{{"row", row},
                    {"position", item.value("position").toInt()},
                    {"index", item.value("index").toString()},
                    {"subIndex", item.value("subIndex").toString()},
                    {"error", itemError}};
                failures.append(fail);
                QJsonObject result = fail;
                result.insert("ok", false);
                results.append(result);
            }
            ++row;
        }
        return CommandDispatcher::success(id, {{"applied", applied}, {"failed", failed},
                                               {"failures", failures}, {"results", results}});
    });

    dispatcher_.registerHandler("setState", [this](const QString &id, const QJsonObject &params) {
        QString error;
        if (backend_->setState(requestedMaster(params), params.value("position").toInt(),
                               params.value("state").toString(), &error)) {
            QJsonObject resp = CommandDispatcher::success(id);
            if (backend_->isNative() && backend_->lastOperationWasFallback()) {
                resp["backend"] = QStringLiteral("cli_fallback");
                resp["reason"] = backend_->lastFallbackReason();
            }
            return resp;
        }
        return CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("setAllStates", [this](const QString &id, const QJsonObject &params) {
        QString error;
        if (backend_->setAllStates(requestedMaster(params), params.value("state").toString(), &error)) {
            QJsonObject resp = CommandDispatcher::success(id);
            if (backend_->isNative() && backend_->lastOperationWasFallback()) {
                resp["backend"] = QStringLiteral("cli_fallback");
                resp["reason"] = backend_->lastFallbackReason();
            }
            return resp;
        }
        return CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("freeRunStart", [this](const QString &id, const QJsonObject &params) {
        uint32_t masterIndex = 0;
        QString error;
        if (!requestedMasterIndex(params, &masterIndex, &error))
            return CommandDispatcher::failure(id, error);
        return freeRun_.start(masterIndex, &error)
            ? CommandDispatcher::success(id, freeRun_.telemetry())
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("freeRunStop", [this](const QString &id, const QJsonObject &) {
        freeRun_.stop();
        return CommandDispatcher::success(id, {{"status", freeRun_.status()}});
    });

    dispatcher_.registerHandler("freeRunStatus", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, freeRun_.telemetry());
    });

    dispatcher_.registerHandler("rtTestStart", [this](const QString &id, const QJsonObject &params) {
        uint32_t masterIndex = 0;
        QString error;
        if (!requestedMasterIndex(params, &masterIndex, &error))
            return CommandDispatcher::failure(id, error);
        const int cycleUsec = params.value("cycleUsec").toInt(1000);
        return rtTest_.start(masterIndex, cycleUsec, &error)
            ? CommandDispatcher::success(id, rtTest_.telemetry())
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("rtTestStop", [this](const QString &id, const QJsonObject &) {
        rtTest_.stop();
        return CommandDispatcher::success(id, rtTest_.telemetry());
    });

    dispatcher_.registerHandler("rtTestStatus", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, rtTest_.telemetry());
    });

    dispatcher_.registerHandler("dcSyncStatus", [this](const QString &id, const QJsonObject &params) {
        return dcSyncHandler_.handle(id, params);
    });

    dispatcher_.registerHandler("dcConfigure", [this](const QString &id, const QJsonObject &params) {
        return dcSyncHandler_.handleDcConfigure(id, params);
    });
    dispatcher_.registerHandler("dcActivate", [this](const QString &id, const QJsonObject &params) {
        return dcSyncHandler_.handleDcActivate(id, params);
    });
    dispatcher_.registerHandler("dcDeactivate", [this](const QString &id, const QJsonObject &params) {
        return dcSyncHandler_.handleDcDeactivate(id, params);
    });

    dispatcher_.registerHandler("alEventLog", [this](const QString &id, const QJsonObject &p) {
        return alEventHandler_.handle(id, p);
    });
    dispatcher_.registerHandler("alEventClear", [this](const QString &id, const QJsonObject &) {
        alEventHandler_.clear();
        return CommandDispatcher::success(id);
    });

    // Adapter discovery and configuration.
    dispatcher_.registerHandler("listAdapters", [this](const QString &id, const QJsonObject &params) {
        return adapterHandler_.handleList(id, params);
    });
    dispatcher_.registerHandler("setAdapter", [this](const QString &id, const QJsonObject &params) {
        return adapterHandler_.handleSet(id, params);
    });

    // File over EtherCAT (FoE) firmware operations.
    dispatcher_.registerHandler("foeRead", [this](const QString &id, const QJsonObject &params) {
        return foeHandler_.handleFoeRead(id, params);
    });
    dispatcher_.registerHandler("foeWrite", [this](const QString &id, const QJsonObject &params) {
        return foeHandler_.handleFoeWrite(id, params);
    });

    // Servo over EtherCAT (SoE) IDN operations.
    dispatcher_.registerHandler("soeRead", [this](const QString &id, const QJsonObject &params) {
        return soeHandler_.handleSoeRead(id, params);
    });
    dispatcher_.registerHandler("soeWrite", [this](const QString &id, const QJsonObject &params) {
        return soeHandler_.handleSoeWrite(id, params);
    });

    // Ethernet over EtherCAT (EoE) protocol operations.
    dispatcher_.registerHandler("eoeStatus", [this](const QString &id, const QJsonObject &params) {
        return eoeHandler_.handleEoeStatus(id, params);
    });
    dispatcher_.registerHandler("eoeConfigureIp", [this](const QString &id, const QJsonObject &params) {
        return eoeHandler_.handleEoeConfigureIp(id, params);
    });
    dispatcher_.registerHandler("eoeGetIp", [this](const QString &id, const QJsonObject &params) {
        return eoeHandler_.handleEoeGetIp(id, params);
    });
    dispatcher_.registerHandler("eoeStats", [this](const QString &id, const QJsonObject &params) {
        return eoeHandler_.handleEoeStats(id, params);
    });

    // Cable redundancy operations.
    dispatcher_.registerHandler("redundancyStatus", [this](const QString &id, const QJsonObject &params) {
        return redundancyHandler_.handleStatus(id, params);
    });
    dispatcher_.registerHandler("redundancyEnable", [this](const QString &id, const QJsonObject &params) {
        return redundancyHandler_.handleEnable(id, params);
    });
    dispatcher_.registerHandler("redundancyDisable", [this](const QString &id, const QJsonObject &params) {
        return redundancyHandler_.handleDisable(id, params);
    });
    dispatcher_.registerHandler("redundancyFailover", [this](const QString &id, const QJsonObject &params) {
        return redundancyHandler_.handleFailover(id, params);
    });
    dispatcher_.registerHandler("redundancyFailback", [this](const QString &id, const QJsonObject &params) {
        return redundancyHandler_.handleFailback(id, params);
    });
    dispatcher_.registerHandler("redundancyHistory", [this](const QString &id, const QJsonObject &params) {
        return redundancyHandler_.handleHistory(id, params);
    });

    // Online change (runtime reconfiguration) operations.
    dispatcher_.registerHandler("onlineChangePreview", [this](const QString &id, const QJsonObject &params) {
        return onlineChangeHandler_.handlePreview(id, params);
    });
    dispatcher_.registerHandler("onlineChangeApply", [this](const QString &id, const QJsonObject &params) {
        return onlineChangeHandler_.handleApply(id, params);
    });
    dispatcher_.registerHandler("onlineChangeStatus", [this](const QString &id, const QJsonObject &params) {
        return onlineChangeHandler_.handleStatus(id, params);
    });

    dispatcher_.registerHandler("setBackend", [this](const QString &id, const QJsonObject &params) {
        QString mode = params.value("mode").toString("auto");
        setBackendMode(mode);
        return CommandDispatcher::success(id, {{"backend", backend_->isNative() ? "native" : "cli"},
                                                {"mode", backendMode_}});
    });

    dispatcher_.registerHandler("getBackend", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, {{"backend", backend_->isNative() ? "native" : "cli"},
                                                {"mode", backendMode_}});
    });

    dispatcher_.registerHandler("signalPoll", [this](const QString &id, const QJsonObject &p) {
        return signalHandler_.handlePoll(id, p);
    });
    dispatcher_.registerHandler("signalSubscribe", [this](const QString &id, const QJsonObject &p) {
        int ch = signalHandler_.subscribe(p.value("name").toString(),
                                           p.value("slave").toInt(),
                                           p.value("index").toString(),
                                           p.value("subIndex").toString());
        return CommandDispatcher::success(id, {{"channelId", ch}});
    });
    dispatcher_.registerHandler("signalUnsubscribe", [this](const QString &id, const QJsonObject &p) {
        signalHandler_.unsubscribe(p.value("channelId").toInt());
        return CommandDispatcher::success(id);
    });
}

// Serialize a JSON response object and write it as a newline-delimited frame to the client socket.
void EcatDaemon::send(QTcpSocket *socket, const QJsonObject &response)
{
    // Encode as newline-delimited JSON and flush immediately for low-latency reply.
    socket->write(JsonProtocol::encode(response));
    socket->flush();
}

void EcatDaemon::setBackendMode(const QString &mode) {
    backendMode_ = mode;

    if (mode == "cli") {
        if (backend_->isNative()) {
            delete backend_;
            backend_ = new EthercatCliBackend(this);
            dcSyncHandler_.setBackend(backend_);
            eoeHandler_ = EoEHandler(backend_);
            redundancyHandler_ = RedundancyHandler(backend_);
            onlineChangeHandler_ = OnlineChangeHandler(backend_);
            qDebug() << "Switched to CLI backend";
        }
    } else if (mode == "native") {
        if (!backend_->isNative()) {
#ifdef HAVE_IGH
            delete backend_;
            backend_ = new EthercatNativeBackend(this);
            dcSyncHandler_.setBackend(backend_);
            eoeHandler_ = EoEHandler(backend_);
            redundancyHandler_ = RedundancyHandler(backend_);
            onlineChangeHandler_ = OnlineChangeHandler(backend_);
            qDebug() << "Switched to native backend";
#else
            qWarning() << "Native backend not available, keeping CLI";
#endif
        }
    } else {
        // Auto mode: try native, fallback to CLI
#ifdef HAVE_IGH
        auto *native = new EthercatNativeBackend(this);
        QString error;
        native->hostDiagnostics(&error);
        if (error.isEmpty()) {
            delete backend_;
            backend_ = native;
            dcSyncHandler_.setBackend(backend_);
            eoeHandler_ = EoEHandler(backend_);
            redundancyHandler_ = RedundancyHandler(backend_);
            onlineChangeHandler_ = OnlineChangeHandler(backend_);
            qDebug() << "Auto-selected native backend";
        } else {
            delete native;
            qDebug() << "Auto-selected CLI backend (native unavailable)";
        }
#else
        qDebug() << "Auto-selected CLI backend (native not compiled)";
#endif
    }
}
