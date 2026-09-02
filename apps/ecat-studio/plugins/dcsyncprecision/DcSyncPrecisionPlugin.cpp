#include "DcSyncPrecisionPlugin.h"
#include "DriftMonitorWidget.h"
#include "infra/EcatClient.h"
#include "JitterAnalysisWidget.h"
#include "services/DcSyncPrecisionService.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

DcSyncPrecisionPlugin::DcSyncPrecisionPlugin(EcatClient* client, EventBus* bus, QObject* parent) {
    if (parent)
        setParent(parent);

    service_ = new DcSyncPrecisionService(client, bus, this);
    buildUi();

    connect(service_, &DcSyncPrecisionService::driftUpdated, this, &DcSyncPrecisionPlugin::handleDriftUpdated);
    connect(service_, &DcSyncPrecisionService::jitterUpdated, this, &DcSyncPrecisionPlugin::handleJitterUpdated);
    connect(service_, &DcSyncPrecisionService::syncQualityChanged, this,
            &DcSyncPrecisionPlugin::handleSyncQualityChanged);
    connect(service_, &DcSyncPrecisionService::monitoringStateChanged, this,
            &DcSyncPrecisionPlugin::handleMonitoringStateChanged);
}

QString DcSyncPrecisionPlugin::id() const {
    return "dcsyncprecision";
}
QString DcSyncPrecisionPlugin::displayName() const {
    return "DC Sync Precision";
}
QString DcSyncPrecisionPlugin::displayNameZh() const {
    return QStringLiteral("DC 同步精度");
}
int DcSyncPrecisionPlugin::defaultOrder() const {
    return 26;
}
bool DcSyncPrecisionPlugin::visible() const {
    return true;
}
QWidget* DcSyncPrecisionPlugin::widget() {
    return containerWidget_;
}

void DcSyncPrecisionPlugin::activate() {}
void DcSyncPrecisionPlugin::deactivate() {}
void DcSyncPrecisionPlugin::onConnectionChanged(bool connected) {
    if (!connected && service_->isMonitoring())
        service_->stopMonitoring();
}

void DcSyncPrecisionPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* toolbar = new QHBoxLayout;
    startStopBtn_ = new QPushButton(tr("Start Monitoring"));
    toolbar->addWidget(startStopBtn_);

    exportBtn_ = new QPushButton(tr("Export Report"));
    exportBtn_->setEnabled(false);
    toolbar->addWidget(exportBtn_);

    toolbar->addStretch();
    qualityLabel_ = new QLabel(tr("Quality: N/A"));
    toolbar->addWidget(qualityLabel_);
    qualityScoreLabel_ = new QLabel;
    toolbar->addWidget(qualityScoreLabel_);

    layout->addLayout(toolbar);

    tabs_ = new QTabWidget;
    tabs_->addTab(buildSyncStatusTab(), tr("Sync Status"));
    tabs_->addTab(buildDriftMonitorTab(), tr("Drift Monitor"));
    tabs_->addTab(buildJitterAnalysisTab(), tr("Jitter Analysis"));
    tabs_->addTab(buildConfigTab(), tr("Configuration"));
    layout->addWidget(tabs_);

    connect(startStopBtn_, &QPushButton::clicked, this, &DcSyncPrecisionPlugin::handleStartStop);
    connect(exportBtn_, &QPushButton::clicked, this, &DcSyncPrecisionPlugin::handleExport);
}

QWidget* DcSyncPrecisionPlugin::buildSyncStatusTab() {
    syncStatusWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(syncStatusWidget_);

    auto* infoRow = new QHBoxLayout;
    refClockLabel_ = new QLabel(tr("Reference Clock: N/A"));
    infoRow->addWidget(refClockLabel_);
    infoRow->addStretch();
    layout->addLayout(infoRow);

    syncTable_ = new QTableWidget;
    syncTable_->setColumnCount(6);
    syncTable_->setHorizontalHeaderLabels(
        {tr("Slave"), tr("Drift (ns)"), tr("Threshold (ns)"), tr("Status"), tr("Jitter Avg"), tr("Last Update")});
    syncTable_->horizontalHeader()->setStretchLastSection(true);
    syncTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    syncTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(syncTable_);

    return syncStatusWidget_;
}

