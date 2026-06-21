#include "AIAssistantPlugin.h"

#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

AIAssistantPlugin::AIAssistantPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString AIAssistantPlugin::id() const { return "aiassistant"; }
QString AIAssistantPlugin::displayName() const { return "AI Assistant"; }
QString AIAssistantPlugin::displayNameZh() const { return QStringLiteral("AI助手"); }
QIcon AIAssistantPlugin::icon() const { return QIcon::fromTheme("system-run"); }
int AIAssistantPlugin::defaultOrder() const { return 330; }
bool AIAssistantPlugin::visible() const { return true; }

void AIAssistantPlugin::activate() {}
void AIAssistantPlugin::deactivate() {}

QWidget *AIAssistantPlugin::widget() { return containerWidget_; }
QTableWidget *AIAssistantPlugin::predictionsTable() const { return predictionsTable_; }
QTreeWidget *AIAssistantPlugin::anomaliesTree() const { return anomaliesTree_; }
QTextEdit *AIAssistantPlugin::optimizationsView() const { return optimizationsView_; }
QTableWidget *AIAssistantPlugin::patternsTable() const { return patternsTable_; }

void AIAssistantPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *predLabel = new QLabel(tr("Predictions"));
  leftLayout->addWidget(predLabel);

  predictionsTable_ = new QTableWidget;
  predictionsTable_->setColumnCount(3);
  predictionsTable_->setHorizontalHeaderLabels({tr("Metric"), tr("Confidence"), tr("Predicted Value")});
  predictionsTable_->horizontalHeader()->setStretchLastSection(true);
  predictionsTable_->setSelectionBehavior(QTableWidget::SelectRows);
  predictionsTable_->setSelectionMode(QTableWidget::SingleSelection);
  leftLayout->addWidget(predictionsTable_);

  auto *anomalyLabel = new QLabel(tr("Anomalies"));
  leftLayout->addWidget(anomalyLabel);

  anomaliesTree_ = new QTreeWidget;
  anomaliesTree_->setHeaderLabels({tr("Source"), tr("Severity"), tr("Description")});
  leftLayout->addWidget(anomaliesTree_);

  analyzeBtn_ = new QPushButton(tr("Run Analysis"));
  leftLayout->addWidget(analyzeBtn_);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *optLabel = new QLabel(tr("Optimizations"));
  rightLayout->addWidget(optLabel);

  optimizationsView_ = new QTextEdit;
  optimizationsView_->setReadOnly(true);
  optimizationsView_->setPlaceholderText(tr("AI optimization suggestions..."));
  rightLayout->addWidget(optimizationsView_);

  auto *patternLabel = new QLabel(tr("Patterns"));
  rightLayout->addWidget(patternLabel);

  patternsTable_ = new QTableWidget;
  patternsTable_->setColumnCount(3);
  patternsTable_->setHorizontalHeaderLabels({tr("Pattern"), tr("Frequency"), tr("Confidence")});
  patternsTable_->horizontalHeader()->setStretchLastSection(true);
  rightLayout->addWidget(patternsTable_);

  auto *exportRow = new QHBoxLayout;
  exportBtn_ = new QPushButton(tr("Export AI Report"));
  exportRow->addWidget(exportBtn_);
  exportRow->addStretch();
  rightLayout->addLayout(exportRow);

  statusLabel_ = new QLabel(tr("Ready"));
  rightLayout->addWidget(statusLabel_);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);

  connect(analyzeBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Analysis running..."));
  });
  connect(exportBtn_, &QPushButton::clicked, this, &AIAssistantPlugin::exportRequested);
}

void AIAssistantPlugin::addPrediction(const QString &metric, double confidence, const QString &value) {
  int row = predictionsTable_->rowCount();
  predictionsTable_->insertRow(row);
  predictionsTable_->setItem(row, 0, new QTableWidgetItem(metric));
  predictionsTable_->setItem(row, 1, new QTableWidgetItem(QString::number(confidence, 'f', 2)));
  predictionsTable_->setItem(row, 2, new QTableWidgetItem(value));
  statusLabel_->setText(tr("Prediction: %1").arg(metric));
  emit predictionAdded(metric);
}

void AIAssistantPlugin::removePrediction(const QString &metric) {
  for (int i = 0; i < predictionsTable_->rowCount(); ++i) {
    if (predictionsTable_->item(i, 0)->text() == metric) {
      predictionsTable_->removeRow(i);
      return;
    }
  }
}

void AIAssistantPlugin::clearPredictions() { predictionsTable_->setRowCount(0); }
int AIAssistantPlugin::predictionCount() const { return predictionsTable_->rowCount(); }

void AIAssistantPlugin::addAnomaly(const QString &source, const QString &severity, const QString &description) {
  new QTreeWidgetItem(anomaliesTree_, {source, severity, description});
  statusLabel_->setText(tr("Anomaly: %1").arg(source));
  emit anomalyDetected(source);
}

void AIAssistantPlugin::clearAnomalies() { anomaliesTree_->clear(); }
int AIAssistantPlugin::anomalyCount() const { return anomaliesTree_->topLevelItemCount(); }

void AIAssistantPlugin::setOptimizationsText(const QString &text) {
  optimizationsView_->setPlainText(text);
  emit optimizationsUpdated();
}

QString AIAssistantPlugin::optimizationsText() const {
  return optimizationsView_->toPlainText();
}

void AIAssistantPlugin::addPattern(const QString &name, const QString &frequency, const QString &confidence) {
  int row = patternsTable_->rowCount();
  patternsTable_->insertRow(row);
  patternsTable_->setItem(row, 0, new QTableWidgetItem(name));
  patternsTable_->setItem(row, 1, new QTableWidgetItem(frequency));
  patternsTable_->setItem(row, 2, new QTableWidgetItem(confidence));
  emit patternRecognized(name);
}

void AIAssistantPlugin::clearPatterns() { patternsTable_->setRowCount(0); }
int AIAssistantPlugin::patternCount() const { return patternsTable_->rowCount(); }

bool AIAssistantPlugin::exportAIReport(const QString &filePath, const QString &format) {
  QJsonObject root;
  root["version"] = 1;
  root["format"] = format;
  root["optimizations"] = optimizationsView_->toPlainText();

  QJsonArray preds;
  for (int i = 0; i < predictionsTable_->rowCount(); ++i) {
    QJsonObject pred;
    pred["metric"] = predictionsTable_->item(i, 0)->text();
    pred["confidence"] = predictionsTable_->item(i, 1)->text();
    pred["value"] = predictionsTable_->item(i, 2)->text();
    preds.append(pred);
  }
  root["predictions"] = preds;

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  file.write(QJsonDocument(root).toJson());
  return true;
}
