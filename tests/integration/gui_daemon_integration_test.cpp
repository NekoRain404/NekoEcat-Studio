// GuiDaemonIntegrationTest — End-to-end GUI-daemon communication test
//
// Test coverage:
//   - Full command cycle: ping, scan, upload, download, setState
//   - Signal verification for each command
//   - Reconnection after server restart
//   - Error handling for daemon-side failures
//   - Multiple interleaved requests

#include "CommandDispatcher.h"
#include "EcatClient.h"
#include "JsonProtocol.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;

void fail(const QString& msg) {
    std::cerr << msg.toStdString() << '\n';
    ++failures;
}

void expectTrue(bool cond, const QString& msg) {
    if (!cond)
        fail(msg);
}

void expectEqual(const QString& actual, const QString& expected, const QString& msg) {
    if (actual != expected)
        fail(QString("%1: expected '%2', got '%3'").arg(msg, expected, actual));
}

void expectEqual(ConnectionState actual, ConnectionState expected, const QString& msg) {
    if (actual != expected)
        fail(QString("%1: unexpected state").arg(msg));
}

bool waitFor(std::function<bool()> cond, QCoreApplication& app, int timeoutMs = 5000) {
    for (int i = 0; i < timeoutMs / 50 && !cond(); ++i) {
        app.processEvents();
        QThread::msleep(50);
    }
    return cond();
}

// Full-featured test server simulating ecatd daemon responses.
class DaemonSimulator : public QTcpServer {
    Q_OBJECT
public:
    explicit DaemonSimulator(QObject* parent = nullptr) : QTcpServer(parent) {
        connect(this, &QTcpServer::newConnection, this, &DaemonSimulator::acceptClient);
    }

    QJsonObject lastRequest;
    int requestCount = 0;

    void forceCloseAll() {
        for (auto* sock : connectedSockets_)
            sock->close();
        close();
    }

private slots:
    void acceptClient() {
        while (auto* socket = nextPendingConnection()) {
            connectedSockets_.append(socket);
            connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
                QByteArray& buf = buffers_[socket];
                buf += socket->readAll();
                int newline = -1;
                while ((newline = buf.indexOf('\n')) >= 0) {
                    const auto line = buf.left(newline);
                    buf.remove(0, newline + 1);
                    const auto doc = QJsonDocument::fromJson(line);
                    if (!doc.isObject())
                        continue;
                    const auto req = doc.object();
                    lastRequest = req;
                    requestCount++;
                    const QString id = req.value("id").toString();
                    const QString method = req.value("method").toString();
                    const auto params = req.value("params").toObject();

                    QJsonObject result;
                    if (method == "ping") {
                        result["name"] = "ecatd-sim";
                        result["version"] = "2.0.0";
                    } else if (method == "scan") {
                        QJsonArray slaves;
                        slaves.append(QJsonObject{{"position", 0}, {"state", "OP"}, {"name", "EL1008"}});
                        slaves.append(QJsonObject{{"position", 1}, {"state", "PREOP"}, {"name", "EL2008"}});
                        result["slaves"] = slaves;
                    } else if (method == "upload") {
                        result["value"] =
                            QString("0xABCD_%1_%2")
                                .arg(params.value("index").toString(), params.value("subIndex").toString());
                    } else if (method == "download") {
                        // Acknowledge write
                    } else if (method == "setState") {
                        // Acknowledge state change
                    } else if (method == "setAllStates") {
                        // Acknowledge broadcast
                    } else if (method == "rescan") {
                        // Acknowledge rescan
                    } else if (method == "slaveInfo") {
                        result["text"] = QString("Slave %1 info").arg(params.value("position").toInt());
                    } else if (method == "pdos") {
                        result["text"] = "PDO mapping data";
                    } else if (method == "sdos") {
                        result["text"] = "SDO dictionary data";
                    } else if (method == "hostDiagnostics") {
                        QJsonArray checks;
                        checks.append(QJsonObject{{"name", "kernel"}, {"status", "ok"}});
                        checks.append(QJsonObject{{"name", "igh"}, {"status", "ok"}});
                        result["checks"] = checks;
                    } else {
                        result["echo"] = params;
                    }
                    socket->write(JsonProtocol::encode(JsonProtocol::success(id, result)));
                    socket->flush();
                }
            });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket] {
                connectedSockets_.removeAll(socket);
                buffers_.remove(socket);
                socket->deleteLater();
            });
        }
    }

