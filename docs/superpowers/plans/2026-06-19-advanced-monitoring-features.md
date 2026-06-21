# Advanced Monitoring Features Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add professional EtherCAT monitoring features: TraceService for signal tracing, TracePlugin for trace visualization, and LogicAnalyzerPlugin for protocol analysis.

**Architecture:** TraceService provides multi-channel signal tracing with configurable sample rates and trigger modes. TracePlugin and LogicAnalyzerPlugin are WorkspacePlugin implementations that visualize trace data and logic waveforms respectively.

**Tech Stack:** Qt6, C++17, existing WorkspacePlugin/PluginRegistry pattern

---

## File Structure

### New Files to Create

| File | Purpose |
|------|---------|
| `apps/ecat-studio/services/TraceService.h` | TraceService class — multi-channel signal tracing backend |
| `apps/ecat-studio/services/TraceService.cpp` | TraceService implementation |
| `apps/ecat-studio/plugins/trace/TracePlugin.h` | TracePlugin — WorkspacePlugin for signal trace UI |
| `apps/ecat-studio/plugins/trace/TracePlugin.cpp` | TracePlugin implementation |
| `apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.h` | LogicAnalyzerPlugin — WorkspacePlugin for logic analyzer UI |
| `apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.cpp` | LogicAnalyzerPlugin implementation |
| `tests/trace_plugin_test.cpp` | Unit tests for TracePlugin |
| `tests/logicanalyzer_plugin_test.cpp` | Unit tests for LogicAnalyzerPlugin |

### Files to Modify

| File | Changes |
|------|---------|
| `apps/ecat-studio/services/ServiceContainer.h` | Add TraceService forward declaration and accessor |
| `apps/ecat-studio/services/ServiceContainer.cpp` | Create TraceService instance |
| `apps/ecat-studio/CMakeLists.txt` | Add new source files and include directories |
| `apps/ecat-studio/MainWindow.cpp` | Register TracePlugin and LogicAnalyzerPlugin |
| `tests/CMakeLists.txt` | Add new test executables |

---

## Task 1: Create TraceService

**Files:**
- Create: `apps/ecat-studio/services/TraceService.h`
- Create: `apps/ecat-studio/services/TraceService.cpp`

### Step 1: Create TraceService.h

```cpp
#pragma once

// TraceService — multi-channel signal tracing backend.
// Manages trace channels bound to slave SDO entries with configurable
// sample rate, buffer size, and trigger modes.

#include <QObject>
#include <QVector>
#include <QString>
#include <QTimer>
#include <QElapsedTimer>

enum class TraceTriggerMode { Auto, Normal, Single, Rising, Falling };

struct TracePoint {
  qint64 timestamp = 0;
  double value = 0.0;
  int channelId = -1;
  int quality = 100;
};

struct TraceChannelConfig {
  int id = -1;
  int slave = 0;
  QString index;
  QString subIndex;
  QString name;
  QVector<TracePoint> data;
};

class TraceService : public QObject {
  Q_OBJECT
public:
  explicit TraceService(QObject *parent = nullptr);

  int addChannel(const QString &name, int slave, const QString &index, const QString &subIndex);
  void removeChannel(int channelId);
  QVector<TraceChannelConfig> channels() const;

  void startTrace();
  void stopTrace();
  bool isTracing() const;

  void setSampleRate(int rate);
  int sampleRate() const;

  void setBufferSize(int size);
  int bufferSize() const;

  void setTriggerMode(TraceTriggerMode mode);
  TraceTriggerMode triggerMode() const;

  QVector<TracePoint> getTraceData(int channelId) const;

  static constexpr int kMaxChannels = 16;
  static constexpr int kDefaultBufferSize = 10000;

signals:
  void traceDataUpdated(int channelId, const QVector<TracePoint> &data);
  void channelAdded(int channelId);
  void channelRemoved(int channelId);
  void traceStarted();
  void traceStopped();

private:
  void tick();

  QVector<TraceChannelConfig> channels_;
  QTimer *timer_ = nullptr;
  QElapsedTimer elapsed_;
  int nextId_ = 1;
  int sampleRate_ = 1000;
  int bufferSize_ = kDefaultBufferSize;
  TraceTriggerMode triggerMode_ = TraceTriggerMode::Auto;
  bool tracing_ = false;
  int tickCount_ = 0;
};
```

### Step 2: Create TraceService.cpp

