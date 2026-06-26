// WorkflowSyncServiceTest — Tests for WorkflowSyncService
//
// Test coverage:
//   - Time, data, state, and configuration sync fail closed without backend
//   - Sync status remains unchanged for rejected requests
//   - No synthetic success signals are emitted without backend

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowSyncService.h"

class WorkflowSyncServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testSyncTime() {
    WorkflowSyncService svc;
    QVERIFY(!svc.syncTime());
    auto status = svc.syncStatus();
    QCOMPARE(status.syncCount, 0);
    QVERIFY(!status.lastSync.isValid());
  }

  void testSyncTimeSignal() {
    WorkflowSyncService svc;
    QSignalSpy spy(&svc, &WorkflowSyncService::timeSynced);
    svc.syncTime();
    QCOMPARE(spy.count(), 0);
  }

  void testSyncData() {
    WorkflowSyncService svc;
    QVERIFY(!svc.syncData());
    QCOMPARE(svc.syncStatus().syncCount, 0);
  }

  void testSyncDataSignal() {
    WorkflowSyncService svc;
    QSignalSpy spy(&svc, &WorkflowSyncService::dataSynced);
    svc.syncData();
    QCOMPARE(spy.count(), 0);
  }

  void testSyncState() {
    WorkflowSyncService svc;
    QVERIFY(!svc.syncState());
    QCOMPARE(svc.syncStatus().syncCount, 0);
  }

  void testSyncConfiguration() {
    WorkflowSyncService svc;
    QVERIFY(!svc.syncConfiguration());
    QCOMPARE(svc.syncStatus().syncCount, 0);
  }

  void testRejectedSyncsDoNotAccumulate() {
    WorkflowSyncService svc;
    svc.syncTime();
    svc.syncData();
    svc.syncState();
    svc.syncConfiguration();
    QCOMPARE(svc.syncStatus().syncCount, 0);
    QCOMPARE(svc.syncStatus().errorCount, 0);
    QVERIFY(!svc.syncStatus().lastSync.isValid());
  }
};

QTEST_MAIN(WorkflowSyncServiceTest)
#include "workflow_sync_service_test.moc"
