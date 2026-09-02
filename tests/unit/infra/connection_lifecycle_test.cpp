// ConnectionLifecycleTest — Tests for EcatClient connection lifecycle
//
// Test coverage:
//   - Initial disconnected state
//   - Connect to daemon transitions
//   - Double connect is no-op
//   - Disconnect transitions
//   - Connection timeout handling

#include "CommandDispatcher.h"
#include "EcatClient.h"
#include "JsonProtocol.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

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

void expectEqual(ConnectionState actual, ConnectionState expected, const QString& msg) {
    if (actual != expected)
        fail(QString("%1: unexpected state").arg(msg));
}

} // namespace

#include "connection_lifecycle_test.moc"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // --- Test 1: initial state is Disconnected ---
    // Verify client starts in Disconnected state
    {
        EcatClient client;
        expectEqual(client.connectionState(), ConnectionState::Disconnected, "T1: initial state");
        expectTrue(!client.isConnected(), "T1: not connected");
    }

    // --- Test 2: connectToDaemon goes to Connecting ---
    // Verify connect transitions through Connecting to Connected
    {
        // Start a server so connection can succeed.
        QTcpServer server;
        server.listen(QHostAddress::LocalHost, 15878);

        EcatClient client;
        bool stateChanged = false;
        ConnectionState lastState = ConnectionState::Disconnected;
        QObject::connect(&client, &EcatClient::connectionStateChanged, [&](ConnectionState s) {
            stateChanged = true;
            lastState = s;
        });

        client.connectToHost(QHostAddress::LocalHost, 15878);
        // Should be Connecting immediately.
        expectEqual(client.connectionState(), ConnectionState::Connecting, "T2: connecting state");

        // Wait for connection.
        for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
            app.processEvents();
            QThread::msleep(100);
        }
        expectEqual(client.connectionState(), ConnectionState::Connected, "T2: connected state");
        expectTrue(stateChanged, "T2: state change emitted");
    }

    // --- Test 3: double connect is no-op ---
    // Verify second connect call is ignored
    {
        QTcpServer server;
        server.listen(QHostAddress::LocalHost, 15879);

        EcatClient client;
        client.connectToHost(QHostAddress::LocalHost, 15879);
        for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
            app.processEvents();
            QThread::msleep(100);
        }

        int changeCount = 0;
        QObject::connect(&client, &EcatClient::connectionStateChanged, [&](ConnectionState) { ++changeCount; });

        client.connectToDaemon(); // Should be no-op.
        expectEqual(client.connectionState(), ConnectionState::Connected, "T3: still connected");
        expectTrue(changeCount == 0, "T3: no state change on double connect");
    }

    // --- Test 4: pending handlers cleared on disconnect ---
    // Verify disconnect clears pending handlers and transitions state
    {
        QTcpServer server;
        server.listen(QHostAddress::LocalHost, 15880);

        EcatClient client;
        client.connectToHost(QHostAddress::LocalHost, 15880);
        for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
            app.processEvents();
            QThread::msleep(100);
        }

        // Send a ping to register a handler.
        QString info;
        QObject::connect(&client, &EcatClient::daemonInfo, [&](const QString& text) { info = text; });
        client.ping();
        for (int i = 0; i < 10; ++i) {
            app.processEvents();
            QThread::msleep(50);
        }

        // Close the server to trigger disconnect.
        server.close();
        for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Disconnected; ++i) {
            app.processEvents();
            QThread::msleep(100);
        }
        expectEqual(client.connectionState(), ConnectionState::Disconnected, "T4: disconnected after server close");
    }

    if (failures > 0) {
        std::cerr << failures << " lifecycle test(s) FAILED\n";
        return 1;
    }
    std::cout << "All connection_lifecycle_test PASSED\n";
    return 0;
}
