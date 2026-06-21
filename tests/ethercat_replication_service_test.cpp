// EtherCATReplicationServiceTest — Tests for EtherCATReplicationService
//
// Test coverage:
//   - Configuration, data, state, and backup replication
//   - Replication history tracking
//   - Empty target list handling
//   - Signal emission for replication start and completion

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATReplicationService.h"

class EtherCATReplicationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Replicate configuration to multiple targets
  // Replicate configuration to multiple targets
  void testReplicateConfiguration() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    QVERIFY(svc.replicateConfiguration(targets));
    QCOMPARE(svc.replicationHistory().size(), 2);
  }

  // Replicate data to target nodes
  // Replicate data to a single target
  void testReplicateData() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(svc.replicateData(targets));
    QCOMPARE(svc.replicationHistory().size(), 1);
  }

  // Replicate state to target nodes
  // Replicate state to a single target
  void testReplicateState() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(svc.replicateState(targets));
    QCOMPARE(svc.replicationHistory().size(), 1);
  }

  // Replicate backup to target nodes
  // Replicate backup to a single target
  void testReplicateBackup() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(svc.replicateBackup(targets));
    QCOMPARE(svc.replicationHistory().size(), 1);
  }

  // History accumulates across multiple replications
  // Replication history accumulates across operations
  void testReplicationHistoryCount() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    svc.replicateConfiguration(targets);
    svc.replicateData({QStringLiteral("node-03")});
    QCOMPARE(svc.replicationHistory().size(), 3);
  }

  // Empty target list produces no history
  // Empty target list produces no history entries
  void testReplicateEmptyTargets() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QVERIFY(svc.replicateConfiguration(QStringList()));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // replicationStarted signal fires on start
  // Verify replicationStarted signal emission
  void testReplicationStartedSignal() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATReplicationService::replicationStarted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 1);
  }

  // replicationCompleted signal fires on completion
  // Verify replicationCompleted signal emission
  void testReplicationCompletedSignal() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATReplicationService::replicationCompleted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(EtherCATReplicationServiceTest)
#include "ethercat_replication_service_test.moc"
