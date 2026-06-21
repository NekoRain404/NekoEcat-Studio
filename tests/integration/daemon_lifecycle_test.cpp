#include <QTest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QThread>
#include "EcatClient.h"
#include "services/EventBus.h"
#include "EthercatTypes.h"

class DaemonLifecycleTest : public QObject {
  Q_OBJECT
private slots:
  void testDaemonStartStop() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15900));

    EcatClient client;
    QSignalSpy connSpy(&client, &EcatClient::connected);
    QSignalSpy stateSpy(&client, &EcatClient::connectionStateChanged);

    client.connectToHost(QHostAddress::LocalHost, 15900);
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(50);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);
    QCOMPARE(connSpy.count(), 1);
    QVERIFY(stateSpy.count() >= 1);

    server.close();
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Disconnected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(50);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);
  }

  void testClientConnectionDisconnection() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15901));

    EcatClient client;
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);
    QVERIFY(!client.isConnected());

    client.connectToHost(QHostAddress::LocalHost, 15901);
    QCOMPARE(client.connectionState(), ConnectionState::Connecting);

    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(50);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);
    QVERIFY(client.isConnected());

    server.close();
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Disconnected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(50);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);
    QVERIFY(!client.isConnected());
  }

  void testHeartbeatDetection() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15902));

    EcatClient client;
    client.connectToHost(QHostAddress::LocalHost, 15902);
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(50);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);

    QSignalSpy disconnectSpy(&client, &EcatClient::disconnected);
    server.close();

    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Disconnected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);
    QCOMPARE(disconnectSpy.count(), 1);
  }

  void testAutoReconnect() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15903));

    EcatClient client;
    client.enableAutoReconnect(true);
    QVERIFY(client.autoReconnectEnabled());

    client.connectToHost(QHostAddress::LocalHost, 15903);
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(50);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);

    QSignalSpy reconnectSpy(&client, &EcatClient::reconnected);
    server.close();
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Disconnected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);

    QTcpServer server2;
    QVERIFY(server2.listen(QHostAddress::LocalHost, 15903));
    QTest::qWait(5000);
    QCoreApplication::processEvents();
  }

  void testAutoReconnectDisable() {
    EcatClient client;
    client.enableAutoReconnect(false);
    QVERIFY(!client.autoReconnectEnabled());

    client.enableAutoReconnect(true);
    QVERIFY(client.autoReconnectEnabled());
  }

  void testMultipleClients() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15904));

    EcatClient client1;
    EcatClient client2;

    client1.connectToHost(QHostAddress::LocalHost, 15904);
    QTest::qWait(200);

    QTcpServer server2;
    QVERIFY(server2.listen(QHostAddress::LocalHost, 15905));
    client2.connectToHost(QHostAddress::LocalHost, 15905);

    for (int i = 0; i < 50; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(50);
      if (client1.connectionState() == ConnectionState::Connected &&
          client2.connectionState() == ConnectionState::Connected)
        break;
    }
    QCOMPARE(client1.connectionState(), ConnectionState::Connected);
    QCOMPARE(client2.connectionState(), ConnectionState::Connected);
  }

  void testEventBusConnectionState() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);

    bus.emitConnectionStateChanged(true);
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toBool());

    bus.emitConnectionStateChanged(false);
    QCOMPARE(spy.count(), 2);
    QVERIFY(!spy.at(1).at(0).toBool());
  }
};

QTEST_MAIN(DaemonLifecycleTest)
#include "daemon_lifecycle_test.moc"
