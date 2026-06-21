#include "MaintenanceSchedulerPlugin.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

MaintenanceSchedulerPlugin::MaintenanceSchedulerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString MaintenanceSchedulerPlugin::id() const { return "maintenancescheduler"; }
QString MaintenanceSchedulerPlugin::displayName() const { return "Maintenance Scheduler"; }
QString MaintenanceSchedulerPlugin::displayNameZh() const { return QStringLiteral("维护调度器"); }
QIcon MaintenanceSchedulerPlugin::icon() const { return QIcon::fromTheme("preferences-system-time"); }
int MaintenanceSchedulerPlugin::defaultOrder() const { return 320; }
bool MaintenanceSchedulerPlugin::visible() const { return true; }
void MaintenanceSchedulerPlugin::activate() {}
void MaintenanceSchedulerPlugin::deactivate() {}

QWidget *MaintenanceSchedulerPlugin::widget() { return container_; }
QTableWidget *MaintenanceSchedulerPlugin::taskTable() const { return taskTable_; }
QTableWidget *MaintenanceSchedulerPlugin::scheduleTable() const { return scheduleTable_; }
QTableWidget *MaintenanceSchedulerPlugin::historyTable() const { return historyTable_; }
QTextEdit *MaintenanceSchedulerPlugin::reportPanel() const { return reportPanel_; }
QLabel *MaintenanceSchedulerPlugin::statusLabel() const { return statusLabel_; }

int MaintenanceSchedulerPlugin::taskCount() const { return tasks_.size(); }
int MaintenanceSchedulerPlugin::scheduleCount() const { return scheduleTable_ ? scheduleTable_->rowCount() : 0; }
int MaintenanceSchedulerPlugin::historyCount() const { return history_.size(); }

void MaintenanceSchedulerPlugin::buildUi() {
  container_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(container_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  leftLayout->addWidget(new QLabel(tr("Maintenance Tasks")));
  taskTable_ = new QTableWidget(0, 5);
  taskTable_->setHorizontalHeaderLabels({tr("Name"), tr("Description"), tr("Schedule"), tr("Priority"), tr("Status")});
  taskTable_->horizontalHeader()->setStretchLastSection(true);
  leftLayout->addWidget(taskTable_);

  auto *taskBtnRow = new QHBoxLayout;
  addTaskBtn_ = new QPushButton(tr("Add Task"));
  removeTaskBtn_ = new QPushButton(tr("Remove Task"));
  taskBtnRow->addWidget(addTaskBtn_);
  taskBtnRow->addWidget(removeTaskBtn_);
  leftLayout->addLayout(taskBtnRow);

  leftLayout->addWidget(new QLabel(tr("Maintenance Schedule")));
  scheduleTable_ = new QTableWidget(0, 2);
  scheduleTable_->setHorizontalHeaderLabels({tr("Task"), tr("Date/Time")});
  scheduleTable_->horizontalHeader()->setStretchLastSection(true);
  leftLayout->addWidget(scheduleTable_);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  rightLayout->addWidget(new QLabel(tr("Maintenance History")));
  historyTable_ = new QTableWidget(0, 4);
  historyTable_->setHorizontalHeaderLabels({tr("Task"), tr("Status"), tr("Time"), tr("Notes")});
  historyTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(historyTable_);

  rightLayout->addWidget(new QLabel(tr("Maintenance Reports")));
  reportPanel_ = new QTextEdit;
  reportPanel_->setReadOnly(true);
  reportPanel_->setPlaceholderText(tr("Generate a report to view summary..."));
  rightLayout->addWidget(reportPanel_);

  statusLabel_ = new QLabel(tr("Status: Ready"));
  rightLayout->addWidget(statusLabel_);

  auto *btnRow = new QHBoxLayout;
  recordBtn_ = new QPushButton(tr("Record Maintenance"));
  reportBtn_ = new QPushButton(tr("Generate Report"));
  exportBtn_ = new QPushButton(tr("Export Report"));
  btnRow->addWidget(recordBtn_);
  btnRow->addWidget(reportBtn_);
  btnRow->addWidget(exportBtn_);
  rightLayout->addLayout(btnRow);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);
  mainLayout->addWidget(splitter);

  connect(addTaskBtn_, &QPushButton::clicked, this, [this]() {
    MaintenanceTask t;
    t.id = QString("mt_%1").arg(nextId_++);
    t.name = tr("New Task");
    t.description = tr("Task description");
    t.schedule = tr("Weekly");
    t.priority = tr("Medium");
    t.status = tr("Pending");
    addTask(t);
  });
  connect(removeTaskBtn_, &QPushButton::clicked, this, [this]() {
    if (!tasks_.isEmpty()) removeTask(tasks_.size() - 1);
  });
  connect(recordBtn_, &QPushButton::clicked, this, [this]() {
    if (!tasks_.isEmpty()) {
      MaintenanceRecord rec;
      rec.id = QString("mrec_%1").arg(nextId_++);
      rec.taskName = tasks_.first().name;
      rec.status = "Completed";
      rec.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
      rec.notes = tr("Maintenance performed");
      recordMaintenance(rec);
    }
  });
  connect(reportBtn_, &QPushButton::clicked, this, &MaintenanceSchedulerPlugin::generateReport);
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getSaveFileName(container_, tr("Export Report"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) exportReport(path);
  });
}

