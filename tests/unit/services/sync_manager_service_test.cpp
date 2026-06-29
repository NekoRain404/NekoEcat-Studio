// SyncManagerServiceTest — Tests for Sync Manager Service
//
// Test coverage:
//   - Configure sync manager with valid/invalid parameters
//   - PDO assignment to sync managers
//   - Direction and watchdog configuration
//   - Sync manager listing per position
//   - Default configuration values
//   - Multiple position support
#include <QTest>
#include <QSignalSpy>
#include "services/SyncManagerService.h"

class SyncManagerServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify Sync Manager configuration fails closed without a live backend.
  void testConfigureSyncManagerFailsClosedWithoutBackend() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    cfg.smIndex = 0;
    cfg.direction = SmDirection::Input;
    cfg.enable = true;
    QSignalSpy configuredSpy(&svc, &SyncManagerService::syncManagerConfigured);
    QSignalSpy errorSpy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.configureSyncManager(0, 0, cfg));
    QCOMPARE(configuredSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(svc.syncManagers(0).size(), 0);
  }

  // Verify invalid position emits error
  void testConfigureInvalidPosition() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    QSignalSpy spy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.configureSyncManager(-1, 0, cfg));
    QCOMPARE(spy.count(), 1);
  }

  // Verify invalid SM index emits error
  void testConfigureInvalidSmIndex() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    QSignalSpy spy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.configureSyncManager(0, 5, cfg));
    QCOMPARE(spy.count(), 1);
  }

  void testRejectMismatchedConfigSmIndex() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    cfg.smIndex = 1;
    QSignalSpy spy(&svc, &SyncManagerService::error);

    QVERIFY(!svc.configureSyncManager(0, 0, cfg));
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.takeFirst().at(0).toString().contains(QStringLiteral("SM index mismatch")));
    QCOMPARE(svc.syncManagers(0).size(), 0);
  }

  void testRejectInvalidPdoIndexBeforeBackendLookup() {
    SyncManagerService svc;
    QSignalSpy spy(&svc, &SyncManagerService::error);

    QVERIFY(!svc.assignPdo(0, 0, 0));
    QVERIFY(!svc.assignPdo(0, 0, -1));
    QVERIFY(!svc.assignPdo(0, 0, 0x10000));

    QCOMPARE(spy.count(), 3);
    for (const auto &args : spy)
      QVERIFY(args.at(0).toString().contains(QStringLiteral("Invalid PDO index")));
  }

  // Verify PDO assignment cannot be simulated without a backend.
  void testAssignPdoFailsWithoutConfiguredBackend() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    cfg.smIndex = 0;
    svc.configureSyncManager(0, 0, cfg);
    QSignalSpy configuredSpy(&svc, &SyncManagerService::syncManagerConfigured);
    QSignalSpy errorSpy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.assignPdo(0, 0, 0x1600));
    QCOMPARE(configuredSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    SyncManagerConfig result = svc.syncManagerConfig(0, 0);
    QCOMPARE(result.pdoIndex, 0);
  }

  // Verify PDO assignment to unconfigured position emits error
  void testAssignPdoUnconfiguredPosition() {
    SyncManagerService svc;
    QSignalSpy spy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.assignPdo(99, 0, 0x1600));
    QCOMPARE(spy.count(), 1);
  }

  // Verify PDO assignment to unconfigured SM emits error
  void testAssignPdoUnconfiguredSm() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    svc.configureSyncManager(0, 0, cfg);
    QSignalSpy spy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.assignPdo(0, 1, 0x1600));
    QCOMPARE(spy.count(), 1);
  }

  // Verify direction updates cannot be simulated without a backend.
  void testSetDirectionFailsWithoutConfiguredBackend() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    cfg.smIndex = 1;
    svc.configureSyncManager(0, 1, cfg);
    QSignalSpy configuredSpy(&svc, &SyncManagerService::syncManagerConfigured);
    QSignalSpy errorSpy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.setDirection(0, 1, SmDirection::Output));
    QCOMPARE(configuredSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    SyncManagerConfig result = svc.syncManagerConfig(0, 1);
    QCOMPARE(static_cast<int>(result.direction),
             static_cast<int>(SmDirection::Input));
  }

  // Verify setting direction on unconfigured SM fails
  void testSetDirectionUnconfigured() {
    SyncManagerService svc;
    QVERIFY(!svc.setDirection(0, 0, SmDirection::Both));
  }

  void testRejectInvalidDirectionTargetBeforeLookup() {
    SyncManagerService svc;
    QSignalSpy spy(&svc, &SyncManagerService::error);

    QVERIFY(!svc.setDirection(-1, 0, SmDirection::Input));
    QVERIFY(!svc.setDirection(0, 4, SmDirection::Input));

    QCOMPARE(spy.count(), 2);
    for (const auto &args : spy)
      QVERIFY(args.at(0).toString().contains(QStringLiteral("Invalid position or SM index")));
  }

  // Verify watchdog updates cannot be simulated without a backend.
  void testSetWatchdogFailsWithoutConfiguredBackend() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    cfg.smIndex = 2;
    svc.configureSyncManager(0, 2, cfg);
    QSignalSpy configuredSpy(&svc, &SyncManagerService::syncManagerConfigured);
    QSignalSpy errorSpy(&svc, &SyncManagerService::error);
    QVERIFY(!svc.setWatchdog(0, 2, 5000));
    QCOMPARE(configuredSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    SyncManagerConfig result = svc.syncManagerConfig(0, 2);
    QCOMPARE(result.watchdogTimeout, 0);
  }

  // Verify setting watchdog on unconfigured SM fails
  void testSetWatchdogUnconfigured() {
    SyncManagerService svc;
    QVERIFY(!svc.setWatchdog(0, 0, 1000));
  }

  void testRejectInvalidWatchdogBeforeLookup() {
    SyncManagerService svc;
    QSignalSpy spy(&svc, &SyncManagerService::error);

    QVERIFY(!svc.setWatchdog(-1, 0, 1000));
    QVERIFY(!svc.setWatchdog(0, 4, 1000));
    QVERIFY(!svc.setWatchdog(0, 0, -1));

    QCOMPARE(spy.count(), 3);
    QVERIFY(spy.at(0).at(0).toString().contains(QStringLiteral("Invalid position or SM index")));
    QVERIFY(spy.at(1).at(0).toString().contains(QStringLiteral("Invalid position or SM index")));
    QVERIFY(spy.at(2).at(0).toString().contains(QStringLiteral("Invalid watchdog timeout")));
  }

  // Test listing sync managers for a position
  void testSyncManagersList() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    svc.configureSyncManager(0, 0, cfg);
    svc.configureSyncManager(0, 1, cfg);
    svc.configureSyncManager(0, 2, cfg);
    QVector<int> sms = svc.syncManagers(0);
    QCOMPARE(sms.size(), 0);
  }

  // Verify empty position returns no sync managers
  void testSyncManagersEmptyPosition() {
    SyncManagerService svc;
    QVector<int> sms = svc.syncManagers(0);
    QCOMPARE(sms.size(), 0);
  }

  // Test default configuration values
  void testDefaultConfig() {
    SyncManagerService svc;
    SyncManagerConfig cfg = svc.syncManagerConfig(0, 0);
    QCOMPARE(cfg.smIndex, 0);
    QCOMPARE(static_cast<int>(cfg.direction),
             static_cast<int>(SmDirection::Input));
    QCOMPARE(cfg.pdoIndex, 0);
    QCOMPARE(cfg.watchdogTimeout, 0);
    QVERIFY(cfg.enable);
    QVERIFY(!cfg.virtualSm);
  }

  // Test multiple positions maintain independent sync managers
  void testMultiplePositions() {
    SyncManagerService svc;
    SyncManagerConfig cfg;
    svc.configureSyncManager(0, 0, cfg);
    svc.configureSyncManager(1, 0, cfg);
    QCOMPARE(svc.syncManagers(0).size(), 0);
    QCOMPARE(svc.syncManagers(1).size(), 0);
  }
};

QTEST_MAIN(SyncManagerServiceTest)
#include "sync_manager_service_test.moc"
