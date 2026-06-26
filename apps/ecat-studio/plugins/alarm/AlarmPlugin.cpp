#include "AlarmPlugin.h"
#include "services/AlarmService.h"
#include "services/LoggingService.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

static constexpr int kColId         = 0;
static constexpr int kColTime       = 1;
static constexpr int kColLevel      = 2;
static constexpr int kColCategory   = 3;
static constexpr int kColState      = 4;
static constexpr int kColSource     = 5;
static constexpr int kColMessage    = 6;
static constexpr int kColCount      = 7;

static QString levelToString(AlarmLevel level) {
  switch (level) {
  case AlarmLevel::Info:     return QStringLiteral("Info");
  case AlarmLevel::Warning:  return QStringLiteral("Warning");
  case AlarmLevel::Error:    return QStringLiteral("Error");
  case AlarmLevel::Critical: return QStringLiteral("Critical");
  }
  return QStringLiteral("Info");
}

static QString categoryToString(AlarmCategory cat) {
  switch (cat) {
  case AlarmCategory::Communication: return QStringLiteral("Communication");
  case AlarmCategory::Device:        return QStringLiteral("Device");
  case AlarmCategory::Network:       return QStringLiteral("Network");
  case AlarmCategory::Configuration: return QStringLiteral("Configuration");
  }
  return QStringLiteral("Communication");
}

static QString stateToString(AlarmState state) {
  switch (state) {
  case AlarmState::Active:        return QStringLiteral("Active");
  case AlarmState::Acknowledged:  return QStringLiteral("Acknowledged");
  case AlarmState::Cleared:       return QStringLiteral("Cleared");
  }
  return QStringLiteral("Active");
}

AlarmPlugin::AlarmPlugin(AlarmService *alarmService, LoggingService *logService,
                         QObject *parent)
    : alarmService_(alarmService), logService_(logService) {
  if (parent) setParent(parent);
  buildUi();

  connect(alarmService_, &AlarmService::alarmRaised,
          this, &AlarmPlugin::onAlarmRaised);
  connect(alarmService_, &AlarmService::alarmAcknowledged,
          this, &AlarmPlugin::onAlarmAcknowledged);
  connect(alarmService_, &AlarmService::alarmCleared,
          this, &AlarmPlugin::onAlarmCleared);
}

QString AlarmPlugin::id() const { return "alarm"; }
QString AlarmPlugin::displayName() const { return "Alarms"; }
QString AlarmPlugin::displayNameZh() const { return QStringLiteral("告警"); }
int AlarmPlugin::defaultOrder() const { return 110; }
bool AlarmPlugin::visible() const { return true; }

QWidget *AlarmPlugin::widget() { return container_; }

void AlarmPlugin::buildUi() {
  container_ = new QWidget;
  auto *rootLayout = new QVBoxLayout(container_);
  rootLayout->setContentsMargins(0, 0, 0, 0);

  auto *toolbar = new QWidget;
  auto *toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(4, 2, 4, 2);

  levelFilter_ = new QComboBox;
  levelFilter_->addItems({tr("All Levels"), tr("Info"), tr("Warning"),
                          tr("Error"), tr("Critical")});
  toolbarLayout->addWidget(levelFilter_);

  categoryFilter_ = new QComboBox;
  categoryFilter_->addItems({tr("All Categories"), tr("Communication"),
                             tr("Device"), tr("Network"), tr("Configuration")});
  toolbarLayout->addWidget(categoryFilter_);

  stateFilter_ = new QComboBox;
  stateFilter_->addItems({tr("All States"), tr("Active"), tr("Acknowledged"),
                          tr("Cleared")});
  toolbarLayout->addWidget(stateFilter_);

  ackBtn_ = new QPushButton(tr("Acknowledge"));
  toolbarLayout->addWidget(ackBtn_);

  clearBtn_ = new QPushButton(tr("Clear"));
  toolbarLayout->addWidget(clearBtn_);

  exportBtn_ = new QPushButton(tr("Export"));
  toolbarLayout->addWidget(exportBtn_);

  toolbarLayout->addStretch();
  rootLayout->addWidget(toolbar);

  table_ = new QTableWidget;
  table_->setColumnCount(kColCount);
  table_->setHorizontalHeaderLabels({
    tr("ID"), tr("Time"), tr("Level"), tr("Category"),
    tr("State"), tr("Source"), tr("Message")
  });
  table_->horizontalHeader()->setStretchLastSection(true);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->setSortingEnabled(false);
  rootLayout->addWidget(table_);

  connect(levelFilter_, &QComboBox::currentIndexChanged,
          this, &AlarmPlugin::applyFilter);
  connect(categoryFilter_, &QComboBox::currentIndexChanged,
          this, &AlarmPlugin::applyFilter);
  connect(stateFilter_, &QComboBox::currentIndexChanged,
          this, &AlarmPlugin::applyFilter);
  connect(ackBtn_, &QPushButton::clicked,
          this, &AlarmPlugin::acknowledgeSelected);
  connect(clearBtn_, &QPushButton::clicked,
          this, &AlarmPlugin::clearSelected);
  connect(exportBtn_, &QPushButton::clicked,
          this, &AlarmPlugin::exportHistory);

  for (const auto &alarm : alarmService_->alarmHistory(500)) {
    int row = table_->rowCount();
    table_->setRowCount(row + 1);
    populateRow(row, alarm);
  }
}

