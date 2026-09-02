/// @brief OnlineDiagnosticsPlugin unit tests.
///
/// @details Tests the Online Diagnostics workspace plugin's service, identity,
/// UI construction, widget hierarchy, and accessor methods.
///
/// @par Test Coverage
///   - Service default state (zero counters, monitoring off)
///   - Service offline start does not synthesize active monitoring
///   - Plugin identity (id, displayName, displayNameZh, defaultOrder, visible)
///   - Widget construction and non-null checks
///   - BusMonitorWidget accessor
///   - ErrorAnalyzerWidget accessor
///   - Service accessor
///   - BusTraffic structure defaults
///   - ErrorRate structure defaults
///   - PerformanceMetrics structure defaults
///   - HealthStatus structure defaults

#include "infra/EcatClient.h"
#include "plugins/onlinediagnostics/BusMonitorWidget.h"
#include "plugins/onlinediagnostics/ErrorAnalyzerWidget.h"
#include "plugins/onlinediagnostics/OnlineDiagnosticsPlugin.h"
#include "services/OnlineDiagnosticsService.h"
#include <QFile>
#include <QRegularExpression>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTest>

class OnlineDiagnosticsPluginTest : public QObject {
    Q_OBJECT
private slots:
    void testServiceDefaultState() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        QVERIFY(!svc.isMonitoring());

        auto traffic = svc.busTraffic();
        QCOMPARE(traffic.txFrames, quint64(0));
        QCOMPARE(traffic.rxFrames, quint64(0));
        QCOMPARE(traffic.txBytes, quint64(0));
        QCOMPARE(traffic.rxBytes, quint64(0));
        QCOMPARE(traffic.bandwidth, 0.0);
        QCOMPARE(traffic.utilization, 0.0);

        auto err = svc.errorRate();
        QCOMPARE(err.totalErrors, quint64(0));
        QCOMPARE(err.crcErrors, quint64(0));
        QCOMPARE(err.lostErrors, quint64(0));
        QCOMPARE(err.rate, 0.0);

        auto perf = svc.performance();
        QCOMPARE(perf.cycleTimeUs, 0.0);
        QCOMPARE(perf.jitterUs, 0.0);
        QCOMPARE(perf.pdoUpdateRate, 0.0);

        auto health = svc.health();
        QCOMPARE(health.score, 0);
        QCOMPARE(health.grade, QStringLiteral("Unknown"));
        QCOMPARE(health.totalSlaves, 0);
        QCOMPARE(health.opSlaves, 0);
    }

    void testServiceStartMonitoringOfflineDoesNotActivate() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        svc.startMonitoring(100);
        QVERIFY(!svc.isMonitoring());
        svc.stopMonitoring();
        QVERIFY(!svc.isMonitoring());
    }

    void testPluginIdentity() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);
        QCOMPARE(plugin.id(), QString("onlinediagnostics"));
        QCOMPARE(plugin.displayName(), QString("Online Diagnostics"));
        QCOMPARE(plugin.displayNameZh(), QStringLiteral("在线诊断"));
    }

    void testPluginDefaultOrder() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);
        QCOMPARE(plugin.defaultOrder(), 28);
    }

    void testPluginVisible() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);
        QVERIFY(plugin.visible());
    }

    void testPluginWidgetNotNull() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);
        QVERIFY(plugin.widget() != nullptr);
    }

    void testPluginServiceAccessor() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);
        QCOMPARE(plugin.service(), &svc);
    }

    void testBusMonitorNotNull() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);
        QVERIFY(plugin.busMonitor() != nullptr);
    }

    void testErrorAnalyzerNotNull() {
        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);
        QVERIFY(plugin.errorAnalyzer() != nullptr);
    }

    void testBusTrafficDefaults() {
        BusTraffic t;
        QCOMPARE(t.txFrames, quint64(0));
        QCOMPARE(t.rxFrames, quint64(0));
        QCOMPARE(t.txBytes, quint64(0));
        QCOMPARE(t.rxBytes, quint64(0));
        QCOMPARE(t.bandwidth, 0.0);
        QCOMPARE(t.utilization, 0.0);
    }

    void testErrorRateDefaults() {
        ErrorRate r;
        QCOMPARE(r.totalErrors, quint64(0));
        QCOMPARE(r.crcErrors, quint64(0));
        QCOMPARE(r.lostErrors, quint64(0));
        QCOMPARE(r.rate, 0.0);
    }

    void testPerformanceMetricsDefaults() {
        PerformanceMetrics m;
        QCOMPARE(m.cycleTimeUs, 0.0);
        QCOMPARE(m.jitterUs, 0.0);
        QCOMPARE(m.frameLossRate, 0.0);
        QCOMPARE(m.sdoResponseMs, 0.0);
        QCOMPARE(m.pdoUpdateRate, 0.0);
    }

    void testHealthStatusDefaults() {
        HealthStatus h;
        QCOMPARE(h.score, 0);
        QCOMPARE(h.grade, QStringLiteral("Unknown"));
        QCOMPARE(h.totalSlaves, 0);
        QCOMPARE(h.opSlaves, 0);
        QCOMPARE(h.errorSlaves, 0);
        QVERIFY(!h.watchdogOk);
    }

    void testServiceSourceDoesNotSynthesizeHealthyWithoutEvidence() {
        QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/OnlineDiagnosticsService.cpp"));
        QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString text = QString::fromUtf8(source.readAll());
        QVERIFY2(!text.contains(QStringLiteral("QStringLiteral(\"Healthy\")")),
                 "Online diagnostics must not synthesize Healthy grade without "
                 "sampled health evidence.");
    }

    void testExportReportReportsPersistenceOutcome() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        EcatClient client;
        OnlineDiagnosticsService svc(&client);
        OnlineDiagnosticsPlugin plugin(&svc);

        const QString path = dir.filePath("online_diagnostics.csv");
        QVERIFY(plugin.exportReportToFile(path));
        QVERIFY(QFile::exists(path));

        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString csv = QString::fromUtf8(file.readAll());
        QVERIFY(csv.startsWith(QStringLiteral("Metric,Value\n")));
        QVERIFY(csv.contains(QStringLiteral("Health Score")));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.exportReportToFile(QString()));
        QVERIFY(!plugin.exportReportToFile(dir.path()));
    }
};

QTEST_MAIN(OnlineDiagnosticsPluginTest)
#include "onlinediagnostics_plugin_test.moc"