QWidget* DcSyncPrecisionPlugin::buildDriftMonitorTab() {
    driftMonitor_ = new DriftMonitorWidget;
    return driftMonitor_;
}

QWidget* DcSyncPrecisionPlugin::buildJitterAnalysisTab() {
    jitterAnalysis_ = new JitterAnalysisWidget;
    return jitterAnalysis_;
}

QWidget* DcSyncPrecisionPlugin::buildConfigTab() {
    auto* configWidget = new QWidget;
    auto* layout = new QVBoxLayout(configWidget);

    auto* thresholdGroup = new QGroupBox(tr("Drift Threshold"));
    auto* threshLayout = new QFormLayout(thresholdGroup);
    thresholdSpin_ = new QDoubleSpinBox;
    thresholdSpin_->setRange(10.0, 100000.0);
    thresholdSpin_->setValue(1000.0);
    thresholdSpin_->setSuffix(" ns");
    thresholdSpin_->setSingleStep(100.0);
    threshLayout->addRow(tr("Threshold:"), thresholdSpin_);
    layout->addWidget(thresholdGroup);

    auto* historyGroup = new QGroupBox(tr("History Window"));
    auto* histLayout = new QFormLayout(historyGroup);
    historyWindowSpin_ = new QSpinBox;
    historyWindowSpin_->setRange(10, 5000);
    historyWindowSpin_->setValue(500);
    historyWindowSpin_->setSuffix(tr(" samples"));
    histLayout->addRow(tr("Window Size:"), historyWindowSpin_);
    layout->addWidget(historyGroup);

    auto* pollGroup = new QGroupBox(tr("Polling"));
    auto* pollLayout = new QFormLayout(pollGroup);
    pollIntervalCombo_ = new QComboBox;
    pollIntervalCombo_->addItem(tr("250 ms"), 250);
    pollIntervalCombo_->addItem(tr("500 ms"), 500);
    pollIntervalCombo_->addItem(tr("1000 ms"), 1000);
    pollIntervalCombo_->addItem(tr("2000 ms"), 2000);
    pollIntervalCombo_->setCurrentIndex(1);
    pollLayout->addRow(tr("Interval:"), pollIntervalCombo_);
    layout->addWidget(pollGroup);

    layout->addStretch();

    connect(thresholdSpin_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &DcSyncPrecisionPlugin::handleThresholdChanged);
    connect(historyWindowSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &DcSyncPrecisionPlugin::handleHistoryWindowChanged);

    return configWidget;
}

void DcSyncPrecisionPlugin::handleStartStop() {
    if (service_->isMonitoring())
        service_->stopMonitoring();
    else
        service_->startMonitoring();
}

void DcSyncPrecisionPlugin::handleMonitoringStateChanged(bool active) {
    startStopBtn_->setText(active ? tr("Stop Monitoring") : tr("Start Monitoring"));
    exportBtn_->setEnabled(active);
}

void DcSyncPrecisionPlugin::handleDriftUpdated(const DriftStatusEx& status) {
    driftMonitor_->addSample(status.drift);
    updateSyncStatusTable();
}

void DcSyncPrecisionPlugin::handleJitterUpdated(const JitterStatsEx& stats) {
    jitterAnalysis_->setJitterData(stats.min, stats.max, stats.avg, stats.stddev, stats.sampleCount);
    jitterAnalysis_->setHistogram(stats.histogram, stats.histogramBinWidth, stats.min);
}

void DcSyncPrecisionPlugin::handleSyncQualityChanged(const SyncQuality& q) {
    updateQualityDisplay(q);
}

void DcSyncPrecisionPlugin::handleThresholdChanged(double value) {
    service_->setDriftThreshold(value);
    driftMonitor_->setThreshold(value);
}

void DcSyncPrecisionPlugin::handleHistoryWindowChanged(int value) {
    service_->setHistoryWindow(value);
    driftMonitor_->setHistorySize(value);
}

