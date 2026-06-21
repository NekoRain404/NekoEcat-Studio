// StateMachineServiceTest — Tests for State Machine Service
//
// Test coverage:
//   - Initial state (all positions at 0)
//   - Valid state request with signal emission
//   - Same-state request (no signal)
//   - Invalid state request with failure signal
//   - Valid and invalid transition validation
//   - Same-state transitions
//   - State history tracking (success and failure)
//   - State recovery from higher states
//   - Multiple position management
//   - Full lifecycle (INIT→PRE-OP→SAFE-OP→OP)
//   - Transition timestamp validity
#include <QTest>
#include <QSignalSpy>
#include "services/StateMachineService.h"

class StateMachineServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify initial state is INIT (0) for all positions
  void testInitialState() {
    StateMachineService svc;
    QCOMPARE(svc.currentState(0), 0);
    QCOMPARE(svc.currentState(1), 0);
  }

  // Test valid state transition emits signal and updates state
  void testRequestValidState() {
    StateMachineService svc;
    QSignalSpy spy(&svc, &StateMachineService::stateChanged);
    QVERIFY(svc.requestState(0, 1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.currentState(0), 1);
  }

  // Verify requesting same state does not emit signal
  void testRequestSameState() {
    StateMachineService svc;
    svc.requestState(0, 1);
    QSignalSpy spy(&svc, &StateMachineService::stateChanged);
    QVERIFY(svc.requestState(0, 1));
    QCOMPARE(spy.count(), 0);
  }

  // Test invalid state transition emits failure signal
  void testInvalidState() {
    StateMachineService svc;
    QSignalSpy spy(&svc, &StateMachineService::stateTransitionFailed);
    QVERIFY(!svc.requestState(0, 3));
    QCOMPARE(spy.count(), 1);
  }

  // Verify all valid state transitions are accepted
  void testValidTransitions() {
    StateMachineService svc;
    QVERIFY(svc.validateTransition(0, 1));
    QVERIFY(svc.validateTransition(1, 2));
    QVERIFY(svc.validateTransition(2, 4));
    QVERIFY(svc.validateTransition(4, 8));
    QVERIFY(svc.validateTransition(4, 2));
    QVERIFY(svc.validateTransition(4, 1));
    QVERIFY(svc.validateTransition(2, 1));
    QVERIFY(svc.validateTransition(8, 4));
  }

  // Verify invalid state transitions are rejected
  void testInvalidTransitions() {
    StateMachineService svc;
    QVERIFY(!svc.validateTransition(1, 4));
    QVERIFY(!svc.validateTransition(1, 8));
    QVERIFY(!svc.validateTransition(2, 8));
    QVERIFY(!svc.validateTransition(8, 1));
    QVERIFY(!svc.validateTransition(8, 2));
  }

  // Verify same-state transitions are valid
  void testSameStateTransition() {
    StateMachineService svc;
    QVERIFY(svc.validateTransition(1, 1));
    QVERIFY(svc.validateTransition(2, 2));
    QVERIFY(svc.validateTransition(4, 4));
    QVERIFY(svc.validateTransition(8, 8));
  }

  // Test state history records successful transitions
  void testStateHistory() {
    StateMachineService svc;
    svc.requestState(0, 1);
    svc.requestState(0, 2);
    auto history = svc.stateHistory(0);
    QCOMPARE(history.size(), 2);
    QVERIFY(history[0].success);
    QVERIFY(history[1].success);
  }

  // Test failed transition is recorded in history
  void testFailedTransitionHistory() {
    StateMachineService svc;
    svc.requestState(0, 1);
    svc.requestState(0, 4);
    auto history = svc.stateHistory(0);
    QCOMPARE(history.size(), 2);
    QVERIFY(history[0].success);
    QVERIFY(!history[1].success);
  }

  // Test state recovery reverts to previous state
  void testRecoverState() {
    StateMachineService svc;
    svc.requestState(0, 1);
    svc.requestState(0, 2);
    svc.requestState(0, 4);
    svc.requestState(0, 8);
    QSignalSpy spy(&svc, &StateMachineService::stateChanged);
    QVERIFY(svc.recoverState(0));
    QCOMPARE(svc.currentState(0), 4);
  }

  // Verify recovery from INIT state fails
  void testRecoverFromInit() {
    StateMachineService svc;
    svc.requestState(0, 1);
    QVERIFY(!svc.recoverState(0));
  }

  // Test multiple positions maintain independent states
  void testMultiplePositions() {
    StateMachineService svc;
    svc.requestState(0, 1);
    svc.requestState(1, 1);
    svc.requestState(0, 2);
    QCOMPARE(svc.currentState(0), 2);
    QCOMPARE(svc.currentState(1), 1);
  }

  // Verify history is isolated per position
  void testHistoryPerPosition() {
    StateMachineService svc;
    svc.requestState(0, 1);
    svc.requestState(1, 1);
    svc.requestState(1, 2);
    QCOMPARE(svc.stateHistory(0).size(), 1);
    QCOMPARE(svc.stateHistory(1).size(), 2);
  }

  // Test full INIT to OP lifecycle
  void testFullLifecycle() {
    StateMachineService svc;
    QSignalSpy spy(&svc, &StateMachineService::stateChanged);
    QVERIFY(svc.requestState(0, 1));
    QVERIFY(svc.requestState(0, 2));
    QVERIFY(svc.requestState(0, 4));
    QVERIFY(svc.requestState(0, 8));
    QCOMPARE(spy.count(), 4);
    QCOMPARE(svc.currentState(0), 8);
  }

  // Verify transition timestamps are valid
  void testTransitionTimestamp() {
    StateMachineService svc;
    svc.requestState(0, 1);
    auto history = svc.stateHistory(0);
    QVERIFY(history[0].timestamp.isValid());
  }
};

QTEST_MAIN(StateMachineServiceTest)
#include "state_machine_service_test.moc"
