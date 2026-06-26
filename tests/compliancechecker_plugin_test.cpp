// ComplianceCheckerPluginTest — Tests for ComplianceCheckerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Empty fail-closed check, violation, and recommendation tables
//   - Compliance score calculation
//   - Add checks
//   - Compliance UI must not mint backend-free results

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

    QCOMPARE(plugin.checkCount(), 0);
    QCOMPARE(plugin.violationCount(), 0);
    QCOMPARE(plugin.recommendationCount(), 0);
  }

  // Verify check table structure
  void testCheckTable() {
    ComplianceCheckerPlugin plugin;

    QTableWidget *table = plugin.checkTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 0);
    QCOMPARE(table->columnCount(), 5);
  }

  // Verify violation table structure
  void testViolationTable() {
    ComplianceCheckerPlugin plugin;

    QTableWidget *table = plugin.violationTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 0);
    QCOMPARE(table->columnCount(), 5);
  }

  // Verify recommendation table structure
  void testRecommendationTable() {
    ComplianceCheckerPlugin plugin;

    QTableWidget *table = plugin.recommendationTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 0);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify compliance score is within valid range
  void testComplianceScore() {
    ComplianceCheckerPlugin plugin;

    double score = plugin.complianceScore();
    QCOMPARE(score, 0.0);
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
    c.passed = false;
    c.checkedAt = QDateTime::currentDateTime();

    plugin.addCheck(c);
    QCOMPARE(plugin.checkCount(), initial + 1);
  }

  // Verify removing a check decrements count
  void testRemoveCheck() {
    ComplianceCheckerPlugin plugin;
    ComplianceCheckerPlugin::ComplianceCheck c;
    c.id = "c_remove";
    c.name = "Remove Check";
    c.category = "Safety";
    c.description = "Remove test";
    c.passed = false;
    plugin.addCheck(c);
    int initial = plugin.checkCount();

    plugin.removeCheck(0);
    QCOMPARE(plugin.checkCount(), initial - 1);
  }

  // Verify run check emits completion signal
  void testRunCheck() {
    ComplianceCheckerPlugin plugin;
    QSignalSpy spy(&plugin, &ComplianceCheckerPlugin::checkCompleted);

    ComplianceCheckerPlugin::ComplianceCheck c;
    c.id = "c_pending";
    c.name = "Pending Check";
    c.category = "Safety";
    c.description = "Requires backend evidence";
    c.passed = false;
    plugin.addCheck(c);

    plugin.runCheck(0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(1).toBool(), false);
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
    plugin.addViolation({"v_remove", "c1", "info", "remove", "fix",
                         QDateTime::currentDateTime()});
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
    plugin.addRecommendation({"r_remove", "low", "Remove Recommendation",
                              "remove", "General"});
    int initial = plugin.recommendationCount();

    plugin.removeRecommendation(0);
    QCOMPARE(plugin.recommendationCount(), initial - 1);
  }

  // Verify generate report returns correct counts
  void testGenerateReport() {
    ComplianceCheckerPlugin plugin;

    auto report = plugin.generateReport();
    QCOMPARE(report.totalChecks, 0);
    QCOMPARE(report.score, 0.0);
    QCOMPARE(report.violations.size(), 0);
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

  void testSourceDoesNotMintSyntheticComplianceResults() {
    QFile file(QStringLiteral(SOURCE_ROOT
                              "/apps/ecat-studio/plugins/compliancechecker/ComplianceCheckerPlugin.cpp"));
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(file.errorString()));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("\"EtherCAT Cable Redundancy\"")),
             "Compliance checker UI must not seed canned checks");
    QVERIFY2(!source.contains(QStringLiteral("\"PDO mapping mismatch on slave 2\"")),
             "Compliance checker UI must not seed canned violations");
    QVERIFY2(!source.contains(QStringLiteral("\"Enable Cable Redundancy\"")),
             "Compliance checker UI must not seed canned recommendations");
    QVERIFY2(!source.contains(QStringLiteral("emit checkCompleted(index, checks_[index].passed)")),
             "Running a UI check must not replay a precomputed pass/fail result");
  }
};

QTEST_MAIN(ComplianceCheckerPluginTest)
#include "compliancechecker_plugin_test.moc"
