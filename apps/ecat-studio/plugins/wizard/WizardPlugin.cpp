#include "WizardPlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

WizardPlugin::WizardPlugin(QObject *parent) {
  if (parent) setParent(parent);
  wizards_ = {
      {"network_setup", "Network Setup", "Configuration",
       "Configure EtherCAT network topology and master settings.", 4},
      {"slave_discovery", "Slave Discovery", "Scanning",
       "Discover and register all slaves on the network.", 3},
      {"pdo_mapping", "PDO Mapping", "Configuration",
       "Map process data objects for real-time exchange.", 5},
      {"sdo_config", "SDO Configuration", "Configuration",
       "Configure service data objects for slave parameters.", 4},
      {"dc_sync_setup", "DC Sync Setup", "Synchronization",
       "Configure distributed clock synchronization.", 3},
      {"diagnostics_check", "Diagnostics Check", "Maintenance",
       "Run comprehensive diagnostics on the network.", 3},
  };
  buildUi();
}

QString WizardPlugin::id() const { return "wizard"; }
QString WizardPlugin::displayName() const { return "Wizard"; }
QString WizardPlugin::displayNameZh() const { return "向导"; }
int WizardPlugin::defaultOrder() const { return 220; }
bool WizardPlugin::visible() const { return true; }

void WizardPlugin::activate() {}
void WizardPlugin::deactivate() {}

QWidget *WizardPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void WizardPlugin::addWizard(const WizardEntry &entry) {
  wizards_.append(entry);
  if (wizardList_) {
    wizardList_->addItem(entry.name + " [" + entry.category + "]");
  }
}

int WizardPlugin::wizardCount() const { return wizards_.size(); }

void WizardPlugin::setWizardSteps(const QString &wizardId,
                                   const QVector<WizardStep> &steps) {
  int idx = findWizardIndex(wizardId);
  if (idx >= 0) {
    currentSteps_ = steps;
    if (running_ && runningWizardId_ == wizardId) {
      updateStepView();
    }
  }
}

int WizardPlugin::currentStep() const { return currentStep_; }
int WizardPlugin::totalSteps() const { return currentSteps_.size(); }
bool WizardPlugin::isRunning() const { return running_; }

void WizardPlugin::startWizard(const QString &wizardId) {
  int idx = findWizardIndex(wizardId);
  if (idx < 0) return;
  runningWizardId_ = wizardId;
  currentStep_ = 0;
  running_ = true;
  const auto &w = wizards_[idx];
  currentSteps_.clear();
  for (int i = 0; i < w.stepCount; ++i) {
    currentSteps_.append({QString("Step %1").arg(i + 1),
                          QString("Execute step %1 of wizard '%2'").arg(i + 1).arg(w.name),
                          QString("Tip for step %1").arg(i + 1)});
  }
  updateStepView();
  emit wizardStarted(wizardId);
  emit stepChanged(0);
}

void WizardPlugin::nextStep() {
  if (!running_ || currentStep_ >= currentSteps_.size() - 1) return;
  ++currentStep_;
  updateStepView();
  emit stepChanged(currentStep_);
}

void WizardPlugin::previousStep() {
  if (!running_ || currentStep_ <= 0) return;
  --currentStep_;
  updateStepView();
  emit stepChanged(currentStep_);
}

void WizardPlugin::finishWizard(bool success) {
  if (!running_) return;
  WizardHistoryEntry h;
  h.wizardId = runningWizardId_;
  int idx = findWizardIndex(runningWizardId_);
  h.wizardName = (idx >= 0) ? wizards_[idx].name : runningWizardId_;
  h.completedAt = QDateTime::currentDateTime();
  h.success = success;
  history_.append(h);
  rebuildHistoryTable();
  emit wizardFinished(runningWizardId_, success);
  running_ = false;
  runningWizardId_.clear();
  currentSteps_.clear();
  currentStep_ = 0;
  updateStepView();
}

void WizardPlugin::cancelWizard() {
  if (!running_) return;
  running_ = false;
  runningWizardId_.clear();
  currentSteps_.clear();
  currentStep_ = 0;
  updateStepView();
}

int WizardPlugin::historyCount() const { return history_.size(); }
QListWidget *WizardPlugin::wizardList() const { return wizardList_; }
QTableWidget *WizardPlugin::stepTable() const { return stepTable_; }
QTableWidget *WizardPlugin::historyTable() const { return historyTable_; }

