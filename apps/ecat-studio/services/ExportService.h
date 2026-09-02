#pragma once

// ExportService — provides CSV/JSON/text export for data tables.
//
// Exports QTableWidget contents to CSV, JSON, or plain-text files.
// Configurable delimiter, quoting, and header inclusion via ExportOptions.
// No EcatClient dependency — operates purely on widget data.
//
// This service provides data export capabilities for the NekoEcat Studio
// application. It handles:
//   - CSV export with configurable delimiter and quoting
//   - JSON export with structured data format
//   - Plain text export for text editors
//   - Export options configuration
//   - Export completion and error notifications
//
// Usage:
//   ServiceContainer *container = ...;
//   ExportService *exportSvc = container->exportService();
//   ExportOptions options;
//   options.delimiter = ',';
//   options.includeHeaders = true;
//   exportSvc->setExportOptions(options);
//   exportSvc->exportToCsv(tableWidget, "/path/to/export.csv");
//   exportSvc->exportToJson(tableWidget, "/path/to/export.json");
//   exportSvc->exportToText(plainTextEdit, "/path/to/export.txt");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. File I/O
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - Export operations are O(n) where n is number of rows
//   - File I/O is synchronous (blocking)
//   - Memory usage is proportional to table size

#include <QObject>
#include <QString>

class QTableWidget;
class QPlainTextEdit;

// Export configuration options.
struct ExportOptions {
    QChar delimiter = ',';      // CSV delimiter character
    bool includeHeaders = true; // Whether to include column headers
    bool quoteStrings = true;   // Whether to quote string values
    QString lineEnding = "\n";  // Line ending character(s)
};

class ExportService : public QObject {
    Q_OBJECT
public:
    explicit ExportService(QObject* parent = nullptr);

    // Export a QTableWidget to CSV format.
    // @param table  Source table widget
    // @param path   Output file path
    // @return true if export was successful
    bool exportToCsv(QTableWidget* table, const QString& path);

    // Export a QTableWidget to JSON format.
    // @param table  Source table widget
    // @param path   Output file path
    // @return true if export was successful
    bool exportToJson(QTableWidget* table, const QString& path);

    // Export a QPlainTextEdit to plain text format.
    // @param editor  Source text editor
    // @param path    Output file path
    // @return true if export was successful
    bool exportToText(QPlainTextEdit* editor, const QString& path);

    // Set the export options.
    // @param options  ExportOptions structure
    void setExportOptions(const ExportOptions& options);

    // Get the current export options.
    // @return ExportOptions structure
    ExportOptions exportOptions() const;

signals:
    // Emitted when an export completes successfully.
    // @param path  Path to the exported file
    void exportCompleted(const QString& path);

    // Emitted when an export fails.
    // @param error  Human-readable error message
    void exportFailed(const QString& error);

private:
    ExportOptions options_; // Current export options
};
