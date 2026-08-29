// connection_recovery_test.cpp
//
// EcatClient reconnection behaviour, tested against a real local QTcpServer
// (mirrors the approach of tests/unit/infra/ecat_client_methods_test.cpp).
//
// Coverage:
//   T1  Server drops an established connection -> client goes Disconnected,
//       then auto-reconnects to the SAME host/port it was told (a non-default
//       port is used so the reconnect can only succeed by dialling it).
//   T2  Reconnect-on-initial-failure: first connect is refused, the server
//       comes up later on that port, and auto-reconnect re-establishes.
//   T3  send() while disconnected emits an error and invokes the caller
//       handler with an error instead of leaving it hanging.
//
// EcatClient::send() is private and returns void, so "returns false" is
// observed through the public surface: errorMessage("ecatd is not connected")
// and the per-method handler's error path (no sdoValue / no hang).

#include "EcatClient.h"
#include "JsonProtocol.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;

void fail(const QString &msg) {
    std::cerr << msg.toStdString() << '\n';
    ++failures;
}

// Minimal daemon stand-in: accepts connections, replies to pings (so the
// client's 5s heartbeat never flaps), and can drop all clients while keeping
// the listening socket open.
class TestServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TestServer(QObject *parent = nullptr) : QTcpServer(parent) {
        connect(this, &QTcpServer::newConnection, this, &TestServer::acceptClient);
    }

    // Close all connected client sockets but KEEP listening so a reconnect to
    // the same port succeeds.
    void dropAllClients() {
        for (auto *sock : connectedSockets_) sock->disconnectFromHost();
    }

    int connectionCount() const { return acceptCount_; }