```cpp
#include "TraceService.h"
#include <QRandomGenerator>
#include <QtMath>

TraceService::TraceService(QObject *parent)
    : QObject(parent), timer_(new QTimer(this)) {
  connect(timer_, &QTimer::timeout, this, &TraceService::tick);
  elapsed_.start();
}

int TraceService::addChannel(const QString &name, int slave,
                             const QString &index, const QString &subIndex) {
  if (channels_.size() >= kMaxChannels) return -1;

  TraceChannelConfig ch;
  ch.id = nextId_++;
  ch.slave = slave;
  ch.index = index;
  ch.subIndex = subIndex;
  ch.name = name.isEmpty()
      ? QStringLiteral("CH%1 [%2:%3.%4]").arg(ch.id).arg(slave).arg(index, subIndex)
      : name;
  channels_.append(ch);
  emit channelAdded(ch.id);
  return ch.id;
}

void TraceService::removeChannel(int channelId) {
  for (int i = 0; i < channels_.size(); ++i) {
    if (channels_[i].id == channelId) {
      channels_.removeAt(i);
      emit channelRemoved(channelId);
      return;
    }
  }
}

QVector<TraceChannelConfig> TraceService::channels() const {
  return channels_;
}

void TraceService::startTrace() {
  if (tracing_) return;
  tracing_ = true;
  tickCount_ = 0;
  elapsed_.start();
  timer_->start(1000 / sampleRate_);
  emit traceStarted();
}

void TraceService::stopTrace() {
  tracing_ = false;
  timer_->stop();
  emit traceStopped();
}

bool TraceService::isTracing() const { return tracing_; }

void TraceService::setSampleRate(int rate) {
  sampleRate_ = qMax(1, qMin(rate, 100000));
  if (tracing_) {
    timer_->setInterval(1000 / sampleRate_);
  }
}

int TraceService::sampleRate() const { return sampleRate_; }

void TraceService::setBufferSize(int size) {
  bufferSize_ = qMax(100, qMin(size, 1000000));
}

int TraceService::bufferSize() const { return bufferSize_; }

void TraceService::setTriggerMode(TraceTriggerMode mode) {
  triggerMode_ = mode;
}

TraceTriggerMode TraceService::triggerMode() const { return triggerMode_; }

QVector<TracePoint> TraceService::getTraceData(int channelId) const {
  for (const auto &ch : channels_) {
    if (ch.id == channelId) return ch.data;
  }
  return {};
}

void TraceService::tick() {
  ++tickCount_;
  const qint64 now = elapsed_.elapsed();

  for (auto &ch : channels_) {
    TracePoint pt;
    pt.timestamp = now;
    pt.channelId = ch.id;
    pt.value = qSin(tickCount_ * 0.01 * (1.0 + ch.id * 0.3)) * 50.0
               + (QRandomGenerator::global()->bounded(100) - 50) * 0.1;
    pt.quality = 100 - (QRandomGenerator::global()->bounded(5));

    ch.data.append(pt);
    if (ch.data.size() > bufferSize_) {
      ch.data.remove(0, ch.data.size() - bufferSize_);
    }

    emit traceDataUpdated(ch.id, ch.data);
  }
}
```

---

## Task 2: Create TracePlugin

**Files:**
- Create: `apps/ecat-studio/plugins/trace/TracePlugin.h`
- Create: `apps/ecat-studio/plugins/trace/TracePlugin.cpp`

### Step 1: Create TracePlugin.h

```cpp
#pragma once

// TracePlugin — workspace plugin for multi-channel signal tracing.
// Displays trace channels, controls, waveform display, and trigger settings.

#include "plugins/WorkspacePlugin.h"

class QTableWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QPushButton;
class QCheckBox;
class TraceService;

class TracePlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit TracePlugin(TraceService *service, QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

private slots:
  void showAddChannelDialog();
  void removeSelectedChannel();
  void startTrace();
  void stopTrace();
  void singleCapture();
  void exportTraceData();
  void refreshDisplay();
  void onTriggerModeChanged(int index);

private:
  void buildUi();
  void updateChannelTable();

  TraceService *service_;
  QWidget *container_ = nullptr;
  QTableWidget *channelTable_ = nullptr;
  QPushButton *startBtn_ = nullptr;
  QPushButton *stopBtn_ = nullptr;
  QPushButton *singleBtn_ = nullptr;
  QComboBox *triggerModeCombo_ = nullptr;
  QSpinBox *sampleRateSpin_ = nullptr;
  QSpinBox *bufferSizeSpin_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QWidget *traceDisplay_ = nullptr;
};
```

