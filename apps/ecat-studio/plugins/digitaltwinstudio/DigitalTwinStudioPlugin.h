#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QVector>

class QLabel;
class QPushButton;
class QTabWidget;
class QTableWidget;

class DigitalTwinStudioPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DigitalTwinStudioPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct Model3D {
    QString name;
    QString filePath;
    double rotX;
    double rotY;
    double rotZ;
    double zoom;
  };

  struct TwinSyncStatus {
    QString deviceId;
    QString status;
    double latency;
    QString lastSync;
  };

  struct SimulationParam {
    QString name;
    double value;
    double minVal;
    double maxVal;
    QString unit;
  };

  struct PredictionData {
    QString metric;
    double currentValue;
    double predictedValue;
    double confidence;
    QString trend;
  };

  void addModel(const Model3D &model);
  void removeModel(int index);
  int modelCount() const;

  void addSyncStatus(const TwinSyncStatus &status);
  void removeSyncStatus(int index);
  int syncStatusCount() const;

  void addSimulationParam(const SimulationParam &param);
  void removeSimulationParam(int index);
  int simulationParamCount() const;

  void addPrediction(const PredictionData &prediction);
  void removePrediction(int index);
  int predictionCount() const;

  QString exportData() const;

  QTabWidget *tabs() const;
  QTableWidget *modelTable() const;
  QTableWidget *syncTable() const;
  QTableWidget *simulationTable() const;
  QTableWidget *predictionTable() const;
  QLabel *statusLabel() const;

signals:
  void modelAdded(const QString &name);
  void syncStatusChanged(const QString &deviceId, const QString &status);
  void simulationStarted();
  void predictionUpdated(const QString &metric, double value);

private:
  void buildUi();
  void rebuildModelTable();
  void rebuildSyncTable();
  void rebuildSimulationTable();
  void rebuildPredictionTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QTableWidget *modelTable_ = nullptr;
  QTableWidget *syncTable_ = nullptr;
  QTableWidget *simulationTable_ = nullptr;
  QTableWidget *predictionTable_ = nullptr;
  QPushButton *addModelBtn_ = nullptr;
  QPushButton *removeModelBtn_ = nullptr;
  QPushButton *rotateBtn_ = nullptr;
  QPushButton *zoomInBtn_ = nullptr;
  QPushButton *zoomOutBtn_ = nullptr;
  QPushButton *addSyncBtn_ = nullptr;
  QPushButton *removeSyncBtn_ = nullptr;
  QPushButton *refreshSyncBtn_ = nullptr;
  QPushButton *addSimParamBtn_ = nullptr;
  QPushButton *removeSimParamBtn_ = nullptr;
  QPushButton *startSimBtn_ = nullptr;
  QPushButton *stopSimBtn_ = nullptr;
  QPushButton *addPredictionBtn_ = nullptr;
  QPushButton *removePredictionBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<Model3D> models_;
  QVector<TwinSyncStatus> syncStatuses_;
  QVector<SimulationParam> simParams_;
  QVector<PredictionData> predictions_;
};
