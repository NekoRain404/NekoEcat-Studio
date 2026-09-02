#include "WorkflowDashboardPlugin.h"
#include "services/WorkflowMonitoringService.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>

WorkflowDashboardPlugin::WorkflowDashboardPlugin(WorkflowMonitoringService* monitoring, QObject* parent)
    : monitoring_(monitoring) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString WorkflowDashboardPlugin::id() const {
    return "workflowdashboard";
}
QString WorkflowDashboardPlugin::displayName() const {
    return "Workflow Dashboard";
}
QString WorkflowDashboardPlugin::displayNameZh() const {
    return QStringLiteral("工作流仪表盘");
}
QIcon WorkflowDashboardPlugin::icon() const {
    return QIcon::fromTheme("utilities-system-monitor");
}
int WorkflowDashboardPlugin::defaultOrder() const {
    return 390;
}
bool WorkflowDashboardPlugin::visible() const {
    return false;
}

void WorkflowDashboardPlugin::activate() {}
void WorkflowDashboardPlugin::deactivate() {}

QWidget* WorkflowDashboardPlugin::widget() {
    return containerWidget_;
}

void WorkflowDashboardPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QHBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter;

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    auto* activeLabel = new QLabel(tr("Active Workflows"));
    leftLayout->addWidget(activeLabel);

    activeWorkflowsList_ = new QListWidget;
    leftLayout->addWidget(activeWorkflowsList_);

    auto* metricsLabel = new QLabel(tr("Workflow Metrics"));
    leftLayout->addWidget(metricsLabel);

    metricsTable_ = new QTableWidget(0, 3);
    metricsTable_->setHorizontalHeaderLabels({tr("Metric"), tr("Value"), tr("Status")});
    metricsTable_->horizontalHeader()->setStretchLastSection(true);
    leftLayout->addWidget(metricsTable_);

    splitter->addWidget(leftPanel);

    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(4, 4, 4, 4);

    auto* alertsLabel = new QLabel(tr("Alerts"));
    rightLayout->addWidget(alertsLabel);

    alertsTable_ = new QTableWidget(0, 4);
    alertsTable_->setHorizontalHeaderLabels({tr("Severity"), tr("Source"), tr("Message"), tr("Time")});
    alertsTable_->horizontalHeader()->setStretchLastSection(true);
    rightLayout->addWidget(alertsTable_);

    auto* notifLabel = new QLabel(tr("Notifications"));
    rightLayout->addWidget(notifLabel);

    notificationsTable_ = new QTableWidget(0, 3);
    notificationsTable_->setHorizontalHeaderLabels({tr("Channel"), tr("Message"), tr("Time")});
    notificationsTable_->horizontalHeader()->setStretchLastSection(true);
    rightLayout->addWidget(notificationsTable_);

    auto* buttonRow = new QHBoxLayout;
    refreshButton_ = new QPushButton(tr("Refresh"));
    buttonRow->addWidget(refreshButton_);
    exportButton_ = new QPushButton(tr("Export Dashboard"));
    buttonRow->addWidget(exportButton_);
    statusLabel_ = new QLabel(tr("Ready"));
    buttonRow->addWidget(statusLabel_);
    rightLayout->addLayout(buttonRow);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    connect(refreshButton_, &QPushButton::clicked, this, &WorkflowDashboardPlugin::refreshActiveWorkflows);
    connect(exportButton_, &QPushButton::clicked, this, [this]() {
        QString path =
            QFileDialog::getSaveFileName(containerWidget_, tr("Export Dashboard"), QString(), "JSON (*.json)");
        if (!path.isEmpty())
            exportDashboard(path);
    });

    connect(activeWorkflowsList_, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row >= 0 && row < activeWorkflows_.size())
            emit workflowActivated(activeWorkflows_[row].id);
    });
}

void WorkflowDashboardPlugin::addActiveWorkflow(const QString& workflowId, const QString& name, const QString& status) {
    ActiveWorkflow entry;
    entry.id = workflowId;
    entry.name = name;
    entry.status = status;
    activeWorkflows_.append(entry);
    refreshActiveWorkflows();
}

void WorkflowDashboardPlugin::updateWorkflowStatus(const QString& workflowId, const QString& status) {
    for (auto& wf : activeWorkflows_) {
        if (wf.id == workflowId) {
            wf.status = status;
            break;
        }
    }
    refreshActiveWorkflows();
}

