#include "WorkflowOptimizerPlugin.h"
#include "services/WorkflowAnalyticsService.h"

#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

WorkflowOptimizerPlugin::WorkflowOptimizerPlugin(
    WorkflowAnalyticsService *analytics, QObject *parent)
    : analytics_(analytics)
{
  if (parent) setParent(parent);
  buildUi();
}

QString WorkflowOptimizerPlugin::id() const { return "workflowoptimizer"; }
QString WorkflowOptimizerPlugin::displayName() const { return "Workflow Optimizer"; }
QString WorkflowOptimizerPlugin::displayNameZh() const { return QStringLiteral("工作流优化器"); }
QIcon WorkflowOptimizerPlugin::icon() const { return QIcon::fromTheme("system-run"); }
int WorkflowOptimizerPlugin::defaultOrder() const { return 385; }
bool WorkflowOptimizerPlugin::visible() const { return true; }

void WorkflowOptimizerPlugin::activate() {}
void WorkflowOptimizerPlugin::deactivate() {}

QWidget *WorkflowOptimizerPlugin::widget() { return containerWidget_; }

void WorkflowOptimizerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *wfLabel = new QLabel(tr("Workflows"));
  leftLayout->addWidget(wfLabel);

  workflowList_ = new QListWidget;
  leftLayout->addWidget(workflowList_);

  workflowSelector_ = new QComboBox;
  workflowSelector_->setPlaceholderText(tr("Select workflow..."));
  leftLayout->addWidget(workflowSelector_);

  optimizeButton_ = new QPushButton(tr("Optimize"));
  leftLayout->addWidget(optimizeButton_);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *suggestionsLabel = new QLabel(tr("Optimization Suggestions"));
  rightLayout->addWidget(suggestionsLabel);

  suggestionsTable_ = new QTableWidget(0, 3);
  suggestionsTable_->setHorizontalHeaderLabels({tr("Workflow"), tr("Priority"), tr("Suggestion")});
  suggestionsTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(suggestionsTable_);

  auto *metricsLabel = new QLabel(tr("Performance Metrics"));
  rightLayout->addWidget(metricsLabel);

  metricsTable_ = new QTableWidget(0, 4);
  metricsTable_->setHorizontalHeaderLabels({tr("Workflow"), tr("Success Rate"), tr("Avg Duration"), tr("Throughput")});
  metricsTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(metricsTable_);

  auto *historyLabel = new QLabel(tr("Execution History"));
  rightLayout->addWidget(historyLabel);

  historyTable_ = new QTableWidget(0, 4);
  historyTable_->setHorizontalHeaderLabels({tr("Workflow"), tr("Status"), tr("Duration (ms)"), tr("Timestamp")});
  historyTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(historyTable_);

  reportPreview_ = new QTextEdit;
  reportPreview_->setPlaceholderText(tr("Export report preview..."));
  reportPreview_->setMaximumHeight(120);
  rightLayout->addWidget(reportPreview_);

  auto *buttonRow = new QHBoxLayout;
  exportButton_ = new QPushButton(tr("Export Report"));
  buttonRow->addWidget(exportButton_);
  statusLabel_ = new QLabel(tr("Ready"));
  buttonRow->addWidget(statusLabel_);
  rightLayout->addLayout(buttonRow);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 3);

  mainLayout->addWidget(splitter);

  connect(optimizeButton_, &QPushButton::clicked, this, [this]() {
    QString wfId = workflowSelector_->currentText();
    if (!wfId.isEmpty()) emit optimizationRequested(wfId);
  });

  connect(exportButton_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Report"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) exportReport(path);
  });

  connect(workflowList_, &QListWidget::currentRowChanged, this, [this](int row) {
    if (row >= 0 && row < workflows_.size())
      emit workflowSelected(workflows_[row].id);
  });
}

void WorkflowOptimizerPlugin::addWorkflow(const QString &workflowId, const QString &name) {
  WorkflowEntry entry;
  entry.id = workflowId;
  entry.name = name;
  entry.status = "Idle";
  workflows_.append(entry);
  refreshWorkflowList();
}

void WorkflowOptimizerPlugin::removeWorkflow(const QString &workflowId) {
  for (int i = 0; i < workflows_.size(); ++i) {
    if (workflows_[i].id == workflowId) {
      workflows_.removeAt(i);
      break;
    }
  }
  refreshWorkflowList();
}

int WorkflowOptimizerPlugin::workflowCount() const { return workflows_.size(); }

void WorkflowOptimizerPlugin::addSuggestion(const QString &workflowId,
                                             const QString &priority,
                                             const QString &suggestion) {
  SuggestionEntry entry;
  entry.workflowId = workflowId;
  entry.priority = priority;
  entry.text = suggestion;
  suggestions_.append(entry);
  refreshSuggestions();
}

