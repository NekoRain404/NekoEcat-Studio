// ComplianceCheckerPluginTest — Tests for ComplianceCheckerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Check, violation, and recommendation tables
//   - Compliance score calculation
//   - Add checks
//   - Run compliance checks

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QLabel>
#include "plugins/compliancechecker/ComplianceCheckerPlugin.h"

class ComplianceCheckerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, and default order
  void testPluginIdentity() {
    ComplianceCheckerPlugin plugin;

    QCOMPARE(plugin.id(), QString("compliancechecker"));
    QCOMPARE(plugin.displayName(), QString("Compliance Checker"));
    QCOMPARE(plugin.displayNameZh(), QString("合规检查器"));
    QCOMPARE(plugin.defaultOrder(), 285);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created
  void testWidgetCreation() {
    ComplianceCheckerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial check, violation, and recommendation counts
  void testInitialState() {
    ComplianceCheckerPlugin plugin;

    QCOMPARE(plugin.checkCount(), 6);
    QCOMPARE(plugin.violationCount(), 2);
    QCOMPARE(plugin.recommendationCount(), 3);
  }

  // Verify check table structure
  void testCheckTable() {
    ComplianceCheckerPlugin plugin;

    QTableWidget *table = plugin.checkTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 6);
    QCOMPARE(table->columnCount(), 5);
  }

  // Verify violation table structure
  void testViolationTable() {
    ComplianceCheckerPlugin plugin;

    QTableWidget *table = plugin.violationTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 2);
    QCOMPARE(table->columnCount(), 5);
  }

  // Verify recommendation table structure
  void testRecommendationTable() {
    ComplianceCheckerPlugin plugin;

    QTableWidget *table = plugin.recommendationTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify compliance score is within valid range
  void testComplianceScore() {
    ComplianceCheckerPlugin plugin;

    double score = plugin.complianceScore();
    QVERIFY(score > 0.0);
    QVERIFY(score <= 100.0);
  }

  // Verify adding a check increments count
  void testAddCheck() {
    ComplianceCheckerPlugin plugin;
    int initial = plugin.checkCount();

    ComplianceCheckerPlugin::ComplianceCheck c;
    c.id = "c_new";
    c.name = "New Check";
    c.category = "Safety";
    c.description = "Test check";
    c.passed = true;
    c.checkedAt = QDateTime::currentDateTime();

    plugin.addCheck(c);
    QCOMPARE(plugin.checkCount(), initial + 1);
  }

  // Verify removing a check decrements count
  void testRemoveCheck() {
    ComplianceCheckerPlugin plugin;
    int initial = plugin.checkCount();

    plugin.removeCheck(0);
    QCOMPARE(plugin.checkCount(), initial - 1);
  }

  // Verify run check emits completion signal
  void testRunCheck() {
    ComplianceCheckerPlugin plugin;
    QSignalSpy spy(&plugin, &ComplianceCheckerPlugin::checkCompleted);

    plugin.runCheck(0);
    QCOMPARE(spy.count(), 1);
  }

  // Verify adding a violation with signal
  void testAddViolation() {
    ComplianceCheckerPlugin plugin;
    QSignalSpy spy(&plugin, &ComplianceCheckerPlugin::violationDetected);
    int initial = plugin.violationCount();

    ComplianceCheckerPlugin::Violation v;
    v.id = "v_new";
    v.checkId = "c1";
    v.severity = "warning";
    v.description = "Test violation";
    v.recommendation = "Fix it";
    v.detectedAt = QDateTime::currentDateTime();

    plugin.addViolation(v);
    QCOMPARE(plugin.violationCount(), initial + 1);
    QCOMPARE(spy.count(), 1);
  }

  // Verify removing a violation decrements count
  void testRemoveViolation() {
    ComplianceCheckerPlugin plugin;
    int initial = plugin.violationCount();

    plugin.removeViolation(0);
    QCOMPARE(plugin.violationCount(), initial - 1);
  }

  // Verify adding a recommendation increments count
  void testAddRecommendation() {
    ComplianceCheckerPlugin plugin;
    int initial = plugin.recommendationCount();

    ComplianceCheckerPlugin::Recommendation r;
    r.id = "r_new";
    r.priority = "high";
    r.title = "New Recommendation";
    r.description = "Do this";
    r.category = "Safety";

    plugin.addRecommendation(r);
    QCOMPARE(plugin.recommendationCount(), initial + 1);
  }

  // Verify removing a recommendation decrements count
  void testRemoveRecommendation() {
    ComplianceCheckerPlugin plugin;
    int initial = plugin.recommendationCount();

    plugin.removeRecommendation(0);
    QCOMPARE(plugin.recommendationCount(), initial - 1);
  }

  // Verify generate report returns correct counts
  void testGenerateReport() {
    ComplianceCheckerPlugin plugin;

    auto report = plugin.generateReport();
    QCOMPARE(report.totalChecks, 6);
    QVERIFY(report.score > 0.0);
    QCOMPARE(report.violations.size(), 2);
  }

  // Verify score changed signal on violation add
  void testScoreChangedSignal() {
    ComplianceCheckerPlugin plugin;
    QSignalSpy spy(&plugin, &ComplianceCheckerPlugin::scoreChanged);

    plugin.addViolation({"v_test", "c1", "info", "test", "fix",
                         QDateTime::currentDateTime()});
    QCOMPARE(spy.count(), 1);
  }

  // Verify status label widget exists
  void testStatusLabel() {
    ComplianceCheckerPlugin plugin;

    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Verify score label displays compliance score
  void testScoreLabel() {
    ComplianceCheckerPlugin plugin;

    QLabel *label = plugin.scoreLabel();
    QVERIFY(label != nullptr);
    QVERIFY(label->text().contains("Compliance Score"));
  }

  // Verify export report creates file
  void testExportReport() {
    ComplianceCheckerPlugin plugin;

    QString path = QDir::temp().absoluteFilePath("compliance_report_test.txt");
    plugin.exportReport(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(ComplianceCheckerPluginTest)
#include "compliancechecker_plugin_test.moc"
