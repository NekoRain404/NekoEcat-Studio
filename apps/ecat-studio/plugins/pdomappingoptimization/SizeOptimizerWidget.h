#pragma once

#include <QWidget>
#include "services/PdoMappingOptimizationService.h"

class QLabel;
class QPushButton;
class QGroupBox;

class SizeOptimizerWidget : public QWidget {
  Q_OBJECT
public:
  explicit SizeOptimizerWidget(QWidget *parent = nullptr);

  void updateCurrentSize(int totalBytes, int inputBytes, int outputBytes, int wastedBytes);
  void showOptimizationResult(const PdoMappingOptimizationResult &result);
  void setOptimized();

  QLabel *totalBytesLabel() const { return totalBytesValue_; }
  QLabel *inputBytesLabel() const { return inputValue_; }
  QLabel *outputBytesLabel() const { return outputValue_; }
  QPushButton *optimizeButton() const { return optimizeBtn_; }

signals:
  void optimizeRequested();

private:
  void buildUi();

  QLabel *totalBytesValue_ = nullptr;
  QLabel *inputValue_ = nullptr;
  QLabel *outputValue_ = nullptr;
  QLabel *wastedValue_ = nullptr;
  QLabel *beforeLabel_ = nullptr;
  QLabel *afterLabel_ = nullptr;
  QLabel *improvementLabel_ = nullptr;
  QPushButton *optimizeBtn_ = nullptr;
};
