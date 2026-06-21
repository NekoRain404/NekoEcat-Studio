// EtherCATSyncServiceTest — Tests for EtherCATSyncService
//
// Test coverage:
//   - Time, data, state, and configuration sync
//   - Sync status tracking and accumulation
//   - Signal emission for time and data sync

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATSyncService.h"

class EtherCATSyncServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Sync time and verify sync count and timestamp
  // Sync time and verify status
  void testSyncTime() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(svc.syncTime());
    auto status = svc.syncStatus();
    QCOMPARE(status.syncCount, 1);
    QVERIFY(status.lastSync.isValid());
  }

  // Verify timeSynced signal emission
  // timeSynced signal fires on sync
  void testSyncTimeSignal() {
    EtherCATSyncService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATSyncService::timeSynced);
    svc.syncTime();
    QCOMPARE(spy.count(), 1);
  }

  // Sync data and verify sync count
  // Sync data and verify status
  void testSyncData() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(svc.syncData());
    QCOMPARE(svc.syncStatus().syncCount, 1);
  }

  // Verify dataSynced signal emission
  // dataSynced signal fires on sync
  void testSyncDataSignal() {
    EtherCATSyncService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATSyncService::dataSynced);
    svc.syncData();
    QCOMPARE(spy.count(), 1);
  }

  // Sync state and verify sync count
  // Sync state and verify count
  void testSyncState() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(svc.syncState());
    QCOMPARE(svc.syncStatus().syncCount, 1);
  }

  // Sync configuration and verify sync count
  // Sync configuration and verify count
  void testSyncConfiguration() {
    EtherCATSyncService svc(nullptr, nullptr);
    QVERIFY(svc.syncConfiguration());
    QCOMPARE(svc.syncStatus().syncCount, 1);
  }

  // Sync status tracks multiple sync operations
  // Sync status tracks cumulative count
  void testSyncStatus() {
    EtherCATSyncService svc(nullptr, nullptr);
    svc.syncTime();
    svc.syncData();
    auto status = svc.syncStatus();
    QCOMPARE(status.syncCount, 2);
    QVERIFY(status.lastSync.isValid());
  }

  // Multiple sync types accumulate in count
  // Multiple syncs accumulate to total count
  void testMultipleSyncsAccumulate() {
    EtherCATSyncService svc(nullptr, nullptr);
    svc.syncTime();
    svc.syncData();
    svc.syncState();
    svc.syncConfiguration();
    QCOMPARE(svc.syncStatus().syncCount, 4);
  }
};

QTEST_MAIN(EtherCATSyncServiceTest)
#include "ethercat_sync_service_test.moc"
