#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QPushButton;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class MonitoringDashboardPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit MonitoringDashboardPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct MonitoringMetric {
    QString name;
    double value;
    double threshold;
    QString status;
  };

  struct MonitoringAlert {
    QDateTime timestamp;
    QString severity;
    QString message;
    bool acknowledged;
  };

  struct MonitoringEvent {
    QDateTime timestamp;
    QString category;
    QString description;
    QString level;
  };

  struct MonitoringDashboard {
    QString name;
    QVector<MonitoringMetric> metrics;
    QDateTime lastUpdate;
  };

  void addMetric(const MonitoringMetric &metric);
  void updateMetric(int index, double value);
  int metricCount() const;
  QVector<MonitoringMetric> metrics() const;

  void addAlert(const MonitoringAlert &alert);
  void acknowledgeAlert(int index);
  int alertCount() const;
  QVector<MonitoringAlert> alerts() const;

  void addEvent(const MonitoringEvent &event);
  int eventCount() const;
  QVector<MonitoringEvent> events() const;

  void addDashboard(const MonitoringDashboard &dashboard);
  int dashboardCount() const;
  QVector<MonitoringDashboard> dashboards() const;

  QTableWidget *metricsTable() const;
  QTableWidget *alertsTable() const;
  QTableWidget *eventsTable() const;
  QTableWidget *dashboardsTable() const;
  QTextEdit *reportView() const;
  QLabel *statusLabel() const;

  QString exportReport() const;
  int activeAlertCount() const;

signals:
  void metricUpdated(const QString &name, double value);
  void alertAdded(const QString &severity, const QString &message);
  void alertAcknowledged(int index);

public slots:
  void refresh();

private:
  void buildUi();
  void rebuildMetricsTable();
  void rebuildAlertsTable();
  void rebuildEventsTable();
  void rebuildDashboardsTable();
  void rebuildReportView();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QTableWidget *metricsTable_ = nullptr;
  QTableWidget *alertsTable_ = nullptr;
  QTableWidget *eventsTable_ = nullptr;
  QTableWidget *dashboardsTable_ = nullptr;
  QTextEdit *reportView_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<MonitoringMetric> metrics_;
  QVector<MonitoringAlert> alerts_;
  QVector<MonitoringEvent> events_;
  QVector<MonitoringDashboard> dashboards_;
};
