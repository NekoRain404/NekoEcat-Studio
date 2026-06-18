#include <QTest>
#include <QThread>
#include <QTimer>
#include <QSignalSpy>
#include <QAtomicInt>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class ConcurrentWorker : public QThread {
  Q_OBJECT
public:
  ConcurrentWorker(EventBus *bus, int id) : bus_(bus), id_(id) {}
  
  void run() override {
    for (int i = 0; i < 100; ++i) {
      SlaveInfo info;
      info.position = id_ * 1000 + i;
      info.name = QString("Worker_%1_Slave_%2").arg(id_).arg(i);
      
      QVector<SlaveInfo> slaves{info};
      bus_->emitSlaveChanged(slaves);
      
      bus_->emitSdoValue(id_, QString("0x%1").arg(i, 4, 16, QChar('0')), "0x00", "0x0000");
    }
  }
  
private:
  EventBus *bus_;
  int id_;
};

class ConcurrentAccessTest : public QObject {
  Q_OBJECT
private slots:
  void testConcurrentSlaveChanged() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;
    
    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus, i);
      workers.append(worker);
      worker->start();
    }
    
    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }
    
    QCOMPARE(spy.count(), workerCount * 100);
  }

  void testConcurrentSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;
    
    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus, i);
      workers.append(worker);
      worker->start();
    }
    
    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }
    
    QCOMPARE(spy.count(), workerCount * 100);
  }

  void testConcurrentDifferentEvents() {
    EventBus bus;
    QSignalSpy slaveSpy(&bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus, &EventBus::sdoValueReceived);
    QSignalSpy connectionSpy(&bus, &EventBus::connectionStateChanged);
    
    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;
    
    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus, i);
      workers.append(worker);
      worker->start();
    }
    
    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }
    
    QCOMPARE(slaveSpy.count(), workerCount * 100);
    QCOMPARE(sdoSpy.count(), workerCount * 100);
  }

  void testMainThreadAccess() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    SlaveInfo info;
    info.position = 1;
    info.name = "MainThreadSlave";
    
    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).name, QString("MainThreadSlave"));
  }
};

QTEST_MAIN(ConcurrentAccessTest)
#include "concurrent_access_test.moc"
