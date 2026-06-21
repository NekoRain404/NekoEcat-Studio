#include "SimulationPlugin.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

static constexpr int kStatsRows = 6;
static constexpr int kDataCols = 6;
static constexpr int kSettingsRows = 4;

SimulationPlugin::SimulationPlugin(QObject *parent) {
  if (parent) setParent(parent);
  simTimer_ = new QTimer(this);
  simTimer_->setInterval(100);
  connect(simTimer_, &QTimer::timeout, this, &SimulationPlugin::onSimulationTimer);
  buildUi();
}

QString SimulationPlugin::id() const { return "simulation"; }
QString SimulationPlugin::displayName() const { return "Simulation"; }
QString SimulationPlugin::displayNameZh() const { return QStringLiteral("仿真"); }
QIcon SimulationPlugin::icon() const { return QIcon::fromTheme("media-playback-start"); }
int SimulationPlugin::defaultOrder() const { return 205; }
bool SimulationPlugin::visible() const { return true; }

void SimulationPlugin::activate() {}
void SimulationPlugin::deactivate() {}

QWidget *SimulationPlugin::widget() { return containerWidget_; }

SimulationPlugin::SimState SimulationPlugin::simulationState() const { return state_; }
bool SimulationPlugin::isRunning() const { return state_ == SimState::Running; }
bool SimulationPlugin::isPaused() const { return state_ == SimState::Paused; }

void SimulationPlugin::setCycleTimeUs(int us) { cycleTimeSpin_->setValue(us); }
int SimulationPlugin::cycleTimeUs() const { return cycleTimeSpin_->value(); }

void SimulationPlugin::setSlaveCount(int count) { slaveCountSpin_->setValue(count); }
int SimulationPlugin::slaveCount() const { return slaveCountSpin_->value(); }

void SimulationPlugin::setSimulationDuration(int seconds) { durationSpin_->setValue(seconds); }
int SimulationPlugin::simulationDuration() const { return durationSpin_->value(); }

int SimulationPlugin::frameCount() const { return frameCount_; }
double SimulationPlugin::averageLatencyUs() const {
  return frameCount_ > 0 ? totalLatencyUs_ / frameCount_ : 0.0;
}
double SimulationPlugin::maxLatencyUs() const { return maxLatencyUs_; }
double SimulationPlugin::minLatencyUs() const { return minLatencyUs_ < 1e9 ? minLatencyUs_ : 0.0; }
int SimulationPlugin::errorCount() const { return errorCount_; }

QTableWidget *SimulationPlugin::statisticsTable() const { return statisticsTable_; }
QTableWidget *SimulationPlugin::dataViewTable() const { return dataViewTable_; }
QTextEdit *SimulationPlugin::logView() const { return logView_; }

void SimulationPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *toolbar = new QWidget;
  auto *toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(4, 2, 4, 2);

  startBtn_ = new QPushButton(tr("Start"));
  stopBtn_ = new QPushButton(tr("Stop"));
  pauseBtn_ = new QPushButton(tr("Pause"));
  stepBtn_ = new QPushButton(tr("Step"));
  resetBtn_ = new QPushButton(tr("Reset"));
  exportBtn_ = new QPushButton(tr("Export"));

  stopBtn_->setEnabled(false);
  pauseBtn_->setEnabled(false);

  toolbarLayout->addWidget(startBtn_);
  toolbarLayout->addWidget(stopBtn_);
  toolbarLayout->addWidget(pauseBtn_);
  toolbarLayout->addWidget(stepBtn_);
  toolbarLayout->addWidget(resetBtn_);
  toolbarLayout->addWidget(exportBtn_);

  stateLabel_ = new QLabel(tr("Idle"));
  stateLabel_->setStyleSheet("font-weight: bold; padding: 0 8px;");
  toolbarLayout->addWidget(stateLabel_);
  toolbarLayout->addStretch();
  mainLayout->addWidget(toolbar);

  auto *settingsBar = new QWidget;
  auto *settingsLayout = new QHBoxLayout(settingsBar);
  settingsLayout->setContentsMargins(4, 2, 4, 2);

  settingsLayout->addWidget(new QLabel(tr("Cycle Time (us):")));
  cycleTimeSpin_ = new QSpinBox;
  cycleTimeSpin_->setRange(100, 100000);
  cycleTimeSpin_->setValue(1000);
  settingsLayout->addWidget(cycleTimeSpin_);

  settingsLayout->addWidget(new QLabel(tr("Slaves:")));
  slaveCountSpin_ = new QSpinBox;
  slaveCountSpin_->setRange(1, 256);
  slaveCountSpin_->setValue(4);
  settingsLayout->addWidget(slaveCountSpin_);

  settingsLayout->addWidget(new QLabel(tr("Duration (s):")));
  durationSpin_ = new QSpinBox;
  durationSpin_->setRange(1, 3600);
  durationSpin_->setValue(10);
  settingsLayout->addWidget(durationSpin_);

  settingsLayout->addWidget(new QLabel(tr("Mode:")));
  modeCombo_ = new QComboBox;
  modeCombo_->addItems({tr("Normal"), tr("High Load"), tr("Stress Test"), tr("Custom")});
  settingsLayout->addWidget(modeCombo_);
  settingsLayout->addStretch();
  mainLayout->addWidget(settingsBar);

  auto *splitter = new QSplitter(Qt::Vertical);

  auto *topSplitter = new QSplitter(Qt::Horizontal);

  auto *statsPanel = new QWidget;
  auto *statsLayout = new QVBoxLayout(statsPanel);
  statsLayout->setContentsMargins(4, 4, 4, 4);
  statsLayout->addWidget(new QLabel(tr("Statistics")));
  statisticsTable_ = new QTableWidget;
  statisticsTable_->setColumnCount(2);
  statisticsTable_->setHorizontalHeaderLabels({tr("Metric"), tr("Value")});
  statisticsTable_->horizontalHeader()->setStretchLastSection(true);
  statisticsTable_->setRowCount(kStatsRows);
  QStringList statsLabels = {tr("Frames"), tr("Avg Latency (us)"),
                             tr("Max Latency (us)"), tr("Min Latency (us)"),
                             tr("Errors"), tr("Elapsed (s)")};
  for (int i = 0; i < kStatsRows; ++i) {
    statisticsTable_->setItem(i, 0, new QTableWidgetItem(statsLabels[i]));
    statisticsTable_->setItem(i, 1, new QTableWidgetItem("0"));
  }
  statisticsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  statsLayout->addWidget(statisticsTable_);
  topSplitter->addWidget(statsPanel);

  auto *dataPanel = new QWidget;
  auto *dataLayout = new QVBoxLayout(dataPanel);
  dataLayout->setContentsMargins(4, 4, 4, 4);
  dataLayout->addWidget(new QLabel(tr("Simulation Data")));
  dataViewTable_ = new QTableWidget;
  dataViewTable_->setColumnCount(kDataCols);
  dataViewTable_->setHorizontalHeaderLabels(
      {tr("Frame"), tr("Time"), tr("Slave"), tr("Type"), tr("Data"), tr("Status")});
  dataViewTable_->horizontalHeader()->setStretchLastSection(true);
  dataViewTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  dataViewTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  dataLayout->addWidget(dataViewTable_);
  topSplitter->addWidget(dataPanel);

  topSplitter->setStretchFactor(0, 1);
  topSplitter->setStretchFactor(1, 2);
  splitter->addWidget(topSplitter);

  auto *logPanel = new QWidget;
  auto *logLayout = new QVBoxLayout(logPanel);
  logLayout->setContentsMargins(4, 4, 4, 4);
  logLayout->addWidget(new QLabel(tr("Simulation Log")));
  logView_ = new QTextEdit;
  logView_->setReadOnly(true);
  logView_->setMaximumHeight(150);
  logLayout->addWidget(logView_);
  splitter->addWidget(logPanel);

  mainLayout->addWidget(splitter);

  connect(startBtn_, &QPushButton::clicked, this, &SimulationPlugin::startSimulation);
  connect(stopBtn_, &QPushButton::clicked, this, &SimulationPlugin::stopSimulation);
  connect(pauseBtn_, &QPushButton::clicked, this, &SimulationPlugin::pauseSimulation);
  connect(stepBtn_, &QPushButton::clicked, this, &SimulationPlugin::stepSimulation);
  connect(resetBtn_, &QPushButton::clicked, this, &SimulationPlugin::resetStatistics);
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getSaveFileName(
        containerWidget_, tr("Export Simulation Results"), "simulation_results.csv",
        tr("CSV Files (*.csv)"));
    if (!path.isEmpty()) exportResults(path);
  });
}

void SimulationPlugin::startSimulation() {
  if (state_ == SimState::Paused) {
    state_ = SimState::Running;
    simTimer_->start();
    stateLabel_->setText(tr("Running"));
    startBtn_->setEnabled(false);
    pauseBtn_->setEnabled(true);
    stepBtn_->setEnabled(false);
    emit simulationStateChanged(state_);
    addLogEntry(tr("Simulation resumed"));
    return;
  }
  state_ = SimState::Running;
  resetStatistics();
  simTimer_->start();
  stateLabel_->setText(tr("Running"));
  startBtn_->setEnabled(false);
  stopBtn_->setEnabled(true);
  pauseBtn_->setEnabled(true);
  stepBtn_->setEnabled(false);
  emit simulationStateChanged(state_);
  addLogEntry(tr("Simulation started with %1 slaves, cycle %2 us")
                  .arg(slaveCountSpin_->value())
                  .arg(cycleTimeSpin_->value()));
}

