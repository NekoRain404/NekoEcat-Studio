#include "DeviceVerificationWidget.h"
#include "services/HardwareVerificationService.h"

#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

DeviceVerificationWidget::DeviceVerificationWidget(
    HardwareVerificationService *service, QWidget *parent)
    : QWidget(parent), service_(service) {
  auto *layout = new QVBoxLayout(this);

  auto *controlRow = new QHBoxLayout;
  auto *posLabel = new QLabel(tr("Slave Position:"));
  controlRow->addWidget(posLabel);

  positionSpin_ = new QSpinBox;
  positionSpin_->setRange(0, 255);
  positionSpin_->setValue(0);
  controlRow->addWidget(positionSpin_);

  runButton_ = new QPushButton(tr("Run Device Verification"));
  controlRow->addWidget(runButton_);
  controlRow->addStretch();

  layout->addLayout(controlRow);

  progress_ = new QProgressBar;
  progress_->setRange(0, 4);
  progress_->setValue(0);
  progress_->setVisible(false);
  layout->addWidget(progress_);

  resultTable_ = new QTableWidget;
  resultTable_->setColumnCount(5);
  resultTable_->setHorizontalHeaderLabels(
      {tr("Test ID"), tr("Test Name"), tr("Status"), tr("Duration"),
       tr("Details")});
  resultTable_->horizontalHeader()->setStretchLastSection(true);
  resultTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  resultTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  layout->addWidget(resultTable_);

  summaryLabel_ = new QLabel;
  layout->addWidget(summaryLabel_);

  connect(runButton_, &QPushButton::clicked, this,
          &DeviceVerificationWidget::runVerification);
}

void DeviceVerificationWidget::runVerification() {
  int position = positionSpin_->value();
  progress_->setVisible(true);
  progress_->setValue(0);
  resultTable_->setRowCount(0);
  emit verificationRequested(position);

  auto result = service_->verifyDevice(position);
  displayResult(result);
  progress_->setVisible(false);
}

void DeviceVerificationWidget::displayResult(
    const VerificationResult &result) {
  resultTable_->setRowCount(result.tests.size());
  for (int i = 0; i < result.tests.size(); ++i) {
    const auto &t = result.tests[i];
    resultTable_->setItem(i, 0, new QTableWidgetItem(t.testId));
    resultTable_->setItem(i, 1, new QTableWidgetItem(t.testName));
    const QString status =
        t.skipped ? tr("SKIP") : (t.passed ? tr("PASS") : tr("FAIL"));
    resultTable_->setItem(i, 2, new QTableWidgetItem(status));
    resultTable_->setItem(
        i, 3,
        new QTableWidgetItem(QStringLiteral("%1 ms").arg(t.durationMs)));
    resultTable_->setItem(i, 4, new QTableWidgetItem(t.details));
  }
  summaryLabel_->setText(QStringLiteral("Results: %1 passed, %2 failed, "
                                         "%3 skipped (%4 ms)")
                             .arg(result.passed)
                             .arg(result.failed)
                             .arg(result.skipped)
                             .arg(result.totalDurationMs));
}

void DeviceVerificationWidget::clear() {
  resultTable_->setRowCount(0);
  summaryLabel_->clear();
  progress_->setValue(0);
}
