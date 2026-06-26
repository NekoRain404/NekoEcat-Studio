#include "FirmwareUpdateService.h"
#include "infra/EcatClient.h"

// FirmwareUpdateService.cpp — Firmware update facade
//
// Implementation notes:
//   - Fails closed until a real daemon firmware-update API exists
//   - Only one update may be active at a time if a backend is added later
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
  Q_UNUSED(position);
  Q_UNUSED(firmwarePath);
  return false;
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

// Reserved for a future backend-driven progress path.
void FirmwareUpdateService::advanceProgress() {
}
