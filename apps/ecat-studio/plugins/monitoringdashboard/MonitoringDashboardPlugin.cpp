#include "MonitoringDashboardPlugin.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

MonitoringDashboardPlugin::MonitoringDashboardPlugin(QObject* parent) : WorkspacePlugin() {}

QString MonitoringDashboardPlugin::id() const {
    return QStringLiteral("monitoring_dashboard");
}

QString MonitoringDashboardPlugin::displayName() const {
    return QStringLiteral("Monitoring Dashboard");
}

QString MonitoringDashboardPlugin::displayNameZh() const {
    return QStringLiteral("监控仪表盘");
}

QWidget* MonitoringDashboardPlugin::widget() {
    if (!containerWidget_)
        buildUi();
    return containerWidget_;
}

int MonitoringDashboardPlugin::defaultOrder() const {
    return 91;
}

bool MonitoringDashboardPlugin::visible() const {
    return false;
}

void MonitoringDashboardPlugin::activate() {}

void MonitoringDashboardPlugin::deactivate() {}

void MonitoringDashboardPlugin::addMetric(const MonitoringMetric& metric) {
    metrics_.append(metric);
    rebuildMetricsTable();
}

void MonitoringDashboardPlugin::updateMetric(int index, double value) {
    if (index >= 0 && index < metrics_.size()) {
        metrics_[index].value = value;
        metrics_[index].status = value > metrics_[index].threshold ? QStringLiteral("ALERT") : QStringLiteral("OK");
        emit metricUpdated(metrics_[index].name, value);
        rebuildMetricsTable();
    }
}

int MonitoringDashboardPlugin::metricCount() const {
    return metrics_.size();
}

QVector<MonitoringDashboardPlugin::MonitoringMetric> MonitoringDashboardPlugin::metrics() const {
    return metrics_;
}

void MonitoringDashboardPlugin::addAlert(const MonitoringAlert& alert) {
    alerts_.append(alert);
    emit alertAdded(alert.severity, alert.message);
    rebuildAlertsTable();
}

void MonitoringDashboardPlugin::acknowledgeAlert(int index) {
    if (index >= 0 && index < alerts_.size()) {
        alerts_[index].acknowledged = true;
        emit alertAcknowledged(index);
        rebuildAlertsTable();
    }
}

int MonitoringDashboardPlugin::alertCount() const {
    return alerts_.size();
}

QVector<MonitoringDashboardPlugin::MonitoringAlert> MonitoringDashboardPlugin::alerts() const {
    return alerts_;
}

int MonitoringDashboardPlugin::activeAlertCount() const {
    int count = 0;
    for (const auto& a : alerts_) {
        if (!a.acknowledged)
            ++count;
    }
    return count;
}

void MonitoringDashboardPlugin::addEvent(const MonitoringEvent& event) {
    events_.append(event);
    rebuildEventsTable();
}

int MonitoringDashboardPlugin::eventCount() const {
    return events_.size();
}

QVector<MonitoringDashboardPlugin::MonitoringEvent> MonitoringDashboardPlugin::events() const {
    return events_;
}

void MonitoringDashboardPlugin::addDashboard(const MonitoringDashboard& dashboard) {
    dashboards_.append(dashboard);
    rebuildDashboardsTable();
}

int MonitoringDashboardPlugin::dashboardCount() const {
    return dashboards_.size();
}

QVector<MonitoringDashboardPlugin::MonitoringDashboard> MonitoringDashboardPlugin::dashboards() const {
    return dashboards_;
}

QTableWidget* MonitoringDashboardPlugin::metricsTable() const {
    return metricsTable_;
}

QTableWidget* MonitoringDashboardPlugin::alertsTable() const {
    return alertsTable_;
}

QTableWidget* MonitoringDashboardPlugin::eventsTable() const {
    return eventsTable_;
}

QTableWidget* MonitoringDashboardPlugin::dashboardsTable() const {
    return dashboardsTable_;
}

QTextEdit* MonitoringDashboardPlugin::reportView() const {
    return reportView_;
}

QLabel* MonitoringDashboardPlugin::statusLabel() const {
    return statusLabel_;
}

QString MonitoringDashboardPlugin::exportReport() const {
    QString report;
    report += QStringLiteral("# Monitoring Report\n\n");
    report += QStringLiteral("## Metrics\n\n");
    for (const auto& m : metrics_) {
        report += QStringLiteral("- %1: %2 (threshold: %3, status: %4)\n")
                      .arg(m.name)
                      .arg(m.value)
                      .arg(m.threshold)
                      .arg(m.status);
    }
    report += QStringLiteral("\n## Active Alerts: %1\n\n").arg(activeAlertCount());
    for (const auto& a : alerts_) {
        if (!a.acknowledged) {
            report +=
                QStringLiteral("- [%1] %2: %3\n").arg(a.severity).arg(a.timestamp.toString(Qt::ISODate)).arg(a.message);
        }
    }
    report += QStringLiteral("\n## Recent Events\n\n");
    int start = qMax(0, events_.size() - 20);
    for (int i = start; i < events_.size(); ++i) {
        report += QStringLiteral("- [%1][%2] %3: %4\n")
                      .arg(events_[i].level, events_[i].category)
                      .arg(events_[i].timestamp.toString(Qt::ISODate))
                      .arg(events_[i].description);
    }
    return report;
}

