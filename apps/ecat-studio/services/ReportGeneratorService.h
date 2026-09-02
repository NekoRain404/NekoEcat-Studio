#pragma once

// ReportGeneratorService — generates diagnostic reports in HTML/text formats.
//
// Accumulates sections (heading + content pairs) and exports them as
// formatted HTML or plain-text reports. Supports template-based generation.
//
// This service provides report generation capabilities for the NekoEcat
// Studio application. It handles:
//   - Report section management (add, clear)
//   - HTML report generation with templates
//   - Plain text report generation
//   - Report export to files
//   - Template-based report customization
//
// Usage:
//   ServiceContainer *container = ...;
//   ReportGeneratorService *report = container->reportGenerator();
//   report->generateReport("Diagnostic Report");
//   report->addSection("Topology", "Slave count: 5");
//   report->addSection("Performance", "Cycle time: 1000us");
//   report->exportReport("/path/to/report.html", "html");
//   report->exportReport("/path/to/report.txt", "text");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Report
//   generation and export are synchronous operations.
//
// Performance:
//   - Section addition is O(1)
//   - Report generation is O(n) where n is number of sections
//   - Export is O(n) for rendering and file I/O

#include <QObject>
#include <QPair>
#include <QString>
#include <QVector>

class ReportGeneratorService : public QObject {
    Q_OBJECT
public:
    explicit ReportGeneratorService(QObject* parent = nullptr);

    // Start generating a new report with the given title.
    // @param title  Report title
    void generateReport(const QString& title);

    // Add a section to the current report.
    // @param heading  Section heading
    // @param content  Section content
    void addSection(const QString& heading, const QString& content);

    // Set the HTML template for report generation.
    // @param htmlTemplate  HTML template with placeholders
    void setTemplate(const QString& htmlTemplate);

    // Export the report to a file.
    // @param path    Output file path
    // @param format  Export format ("html" or "text")
    // @return true if export was successful
    bool exportReport(const QString& path, const QString& format);

    // Clear all sections from the current report.
    void clearSections();

signals:
    // Emitted when a report is generated successfully.
    // @param path  Path to the generated report file
    void reportGenerated(const QString& path);

    // Emitted when report generation fails.
    // @param error  Human-readable error message
    void reportFailed(const QString& error);

private:
    // Render the report as HTML.
    QString renderHtml(const QString& title) const;

    // Render the report as plain text.
    QString renderText(const QString& title) const;

    QVector<QPair<QString, QString>> sections_; // Report sections (heading, content)
    QString template_;                          // HTML template
};
