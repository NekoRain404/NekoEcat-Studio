#include <QCoreApplication>
#include <QTest>
#include <QSignalSpy>

#include "OpStateService.h"

class OpStateServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testInitialState() {
    OpStateService svc;
    QCOMPARE(svc.currentState(0), 0);
  }

  void testRequestOpStateWithoutBackendDoesNotSimulateSuccess() {
    OpStateService svc;
    QSignalSpy spy(&svc, &OpStateService::opStateChanged);

    QVERIFY(!svc.requestOpState(0));
    QCOMPARE(svc.currentState(0), 0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toInt(), 0);
    QCOMPARE(spy[0][1].toBool(), false);

    auto history = svc.transitionHistory(0);
    QCOMPARE(history.size(), 1);
    QCOMPARE(history[0].fromState, 0);
    QCOMPARE(history[0].toState, 8);
    QVERIFY(!history[0].success);
    QVERIFY(!history[0].error.isEmpty());
  }

  void testRequestOpStateFromInitFailsClosed() {
    OpStateService svc;
    QCOMPARE(svc.currentState(0), 0);

    QSignalSpy spy(&svc, &OpStateService::opStateChanged);
    QVERIFY(!svc.requestOpState(0));
    QCOMPARE(svc.currentState(0), 0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toInt(), 0);
    QCOMPARE(spy[0][1].toBool(), false);
  }

  void testRepeatedRequestDoesNotCreateLocalOpState() {
    OpStateService svc;
    QVERIFY(!svc.requestOpState(0));
    QVERIFY(!svc.requestOpState(0));
    QCOMPARE(svc.currentState(0), 0);
    QCOMPARE(svc.transitionHistory(0).size(), 2);
  }

  void testCheckOpStateAfterFailedRequest() {
    OpStateService svc;
    svc.requestOpState(0);

    auto status = svc.checkOpState(0);
    QCOMPARE(status.position, 0);
    QCOMPARE(status.currentState, 0);
    QCOMPARE(status.targetState, 8);
    QVERIFY(status.error.isEmpty());
    QVERIFY(status.timestamp.isValid());
  }

  void testCheckOpStateNotInOp() {
    OpStateService svc;
    auto status = svc.checkOpState(0);
    QCOMPARE(status.currentState, 0);
    QCOMPARE(status.targetState, 8);
    QVERIFY(!status.details.isEmpty());
  }

  void testHandleOpStateError() {
    OpStateService svc;
    OpStateErrorInfo err;
    err.errorCode = 0x1234;
    err.message = "Sync error";
    err.source = "Slave";
    err.recoverable = true;

    QSignalSpy spy(&svc, &OpStateService::opStateError);
    QVERIFY(svc.handleOpStateError(0, err));
    QCOMPARE(spy.count(), 1);

    auto history = svc.errorHistory(0);
    QCOMPARE(history.size(), 1);
    QCOMPARE(history[0].errorCode, 0x1234);
  }

  void testHandleNonRecoverableError() {
    OpStateService svc;
    OpStateErrorInfo err;
    err.errorCode = 0xFFFF;
    err.message = "Fatal error";
    err.recoverable = false;

    QSignalSpy spy(&svc, &OpStateService::opStateError);
    QVERIFY(!svc.handleOpStateError(0, err));
    QCOMPARE(spy.count(), 1);
  }

  void testRecoverWithoutKnownOnlineStateFailsClosed() {
    OpStateService svc;
    svc.requestOpState(0);
    QCOMPARE(svc.currentState(0), 0);

    QSignalSpy spy(&svc, &OpStateService::opStateChanged);
    QVERIFY(!svc.recoverFromError(0));
    QCOMPARE(svc.currentState(0), 0);
    QCOMPARE(spy.count(), 0);
  }

  void testRepeatedRecoverWithoutKnownOnlineStateFailsClosed() {
    OpStateService svc;
    svc.requestOpState(0);
    QVERIFY(!svc.recoverFromError(0));
    QVERIFY(!svc.recoverFromError(0));
    QCOMPARE(svc.currentState(0), 0);
  }

  void testTransitionHistory() {
    OpStateService svc;
    svc.requestOpState(0);

    auto history = svc.transitionHistory(0);
    QCOMPARE(history.size(), 1);
    QCOMPARE(history[0].fromState, 0);
    QCOMPARE(history[0].toState, 8);
    QVERIFY(!history[0].success);
    QVERIFY(!history[0].error.isEmpty());
  }

  void testErrorHistory() {
    OpStateService svc;
    OpStateErrorInfo err1;
    err1.errorCode = 1;
    err1.message = "Error 1";
    err1.recoverable = true;
    svc.handleOpStateError(0, err1);

    OpStateErrorInfo err2;
    err2.errorCode = 2;
    err2.message = "Error 2";
    err2.recoverable = true;
    svc.handleOpStateError(0, err2);

    auto history = svc.errorHistory(0);
    QCOMPARE(history.size(), 2);
    QCOMPARE(history[0].errorCode, 1);
    QCOMPARE(history[1].errorCode, 2);
  }

  void testMultiplePositions() {
    OpStateService svc;
    QVERIFY(!svc.requestOpState(0));
    QVERIFY(!svc.requestOpState(1));

    QCOMPARE(svc.currentState(0), 0);
    QCOMPARE(svc.currentState(1), 0);

    QCOMPARE(svc.transitionHistory(0).size(), 1);
    QCOMPARE(svc.transitionHistory(1).size(), 1);
  }

  void testOpStateStatusTimestamp() {
    OpStateService svc;
    auto status = svc.checkOpState(0);
    QVERIFY(status.timestamp.isValid());
  }
};

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  OpStateServiceTest t;
  return QTest::qExec(&t, argc, argv);
}

#include "op_state_service_test.moc"