void WizardPlugin::exportHistory(const QString &path) {
  QFile f(path);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    for (const auto &h : history_) {
      out << h.wizardId << "," << h.wizardName << ","
          << h.completedAt.toString(Qt::ISODate) << ","
          << (h.success ? "success" : "failed") << "\n";
    }
  }
}

void WizardPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  auto *splitter = new QSplitter;

  wizardList_ = new QListWidget;
  for (const auto &w : wizards_) {
    wizardList_->addItem(w.name + " [" + w.category + "]");
  }
  splitter->addWidget(wizardList_);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);

  auto *btnRow = new QWidget;
  auto *btnLayout = new QHBoxLayout(btnRow);
  startBtn_ = new QPushButton("Start");
  nextBtn_ = new QPushButton("Next");
  prevBtn_ = new QPushButton("Previous");
  finishBtn_ = new QPushButton("Finish");
  cancelBtn_ = new QPushButton("Cancel");
  btnLayout->addWidget(startBtn_);
  btnLayout->addWidget(prevBtn_);
  btnLayout->addWidget(nextBtn_);
  btnLayout->addWidget(finishBtn_);
  btnLayout->addWidget(cancelBtn_);
  rightLayout->addWidget(btnRow);

  stepLabel_ = new QLabel("No wizard running");
  rightLayout->addWidget(stepLabel_);

  instructionView_ = new QTextEdit;
  instructionView_->setReadOnly(true);
  rightLayout->addWidget(instructionView_);

  tabs_ = new QTabWidget;
  stepTable_ = new QTableWidget;
  stepTable_->setColumnCount(3);
  stepTable_->setHorizontalHeaderLabels({"Step", "Title", "Instruction"});
  tabs_->addTab(stepTable_, "Steps");

  historyTable_ = new QTableWidget;
  historyTable_->setColumnCount(4);
  historyTable_->setHorizontalHeaderLabels({"Wizard", "Completed", "Status", "ID"});
  tabs_->addTab(historyTable_, "History");

  rightLayout->addWidget(tabs_);
  splitter->addWidget(rightPanel);
  mainLayout->addWidget(splitter);

  connect(startBtn_, &QPushButton::clicked, this, [this]() {
    int row = wizardList_->currentRow();
    if (row >= 0 && row < wizards_.size()) {
      startWizard(wizards_[row].id);
    }
  });
  connect(nextBtn_, &QPushButton::clicked, this, &WizardPlugin::nextStep);
  connect(prevBtn_, &QPushButton::clicked, this, &WizardPlugin::previousStep);
  connect(finishBtn_, &QPushButton::clicked, this, [this]() { finishWizard(true); });
  connect(cancelBtn_, &QPushButton::clicked, this, &WizardPlugin::cancelWizard);
}

void WizardPlugin::updateStepView() {
  if (!running_) {
    if (stepLabel_) stepLabel_->setText("No wizard running");
    if (instructionView_) instructionView_->clear();
    if (stepTable_) {
      stepTable_->setRowCount(0);
    }
    return;
  }
  if (stepLabel_) {
    stepLabel_->setText(tr("Step %1 of %2").arg(currentStep_ + 1).arg(currentSteps_.size()));
  }
  if (instructionView_ && currentStep_ < currentSteps_.size()) {
    const auto &s = currentSteps_[currentStep_];
    instructionView_->setText(s.title + "\n\n" + s.instruction + "\n\nTip: " + s.tip);
  }
  if (stepTable_) {
    stepTable_->setRowCount(currentSteps_.size());
    for (int i = 0; i < currentSteps_.size(); ++i) {
      stepTable_->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
      stepTable_->setItem(i, 1, new QTableWidgetItem(currentSteps_[i].title));
      stepTable_->setItem(i, 2, new QTableWidgetItem(currentSteps_[i].instruction));
    }
  }
}

void WizardPlugin::rebuildHistoryTable() {
  if (!historyTable_) return;
  historyTable_->setRowCount(history_.size());
  for (int i = 0; i < history_.size(); ++i) {
    const auto &h = history_[i];
    historyTable_->setItem(i, 0, new QTableWidgetItem(h.wizardName));
    historyTable_->setItem(i, 1, new QTableWidgetItem(h.completedAt.toString(Qt::ISODate)));
    historyTable_->setItem(i, 2, new QTableWidgetItem(h.success ? "success" : "failed"));
    historyTable_->setItem(i, 3, new QTableWidgetItem(h.wizardId));
  }
}

int WizardPlugin::findWizardIndex(const QString &wizardId) const {
  for (int i = 0; i < wizards_.size(); ++i) {
    if (wizards_[i].id == wizardId) return i;
  }
  return -1;
}
