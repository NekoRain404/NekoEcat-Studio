// EtherCATReplicationServiceTest — Tests for EtherCATReplicationService
//
// Test coverage:
//   - Configuration, data, state, and backup replication fail closed offline
//   - Replication history is not synthesized without a live backend
//   - Empty target list handling
//   - Success signals are not synthesized offline

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATReplicationService.h"

class EtherCATReplicationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Replicate configuration fails closed without a live backend.
  void testReplicateConfigurationFailsClosedWithoutBackend() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    QVERIFY(!svc.replicateConfiguration(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Replicate data fails closed without a live backend.
  void testReplicateDataFailsClosedWithoutBackend() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(!svc.replicateData(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Replicate state fails closed without a live backend.
  void testReplicateStateFailsClosedWithoutBackend() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(!svc.replicateState(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Replicate backup fails closed without a live backend.
  void testReplicateBackupFailsClosedWithoutBackend() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01")};
    QVERIFY(!svc.replicateBackup(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Offline replication attempts do not synthesize history.
  void testReplicationHistoryDoesNotAccumulateOfflineAttempts() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QStringList targets = {QStringLiteral("node-01"), QStringLiteral("node-02")};
    svc.replicateConfiguration(targets);
    svc.replicateData({QStringLiteral("node-03")});
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Empty target list produces no history
  // Empty target list produces no history entries
  void testReplicateEmptyTargets() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QVERIFY(svc.replicateConfiguration(QStringList()));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // replicationStarted is not emitted for offline failure.
  void testReplicationStartedSignalNotEmittedWithoutBackend() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATReplicationService::replicationStarted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 0);
  }

  // replicationCompleted is not emitted for offline failure.
  void testReplicationCompletedSignalNotEmittedWithoutBackend() {
    EtherCATReplicationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATReplicationService::replicationCompleted);
    svc.replicateConfiguration({QStringLiteral("node-01")});
    QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(EtherCATReplicationServiceTest)
#include "ethercat_replication_service_test.moc"
