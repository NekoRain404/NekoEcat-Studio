#pragma once

// EtherCATMaintenanceService — maintenance planning facade for EtherCAT networks.
//
// Provides local task scheduling, cancellation, and status tracking. Actual
// maintenance execution fails closed until a live maintenance backend is wired.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QString>
#include <QStringList>

class EcatClient;
class EventBus;

struct MaintenanceTaskInfo {
  QString id;
  QString taskType;
  QString schedule;
  QString status;
  QString lastRun;
  QString nextRun;
  QString result;
};

enum class MaintenanceType { Preventive, Corrective, Predictive, Scheduled };
enum class MaintenancePriority { Low, Medium, High, Critical };

struct ScheduledMaintenanceTask {
  int taskId = 0;
  MaintenanceType type = MaintenanceType::Preventive;
  QString description;
  QDateTime schedule;
  MaintenancePriority priority = MaintenancePriority::Medium;
  int estimatedDuration = 0;
  QStringList requiredResources;
};

struct MaintenanceExecutionRecord {
  int taskId = 0;
  MaintenanceType type = MaintenanceType::Preventive;
  QString description;
  QDateTime executedAt;
  bool success = false;
  QString notes;
};

class EtherCATMaintenanceService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATMaintenanceService(EventBus *bus, EcatClient *client,
                                      QObject *parent = nullptr);

  MaintenanceTaskInfo scheduleTask(const QString &taskType,
                                   const QString &schedule);
  bool cancelTask(const QString &taskId);
  QVector<MaintenanceTaskInfo> listTasks();
  MaintenanceTaskInfo runTask(const QString &taskId);
  MaintenanceTaskInfo getTaskStatus(const QString &taskId);

  bool scheduleMaintenance(const ScheduledMaintenanceTask &task);
  bool executeMaintenance(int taskId);
  QVector<MaintenanceExecutionRecord> maintenanceHistory() const;
  QVector<ScheduledMaintenanceTask> maintenanceSchedule() const;

signals:
  void taskCompleted(const MaintenanceTaskInfo &task);
  void maintenanceScheduled(const ScheduledMaintenanceTask &task);
  void maintenanceCompleted(const MaintenanceExecutionRecord &record);

private:
  MaintenanceTaskInfo makeTask(const QString &id, const QString &taskType,
                               const QString &schedule, const QString &status,
                               const QString &result);
  bool backendReady() const;

  EventBus *bus_;
  EcatClient *client_;
  QVector<MaintenanceTaskInfo> tasks_;
  int nextId_ = 1;
  int nextTaskId_ = 1;
  QVector<ScheduledMaintenanceTask> schedule_;
  QVector<MaintenanceExecutionRecord> history_;
};
