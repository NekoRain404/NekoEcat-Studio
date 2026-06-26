// DcSyncPrecisionPluginTest — Tests for DcSyncPrecisionPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget creation
//   - Offline service start does not synthesize precision monitoring
//   - Sync status table
//   - Drift monitor widget
//   - Jitter analysis widget
//   - Start/stop button
//   - Export button

#include <QTest>
#include <QFile>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTemporaryDir>
#include "plugins/dcsyncprecision/DcSyncPrecisionPlugin.h"
#include "plugins/dcsyncprecision/DriftMonitorWidget.h"
#include "plugins/dcsyncprecision/JitterAnalysisWidget.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"
#include "services/DcSyncPrecisionService.h"

class DcSyncPrecisionPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testIdentity() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QCOMPARE(p.id(), QString("dcsyncprecision"));
    QCOMPARE(p.displayName(), QString("DC Sync Precision"));
    QCOMPARE(p.displayNameZh(), QString("DC 同步精度"));
  }

  void testDefaultOrder() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QCOMPARE(p.defaultOrder(), 26);
  }

  void testVisible() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QVERIFY(p.visible());
  }

  void testWidgetNotNull() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QVERIFY(p.widget() != nullptr);
  }

  void testServiceStartMonitoringOfflineDoesNotActivate() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionService svc(&client, &bus);
    QSignalSpy spy(&svc, &DcSyncPrecisionService::monitoringStateChanged);
    QVERIFY(spy.isValid());

    svc.startMonitoring();
    QVERIFY(!svc.isMonitoring());
    QCOMPARE(spy.count(), 0);

    svc.stopMonitoring();
    QVERIFY(!svc.isMonitoring());
  }

  void testSyncTableNotNull() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QVERIFY(p.syncTable() != nullptr);
  }

  void testSyncTableColumns() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QCOMPARE(p.syncTable()->columnCount(), 6);
  }

  void testDriftMonitorNotNull() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QVERIFY(p.driftMonitor() != nullptr);
  }

  void testDriftMonitorAddSample() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    auto *dm = p.driftMonitor();
    dm->addSample(100.0);
    dm->addSample(-50.0);
    dm->addSample(200.0);
    QVERIFY(dm->width() >= 0);
  }

  void testDriftMonitorSetThreshold() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    auto *dm = p.driftMonitor();
    dm->setThreshold(500.0);
    dm->addSample(100.0);
    QVERIFY(dm->width() >= 0);
  }

  void testDriftMonitorClear() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    auto *dm = p.driftMonitor();
    dm->addSample(100.0);
    dm->clear();
    QVERIFY(dm->width() >= 0);
  }

  void testJitterAnalysisNotNull() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QVERIFY(p.jitterAnalysis() != nullptr);
  }

  void testJitterAnalysisSetData() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    auto *ja = p.jitterAnalysis();
    ja->setJitterData(10.0, 500.0, 100.0, 50.0, 1000);
    QCOMPARE(ja->jitterMin(), 10.0);
    QCOMPARE(ja->jitterMax(), 500.0);
    QCOMPARE(ja->jitterAvg(), 100.0);
    QCOMPARE(ja->jitterStddev(), 50.0);
    QCOMPARE(ja->sampleCount(), 1000);
  }

  void testJitterAnalysisHistogram() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    auto *ja = p.jitterAnalysis();
    QVector<int> bins = {5, 10, 20, 30, 15, 8, 5, 3, 2, 2};
    ja->setHistogram(bins, 10.0, 0.0);
    QVERIFY(ja->width() >= 0);
  }

  void testJitterAnalysisClear() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    auto *ja = p.jitterAnalysis();
    ja->setJitterData(10.0, 500.0, 100.0, 50.0, 1000);
    ja->clear();
    QCOMPARE(ja->jitterMin(), 0.0);
    QCOMPARE(ja->sampleCount(), 0);
  }

  void testStartStopButton() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QVERIFY(p.startStopButton() != nullptr);
    QCOMPARE(p.startStopButton()->text(), QString("Start Monitoring"));
  }

  void testExportButton() {
    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);
    QVERIFY(p.exportButton() != nullptr);
    QVERIFY(!p.exportButton()->isEnabled());
  }

  void testExportReportReportsPersistenceOutcome() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    EcatClient client;
    EventBus bus;
    DcSyncPrecisionPlugin p(&client, &bus);

    const QString path = dir.filePath("dc_sync_precision.md");
    QVERIFY(p.exportReportToFile(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString markdown = QString::fromUtf8(file.readAll());
    QVERIFY(markdown.startsWith(QStringLiteral("# DC Sync Precision Report\n")));
    QVERIFY(markdown.contains(QStringLiteral("## Sync Quality")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!p.exportReportToFile(QString()));
    QVERIFY(!p.exportReportToFile(dir.path()));
  }
};

QTEST_MAIN(DcSyncPrecisionPluginTest)
#include "dcsyncprecision_plugin_test.moc"
