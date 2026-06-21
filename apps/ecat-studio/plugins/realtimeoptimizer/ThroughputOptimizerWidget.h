#pragma once

#include "services/EtherCATOptimizerService.h"
#include <QWidget>

class QLabel;
class QPushButton;
class QTextEdit;

class ThroughputOptimizerWidget : public QWidget {
  Q_OBJECT
public:
  explicit ThroughputOptimizerWidget(QWidget *parent = nullptr);

  void updateResult(const OptimizationResult &result);

signals:
  void optimizeRequested();

private:
  void buildUi();

  QLabel *currentThroughputLabel_ = nullptr;
  QLabel *optimizedThroughputLabel_ = nullptr;
  QLabel *improvementLabel_ = nullptr;
  QTextEdit *bottleneckEdit_ = nullptr;
  QTextEdit *recommendationsEdit_ = nullptr;
  QPushButton *optimizeBtn_ = nullptr;
  QPushButton *applyBtn_ = nullptr;

  OptimizationResult lastResult_;
};
