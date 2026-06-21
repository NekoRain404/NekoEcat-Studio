#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/StateMachineService.h"

class StateMachinePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testRequestStateThroughput() {
    StateMachineService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 100000;
    for (int i = 0; i < count; i++) {
      svc.requestState(i % 100, 1);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "RequestState throughput:" << count << "in" << elapsed << "ms";
  }

  void testValidateTransitionThroughput() {
    StateMachineService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000000;
    volatile bool sink = false;
    for (int i = 0; i < count; i++) {
      sink = svc.validateTransition(1, 2);
    }

    qint64 elapsed = timer.elapsed();
    Q_UNUSED(sink);
    QVERIFY(elapsed < 500);
    qDebug() << "ValidateTransition throughput:" << count << "in" << elapsed
             << "ms";
  }

  void testCurrentStateQueryLatency() {
    StateMachineService svc;
    svc.requestState(0, 1);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000000;
    volatile int sink = 0;
    for (int i = 0; i < count; i++) {
      sink = svc.currentState(0);
    }

    qint64 elapsed = timer.elapsed();
    Q_UNUSED(sink);
    QVERIFY(elapsed < 500);
    qDebug() << "CurrentState query latency:" << count << "in" << elapsed
             << "ms";
  }

  void testHistoryQueryLatency() {
    StateMachineService svc;
    for (int i = 0; i < 100; i++) {
      svc.requestState(0, 1);
      svc.requestState(0, 2);
    }

    QElapsedTimer timer;
    timer.start();

    const int count = 100000;
    volatile int sink = 0;
    for (int i = 0; i < count; i++) {
      sink = svc.stateHistory(0).size();
    }

    qint64 elapsed = timer.elapsed();
    Q_UNUSED(sink);
    QVERIFY(elapsed < 500);
    qDebug() << "History query latency:" << count << "in" << elapsed << "ms";
  }

  void testMemoryStability() {
    for (int round = 0; round < 100; round++) {
      StateMachineService svc;
      for (int i = 0; i < 1000; i++) {
        svc.requestState(i % 100, 1);
        svc.requestState(i % 100, 2);
        svc.stateHistory(i % 100);
      }
    }
    qDebug() << "Memory stability: 100 rounds of 1000 transitions completed";
  }
};

QTEST_MAIN(StateMachinePerformanceTest)
#include "state_machine_performance_test.moc"