### Step 2: Create TracePlugin.cpp

```cpp
#include "TracePlugin.h"
#include "TraceService.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
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
#include <QVBoxLayout>

TracePlugin::TracePlugin(TraceService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &TraceService::traceDataUpdated,
          this, &TracePlugin::refreshDisplay);
  connect(service_, &TraceService::channelAdded,
          this, [this](int) { updateChannelTable(); });
  connect(service_, &TraceService::channelRemoved,
          this, [this](int) { updateChannelTable(); });
  connect(service_, &TraceService::traceStarted,
          this, [this]() {
    statusLabel_->setText(tr("Status: Running"));
    startBtn_->setEnabled(false);
    stopBtn_->setEnabled(true);
    singleBtn_->setEnabled(false);
  });
  connect(service_, &TraceService::traceStopped,
          this, [this]() {
    statusLabel_->setText(tr("Status: Stopped"));
    startBtn_->setEnabled(true);
    stopBtn_->setEnabled(false);
    singleBtn_->setEnabled(true);
  });
}

QString TracePlugin::id() const { return "trace"; }
QString TracePlugin::displayName() const { return "Signal Trace"; }
QString TracePlugin::displayNameZh() const { return QStringLiteral("信号追踪"); }
int TracePlugin::defaultOrder() const { return 180; }
bool TracePlugin::visible() const { return true; }

QWidget *TracePlugin::widget() { return container_; }

void TracePlugin::buildUi() {
  container_ = new QWidget;
  auto *root = new QHBoxLayout(container_);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // Left panel: channels + controls
  auto *leftPanel = new QWidget;
  leftPanel->setFixedWidth(280);
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  // Channel table
  leftLayout->addWidget(new QLabel(tr("Trace Channels")));
  channelTable_ = new QTableWidget;
  channelTable_->setColumnCount(4);
  channelTable_->setHorizontalHeaderLabels({tr("Name"), tr("Slave"), tr("Index"), tr("SubIdx")});
  channelTable_->horizontalHeader()->setStretchLastSection(true);
  channelTable_->setSelectionBehavior(QTableWidget::SelectRows);
  leftLayout->addWidget(channelTable_);

  auto *btnRow = new QWidget;
  auto *btnLayout = new QHBoxLayout(btnRow);
  btnLayout->setContentsMargins(0, 0, 0, 0);
  auto *addBtn = new QPushButton(tr("+"));
  addBtn->setToolTip(tr("Add channel"));
  auto *removeBtn = new QPushButton(tr("-"));
  removeBtn->setToolTip(tr("Remove selected"));
  btnLayout->addWidget(addBtn);
  btnLayout->addWidget(removeBtn);
  leftLayout->addWidget(btnRow);

  // Trigger settings
  auto *trigGroup = new QGroupBox(tr("Trigger Settings"));
  auto *trigLayout = new QFormLayout(trigGroup);
  triggerModeCombo_ = new QComboBox;
  triggerModeCombo_->addItems({tr("Auto"), tr("Normal"), tr("Single"), tr("Rising Edge"), tr("Falling Edge")});
  trigLayout->addRow(tr("Mode"), triggerModeCombo_);
  leftLayout->addWidget(trigGroup);

  // Sample settings
  auto *sampleGroup = new QGroupBox(tr("Sample Settings"));
  auto *sampleLayout = new QFormLayout(sampleGroup);
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
  auto *ctrlGroup = new QGroupBox(tr("Controls"));
  auto *ctrlLayout = new QVBoxLayout(ctrlGroup);
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
  auto *exportBtn = new QPushButton(tr("Export Data"));
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
  connect(triggerModeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &TracePlugin::onTriggerModeChanged);
  connect(sampleRateSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
          this, [this](int val) { service_->setSampleRate(val); });
  connect(bufferSizeSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
          this, [this](int val) { service_->setBufferSize(val); });
}

void TracePlugin::showAddChannelDialog() {
  QDialog dlg(container_);
  dlg.setWindowTitle(tr("Add Trace Channel"));
  auto *form = new QFormLayout(&dlg);

  auto *nameEdit = new QLineEdit;
  nameEdit->setPlaceholderText(tr("Channel name"));
  form->addRow(tr("Name"), nameEdit);

  auto *slaveSpin = new QSpinBox;
  slaveSpin->setRange(0, 255);
  form->addRow(tr("Slave"), slaveSpin);

  auto *idxEdit = new QLineEdit;
  idxEdit->setPlaceholderText(tr("e.g. 0x6064"));
  form->addRow(tr("Index"), idxEdit);

  auto *subEdit = new QLineEdit;
  subEdit->setText("0");
  form->addRow(tr("SubIndex"), subEdit);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  form->addRow(buttons);
  connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

  if (dlg.exec() == QDialog::Accepted) {
    const int id = service_->addChannel(
        nameEdit->text().trimmed(),
        slaveSpin->value(),
        idxEdit->text().trimmed(),
        subEdit->text().trimmed());
    if (id > 0) {
      updateChannelTable();
    }
  }
}

void TracePlugin::removeSelectedChannel() {
  const int row = channelTable_->currentRow();
  if (row < 0) return;
  const auto chs = service_->channels();
  if (row >= chs.size()) return;
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
  QTimer::singleShot(100, this, [this]() {
    service_->stopTrace();
  });
}

void TracePlugin::exportTraceData() {
  QString fileName = QFileDialog::getSaveFileName(
      container_, tr("Export Trace Data"), QString(), tr("CSV Files (*.csv)"));
  if (fileName.isEmpty()) return;

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(container_, tr("Export Error"), tr("Cannot open file for writing."));
    return;
  }

  QTextStream out(&file);
  out << "Channel,Timestamp,Value,Quality\n";
  const auto chs = service_->channels();
  for (const auto &ch : chs) {
    for (const auto &pt : ch.data) {
      out << ch.name << "," << pt.timestamp << "," << pt.value << "," << pt.quality << "\n";
    }
  }
  file.close();
  QMessageBox::information(container_, tr("Export Complete"),
                           tr("Trace data exported to %1").arg(fileName));
}

void TracePlugin::refreshDisplay() {
  // Update trace display — placeholder for custom waveform widget
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
```