int WorkflowOptimizerPlugin::suggestionCount() const { return suggestions_.size(); }

void WorkflowOptimizerPlugin::addExecutionRecord(const QString &workflowId,
                                                   const QString &status,
                                                   double durationMs) {
  HistoryEntry entry;
  entry.workflowId = workflowId;
  entry.status = status;
  entry.durationMs = durationMs;
  entry.timestamp = QDateTime::currentDateTime();
  history_.append(entry);

  int row = historyTable_->rowCount();
  historyTable_->insertRow(row);
  historyTable_->setItem(row, 0, new QTableWidgetItem(workflowId));
  historyTable_->setItem(row, 1, new QTableWidgetItem(status));
  historyTable_->setItem(row, 2, new QTableWidgetItem(QString::number(durationMs, 'f', 1)));
  historyTable_->setItem(row, 3, new QTableWidgetItem(entry.timestamp.toString(Qt::ISODate)));
}

int WorkflowOptimizerPlugin::executionHistoryCount() const { return history_.size(); }

void WorkflowOptimizerPlugin::refreshWorkflowList() {
  workflowList_->clear();
  workflowSelector_->clear();
  for (const auto &entry : workflows_) {
    workflowList_->addItem(QStringLiteral("%1 (%2)").arg(entry.name, entry.status));
    workflowSelector_->addItem(entry.id);
  }
}

void WorkflowOptimizerPlugin::refreshSuggestions() {
  suggestionsTable_->setRowCount(0);
  for (const auto &s : suggestions_) {
    int row = suggestionsTable_->rowCount();
    suggestionsTable_->insertRow(row);
    suggestionsTable_->setItem(row, 0, new QTableWidgetItem(s.workflowId));
    suggestionsTable_->setItem(row, 1, new QTableWidgetItem(s.priority));
    suggestionsTable_->setItem(row, 2, new QTableWidgetItem(s.text));
  }
}

void WorkflowOptimizerPlugin::refreshMetrics() {
  metricsTable_->setRowCount(0);
  for (const auto &wf : workflows_) {
    auto analysis = analytics_->analyzeExecution(wf.id);
    int row = metricsTable_->rowCount();
    metricsTable_->insertRow(row);
    metricsTable_->setItem(row, 0, new QTableWidgetItem(wf.id));
    metricsTable_->setItem(row, 1, new QTableWidgetItem(
        QString::number(analysis.successRate, 'f', 1) + "%"));
    metricsTable_->setItem(row, 2, new QTableWidgetItem(
        QString::number(analysis.averageDurationMs, 'f', 1) + " ms"));
    metricsTable_->setItem(row, 3, new QTableWidgetItem(
        QString::number(analysis.totalExecutions)));
  }
}

bool WorkflowOptimizerPlugin::exportReport(const QString &filePath) {
  if (filePath.isEmpty()) return false;

  QJsonObject root;
  root[QStringLiteral("version")] = 1;
  root[QStringLiteral("exportTime")] = QDateTime::currentDateTime().toString(Qt::ISODate);

  QJsonArray workflowsArray;
  for (const auto &wf : workflows_) {
    QJsonObject wfObj;
    wfObj[QStringLiteral("id")] = wf.id;
    wfObj[QStringLiteral("name")] = wf.name;
    wfObj[QStringLiteral("status")] = wf.status;
    workflowsArray.append(wfObj);
  }
  root[QStringLiteral("workflows")] = workflowsArray;

  QJsonArray suggestionsArray;
  for (const auto &s : suggestions_) {
    QJsonObject sObj;
    sObj[QStringLiteral("workflowId")] = s.workflowId;
    sObj[QStringLiteral("priority")] = s.priority;
    sObj[QStringLiteral("suggestion")] = s.text;
    suggestionsArray.append(sObj);
  }
  root[QStringLiteral("suggestions")] = suggestionsArray;

  QJsonArray historyArray;
  for (const auto &h : history_) {
    QJsonObject hObj;
    hObj[QStringLiteral("workflowId")] = h.workflowId;
    hObj[QStringLiteral("status")] = h.status;
    hObj[QStringLiteral("durationMs")] = h.durationMs;
    hObj[QStringLiteral("timestamp")] = h.timestamp.toString(Qt::ISODate);
    historyArray.append(hObj);
  }
  root[QStringLiteral("executionHistory")] = historyArray;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  const QByteArray bytes = QJsonDocument(root).toJson();
  if (file.write(bytes) != bytes.size() || !file.flush()) return false;

  reportPreview_->setText(QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact)));
  statusLabel_->setText(tr("Report exported"));
  emit reportExported(filePath);
  return true;
}