private:
    QHash<QTcpSocket*, QByteArray> buffers_;
    QList<QTcpSocket*> connectedSockets_;
};

// Server that always returns errors.
class ErrorDaemon : public QTcpServer {
    Q_OBJECT
public:
    explicit ErrorDaemon(QObject* parent = nullptr) : QTcpServer(parent) {
        connect(this, &QTcpServer::newConnection, this, &ErrorDaemon::acceptClient);
    }

private slots:
    void acceptClient() {
        while (auto* socket = nextPendingConnection()) {
            connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
                QByteArray& buf = buffers_[socket];
                buf += socket->readAll();
                int newline = -1;
                while ((newline = buf.indexOf('\n')) >= 0) {
                    const auto line = buf.left(newline);
                    buf.remove(0, newline + 1);
                    const auto doc = QJsonDocument::fromJson(line);
                    if (!doc.isObject())
                        continue;
                    const auto req = doc.object();
                    const QString id = req.value("id").toString();
                    socket->write(JsonProtocol::encode(JsonProtocol::failure(id, "Slave not found", -2)));
                    socket->flush();
                }
            });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket] {
                buffers_.remove(socket);
                socket->deleteLater();
            });
        }
    }

private:
    QHash<QTcpSocket*, QByteArray> buffers_;
};

} // namespace