void SimulationPlugin::stopSimulation() {
  state_ = SimState::Idle;
  simTimer_->stop();
  stateLabel_->setText(tr("Idle"));
  startBtn_->setEnabled(true);
  stopBtn_->setEnabled(false);
  pauseBtn_->setEnabled(false);
  stepBtn_->setEnabled(true);
  emit simulationStateChanged(state_);
  addLogEntry(tr("Simulation stopped after %1 frames").arg(frameCount_));
}

void SimulationPlugin::pauseSimulation() {
  state_ = SimState::Paused;
  simTimer_->stop();
  stateLabel_->setText(tr("Paused"));
  startBtn_->setEnabled(true);
  pauseBtn_->setEnabled(false);
  stepBtn_->setEnabled(true);
  emit simulationStateChanged(state_);
  addLogEntry(tr("Simulation paused at frame %1").arg(frameCount_));
}

void SimulationPlugin::stepSimulation() {
  onSimulationTimer();
  addLogEntry(tr("Step: frame %1").arg(frameCount_));
}

void SimulationPlugin::resetStatistics() {
  frameCount_ = 0;
  totalLatencyUs_ = 0.0;
  maxLatencyUs_ = 0.0;
  minLatencyUs_ = 1e9;
  errorCount_ = 0;
  elapsedMs_ = 0;
  dataViewTable_->setRowCount(0);
  updateStatisticsDisplay();
  emit statisticsUpdated();
}

void SimulationPlugin::onSimulationTimer() {
  ++frameCount_;
  elapsedMs_ += 100;

  double latency = 50.0 + (frameCount_ % 100) * 0.5;
  totalLatencyUs_ += latency;
  if (latency > maxLatencyUs_) maxLatencyUs_ = latency;
  if (latency < minLatencyUs_) minLatencyUs_ = latency;

  if (frameCount_ % 50 == 0) ++errorCount_;

  int row = dataViewTable_->rowCount();
  if (row > 200) {
    dataViewTable_->removeRow(0);
    row = dataViewTable_->rowCount();
  }
  dataViewTable_->insertRow(row);
  dataViewTable_->setItem(row, 0, new QTableWidgetItem(QString::number(frameCount_)));
  dataViewTable_->setItem(row, 1,
                          new QTableWidgetItem(QString::number(elapsedMs_ / 1000.0, 'f', 2)));
  dataViewTable_->setItem(row, 2,
                          new QTableWidgetItem(QString::number(frameCount_ % slaveCountSpin_->value())));
  dataViewTable_->setItem(row, 3, new QTableWidgetItem(frameCount_ % 3 == 0 ? "PDO" : "SDO"));
  dataViewTable_->setItem(row, 4, new QTableWidgetItem(
                                      QString("0x%1").arg(frameCount_ & 0xFFFF, 4, 16, QChar('0'))));
  dataViewTable_->setItem(row, 5, new QTableWidgetItem(frameCount_ % 50 == 0 ? "Error" : "OK"));
  dataViewTable_->scrollToBottom();

  updateStatisticsDisplay();
  emit frameProcessed(frameCount_);

  int durationMs = durationSpin_->value() * 1000;
  if (elapsedMs_ >= durationMs) {
    stopSimulation();
  }
}

void SimulationPlugin::updateStatisticsDisplay() {
  statisticsTable_->item(0, 1)->setText(QString::number(frameCount_));
  statisticsTable_->item(1, 1)->setText(QString::number(averageLatencyUs(), 'f', 2));
  statisticsTable_->item(2, 1)->setText(QString::number(maxLatencyUs_, 'f', 2));
  statisticsTable_->item(3, 1)->setText(QString::number(minLatencyUs(), 'f', 2));
  statisticsTable_->item(4, 1)->setText(QString::number(errorCount_));
  statisticsTable_->item(5, 1)->setText(QString::number(elapsedMs_ / 1000.0, 'f', 1));
  emit statisticsUpdated();
}

void SimulationPlugin::addLogEntry(const QString &entry) {
  logView_->append(QStringLiteral("[%1] %2")
                       .arg(QTime::currentTime().toString("HH:mm:ss.zzz"))
                       .arg(entry));
}

void SimulationPlugin::clearLog() { logView_->clear(); }

int SimulationPlugin::logCount() const {
  return logView_->toPlainText().isEmpty() ? 0 : logView_->document()->blockCount();
}

void SimulationPlugin::exportResults(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&file);
  out << "Frame,Time,Slave,Type,Data,Status\n";
  for (int r = 0; r < dataViewTable_->rowCount(); ++r) {
    QStringList cols;
    for (int c = 0; c < kDataCols; ++c) {
      auto *item = dataViewTable_->item(r, c);
      cols << (item ? item->text() : "");
    }
    out << cols.join(",") << "\n";
  }
  addLogEntry(tr("Results exported to %1").arg(path));
}