private slots:
    void acceptClient() {
        while (auto *socket = nextPendingConnection()) {
            ++acceptCount_;
            connectedSockets_.append(socket);
            connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
                QByteArray &buf = buffers_[socket];
                buf += socket->readAll();
                int newline = -1;
                while ((newline = buf.indexOf('\n')) >= 0) {
                    const auto line = buf.left(newline);
                    buf.remove(0, newline + 1);
                    const auto doc = QJsonDocument::fromJson(line);
                    if (!doc.isObject()) continue;
                    const auto req = doc.object();
                    const QString id = req.value("id").toString();
                    const QString method = req.value("method").toString();
                    if (method == "ping") {
                        QJsonObject result;
                        result["name"] = "test-daemon";
                        result["version"] = "1.0";
                        socket->write(JsonProtocol::encode(JsonProtocol::success(id, result)));
                        socket->flush();
                    }
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
    QHash<QTcpSocket *, QByteArray> buffers_;
    QList<QTcpSocket *> connectedSockets_;
    int acceptCount_ = 0;
};

// Spin the event loop until cond() is true or the timeout elapses.
bool waitFor(std::function<bool()> cond, QCoreApplication &app, int timeoutMs = 5000) {
    const int steps = timeoutMs / 50;
    for (int i = 0; i < steps && !cond(); ++i) {
        app.processEvents();
        QThread::msleep(50);
    }
    return cond();
}

} // namespace

#include "connection_recovery_test.moc"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // ─── T1: reconnect to the SAME host/port after a forced drop ─────────
    {
        TestServer server;
        const quint16 port = 17100;  // non-default: reconnect only works if the
                                     // client remembers this exact port
        if (!server.listen(QHostAddress::LocalHost, port)) fail("T1: server listen");

        EcatClient client;
        int reconnectedCount = 0;
        bool sawConnectionLost = false;
        bool sawReconnecting = false;
        QObject::connect(&client, &EcatClient::reconnected, [&] { ++reconnectedCount; });
        QObject::connect(&client, &EcatClient::connectionLost, [&] { sawConnectionLost = true; });
        QObject::connect(&client, &EcatClient::reconnecting,
                         [&](int, int) { sawReconnecting = true; });

        client.connectToHost(QHostAddress::LocalHost, port);
        if (!waitFor([&] { return client.isConnected(); }, app, 5000))
            fail("T1: initial connect timed out");
        const int initialReconnects = reconnectedCount;
        if (initialReconnects < 1) fail("T1: initial connect did not signal");

        server.dropAllClients();
        if (!waitFor([&] { return sawConnectionLost; }, app, 5000))
            fail("T1: connectionLost not observed after server drop");

        // Auto-reconnect (enabled by default) must dial the SAME host/port:
        // the only listener in the process is on `port`, so a success proves
        // the target was remembered.
        if (!waitFor([&] { return client.isConnected(); }, app, 8000))
            fail("T1: client did not reconnect within 8s");
        if (reconnectedCount <= initialReconnects)
            fail("T1: no new reconnect after the drop");
        if (!sawReconnecting) fail("T1: Reconnecting state was not observed");
        if (server.connectionCount() < 2)
            fail("T1: server accepted only one connection (wrong host/port dialled?)");
        if (client.connectionState() != ConnectionState::Connected)
            fail("T1: final state is not Connected");
        std::cout << "T1 PASS: reconnect to remembered host/port after drop\n";
    }

    // ─── T2: reconnect-on-initial-failure ─────────────────────────────────
    {
        const quint16 port = 17101;  // nothing listening yet
        EcatClient client;
        bool sawRefused = false;
        QObject::connect(&client, &EcatClient::errorMessage, [&](const QString &m) {
            if (m.contains("refused")) sawRefused = true;
        });

        client.connectToHost(QHostAddress::LocalHost, port);
        if (!waitFor([&] { return sawRefused; }, app, 5000))
            fail("T2: initial connect was not refused");

        // The daemon comes up later on the same port.
        TestServer server;
        if (!server.listen(QHostAddress::LocalHost, port)) fail("T2: server listen");

        if (!waitFor([&] { return client.isConnected(); }, app, 10000))
            fail("T2: auto-reconnect after initial failure did not re-establish");
        if (server.connectionCount() < 1)
            fail("T2: server never accepted a reconnect");
        std::cout << "T2 PASS: reconnect-on-initial-failure\n";
    }

    // ─── T3: send while disconnected errors fast, no hang ────────────────
    {
        // Never connected.
        EcatClient client;
        QStringList errors;
        bool sdoFired = false;
        QObject::connect(&client, &EcatClient::errorMessage, [&](const QString &m) {
            errors << m;
        });
        QObject::connect(&client, &EcatClient::sdoValue,
                         [&](int, const QString &, const QString &, const QString &) {
                             sdoFired = true;
                         });

        client.upload(0, "0x1018", "0x01");  // must fail synchronously
        if (errors.isEmpty()) fail("T3: no error emitted when disconnected");
        if (!errors.last().contains("not connected"))
            fail("T3: error should mention 'not connected'");
        if (sdoFired) fail("T3: sdoValue must not fire on a failed send");

        client.ping();  // also fails fast, does not hang
        if (!errors.last().contains("not connected"))
            fail("T3: ping while disconnected should error too");

        // Connected then dropped, with auto-reconnect disabled so it stays
        // disconnected.
        TestServer server;
        const quint16 port = 17102;
        if (!server.listen(QHostAddress::LocalHost, port)) fail("T3b: server listen");

        EcatClient client2;
        client2.enableAutoReconnect(false);
        client2.connectToHost(QHostAddress::LocalHost, port);
        if (!waitFor([&] { return client2.isConnected(); }, app, 5000))
            fail("T3b: connect failed");
        server.dropAllClients();
        if (!waitFor([&] { return !client2.isConnected(); }, app, 5000))
            fail("T3b: client did not drop");

        QStringList errors2;
        QObject::connect(&client2, &EcatClient::errorMessage, [&](const QString &m) {
            errors2 << m;
        });
        client2.upload(0, "0x1018", "0x01");
        if (errors2.isEmpty() || !errors2.last().contains("not connected"))
            fail("T3b: send after drop should error with 'not connected'");
        if (client2.connectionState() != ConnectionState::Disconnected)
            fail("T3b: should stay Disconnected with auto-reconnect off");
        std::cout << "T3 PASS: send while disconnected errors without hanging\n";
    }

    if (failures > 0) {
        std::cerr << failures << " connection_recovery_test FAILURE(S)\n";
        return 1;
    }
    std::cout << "All connection_recovery_test PASSED\n";
    return 0;
}
