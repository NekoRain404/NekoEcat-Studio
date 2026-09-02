#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;

struct MaintenanceTask {
    QString id;
    QString name;
    QString description;
    QString schedule;
    QString priority;
    QString status;
};

struct MaintenanceRecord {
    QString id;
    QString taskName;
    QString status;
    QString timestamp;
    QString notes;
};

class MaintenanceSchedulerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit MaintenanceSchedulerPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    QTableWidget* taskTable() const;
    QTableWidget* scheduleTable() const;
    QTableWidget* historyTable() const;
    QTextEdit* reportPanel() const;
    QLabel* statusLabel() const;

    int taskCount() const;
    int scheduleCount() const;
    int historyCount() const;

    void addTask(const MaintenanceTask& task);
    void updateTask(int index, const MaintenanceTask& task);
    void removeTask(int index);

    void addScheduleEntry(const QString& taskName, const QString& dateTime);
    void removeScheduleEntry(int index);

    bool recordMaintenance(const MaintenanceRecord& record);
    void clearHistory();

    void generateReport();
    bool exportReport(const QString& filePath);

signals:
    void taskAdded(const QString& taskId, const QString& name);
    void taskUpdated(const QString& taskId);
    void taskRemoved(const QString& taskId);
    void maintenanceRecorded(const QString& recordId);
    void reminderTriggered(const QString& taskName);

private:
    void buildUi();
    void refreshReport();

    QWidget* container_ = nullptr;
    QTableWidget* taskTable_ = nullptr;
    QTableWidget* scheduleTable_ = nullptr;
    QTableWidget* historyTable_ = nullptr;
    QTextEdit* reportPanel_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QPushButton* addTaskBtn_ = nullptr;
    QPushButton* removeTaskBtn_ = nullptr;
    QPushButton* recordBtn_ = nullptr;
    QPushButton* reportBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;

    QVector<MaintenanceTask> tasks_;
    QVector<MaintenanceRecord> history_;
    int nextId_ = 1;
};