void MonitoringDashboardPlugin::refresh() {
    rebuildMetricsTable();
    rebuildAlertsTable();
    rebuildEventsTable();
    rebuildDashboardsTable();
    rebuildReportView();
    if (statusLabel_)
        statusLabel_->setText(QStringLiteral("Last refreshed: %1 | Metrics: %2 | Active Alerts: %3")
                                  .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                                  .arg(metrics_.size())
                                  .arg(activeAlertCount()));
}

void MonitoringDashboardPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);

    statusLabel_ = new QLabel;
    mainLayout->addWidget(statusLabel_);

    tabs_ = new QTabWidget;
    mainLayout->addWidget(tabs_);

    metricsTable_ = new QTableWidget;
    metricsTable_->setColumnCount(4);
    metricsTable_->setHorizontalHeaderLabels({tr("Name"), tr("Value"), tr("Threshold"), tr("Status")});
    tabs_->addTab(metricsTable_, tr("Metrics"));

    alertsTable_ = new QTableWidget;
    alertsTable_->setColumnCount(4);
    alertsTable_->setHorizontalHeaderLabels({tr("Timestamp"), tr("Severity"), tr("Message"), tr("Acknowledged")});
    tabs_->addTab(alertsTable_, tr("Alerts"));

    eventsTable_ = new QTableWidget;
    eventsTable_->setColumnCount(4);
    eventsTable_->setHorizontalHeaderLabels({tr("Timestamp"), tr("Category"), tr("Description"), tr("Level")});
    tabs_->addTab(eventsTable_, tr("Events"));

    dashboardsTable_ = new QTableWidget;
    dashboardsTable_->setColumnCount(3);
    dashboardsTable_->setHorizontalHeaderLabels({tr("Name"), tr("Metrics"), tr("Last Update")});
    tabs_->addTab(dashboardsTable_, tr("Dashboards"));

    reportView_ = new QTextEdit;
    reportView_->setReadOnly(true);
    tabs_->addTab(reportView_, tr("Report"));

    auto* btnLayout = new QHBoxLayout;
    refreshBtn_ = new QPushButton(tr("Refresh"));
    exportBtn_ = new QPushButton(tr("Export"));
    btnLayout->addWidget(refreshBtn_);
    btnLayout->addWidget(exportBtn_);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(refreshBtn_, &QPushButton::clicked, this, &MonitoringDashboardPlugin::refresh);
    connect(exportBtn_, &QPushButton::clicked, this, [this]() {
        if (reportView_)
            reportView_->setText(exportReport());
    });

    refresh();
}

void MonitoringDashboardPlugin::rebuildMetricsTable() {
    if (!metricsTable_)
        return;
    metricsTable_->setRowCount(metrics_.size());
    for (int i = 0; i < metrics_.size(); ++i) {
        metricsTable_->setItem(i, 0, new QTableWidgetItem(metrics_[i].name));
        metricsTable_->setItem(i, 1, new QTableWidgetItem(QString::number(metrics_[i].value, 'f', 2)));
        metricsTable_->setItem(i, 2, new QTableWidgetItem(QString::number(metrics_[i].threshold, 'f', 2)));
        metricsTable_->setItem(i, 3, new QTableWidgetItem(metrics_[i].status));
    }
}

void MonitoringDashboardPlugin::rebuildAlertsTable() {
    if (!alertsTable_)
        return;
    alertsTable_->setRowCount(alerts_.size());
    for (int i = 0; i < alerts_.size(); ++i) {
        alertsTable_->setItem(i, 0, new QTableWidgetItem(alerts_[i].timestamp.toString(Qt::ISODate)));
        alertsTable_->setItem(i, 1, new QTableWidgetItem(alerts_[i].severity));
        alertsTable_->setItem(i, 2, new QTableWidgetItem(alerts_[i].message));
        alertsTable_->setItem(i, 3, new QTableWidgetItem(alerts_[i].acknowledged ? tr("Yes") : tr("No")));
    }
}

void MonitoringDashboardPlugin::rebuildEventsTable() {
    if (!eventsTable_)
        return;
    eventsTable_->setRowCount(events_.size());
    for (int i = 0; i < events_.size(); ++i) {
        eventsTable_->setItem(i, 0, new QTableWidgetItem(events_[i].timestamp.toString(Qt::ISODate)));
        eventsTable_->setItem(i, 1, new QTableWidgetItem(events_[i].category));
        eventsTable_->setItem(i, 2, new QTableWidgetItem(events_[i].description));
        eventsTable_->setItem(i, 3, new QTableWidgetItem(events_[i].level));
    }
}

void MonitoringDashboardPlugin::rebuildDashboardsTable() {
    if (!dashboardsTable_)
        return;
    dashboardsTable_->setRowCount(dashboards_.size());
    for (int i = 0; i < dashboards_.size(); ++i) {
        dashboardsTable_->setItem(i, 0, new QTableWidgetItem(dashboards_[i].name));
        dashboardsTable_->setItem(i, 1, new QTableWidgetItem(QString::number(dashboards_[i].metrics.size())));
        dashboardsTable_->setItem(i, 2, new QTableWidgetItem(dashboards_[i].lastUpdate.toString(Qt::ISODate)));
    }
}

void MonitoringDashboardPlugin::rebuildReportView() {
    if (reportView_)
        reportView_->setText(exportReport());
}
