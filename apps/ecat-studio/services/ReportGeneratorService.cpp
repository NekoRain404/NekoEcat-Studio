#include "ReportGeneratorService.h"

#include <QFile>
#include <QTextStream>

// ReportGeneratorService.cpp — Section-based report builder with HTML and text rendering
//
// Implementation notes:
//   - Reports composed of heading/content section pairs added incrementally
//   - HTML renderer supports a custom template with {{title}} and {{body}} placeholders
//   - Text renderer produces RST-style output with underline headings

ReportGeneratorService::ReportGeneratorService(QObject* parent) : QObject(parent) {}

void ReportGeneratorService::generateReport(const QString& title) {
    if (sections_.isEmpty()) {
        emit reportFailed(QStringLiteral("No sections to generate"));
        return;
    }
    Q_UNUSED(title)
}

void ReportGeneratorService::addSection(const QString& heading, const QString& content) {
    sections_.append({heading, content});
}

void ReportGeneratorService::setTemplate(const QString& htmlTemplate) {
    template_ = htmlTemplate;
}

bool ReportGeneratorService::exportReport(const QString& path, const QString& format) {
    if (sections_.isEmpty()) {
        emit reportFailed(QStringLiteral("No sections to export"));
        return false;
    }
    if (path.isEmpty()) {
        emit reportFailed(QStringLiteral("Report export path is empty"));
        return false;
    }

    QString title = QStringLiteral("NekoEcat Diagnostic Report");
    QString content;

    if (format == "html") {
        content = renderHtml(title);
    } else if (format == "text") {
        content = renderText(title);
    } else {
        emit reportFailed(QStringLiteral("Unsupported report format: %1").arg(format));
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit reportFailed(QStringLiteral("Cannot open file: %1").arg(path));
        return false;
    }
    const QByteArray bytes = content.toUtf8();
    if (file.write(bytes) != bytes.size()) {
        emit reportFailed(QStringLiteral("Cannot write report: %1").arg(path));
        return false;
    }
    file.close();
    emit reportGenerated(path);
    return true;
}

void ReportGeneratorService::clearSections() {
    sections_.clear();
}

QString ReportGeneratorService::renderHtml(const QString& title) const {
    if (!template_.isEmpty()) {
        QString html = template_;
        html.replace("{{title}}", title);

        QString body;
        for (const auto& section : sections_) {
            body += QStringLiteral("<h2>%1</h2>\n<pre>%2</pre>\n").arg(section.first, section.second);
        }
        html.replace("{{body}}", body);
        return html;
    }

    QString html = QStringLiteral("<!DOCTYPE html>\n<html><head><meta charset=\"utf-8\">"
                                  "<title>%1</title></head><body>\n<h1>%1</h1>\n")
                       .arg(title);

    for (const auto& section : sections_) {
        html += QStringLiteral("<h2>%1</h2>\n<pre>%2</pre>\n").arg(section.first, section.second);
    }
    html += QStringLiteral("</body></html>\n");
    return html;
}

QString ReportGeneratorService::renderText(const QString& title) const {
    QString text = title + QStringLiteral("\n");
    text += QString(title.size(), '=') + QStringLiteral("\n\n");

    for (const auto& section : sections_) {
        text += section.first + QStringLiteral("\n");
        text += QString(section.first.size(), '-') + QStringLiteral("\n");
        text += section.second + QStringLiteral("\n\n");
    }
    return text;
}
