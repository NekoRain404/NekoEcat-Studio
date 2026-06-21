#pragma once

// WorkflowReplicationService -- manages replication of configuration,
// data, state, and backups across workflow targets.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVector>

struct WorkflowReplicationStatus {
  QString target;
  QString status;
  QDateTime startTime;
  QDateTime endTime;
  int recordsReplicated = 0;
  int errors = 0;
};

class WorkflowReplicationService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowReplicationService(QObject *parent = nullptr);

  bool replicateConfiguration(const QStringList &targets);
  bool replicateData(const QStringList &targets);
  bool replicateState(const QStringList &targets);
  bool replicateBackup(const QStringList &targets);
  QVector<WorkflowReplicationStatus> replicationHistory() const;

signals:
  void replicationStarted(const QString &target);
  void replicationCompleted(const QString &target, bool success);

private:
  QVector<WorkflowReplicationStatus> history_;
};
