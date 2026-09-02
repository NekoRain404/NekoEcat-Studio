#include "DataPipelinePlugin.h"
#include "services/DataPipelineService.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

DataPipelinePlugin::DataPipelinePlugin(DataPipelineService* service, QObject* parent) : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &DataPipelineService::pipelineUpdated, this, [this]() { updateDisplay(); });
}

QString DataPipelinePlugin::id() const {
    return "datapipeline";
}
QString DataPipelinePlugin::displayName() const {
    return "Data Pipeline";
}
QString DataPipelinePlugin::displayNameZh() const {
    return QStringLiteral("数据管道");
}
QIcon DataPipelinePlugin::icon() const {
    return QIcon::fromTheme("view-sort-ascending");
}
int DataPipelinePlugin::defaultOrder() const {
    return 90;
}
bool DataPipelinePlugin::visible() const {
    return false;
}

void DataPipelinePlugin::activate() {}
void DataPipelinePlugin::deactivate() {}

QWidget* DataPipelinePlugin::widget() {
    return containerWidget_;
}

void DataPipelinePlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(10);

    auto* toolbar = new QHBoxLayout;
    toolbar->setSpacing(8);

    pipelineCombo_ = new QComboBox;
    pipelineCombo_->setMinimumWidth(160);
    toolbar->addWidget(pipelineCombo_);

    startStopBtn_ = new QPushButton(tr("Start Pipeline"));
    toolbar->addWidget(startStopBtn_);

    addStageBtn_ = new QPushButton(tr("Add Stage"));
    toolbar->addWidget(addStageBtn_);

    removeStageBtn_ = new QPushButton(tr("Remove Stage"));
    toolbar->addWidget(removeStageBtn_);

    resetBtn_ = new QPushButton(tr("Reset"));
    toolbar->addWidget(resetBtn_);

    statusLabel_ = new QLabel(tr("Stopped"));
    toolbar->addWidget(statusLabel_);

    toolbar->addStretch();

    throughputLabel_ = new QLabel(tr("Throughput: --"));
    toolbar->addWidget(throughputLabel_);

    latencyLabel_ = new QLabel(tr("Latency: --"));
    toolbar->addWidget(latencyLabel_);

    layout->addLayout(toolbar);

    stageTable_ = new QTableWidget;
    stageTable_->setColumnCount(5);
    stageTable_->setHorizontalHeaderLabels({tr("Stage"), tr("Type"), tr("Status"), tr("Processed"), tr("Errors")});
    stageTable_->horizontalHeader()->setStretchLastSection(true);
    stageTable_->verticalHeader()->setVisible(false);
    stageTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stageTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    stageTable_->setShowGrid(false);
    stageTable_->setAlternatingRowColors(true);
    layout->addWidget(stageTable_, 1);

    connect(startStopBtn_, &QPushButton::clicked, this, [this]() {
        if (service_->isRunning()) {
            service_->stop();
            startStopBtn_->setText(tr("Start Pipeline"));
            statusLabel_->setText(tr("Stopped"));
        } else {
            service_->start();
            startStopBtn_->setText(tr("Stop Pipeline"));
            statusLabel_->setText(tr("Running"));
        }
    });

    connect(addStageBtn_, &QPushButton::clicked, this, [this]() { service_->addDefaultStage(); });

    connect(removeStageBtn_, &QPushButton::clicked, this, [this]() {
        int row = stageTable_->currentRow();
        if (row >= 0)
            service_->removeStage(row);
    });

    connect(resetBtn_, &QPushButton::clicked, this, [this]() {
        service_->resetStatistics();
        updateDisplay();
    });
}

void DataPipelinePlugin::updateDisplay() {
    auto stages = service_->allStages();
    auto metrics = service_->pipelineMetrics();

    stageTable_->setRowCount(0);
    for (const auto& s : stages) {
        int r = stageTable_->rowCount();
        stageTable_->insertRow(r);
        stageTable_->setItem(r, 0, new QTableWidgetItem(s.name));
        stageTable_->setItem(r, 1, new QTableWidgetItem(s.type));
        stageTable_->setItem(r, 2, new QTableWidgetItem(s.active ? tr("Active") : tr("Idle")));
        stageTable_->setItem(r, 3, new QTableWidgetItem(QString::number(s.processed)));
        stageTable_->setItem(r, 4, new QTableWidgetItem(QString::number(s.errors)));
    }

    throughputLabel_->setText(tr("Throughput: %1 msg/s").arg(QString::number(metrics.throughput, 'f', 1)));
    latencyLabel_->setText(tr("Latency: %1 ms").arg(QString::number(metrics.latencyMs, 'f', 2)));
}
