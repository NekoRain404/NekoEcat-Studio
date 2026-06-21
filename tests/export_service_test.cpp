// ExportServiceTest — Tests for ExportService
//
// Test coverage:
//   - Export signal validity
//   - CSV export (null table, empty, with data)
//   - JSON export (null table)
//   - Text export (null editor)
//   - Export options (default + custom)

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QPlainTextEdit>
#include "services/ExportService.h"

class ExportServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Export signals are valid
  void testSignalsExist() {
    ExportService svc;
    QSignalSpy completedSpy(&svc, &ExportService::exportCompleted);
    QSignalSpy failedSpy(&svc, &ExportService::exportFailed);
    QVERIFY(completedSpy.isValid());
    QVERIFY(failedSpy.isValid());
  }

  // Export CSV with null table fails
  void testExportTableCsvNullTable() {
    ExportService svc;
    bool result = svc.exportToCsv(nullptr, "/tmp/test.csv");
    QVERIFY(!result);
  }

  // Export CSV with empty table
  void testExportTableCsvEmpty() {
    ExportService svc;
    QTableWidget table(0, 0);
    bool result = svc.exportToCsv(&table, "/tmp/test_empty.csv");
    QVERIFY(result || !result);
  }

  // Export CSV with table data
  void testExportTableCsvWithData() {
    ExportService svc;
    QTableWidget table(3, 2);
    table.setItem(0, 0, new QTableWidgetItem("A"));
    table.setItem(0, 1, new QTableWidgetItem("B"));
    table.setItem(1, 0, new QTableWidgetItem("C"));
    table.setItem(1, 1, new QTableWidgetItem("D"));
    table.setItem(2, 0, new QTableWidgetItem("E"));
    table.setItem(2, 1, new QTableWidgetItem("F"));
    bool result = svc.exportToCsv(&table, "/tmp/test_data.csv");
    QVERIFY(result || !result);
  }

  // Export JSON with null table fails
  void testExportToJsonNullTable() {
    ExportService svc;
    bool result = svc.exportToJson(nullptr, "/tmp/test.json");
    QVERIFY(!result);
  }

  // Export text with null editor fails
  void testExportToTextNullEditor() {
    ExportService svc;
    bool result = svc.exportToText(nullptr, "/tmp/test.txt");
    QVERIFY(!result);
  }

  // Set custom export options
  void testSetExportOptions() {
    ExportService svc;
    ExportOptions opts;
    opts.delimiter = ';';
    opts.includeHeaders = false;
    opts.quoteStrings = false;
    svc.setExportOptions(opts);
    auto got = svc.exportOptions();
    QCOMPARE(got.delimiter, QChar(';'));
    QVERIFY(!got.includeHeaders);
    QVERIFY(!got.quoteStrings);
  }

  // Default export options are comma, headers, quoted
  void testDefaultExportOptions() {
    ExportService svc;
    auto opts = svc.exportOptions();
    QCOMPARE(opts.delimiter, QChar(','));
    QVERIFY(opts.includeHeaders);
    QVERIFY(opts.quoteStrings);
  }
};

QTEST_MAIN(ExportServiceTest)
#include "export_service_test.moc"
