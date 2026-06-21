#pragma once

// CalibrationPlugin — workspace plugin for EtherCAT device calibration with
// step-by-step wizard, data collection, results analysis, and history tracking.

#include "plugins/WorkspacePlugin.h"

#include <QSpinBox>
#include <QVector>

class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;
class QStackedWidget;
class QTableWidget;
class QTextEdit;

class CalibrationPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit CalibrationPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  enum class WizardStep { SelectDevice, Configure, Collecting, Analyzing, Results, Complete };
  enum class CalibrationType { Offset, Gain, Linearity, Full };

  WizardStep currentStep() const;
  CalibrationType calibrationType() const;
  void setCalibrationType(CalibrationType type);

  bool isCalibrating() const;
  int collectedSamples() const;
  int requiredSamples() const;
  void setRequiredSamples(int count);

  double offsetResult() const;
  double gainResult() const;
  double linearityError() const;

  void addHistoryEntry(const QString &device, CalibrationType type,
                       double offset, double gain, double linearity);
  int historyCount() const;

  QTableWidget *dataTable() const;
  QTableWidget *historyTable() const;
  QTextEdit *resultsView() const;

signals:
  void stepChanged(CalibrationPlugin::WizardStep step);
  void calibrationComplete();
  void sampleCollected(int sampleIndex, double value);

public slots:
  void nextStep();
  void prevStep();
  void startCalibration();
  void stopCalibration();
  void collectSample();
  void analyzeResults();
  void resetWizard();
  void exportCalibrationData(const QString &path);
  void exportHistory(const QString &path);

private:
  void buildUi();
  QWidget *buildSelectPage();
  QWidget *buildConfigurePage();
  QWidget *buildCollectingPage();
  QWidget *buildAnalyzePage();
  QWidget *buildResultsPage();
  void updateStepDisplay();
  void updateButtons();

  QWidget *containerWidget_ = nullptr;
  QStackedWidget *stack_ = nullptr;
  WizardStep currentStep_ = WizardStep::SelectDevice;
  CalibrationType calType_ = CalibrationType::Full;

  QPushButton *nextBtn_ = nullptr;
  QPushButton *prevBtn_ = nullptr;
  QPushButton *startBtn_ = nullptr;
  QPushButton *stopBtn_ = nullptr;
  QPushButton *collectBtn_ = nullptr;
  QPushButton *analyzeBtn_ = nullptr;
  QPushButton *resetBtn_ = nullptr;
  QPushButton *exportDataBtn_ = nullptr;
  QPushButton *exportHistoryBtn_ = nullptr;
  QLabel *stepLabel_ = nullptr;
  QLabel *progressLabel_ = nullptr;

  QComboBox *deviceCombo_ = nullptr;
  QComboBox *typeCombo_ = nullptr;
  QSpinBox *sampleCountSpin_ = nullptr;
  QTableWidget *dataTable_ = nullptr;
  QTableWidget *historyTable_ = nullptr;
  QTableWidget *settingsTable_ = nullptr;
  QTextEdit *resultsView_ = nullptr;

  int requiredSamples_ = 50;
  int collectedSamples_ = 0;
  QVector<double> sampleValues_;
  double offsetResult_ = 0.0;
  double gainResult_ = 1.0;
  double linearityError_ = 0.0;
  bool calibrating_ = false;
};