void WorkflowDashboardPlugin::removeActiveWorkflow(const QString& workflowId) {
    for (int i = 0; i < activeWorkflows_.size(); ++i) {
        if (activeWorkflows_[i].id == workflowId) {
            activeWorkflows_.removeAt(i);
            break;
        }
    }
    refreshActiveWorkflows();
}

int WorkflowDashboardPlugin::activeWorkflowCount() const {
    return activeWorkflows_.size();
}

void WorkflowDashboardPlugin::addAlert(const QString& severity, const QString& source, const QString& message) {
    Alert alert;
    alert.severity = severity;
    alert.source = source;
    alert.message = message;
    alert.timestamp = QDateTime::currentDateTime();
    alerts_.append(alert);

    int row = alertsTable_->rowCount();
    alertsTable_->insertRow(row);
    alertsTable_->setItem(row, 0, new QTableWidgetItem(severity));
    alertsTable_->setItem(row, 1, new QTableWidgetItem(source));
    alertsTable_->setItem(row, 2, new QTableWidgetItem(message));
    alertsTable_->setItem(row, 3, new QTableWidgetItem(alert.timestamp.toString(Qt::ISODate)));
}

int WorkflowDashboardPlugin::alertCount() const {
    return alerts_.size();
}

void WorkflowDashboardPlugin::addNotification(const QString& channel, const QString& message) {
    Notification notif;
    notif.channel = channel;
    notif.message = message;
    notif.timestamp = QDateTime::currentDateTime();
    notifications_.append(notif);

    int row = notificationsTable_->rowCount();
    notificationsTable_->insertRow(row);
    notificationsTable_->setItem(row, 0, new QTableWidgetItem(channel));
    notificationsTable_->setItem(row, 1, new QTableWidgetItem(message));
    notificationsTable_->setItem(row, 2, new QTableWidgetItem(notif.timestamp.toString(Qt::ISODate)));
}

int WorkflowDashboardPlugin::notificationCount() const {
    return notifications_.size();
}

void WorkflowDashboardPlugin::refreshActiveWorkflows() {
    activeWorkflowsList_->clear();
    for (const auto& wf : activeWorkflows_) {
        activeWorkflowsList_->addItem(QStringLiteral("[%1] %2 — %3").arg(wf.id, wf.name, wf.status));
    }
}

bool WorkflowDashboardPlugin::exportDashboard(const QString& filePath) {
    if (filePath.isEmpty())
        return false;

    QJsonObject root;
    root[QStringLiteral("version")] = 1;
    root[QStringLiteral("exportTime")] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonArray activeArray;
    for (const auto& wf : activeWorkflows_) {
        QJsonObject wfObj;
        wfObj[QStringLiteral("id")] = wf.id;
        wfObj[QStringLiteral("name")] = wf.name;
        wfObj[QStringLiteral("status")] = wf.status;
        activeArray.append(wfObj);
    }
    root[QStringLiteral("activeWorkflows")] = activeArray;

    QJsonArray alertsArray;
    for (const auto& a : alerts_) {
        QJsonObject aObj;
        aObj[QStringLiteral("severity")] = a.severity;
        aObj[QStringLiteral("source")] = a.source;
        aObj[QStringLiteral("message")] = a.message;
        aObj[QStringLiteral("timestamp")] = a.timestamp.toString(Qt::ISODate);
        alertsArray.append(aObj);
    }
    root[QStringLiteral("alerts")] = alertsArray;

    QJsonArray notifsArray;
    for (const auto& n : notifications_) {
        QJsonObject nObj;
        nObj[QStringLiteral("channel")] = n.channel;
        nObj[QStringLiteral("message")] = n.message;
        nObj[QStringLiteral("timestamp")] = n.timestamp.toString(Qt::ISODate);
        notifsArray.append(nObj);
    }
    root[QStringLiteral("notifications")] = notifsArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    const QByteArray bytes = QJsonDocument(root).toJson();
    if (file.write(bytes) != bytes.size() || !file.flush())
        return false;

    statusLabel_->setText(tr("Dashboard exported"));
    emit dashboardExported(filePath);
    return true;
}
