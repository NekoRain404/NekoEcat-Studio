#pragma once

// EtherCATReplicationService — manages replication of configuration,
// data, state, and backups across EtherCAT network targets.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVector>

class EventBus;
class EcatClient;

struct ReplicationStatus {
  QString target;
  QString status;
  QDateTime startTime;
  QDateTime endTime;
  int recordsReplicated = 0;
  int errors = 0;
};

class EtherCATReplicationService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATReplicationService(EventBus *bus, EcatClient *client,
                                      QObject *parent = nullptr);

  bool replicateConfiguration(const QStringList &targets);
  bool replicateData(const QStringList &targets);
  bool replicateState(const QStringList &targets);
  bool replicateBackup(const QStringList &targets);
  QVector<ReplicationStatus> replicationHistory() const;

signals:
  void replicationStarted(const QString &target);
  void replicationCompleted(const QString &target, bool success);

private:
  EventBus *bus_;
  EcatClient *client_;
  QVector<ReplicationStatus> history_;
};
