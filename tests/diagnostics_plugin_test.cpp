// DiagnosticsPluginTest — Tests for DiagnosticsPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget, filter, and level filter creation
//   - Table and baseline button controls
//   - Text filter hides non-matching rows
//   - Summary counts (errors, warnings)

// DiagnosticsPluginTest — Tests for DiagnosticsPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget and table existence
//   - Filter and level filter controls
//   - Baseline label and buttons
//   - Filter hides non-matching rows
//   - Summary counts display

#include <QTest>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "plugins/diagnostics/DiagnosticsPlugin.h"
#include "services/ServiceContainer.h"

class DiagnosticsPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names (EN/ZH)
  void testIdentity() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QCOMPARE(p.id(), QString("diagnostics"));
    QCOMPARE(p.displayName(), QString("Diagnostics"));
    QCOMPARE(p.displayNameZh(), QString("诊断"));
  }
  // Verify default ordering value
  // Verify default tab order value
  void testDefaultOrder() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QCOMPARE(p.defaultOrder(), 70);
  }
  // Verify plugin is visible
  // Verify plugin is visible
  void testVisible() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QVERIFY(p.visible());
  }
  // Verify main widget is created
  // Verify main widget is created
  void testWidgetNotNull() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QVERIFY(p.widget() != nullptr);
  }
  // Verify diagnostics table widget exists
  // Verify diagnostics table widget is created
  void testTableNotNull() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QVERIFY(p.diagnosticsTable() != nullptr);
  }
  // Verify text filter widget exists
  // Verify diagnostics filter line edit is created
  void testFilterNotNull() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QVERIFY(p.diagnosticsFilter() != nullptr);
  }
  // Verify level filter widget exists
  // Verify level filter widget is created
  void testLevelFilterNotNull() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QVERIFY(p.diagnosticsLevelFilter() != nullptr);
  }
  // Verify topology baseline label exists
  // Verify topology baseline label widget is created
  void testBaselineLabelNotNull() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QVERIFY(p.topologyBaselineLabel() != nullptr);
  }
  // Verify capture and clear baseline buttons exist
  // Verify capture and clear baseline buttons are created
  void testBaselineButtonsNotNull() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    QVERIFY(p.captureBaselineButton() != nullptr);
    QVERIFY(p.clearBaselineButton() != nullptr);
  }
  // Verify text filter hides non-matching rows
  // Verify filter text hides non-matching table rows
  void testFilterHidesNonMatchingRows() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    auto *table = p.diagnosticsTable();
    table->setRowCount(2);
    table->setItem(0, 0, new QTableWidgetItem("10:00"));
    table->setItem(0, 1, new QTableWidgetItem("Error"));
    table->setItem(0, 2, new QTableWidgetItem("Connection lost"));
    table->setItem(1, 0, new QTableWidgetItem("10:01"));
    table->setItem(1, 1, new QTableWidgetItem("Info"));
    table->setItem(1, 2, new QTableWidgetItem("Slave found"));

    p.diagnosticsFilter()->setText("Connection");
    p.filterDiagnosticsTable();
    QVERIFY(!table->isRowHidden(0));
    QVERIFY(table->isRowHidden(1));
  }
  // Verify summary label shows correct error/warning counts
  // Verify summary label shows correct counts after updates
  void testSummaryCounts() {
    ServiceContainer container;
    DiagnosticsPlugin p(&container);
    auto *table = p.diagnosticsTable();
    table->setRowCount(3);
    table->setItem(0, 0, new QTableWidgetItem("10:00"));
    table->setItem(0, 1, new QTableWidgetItem("Error"));
    table->setItem(0, 2, new QTableWidgetItem("Bad"));
    table->setItem(1, 0, new QTableWidgetItem("10:01"));
    table->setItem(1, 1, new QTableWidgetItem("Warning"));
    table->setItem(1, 2, new QTableWidgetItem("Warn"));
    table->setItem(2, 0, new QTableWidgetItem("10:02"));
    table->setItem(2, 1, new QTableWidgetItem("Info"));
    table->setItem(2, 2, new QTableWidgetItem("Ok"));

    p.updateDiagnosticsSummary();
    QVERIFY(p.diagnosticsSummaryLabel()->text().contains("3 shown"));
    QVERIFY(p.diagnosticsSummaryLabel()->text().contains("1 errors"));
    QVERIFY(p.diagnosticsSummaryLabel()->text().contains("1 warnings"));
  }
};

QTEST_MAIN(DiagnosticsPluginTest)
#include "diagnostics_plugin_test.moc"
