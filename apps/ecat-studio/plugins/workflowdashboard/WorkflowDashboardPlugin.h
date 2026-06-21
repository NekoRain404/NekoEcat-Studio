#pragma once

// WorkflowDashboardPlugin — workspace plugin for workflow dashboard,
// active workflows, metrics, alerts, and notifications.
//
// Default order: 390

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>

class WorkflowMonitoringService;
class QLabel;
class QListWidget;
class QPushButton;
class QTableWidget;

class WorkflowDashboardPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit WorkflowDashboardPlugin(WorkflowMonitoringService *monitoring,
                                   QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  void addActiveWorkflow(const QString &workflowId, const QString &name,
                         const QString &status);
  void updateWorkflowStatus(const QString &workflowId, const QString &status);
  void removeActiveWorkflow(const QString &workflowId);
  int activeWorkflowCount() const;

  void addAlert(const QString &severity, const QString &source,
                const QString &message);
  int alertCount() const;

  void addNotification(const QString &channel, const QString &message);
  int notificationCount() const;

  bool exportDashboard(const QString &filePath);

signals:
  void workflowActivated(const QString &workflowId);
  void alertAcknowledged(int index);
  void dashboardExported(const QString &filePath);

private:
  void buildUi();
  void refreshActiveWorkflows();

  WorkflowMonitoringService *monitoring_;
  QWidget *containerWidget_ = nullptr;
  QListWidget *activeWorkflowsList_ = nullptr;
  QTableWidget *metricsTable_ = nullptr;
  QTableWidget *alertsTable_ = nullptr;
  QTableWidget *notificationsTable_ = nullptr;
  QPushButton *refreshButton_ = nullptr;
  QPushButton *exportButton_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  struct ActiveWorkflow {
    QString id;
    QString name;
    QString status;
  };
  QVector<ActiveWorkflow> activeWorkflows_;

  struct Alert {
    QString severity;
    QString source;
    QString message;
    QDateTime timestamp;
  };
  QVector<Alert> alerts_;

  struct Notification {
    QString channel;
    QString message;
    QDateTime timestamp;
  };
  QVector<Notification> notifications_;
};
