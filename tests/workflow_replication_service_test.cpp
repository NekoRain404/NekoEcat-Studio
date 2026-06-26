// WorkflowReplicationServiceTest — Tests for WorkflowReplicationService
//
// Test coverage:
//   - Configuration, data, state, and backup replication fail closed without backend
//   - Replication history is not synthesized
//   - Empty target list handling
//   - No synthetic replication start/completion signals

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowReplicationService.h"

class WorkflowReplicationServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testReplicateConfigurationFailsWithoutBackend() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    QVERIFY(!svc.replicateConfiguration(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  void testReplicateDataFailsWithoutBackend() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(!svc.replicateData(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  void testReplicateStateFailsWithoutBackend() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(!svc.replicateState(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  void testReplicateBackupFailsWithoutBackend() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(!svc.replicateBackup(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  void testReplicationHistoryRemainsEmptyWithoutBackend() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    svc.replicateConfiguration(targets);
    svc.replicateData({QStringLiteral("node-03")});
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  void testReplicateEmptyTargets() {
    WorkflowReplicationService svc;
    QVERIFY(!svc.replicateConfiguration(QStringList()));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  void testNoSyntheticReplicationStartedSignal() {
    WorkflowReplicationService svc;
    QSignalSpy spy(&svc, &WorkflowReplicationService::replicationStarted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 0);
  }

  void testNoSyntheticReplicationCompletedSignal() {
    WorkflowReplicationService svc;
    QSignalSpy spy(&svc, &WorkflowReplicationService::replicationCompleted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(WorkflowReplicationServiceTest)
#include "workflow_replication_service_test.moc"
