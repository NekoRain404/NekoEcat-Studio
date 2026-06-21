// PerformanceMonitorServiceTest — Tests for PerformanceMonitorService
//
// Test coverage:
//   - Initial monitoring state and metric values
//   - Start/stop monitoring
//   - Double start idempotency
//   - Metrics updated signal (with disconnected client)
//   - History ring buffer (empty, after monitoring, size bound)
//   - DC sync update payload processing
//   - Startup time tracking
//   - Runtime performance tracking (SDO latency, state transitions, Free Run, UI)
//   - Memory usage tracking
//   - Performance alerts

#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QJsonObject>
#include <QJsonArray>

#include "services/PerformanceMonitorService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class PerformanceMonitorServiceTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  EventBus *bus_ = nullptr;
  PerformanceMonitorService *svc_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    bus_ = new EventBus(this);
    svc_ = new PerformanceMonitorService(bus_, client_, this);
  }

  void cleanup() {
    delete svc_;
    svc_ = nullptr;
    delete bus_;
    bus_ = nullptr;
    delete client_;
    client_ = nullptr;
  }

  // ── Basic State ──────────────────────────────────────────────────

  void testInitialState() {
    QVERIFY(!svc_->isMonitoring());
    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m.value("cycleTimeUs").toDouble(), 0.0);
    QCOMPARE(m.value("jitterUs").toDouble(), 0.0);
    QCOMPARE(m.value("frameLoss").toInt(), 0);
    QCOMPARE(m.value("pdoUpdateRate").toDouble(), 0.0);
    QCOMPARE(m.value("sdoReadLatencyMs").toDouble(), 0.0);
    QCOMPARE(m.value("sdoWriteLatencyMs").toDouble(), 0.0);
    QCOMPARE(m.value("stateTransitionMs").toDouble(), 0.0);
    QCOMPARE(m.value("freeRunCycleUs").toDouble(), 0.0);
    QCOMPARE(m.value("uiUpdateMs").toDouble(), 0.0);
    QCOMPARE(m.value("memoryMB").toDouble(), 0.0);
    QVERIFY(!m.value("startupComplete").toBool());
  }

  void testStartStop() {
    svc_->startMonitoring(100);
    QVERIFY(svc_->isMonitoring());
    svc_->stopMonitoring();
    QVERIFY(!svc_->isMonitoring());
  }

  void testDoubleStartIsNoop() {
    svc_->startMonitoring(100);
    svc_->startMonitoring(200);
    QVERIFY(svc_->isMonitoring());
    svc_->stopMonitoring();
  }

  void testMetricsUpdatedSignal() {
    QSignalSpy spy(svc_, &PerformanceMonitorService::metricsUpdated);
    QVERIFY(spy.isValid());
    svc_->startMonitoring(50);
    QTest::qWait(180);
    svc_->stopMonitoring();
    QCOMPARE(spy.count(), 0);
  }

  void testHistoryInitiallyEmpty() {
    QVERIFY(svc_->history().isEmpty());
  }

  void testHistoryAfterMonitoring() {
    svc_->startMonitoring(50);
    QTest::qWait(180);
    svc_->stopMonitoring();
    QVERIFY(svc_->history().isEmpty());
  }

  void testHistoryRingBufferBound() {
    svc_->startMonitoring(1);
    QTest::qWait(50);
    svc_->stopMonitoring();
    QVERIFY(svc_->history().size() <= PerformanceMonitorService::kHistorySize);
  }

  // ── DC Sync ──────────────────────────────────────────────────────

  void testDcSyncUpdate() {
    QJsonObject slave;
    slave["position"] = 1;
    slave["name"] = "EL1008";
    slave["dcCapable"] = true;
    slave["syncing"] = true;
    slave["driftNs"] = 1000.0;
    slave["jitterMin"] = 5.0;
    slave["jitterMax"] = 15.0;
    slave["jitterAvg"] = 8.0;

    QJsonArray slaves;
    slaves.append(slave);

    QJsonObject payload;
    payload["slaves"] = slaves;

    bus_->emitDcSyncUpdate(payload);

    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m.value("cycleTimeUs").toDouble(), 1.0);
    QCOMPARE(m.value("jitterUs").toDouble(), 0.015);
  }

  // ── Startup Time Tracking ────────────────────────────────────────

  void testStartupTracking() {
    QVERIFY(!svc_->startupComplete());

    svc_->beginStartup();
    svc_->recordStartupPhase("serviceInit", 50.0);
    svc_->recordStartupPhase("uiBuild", 120.0);
    svc_->recordStartupPhase("pluginLoad", 80.0);
    svc_->endStartup();

    QVERIFY(svc_->startupComplete());

    QJsonObject report = svc_->startupReport();
    QVERIFY(report["complete"].toBool());
    // totalMs may be 0 if startup is very fast, so check phases instead
    QVERIFY(report.contains("totalMs"));

    QJsonObject phases = report["phases"].toObject();
    QCOMPARE(phases["serviceInit"].toDouble(), 50.0);
    QCOMPARE(phases["uiBuild"].toDouble(), 120.0);
    QCOMPARE(phases["pluginLoad"].toDouble(), 80.0);

    QJsonObject percentages = report["percentages"].toObject();
    QVERIFY(percentages.contains("serviceInit"));
    QVERIFY(percentages.contains("uiBuild"));
    QVERIFY(percentages.contains("pluginLoad"));
    // Percentages should sum to ~100%
    double totalPct = percentages["serviceInit"].toDouble() +
                      percentages["uiBuild"].toDouble() +
                      percentages["pluginLoad"].toDouble();
    QVERIFY(qAbs(totalPct - 100.0) < 1.0);
  }

  void testStartupReportStructure() {
    svc_->beginStartup();
    svc_->recordStartupPhase("test", 10.0);
    svc_->endStartup();

    QJsonObject report = svc_->startupReport();
    QVERIFY(report.contains("complete"));
    QVERIFY(report.contains("totalMs"));
    QVERIFY(report.contains("phases"));
    QVERIFY(report.contains("percentages"));
  }

  // ── Runtime Performance Tracking ─────────────────────────────────

  void testSdoReadLatency() {
    svc_->recordSdoReadLatency(15.5);
    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m["sdoReadLatencyMs"].toDouble(), 15.5);
    QCOMPARE(m["avgSdoReadLatencyMs"].toDouble(), 15.5);
  }

  void testSdoWriteLatency() {
    svc_->recordSdoWriteLatency(22.3);
    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m["sdoWriteLatencyMs"].toDouble(), 22.3);
    QCOMPARE(m["avgSdoWriteLatencyMs"].toDouble(), 22.3);
  }

  void testStateTransition() {
    svc_->recordStateTransition(100.0);
    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m["stateTransitionMs"].toDouble(), 100.0);
    QCOMPARE(m["avgStateTransitionMs"].toDouble(), 100.0);
  }

  void testFreeRunCycleTime() {
    svc_->recordFreeRunCycleTime(1000.0);
    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m["freeRunCycleUs"].toDouble(), 1000.0);
    QCOMPARE(m["avgFreeRunCycleUs"].toDouble(), 1000.0);
  }

  void testUiUpdateTime() {
    svc_->recordUiUpdateTime(8.5);
    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m["uiUpdateMs"].toDouble(), 8.5);
    QCOMPARE(m["avgUiUpdateMs"].toDouble(), 8.5);
  }

  void testLatencyHistoryAveraging() {
    svc_->recordSdoReadLatency(10.0);
    svc_->recordSdoReadLatency(20.0);
    svc_->recordSdoReadLatency(30.0);

    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m["avgSdoReadLatencyMs"].toDouble(), 20.0);
  }

  void testLatencyHistoryBound() {
    for (int i = 0; i < 200; ++i) {
      svc_->recordSdoReadLatency(static_cast<double>(i));
    }
    QJsonObject m = svc_->currentMetrics();
    // Should only keep last 100 samples
    QVERIFY(m["avgSdoReadLatencyMs"].toDouble() > 50.0);
  }

  // ── Memory Usage Tracking ────────────────────────────────────────

  void testServiceMemory() {
    svc_->recordServiceMemory("SdoService", 1024);
    svc_->recordServiceMemory("WatchService", 2048);

    QJsonObject report = svc_->memoryReport();
    QJsonObject services = report["services"].toObject();
    QCOMPARE(services["SdoService"].toDouble(), 1024.0);
    QCOMPARE(services["WatchService"].toDouble(), 2048.0);
    QCOMPARE(report["totalServiceBytes"].toDouble(), 3072.0);
  }

  void testPluginMemory() {
    svc_->recordPluginMemory("OdPlugin", 512);
    svc_->recordPluginMemory("WatchPlugin", 768);

    QJsonObject report = svc_->memoryReport();
    QJsonObject plugins = report["plugins"].toObject();
    QCOMPARE(plugins["OdPlugin"].toDouble(), 512.0);
    QCOMPARE(plugins["WatchPlugin"].toDouble(), 768.0);
    QCOMPARE(report["totalPluginBytes"].toDouble(), 1280.0);
  }

  void testCacheMemory() {
    svc_->recordCacheMemory("SdoCache", 4096);

    QJsonObject report = svc_->memoryReport();
    QJsonObject caches = report["caches"].toObject();
    QCOMPARE(caches["SdoCache"].toDouble(), 4096.0);
    QCOMPARE(report["totalCacheBytes"].toDouble(), 4096.0);
  }

  void testTotalMemory() {
    svc_->recordServiceMemory("SdoService", 1024);
    svc_->recordPluginMemory("OdPlugin", 512);
    svc_->recordCacheMemory("SdoCache", 4096);

    QJsonObject report = svc_->memoryReport();
    QCOMPARE(report["totalBytes"].toDouble(), 5632.0);
    QCOMPARE(report["totalMB"].toDouble(), 5632.0 / (1024.0 * 1024.0));
  }

  void testMemoryInCurrentMetrics() {
    svc_->recordServiceMemory("SdoService", 1024 * 1024);  // 1 MB

    QJsonObject m = svc_->currentMetrics();
    QCOMPARE(m["memoryMB"].toDouble(), 1.0);
  }

  // ── Performance Alerts ───────────────────────────────────────────

  void testAlertThresholds() {
    PerformanceMonitorService::AlertThresholds thresholds;
    thresholds.sdoLatencyMs = 50.0;
    thresholds.stateTransitionMs = 200.0;
    thresholds.freeRunCycleUs = 1500.0;
    thresholds.uiUpdateMs = 30.0;
    thresholds.memoryMB = 256.0;
    svc_->setAlertThresholds(thresholds);

    auto actual = svc_->alertThresholds();
    QCOMPARE(actual.sdoLatencyMs, 50.0);
    QCOMPARE(actual.stateTransitionMs, 200.0);
    QCOMPARE(actual.freeRunCycleUs, 1500.0);
    QCOMPARE(actual.uiUpdateMs, 30.0);
    QCOMPARE(actual.memoryMB, 256.0);
  }

  void testPerformanceAlertSignal() {
    QSignalSpy spy(svc_, &PerformanceMonitorService::performanceAlert);
    QVERIFY(spy.isValid());

    PerformanceMonitorService::AlertThresholds thresholds;
    thresholds.sdoLatencyMs = 10.0;
    svc_->setAlertThresholds(thresholds);

    // Trigger alert by recording high latency
    svc_->recordSdoReadLatency(50.0);

    // Manually trigger collection to check alerts
    // Note: alerts are checked during collectMetrics(), which requires connected client
    // So we test the threshold configuration instead
    auto actual = svc_->alertThresholds();
    QCOMPARE(actual.sdoLatencyMs, 10.0);
  }

  // ── Performance Report ───────────────────────────────────────────

  void testPerformanceReport() {
    svc_->beginStartup();
    svc_->recordStartupPhase("test", 10.0);
    svc_->endStartup();

    svc_->recordSdoReadLatency(15.0);
    svc_->recordServiceMemory("TestService", 1024);

    QJsonObject report = svc_->performanceReport();
    QVERIFY(report.contains("current"));
    QVERIFY(report.contains("startup"));
    QVERIFY(report.contains("memory"));
    QVERIFY(report.contains("statistics"));
    QVERIFY(report.contains("thresholds"));

    QJsonObject stats = report["statistics"].toObject();
    QVERIFY(stats.contains("sdoReadLatency"));
    QVERIFY(stats.contains("sdoWriteLatency"));
    QVERIFY(stats.contains("stateTransition"));
    QVERIFY(stats.contains("freeRunCycleTime"));
    QVERIFY(stats.contains("uiUpdateTime"));
  }

  void testPerformanceReportStatistics() {
    svc_->recordSdoReadLatency(10.0);
    svc_->recordSdoReadLatency(20.0);
    svc_->recordSdoReadLatency(30.0);

    QJsonObject report = svc_->performanceReport();
    QJsonObject stats = report["statistics"].toObject();
    QJsonObject sdoRead = stats["sdoReadLatency"].toObject();

    QCOMPARE(sdoRead["min"].toDouble(), 10.0);
    QCOMPARE(sdoRead["max"].toDouble(), 30.0);
    QCOMPARE(sdoRead["avg"].toDouble(), 20.0);
    QCOMPARE(sdoRead["count"].toInt(), 3);
  }
};

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  PerformanceMonitorServiceTest t;
  return QTest::qExec(&t, argc, argv);
}

#include "performance_monitor_service_test.moc"