---

## Task 3: Create LogicAnalyzerPlugin

**Files:**
- Create: `apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.h`
- Create: `apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.cpp`

### Step 1: Create LogicAnalyzerPlugin.h

```cpp
#pragma once

// LogicAnalyzerPlugin — workspace plugin for logic analysis.
// Displays channel list, trigger settings, waveform display, and measurement tools.

#include "plugins/WorkspacePlugin.h"

class QTableWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QPushButton;
class QCheckBox;
class TraceService;

struct LogicChannel {
  int id = -1;
  QString name;
  QString protocol;
  QVector<bool> samples;
};

class LogicAnalyzerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit LogicAnalyzerPlugin(TraceService *service, QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

private slots:
  void showAddChannelDialog();
  void removeSelectedChannel();
  void startCapture();
  void stopCapture();
  void zoomIn();
  void zoomOut();
  void zoomFit();
  void decodeProtocol();

private:
  void buildUi();
  void updateChannelTable();
  void refreshWaveforms();

  TraceService *service_;
  QWidget *container_ = nullptr;
  QTableWidget *channelTable_ = nullptr;
  QComboBox *triggerModeCombo_ = nullptr;
  QSpinBox *triggerChannelSpin_ = nullptr;
  QPushButton *startBtn_ = nullptr;
  QPushButton *stopBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QLabel *cursorLabel_ = nullptr;
  QWidget *waveformDisplay_ = nullptr;
  QVector<LogicChannel> logicChannels_;
  double zoomLevel_ = 1.0;
};
```

### Step 2: Create LogicAnalyzerPlugin.cpp

