#pragma once

// WorkflowMaintenanceService -- schedules preventive, corrective, predictive,
// and scheduled maintenance tasks for EtherCAT networks. Execution requires a
// maintenance backend; until one is wired, execution fails closed.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

enum class WfMaintenanceType { Preventive, Corrective, Predictive, Scheduled };

struct WfMaintenanceTask {
    int taskId = 0;
    WfMaintenanceType type = WfMaintenanceType::Scheduled;
    QString description;
    QString schedule;
    int priority = 0;
    int estimatedDurationMin = 0;
    QStringList requiredResources;
};

struct WfMaintenanceRecord {
    int taskId = 0;
    WfMaintenanceType type = WfMaintenanceType::Scheduled;
    QString description;
    QDateTime startTime;
    QDateTime endTime;
    bool success = false;
    QString notes;
};

Q_DECLARE_METATYPE(WfMaintenanceTask)
Q_DECLARE_METATYPE(WfMaintenanceRecord)

class WorkflowMaintenanceService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowMaintenanceService(QObject* parent = nullptr);

    bool scheduleMaintenance(const WfMaintenanceTask& task);
    bool executeMaintenance(int taskId);
    QVector<WfMaintenanceRecord> maintenanceHistory() const;
    QVector<WfMaintenanceTask> maintenanceSchedule() const;

signals:
    void maintenanceScheduled(const WfMaintenanceTask& task);
    void maintenanceCompleted(const WfMaintenanceRecord& record);

private:
    QVector<WfMaintenanceTask> tasks_;
    QVector<WfMaintenanceRecord> history_;
    int nextTaskId_ = 1;
};
