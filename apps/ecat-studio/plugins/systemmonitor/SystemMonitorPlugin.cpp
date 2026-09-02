#include "SystemMonitorPlugin.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

SystemMonitorPlugin::SystemMonitorPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    alerts_ = {
        {"CPU", 90.0, ">", "CPU usage critical", false},
        {"Memory", 85.0, ">", "Memory usage high", false},
        {"Disk", 95.0, ">", "Disk usage critical", false},
        {"Network", 900.0, ">", "Network bandwidth saturated", false},
    };
    buildUi();
}

QString SystemMonitorPlugin::id() const {
    return "systemmonitor";
}
QString SystemMonitorPlugin::displayName() const {
    return "System Monitor";
}
QString SystemMonitorPlugin::displayNameZh() const {
    return "系统监视器";
}
int SystemMonitorPlugin::defaultOrder() const {
    return 275;
}
bool SystemMonitorPlugin::visible() const {
    return false;
}

void SystemMonitorPlugin::activate() {}
void SystemMonitorPlugin::deactivate() {}

QWidget* SystemMonitorPlugin::widget() {
    if (!containerWidget_)
        buildUi();
    return containerWidget_;
}

void SystemMonitorPlugin::updateCpuUsage(double percent) {
    cpuUsage_ = percent;
    recordSample(cpuHistory_, percent);
    emit usageUpdated("CPU", percent);
    rebuildOverviewTable();
}

void SystemMonitorPlugin::updateMemoryUsage(double percent) {
    memoryUsage_ = percent;
    recordSample(memoryHistory_, percent);
    emit usageUpdated("Memory", percent);
    rebuildOverviewTable();
}

void SystemMonitorPlugin::updateDiskUsage(double percent) {
    diskUsage_ = percent;
    recordSample(diskHistory_, percent);
    emit usageUpdated("Disk", percent);
    rebuildOverviewTable();
}

void SystemMonitorPlugin::updateNetworkUsage(double mbps) {
    networkUsage_ = mbps;
    recordSample(networkHistory_, mbps);
    emit usageUpdated("Network", mbps);
    rebuildOverviewTable();
}

double SystemMonitorPlugin::cpuUsage() const {
    return cpuUsage_;
}
double SystemMonitorPlugin::memoryUsage() const {
    return memoryUsage_;
}
double SystemMonitorPlugin::diskUsage() const {
    return diskUsage_;
}
double SystemMonitorPlugin::networkUsage() const {
    return networkUsage_;
}

QVector<SystemMonitorPlugin::UsageSample> SystemMonitorPlugin::cpuHistory() const {
    return cpuHistory_;
}
QVector<SystemMonitorPlugin::UsageSample> SystemMonitorPlugin::memoryHistory() const {
    return memoryHistory_;
}
QVector<SystemMonitorPlugin::UsageSample> SystemMonitorPlugin::diskHistory() const {
    return diskHistory_;
}
QVector<SystemMonitorPlugin::UsageSample> SystemMonitorPlugin::networkHistory() const {
    return networkHistory_;
}

void SystemMonitorPlugin::addAlert(const AlertRule& rule) {
    alerts_.append(rule);
    rebuildAlertTable();
}

void SystemMonitorPlugin::removeAlert(int index) {
    if (index >= 0 && index < alerts_.size()) {
        alerts_.removeAt(index);
        rebuildAlertTable();
    }
}

int SystemMonitorPlugin::alertCount() const {
    return alerts_.size();
}

int SystemMonitorPlugin::triggeredAlertCount() const {
    int count = 0;
    for (const auto& a : alerts_) {
        if (a.triggered)
            ++count;
    }
    return count;
}

void SystemMonitorPlugin::checkAlerts() {
    for (auto& a : alerts_) {
        double value = 0.0;
        if (a.metric == "CPU")
            value = cpuUsage_;
        else if (a.metric == "Memory")
            value = memoryUsage_;
        else if (a.metric == "Disk")
            value = diskUsage_;
        else if (a.metric == "Network")
            value = networkUsage_;

        bool wasTriggered = a.triggered;
        if (a.condition == ">")
            a.triggered = value > a.threshold;
        else if (a.condition == "<")
            a.triggered = value < a.threshold;
        else if (a.condition == ">=")
            a.triggered = value >= a.threshold;
        else if (a.condition == "<=")
            a.triggered = value <= a.threshold;

        if (a.triggered && !wasTriggered) {
            emit alertTriggered(a.metric, value, a.threshold);
        }
    }
    rebuildAlertTable();
}

QTableWidget* SystemMonitorPlugin::overviewTable() const {
    return overviewTable_;
}
QTableWidget* SystemMonitorPlugin::alertTable() const {
    return alertTable_;
}
QTextEdit* SystemMonitorPlugin::historyView() const {
    return historyView_;
}
QLabel* SystemMonitorPlugin::statusLabel() const {
    return statusLabel_;
}

void SystemMonitorPlugin::refresh() {
    rebuildOverviewTable();
    rebuildHistoryView();
    checkAlerts();
    if (statusLabel_) {
        statusLabel_->setText(QString("CPU: %1% | Mem: %2% | Disk: %3% | Net: %4 Mbps")
                                  .arg(cpuUsage_, 0, 'f', 1)
                                  .arg(memoryUsage_, 0, 'f', 1)
                                  .arg(diskUsage_, 0, 'f', 1)
                                  .arg(networkUsage_, 0, 'f', 1));
    }
}

void SystemMonitorPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);

    auto* controlRow = new QWidget;
    auto* controlLayout = new QHBoxLayout(controlRow);
    refreshBtn_ = new QPushButton("Refresh");
    addAlertBtn_ = new QPushButton("Add Alert");
    removeAlertBtn_ = new QPushButton("Remove Alert");
    controlLayout->addWidget(refreshBtn_);
    controlLayout->addWidget(addAlertBtn_);
    controlLayout->addWidget(removeAlertBtn_);
    mainLayout->addWidget(controlRow);

    tabs_ = new QTabWidget;

    overviewTable_ = new QTableWidget;
    overviewTable_->setColumnCount(4);
    overviewTable_->setHorizontalHeaderLabels({"Metric", "Current", "History Samples", "Status"});
    tabs_->addTab(overviewTable_, "Overview");

    historyView_ = new QTextEdit;
    historyView_->setReadOnly(true);
    tabs_->addTab(historyView_, "History");

    alertTable_ = new QTableWidget;
    alertTable_->setColumnCount(5);
    alertTable_->setHorizontalHeaderLabels({"Metric", "Condition", "Threshold", "Message", "Triggered"});
    tabs_->addTab(alertTable_, "Alerts");

    mainLayout->addWidget(tabs_);

    statusLabel_ = new QLabel("Ready");
    mainLayout->addWidget(statusLabel_);

    rebuildOverviewTable();
    rebuildAlertTable();

    connect(refreshBtn_, &QPushButton::clicked, this, [this]() { refresh(); });
    connect(addAlertBtn_, &QPushButton::clicked, this,
            [this]() { addAlert({"Custom", 50.0, ">", "Custom alert", false}); });
    connect(removeAlertBtn_, &QPushButton::clicked, this, [this]() {
        int row = alertTable_->currentRow();
        if (row >= 0)
            removeAlert(row);
    });
}

void SystemMonitorPlugin::rebuildOverviewTable() {
    if (!overviewTable_)
        return;
    overviewTable_->setRowCount(4);
    auto makeRow = [](int row, const QString& metric, double value, int histSize, const QString& status) {
        return QList<QString>{metric, QString::number(value, 'f', 1) + "%", QString::number(histSize), status};
    };
    auto setRow = [this](int row, const QList<QString>& cols) {
        for (int c = 0; c < cols.size(); ++c) {
            overviewTable_->setItem(row, c, new QTableWidgetItem(cols[c]));
        }
    };
    setRow(0, makeRow(0, "CPU", cpuUsage_, cpuHistory_.size(),
                      cpuUsage_ > 90   ? "CRITICAL"
                      : cpuUsage_ > 70 ? "WARNING"
                                       : "OK"));
    setRow(1, makeRow(1, "Memory", memoryUsage_, memoryHistory_.size(),
                      memoryUsage_ > 85   ? "CRITICAL"
                      : memoryUsage_ > 70 ? "WARNING"
                                          : "OK"));
    setRow(2, makeRow(2, "Disk", diskUsage_, diskHistory_.size(),
                      diskUsage_ > 95   ? "CRITICAL"
                      : diskUsage_ > 80 ? "WARNING"
                                        : "OK"));
    overviewTable_->setItem(3, 0, new QTableWidgetItem("Network"));
    overviewTable_->setItem(3, 1, new QTableWidgetItem(QString::number(networkUsage_, 'f', 1) + " Mbps"));
    overviewTable_->setItem(3, 2, new QTableWidgetItem(QString::number(networkHistory_.size())));
    overviewTable_->setItem(3, 3, new QTableWidgetItem(networkUsage_ > 900 ? "SATURATED" : "OK"));
}

void SystemMonitorPlugin::rebuildAlertTable() {
    if (!alertTable_)
        return;
    alertTable_->setRowCount(alerts_.size());
    for (int i = 0; i < alerts_.size(); ++i) {
        const auto& a = alerts_[i];
        alertTable_->setItem(i, 0, new QTableWidgetItem(a.metric));
        alertTable_->setItem(i, 1, new QTableWidgetItem(a.condition));
        alertTable_->setItem(i, 2, new QTableWidgetItem(QString::number(a.threshold)));
        alertTable_->setItem(i, 3, new QTableWidgetItem(a.message));
        alertTable_->setItem(i, 4, new QTableWidgetItem(a.triggered ? "YES" : "no"));
    }
}

void SystemMonitorPlugin::rebuildHistoryView() {
    if (!historyView_)
        return;
    QString text;
    auto appendHistory = [&text](const QString& label, const QVector<UsageSample>& h) {
        text += "--- " + label + " (" + QString::number(h.size()) + " samples) ---\n";
        int start = qMax(0, h.size() - 10);
        for (int i = start; i < h.size(); ++i) {
            text += "  " + h[i].timestamp.toString(Qt::ISODate) + "  " + QString::number(h[i].value, 'f', 1) + "\n";
        }
        text += "\n";
    };
    appendHistory("CPU", cpuHistory_);
    appendHistory("Memory", memoryHistory_);
    appendHistory("Disk", diskHistory_);
    appendHistory("Network", networkHistory_);
    historyView_->setText(text);
}

void SystemMonitorPlugin::recordSample(QVector<UsageSample>& history, double value) {
    history.append({QDateTime::currentDateTime(), value});
    if (history.size() > kMaxHistorySize)
        history.removeFirst();
}
