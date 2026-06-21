#include "DigitalTwinStudioPlugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

DigitalTwinStudioPlugin::DigitalTwinStudioPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString DigitalTwinStudioPlugin::id() const { return "digitaltwinstudio"; }
QString DigitalTwinStudioPlugin::displayName() const { return "Digital Twin Studio"; }
QString DigitalTwinStudioPlugin::displayNameZh() const { return "数字孪生工作室"; }
int DigitalTwinStudioPlugin::defaultOrder() const { return 370; }
bool DigitalTwinStudioPlugin::visible() const { return true; }

void DigitalTwinStudioPlugin::activate() {}
void DigitalTwinStudioPlugin::deactivate() {}

QWidget *DigitalTwinStudioPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void DigitalTwinStudioPlugin::addModel(const Model3D &model) {
  models_.append(model);
  rebuildModelTable();
  emit modelAdded(model.name);
}

void DigitalTwinStudioPlugin::removeModel(int index) {
  if (index >= 0 && index < models_.size()) {
    models_.removeAt(index);
    rebuildModelTable();
  }
}

int DigitalTwinStudioPlugin::modelCount() const { return models_.size(); }

void DigitalTwinStudioPlugin::addSyncStatus(const TwinSyncStatus &status) {
  syncStatuses_.append(status);
  rebuildSyncTable();
  emit syncStatusChanged(status.deviceId, status.status);
}

void DigitalTwinStudioPlugin::removeSyncStatus(int index) {
  if (index >= 0 && index < syncStatuses_.size()) {
    syncStatuses_.removeAt(index);
    rebuildSyncTable();
  }
}

int DigitalTwinStudioPlugin::syncStatusCount() const { return syncStatuses_.size(); }

void DigitalTwinStudioPlugin::addSimulationParam(const SimulationParam &param) {
  simParams_.append(param);
  rebuildSimulationTable();
}

void DigitalTwinStudioPlugin::removeSimulationParam(int index) {
  if (index >= 0 && index < simParams_.size()) {
    simParams_.removeAt(index);
    rebuildSimulationTable();
  }
}

int DigitalTwinStudioPlugin::simulationParamCount() const { return simParams_.size(); }

void DigitalTwinStudioPlugin::addPrediction(const PredictionData &prediction) {
  predictions_.append(prediction);
  rebuildPredictionTable();
  emit predictionUpdated(prediction.metric, prediction.predictedValue);
}

void DigitalTwinStudioPlugin::removePrediction(int index) {
  if (index >= 0 && index < predictions_.size()) {
    predictions_.removeAt(index);
    rebuildPredictionTable();
  }
}

int DigitalTwinStudioPlugin::predictionCount() const { return predictions_.size(); }

