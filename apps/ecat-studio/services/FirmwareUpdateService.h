#pragma once

// FirmwareUpdateService — manages firmware update workflow for EtherCAT slaves.
//
// Handles update checking, staged upload with progress tracking, and
// cancellation. Uses QTimer for progress simulation during updates.
//
// This service provides firmware update capabilities for EtherCAT slaves.
// It handles:
//   - Firmware update checking for specific slaves
//   - Staged firmware upload with progress tracking
//   - Update cancellation support
//   - Progress reporting via signals
//   - Error handling and recovery
//
// Usage:
//   ServiceContainer *container = ...;
//   FirmwareUpdateService *firmware = container->firmwareUpdate();
//   firmware->checkForUpdates(0);  // Check slave 0 for updates
//   firmware->startUpdate(0, "/path/to/firmware.bin");
//   // Monitor progress
//   connect(firmware, &FirmwareUpdateService::updateProgressChanged, ...);
//   firmware->cancelUpdate();  // Cancel if needed
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   uses a QTimer for progress simulation, which runs on the main thread.
//
// Performance:
//   - Update checking is O(1) per slave
//   - Firmware upload progress is simulated (not real I/O)
//   - Cancellation is immediate

#include <QObject>
#include <QTimer>

class EcatClient;

class FirmwareUpdateService : public QObject {
  Q_OBJECT
public:
  explicit FirmwareUpdateService(EcatClient *client,
                                 QObject *parent = nullptr);

  // Check for firmware updates for a specific slave.
  // @param position  Slave position on the bus
  void checkForUpdates(int position);

  // Start a firmware update for a specific slave.
  // @param position      Slave position on the bus
  // @param firmwarePath  Path to the firmware file
  // @return true if update was started successfully
  bool startUpdate(int position, const QString &firmwarePath);

  // Cancel the current firmware update.
  void cancelUpdate();

  // Get the current update progress.
  // @return Progress percentage (0-100)
  int updateProgress() const;

  // Check if a firmware update is in progress.
  // @return true if update is in progress
  bool isUpdating() const;

signals:
  // Emitted when a firmware update starts.
  // @param position  Slave position
  void updateStarted(int position);

  // Emitted when update progress changes.
  // @param percent  Progress percentage (0-100)
  // @param status   Human-readable status message
  void updateProgressChanged(int percent, const QString &status);

  // Emitted when a firmware update completes successfully.
  // @param position  Slave position
  void updateCompleted(int position);

  // Emitted when a firmware update fails.
  // @param position  Slave position
  // @param error     Human-readable error message
  void updateFailed(int position, const QString &error);

private:
  // Advance the progress simulation.
  void advanceProgress();

  EcatClient *client_;              // TCP client to ecatd daemon
  QTimer *progressTimer_ = nullptr; // Timer for progress simulation
  bool updating_ = false;           // Whether update is in progress
  int targetPosition_ = -1;         // Target slave position
  int progress_ = 0;                // Current progress percentage
  QString firmwarePath_;            // Path to firmware file
};
