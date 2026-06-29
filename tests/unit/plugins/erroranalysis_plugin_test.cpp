// AdvancedErrorAnalysisPluginTest — Tests for AdvancedErrorAnalysisPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget, table, timeline, and correlation widget creation
//   - Filter and severity filter controls
//   - Analysis service integration
//   - Summary label updates

#include <QTest>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTemporaryDir>
#include "plugins/erroranalysis/AdvancedErrorAnalysisPlugin.h"
#include "plugins/erroranalysis/ErrorTimelineWidget.h"
#include "plugins/erroranalysis/ErrorCorrelationWidget.h"
#include "services/AdvancedErrorAnalysisService.h"

class AdvancedErrorAnalysisPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testIdentity() {
    AdvancedErrorAnalysisPlugin p;
    QCOMPARE(p.id(), QString("erroranalysis"));
    QCOMPARE(p.displayName(), QString("Error Analysis"));
    QCOMPARE(p.displayNameZh(), QString("错误分析"));
  }

  void testDefaultOrder() {
    AdvancedErrorAnalysisPlugin p;
    QCOMPARE(p.defaultOrder(), 34);
  }

  void testVisible() {
    AdvancedErrorAnalysisPlugin p;
    QVERIFY(p.visible());
  }

  void testWidgetNotNull() {
    AdvancedErrorAnalysisPlugin p;
    QVERIFY(p.widget() != nullptr);
  }

  void testErrorTableNotNull() {
    AdvancedErrorAnalysisPlugin p;
    QVERIFY(p.errorTable() != nullptr);
  }

  void testTimelineWidgetNotNull() {
    AdvancedErrorAnalysisPlugin p;
    QVERIFY(p.timelineWidget() != nullptr);
  }

  void testCorrelationWidgetNotNull() {
    AdvancedErrorAnalysisPlugin p;
    QVERIFY(p.correlationWidget() != nullptr);
  }

  void testSummaryLabelNotNull() {
    AdvancedErrorAnalysisPlugin p;
    QVERIFY(p.summaryLabel() != nullptr);
  }

  void testPopulateTestData() {
    AdvancedErrorAnalysisPlugin p;
    QVERIFY(p.errorTable()->rowCount() > 0);
    QVERIFY(p.timelineWidget()->eventCount() > 0);
  }

  void testServicePatternDetection() {
    AdvancedErrorAnalysisService svc;
    QVector<AdvancedErrorInfo> errors;
    for (int i = 0; i < 5; ++i) {
      AdvancedErrorInfo e;
      e.id = i + 1;
      e.timestamp = QDateTime::currentDateTime().addSecs(i * 60);
      e.category = "Communication";
      e.severity = "Error";
      e.message = QStringLiteral("Error %1").arg(i);
      errors.append(e);
    }
    auto patterns = svc.detectPatterns(errors);
    QVERIFY(!patterns.isEmpty());
    QCOMPARE(patterns.first().frequency, 5);
  }

  void testServiceCorrelation() {
    AdvancedErrorAnalysisService svc;
    QVector<AdvancedErrorInfo> errors;
    for (int i = 0; i < 6; ++i) {
      AdvancedErrorInfo e;
      e.id = i + 1;
      e.timestamp = QDateTime::currentDateTime().addSecs(i * 30);
      e.category = (i % 2 == 0) ? "Communication" : "Device";
      e.severity = "Error";
      e.message = QStringLiteral("Error %1").arg(i);
      errors.append(e);
    }
    auto matrix = svc.analyzeCorrelation(errors);
    QVERIFY(matrix.totalErrors == 6);
  }

  void testServiceRootCause() {
    AdvancedErrorAnalysisService svc;
    AdvancedErrorInfo e;
    e.id = 1;
    e.timestamp = QDateTime::currentDateTime();
    e.category = "Communication";
    e.severity = "Error";
    e.message = "Lost frame";
    auto rca = svc.analyzeRootCause(e);
    QVERIFY(!rca.rootCause.isEmpty());
    QVERIFY(rca.confidence > 0.0);
    QVERIFY(!rca.recommendedActions.isEmpty());
  }

  void testExportReportReportsPersistenceOutcome() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    AdvancedErrorAnalysisPlugin p;
    p.runAnalysis();

    const QString path = dir.filePath("error_analysis.md");
    QVERIFY(p.exportReportToFile(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString markdown = QString::fromUtf8(file.readAll());
    QVERIFY(markdown.startsWith(QStringLiteral("# Error Analysis Report\n")));
    QVERIFY(markdown.contains(QStringLiteral("Lost frame on port 0")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!p.exportReportToFile(QString()));
    QVERIFY(!p.exportReportToFile(dir.path()));
  }
};

QTEST_MAIN(AdvancedErrorAnalysisPluginTest)
#include "erroranalysis_plugin_test.moc"
