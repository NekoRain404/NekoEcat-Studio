#pragma once

// EtherCATReportingService — generates system, performance, error,
// and compliance reports with explicit file export capabilities.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>
#include <QDateTime>

class EcatClient;
class EventBus;

struct ReportChart {
  QString title;
  QString type;
  QStringList labels;
  QVector<double> values;
};

struct ReportTable {
  QString title;
  QStringList headers;
  QVector<QStringList> rows;
};

struct ReportSection {
  QString title;
  QString content;
  QVector<ReportChart> charts;
  QVector<ReportTable> tables;
  QStringList recommendations;
};

struct Report {
  QString title;
  QString summary;
  QVector<ReportSection> sections;
  QVector<ReportChart> charts;
  QVector<ReportTable> tables;
  QStringList recommendations;
  QString format;
  QDateTime timestamp;
};

class EtherCATReportingService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATReportingService(EventBus *bus, EcatClient *client,
                                    QObject *parent = nullptr);

  Report generateSystemReport();
  Report generatePerformanceReport();
  Report generateErrorReport();
  Report generateComplianceReport();

  // Render/export compatibility check for existing callers. This overload does
  // not write a file because no output path is provided.
  bool exportReport(const Report &report, const QString &format);
  bool exportReport(const Report &report, const QString &filePath,
                    const QString &format);

signals:
  void reportGenerated(const Report &report);

private:
  Report makeReport(const QString &title, const QString &summary,
                    const QVector<ReportSection> &sections,
                    const QStringList &recommendations);
  QString renderReport(const Report &report, const QString &format) const;

  EventBus *bus_;
  EcatClient *client_;
};
