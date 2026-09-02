#pragma once

#include <QWidget>

class QLabel;
class QProgressBar;
class QPushButton;
class QTableWidget;
class HardwareVerificationService;
struct VerificationResult;

class NetworkVerificationWidget : public QWidget {
    Q_OBJECT
public:
    explicit NetworkVerificationWidget(HardwareVerificationService* service, QWidget* parent = nullptr);

    void displayResult(const VerificationResult& result);
    void clear();

signals:
    void verificationRequested();

private:
    void runVerification();

    HardwareVerificationService* service_;
    QPushButton* runButton_ = nullptr;
    QProgressBar* progress_ = nullptr;
    QTableWidget* resultTable_ = nullptr;
    QLabel* summaryLabel_ = nullptr;
};
