#include "BusStatsPlugin.h"
#include "services/BusStatsService.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

BusStatsPlugin::BusStatsPlugin(BusStatsService* service, QObject* parent) : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &BusStatsService::statsUpdated, this, [this](const QJsonObject&) { updateDisplay(); });
}

QString BusStatsPlugin::id() const {
    return "busstats";
}
QString BusStatsPlugin::displayName() const {
    return "Bus Statistics";
}
QString BusStatsPlugin::displayNameZh() const {
    return QStringLiteral("总线统计");
}
QIcon BusStatsPlugin::icon() const {
    return QIcon::fromTheme("utilities-system-monitor");
}
int BusStatsPlugin::defaultOrder() const {
    return 95;
}
bool BusStatsPlugin::visible() const {
    return true;
}

void BusStatsPlugin::activate() {}
void BusStatsPlugin::deactivate() {}

QWidget* BusStatsPlugin::widget() {
    return containerWidget_;
}

void BusStatsPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(10);

    auto* toolbar = new QHBoxLayout;
    toolbar->setSpacing(8);

    startStopBtn_ = new QPushButton(tr("Start Monitoring"));
    toolbar->addWidget(startStopBtn_);

    statusLabel_ = new QLabel(tr("Stopped"));
    toolbar->addWidget(statusLabel_);

    toolbar->addStretch();

    frameRateLabel_ = new QLabel(tr("Frame Rate: --"));
    toolbar->addWidget(frameRateLabel_);

    bandwidthLabel_ = new QLabel(tr("Bandwidth: --"));
    toolbar->addWidget(bandwidthLabel_);

    layout->addLayout(toolbar);

    statsTable_ = new QTableWidget;
    statsTable_->setColumnCount(2);
    statsTable_->setHorizontalHeaderLabels({tr("Metric"), tr("Value")});
    statsTable_->horizontalHeader()->setStretchLastSection(true);
    statsTable_->verticalHeader()->setVisible(false);
    statsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    statsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    statsTable_->setShowGrid(false);
    statsTable_->setAlternatingRowColors(true);

    auto addMetric = [&](const QString& name) {
        int r = statsTable_->rowCount();
        statsTable_->insertRow(r);
        statsTable_->setItem(r, 0, new QTableWidgetItem(name));
        statsTable_->setItem(r, 1, new QTableWidgetItem("0"));
    };

    addMetric(tr("TX Frames"));
    addMetric(tr("RX Frames"));
    addMetric(tr("TX Errors"));
    addMetric(tr("RX Errors"));
    addMetric(tr("CRC Errors"));
    addMetric(tr("Lost Frames"));
    addMetric(tr("Total Errors"));
    addMetric(tr("Uptime"));

    layout->addWidget(statsTable_, 1);

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
}

void BusStatsPlugin::updateDisplay() {
    auto stats = service_->currentStats();

    auto setValue = [&](int row, const QString& val) {
        if (auto* item = statsTable_->item(row, 1))
            item->setText(val);
    };

    setValue(0, QString::number(stats.txFrames));
    setValue(1, QString::number(stats.rxFrames));
    setValue(2, QString::number(stats.txErrors));
    setValue(3, QString::number(stats.rxErrors));
    setValue(4, QString::number(stats.crcErrors));
    setValue(5, QString::number(stats.lostFrames));
    setValue(6, QString::number(stats.txErrors + stats.rxErrors + stats.crcErrors));

    qint64 uptimeMs = stats.timestampMs > 0 ? stats.timestampMs : 0;
    if (uptimeMs > 0) {
        double secs = static_cast<double>(uptimeMs) / 1000.0;
        setValue(7, QString::number(secs, 'f', 1) + " s");
    }

    frameRateLabel_->setText(tr("Frame Rate: %1 f/s").arg(stats.frameRate, 0, 'f', 1));
    bandwidthLabel_->setText(tr("Bandwidth: %1 Mbps").arg(stats.bandwidthMbps, 0, 'f', 2));
}
