// WorkflowSyncServiceTest — Tests for WorkflowSyncService
//
// Test coverage:
//   - Time, data, state, and configuration sync
//   - Sync status tracking and accumulation
//   - Signal emission for time and data sync

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowSyncService.h"

class WorkflowSyncServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testSyncTime() {
    WorkflowSyncService svc;
    QVERIFY(svc.syncTime());
    auto status = svc.syncStatus();
    QCOMPARE(status.syncCount, 1);
    QVERIFY(status.lastSync.isValid());
  }

  void testSyncTimeSignal() {
    WorkflowSyncService svc;
    QSignalSpy spy(&svc, &WorkflowSyncService::timeSynced);
    svc.syncTime();
    QCOMPARE(spy.count(), 1);
  }

  void testSyncData() {
    WorkflowSyncService svc;
    QVERIFY(svc.syncData());
    QCOMPARE(svc.syncStatus().syncCount, 1);
  }

  void testSyncDataSignal() {
    WorkflowSyncService svc;
    QSignalSpy spy(&svc, &WorkflowSyncService::dataSynced);
    svc.syncData();
    QCOMPARE(spy.count(), 1);
  }

  void testSyncState() {
    WorkflowSyncService svc;
    QVERIFY(svc.syncState());
    QCOMPARE(svc.syncStatus().syncCount, 1);
  }

  void testSyncConfiguration() {
    WorkflowSyncService svc;
    QVERIFY(svc.syncConfiguration());
    QCOMPARE(svc.syncStatus().syncCount, 1);
  }

  void testMultipleSyncsAccumulate() {
    WorkflowSyncService svc;
    svc.syncTime();
    svc.syncData();
    svc.syncState();
    svc.syncConfiguration();
    QCOMPARE(svc.syncStatus().syncCount, 4);
  }
};

QTEST_MAIN(WorkflowSyncServiceTest)
#include "workflow_sync_service_test.moc"
