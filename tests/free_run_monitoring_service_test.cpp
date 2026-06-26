// FreeRunMonitoringServiceTest — Tests for FreeRunMonitoringService
//
// Test coverage:
//   - Default state
//   - Offline monitoring start rejection (including double start/stop)
//   - Process data update
//   - Performance metrics update
//   - Error tracking
//   - Status update
//   - Signal emission

#include <QTest>
#include <QSignalSpy>
#include "services/FreeRunMonitoringService.h"

class FreeRunMonitoringServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testDefaultState() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    QVERIFY(!svc.isMonitoring());
    QCOMPARE(svc.processData().cycleCount, static_cast<quint64>(0));
    QCOMPARE(svc.performance().cycleTimeUs, 0.0);
    QCOMPARE(svc.errors().size(), 0);
    QCOMPARE(svc.status().state, FreeRunState::Idle);
  }

  void testStartStop() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    svc.startMonitoring();
    QVERIFY(!svc.isMonitoring());
    svc.stopMonitoring();
    QVERIFY(!svc.isMonitoring());
  }

  void testDoubleStart() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    svc.startMonitoring();
    svc.startMonitoring();
    QVERIFY(!svc.isMonitoring());
    svc.stopMonitoring();
  }

  void testDoubleStop() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    svc.stopMonitoring();
    QVERIFY(!svc.isMonitoring());
  }

  void testUpdateProcessData() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunMonitoringService::processDataUpdated);
    FreeRunProcessData pd;
    pd.inputs = {0x01, 0x02, 0x03};
    pd.outputs = {0x04, 0x05};
    pd.cycleCount = 100;
    pd.timestamp = 12345;
    svc.updateProcessData(pd);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.processData().inputs.size(), 3);
    QCOMPARE(svc.processData().cycleCount, static_cast<quint64>(100));
    QCOMPARE(svc.processData().timestamp, 12345);
  }

  void testUpdatePerformance() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunMonitoringService::performanceUpdated);
    FreeRunPerformanceMetrics m;
    m.cycleTimeUs = 1000.0;
    m.jitterUs = 5.0;
    m.cpuLoadPercent = 12.5;
    m.totalCycles = 5000;
    m.missedCycles = 2;
    svc.updatePerformance(m);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.performance().cycleTimeUs, 1000.0);
    QCOMPARE(svc.performance().jitterUs, 5.0);
    QCOMPARE(svc.performance().missedCycles, static_cast<quint64>(2));
  }

  void testAddError() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunMonitoringService::errorOccurred);
    FreeRunErrorInfo err;
    err.code = QStringLiteral("E001");
    err.message = QStringLiteral("Watchdog timeout");
    err.severity = QStringLiteral("Error");
    err.slavePosition = 3;
    svc.addError(err);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.errors().size(), 1);
    QCOMPARE(svc.errors().first().code, QStringLiteral("E001"));
    QCOMPARE(svc.errors().first().slavePosition, 3);
  }

  void testMultipleErrors() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    FreeRunErrorInfo e1;
    e1.code = QStringLiteral("E001");
    svc.addError(e1);
    FreeRunErrorInfo e2;
    e2.code = QStringLiteral("E002");
    svc.addError(e2);
    QCOMPARE(svc.errors().size(), 2);
  }

  void testUpdateStatus() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunMonitoringService::statusChanged);
    FreeRunStatus st;
    st.state = FreeRunState::Running;
    st.stateString = QStringLiteral("Running");
    st.totalCycles = 1000;
    st.activeSlaves = 4;
    st.errorSlaves = 0;
    svc.updateStatus(st);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.status().state, FreeRunState::Running);
    QCOMPARE(svc.status().totalCycles, static_cast<quint64>(1000));
    QCOMPARE(svc.status().activeSlaves, 4);
  }

  void testMonitoringStateSignal() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunMonitoringService::monitoringStateChanged);
    svc.startMonitoring();
    svc.stopMonitoring();
    QCOMPARE(spy.count(), 0);
  }

  void testStopUpdatesStatus() {
    FreeRunMonitoringService svc(nullptr, nullptr);
    svc.startMonitoring();
    svc.stopMonitoring();
    QCOMPARE(svc.status().state, FreeRunState::Idle);
    QCOMPARE(svc.status().stateString, QStringLiteral("Idle"));
  }
};

QTEST_MAIN(FreeRunMonitoringServiceTest)
#include "free_run_monitoring_service_test.moc"