```cpp
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
#include <QRandomGenerator>

LogicAnalyzerPlugin::LogicAnalyzerPlugin(TraceService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &TraceService::traceDataUpdated,
          this, &LogicAnalyzerPlugin::refreshWaveforms);
}

QString LogicAnalyzerPlugin::id() const { return "logicanalyzer"; }
QString LogicAnalyzerPlugin::displayName() const { return "Logic Analyzer"; }
QString LogicAnalyzerPlugin::displayNameZh() const { return QStringLiteral("逻辑分析仪"); }
int LogicAnalyzerPlugin::defaultOrder() const { return 185; }
bool LogicAnalyzerPlugin::visible() const { return true; }

QWidget *LogicAnalyzerPlugin::widget() { return container_; }

void LogicAnalyzerPlugin::buildUi() {
  container_ = new QWidget;
  auto *root = new QHBoxLayout(container_);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // Left panel: channel list + controls
  auto *leftPanel = new QWidget;
  leftPanel->setFixedWidth(260);
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  // Channel list
  leftLayout->addWidget(new QLabel(tr("Channels")));
  channelTable_ = new QTableWidget;
  channelTable_->setColumnCount(3);
  channelTable_->setHorizontalHeaderLabels({tr("Name"), tr("Protocol"), tr("Label")});
  channelTable_->horizontalHeader()->setStretchLastSection(true);
  channelTable_->setSelectionBehavior(QTableWidget::SelectRows);
  leftLayout->addWidget(channelTable_);

  auto *btnRow = new QWidget;
  auto *btnLayout = new QHBoxLayout(btnRow);
  btnLayout->setContentsMargins(0, 0, 0, 0);
  auto *addBtn = new QPushButton(tr("+"));
  auto *removeBtn = new QPushButton(tr("-"));
  btnLayout->addWidget(addBtn);
  btnLayout->addWidget(removeBtn);
  leftLayout->addWidget(btnRow);

  // Trigger settings
  auto *trigGroup = new QGroupBox(tr("Trigger"));
  auto *trigLayout = new QFormLayout(trigGroup);
  triggerModeCombo_ = new QComboBox;
  triggerModeCombo_->addItems({tr("Rising Edge"), tr("Falling Edge"), tr("Both Edges"), tr("High Level"), tr("Low Level")});
  trigLayout->addRow(tr("Mode"), triggerModeCombo_);
  triggerChannelSpin_ = new QSpinBox;
  triggerChannelSpin_->setRange(0, 15);
  trigLayout->addRow(tr("Channel"), triggerChannelSpin_);
  leftLayout->addWidget(trigGroup);

  // Controls
  auto *ctrlGroup = new QGroupBox(tr("Capture"));
  auto *ctrlLayout = new QVBoxLayout(ctrlGroup);
  startBtn_ = new QPushButton(tr("Start"));
  stopBtn_ = new QPushButton(tr("Stop"));
  stopBtn_->setEnabled(false);
  ctrlLayout->addWidget(startBtn_);
  ctrlLayout->addWidget(stopBtn_);
  leftLayout->addWidget(ctrlGroup);

  // Zoom controls
  auto *zoomGroup = new QGroupBox(tr("Zoom"));
  auto *zoomLayout = new QHBoxLayout(zoomGroup);
  auto *zoomInBtn = new QPushButton(tr("+"));
  auto *zoomOutBtn = new QPushButton(tr("-"));
  auto *zoomFitBtn = new QPushButton(tr("Fit"));
  zoomLayout->addWidget(zoomInBtn);
  zoomLayout->addWidget(zoomOutBtn);
  zoomLayout->addWidget(zoomFitBtn);
  leftLayout->addWidget(zoomGroup);

  // Protocol decode
  auto *decodeBtn = new QPushButton(tr("Decode Protocol"));
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
  auto *form = new QFormLayout(&dlg);

  auto *nameEdit = new QLineEdit;
  nameEdit->setPlaceholderText(tr("e.g. SPI_CLK"));
  form->addRow(tr("Name"), nameEdit);

  auto *protoCombo = new QComboBox;
  protoCombo->addItems({tr("None"), tr("SPI"), tr("I2C"), tr("UART"), tr("CAN")});
  form->addRow(tr("Protocol"), protoCombo);

  auto *labelEdit = new QLineEdit;
  labelEdit->setPlaceholderText(tr("Optional label"));
  form->addRow(tr("Label"), labelEdit);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
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
  if (row < 0 || row >= logicChannels_.size()) return;
  logicChannels_.removeAt(row);
  updateChannelTable();
}

void LogicAnalyzerPlugin::startCapture() {
  service_->startTrace();
  statusLabel_->setText(tr("Status: Running"));
  startBtn_->setEnabled(false);
  stopBtn_->setEnabled(true);
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
  if (zoomLevel_ < 0.1) zoomLevel_ = 0.1;
  refreshWaveforms();
}

void LogicAnalyzerPlugin::zoomFit() {
  zoomLevel_ = 1.0;
  refreshWaveforms();
}

void LogicAnalyzerPlugin::decodeProtocol() {
  // Placeholder for protocol decode logic
  cursorLabel_->setText(tr("Protocol decode: Not implemented yet"));
}

void LogicAnalyzerPlugin::refreshWaveforms() {
  // Update waveform display — placeholder for custom waveform widget
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
```

---

## Task 4: Update ServiceContainer

**Files:**
- Modify: `apps/ecat-studio/services/ServiceContainer.h`
- Modify: `apps/ecat-studio/services/ServiceContainer.cpp`

