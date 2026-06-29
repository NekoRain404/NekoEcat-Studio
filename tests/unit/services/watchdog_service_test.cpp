// WatchdogServiceTest — Tests for Watchdog Service
//
// Test coverage:
//   - Initial monitoring state
//   - Offline start does not synthesize an active monitoring session
//   - Idempotent offline start
//   - Topology change updates slave list
//   - Status JSON contains slave information
//   - Offline monitoring does not emit runtime status changes
#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include <QJsonArray>
#include "services/WatchdogService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class WatchdogServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Initial state: not monitoring, no slaves, zero counters
  void testInitialState() {
    EventBus bus;
    EcatClient client;
    WatchdogService svc(&bus, &client);

    QCOMPARE(svc.isMonitoring(), false);
    QCOMPARE(svc.slaveStatuses().size(), 0);

    QJsonObject status = svc.currentStatus();
    QCOMPARE(status["totalTimeouts"].toInt(), 0);
    QCOMPARE(status["totalTriggers"].toInt(), 0);
    QCOMPARE(status["monitoring"].toBool(), false);
  }

  // Offline start requests must not synthesize an active monitoring session.
  void testStartMonitoringOfflineDoesNotActivate() {
    EventBus bus;
    EcatClient client;
    WatchdogService svc(&bus, &client);

    svc.startMonitoring(100);
    QCOMPARE(svc.isMonitoring(), false);

    svc.stopMonitoring();
    QCOMPARE(svc.isMonitoring(), false);
  }

  // Starting twice while offline remains inactive and idempotent.
  void testIdempotentStartWhileOffline() {
    EventBus bus;
    EcatClient client;
    WatchdogService svc(&bus, &client);

    svc.startMonitoring(100);
    svc.startMonitoring(200);
    QCOMPARE(svc.isMonitoring(), false);
    svc.stopMonitoring();
  }

  // Topology change event updates slave statuses
  void testTopologyChangeUpdatesSlaves() {
    EventBus bus;
    EcatClient client;
    WatchdogService svc(&bus, &client);

    QVector<SlaveInfo> slaves;
    SlaveInfo s1;
    s1.position = 0;
    s1.state = "OP";
    s1.name = "Slave 0";
    slaves.append(s1);

    SlaveInfo s2;
    s2.position = 1;
    s2.state = "OP";
    s2.name = "Slave 1";
    slaves.append(s2);

    bus.emitTopologyChanged(slaves);

    QCOMPARE(svc.slaveStatuses().size(), 2);
    QCOMPARE(svc.slaveStatuses()[0].position, 0);
    QCOMPARE(svc.slaveStatuses()[1].position, 1);
    QCOMPARE(svc.slaveStatuses()[0].watchdogOk, true);
  }

  // Status JSON contains slave position and watchdog state
  void testStatusJsonContainsSlaves() {
    EventBus bus;
    EcatClient client;
    WatchdogService svc(&bus, &client);

    QVector<SlaveInfo> slaves;
    SlaveInfo s;
    s.position = 3;
    s.state = "OP";
    s.name = "Drive";
    slaves.append(s);
    bus.emitTopologyChanged(slaves);

    QJsonObject status = svc.currentStatus();
    QJsonArray arr = status["slaves"].toArray();
    QCOMPARE(arr.size(), 1);
    QCOMPARE(arr[0].toObject()["position"].toInt(), 3);
    QCOMPARE(arr[0].toObject()["watchdogOk"].toBool(), true);
  }

  // Offline monitoring does not emit synthetic runtime status changes.
  void testOfflineStartDoesNotEmitStatusChangedSignal() {
    EventBus bus;
    EcatClient client;
    WatchdogService svc(&bus, &client);

    QSignalSpy spy(&svc, &WatchdogService::watchdogStatusChanged);
    QVERIFY(spy.isValid());

    svc.startMonitoring(50);
    QTest::qWait(120);
    svc.stopMonitoring();

    QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(WatchdogServiceTest)
#include "watchdog_service_test.moc"