void MaintenanceSchedulerPlugin::addTask(const MaintenanceTask &task) {
  MaintenanceTask t = task;
  if (t.id.isEmpty()) t.id = QString("mt_%1").arg(nextId_++);
  tasks_.append(t);

  int row = taskTable_->rowCount();
  taskTable_->insertRow(row);
  taskTable_->setItem(row, 0, new QTableWidgetItem(t.name));
  taskTable_->setItem(row, 1, new QTableWidgetItem(t.description));
  taskTable_->setItem(row, 2, new QTableWidgetItem(t.schedule));
  taskTable_->setItem(row, 3, new QTableWidgetItem(t.priority));
  taskTable_->setItem(row, 4, new QTableWidgetItem(t.status));

  emit taskAdded(t.id, t.name);
}

void MaintenanceSchedulerPlugin::updateTask(int index, const MaintenanceTask &task) {
  if (index < 0 || index >= tasks_.size()) return;
  tasks_[index] = task;
  taskTable_->setItem(index, 0, new QTableWidgetItem(task.name));
  taskTable_->setItem(index, 1, new QTableWidgetItem(task.description));
  taskTable_->setItem(index, 2, new QTableWidgetItem(task.schedule));
  taskTable_->setItem(index, 3, new QTableWidgetItem(task.priority));
  taskTable_->setItem(index, 4, new QTableWidgetItem(task.status));
  emit taskUpdated(task.id);
}

void MaintenanceSchedulerPlugin::removeTask(int index) {
  if (index < 0 || index >= tasks_.size()) return;
  QString id = tasks_[index].id;
  tasks_.removeAt(index);
  taskTable_->removeRow(index);
  emit taskRemoved(id);
}

void MaintenanceSchedulerPlugin::addScheduleEntry(const QString &taskName, const QString &dateTime) {
  int row = scheduleTable_->rowCount();
  scheduleTable_->insertRow(row);
  scheduleTable_->setItem(row, 0, new QTableWidgetItem(taskName));
  scheduleTable_->setItem(row, 1, new QTableWidgetItem(dateTime));
}

void MaintenanceSchedulerPlugin::removeScheduleEntry(int index) {
  if (index < 0 || index >= scheduleTable_->rowCount()) return;
  scheduleTable_->removeRow(index);
}

void MaintenanceSchedulerPlugin::recordMaintenance(const MaintenanceRecord &record) {
  MaintenanceRecord r = record;
  if (r.id.isEmpty()) r.id = QString("mrec_%1").arg(nextId_++);
  history_.append(r);

  int row = historyTable_->rowCount();
  historyTable_->insertRow(row);
  historyTable_->setItem(row, 0, new QTableWidgetItem(r.taskName));
  historyTable_->setItem(row, 1, new QTableWidgetItem(r.status));
  historyTable_->setItem(row, 2, new QTableWidgetItem(r.timestamp));
  historyTable_->setItem(row, 3, new QTableWidgetItem(r.notes));

  emit maintenanceRecorded(r.id);
}

void MaintenanceSchedulerPlugin::clearHistory() {
  history_.clear();
  historyTable_->setRowCount(0);
  refreshReport();
}

void MaintenanceSchedulerPlugin::generateReport() {
  refreshReport();
}

void MaintenanceSchedulerPlugin::refreshReport() {
  QString report;
  report += tr("=== Maintenance Report ===\n\n");
  report += tr("Total tasks: %1\n").arg(tasks_.size());
  report += tr("Scheduled entries: %1\n").arg(scheduleTable_ ? scheduleTable_->rowCount() : 0);
  report += tr("Completed maintenances: %1\n\n").arg(history_.size());

  report += tr("-- Tasks --\n");
  for (const auto &t : tasks_) {
    report += tr("  [%1] %2 - %3 (%4)\n").arg(t.priority, t.name, t.status, t.schedule);
  }

  report += tr("\n-- Recent History --\n");
  for (const auto &r : history_) {
    report += tr("  %1: %2 - %3\n").arg(r.timestamp, r.taskName, r.status);
  }

  reportPanel_->setPlainText(report);
  statusLabel_->setText(tr("Report generated"));
}

bool MaintenanceSchedulerPlugin::exportReport(const QString &filePath) {
  QJsonObject root;
  root["version"] = 1;
  root["totalTasks"] = tasks_.size();
  root["totalRecords"] = history_.size();

  QJsonArray taskArr;
  for (const auto &t : tasks_) {
    QJsonObject obj;
    obj["id"] = t.id;
    obj["name"] = t.name;
    obj["description"] = t.description;
    obj["schedule"] = t.schedule;
    obj["priority"] = t.priority;
    obj["status"] = t.status;
    taskArr.append(obj);
  }
  root["tasks"] = taskArr;

  QJsonArray histArr;
  for (const auto &r : history_) {
    QJsonObject obj;
    obj["id"] = r.id;
    obj["taskName"] = r.taskName;
    obj["status"] = r.status;
    obj["timestamp"] = r.timestamp;
    obj["notes"] = r.notes;
    histArr.append(obj);
  }
  root["history"] = histArr;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  file.write(QJsonDocument(root).toJson());
  return true;
}
