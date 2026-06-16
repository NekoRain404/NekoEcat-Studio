// ecatd runtime daemon: TCP server, command dispatch, and master lifecycle.
#include "EcatDaemon.h"

#include "JsonProtocol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

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
    : QObject(parent)
{
    backend_ = new EthercatCliBackend(this);
    connect(&server_, &QTcpServer::newConnection, this, &EcatDaemon::acceptClient);
    setupHandlers();
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
        connect(socket, &QTcpSocket::readyRead, this, &EcatDaemon::readClient);
        connect(socket, &QTcpSocket::disconnected, socket, [this, socket] {
            buffers_.remove(socket);
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

    auto buffer = buffers_.value(socket);
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

    buffers_[socket] = buffer;
}

void EcatDaemon::handle(QTcpSocket *socket, const QJsonObject &request) {
    send(socket, dispatcher_.dispatch(request));
}

void EcatDaemon::setupHandlers() {
    dispatcher_.registerHandler("ping", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, {{"name", "ecatd"}, {"version", "0.1.0"}, {"multiMaster", true}});
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
        return backend_->rescan(requestedMaster(params), &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
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
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("xml", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->slaveXml(requestedMaster(params), params.value("position").toInt(), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("upload", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_->upload(requestedMaster(params),
                                             params.value("position").toInt(),
                                             params.value("index").toString(),
                                             params.value("subIndex").toString(),
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
        return backend_->setState(requestedMaster(params), params.value("position").toInt(),
                                 params.value("state").toString(), &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("setAllStates", [this](const QString &id, const QJsonObject &params) {
        QString error;
        return backend_->setAllStates(requestedMaster(params), params.value("state").toString(), &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
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
}

void EcatDaemon::send(QTcpSocket *socket, const QJsonObject &response)
{
    // Encode as newline-delimited JSON and flush immediately for low-latency reply.
    socket->write(JsonProtocol::encode(response));
    socket->flush();
}