QString DigitalTwinStudioPlugin::exportData() const {
  QJsonObject root;

  QJsonArray modelsArr;
  for (const auto &m : models_) {
    QJsonObject obj;
    obj["name"] = m.name;
    obj["filePath"] = m.filePath;
    obj["rotX"] = m.rotX;
    obj["rotY"] = m.rotY;
    obj["rotZ"] = m.rotZ;
    obj["zoom"] = m.zoom;
    modelsArr.append(obj);
  }
  root["models"] = modelsArr;

  QJsonArray syncArr;
  for (const auto &s : syncStatuses_) {
    QJsonObject obj;
    obj["deviceId"] = s.deviceId;
    obj["status"] = s.status;
    obj["latency"] = s.latency;
    obj["lastSync"] = s.lastSync;
    syncArr.append(obj);
  }
  root["syncStatuses"] = syncArr;

  QJsonArray simArr;
  for (const auto &p : simParams_) {
    QJsonObject obj;
    obj["name"] = p.name;
    obj["value"] = p.value;
    obj["min"] = p.minVal;
    obj["max"] = p.maxVal;
    obj["unit"] = p.unit;
    simArr.append(obj);
  }
  root["simulationParams"] = simArr;

  QJsonArray predArr;
  for (const auto &p : predictions_) {
    QJsonObject obj;
    obj["metric"] = p.metric;
    obj["currentValue"] = p.currentValue;
    obj["predictedValue"] = p.predictedValue;
    obj["confidence"] = p.confidence;
    obj["trend"] = p.trend;
    predArr.append(obj);
  }
  root["predictions"] = predArr;

  return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QTabWidget *DigitalTwinStudioPlugin::tabs() const { return tabs_; }
QTableWidget *DigitalTwinStudioPlugin::modelTable() const { return modelTable_; }
QTableWidget *DigitalTwinStudioPlugin::syncTable() const { return syncTable_; }
QTableWidget *DigitalTwinStudioPlugin::simulationTable() const { return simulationTable_; }
QTableWidget *DigitalTwinStudioPlugin::predictionTable() const { return predictionTable_; }
QLabel *DigitalTwinStudioPlugin::statusLabel() const { return statusLabel_; }

void DigitalTwinStudioPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  tabs_ = new QTabWidget;

  auto *modelTab = new QWidget;
  auto *modelLayout = new QVBoxLayout(modelTab);
  modelTable_ = new QTableWidget;
  modelTable_->setColumnCount(6);
  modelTable_->setHorizontalHeaderLabels({"Name", "File", "RotX", "RotY", "RotZ", "Zoom"});
  modelTable_->horizontalHeader()->setStretchLastSection(true);
  modelLayout->addWidget(modelTable_);
  auto *modelBtnRow = new QWidget;
  auto *modelBtnLayout = new QHBoxLayout(modelBtnRow);
  addModelBtn_ = new QPushButton("Add");
  removeModelBtn_ = new QPushButton("Remove");
  rotateBtn_ = new QPushButton("Rotate");
  zoomInBtn_ = new QPushButton("Zoom In");
  zoomOutBtn_ = new QPushButton("Zoom Out");
  modelBtnLayout->addWidget(addModelBtn_);
  modelBtnLayout->addWidget(removeModelBtn_);
  modelBtnLayout->addWidget(rotateBtn_);
  modelBtnLayout->addWidget(zoomInBtn_);
  modelBtnLayout->addWidget(zoomOutBtn_);
  modelLayout->addWidget(modelBtnRow);
  tabs_->addTab(modelTab, "3D Model Viewer");

  auto *syncTab = new QWidget;
  auto *syncLayout = new QVBoxLayout(syncTab);
  syncTable_ = new QTableWidget;
  syncTable_->setColumnCount(4);
  syncTable_->setHorizontalHeaderLabels({"Device", "Status", "Latency", "Last Sync"});
  syncTable_->horizontalHeader()->setStretchLastSection(true);
  syncLayout->addWidget(syncTable_);
  auto *syncBtnRow = new QWidget;
  auto *syncBtnLayout = new QHBoxLayout(syncBtnRow);
  addSyncBtn_ = new QPushButton("Add");
  removeSyncBtn_ = new QPushButton("Remove");
  refreshSyncBtn_ = new QPushButton("Refresh");
  syncBtnLayout->addWidget(addSyncBtn_);
  syncBtnLayout->addWidget(removeSyncBtn_);
  syncBtnLayout->addWidget(refreshSyncBtn_);
  syncLayout->addWidget(syncBtnRow);
  tabs_->addTab(syncTab, "Synchronization");

  auto *simTab = new QWidget;
  auto *simLayout = new QVBoxLayout(simTab);
  simulationTable_ = new QTableWidget;
  simulationTable_->setColumnCount(5);
  simulationTable_->setHorizontalHeaderLabels({"Parameter", "Value", "Min", "Max", "Unit"});
  simulationTable_->horizontalHeader()->setStretchLastSection(true);
  simLayout->addWidget(simulationTable_);
  auto *simBtnRow = new QWidget;
  auto *simBtnLayout = new QHBoxLayout(simBtnRow);
  addSimParamBtn_ = new QPushButton("Add");
  removeSimParamBtn_ = new QPushButton("Remove");
  startSimBtn_ = new QPushButton("Start");
  stopSimBtn_ = new QPushButton("Stop");
  simBtnLayout->addWidget(addSimParamBtn_);
  simBtnLayout->addWidget(removeSimParamBtn_);
  simBtnLayout->addWidget(startSimBtn_);
  simBtnLayout->addWidget(stopSimBtn_);
  simLayout->addWidget(simBtnRow);
  tabs_->addTab(simTab, "Simulation");

  auto *predTab = new QWidget;
  auto *predLayout = new QVBoxLayout(predTab);
  predictionTable_ = new QTableWidget;
  predictionTable_->setColumnCount(5);
  predictionTable_->setHorizontalHeaderLabels({"Metric", "Current", "Predicted", "Confidence", "Trend"});
  predictionTable_->horizontalHeader()->setStretchLastSection(true);
  predLayout->addWidget(predictionTable_);
  auto *predBtnRow = new QWidget;
  auto *predBtnLayout = new QHBoxLayout(predBtnRow);
  addPredictionBtn_ = new QPushButton("Add");
  removePredictionBtn_ = new QPushButton("Remove");
  predBtnLayout->addWidget(addPredictionBtn_);
  predBtnLayout->addWidget(removePredictionBtn_);
  predLayout->addWidget(predBtnRow);
  tabs_->addTab(predTab, "Predictive Analytics");

  mainLayout->addWidget(tabs_);

  exportBtn_ = new QPushButton("Export Digital Twin Data");
  mainLayout->addWidget(exportBtn_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  connect(addModelBtn_, &QPushButton::clicked, this, [this]() {
    Model3D m;
    m.name = "model_" + QString::number(models_.size());
    m.filePath = "/models/default.obj";
    m.rotX = 0.0;
    m.rotY = 0.0;
    m.rotZ = 0.0;
    m.zoom = 1.0;
    addModel(m);
  });
  connect(removeModelBtn_, &QPushButton::clicked, this, [this]() {
    int row = modelTable_->currentRow();
    if (row >= 0) removeModel(row);
  });
  connect(rotateBtn_, &QPushButton::clicked, this, [this]() {
    int row = modelTable_->currentRow();
    if (row >= 0 && row < models_.size()) {
      models_[row].rotY += 45.0;
      if (models_[row].rotY >= 360.0) models_[row].rotY -= 360.0;
      rebuildModelTable();
    }
  });
  connect(zoomInBtn_, &QPushButton::clicked, this, [this]() {
    int row = modelTable_->currentRow();
    if (row >= 0 && row < models_.size()) {
      models_[row].zoom *= 1.2;
      rebuildModelTable();
    }
  });
  connect(zoomOutBtn_, &QPushButton::clicked, this, [this]() {
    int row = modelTable_->currentRow();
    if (row >= 0 && row < models_.size()) {
      models_[row].zoom /= 1.2;
      rebuildModelTable();
    }
  });
  connect(addSyncBtn_, &QPushButton::clicked, this, [this]() {
    TwinSyncStatus s;
    s.deviceId = "device_" + QString::number(syncStatuses_.size());
    s.status = "synced";
    s.latency = 1.5;
    s.lastSync = QDateTime::currentDateTime().toString(Qt::ISODate);
    addSyncStatus(s);
  });
  connect(removeSyncBtn_, &QPushButton::clicked, this, [this]() {
    int row = syncTable_->currentRow();
    if (row >= 0) removeSyncStatus(row);
  });
  connect(refreshSyncBtn_, &QPushButton::clicked, this, [this]() {
    for (auto &s : syncStatuses_) {
      s.lastSync = QDateTime::currentDateTime().toString(Qt::ISODate);
    }
    rebuildSyncTable();
  });
  connect(addSimParamBtn_, &QPushButton::clicked, this, [this]() {
    SimulationParam p;
    p.name = "param_" + QString::number(simParams_.size());
    p.value = 0.0;
    p.minVal = 0.0;
    p.maxVal = 100.0;
    p.unit = "units";
    addSimulationParam(p);
  });
  connect(removeSimParamBtn_, &QPushButton::clicked, this, [this]() {
    int row = simulationTable_->currentRow();
    if (row >= 0) removeSimulationParam(row);
  });
  connect(startSimBtn_, &QPushButton::clicked, this, [this]() {
    emit simulationStarted();
  });
  connect(stopSimBtn_, &QPushButton::clicked, this, [this]() {});
  connect(addPredictionBtn_, &QPushButton::clicked, this, [this]() {
    PredictionData p;
    p.metric = "metric_" + QString::number(predictions_.size());
    p.currentValue = 50.0;
    p.predictedValue = 55.0;
    p.confidence = 0.85;
    p.trend = "increasing";
    addPrediction(p);
  });
  connect(removePredictionBtn_, &QPushButton::clicked, this, [this]() {
    int row = predictionTable_->currentRow();
    if (row >= 0) removePrediction(row);
  });
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    exportData();
  });
}