#include "gui_daemon_integration_test.moc"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // ─── T1: Full command cycle — ping + scan + upload + download + setState
    {
        DaemonSimulator daemon;
        const quint16 port = 17001;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.setRequestTimeout(5000);
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T1: connected");

        // ping
        QString daemonInfoText;
        QObject::connect(&client, &EcatClient::daemonInfo, [&](const QString& text) { daemonInfoText = text; });
        client.ping();
        expectTrue(waitFor([&] { return !daemonInfoText.isEmpty(); }, app), "T1: ping response");
        expectTrue(daemonInfoText.contains("ecatd-sim"), "T1: daemon name");
        expectTrue(daemonInfoText.contains("2.0.0"), "T1: daemon version");

        // scan
        QVector<SlaveInfo> slaves;
        QObject::connect(&client, &EcatClient::slavesChanged, [&](const QVector<SlaveInfo>& s) { slaves = s; });
        client.scan();
        expectTrue(waitFor([&] { return slaves.size() >= 2; }, app), "T1: scan returned 2 slaves");
        expectEqual(slaves[0].name, "EL1008", "T1: slave 0 name");
        expectEqual(slaves[1].name, "EL2008", "T1: slave 1 name");

        // upload
        QString sdoVal;
        QObject::connect(&client, &EcatClient::sdoValue,
                         [&](int pos, const QString& idx, const QString& sub, const QString& val) {
                             sdoVal = val;
                             Q_UNUSED(pos);
                             Q_UNUSED(idx);
                             Q_UNUSED(sub);
                         });
        client.upload(0, "0x1018", "0x01");
        expectTrue(waitFor([&] { return !sdoVal.isEmpty(); }, app), "T1: upload returned value");
        expectTrue(sdoVal.contains("0x1018"), "T1: upload value contains index");
        expectTrue(sdoVal.contains("0x01"), "T1: upload value contains subIndex");

        // download (triggers commandSucceeded + auto-upload readback)
        QString successMsg;
        QObject::connect(&client, &EcatClient::commandSucceeded, [&](const QString& msg) { successMsg = msg; });
        sdoVal.clear();
        client.download(1, "0x6040", "0x00", "0x000F", "UINT16");
        expectTrue(waitFor([&] { return !successMsg.isEmpty(); }, app), "T1: download succeeded");
        expectTrue(successMsg.contains("SDO download"), "T1: download success message");
        expectTrue(waitFor([&] { return !sdoVal.isEmpty(); }, app), "T1: download readback");

        // setState (triggers commandSucceeded + scan)
        successMsg.clear();
        slaves.clear();
        client.setState(0, "OP");
        expectTrue(waitFor([&] { return !successMsg.isEmpty(); }, app), "T1: setState succeeded");
        expectTrue(successMsg.contains("OP"), "T1: setState mentions OP");
        expectTrue(waitFor([&] { return !slaves.isEmpty(); }, app), "T1: setState triggered scan");
    }

    // ─── T2: hostDiagnostics returns checks array ──────────────────────
    {
        DaemonSimulator daemon;
        const quint16 port = 17002;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T2: connected");

        QJsonArray checks;
        QObject::connect(&client, &EcatClient::hostDiagnosticsReady, [&](const QJsonArray& c) { checks = c; });
        client.hostDiagnostics();
        expectTrue(waitFor([&] { return !checks.isEmpty(); }, app), "T2: diagnostics received");
        expectTrue(checks.size() == 2, "T2: two checks");
        expectEqual(checks[0].toObject().value("name").toString(), "kernel", "T2: check name");
    }

    // ─── T3: Reconnection after server restart ─────────────────────────
    {
        DaemonSimulator daemon;
        const quint16 port = 17003;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T3: initial connect");

        // Verify connectivity.
        QString info;
        QObject::connect(&client, &EcatClient::daemonInfo, [&](const QString& t) { info = t; });
        client.ping();
        expectTrue(waitFor([&] { return !info.isEmpty(); }, app), "T3: initial ping ok");

        // Kill the server.
        bool disconnected = false;
        QObject::connect(&client, &EcatClient::disconnected, [&] { disconnected = true; });
        daemon.forceCloseAll();
        expectTrue(waitFor([&] { return disconnected; }, app), "T3: disconnected after kill");
        expectTrue(!client.isConnected(), "T3: client reports disconnected");

        // Restart server on same port.
        DaemonSimulator daemon2;
        daemon2.listen(QHostAddress::LocalHost, port);

        // Manually reconnect.
        info.clear();
        bool reconnected = false;
        QObject::connect(&client, &EcatClient::connected, [&] { reconnected = true; });
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return reconnected; }, app), "T3: reconnected after restart");
        expectTrue(client.isConnected(), "T3: isConnected after reconnect");

        // Verify commands work again.
        client.ping();
        expectTrue(waitFor([&] { return !info.isEmpty(); }, app), "T3: ping after reconnect");
        expectTrue(info.contains("ecatd-sim"), "T3: daemon name after reconnect");
    }

    // ─── T4: Error handling — daemon returns failure ────────────────────
    {
        ErrorDaemon daemon;
        const quint16 port = 17004;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T4: connected");

        QString errorMsg;
        QObject::connect(&client, &EcatClient::errorMessage, [&](const QString& msg) { errorMsg = msg; });

        client.upload(0, "0x1018", "0x01");
        expectTrue(waitFor([&] { return !errorMsg.isEmpty(); }, app), "T4: error emitted");
        expectTrue(errorMsg.contains("Slave not found"), "T4: error message contains 'Slave not found'");

        // Verify success handlers did NOT fire.
        bool sdoFired = false;
        QObject::connect(&client, &EcatClient::sdoValue,
                         [&](int, const QString&, const QString&, const QString&) { sdoFired = true; });
        bool cmdFired = false;
        QObject::connect(&client, &EcatClient::commandSucceeded, [&](const QString&) { cmdFired = true; });

        client.upload(1, "0x6040", "0x00");
        for (int i = 0; i < 20; ++i) {
            app.processEvents();
            QThread::msleep(50);
        }
        expectTrue(!sdoFired, "T4: sdoValue not emitted on error");
        expectTrue(!cmdFired, "T4: commandSucceeded not emitted on error");
    }

    // ─── T5: Command when not connected ────────────────────────────────
    {
        EcatClient client;
        client.enableAutoReconnect(false);

        QString errorMsg;
        QObject::connect(&client, &EcatClient::errorMessage, [&](const QString& msg) { errorMsg = msg; });

        client.ping();
        for (int i = 0; i < 10; ++i) {
            app.processEvents();
            QThread::msleep(50);
        }
        expectTrue(!errorMsg.isEmpty(), "T5: error on disconnected command");
        expectTrue(errorMsg.contains("not connected"), "T5: error mentions not connected");
    }

    // ─── T6: Multiple interleaved commands ──────────────────────────────
    {
        DaemonSimulator daemon;
        const quint16 port = 17006;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T6: connected");

        QStringList values;
        QObject::connect(&client, &EcatClient::sdoValue,
                         [&](int, const QString&, const QString&, const QString& val) { values.append(val); });

        client.upload(0, "0x1018", "0x01");
        client.upload(1, "0x6040", "0x00");
        client.upload(0, "0x6041", "0x00");

        expectTrue(waitFor([&] { return values.size() >= 3; }, app), "T6: all 3 uploads completed");
        expectTrue(values.size() == 3, "T6: exactly 3 sdoValue signals");
    }

    // ─── T7: setAllStates broadcast + scan trigger ─────────────────────
    {
        DaemonSimulator daemon;
        const quint16 port = 17007;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T7: connected");

        QString successMsg;
        QObject::connect(&client, &EcatClient::commandSucceeded, [&](const QString& msg) { successMsg = msg; });

        QVector<SlaveInfo> slaves;
        QObject::connect(&client, &EcatClient::slavesChanged, [&](const QVector<SlaveInfo>& s) { slaves = s; });

        client.setAllStates("INIT");
        expectTrue(waitFor([&] { return !successMsg.isEmpty(); }, app), "T7: setAllStates succeeded");
        expectTrue(successMsg.contains("INIT"), "T7: mentions INIT");
        expectTrue(waitFor([&] { return !slaves.isEmpty(); }, app), "T7: triggered scan");
    }

    // ─── T8: Connection state transitions ───────────────────────────────
    {
        DaemonSimulator daemon;
        const quint16 port = 17008;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        expectEqual(client.connectionState(), ConnectionState::Disconnected, "T8: initial state");

        QList<ConnectionState> states;
        QObject::connect(&client, &EcatClient::connectionStateChanged, [&](ConnectionState s) { states.append(s); });

        client.connectToHost(QHostAddress::LocalHost, port);
        expectEqual(client.connectionState(), ConnectionState::Connecting, "T8: connecting");

        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T8: connected");
        expectTrue(states.contains(ConnectionState::Connecting), "T8: Connecting emitted");
        expectTrue(states.contains(ConnectionState::Connected), "T8: Connected emitted");

        // Disconnect
        bool disconnected = false;
        QObject::connect(&client, &EcatClient::disconnected, [&] { disconnected = true; });
        daemon.forceCloseAll();
        expectTrue(waitFor([&] { return disconnected; }, app), "T8: disconnected");
        expectTrue(!client.isConnected(), "T8: final state is disconnected");
    }

    // ─── T9: Slave-specific commands (slaveInfo, pdos, sdos) ───────────
    {
        DaemonSimulator daemon;
        const quint16 port = 17009;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T9: connected");

        // slaveInfo
        QString infoTitle, infoText;
        int infoPos = -1;
        QObject::connect(&client, &EcatClient::slaveTextResult,
                         [&](const QString& title, int pos, const QString& text) {
                             infoTitle = title;
                             infoPos = pos;
                             infoText = text;
                         });
        client.slaveInfo(0);
        expectTrue(waitFor([&] { return !infoText.isEmpty(); }, app), "T9: slaveInfo response");
        expectEqual(infoTitle, "Info", "T9: slaveInfo title");
        expectTrue(infoPos == 0, "T9: slaveInfo position");
        expectTrue(infoText.contains("Slave 0"), "T9: slaveInfo text");

        // pdos
        infoTitle.clear();
        infoText.clear();
        client.pdos(1);
        expectTrue(waitFor([&] { return !infoText.isEmpty(); }, app), "T9: pdos response");
        expectEqual(infoTitle, "PDO", "T9: pdos title");

        // sdos
        infoTitle.clear();
        infoText.clear();
        client.sdos(0);
        expectTrue(waitFor([&] { return !infoText.isEmpty(); }, app), "T9: sdos response");
        expectEqual(infoTitle, "SDO", "T9: sdos title");
    }

    // ─── T10: masterTarget injected into every request ─────────────────
    {
        DaemonSimulator daemon;
        const quint16 port = 17010;
        daemon.listen(QHostAddress::LocalHost, port);

        EcatClient client;
        client.enableAutoReconnect(false);
        client.setMasterTarget("3");
        client.connectToHost(QHostAddress::LocalHost, port);
        expectTrue(waitFor([&] { return client.isConnected(); }, app), "T10: connected");

        QString info;
        QObject::connect(&client, &EcatClient::daemonInfo, [&](const QString& t) { info = t; });
        client.ping();
        expectTrue(waitFor([&] { return !info.isEmpty(); }, app), "T10: ping ok");

        const auto params = daemon.lastRequest.value("params").toObject();
        expectEqual(params.value("master").toString(), "3", "T10: masterTarget=3");
    }

    // ─── Summary ────────────────────────────────────────────────────────
    if (failures > 0) {
        std::cerr << failures << " integration test(s) FAILED\n";
        return 1;
    }
    std::cout << "All gui_daemon_integration_test PASSED\n";
    return 0;
}