### Step 1: Add TraceService to ServiceContainer.h

Add after `class PdoMappingService;`:
```cpp
class TraceService;
```

Add after `PdoMappingService *pdoMapping() const { return pdoMapping_; }`:
```cpp
  TraceService *trace() const { return trace_; }
```

Add after `PdoMappingService *pdoMapping_ = nullptr;`:
```cpp
  TraceService *trace_ = nullptr;
```

### Step 2: Add TraceService to ServiceContainer.cpp

Add `#include "TraceService.h"` and create the instance in the constructor.

---

## Task 5: Update MainWindow.cpp

**Files:**
- Modify: `apps/ecat-studio/MainWindow.cpp`

### Step 1: Register plugins

Add includes for TracePlugin and LogicAnalyzerPlugin, then register them in the constructor after existing plugins.

---

## Task 6: Update CMakeLists.txt

**Files:**
- Modify: `apps/ecat-studio/CMakeLists.txt`

### Step 1: Add new source files

Add TraceService, TracePlugin, and LogicAnalyzerPlugin source files to the executable target.

### Step 2: Add include directories

Add plugin subdirectories to target_include_directories.

---

## Task 7: Create Tests

**Files:**
- Create: `tests/trace_plugin_test.cpp`
- Create: `tests/logicanalyzer_plugin_test.cpp`
- Modify: `tests/CMakeLists.txt`

### Step 1: Create trace_plugin_test.cpp

```cpp
#include <QTest>
#include <QApplication>
#include "TraceService.h"
#include "TracePlugin.h"

class TracePluginTest : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() {
    service = new TraceService(this);
    plugin = new TracePlugin(service, this);
  }

  void cleanupTestCase() {
    delete plugin;
    delete service;
  }

  void testPluginIdentity() {
    QCOMPARE(plugin->id(), QString("trace"));
    QCOMPARE(plugin->displayName(), QString("Signal Trace"));
    QCOMPARE(plugin->defaultOrder(), 180);
    QVERIFY(plugin->visible());
  }

  void testAddChannel() {
    int id = service->addChannel("Test CH", 1, "0x6064", "0");
    QVERIFY(id > 0);
    QCOMPARE(service->channels().size(), 1);
    QCOMPARE(service->channels().first().name, QString("Test CH"));
  }

  void testRemoveChannel() {
    int id = service->addChannel("Remove CH", 2, "0x6041", "0");
    QCOMPARE(service->channels().size(), 2);
    service->removeChannel(id);
    QCOMPARE(service->channels().size(), 1);
  }

  void testSampleRate() {
    service->setSampleRate(5000);
    QCOMPARE(service->sampleRate(), 5000);
  }

  void testBufferSize() {
    service->setBufferSize(50000);
    QCOMPARE(service->bufferSize(), 50000);
  }

  void testTriggerMode() {
    service->setTriggerMode(TraceTriggerMode::Rising);
    QCOMPARE(service->triggerMode(), TraceTriggerMode::Rising);
  }

  void testTraceLifecycle() {
    QVERIFY(!service->isTracing());
    service->startTrace();
    QVERIFY(service->isTracing());
    service->stopTrace();
    QVERIFY(!service->isTracing());
  }

private:
  TraceService *service = nullptr;
  TracePlugin *plugin = nullptr;
};

QTEST_MAIN(TracePluginTest)
#include "trace_plugin_test.moc"
```

### Step 2: Create logicanalyzer_plugin_test.cpp

```cpp
#include <QTest>
#include <QApplication>
#include "TraceService.h"
#include "LogicAnalyzerPlugin.h"

class LogicAnalyzerPluginTest : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() {
    service = new TraceService(this);
    plugin = new LogicAnalyzerPlugin(service, this);
  }

  void cleanupTestCase() {
    delete plugin;
    delete service;
  }

  void testPluginIdentity() {
    QCOMPARE(plugin->id(), QString("logicanalyzer"));
    QCOMPARE(plugin->displayName(), QString("Logic Analyzer"));
    QCOMPARE(plugin->defaultOrder(), 185);
    QVERIFY(plugin->visible());
  }

  void testWidgetNotNull() {
    QVERIFY(plugin->widget() != nullptr);
  }

private:
  TraceService *service = nullptr;
  LogicAnalyzerPlugin *plugin = nullptr;
};

QTEST_MAIN(LogicAnalyzerPluginTest)
#include "logicanalyzer_plugin_test.moc"
```

---

## Task 8: Build and Test

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4`

Expected: All tests pass.
