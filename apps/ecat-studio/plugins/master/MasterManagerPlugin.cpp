#include "MasterManagerPlugin.h"
#include "services/MasterManagerService.h"
#include "services/DistributedClockService.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

MasterManagerPlugin::MasterManagerPlugin(MasterManagerService *masterService,
                                         DistributedClockService *dcService,
                                         QObject *parent)
    : masterService_(masterService), dcService_(dcService) {
  if (parent) setParent(parent);
  buildUi();

  connect(masterService_, &MasterManagerService::masterInfoUpdated, this,
          [this](const MasterMgrInfo &) { updateMasterInfo(); });

  connect(masterService_, &MasterManagerService::masterStateChanged, this,
          [this](const MasterMgrState &) { updateMasterInfo(); });

  connect(masterService_, &MasterManagerService::masterError, this,
          [this](const QString &error) { appendLog("Error: " + error); });

  connect(dcService_, &DistributedClockService::driftDetected, this,
          [this](int slave, double drift) {
            appendLog(QString("DC drift on slave %1: %2 ns").arg(slave).arg(drift));
          });
}

QString MasterManagerPlugin::id() const { return "master"; }
QString MasterManagerPlugin::displayName() const { return "Master Manager"; }
QString MasterManagerPlugin::displayNameZh() const {
  return QStringLiteral("主站管理");
}
QIcon MasterManagerPlugin::icon() const {
  return QIcon::fromTheme("network-server");
}
int MasterManagerPlugin::defaultOrder() const { return 155; }
bool MasterManagerPlugin::visible() const { return false; }

void MasterManagerPlugin::activate() { updateMasterInfo(); }
void MasterManagerPlugin::deactivate() {}

QWidget *MasterManagerPlugin::widget() { return container_; }

void MasterManagerPlugin::buildUi() {
  container_ = new QWidget;
  auto *layout = new QVBoxLayout(container_);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(10);

  auto *statusBar = new QHBoxLayout;
  statusBar->setSpacing(12);

  stateLabel_ = new QLabel(tr("State: Unknown"));
  statusBar->addWidget(stateLabel_);

  adapterLabel_ = new QLabel(tr("Adapter: --"));
  statusBar->addWidget(adapterLabel_);

  slaveCountLabel_ = new QLabel(tr("Slaves: 0"));
  statusBar->addWidget(slaveCountLabel_);

  errorCountLabel_ = new QLabel(tr("Errors: 0"));
  statusBar->addWidget(errorCountLabel_);

  statusBar->addStretch();
  layout->addLayout(statusBar);

  infoTable_ = new QTableWidget;
  infoTable_->setColumnCount(2);
  infoTable_->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  infoTable_->horizontalHeader()->setStretchLastSection(true);
  infoTable_->verticalHeader()->setVisible(false);
  infoTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  infoTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  infoTable_->setShowGrid(false);
  infoTable_->setAlternatingRowColors(true);
  infoTable_->setRowCount(8);
  const QStringList props = {
      tr("Version"), tr("Build Date"), tr("Adapter Name"), tr("Adapter MAC"),
      tr("Cycle Time"), tr("Sync0 Time"), tr("Error Count"), tr("State")};
  for (int i = 0; i < props.size(); ++i) {
    infoTable_->setItem(i, 0, new QTableWidgetItem(props[i]));
    infoTable_->setItem(i, 1, new QTableWidgetItem("--"));
  }
  layout->addWidget(infoTable_, 1);

  auto *toolbar = new QHBoxLayout;
  toolbar->setSpacing(8);

  diagnoseBtn_ = new QPushButton(tr("Diagnose"));
  toolbar->addWidget(diagnoseBtn_);

  restartBtn_ = new QPushButton(tr("Restart Master"));
  toolbar->addWidget(restartBtn_);

  toolbar->addStretch();
  layout->addLayout(toolbar);

  logViewer_ = new QPlainTextEdit;
  logViewer_->setReadOnly(true);
  logViewer_->setMaximumHeight(150);
  logViewer_->setPlaceholderText(tr("Master log messages..."));
  layout->addWidget(logViewer_);

  connect(diagnoseBtn_, &QPushButton::clicked, this,
          [this]() { updateDiagnostics(); });

  connect(restartBtn_, &QPushButton::clicked, this, [this]() {
    appendLog("Restarting master...");
    if (masterService_->restartMaster()) {
      appendLog("Master restart initiated.");
    } else {
      appendLog("Failed to restart master (not connected?).");
    }
  });
}

void MasterManagerPlugin::updateMasterInfo() {
  const MasterMgrInfo info = masterService_->masterInfo();

  const auto stateToString = [](MasterMgrState s) -> QString {
    switch (s) {
    case MasterMgrState::Idle: return "Idle";
    case MasterMgrState::Active: return "Active";
    case MasterMgrState::Error: return "Error";
    case MasterMgrState::Configuring: return "Configuring";
    default: return "Unknown";
    }
  };

  stateLabel_->setText(tr("State: %1").arg(stateToString(info.masterState)));
  adapterLabel_->setText(tr("Adapter: %1").arg(info.adapterName));
  slaveCountLabel_->setText(tr("Slaves: %1").arg(info.slaveCount));
  errorCountLabel_->setText(tr("Errors: %1").arg(info.errorCount));

  auto setRow = [&](int row, const QString &val) {
    if (infoTable_->item(row, 1))
      infoTable_->item(row, 1)->setText(val);
  };
  setRow(0, info.version);
  setRow(1, info.buildDate);
  setRow(2, info.adapterName);
  setRow(3, info.adapterMac);
  setRow(4, QString::number(info.cycleTime));
  setRow(5, QString::number(info.sync0Time));
  setRow(6, QString::number(info.errorCount));
  setRow(7, stateToString(info.masterState));
}

void MasterManagerPlugin::updateDiagnostics() {
  const MasterMgrDiagnosticResult result = masterService_->diagnoseMaster();
  appendLog("=== Diagnostics ===");
  appendLog(result.summary);
  for (const QString &detail : result.details) {
    appendLog("  " + detail);
  }

  const DriftStatus drift = dcService_->driftStatus();
  appendLog(QString("DC drift: slave=%1 drift=%2 ns status=%3")
                .arg(drift.slave)
                .arg(drift.drift)
                .arg(drift.status));

  const JitterStats jitter = dcService_->jitterStatistics();
  appendLog(QString("Jitter: min=%1 max=%2 avg=%3 samples=%4")
                .arg(jitter.min)
                .arg(jitter.max)
                .arg(jitter.avg)
                .arg(jitter.sampleCount));
}

void MasterManagerPlugin::appendLog(const QString &message) {
  logViewer_->appendPlainText(
      QDateTime::currentDateTime().toString("hh:mm:ss") + " " + message);
}
