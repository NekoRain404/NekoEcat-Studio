#include "TracePlugin.h"
#include "TraceService.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

TracePlugin::TracePlugin(TraceService* service, QObject* parent) : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    connect(service_, &TraceService::traceDataUpdated, this, &TracePlugin::refreshDisplay);
    connect(service_, &TraceService::channelAdded, this, [this](int) { updateChannelTable(); });
    connect(service_, &TraceService::channelRemoved, this, [this](int) { updateChannelTable(); });
    connect(service_, &TraceService::traceStarted, this, [this]() {
        statusLabel_->setText(tr("Status: Running"));
        startBtn_->setEnabled(false);
        stopBtn_->setEnabled(true);
        singleBtn_->setEnabled(false);
    });
    connect(service_, &TraceService::traceStopped, this, [this]() {
        statusLabel_->setText(tr("Status: Stopped"));
        startBtn_->setEnabled(true);
        stopBtn_->setEnabled(false);
        singleBtn_->setEnabled(true);
    });
}

QString TracePlugin::id() const {
    return "trace";
}
QString TracePlugin::displayName() const {
    return "Signal Trace";
}
QString TracePlugin::displayNameZh() const {
    return QStringLiteral("信号追踪");
}
int TracePlugin::defaultOrder() const {
    return 180;
}
bool TracePlugin::visible() const {
    return true;
}

QWidget* TracePlugin::widget() {
    return container_;
}

void TracePlugin::buildUi() {
    container_ = new QWidget;
    auto* root = new QHBoxLayout(container_);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Left panel: channels + controls
    auto* leftPanel = new QWidget;
    leftPanel->setFixedWidth(280);
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    // Channel table
    leftLayout->addWidget(new QLabel(tr("Trace Channels")));
    channelTable_ = new QTableWidget;
    channelTable_->setColumnCount(4);
    channelTable_->setHorizontalHeaderLabels({tr("Name"), tr("Slave"), tr("Index"), tr("SubIdx")});
    channelTable_->horizontalHeader()->setStretchLastSection(true);
    channelTable_->setSelectionBehavior(QTableWidget::SelectRows);
    leftLayout->addWidget(channelTable_);

    auto* btnRow = new QWidget;
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    auto* addBtn = new QPushButton(tr("+"));
    addBtn->setToolTip(tr("Add channel"));
    auto* removeBtn = new QPushButton(tr("-"));
    removeBtn->setToolTip(tr("Remove selected"));
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(removeBtn);
    leftLayout->addWidget(btnRow);

    // Trigger settings
    auto* trigGroup = new QGroupBox(tr("Trigger Settings"));
    auto* trigLayout = new QFormLayout(trigGroup);
    triggerModeCombo_ = new QComboBox;
    triggerModeCombo_->addItems({tr("Auto"), tr("Normal"), tr("Single"), tr("Rising Edge"), tr("Falling Edge")});
    trigLayout->addRow(tr("Mode"), triggerModeCombo_);
    leftLayout->addWidget(trigGroup);

    // Sample settings
    auto* sampleGroup = new QGroupBox(tr("Sample Settings"));
    auto* sampleLayout = new QFormLayout(sampleGroup);
    sampleRateSpin_ = new QSpinBox;
    sampleRateSpin_->setRange(1, 100000);
    sampleRateSpin_->setValue(1000);
    sampleRateSpin_->setSuffix(tr(" Hz"));
    sampleLayout->addRow(tr("Rate"), sampleRateSpin_);
    bufferSizeSpin_ = new QSpinBox;
    bufferSizeSpin_->setRange(100, 1000000);
    bufferSizeSpin_->setValue(10000);
    sampleLayout->addRow(tr("Buffer"), bufferSizeSpin_);
    leftLayout->addWidget(sampleGroup);

    // Controls
    auto* ctrlGroup = new QGroupBox(tr("Controls"));
    auto* ctrlLayout = new QVBoxLayout(ctrlGroup);
    startBtn_ = new QPushButton(tr("Start"));
    stopBtn_ = new QPushButton(tr("Stop"));
    singleBtn_ = new QPushButton(tr("Single"));
    stopBtn_->setEnabled(false);
    ctrlLayout->addWidget(startBtn_);
    ctrlLayout->addWidget(stopBtn_);
    ctrlLayout->addWidget(singleBtn_);
    leftLayout->addWidget(ctrlGroup);

    // Status
    statusLabel_ = new QLabel(tr("Status: Stopped"));
    leftLayout->addWidget(statusLabel_);

    // Export
    auto* exportBtn = new QPushButton(tr("Export Data"));
    leftLayout->addWidget(exportBtn);

    leftLayout->addStretch();
    root->addWidget(leftPanel);

    // Center: trace display placeholder
    traceDisplay_ = new QWidget;
    traceDisplay_->setStyleSheet("background-color: #1a1a2e;");
    root->addWidget(traceDisplay_, 1);

    // Connections
    connect(addBtn, &QPushButton::clicked, this, &TracePlugin::showAddChannelDialog);
    connect(removeBtn, &QPushButton::clicked, this, &TracePlugin::removeSelectedChannel);
    connect(startBtn_, &QPushButton::clicked, this, &TracePlugin::startTrace);
    connect(stopBtn_, &QPushButton::clicked, this, &TracePlugin::stopTrace);
    connect(singleBtn_, &QPushButton::clicked, this, &TracePlugin::singleCapture);
    connect(exportBtn, &QPushButton::clicked, this, &TracePlugin::exportTraceData);
    connect(triggerModeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &TracePlugin::onTriggerModeChanged);
    connect(sampleRateSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [this](int val) { service_->setSampleRate(val); });
    connect(bufferSizeSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [this](int val) { service_->setBufferSize(val); });
}

