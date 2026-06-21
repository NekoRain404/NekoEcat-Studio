#pragma once

// WorkflowReportDesignerService — designs and generates workflow reports.
//
// Provides report template management, section composition, data field
// binding, and report generation in multiple formats.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QJsonObject>
#include <QDateTime>

struct WfReportTemplate {
  QString id;
  QString name;
  QString description;
  QStringList sections;
  QStringList dataFields;
  QString format;
  QDateTime createdAt;
};

struct WfCustomReport {
  QString id;
  QString templateId;
  QString title;
  QString summary;
  QVector<QPair<QString, QString>> sections;
  QDateTime generatedAt;
  bool success = true;
};

class WorkflowReportDesignerService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowReportDesignerService(QObject *parent = nullptr);

  QString createTemplate(const QString &name, const QString &description,
                         const QStringList &sections, const QStringList &dataFields);
  bool removeTemplate(const QString &templateId);
  bool updateTemplate(const QString &templateId, const QString &name,
                      const QStringList &sections);
  WfReportTemplate templateById(const QString &templateId) const;
  QVector<WfReportTemplate> allTemplates() const;
  int templateCount() const;

  WfCustomReport generateReport(const QString &templateId,
                                const QJsonObject &data);
  QVector<WfCustomReport> allReports() const;
  int reportCount() const;

signals:
  void templateCreated(const QString &templateId);
  void templateRemoved(const QString &templateId);
  void templateUpdated(const QString &templateId);
  void reportGenerated(const QString &reportId);

private:
  QMap<QString, WfReportTemplate> templates_;
  QVector<WfCustomReport> reports_;
  int nextTemplateId_ = 1;
  int nextReportId_ = 1;
};
