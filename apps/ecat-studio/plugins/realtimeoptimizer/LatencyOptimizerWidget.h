#pragma once

#include "services/EtherCATOptimizerService.h"
#include <QWidget>

class QLabel;
class QPushButton;
class QTextEdit;

class LatencyOptimizerWidget : public QWidget {
  Q_OBJECT
public:
  explicit LatencyOptimizerWidget(QWidget *parent = nullptr);

  void updateResult(const OptimizationResult &result);

signals:
  void optimizeRequested();

private:
  void buildUi();

  QLabel *currentLatencyLabel_ = nullptr;
  QLabel *optimizedLatencyLabel_ = nullptr;
  QLabel *improvementLabel_ = nullptr;
  QTextEdit *recommendationsEdit_ = nullptr;
  QPushButton *optimizeBtn_ = nullptr;
  QPushButton *applyBtn_ = nullptr;

  OptimizationResult lastResult_;
};
