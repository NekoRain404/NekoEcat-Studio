// RealtimePerformancePlugin — workspace plugin for real-time EtherCAT performance.
//
// Provides a tabbed dashboard with latency monitoring, throughput monitoring,
// resource monitoring, and quality assessment. Follows the same pattern as
// OnlineDiagnosticsPlugin and DcSyncPrecisionPlugin.

#include "RealtimePerformancePlugin.h"
#include "LatencyMonitorWidget.h"
#include "services/RealtimePerformanceService.h"
#include "ThroughputMonitorWidget.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

RealtimePerformancePlugin::RealtimePerformancePlugin(RealtimePerformanceService* service, QObject* parent)
    : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &RealtimePerformanceService::latencyUpdated, this, [this](const LatencyMetrics& m) {
        latencyMonitor_->updateMetrics(m);
        latencyAvgLabel_->setText(tr("Avg: %1 us").arg(m.avgUs, 0, 'f', 1));
    });

    connect(service_, &RealtimePerformanceService::throughputUpdated, this, [this](const ThroughputMetrics& m) {
        throughputMonitor_->updateMetrics(m);
        throughputLabel_->setText(tr("%1 f/s").arg(m.framesPerSecond, 0, 'f', 0));
    });

    connect(service_, &RealtimePerformanceService::resourceUpdated, this, [this](const ResourceMetrics& m) {
        cpuLabel_->setText(tr("CPU: %1%").arg(m.cpuPercent, 0, 'f', 1));
        memLabel_->setText(tr("Memory: %1 MB").arg(m.memoryMB, 0, 'f', 0));
        threadLabel_->setText(tr("Threads: %1").arg(m.threadCount));
        socketLabel_->setText(tr("Sockets: %1").arg(m.socketCount));
        filesLabel_->setText(tr("Open Files: %1%").arg(m.openFilesPercent, 0, 'f', 0));
    });

    connect(service_, &RealtimePerformanceService::qualityUpdated, this, [this](const QualityAssessment& q) {
        qualityScoreLabel_->setText(tr("Score: %1").arg(q.score, 0, 'f', 0));
        qualityLabel_->setText(tr("Quality: %1").arg(q.grade));
        QString color = q.grade == "Excellent" ? "#22c55e"
                        : q.grade == "Good"    ? "#60a5fa"
                        : q.grade == "Fair"    ? "#f59e0b"
                                               : "#ef4444";
        qualityLabel_->setStyleSheet(QStringLiteral("color: %1; font-weight: bold;").arg(color));
    });
}

QString RealtimePerformancePlugin::id() const {
    return "realtimeperf";
}
QString RealtimePerformancePlugin::displayName() const {
    return "Real-time Performance";
}
QString RealtimePerformancePlugin::displayNameZh() const {
    return QStringLiteral("实时性能");
}
QIcon RealtimePerformancePlugin::icon() const {
    return QIcon::fromTheme("utilities-system-monitor");
}
int RealtimePerformancePlugin::defaultOrder() const {
    return 32;
}
bool RealtimePerformancePlugin::visible() const {
    return false;
}

void RealtimePerformancePlugin::activate() {}
void RealtimePerformancePlugin::deactivate() {}

QWidget* RealtimePerformancePlugin::widget() {
    return containerWidget_;
}

void RealtimePerformancePlugin::buildUi() {
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

    qualityLabel_ = new QLabel(tr("Quality: Unknown"));
    qualityLabel_->setStyleSheet("color: #9e9e9e; font-weight: bold;");
    toolbar->addWidget(qualityLabel_);

    qualityScoreLabel_ = new QLabel(tr("Score: --"));
    toolbar->addWidget(qualityScoreLabel_);

    exportBtn_ = new QPushButton(tr("Export Report"));
    toolbar->addWidget(exportBtn_);

    mainLayout->addLayout(toolbar);

    tabWidget_ = new QTabWidget;

    buildDashboardTab();
    buildLatencyTab();
    buildThroughputTab();
    buildResourceTab();

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

    connect(exportBtn_, &QPushButton::clicked, this, &RealtimePerformancePlugin::exportReport);
}

void RealtimePerformancePlugin::buildDashboardTab() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* summaryGroup = new QGroupBox(tr("Performance Summary"));
    auto* grid = new QGridLayout(summaryGroup);

    auto makeCard = [&](const QString& name, int row, int col) -> QLabel* {
        auto* lbl = new QLabel(name);
        lbl->setStyleSheet("color: #8888aa; font-size: 11px;");
        grid->addWidget(lbl, row * 2, col);
        auto* val = new QLabel("--");
        val->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 18px;");
        grid->addWidget(val, row * 2 + 1, col);
        return val;
    };

    latencyAvgLabel_ = makeCard(tr("Latency (avg)"), 0, 0);
    throughputLabel_ = makeCard(tr("Throughput"), 0, 1);
    cpuLabel_ = makeCard(tr("CPU Usage"), 0, 2);

    layout->addWidget(summaryGroup);
    layout->addStretch();

    tabWidget_->addTab(widget, tr("Dashboard"));
}

