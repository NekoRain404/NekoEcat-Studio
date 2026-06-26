#include "WorkflowReportDesignerService.h"

WorkflowReportDesignerService::WorkflowReportDesignerService(QObject *parent)
    : QObject(parent) {}

QString WorkflowReportDesignerService::createTemplate(const QString &name,
                                                       const QString &description,
                                                       const QStringList &sections,
                                                       const QStringList &dataFields) {
  QString id = QString("tmpl_%1").arg(nextTemplateId_++);
  WfReportTemplate t;
  t.id = id;
  t.name = name;
  t.description = description;
  t.sections = sections;
  t.dataFields = dataFields;
  t.format = "text";
  t.createdAt = QDateTime::currentDateTime();
  templates_[id] = t;
  emit templateCreated(id);
  return id;
}

bool WorkflowReportDesignerService::removeTemplate(const QString &templateId) {
  if (!templates_.contains(templateId))
    return false;
  templates_.remove(templateId);
  emit templateRemoved(templateId);
  return true;
}

bool WorkflowReportDesignerService::updateTemplate(const QString &templateId,
                                                    const QString &name,
                                                    const QStringList &sections) {
  if (!templates_.contains(templateId))
    return false;
  templates_[templateId].name = name;
  templates_[templateId].sections = sections;
  emit templateUpdated(templateId);
  return true;
}

WfReportTemplate WorkflowReportDesignerService::templateById(const QString &templateId) const {
  return templates_.value(templateId, WfReportTemplate{});
}

QVector<WfReportTemplate> WorkflowReportDesignerService::allTemplates() const {
  QVector<WfReportTemplate> result;
  for (auto it = templates_.begin(); it != templates_.end(); ++it)
    result << it.value();
  return result;
}

int WorkflowReportDesignerService::templateCount() const {
  return templates_.size();
}

WfCustomReport WorkflowReportDesignerService::generateReport(const QString &templateId,
                                                              const QJsonObject &data) {
  WfCustomReport report;
  if (!templates_.contains(templateId)) {
    report.success = false;
    return report;
  }
  const WfReportTemplate &tmpl = templates_[templateId];
  report.id = QString("rpt_%1").arg(nextReportId_++);
  report.templateId = templateId;
  report.title = tmpl.name + " Report";
  report.summary = "Generated from template: " + tmpl.name;
  report.generatedAt = QDateTime::currentDateTime();
  report.success = true;
  for (const auto &section : tmpl.sections) {
    QString content = "Section: " + section;
    if (data.contains(section))
      content += " — data: " + data.value(section).toString();
    report.sections << qMakePair(section, content);
  }
  reports_ << report;
  emit reportGenerated(report.id);
  return report;
}

QVector<WfCustomReport> WorkflowReportDesignerService::allReports() const {
  return reports_;
}

int WorkflowReportDesignerService::reportCount() const {
  return reports_.size();
}
