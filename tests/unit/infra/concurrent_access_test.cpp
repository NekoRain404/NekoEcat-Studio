// ConcurrentAccessTest — Tests for concurrent EventBus access
//
// Test coverage:
//   - Concurrent slaveChanged emissions
//   - Concurrent sdoValue emissions
//   - Mixed concurrent event types
//   - Thread safety validation

#include <QTest>
#include <QThread>
#include <QTimer>
#include <QSignalSpy>
#include <QAtomicInt>
#include <QElapsedTimer>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class ConcurrentWorker : public QThread {
  Q_OBJECT
public:
  ConcurrentWorker(EventBus *bus, int id) : bus_(bus), id_(id) {}

  void run() override {
    const int myId = id_;
    EventBus *bus = bus_;  // copy for the lambda (worker is deleted before it runs)
    for (int i = 0; i < 100; ++i) {
      SlaveInfo info;
      info.position = myId * 1000 + i;
      info.name = QString("Worker_%1_Slave_%2").arg(myId).arg(i);

      QVector<SlaveInfo> slaves{info};
      QMetaObject::invokeMethod(bus, [bus, slaves] {
          bus->emitSlaveChanged(slaves);
      }, Qt::QueuedConnection);

      QMetaObject::invokeMethod(bus, [bus, myId, i] {
          bus->emitSdoValue(myId, QString("0x%1").arg(i, 4, 16, QChar('0')), "0x00", "0x0000");
      }, Qt::QueuedConnection);
    }
  }

private:
  EventBus *bus_;
  int id_;
};

class ConcurrentAccessTest : public QObject {
  Q_OBJECT

  // EventBus must outlive all queued emissions; keep it a member so it lives
  // for the whole test class rather than per-slot (a stack bus in a slot would
  // be destroyed while other tests' still-pending queued calls target it).
  EventBus bus_;

  // Drain queued emissions delivered on the main thread after the workers
  // finish. Keeps processing until the count is reached AND no new events
  // arrive for a quiet period (so nothing stays pending into the next test).
  template <typename Spy>
  static int drain(Spy &spy, int expected, int timeoutMs = 8000) {
    QElapsedTimer t;
    t.start();
    int last = -1, quiet = 0;
    while (t.elapsed() < timeoutMs) {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
      const int n = spy.count();
      if (n == last) {
        if (++quiet >= 30 && n >= expected) break;  // stable AND sufficient
      } else {
        last = n;
        quiet = 0;
      }
    }
    return spy.count();
  }

private slots:
  // Verify concurrent slaveChanged emissions are all received
  void testConcurrentSlaveChanged() {
    QSignalSpy spy(&bus_, &EventBus::slaveChanged);

    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;

    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus_, i);
      workers.append(worker);
      worker->start();
    }

    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }

    QCOMPARE(drain(spy, workerCount * 100), workerCount * 100);
  }

  // Verify concurrent sdoValue emissions are all received
  void testConcurrentSdoValue() {
    QSignalSpy spy(&bus_, &EventBus::sdoValueReceived);

    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;

    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus_, i);
      workers.append(worker);
      worker->start();
    }

    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }

    QCOMPARE(drain(spy, workerCount * 100), workerCount * 100);
  }

  // Verify mixed concurrent event types are handled correctly
  void testConcurrentDifferentEvents() {
    QSignalSpy slaveSpy(&bus_, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus_, &EventBus::sdoValueReceived);
    QSignalSpy connectionSpy(&bus_, &EventBus::connectionStateChanged);

    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;

    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus_, i);
      workers.append(worker);
      worker->start();
    }

    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }

    QCOMPARE(drain(slaveSpy, workerCount * 100), workerCount * 100);
    QCOMPARE(drain(sdoSpy, workerCount * 100), workerCount * 100);
  }

  void testMainThreadAccess() {
    QSignalSpy spy(&bus_, &EventBus::slaveChanged);

    SlaveInfo info;
    info.position = 1;
    info.name = "MainThreadSlave";

    QVector<SlaveInfo> slaves{info};
    bus_.emitSlaveChanged(slaves);

    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).name, QString("MainThreadSlave"));
  }
};

QTEST_MAIN(ConcurrentAccessTest)
#include "concurrent_access_test.moc"
