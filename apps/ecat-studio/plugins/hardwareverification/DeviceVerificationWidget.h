#pragma once

#include <QWidget>

class QLabel;
class QProgressBar;
class QPushButton;
class QTableWidget;
class QSpinBox;
class HardwareVerificationService;
struct VerificationResult;

class DeviceVerificationWidget : public QWidget {
  Q_OBJECT
public:
  explicit DeviceVerificationWidget(HardwareVerificationService *service,
                                    QWidget *parent = nullptr);

  void displayResult(const VerificationResult &result);
  void clear();

signals:
  void verificationRequested(int position);

private:
  void runVerification();

  HardwareVerificationService *service_;
  QSpinBox *positionSpin_ = nullptr;
  QPushButton *runButton_ = nullptr;
  QProgressBar *progress_ = nullptr;
  QTableWidget *resultTable_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
};
