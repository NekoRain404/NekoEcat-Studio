#include <QTest>
#include <QVector>
#include <QSignalSpy>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class LargeDataBoundaryTest : public QObject {
  Q_OBJECT
private slots:
  void testLargeSlaveVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    QVector<SlaveInfo> slaves;
    for (int i = 0; i < 1000; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("Slave_%1").arg(i);
      info.state = "OP";
      slaves.append(info);
    }
    
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1000);
    QCOMPARE(received.at(999).name, QString("Slave_999"));
  }

  void testLargeTopologyVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    
    QVector<SlaveInfo> topology;
    for (int i = 0; i < 500; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("TopologySlave_%1").arg(i);
      topology.append(info);
    }
    
    bus.emitTopologyChanged(topology);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 500);
  }

  void testLargeSignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QVector<double> values;
    QVector<qint64> timestamps;
    for (int i = 0; i < 10000; ++i) {
      values.append(static_cast<double>(i) * 0.1);
      timestamps.append(i * 1000);
    }
    
    bus.emitSignalData(0, values, timestamps);
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  void testLargeJsonObject() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    
    QJsonObject largeObj;
    for (int i = 0; i < 100; ++i) {
      largeObj[QString("key_%1").arg(i)] = QJsonValue(QString("value_%1").arg(i));
    }
    
    bus.emitDcSyncUpdate(largeObj);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QCOMPARE(received.size(), 100);
  }

  void testMultipleEmissions() {
    EventBus bus;
    QSignalSpy slaveSpy(&bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus, &EventBus::sdoValueReceived);
    
    for (int i = 0; i < 100; ++i) {
      QVector<SlaveInfo> slaves;
      SlaveInfo info;
      info.position = i;
      slaves.append(info);
      bus.emitSlaveChanged(slaves);
      
      bus.emitSdoValue(i, QString("0x%1").arg(i, 4, 16, QChar('0')), "0x00", "0x0000");
    }
    
    QCOMPARE(slaveSpy.count(), 100);
    QCOMPARE(sdoSpy.count(), 100);
  }
};

QTEST_MAIN(LargeDataBoundaryTest)
#include "large_data_boundary_test.moc"
