// ExportServiceTest — Tests for ExportService
//
// Test coverage:
//   - Export signal validity
//   - CSV export (null table, empty, with data)
//   - JSON export (null table)
//   - Text export (null editor)
//   - Export options (default + custom)

#include "services/ExportService.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPlainTextEdit>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTest>

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
        bool result = svc.exportToCsv(nullptr, QStringLiteral("/tmp/test.csv"));
        QVERIFY(!result);
    }

    // Export CSV with empty table
    void testExportTableCsvEmpty() {
        ExportService svc;
        QTableWidget table(0, 0);
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("empty.csv"));
        QVERIFY(svc.exportToCsv(&table, path));
        QVERIFY(QFile::exists(path));
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
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("data.csv"));
        QVERIFY(svc.exportToCsv(&table, path));
        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString content = QString::fromUtf8(file.readAll());
        QVERIFY(content.contains(QStringLiteral("\"A\",\"B\"")));
        QVERIFY(content.contains(QStringLiteral("\"E\",\"F\"")));
    }

    // Export JSON with null table fails
    void testExportToJsonNullTable() {
        ExportService svc;
        bool result = svc.exportToJson(nullptr, QStringLiteral("/tmp/test.json"));
        QVERIFY(!result);
    }

    // Export JSON with table data writes a parseable array.
    void testExportToJsonWithData() {
        ExportService svc;
        QTableWidget table(1, 2);
        table.setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
        table.setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
        table.setItem(0, 0, new QTableWidgetItem("Cycle"));
        table.setItem(0, 1, new QTableWidgetItem("1000"));

        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("data.json"));
        QVERIFY(svc.exportToJson(&table, path));
        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly));
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QVERIFY(doc.isArray());
        QCOMPARE(doc.array().size(), 1);
        QCOMPARE(doc.array().first().toObject().value("Name").toString(), QStringLiteral("Cycle"));
    }

    // Export text with null editor fails
    void testExportToTextNullEditor() {
        ExportService svc;
        bool result = svc.exportToText(nullptr, QStringLiteral("/tmp/test.txt"));
        QVERIFY(!result);
    }

    // Export text writes editor content and invalid paths fail.
    void testExportToTextWithDataAndInvalidPath() {
        ExportService svc;
        QPlainTextEdit editor;
        editor.setPlainText(QStringLiteral("line 1\nline 2"));

        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("notes.txt"));
        QVERIFY(svc.exportToText(&editor, path));
        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        QCOMPARE(QString::fromUtf8(file.readAll()), QStringLiteral("line 1\nline 2"));

        QSignalSpy failedSpy(&svc, &ExportService::exportFailed);
        QTableWidget emptyCsvTable(0, 0);
        QTableWidget emptyJsonTable(0, 0);
        QVERIFY(!svc.exportToCsv(&emptyCsvTable, QString()));
        QVERIFY(!svc.exportToJson(&emptyJsonTable, QString()));
        QVERIFY(!svc.exportToText(&editor, QString()));
        QCOMPARE(failedSpy.count(), 3);
        for (const auto& args : failedSpy) {
            QCOMPARE(args.at(0).toString(), QStringLiteral("Export path is empty"));
        }
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
