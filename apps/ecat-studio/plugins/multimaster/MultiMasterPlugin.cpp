#include "MultiMasterPlugin.h"
#include "MasterComparisonWidget.h"
#include "services/MultiMasterService.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>

MultiMasterPlugin::MultiMasterPlugin(MultiMasterService *service,
                                     QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &MultiMasterService::masterDiscovered, this,
          [this](const MmMasterInfo &) { refreshMasterList(); });

  connect(service_, &MultiMasterService::masterStatusChanged, this,
          [this](int masterId, const MmMasterStatus &) {
            if (masterId == selectedMasterId_) {
              updateMasterDetails(masterId);
            }
          });

  connect(service_, &MultiMasterService::masterSyncCompleted, this,
          [this](const MmMasterSyncResult &result) {
            syncLog_->appendPlainText(
                QDateTime::currentDateTime().toString("hh:mm:ss") + " " +
                result.message);
          });

  connect(service_, &MultiMasterService::masterError, this,
          [this](int masterId, const QString &error) {
            syncLog_->appendPlainText(
                QDateTime::currentDateTime().toString("hh:mm:ss") +
                QString(" Error on master %1: %2").arg(masterId).arg(error));
          });
}

QString MultiMasterPlugin::id() const { return "multimaster"; }
QString MultiMasterPlugin::displayName() const { return "Multi-Master"; }
QString MultiMasterPlugin::displayNameZh() const {
  return QStringLiteral("多主站");
}
QIcon MultiMasterPlugin::icon() const {
  return QIcon::fromTheme("network-server");
}
int MultiMasterPlugin::defaultOrder() const { return 30; }
bool MultiMasterPlugin::visible() const { return true; }

void MultiMasterPlugin::activate() { refreshMasterList(); }
void MultiMasterPlugin::deactivate() {}

QWidget *MultiMasterPlugin::widget() { return containerWidget_; }

int MultiMasterPlugin::masterCount() const { return service_->masterCount(); }

int MultiMasterPlugin::selectedMasterId() const { return selectedMasterId_; }

void MultiMasterPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(8);

  auto *toolbar = new QHBoxLayout;
  toolbar->setSpacing(8);

  refreshBtn_ = new QPushButton(tr("Refresh"));
  toolbar->addWidget(refreshBtn_);

  addMasterBtn_ = new QPushButton(tr("Add Master"));
  toolbar->addWidget(addMasterBtn_);

  removeMasterBtn_ = new QPushButton(tr("Remove Master"));
  toolbar->addWidget(removeMasterBtn_);

  syncBtn_ = new QPushButton(tr("Sync Selected"));
  toolbar->addWidget(syncBtn_);

  toolbar->addStretch();

  statusLabel_ = new QLabel(tr("Masters: 0"));
  toolbar->addWidget(statusLabel_);

  exportBtn_ = new QPushButton(tr("Export Report"));
  toolbar->addWidget(exportBtn_);

  mainLayout->addLayout(toolbar);

  tabWidget_ = new QTabWidget;

  auto *listWidget = new QWidget;
  auto *listLayout = new QVBoxLayout(listWidget);
  listLayout->setContentsMargins(4, 4, 4, 4);

  masterListTable_ = new QTableWidget;
  masterListTable_->setColumnCount(6);
  masterListTable_->setHorizontalHeaderLabels(
      {tr("ID"), tr("Adapter"), tr("Slaves"), tr("State"), tr("IP"), tr("MAC")});
  masterListTable_->horizontalHeader()->setStretchLastSection(true);
  masterListTable_->verticalHeader()->setVisible(false);
  masterListTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  masterListTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  masterListTable_->setAlternatingRowColors(true);
  listLayout->addWidget(masterListTable_);
  tabWidget_->addTab(listWidget, tr("Master List"));

  auto *detailWidget = new QWidget;
  auto *detailLayout = new QVBoxLayout(detailWidget);
  detailLayout->setContentsMargins(4, 4, 4, 4);

  detailTable_ = new QTableWidget;
  detailTable_->setColumnCount(2);
  detailTable_->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  detailTable_->horizontalHeader()->setStretchLastSection(true);
  detailTable_->verticalHeader()->setVisible(false);
  detailTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  detailTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  detailTable_->setAlternatingRowColors(true);
  detailLayout->addWidget(detailTable_);
  tabWidget_->addTab(detailWidget, tr("Master Details"));

  comparisonWidget_ = new MasterComparisonWidget;
  tabWidget_->addTab(comparisonWidget_, tr("Comparison"));

  auto *syncWidget = new QWidget;
  auto *syncLayout = new QVBoxLayout(syncWidget);
  syncLayout->setContentsMargins(4, 4, 4, 4);

  syncLog_ = new QPlainTextEdit;
  syncLog_->setReadOnly(true);
  syncLog_->setPlaceholderText(tr("Synchronization log..."));
  syncLayout->addWidget(syncLog_);
  tabWidget_->addTab(syncWidget, tr("Synchronization"));

  mainLayout->addWidget(tabWidget_, 1);

  connect(refreshBtn_, &QPushButton::clicked, this,
          &MultiMasterPlugin::refreshMasterList);

  connect(addMasterBtn_, &QPushButton::clicked, this, [this]() {
    MmMasterInfo info;
    info.adapterName = QString("eth%1").arg(service_->masterCount());
    info.state = MultiMasterState::Idle;
    service_->addMaster(info);
    refreshMasterList();
  });

  connect(removeMasterBtn_, &QPushButton::clicked, this, [this]() {
    if (selectedMasterId_ >= 0) {
      service_->removeMaster(selectedMasterId_);
      selectedMasterId_ = -1;
      refreshMasterList();
    }
  });

  connect(syncBtn_, &QPushButton::clicked, this, [this]() {
    auto masters = service_->allMasters();
    if (masters.size() >= 2) {
      int src = masters.first().masterId;
      int dst = masters.last().masterId;
      syncLog_->appendPlainText(
          QDateTime::currentDateTime().toString("hh:mm:ss") +
          QString(" Syncing master %1 → %2...").arg(src).arg(dst));
      service_->synchronizeMasters(src, dst);
    }
  });

  connect(exportBtn_, &QPushButton::clicked, this,
          &MultiMasterPlugin::exportReport);

  connect(masterListTable_, &QTableWidget::cellClicked, this,
          [this](int row, int col) {
            Q_UNUSED(col);
            auto *idItem = masterListTable_->item(row, 0);
            if (idItem) {
              selectedMasterId_ = idItem->text().toInt();
              updateMasterDetails(selectedMasterId_);
            }
          });
}

