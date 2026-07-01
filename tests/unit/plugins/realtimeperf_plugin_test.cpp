// realtimeperf_plugin_test — unit tests for RealtimePerformancePlugin,
// LatencyMonitorWidget, ThroughputMonitorWidget, and RealtimePerformanceService.

#include "plugins/realtimeperf/RealtimePerformancePlugin.h"
#include "plugins/realtimeperf/LatencyMonitorWidget.h"
#include "plugins/realtimeperf/ThroughputMonitorWidget.h"
#include "services/RealtimePerformanceService.h"

#include <QApplication>
#include <QFile>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTest>
#include <QTemporaryDir>
#include "infra/EcatClient.h"

class RealtimePerformancePluginTest : public QObject {
  Q_OBJECT
private slots:
  void testPluginIdentity() {
    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("realtimeperf"));
    QCOMPARE(plugin.displayName(), QString("Real-time Performance"));
    QCOMPARE(plugin.displayNameZh(), QString("实时性能"));
  }

  void testDefaultOrder() {
    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 32);
  }

  void testVisible() {
    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);
    QVERIFY(!plugin.visible());
  }

  void testWidgetNotNull() {
    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  void testServiceAccessor() {
    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  void testLatencyMonitorAccessor() {
    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);
    QVERIFY(plugin.latencyMonitor() != nullptr);
  }

  void testThroughputMonitorAccessor() {
    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);
    QVERIFY(plugin.throughputMonitor() != nullptr);
  }

  void testServiceDefaultState() {
    RealtimePerformanceService svc(nullptr);
    QVERIFY(!svc.isMonitoring());
    QCOMPARE(svc.latency().sampleCount, 0);
    QCOMPARE(svc.latency().avgUs, 0.0);
    QCOMPARE(svc.throughput().totalFrames, quint64(0));
    QCOMPARE(svc.quality().score, 100.0);
  }

  void testServiceStartStop() {
    RealtimePerformanceService svc(nullptr);
    QSignalSpy startedSpy(&svc, &RealtimePerformanceService::monitoringStateChanged);
    svc.startMonitoring(100);
    QVERIFY(!svc.isMonitoring());
    QCOMPARE(startedSpy.count(), 0);

    svc.stopMonitoring();
    QVERIFY(!svc.isMonitoring());
    QCOMPARE(startedSpy.count(), 0);
  }

  void testServiceLatencyThreshold() {
    RealtimePerformanceService svc(nullptr);
    QCOMPARE(svc.latencyThreshold(), 1000.0);
    svc.setLatencyThreshold(500.0);
    QCOMPARE(svc.latencyThreshold(), 500.0);
  }

  void testServiceHistoryWindowSize() {
    RealtimePerformanceService svc(nullptr);
    QCOMPARE(svc.historyWindowSize(), 200);
    svc.setHistoryWindowSize(500);
    QCOMPARE(svc.historyWindowSize(), 500);
  }

  void testLatencyMetricsSignals() {
    RealtimePerformanceService svc(nullptr);
    QSignalSpy spy(&svc, &RealtimePerformanceService::latencyUpdated);
    svc.startMonitoring(50);
    QTest::qWait(200);
    svc.stopMonitoring();
    QCOMPARE(spy.count(), 0);
    QCOMPARE(svc.latency().sampleCount, 0);
    QCOMPARE(svc.latency().avgUs, 0.0);
  }

  void testThroughputMetricsSignals() {
    RealtimePerformanceService svc(nullptr);
    QSignalSpy spy(&svc, &RealtimePerformanceService::throughputUpdated);
    svc.startMonitoring(50);
    QTest::qWait(200);
    svc.stopMonitoring();
    QCOMPARE(spy.count(), 0);
    QCOMPARE(svc.throughput().totalFrames, quint64(0));
  }

  void testConnectedDaemonWithoutTelemetryDoesNotCreateSamples() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    EcatClient client;
    client.enableAutoReconnect(false);
    RealtimePerformanceService svc(&client);
    QSignalSpy latencySpy(&svc, &RealtimePerformanceService::latencyUpdated);
    QSignalSpy qualitySpy(&svc, &RealtimePerformanceService::qualityUpdated);

    client.connectToHost(QHostAddress::LocalHost, server.serverPort());
    QVERIFY(server.waitForNewConnection(1000));
    QTcpSocket *socket = server.nextPendingConnection();
    QVERIFY(socket != nullptr);
    socket->setParent(&server);
    QTRY_VERIFY(client.isConnected());

    svc.startMonitoring(10);
    QTest::qWait(80);
    svc.stopMonitoring();

    QCOMPARE(latencySpy.count(), 0);
    QCOMPARE(qualitySpy.count(), 0);
    QCOMPARE(svc.latency().sampleCount, 0);
    QCOMPARE(svc.quality().grade, QString());
  }

  void testLatencyMonitorAddSample() {
    LatencyMonitorWidget w;
    w.addSample(100.0);
    w.addSample(200.0);
    w.addSample(150.0);
    QVERIFY(w.width() >= 0);
  }

  void testLatencyMonitorThreshold() {
    LatencyMonitorWidget w;
    w.setThreshold(500.0);
    w.addSample(100.0);
  }

  void testLatencyMonitorClear() {
    LatencyMonitorWidget w;
    w.addSample(100.0);
    w.clear();
  }

  void testLatencyMonitorHistorySize() {
    LatencyMonitorWidget w;
    w.setHistorySize(50);
    for (int i = 0; i < 100; ++i)
      w.addSample(static_cast<double>(i));
  }

  void testLatencyMonitorUpdateMetrics() {
    LatencyMonitorWidget w;
    LatencyMetrics m;
    m.minUs = 50.0;
    m.maxUs = 200.0;
    m.avgUs = 100.0;
    m.stddevUs = 25.0;
    m.sampleCount = 10;
    w.updateMetrics(m);
  }

  void testThroughputMonitorUpdateMetrics() {
    ThroughputMonitorWidget w;
    ThroughputMetrics m;
    m.framesPerSecond = 1000.0;
    m.bytesPerSecond = 1518000.0;
    m.errorRate = 0.5;
    m.utilizationPercent = 45.0;
    m.totalFrames = 100000;
    m.totalBytes = 151800000;
    m.totalErrors = 50;
    w.updateMetrics(m);
  }

  void testDoubleStartNoOp() {
    RealtimePerformanceService svc(nullptr);
    svc.startMonitoring(100);
    svc.startMonitoring(100);
    QVERIFY(!svc.isMonitoring());
    svc.stopMonitoring();
  }

  void testDoubleStopNoOp() {
    RealtimePerformanceService svc(nullptr);
    svc.stopMonitoring();
    QVERIFY(!svc.isMonitoring());
  }

  void testExportReportReportsPersistenceOutcome() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    RealtimePerformanceService svc(nullptr);
    RealtimePerformancePlugin plugin(&svc);

    const QString path = dir.filePath("performance_report.csv");
    QVERIFY(plugin.exportReportToFile(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString csv = QString::fromUtf8(file.readAll());
    QVERIFY(csv.startsWith(QStringLiteral("Metric,Value\n")));
    QVERIFY(csv.contains(QStringLiteral("Quality Score")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!plugin.exportReportToFile(QString()));
    QVERIFY(!plugin.exportReportToFile(dir.path()));
  }
};

QTEST_MAIN(RealtimePerformancePluginTest)
#include "realtimeperf_plugin_test.moc"
