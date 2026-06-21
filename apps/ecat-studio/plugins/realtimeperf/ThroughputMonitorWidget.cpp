// ThroughputMonitorWidget — numeric readouts + progress bar for bus throughput.
//
// Displays frame rate, byte rate, error rate, utilization, and cumulative
// totals in a clean grid layout.

#include "ThroughputMonitorWidget.h"
#include "services/RealtimePerformanceService.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

ThroughputMonitorWidget::ThroughputMonitorWidget(QWidget *parent)
    : QWidget(parent) {
  buildUi();
}

static QString formatBytes(double bytes) {
  if (bytes >= 1024.0 * 1024)
    return QString::number(bytes / (1024.0 * 1024), 'f', 1) + " MB/s";
  if (bytes >= 1024.0)
    return QString::number(bytes / 1024.0, 'f', 1) + " KB/s";
  return QString::number(bytes, 'f', 0) + " B/s";
}

static QString formatCount(quint64 count) {
  if (count >= 1000000)
    return QString::number(count / 1000000.0, 'f', 2) + " M";
  if (count >= 1000)
    return QString::number(count / 1000.0, 'f', 1) + " K";
  return QString::number(count);
}

void ThroughputMonitorWidget::buildUi() {
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);

  auto *ratesGroup = new QGroupBox(tr("Current Rates"));
  auto *ratesGrid = new QGridLayout(ratesGroup);

  frameRateLabel_ = new QLabel("--");
  frameRateLabel_->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 14px;");
  byteRateLabel_ = new QLabel("--");
  byteRateLabel_->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 14px;");
  errorRateLabel_ = new QLabel("--");
  errorRateLabel_->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 14px;");
  utilizationLabel_ = new QLabel("--");
  utilizationLabel_->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 14px;");

  auto addRow = [&](const QString &name, QLabel *val, int row) {
    auto *lbl = new QLabel(name);
    lbl->setStyleSheet("color: #8888aa;");
    ratesGrid->addWidget(lbl, row, 0);
    ratesGrid->addWidget(val, row, 1);
  };

  addRow(tr("Frame Rate:"), frameRateLabel_, 0);
  addRow(tr("Byte Rate:"), byteRateLabel_, 1);
  addRow(tr("Error Rate:"), errorRateLabel_, 2);
  addRow(tr("Utilization:"), utilizationLabel_, 3);

  utilizationBar_ = new QProgressBar;
  utilizationBar_->setRange(0, 100);
  utilizationBar_->setTextVisible(true);
  utilizationBar_->setStyleSheet(
      "QProgressBar { background: #2a2a3e; border: 1px solid #3e3e5e; border-radius: 3px; }"
      "QProgressBar::chunk { background: #60a5fa; border-radius: 2px; }");
  ratesGrid->addWidget(utilizationBar_, 4, 0, 1, 2);

  mainLayout->addWidget(ratesGroup);

  auto *totalsGroup = new QGroupBox(tr("Cumulative Totals"));
  auto *totalsGrid = new QGridLayout(totalsGroup);

  totalFramesLabel_ = new QLabel("0");
  totalFramesLabel_->setStyleSheet("color: #cccccc;");
  totalBytesLabel_ = new QLabel("0");
  totalBytesLabel_->setStyleSheet("color: #cccccc;");
  totalErrorsLabel_ = new QLabel("0");
  totalErrorsLabel_->setStyleSheet("color: #cccccc;");

  auto addTotalRow = [&](const QString &name, QLabel *val, int row) {
    auto *lbl = new QLabel(name);
    lbl->setStyleSheet("color: #8888aa;");
    totalsGrid->addWidget(lbl, row, 0);
    totalsGrid->addWidget(val, row, 1);
  };

  addTotalRow(tr("Total Frames:"), totalFramesLabel_, 0);
  addTotalRow(tr("Total Bytes:"), totalBytesLabel_, 1);
  addTotalRow(tr("Total Errors:"), totalErrorsLabel_, 2);

  mainLayout->addWidget(totalsGroup);
  mainLayout->addStretch();
}

void ThroughputMonitorWidget::updateMetrics(const ThroughputMetrics &m) {
  frameRateLabel_->setText(QString::number(m.framesPerSecond, 'f', 1) + " f/s");
  byteRateLabel_->setText(formatBytes(m.bytesPerSecond));
  errorRateLabel_->setText(QString::number(m.errorRate, 'f', 2) + " /s");

  QString utilText = QString::number(m.utilizationPercent, 'f', 1) + "%";
  utilizationLabel_->setText(utilText);
  utilizationBar_->setValue(static_cast<int>(m.utilizationPercent));

  if (m.utilizationPercent > 80.0) {
    utilizationBar_->setStyleSheet(
        "QProgressBar { background: #2a2a3e; border: 1px solid #3e3e5e; border-radius: 3px; }"
        "QProgressBar::chunk { background: #ef4444; border-radius: 2px; }");
  } else if (m.utilizationPercent > 60.0) {
    utilizationBar_->setStyleSheet(
        "QProgressBar { background: #2a2a3e; border: 1px solid #3e3e5e; border-radius: 3px; }"
        "QProgressBar::chunk { background: #f59e0b; border-radius: 2px; }");
  } else {
    utilizationBar_->setStyleSheet(
        "QProgressBar { background: #2a2a3e; border: 1px solid #3e3e5e; border-radius: 3px; }"
        "QProgressBar::chunk { background: #22c55e; border-radius: 2px; }");
  }

  totalFramesLabel_->setText(formatCount(m.totalFrames));
  totalBytesLabel_->setText(formatCount(m.totalBytes));
  totalErrorsLabel_->setText(formatCount(m.totalErrors));
}
