// ReportPluginTest — Tests for ReportPlugin
//
// Test coverage:
//   - Plugin identity, widget creation, and initial state
//   - Template and data source table management
//   - Report generation, preview, and history
//   - Format selection, export, and status updates

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include "plugins/report/ReportPlugin.h"

class ReportPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, displayName, displayNameZh, defaultOrder, visible
  // Plugin reports correct id, display names, order, and visibility
  void testPluginIdentity() {
    ReportPlugin plugin;

    QCOMPARE(plugin.id(), QString("report"));
    QCOMPARE(plugin.displayName(), QString("Report Generator"));
    QCOMPARE(plugin.displayNameZh(), QString("报告生成器"));
    QCOMPARE(plugin.defaultOrder(), 230);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created and non-null
  // Widget is created and not null
  void testWidgetCreation() {
    ReportPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial template count, data source count, and report count
  // Initial state has correct template, data source, and report counts
  void testInitialState() {
    ReportPlugin plugin;

    QCOMPARE(plugin.reportTemplateCount(), 5);
    QCOMPARE(plugin.dataSourceCount(), 6);
    QCOMPARE(plugin.reportCount(), 0);
  }

  // Verify template table has correct row and column counts
  // Template table has correct row and column count
  void testTemplateTable() {
    ReportPlugin plugin;

    QTableWidget *table = plugin.templateTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 5);
    QCOMPARE(table->columnCount(), 3);
  }

  // Verify data source table has correct row and column counts
  // Data source table has correct row and column count
  void testDataSourceTable() {
    ReportPlugin plugin;

    QTableWidget *table = plugin.dataSourceTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 6);
    QCOMPARE(table->columnCount(), 3);
  }

  // Select template and verify templateSelected signal
  // Select template emits selection signal
  void testSelectTemplate() {
    ReportPlugin plugin;
    QSignalSpy selectSpy(&plugin, &ReportPlugin::templateSelected);

    plugin.selectTemplate(0);
    QCOMPARE(selectSpy.count(), 1);
  }

  // Verify preview view is read-only and shows content after template selection
  // Preview view shows template content after selection
  void testPreview() {
    ReportPlugin plugin;

    QTextEdit *pv = plugin.previewView();
    QVERIFY(pv != nullptr);
    QVERIFY(pv->isReadOnly());

    plugin.selectTemplate(0);
    QVERIFY(!pv->toPlainText().isEmpty());
  }

  // Generate report and verify reportCount and reportGenerated signal
  // Generate report increments count and emits signal
  void testGenerateReport() {
    ReportPlugin plugin;
    QSignalSpy genSpy(&plugin, &ReportPlugin::reportGenerated);

    plugin.selectTemplate(0);
    plugin.generateReport();

    QCOMPARE(plugin.reportCount(), 1);
    QCOMPARE(genSpy.count(), 1);
  }

  // Verify history table shows generated reports with correct columns
  // History table shows generated reports
  void testHistoryTable() {
    ReportPlugin plugin;

    plugin.selectTemplate(0);
    plugin.generateReport();

    QTableWidget *table = plugin.historyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->columnCount(), 4);
  }

  // Add custom report template and verify count increments
  // Add custom report template increments count
  void testAddReportTemplate() {
    ReportPlugin plugin;
    int initial = plugin.reportTemplateCount();

    ReportPlugin::ReportTemplate tmpl;
    tmpl.id = "custom";
    tmpl.name = "Custom Report";
    tmpl.description = "Custom description";
    tmpl.format = "HTML";

    plugin.addReportTemplate(tmpl);
    QCOMPARE(plugin.reportTemplateCount(), initial + 1);
  }

  // Add custom data source and verify count increments
  // Add custom data source increments count
  void testAddDataSource() {
    ReportPlugin plugin;
    int initial = plugin.dataSourceCount();

    ReportPlugin::DataSource ds;
    ds.id = "custom";
    ds.name = "Custom Source";
    ds.type = "custom_type";
    ds.enabled = true;

    plugin.addDataSource(ds);
    QCOMPARE(plugin.dataSourceCount(), initial + 1);
  }

  // Toggle data source enabled state and verify table cell text
  // Toggle data source updates enabled state in table
  void testToggleDataSource() {
    ReportPlugin plugin;

    plugin.toggleDataSource(0, false);
    QCOMPARE(plugin.dataSourceTable()->item(0, 2)->text(), QString("No"));

    plugin.toggleDataSource(0, true);
    QCOMPARE(plugin.dataSourceTable()->item(0, 2)->text(), QString("Yes"));
  }

  // Select output format and verify preview reflects the format
  // Select format updates preview content
  void testSelectFormat() {
    ReportPlugin plugin;

    plugin.selectTemplate(0);
    plugin.selectFormat("PDF");
    QVERIFY(plugin.previewView()->toPlainText().contains("PDF"));
  }

  // Verify status label updates after report generation
  // Status label shows generated text after report creation
  void testStatusLabel() {
    ReportPlugin plugin;

    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);

    plugin.selectTemplate(0);
    plugin.generateReport();
    QVERIFY(label->text().contains("Generated"));
  }

  // Generate multiple reports and verify reportCount
  // Multiple reports can be generated sequentially
  void testMultipleReports() {
    ReportPlugin plugin;

    plugin.selectTemplate(0);
    plugin.generateReport();
    plugin.selectTemplate(1);
    plugin.generateReport();

    QCOMPARE(plugin.reportCount(), 2);
  }

  // Export history to CSV and verify file exists
  // Export history writes CSV file
  void testExportHistory() {
    ReportPlugin plugin;

    plugin.selectTemplate(0);
    plugin.generateReport();

    QString path = QDir::temp().absoluteFilePath("report_history_test.csv");
    plugin.exportHistory(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }

  // Export report to file and verify file exists
  // Export report writes file to disk
  void testExportReport() {
    ReportPlugin plugin;

    plugin.selectTemplate(0);
    QString path = QDir::temp().absoluteFilePath("report_export_test.txt");
    plugin.exportReport(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(ReportPluginTest)
#include "report_plugin_test.moc"