void DcSyncPrecisionPlugin::updateSyncStatusTable() {
    auto drifts = service_->slaveDrifts();
    syncTable_->setRowCount(drifts.size());

    int ref = service_->referenceClock();
    refClockLabel_->setText(ref >= 0 ? tr("Reference Clock: Slave %1").arg(ref) : tr("Reference Clock: N/A"));

    for (int i = 0; i < drifts.size(); ++i) {
        const auto& ds = drifts[i];
        syncTable_->setItem(i, 0, new QTableWidgetItem(QString::number(ds.slave)));
        syncTable_->setItem(i, 1, new QTableWidgetItem(QString::number(ds.drift, 'f', 1)));
        syncTable_->setItem(i, 2, new QTableWidgetItem(QString::number(ds.threshold, 'f', 0)));

        auto* statusItem = new QTableWidgetItem(ds.status);
        if (ds.status == "Error")
            statusItem->setForeground(QColor("#ef4444"));
        else if (ds.status == "Warning")
            statusItem->setForeground(QColor("#f59e0b"));
        else
            statusItem->setForeground(QColor("#22c55e"));
        syncTable_->setItem(i, 3, statusItem);

        syncTable_->setItem(i, 4,
                            new QTableWidgetItem(ds.history.isEmpty() ? QStringLiteral("--")
                                                                      : QString::number(ds.history.last(), 'f', 1)));

        QDateTime ts = QDateTime::fromMSecsSinceEpoch(ds.timestamp);
        syncTable_->setItem(i, 5, new QTableWidgetItem(ts.toString("hh:mm:ss.zzz")));
    }
}

void DcSyncPrecisionPlugin::updateQualityDisplay(const SyncQuality& q) {
    qualityLabel_->setText(tr("Quality: %1 (%2/%3 synced)").arg(q.grade).arg(q.syncedSlaves).arg(q.totalSlaves));
    qualityScoreLabel_->setText(QString::number(q.score, 'f', 1) + "%");

    if (q.score >= 90.0)
        qualityScoreLabel_->setStyleSheet("color: #22c55e; font-weight: bold;");
    else if (q.score >= 75.0)
        qualityScoreLabel_->setStyleSheet("color: #f59e0b; font-weight: bold;");
    else
        qualityScoreLabel_->setStyleSheet("color: #ef4444; font-weight: bold;");
}

void DcSyncPrecisionPlugin::handleExport() {
    const QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export DC Sync Precision Report"),
                                                      QString(), "Markdown (*.md);;Text (*.txt)");
    if (path.isEmpty())
        return;

    exportReportToFile(path);
}

bool DcSyncPrecisionPlugin::exportReportToFile(const QString& path) {
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "# DC Sync Precision Report\n\n";

    const SyncQuality q = service_->syncQuality();
    out << "## Sync Quality\n";
    out << "- Grade: " << q.grade << "\n";
    out << "- Score: " << QString::number(q.score, 'f', 1) << "%\n";
    out << "- Synced: " << q.syncedSlaves << "/" << q.totalSlaves << "\n";
    out << "- Warnings: " << q.warningSlaves << "\n";
    out << "- Errors: " << q.errorSlaves << "\n\n";

    const JitterStatsEx js = service_->jitterStatistics();
    out << "## Jitter Statistics\n";
    out << "- Min: " << QString::number(js.min, 'f', 1) << " ns\n";
    out << "- Max: " << QString::number(js.max, 'f', 1) << " ns\n";
    out << "- Avg: " << QString::number(js.avg, 'f', 1) << " ns\n";
    out << "- Stddev: " << QString::number(js.stddev, 'f', 1) << " ns\n";
    out << "- Samples: " << js.sampleCount << "\n\n";

    out << "## Per-Slave Drift\n";
    out << "| Slave | Drift (ns) | Status |\n";
    out << "| --- | --- | --- |\n";
    for (const auto& ds : service_->slaveDrifts()) {
        out << "| " << ds.slave << " | " << QString::number(ds.drift, 'f', 1) << " | " << ds.status << " |\n";
    }
    return out.status() == QTextStream::Ok && file.flush();
}