void AlarmPlugin::onAlarmRaised(const Alarm &alarm) {
  int row = table_->rowCount();
  table_->setRowCount(row + 1);
  populateRow(row, alarm);
  table_->scrollToBottom();
  applyFilter();
}

void AlarmPlugin::onAlarmAcknowledged(int alarmId) {
  int row = findRowById(alarmId);
  if (row >= 0) {
    updateRowState(row, AlarmState::Acknowledged);
  }
}

void AlarmPlugin::onAlarmCleared(int alarmId) {
  int row = findRowById(alarmId);
  if (row >= 0) {
    updateRowState(row, AlarmState::Cleared);
  }
}

void AlarmPlugin::applyFilter() {
  const QString levelText = levelFilter_->currentText();
  const QString catText = categoryFilter_->currentText();
  const QString stateText = stateFilter_->currentText();

  for (int r = 0; r < table_->rowCount(); ++r) {
    bool show = true;
    if (levelText != tr("All Levels")) {
      auto *item = table_->item(r, kColLevel);
      if (!item || item->text() != levelText) show = false;
    }
    if (show && catText != tr("All Categories")) {
      auto *item = table_->item(r, kColCategory);
      if (!item || item->text() != catText) show = false;
    }
    if (show && stateText != tr("All States")) {
      auto *item = table_->item(r, kColState);
      if (!item || item->text() != stateText) show = false;
    }
    table_->setRowHidden(r, !show);
  }
}

void AlarmPlugin::acknowledgeSelected() {
  const auto rows = table_->selectionModel()->selectedRows();
  for (const auto &index : rows) {
    auto *idItem = table_->item(index.row(), kColId);
    if (idItem) {
      alarmService_->acknowledgeAlarm(idItem->text().toInt());
    }
  }
}

void AlarmPlugin::clearSelected() {
  const auto rows = table_->selectionModel()->selectedRows();
  for (const auto &index : rows) {
    auto *idItem = table_->item(index.row(), kColId);
    if (idItem) {
      alarmService_->clearAlarm(idItem->text().toInt());
    }
  }
}

void AlarmPlugin::exportHistory() {
  QString path = QFileDialog::getSaveFileName(
      container_, tr("Export Alarm History"), "alarms.csv",
      tr("CSV Files (*.csv)"));
  if (path.isEmpty()) return;

  exportHistoryToFile(path);
}

bool AlarmPlugin::exportHistoryToFile(const QString &path) {
  if (path.isEmpty()) return false;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&file);
  out << "ID,Time,Level,Category,State,Source,Message\n";
  for (const auto &a : alarmService_->alarmHistory(10000)) {
    out << a.id << ","
        << a.timestamp.toString(Qt::ISODate) << ","
        << levelToString(a.level) << ","
        << categoryToString(a.category) << ","
        << stateToString(a.state) << ","
        << a.source << ","
        << a.message << "\n";
  }
  return out.status() == QTextStream::Ok && file.flush();
}

void AlarmPlugin::populateRow(int row, const Alarm &alarm) {
  auto *idItem = new QTableWidgetItem(QString::number(alarm.id));
  idItem->setData(Qt::UserRole, alarm.id);
  table_->setItem(row, kColId, idItem);
  table_->setItem(row, kColTime,
                  new QTableWidgetItem(alarm.timestamp.toString(Qt::ISODate)));
  table_->setItem(row, kColLevel, new QTableWidgetItem(levelToString(alarm.level)));
  table_->setItem(row, kColCategory,
                  new QTableWidgetItem(categoryToString(alarm.category)));
  table_->setItem(row, kColState, new QTableWidgetItem(stateToString(alarm.state)));
  table_->setItem(row, kColSource, new QTableWidgetItem(alarm.source));
  table_->setItem(row, kColMessage, new QTableWidgetItem(alarm.message));
}

void AlarmPlugin::updateRowState(int row, AlarmState state) {
  auto *item = table_->item(row, kColState);
  if (item) {
    item->setText(stateToString(state));
  }
}

int AlarmPlugin::findRowById(int alarmId) const {
  for (int r = 0; r < table_->rowCount(); ++r) {
    auto *item = table_->item(r, kColId);
    if (item && item->data(Qt::UserRole).toInt() == alarmId) {
      return r;
    }
  }
  return -1;
}
