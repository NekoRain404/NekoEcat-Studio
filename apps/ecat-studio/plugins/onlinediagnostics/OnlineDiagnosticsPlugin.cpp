#include "OnlineDiagnosticsPlugin.h"
#include "BusMonitorWidget.h"
#include "ErrorAnalyzerWidget.h"
#include "services/OnlineDiagnosticsService.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

OnlineDiagnosticsPlugin::OnlineDiagnosticsPlugin(OnlineDiagnosticsService* service, QObject* parent)
    : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &OnlineDiagnosticsService::trafficUpdated, this,
            [this](const BusTraffic& t) { busMonitor_->updateTraffic(t); });
    connect(service_, &OnlineDiagnosticsService::errorRateUpdated, this, [this](const ErrorRate& r) {
        busMonitor_->updateErrorRate(r);
        errorAnalyzer_->updateErrors(r);
    });
    connect(service_, &OnlineDiagnosticsService::performanceUpdated, this, [this](const PerformanceMetrics& m) {
        perfLabel_->setText(
            tr("Perf: %1 f/s | Cycle: %2 us").arg(m.pdoUpdateRate, 0, 'f', 1).arg(m.cycleTimeUs, 0, 'f', 1));
    });
    connect(service_, &OnlineDiagnosticsService::healthUpdated, this, [this](const HealthStatus& h) {
        QString color = h.grade == "Healthy"    ? "#4caf50"
                        : h.grade == "Warning"  ? "#ff9800"
                        : h.grade == "Critical" ? "#f44336"
                                                : "#9e9e9e";
        healthLabel_->setText(tr("Health: %1").arg(h.grade));
        healthLabel_->setStyleSheet(QStringLiteral("color: %1; font-weight: bold;").arg(color));
    });
}

QString OnlineDiagnosticsPlugin::id() const {
    return "onlinediagnostics";
}
QString OnlineDiagnosticsPlugin::displayName() const {
    return "Online Diagnostics";
}
QString OnlineDiagnosticsPlugin::displayNameZh() const {
    return QStringLiteral("在线诊断");
}
QIcon OnlineDiagnosticsPlugin::icon() const {
    return QIcon::fromTheme("utilities-system-monitor");
}
int OnlineDiagnosticsPlugin::defaultOrder() const {
    return 28;
}
bool OnlineDiagnosticsPlugin::visible() const {
    return true;
}

void OnlineDiagnosticsPlugin::activate() {}
void OnlineDiagnosticsPlugin::deactivate() {}

QWidget* OnlineDiagnosticsPlugin::widget() {
    return containerWidget_;
}

void OnlineDiagnosticsPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    auto* toolbar = new QHBoxLayout;
    toolbar->setSpacing(8);

    startStopBtn_ = new QPushButton(tr("Start Monitoring"));
    toolbar->addWidget(startStopBtn_);

    statusLabel_ = new QLabel(tr("Stopped"));
    toolbar->addWidget(statusLabel_);

    toolbar->addStretch();

    healthLabel_ = new QLabel(tr("Health: Unknown"));
    healthLabel_->setStyleSheet("color: #9e9e9e; font-weight: bold;");
    toolbar->addWidget(healthLabel_);

    perfLabel_ = new QLabel(tr("Perf: --"));
    toolbar->addWidget(perfLabel_);

    exportBtn_ = new QPushButton(tr("Export Report"));
    toolbar->addWidget(exportBtn_);

    mainLayout->addLayout(toolbar);

    tabWidget_ = new QTabWidget;

    busMonitor_ = new BusMonitorWidget;
    tabWidget_->addTab(busMonitor_, tr("Bus Monitor"));

    errorAnalyzer_ = new ErrorAnalyzerWidget;
    tabWidget_->addTab(errorAnalyzer_, tr("Error Analyzer"));

    mainLayout->addWidget(tabWidget_, 1);

    connect(startStopBtn_, &QPushButton::clicked, this, [this]() {
        if (service_->isMonitoring()) {
            service_->stopMonitoring();
            startStopBtn_->setText(tr("Start Monitoring"));
            statusLabel_->setText(tr("Stopped"));
        } else {
            service_->startMonitoring();
            startStopBtn_->setText(tr("Stop Monitoring"));
            statusLabel_->setText(tr("Running"));
        }
    });

    connect(exportBtn_, &QPushButton::clicked, this, &OnlineDiagnosticsPlugin::exportReport);
}

void OnlineDiagnosticsPlugin::exportReport() {
    QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Diagnostics Report"),
                                                "diagnostics_report.csv", tr("CSV Files (*.csv);;All Files (*)"));
    if (path.isEmpty())
        return;

    exportReportToFile(path);
}

bool OnlineDiagnosticsPlugin::exportReportToFile(const QString& path) {
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "Metric,Value\n";
    auto traffic = service_->busTraffic();
    auto err = service_->errorRate();
    auto perf = service_->performance();
    auto h = service_->health();

    out << "TX Frames," << traffic.txFrames << "\n";
    out << "RX Frames," << traffic.rxFrames << "\n";
    out << "TX Bytes," << traffic.txBytes << "\n";
    out << "RX Bytes," << traffic.rxBytes << "\n";
    out << "Bandwidth (Mbps)," << traffic.bandwidth << "\n";
    out << "Utilization (%)," << traffic.utilization << "\n";
    out << "Total Errors," << err.totalErrors << "\n";
    out << "CRC Errors," << err.crcErrors << "\n";
    out << "Lost Errors," << err.lostErrors << "\n";
    out << "Error Rate (/s)," << err.rate << "\n";
    out << "PDO Update Rate," << perf.pdoUpdateRate << "\n";
    out << "Cycle Time (us)," << perf.cycleTimeUs << "\n";
    out << "Jitter (us)," << perf.jitterUs << "\n";
    out << "Health Grade," << h.grade << "\n";
    out << "Health Score," << h.score << "\n";
    out << "Total Slaves," << h.totalSlaves << "\n";
    out << "OP Slaves," << h.opSlaves << "\n";
    return out.status() == QTextStream::Ok && file.flush();
}
