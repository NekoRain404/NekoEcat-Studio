#include "LogicAnalyzerPlugin.h"
#include "TraceService.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

LogicAnalyzerPlugin::LogicAnalyzerPlugin(TraceService* service, QObject* parent) : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &TraceService::traceDataUpdated, this, &LogicAnalyzerPlugin::refreshWaveforms);
}

QString LogicAnalyzerPlugin::id() const {
    return "logicanalyzer";
}
QString LogicAnalyzerPlugin::displayName() const {
    return "Logic Analyzer";
}
QString LogicAnalyzerPlugin::displayNameZh() const {
    return QStringLiteral("逻辑分析仪");
}
int LogicAnalyzerPlugin::defaultOrder() const {
    return 185;
}
bool LogicAnalyzerPlugin::visible() const {
    return true;
}

QWidget* LogicAnalyzerPlugin::widget() {
    return container_;
}

void LogicAnalyzerPlugin::buildUi() {
    container_ = new QWidget;
    auto* root = new QHBoxLayout(container_);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Left panel: channel list + controls
    auto* leftPanel = new QWidget;
    leftPanel->setFixedWidth(260);
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    // Channel list
    leftLayout->addWidget(new QLabel(tr("Channels")));
    channelTable_ = new QTableWidget;
    channelTable_->setColumnCount(3);
    channelTable_->setHorizontalHeaderLabels({tr("Name"), tr("Protocol"), tr("Label")});
    channelTable_->horizontalHeader()->setStretchLastSection(true);
    channelTable_->setSelectionBehavior(QTableWidget::SelectRows);
    leftLayout->addWidget(channelTable_);

    auto* btnRow = new QWidget;
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    auto* addBtn = new QPushButton(tr("+"));
    auto* removeBtn = new QPushButton(tr("-"));
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(removeBtn);
    leftLayout->addWidget(btnRow);

    // Trigger settings
    auto* trigGroup = new QGroupBox(tr("Trigger"));
    auto* trigLayout = new QFormLayout(trigGroup);
    triggerModeCombo_ = new QComboBox;
    triggerModeCombo_->addItems(
        {tr("Rising Edge"), tr("Falling Edge"), tr("Both Edges"), tr("High Level"), tr("Low Level")});
    trigLayout->addRow(tr("Mode"), triggerModeCombo_);
    triggerChannelSpin_ = new QSpinBox;
    triggerChannelSpin_->setRange(0, 15);
    trigLayout->addRow(tr("Channel"), triggerChannelSpin_);
    leftLayout->addWidget(trigGroup);

    // Controls
    auto* ctrlGroup = new QGroupBox(tr("Capture"));
    auto* ctrlLayout = new QVBoxLayout(ctrlGroup);
    startBtn_ = new QPushButton(tr("Start"));
    stopBtn_ = new QPushButton(tr("Stop"));
    stopBtn_->setEnabled(false);
    ctrlLayout->addWidget(startBtn_);
    ctrlLayout->addWidget(stopBtn_);
    leftLayout->addWidget(ctrlGroup);

    // Zoom controls
    auto* zoomGroup = new QGroupBox(tr("Zoom"));
    auto* zoomLayout = new QHBoxLayout(zoomGroup);
    auto* zoomInBtn = new QPushButton(tr("+"));
    auto* zoomOutBtn = new QPushButton(tr("-"));
    auto* zoomFitBtn = new QPushButton(tr("Fit"));
    zoomLayout->addWidget(zoomInBtn);
    zoomLayout->addWidget(zoomOutBtn);
    zoomLayout->addWidget(zoomFitBtn);
    leftLayout->addWidget(zoomGroup);

    // Protocol decode
    auto* decodeBtn = new QPushButton(tr("Decode Protocol"));
    leftLayout->addWidget(decodeBtn);

    // Status
    statusLabel_ = new QLabel(tr("Status: Stopped"));
    leftLayout->addWidget(statusLabel_);
    cursorLabel_ = new QLabel;
    cursorLabel_->setWordWrap(true);
    leftLayout->addWidget(cursorLabel_);

    leftLayout->addStretch();
    root->addWidget(leftPanel);

    // Center: waveform display placeholder
    waveformDisplay_ = new QWidget;
    waveformDisplay_->setStyleSheet("background-color: #0a0a1a;");
    root->addWidget(waveformDisplay_, 1);

    // Connections
    connect(addBtn, &QPushButton::clicked, this, &LogicAnalyzerPlugin::showAddChannelDialog);
    connect(removeBtn, &QPushButton::clicked, this, &LogicAnalyzerPlugin::removeSelectedChannel);
    connect(startBtn_, &QPushButton::clicked, this, &LogicAnalyzerPlugin::startCapture);
    connect(stopBtn_, &QPushButton::clicked, this, &LogicAnalyzerPlugin::stopCapture);
    connect(zoomInBtn, &QPushButton::clicked, this, &LogicAnalyzerPlugin::zoomIn);
    connect(zoomOutBtn, &QPushButton::clicked, this, &LogicAnalyzerPlugin::zoomOut);
    connect(zoomFitBtn, &QPushButton::clicked, this, &LogicAnalyzerPlugin::zoomFit);
    connect(decodeBtn, &QPushButton::clicked, this, &LogicAnalyzerPlugin::decodeProtocol);
}

