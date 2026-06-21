#include "FirmwareUpdateService.h"
#include "infra/EcatClient.h"

// FirmwareUpdateService.cpp — Firmware update orchestration with progress tracking
//
// Implementation notes:
//   - Timer-driven progress simulation through upload/verify/apply/finalize phases
//   - Only one update may be active at a time (guarded by updating_ flag)
//   - Cancel aborts in-flight updates and resets state

FirmwareUpdateService::FirmwareUpdateService(EcatClient *client,
                                             QObject *parent)
    : QObject(parent), client_(client) {
  progressTimer_ = new QTimer(this);
  connect(progressTimer_, &QTimer::timeout,
          this, &FirmwareUpdateService::advanceProgress);
}

void FirmwareUpdateService::checkForUpdates(int position) {
  if (!client_ || !client_->isConnected()) return;
  client_->slaveInfo(position);
}

bool FirmwareUpdateService::startUpdate(int position,
                                         const QString &firmwarePath) {
  if (updating_) return false;
  if (!client_ || !client_->isConnected()) return false;

  updating_ = true;
  targetPosition_ = position;
  firmwarePath_ = firmwarePath;
  progress_ = 0;

  emit updateStarted(position);
  emit updateProgressChanged(0, QStringLiteral("Starting"));
  progressTimer_->start(200);
  return true;
}

void FirmwareUpdateService::cancelUpdate() {
  if (!updating_) return;
  progressTimer_->stop();
  updating_ = false;
  emit updateFailed(targetPosition_, QStringLiteral("Cancelled by user"));
  targetPosition_ = -1;
  progress_ = 0;
}

int FirmwareUpdateService::updateProgress() const { return progress_; }

bool FirmwareUpdateService::isUpdating() const { return updating_; }

// Advances simulated progress through four phases, emitting status at each step
void FirmwareUpdateService::advanceProgress() {
  progress_ += 5;
  if (progress_ >= 100) {
    progressTimer_->stop();
    updating_ = false;
    emit updateProgressChanged(100, QStringLiteral("Complete"));
    emit updateCompleted(targetPosition_);
    targetPosition_ = -1;
    progress_ = 0;
  } else {
    QString status;
    if (progress_ < 30)
      status = QStringLiteral("Uploading firmware");
    else if (progress_ < 60)
      status = QStringLiteral("Verifying");
    else if (progress_ < 90)
      status = QStringLiteral("Applying update");
    else
      status = QStringLiteral("Finalizing");
    emit updateProgressChanged(progress_, status);
  }
}
