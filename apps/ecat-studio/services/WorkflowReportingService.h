#pragma once

// WorkflowReportingService — generates structured reports for workflow
// execution, performance, errors, and resource utilization.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

enum class WfReportFormat { Text, Html, Json, Markdown };

struct WfReportSection {
    QString title;
    QString content;
    QJsonObject data;
};

struct WfReportChart {
    QString title;
    QString type;
    QJsonObject data;
};

struct WfReportTable {
    QString title;
    QStringList headers;
    QVector<QStringList> rows;
};

struct WfReport {
    QString id;
    QString title;
    QString summary;
    QDateTime timestamp;
    WfReportFormat format = WfReportFormat::Text;
    QVector<WfReportSection> sections;
    QVector<WfReportChart> charts;
    QVector<WfReportTable> tables;
    QStringList recommendations;
};

class WorkflowReportingService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowReportingService(QObject* parent = nullptr);

    WfReport generateExecutionReport(const QString& workflowId);
    WfReport generatePerformanceReport(const QString& workflowId);
    WfReport generateErrorReport(const QString& workflowId);
    WfReport generateResourceReport(const QString& workflowId);

signals:
    void reportGenerated(const WfReport& report);

private:
    WfReport makeReport(const QString& workflowId, const QString& title, const QString& summary,
                        const QVector<WfReportSection>& sections, const QStringList& recommendations);
    WfReportSection makeSection(const QString& title, const QString& content, const QJsonObject& data = {});
    WfReportChart makeChart(const QString& title, const QString& type, const QJsonObject& data);
    WfReportTable makeTable(const QString& title, const QStringList& headers, const QVector<QStringList>& rows);
};