void DigitalTwinStudioPlugin::rebuildModelTable() {
  if (!modelTable_) return;
  modelTable_->setRowCount(models_.size());
  for (int i = 0; i < models_.size(); ++i) {
    const auto &m = models_[i];
    modelTable_->setItem(i, 0, new QTableWidgetItem(m.name));
    modelTable_->setItem(i, 1, new QTableWidgetItem(m.filePath));
    modelTable_->setItem(i, 2, new QTableWidgetItem(QString::number(m.rotX)));
    modelTable_->setItem(i, 3, new QTableWidgetItem(QString::number(m.rotY)));
    modelTable_->setItem(i, 4, new QTableWidgetItem(QString::number(m.rotZ)));
    modelTable_->setItem(i, 5, new QTableWidgetItem(QString::number(m.zoom)));
  }
}

void DigitalTwinStudioPlugin::rebuildSyncTable() {
  if (!syncTable_) return;
  syncTable_->setRowCount(syncStatuses_.size());
  for (int i = 0; i < syncStatuses_.size(); ++i) {
    const auto &s = syncStatuses_[i];
    syncTable_->setItem(i, 0, new QTableWidgetItem(s.deviceId));
    syncTable_->setItem(i, 1, new QTableWidgetItem(s.status));
    syncTable_->setItem(i, 2, new QTableWidgetItem(QString::number(s.latency)));
    syncTable_->setItem(i, 3, new QTableWidgetItem(s.lastSync));
  }
}

