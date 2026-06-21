// WorkflowReplicationServiceTest — Tests for WorkflowReplicationService
//
// Test coverage:
//   - Configuration, data, state, and backup replication
//   - Replication history tracking
//   - Empty target list handling
//   - Signal emission for replication start and completion

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowReplicationService.h"

class WorkflowReplicationServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testReplicateConfiguration() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    QVERIFY(svc.replicateConfiguration(targets));
    QCOMPARE(svc.replicationHistory().size(), 2);
  }

  void testReplicateData() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(svc.replicateData(targets));
    QCOMPARE(svc.replicationHistory().size(), 1);
  }

  void testReplicateState() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(svc.replicateState(targets));
    QCOMPARE(svc.replicationHistory().size(), 1);
  }

  void testReplicateBackup() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(svc.replicateBackup(targets));
    QCOMPARE(svc.replicationHistory().size(), 1);
  }

  void testReplicationHistoryCount() {
    WorkflowReplicationService svc;
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    svc.replicateConfiguration(targets);
    svc.replicateData({QStringLiteral("node-03")});
    QCOMPARE(svc.replicationHistory().size(), 3);
  }

  void testReplicateEmptyTargets() {
    WorkflowReplicationService svc;
    QVERIFY(svc.replicateConfiguration(QStringList()));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  void testReplicationStartedSignal() {
    WorkflowReplicationService svc;
    QSignalSpy spy(&svc, &WorkflowReplicationService::replicationStarted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 1);
  }

  void testReplicationCompletedSignal() {
    WorkflowReplicationService svc;
    QSignalSpy spy(&svc, &WorkflowReplicationService::replicationCompleted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(WorkflowReplicationServiceTest)
#include "workflow_replication_service_test.moc"
