#pragma once

// ErrorAnalyzerWidget — Error analysis display widget.
//
// Shows error classification, error history table, and error trend chart.

#include "services/EtherCATMonitorService.h"

#include <QWidget>

class QTableWidget;
class QLabel;

class ErrorAnalyzerWidget : public QWidget {
  Q_OBJECT
public:
  explicit ErrorAnalyzerWidget(QWidget *parent = nullptr);

  void updateErrors(const ErrorRate &rate);

private:
  void buildUi();

  QTableWidget *historyTable_ = nullptr;
  QLabel *totalLabel_ = nullptr;
  QLabel *crcLabel_ = nullptr;
  QLabel *lostLabel_ = nullptr;
  QLabel *rateLabel_ = nullptr;
};
