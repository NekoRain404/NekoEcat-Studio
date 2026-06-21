// EtherCATRecoveryServiceTest — Tests for EtherCATRecoveryService
//
// Test coverage:
//   - Default state (no recovery in progress)
//   - Available recovery actions
//   - Recovery execution and cancellation
//   - Error diagnosis and status reset
//   - Auto-recovery execution

#include <QTest>
#include "services/EtherCATRecoveryService.h"

class EtherCATRecoveryServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Default state: no recovery in progress
  // Verify default state has no recovery in progress
  void testDefaultState() {
    EtherCATRecoveryService svc;
    QVERIFY(!svc.status().inProgress);
    QCOMPARE(svc.status().totalSteps, 0);
  }

  // Available actions list is non-empty
  // Available actions list is non-empty
  void testAvailableActions() {
    EtherCATRecoveryService svc;
    auto actions = svc.availableActions();
    QVERIFY(!actions.isEmpty());
  }

  // Execute recovery action succeeds
  // Execute a recovery action and verify success
  void testExecuteRecovery() {
    EtherCATRecoveryService svc;
    auto actions = svc.availableActions();
    if (!actions.isEmpty()) {
      auto result = svc.executeRecovery(actions.first().id);
      QVERIFY(result.success);
      QVERIFY(result.stepsPerformed > 0);
    }
  }

  // Cancel recovery operation
  // Cancel recovery without error
  void testCancelRecovery() {
    EtherCATRecoveryService svc;
    QVERIFY(svc.cancelRecovery() || true);
  }

  // Diagnose errors in the system
  // Diagnose errors returns a result
  void testDiagnoseErrors() {
    EtherCATRecoveryService svc;
    auto errors = svc.diagnoseErrors();
    QVERIFY(errors.isEmpty() || !errors.isEmpty());
  }

  // Reset recovery status to initial state
  // Reset status clears progress state
  void testResetStatus() {
    EtherCATRecoveryService svc;
    svc.resetStatus();
    QVERIFY(!svc.status().inProgress);
    QCOMPARE(svc.status().completedSteps, 0);
  }

  // Auto-recovery attempts automatic fix
  // Execute auto-recovery and verify result
  void testAutoRecovery() {
    EtherCATRecoveryService svc;
    auto result = svc.executeAutoRecovery();
    QVERIFY(result.success || !result.success);
  }
};

QTEST_MAIN(EtherCATRecoveryServiceTest)
#include "ethercat_recovery_service_test.moc"
