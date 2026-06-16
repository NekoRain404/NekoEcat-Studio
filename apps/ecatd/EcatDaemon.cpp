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
    connect(&server_, &QTcpServer::newConnection, this, &EcatDaemon::acceptClient);
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

void EcatDaemon::handle(QTcpSocket *socket, const QJsonObject &request)
{
    // Dispatch a JSON-RPC-style request to the matching backend operation.
    const QString id = request.value("id").toString();
    const QString method = request.value("method").toString();
    const QJsonObject params = request.value("params").toObject();
    const QString master = requestedMaster(params);
    QString error;

    if (method == "ping") {
        send(socket, JsonProtocol::success(id, {{"name", "ecatd"}, {"version", "0.1.0"}, {"multiMaster", true}}));
    } else if (method == "hostDiagnostics") {
        const auto checks = backend_.hostDiagnostics(&error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"checks", checks}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "master") {
        const QString text = backend_.masterText(master, &error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"text", text}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "scan") {
        const auto slaves = backend_.scanSlaves(master, &error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"slaves", toJson(slaves)}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "rescan") {
        backend_.rescan(master, &error) ? send(socket, JsonProtocol::success(id))
                                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "pdos") {
        const QString text = backend_.pdos(master, params.value("position").toInt(), &error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"text", text}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "slaveInfo") {
        const QString text = backend_.slaveInfo(master, params.value("position").toInt(), &error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"text", text}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "sdos") {
        const QString text = backend_.sdos(master, params.value("position").toInt(), &error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"text", text}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "xml") {
        const QString text = backend_.slaveXml(master, params.value("position").toInt(), &error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"text", text}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "upload") {
        const QString text = backend_.upload(master,
                                             params.value("position").toInt(),
                                             params.value("index").toString(),
                                             params.value("subIndex").toString(),
                                             &error);
        error.isEmpty() ? send(socket, JsonProtocol::success(id, {{"value", text}}))
                        : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "download") {
        backend_.download(master,
                          params.value("position").toInt(),
                          params.value("index").toString(),
                          params.value("subIndex").toString(),
                          params.value("value").toString(),
                          params.value("type").toString(),
                          &error)
            ? send(socket, JsonProtocol::success(id))
            : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "applyStartupSdos") {
        int applied = 0;
        int failed = 0;
        QJsonArray failures;
        QJsonArray results;
        int row = 0;
        for (const auto &value : params.value("items").toArray()) {
            const auto item = value.toObject();
            QString itemError;
            if (backend_.download(master,
                                  item.value("position").toInt(),
                                  item.value("index").toString(),
                                  item.value("subIndex").toString(),
                                  item.value("value").toString(),
                                  item.value("type").toString(),
                                  &itemError)) {
                ++applied;
                results.append(QJsonObject{
                    {"row", row},
                    {"ok", true},
                    {"position", item.value("position").toInt()},
                    {"index", item.value("index").toString()},
                    {"subIndex", item.value("subIndex").toString()},
                });
            } else {
                ++failed;
                const QJsonObject failure{
                    {"row", row},
                    {"position", item.value("position").toInt()},
                    {"index", item.value("index").toString()},
                    {"subIndex", item.value("subIndex").toString()},
                    {"error", itemError},
                };
                failures.append(failure);
                QJsonObject result = failure;
                result.insert("ok", false);
                results.append(result);
            }
            ++row;
        }
        send(socket, JsonProtocol::success(id, {{"applied", applied}, {"failed", failed}, {"failures", failures}, {"results", results}}));
    } else if (method == "setState") {
        backend_.setState(master, params.value("position").toInt(), params.value("state").toString(), &error)
            ? send(socket, JsonProtocol::success(id))
            : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "setAllStates") {
        backend_.setAllStates(master, params.value("state").toString(), &error)
            ? send(socket, JsonProtocol::success(id))
            : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "freeRunStart") {
        uint32_t masterIndex = 0;
        if (!requestedMasterIndex(params, &masterIndex, &error)) {
            send(socket, JsonProtocol::failure(id, error));
            return;
        }
        freeRun_.start(masterIndex, &error) ? send(socket, JsonProtocol::success(id, freeRun_.telemetry()))
                                           : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "freeRunStop") {
        freeRun_.stop();
        send(socket, JsonProtocol::success(id, {{"status", freeRun_.status()}}));
    } else if (method == "freeRunStatus") {
        send(socket, JsonProtocol::success(id, freeRun_.telemetry()));
    } else if (method == "rtTestStart") {
        uint32_t masterIndex = 0;
        if (!requestedMasterIndex(params, &masterIndex, &error)) {
            send(socket, JsonProtocol::failure(id, error));
            return;
        }
        const int cycleUsec = params.value("cycleUsec").toInt(1000);
        rtTest_.start(masterIndex, cycleUsec, &error)
            ? send(socket, JsonProtocol::success(id, rtTest_.telemetry()))
            : send(socket, JsonProtocol::failure(id, error));
    } else if (method == "rtTestStop") {
        rtTest_.stop();
        send(socket, JsonProtocol::success(id, rtTest_.telemetry()));
    } else if (method == "rtTestStatus") {
        send(socket, JsonProtocol::success(id, rtTest_.telemetry()));
    } else {
        send(socket, JsonProtocol::failure(id, QString("Unknown method: %1").arg(method)));
    }
}

void EcatDaemon::send(QTcpSocket *socket, const QJsonObject &response)
{
    // Encode as newline-delimited JSON and flush immediately for low-latency reply.
    socket->write(JsonProtocol::encode(response));
    socket->flush();
}
