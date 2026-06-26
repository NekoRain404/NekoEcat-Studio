// EtherCATSyncServiceTest — Tests for EtherCATSyncService
//
// Test coverage:
//   - Time, data, state, and configuration sync fail closed offline
//   - Sync status is not advanced without a live backend
//   - Success signals are not synthesized offline

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATSyncService.h"

class EtherCATSyncServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Sync time fails closed without a live backend.
  void testSyncTimeFailsClosedWithoutBackend() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(!svc.syncTime());
    auto status = svc.syncStatus();
    QCOMPARE(status.syncCount, 0);
    QVERIFY(!status.lastSync.isValid());
  }

  // timeSynced is not emitted for offline failure.
  void testSyncTimeSignalNotEmittedWithoutBackend() {
    EtherCATSyncService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATSyncService::timeSynced);
    svc.syncTime();
    QCOMPARE(spy.count(), 0);
  }

  // Sync data fails closed without a live backend.
  void testSyncDataFailsClosedWithoutBackend() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(!svc.syncData());
    QCOMPARE(svc.syncStatus().syncCount, 0);
  }

  // dataSynced is not emitted for offline failure.
  void testSyncDataSignalNotEmittedWithoutBackend() {
    EtherCATSyncService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATSyncService::dataSynced);
    svc.syncData();
    QCOMPARE(spy.count(), 0);
  }

  // Sync state fails closed without a live backend.
  void testSyncStateFailsClosedWithoutBackend() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(!svc.syncState());
    QCOMPARE(svc.syncStatus().syncCount, 0);
  }

  // Sync configuration fails closed without a live backend.
  void testSyncConfigurationFailsClosedWithoutBackend() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(!svc.syncConfiguration());
    QCOMPARE(svc.syncStatus().syncCount, 0);
  }

  // Sync status does not accumulate rejected offline attempts.
  void testSyncStatusDoesNotAccumulateOfflineAttempts() {
    EtherCATSyncService svc(nullptr, nullptr);
    svc.syncTime();
    svc.syncData();
    auto status = svc.syncStatus();
    QCOMPARE(status.syncCount, 0);
    QVERIFY(!status.lastSync.isValid());
  }

  // Multiple offline sync types do not synthesize success counts.
  void testMultipleOfflineSyncsDoNotAccumulate() {
    EtherCATSyncService svc(nullptr, nullptr);
    svc.syncTime();
    svc.syncData();
    svc.syncState();
    svc.syncConfiguration();
    QCOMPARE(svc.syncStatus().syncCount, 0);
  }
};

QTEST_MAIN(EtherCATSyncServiceTest)
#include "ethercat_sync_service_test.moc"