void TracePlugin::showAddChannelDialog() {
    QDialog dlg(container_);
    dlg.setWindowTitle(tr("Add Trace Channel"));
    auto* form = new QFormLayout(&dlg);

    auto* nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText(tr("Channel name"));
    form->addRow(tr("Name"), nameEdit);

    auto* slaveSpin = new QSpinBox;
    slaveSpin->setRange(0, 255);
    form->addRow(tr("Slave"), slaveSpin);

    auto* idxEdit = new QLineEdit;
    idxEdit->setPlaceholderText(tr("e.g. 0x6064"));
    form->addRow(tr("Index"), idxEdit);

    auto* subEdit = new QLineEdit;
    subEdit->setText("0");
    form->addRow(tr("SubIndex"), subEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        const int id = service_->addChannel(nameEdit->text().trimmed(), slaveSpin->value(), idxEdit->text().trimmed(),
                                            subEdit->text().trimmed());
        if (id > 0) {
            updateChannelTable();
        }
    }
}

void TracePlugin::removeSelectedChannel() {
    const int row = channelTable_->currentRow();
    if (row < 0)
        return;
    const auto chs = service_->channels();
    if (row >= chs.size())
        return;
    service_->removeChannel(chs[row].id);
    updateChannelTable();
}

void TracePlugin::startTrace() {
    service_->startTrace();
}

void TracePlugin::stopTrace() {
    service_->stopTrace();
}

void TracePlugin::singleCapture() {
    service_->startTrace();
    QTimer::singleShot(100, this, [this]() { service_->stopTrace(); });
}

void TracePlugin::exportTraceData() {
    QString fileName =
        QFileDialog::getSaveFileName(container_, tr("Export Trace Data"), QString(), tr("CSV Files (*.csv)"));
    if (fileName.isEmpty())
        return;

    if (!exportTraceDataToFile(fileName)) {
        QMessageBox::warning(container_, tr("Export Error"), tr("Cannot open file for writing."));
        return;
    }

    QMessageBox::information(container_, tr("Export Complete"), tr("Trace data exported to %1").arg(fileName));
}

bool TracePlugin::exportTraceDataToFile(const QString& path) {
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "Channel,Timestamp,Value,Quality\n";
    const auto chs = service_->channels();
    for (const auto& ch : chs) {
        for (const auto& pt : ch.data) {
            out << ch.name << "," << pt.timestamp << "," << pt.value << "," << pt.quality << "\n";
        }
    }
    return out.status() == QTextStream::Ok && file.flush();
}

void TracePlugin::refreshDisplay() {
    traceDisplay_->update();
}

void TracePlugin::onTriggerModeChanged(int index) {
    service_->setTriggerMode(static_cast<TraceTriggerMode>(index));
}

void TracePlugin::updateChannelTable() {
    const auto chs = service_->channels();
    channelTable_->setRowCount(chs.size());
    for (int i = 0; i < chs.size(); ++i) {
        channelTable_->setItem(i, 0, new QTableWidgetItem(chs[i].name));
        channelTable_->setItem(i, 1, new QTableWidgetItem(QString::number(chs[i].slave)));
        channelTable_->setItem(i, 2, new QTableWidgetItem(chs[i].index));
        channelTable_->setItem(i, 3, new QTableWidgetItem(chs[i].subIndex));
    }
}