void RealtimePerformancePlugin::buildLatencyTab() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* configLayout = new QHBoxLayout;
    configLayout->addWidget(new QLabel(tr("Threshold (us):")));
    thresholdSpin_ = new QDoubleSpinBox;
    thresholdSpin_->setRange(10.0, 100000.0);
    thresholdSpin_->setValue(1000.0);
    thresholdSpin_->setSuffix(" us");
    configLayout->addWidget(thresholdSpin_);

    configLayout->addWidget(new QLabel(tr("History Window:")));
    historyWindowSpin_ = new QSpinBox;
    historyWindowSpin_->setRange(10, 2000);
    historyWindowSpin_->setValue(200);
    configLayout->addWidget(historyWindowSpin_);

    configLayout->addStretch();
    layout->addLayout(configLayout);

    latencyMonitor_ = new LatencyMonitorWidget;
    layout->addWidget(latencyMonitor_, 1);

    connect(thresholdSpin_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        service_->setLatencyThreshold(v);
        latencyMonitor_->setThreshold(v);
    });

    connect(historyWindowSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        service_->setHistoryWindowSize(v);
        latencyMonitor_->setHistorySize(v);
    });

    tabWidget_->addTab(widget, tr("Latency"));
}

void RealtimePerformancePlugin::buildThroughputTab() {
    throughputMonitor_ = new ThroughputMonitorWidget;
    tabWidget_->addTab(throughputMonitor_, tr("Throughput"));
}

void RealtimePerformancePlugin::buildResourceTab() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);

    auto* group = new QGroupBox(tr("System Resources"));
    auto* form = new QFormLayout(group);

    cpuLabel_ = new QLabel("--");
    cpuLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("CPU Usage:"), cpuLabel_);

    memLabel_ = new QLabel("--");
    memLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("Memory:"), memLabel_);

    threadLabel_ = new QLabel("--");
    threadLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("Threads:"), threadLabel_);

    socketLabel_ = new QLabel("--");
    socketLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("Sockets:"), socketLabel_);

    filesLabel_ = new QLabel("--");
    filesLabel_->setStyleSheet("color: #cccccc;");
    form->addRow(tr("Open Files:"), filesLabel_);

    layout->addWidget(group);
    layout->addStretch();

    tabWidget_->addTab(widget, tr("Resources"));
}

void RealtimePerformancePlugin::exportReport() {
    QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Performance Report"),
                                                "performance_report.csv", tr("CSV Files (*.csv);;All Files (*)"));
    if (path.isEmpty())
        return;

    exportReportToFile(path);
}

bool RealtimePerformancePlugin::exportReportToFile(const QString& path) {
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "Metric,Value\n";

    auto lat = service_->latency();
    out << "Latency Min (us)," << lat.minUs << "\n";
    out << "Latency Max (us)," << lat.maxUs << "\n";
    out << "Latency Avg (us)," << lat.avgUs << "\n";
    out << "Latency Stddev (us)," << lat.stddevUs << "\n";
    out << "Latency Samples," << lat.sampleCount << "\n";

    auto thr = service_->throughput();
    out << "Frame Rate (f/s)," << thr.framesPerSecond << "\n";
    out << "Byte Rate (B/s)," << thr.bytesPerSecond << "\n";
    out << "Error Rate (/s)," << thr.errorRate << "\n";
    out << "Utilization (%)," << thr.utilizationPercent << "\n";
    out << "Total Frames," << thr.totalFrames << "\n";
    out << "Total Bytes," << thr.totalBytes << "\n";
    out << "Total Errors," << thr.totalErrors << "\n";

    auto res = service_->resources();
    out << "CPU (%)," << res.cpuPercent << "\n";
    out << "Memory (MB)," << res.memoryMB << "\n";
    out << "Threads," << res.threadCount << "\n";
    out << "Sockets," << res.socketCount << "\n";
    out << "Open Files (%)," << res.openFilesPercent << "\n";

    auto q = service_->quality();
    out << "Quality Score," << q.score << "\n";
    out << "Quality Grade," << q.grade << "\n";
    out << "Jitter (us)," << q.jitterUs << "\n";
    out << "Packet Loss (%)," << q.packetLossPercent << "\n";
    out << "Consecutive Errors," << q.consecutiveErrors << "\n";
    return out.status() == QTextStream::Ok && file.flush();
}