void LogicAnalyzerPlugin::showAddChannelDialog() {
    QDialog dlg(container_);
    dlg.setWindowTitle(tr("Add Logic Channel"));
    auto* form = new QFormLayout(&dlg);

    auto* nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText(tr("e.g. SPI_CLK"));
    form->addRow(tr("Name"), nameEdit);

    auto* protoCombo = new QComboBox;
    protoCombo->addItems({tr("None"), tr("SPI"), tr("I2C"), tr("UART"), tr("CAN")});
    form->addRow(tr("Protocol"), protoCombo);

    auto* labelEdit = new QLineEdit;
    labelEdit->setPlaceholderText(tr("Optional label"));
    form->addRow(tr("Label"), labelEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        LogicChannel ch;
        ch.id = logicChannels_.size();
        ch.name = nameEdit->text().trimmed();
        ch.protocol = protoCombo->currentText();
        logicChannels_.append(ch);
        updateChannelTable();
    }
}

void LogicAnalyzerPlugin::removeSelectedChannel() {
    const int row = channelTable_->currentRow();
    if (row < 0 || row >= logicChannels_.size())
        return;
    logicChannels_.removeAt(row);
    updateChannelTable();
}

void LogicAnalyzerPlugin::startCapture() {
    service_->startTrace();
    if (service_->isTracing()) {
        statusLabel_->setText(tr("Status: Running"));
        startBtn_->setEnabled(false);
        stopBtn_->setEnabled(true);
        return;
    }

    statusLabel_->setText(tr("Status: Stopped - capture backend required"));
    startBtn_->setEnabled(true);
    stopBtn_->setEnabled(false);
}

void LogicAnalyzerPlugin::stopCapture() {
    service_->stopTrace();
    statusLabel_->setText(tr("Status: Stopped"));
    startBtn_->setEnabled(true);
    stopBtn_->setEnabled(false);
}

void LogicAnalyzerPlugin::zoomIn() {
    zoomLevel_ *= 1.5;
    refreshWaveforms();
}

void LogicAnalyzerPlugin::zoomOut() {
    zoomLevel_ /= 1.5;
    if (zoomLevel_ < 0.1)
        zoomLevel_ = 0.1;
    refreshWaveforms();
}

void LogicAnalyzerPlugin::zoomFit() {
    zoomLevel_ = 1.0;
    refreshWaveforms();
}

void LogicAnalyzerPlugin::decodeProtocol() {
    cursorLabel_->setText(tr("Protocol decode requires captured signal evidence from the trace backend"));
}

void LogicAnalyzerPlugin::refreshWaveforms() {
    waveformDisplay_->update();
}

void LogicAnalyzerPlugin::updateChannelTable() {
    channelTable_->setRowCount(logicChannels_.size());
    for (int i = 0; i < logicChannels_.size(); ++i) {
        channelTable_->setItem(i, 0, new QTableWidgetItem(logicChannels_[i].name));
        channelTable_->setItem(i, 1, new QTableWidgetItem(logicChannels_[i].protocol));
        channelTable_->setItem(i, 2, new QTableWidgetItem(QString::number(logicChannels_[i].id)));
    }
}