void DigitalTwinStudioPlugin::rebuildSimulationTable() {
  if (!simulationTable_) return;
  simulationTable_->setRowCount(simParams_.size());
  for (int i = 0; i < simParams_.size(); ++i) {
    const auto &p = simParams_[i];
    simulationTable_->setItem(i, 0, new QTableWidgetItem(p.name));
    simulationTable_->setItem(i, 1, new QTableWidgetItem(QString::number(p.value)));
    simulationTable_->setItem(i, 2, new QTableWidgetItem(QString::number(p.minVal)));
    simulationTable_->setItem(i, 3, new QTableWidgetItem(QString::number(p.maxVal)));
    simulationTable_->setItem(i, 4, new QTableWidgetItem(p.unit));
  }
}

void DigitalTwinStudioPlugin::rebuildPredictionTable() {
  if (!predictionTable_) return;
  predictionTable_->setRowCount(predictions_.size());
  for (int i = 0; i < predictions_.size(); ++i) {
    const auto &p = predictions_[i];
    predictionTable_->setItem(i, 0, new QTableWidgetItem(p.metric));
    predictionTable_->setItem(i, 1, new QTableWidgetItem(QString::number(p.currentValue)));
    predictionTable_->setItem(i, 2, new QTableWidgetItem(QString::number(p.predictedValue)));
    predictionTable_->setItem(i, 3, new QTableWidgetItem(QString::number(p.confidence)));
    predictionTable_->setItem(i, 4, new QTableWidgetItem(p.trend));
  }
}
