#pragma once

// WorkflowCertificationManagerService — high-level certification requirement
// management for workflow operations. Provides requirement CRUD, status
// tracking, and renewal management.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>

struct WfCertRequirement {
  QString id;
  QString name;
  QString standard;
  QString status;
  QDateTime createdAt;
  QDateTime expiry;
};

class WorkflowCertificationManagerService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowCertificationManagerService(QObject *parent = nullptr);

  QString addRequirement(const QString &name, const QString &standard,
                         const QDateTime &expiry = {});
  bool removeRequirement(const QString &reqId);
  bool updateStatus(const QString &reqId, const QString &status);
  WfCertRequirement requirement(const QString &reqId) const;
  QVector<WfCertRequirement> allRequirements() const;
  int requirementCount() const;
  QVector<WfCertRequirement> requirementsByStandard(const QString &standard) const;
  bool renewRequirement(const QString &reqId, const QDateTime &newExpiry);
  int pendingCount() const;

signals:
  void requirementAdded(const QString &reqId);
  void requirementRemoved(const QString &reqId);
  void statusUpdated(const QString &reqId, const QString &status);
  void requirementRenewed(const QString &reqId);

private:
  QVector<WfCertRequirement> requirements_;
  int nextId_ = 1;
};
