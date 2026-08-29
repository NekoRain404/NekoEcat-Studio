// ProtocolIntegrationTest — Tests for EcatClient protocol integration
//
// Test coverage:
//   - JSON protocol framing and dispatch
//   - Client connection and ping
//   - Daemon info response format

// Integration test: EcatClient ↔ test server over localhost TCP.
// Tests JSON protocol framing, dispatch, and error handling end-to-end.
#include "EcatClient.h"
#include "CommandDispatcher.h"
#include "JsonProtocol.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;

void fail(const QString &msg) {
    std::cerr << msg.toStdString() << '\n';
    ++failures;
}

void expectTrue(bool cond, const QString &msg) {
    if (!cond) fail(msg);
}

void expectEqual(const QString &actual, const QString &expected, const QString &msg) {
    if (actual != expected)
        fail(QString("%1: expected '%2', got '%3'").arg(msg, expected, actual));
}

// Minimal test server: listens on a port, dispatches JSON requests via CommandDispatcher.
class TestServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TestServer(QObject *parent = nullptr) : QTcpServer(parent) {
        dispatcher_.registerHandler("ping", [](const QString &id, const QJsonObject &) {
            return CommandDispatcher::success(id, {{"name", "ecatd-test"}, {"version", "0.0.1"}});
        });

        dispatcher_.registerHandler("echo", [](const QString &id, const QJsonObject &params) {
            return CommandDispatcher::success(id, {{"echo", params}});
        });

        connect(this, &QTcpServer::newConnection, this, &TestServer::acceptClient);
    }

private slots:
    // Accept client connection and dispatch JSON requests
    void acceptClient() {
        while (auto *socket = nextPendingConnection()) {
            connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
                QByteArray &buf = buffers_[socket];
                buf += socket->readAll();
                int newline = -1;
                while ((newline = buf.indexOf('\n')) >= 0) {
                    const auto line = buf.left(newline);
                    buf.remove(0, newline + 1);
                    const auto doc = QJsonDocument::fromJson(line);
                    if (doc.isObject()) {
                        socket->write(JsonProtocol::encode(dispatcher_.dispatch(doc.object())));
                        socket->flush();
                    }
                }
            });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket] {
                buffers_.remove(socket);
                socket->deleteLater();
            });
        }
    }

private:
    CommandDispatcher dispatcher_;
    QHash<QTcpSocket *, QByteArray> buffers_;
};

} // namespace

#include "protocol_integration_test.moc"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Start test server on a high port.
    TestServer server;
    const quint16 port = 15877;
    if (!server.listen(QHostAddress::LocalHost, port)) {
        fail(QString("Test server failed to listen on port %1").arg(port));
        return 1;
    }

    // Connect client.
    EcatClient client;
    client.setRequestTimeout(3000);

    bool connectedOk = false;
    QObject::connect(&client, &EcatClient::connected, [&] { connectedOk = true; });
    client.connectToHost(QHostAddress::LocalHost, port);

    // Wait for connection.
    for (int i = 0; i < 50 && !connectedOk; ++i) {
        app.processEvents();
        QThread::msleep(100);
    }
    expectTrue(connectedOk, "T1: client connected to test server");
    expectTrue(client.isConnected(), "T2: isConnected() returns true");

    // Test ping.
    QString daemonInfoText;
    QObject::connect(&client, &EcatClient::daemonInfo, [&](const QString &text) {
        daemonInfoText = text;
    });
    client.ping();

    for (int i = 0; i < 30 && daemonInfoText.isEmpty(); ++i) {
        app.processEvents();
        QThread::msleep(100);
    }
    expectTrue(daemonInfoText.contains("ecatd-test"), "T3: ping returned daemon info");
    expectEqual(daemonInfoText, QString("ecatd-test 0.0.1"), "T4: daemon info format correct");

    if (failures > 0) {
        std::cerr << failures << " integration test(s) FAILED\n";
        return 1;
    }
    std::cout << "All protocol_integration_test PASSED\n";
    return 0;
}