void MultiMasterPlugin::refreshMasterList() {
  auto masters = service_->allMasters();
  masterListTable_->setRowCount(masters.size());

  auto stateToString = [](MultiMasterState s) -> QString {
    switch (s) {
    case MultiMasterState::Idle: return "Idle";
    case MultiMasterState::Active: return "Active";
    case MultiMasterState::Error: return "Error";
    case MultiMasterState::Syncing: return "Syncing";
    default: return "Unknown";
    }
  };

  for (int i = 0; i < masters.size(); ++i) {
    const auto &m = masters[i];
    masterListTable_->setItem(i, 0, new QTableWidgetItem(QString::number(m.masterId)));
    masterListTable_->setItem(i, 1, new QTableWidgetItem(m.adapterName));
    masterListTable_->setItem(i, 2, new QTableWidgetItem(QString::number(m.slaveCount)));
    masterListTable_->setItem(i, 3, new QTableWidgetItem(stateToString(m.state)));
    masterListTable_->setItem(i, 4, new QTableWidgetItem(m.ipAddress));
    masterListTable_->setItem(i, 5, new QTableWidgetItem(m.macAddress));
  }

  statusLabel_->setText(tr("Masters: %1").arg(masters.size()));
}

void MultiMasterPlugin::updateMasterDetails(int masterId) {
  auto info = service_->masterInfo(masterId);
  auto status = service_->monitorMaster(masterId);

  detailTable_->setRowCount(8);

  auto setRow = [&](int row, const QString &prop, const QString &val) {
    detailTable_->setItem(row, 0, new QTableWidgetItem(prop));
    detailTable_->setItem(row, 1, new QTableWidgetItem(val));
  };

  setRow(0, tr("Master ID"), QString::number(info.masterId));
  setRow(1, tr("Adapter"), info.adapterName);
  setRow(2, tr("Slave Count"), QString::number(info.slaveCount));
  setRow(3, tr("State"), QString::number(static_cast<int>(info.state)));
  setRow(4, tr("IP Address"), info.ipAddress);
  setRow(5, tr("MAC Address"), info.macAddress);
  setRow(6, tr("Error Count"), QString::number(status.errorCount));
  setRow(7, tr("Summary"), status.summary);
}

void MultiMasterPlugin::exportReport() {
  QString path = QFileDialog::getSaveFileName(
      containerWidget_, tr("Export Master Report"), "master_report.csv",
      tr("CSV Files (*.csv);;All Files (*)"));
  if (path.isEmpty()) return;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

  QTextStream out(&file);
  out << "Master ID,Adapter,Slave Count,State,IP Address,MAC Address\n";

  auto masters = service_->allMasters();
  for (const auto &m : masters) {
    out << m.masterId << "," << m.adapterName << "," << m.slaveCount << ","
        << static_cast<int>(m.state) << "," << m.ipAddress << ","
        << m.macAddress << "\n";
  }
}
