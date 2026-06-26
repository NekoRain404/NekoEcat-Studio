#include "EtherCATReportingService.h"
#include "infra/EcatClient.h"
#include "EventBus.h"

#include <QFile>
#include <QTextStream>

// EtherCATReportingService.cpp — Generates categorized EtherCAT reports with export
//
// Implementation notes:
//   - Four report types: system, performance, error, compliance
//   - Each report has titled sections with per-section recommendations
//   - Export supports HTML (full page) and plain text (RST-like) formats

EtherCATReportingService::EtherCATReportingService(EventBus *bus,
                                                   EcatClient *client,
                                                   QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

Report EtherCATReportingService::makeReport(
    const QString &title, const QString &summary,
    const QVector<ReportSection> &sections,
    const QStringList &recommendations)
{
    Report r;
    r.title = title;
    r.summary = summary;
    r.sections = sections;
    r.recommendations = recommendations;
    r.timestamp = QDateTime::currentDateTime();
    emit reportGenerated(r);
    return r;
}

Report EtherCATReportingService::generateSystemReport()
{
    QVector<ReportSection> sections;

    ReportSection overview;
    overview.title = QStringLiteral("System Overview");
    overview.content = QStringLiteral(
        "Comprehensive system health and configuration report.");
    overview.recommendations << QStringLiteral("Review all active slaves")
                             << QStringLiteral("Verify DC synchronization");
    sections << overview;

    ReportSection config;
    config.title = QStringLiteral("Configuration");
    config.content = QStringLiteral("Current EtherCAT network configuration details.");
    sections << config;

    QStringList recs;
    recs << QStringLiteral("Keep firmware up to date")
         << QStringLiteral("Monitor system health regularly");

    return makeReport(QStringLiteral("System Report"),
                      QStringLiteral("System overview and configuration analysis"),
                      sections, recs);
}

Report EtherCATReportingService::generatePerformanceReport()
{
    QVector<ReportSection> sections;

    ReportSection timing;
    timing.title = QStringLiteral("Timing Analysis");
    timing.content = QStringLiteral("Cycle time, jitter, and synchronization metrics.");
    timing.recommendations << QStringLiteral("Optimize cycle time below 1ms")
                           << QStringLiteral("Minimize jitter for stable operation");
    sections << timing;

    ReportSection throughput;
    throughput.title = QStringLiteral("Throughput");
    throughput.content = QStringLiteral("PDO/SDO throughput and bandwidth utilization.");
    sections << throughput;

    QStringList recs;
    recs << QStringLiteral("Profile cycle time distribution")
         << QStringLiteral("Check for frame loss under load");

    return makeReport(QStringLiteral("Performance Report"),
                      QStringLiteral("Network timing and throughput analysis"),
                      sections, recs);
}

Report EtherCATReportingService::generateErrorReport()
{
    QVector<ReportSection> sections;

    ReportSection errors;
    errors.title = QStringLiteral("Error Summary");
    errors.content = QStringLiteral("Categorized error counts and trends.");
    errors.recommendations << QStringLiteral("Address CRC errors first")
                           << QStringLiteral("Check cable integrity");
    sections << errors;

    ReportSection history;
    history.title = QStringLiteral("Error History");
    history.content = QStringLiteral("Timeline of recent error events.");
    sections << history;

    QStringList recs;
    recs << QStringLiteral("Set up automated error alerting")
         << QStringLiteral("Log errors for trend analysis");

    return makeReport(QStringLiteral("Error Report"),
                      QStringLiteral("Error analysis and trend reporting"),
                      sections, recs);
}

Report EtherCATReportingService::generateComplianceReport()
{
    QVector<ReportSection> sections;

    ReportSection ethercat;
    ethercat.title = QStringLiteral("EtherCAT Standard Compliance");
    ethercat.content = QStringLiteral("Verification against EtherCAT specification.");
    ethercat.recommendations << QStringLiteral("Verify all slaves support required features")
                             << QStringLiteral("Test interoperability");
    sections << ethercat;

    ReportSection safety;
    safety.title = QStringLiteral("Safety Compliance");
    safety.content = QStringLiteral("Safety-related protocol compliance checks.");
    sections << safety;

    QStringList recs;
    recs << QStringLiteral("Run compliance tests after configuration changes")
         << QStringLiteral("Document compliance status for audits");

    return makeReport(QStringLiteral("Compliance Report"),
                      QStringLiteral("Standards and safety compliance verification"),
                      sections, recs);
}

bool EtherCATReportingService::exportReport(const Report &report,
                                            const QString &format)
{
    return !renderReport(report, format).isEmpty();
}

bool EtherCATReportingService::exportReport(const Report &report,
                                            const QString &filePath,
                                            const QString &format)
{
    if (filePath.isEmpty())
        return false;

    const QString content = renderReport(report, format);
    if (content.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << content;
    return out.status() == QTextStream::Ok;
}

QString EtherCATReportingService::renderReport(const Report &report,
                                               const QString &format) const
{
    if (report.title.isEmpty())
        return {};

    QString content;
    if (format == QStringLiteral("html")) {
        content = QStringLiteral(
            "<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
            "<title>%1</title></head><body>\n"
            "<h1>%1</h1>\n<p>%2</p>\n")
                      .arg(report.title, report.summary);

        for (const auto &section : report.sections) {
            content += QStringLiteral("<h2>%1</h2>\n<p>%2</p>\n")
                           .arg(section.title, section.content);
            for (const auto &rec : section.recommendations)
                content += QStringLiteral("<li>%1</li>\n").arg(rec);
        }

        if (!report.recommendations.isEmpty()) {
            content += QStringLiteral("<h2>Recommendations</h2>\n<ul>\n");
            for (const auto &rec : report.recommendations)
                content += QStringLiteral("<li>%1</li>\n").arg(rec);
            content += QStringLiteral("</ul>\n");
        }

        content += QStringLiteral(
            "<hr><p>Generated: %1</p></body></html>")
                       .arg(report.timestamp.toString(Qt::ISODate));
    } else if (format == QStringLiteral("text")) {
        content = report.title + QStringLiteral("\n");
        content += QString(report.title.size(), '=') + QStringLiteral("\n\n");
        content += report.summary + QStringLiteral("\n\n");

        for (const auto &section : report.sections) {
            content += section.title + QStringLiteral("\n");
            content += QString(section.title.size(), '-') + QStringLiteral("\n");
            content += section.content + QStringLiteral("\n\n");
            for (const auto &rec : section.recommendations)
                content += QStringLiteral("  * %1\n").arg(rec);
        }

        if (!report.recommendations.isEmpty()) {
            content += QStringLiteral("\nRecommendations:\n");
            for (const auto &rec : report.recommendations)
                content += QStringLiteral("  * %1\n").arg(rec);
        }

        content += QStringLiteral("\nGenerated: %1\n")
                       .arg(report.timestamp.toString(Qt::ISODate));
    } else {
        return {};
    }

    return content;
}
