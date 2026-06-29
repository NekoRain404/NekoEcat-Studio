#pragma once

// FirmwareUpdateService — firmware update facade for EtherCAT slaves.
//
// Handles update checking and exposes the future update workflow boundary.
// Firmware writes fail closed until the daemon exposes a real firmware-update
// API; progress must not be simulated as successful device I/O.
//
// This service currently handles:
//   - Firmware update checking for specific slaves
//   - Explicit rejection of unsupported offline firmware writes
//   - Idle cancellation support
//   - Future progress reporting signals
//
// Usage:
//   ServiceContainer *container = ...;
//   FirmwareUpdateService *firmware = container->firmwareUpdate();
//   firmware->checkForUpdates(0);  // Check slave 0 for updates
//   // Returns false until connected to a backend with real firmware support:
//   firmware->startUpdate(0, "/path/to/firmware.bin");
//   // Monitor progress
//   connect(firmware, &FirmwareUpdateService::updateProgressChanged, ...);
//   firmware->cancelUpdate();  // Cancel if needed
//
// Thread safety:
//   All methods must be called from the main (GUI) thread.
//
// Performance:
//   - Update checking is O(1) per slave
//   - Unsupported firmware writes are rejected in O(1)
//   - Cancellation is immediate

#include <QObject>
#include <QTimer>

class EcatClient;

/// @brief Firmware update facade for EtherCAT slaves.
///
/// Handles update checking and exposes the future update workflow boundary.
/// Firmware writes fail closed until the daemon exposes a real firmware-update
/// API; progress must not be simulated as successful device I/O.
class FirmwareUpdateService : public QObject {
  Q_OBJECT
public:
  /// @brief Construct the firmware update service.
  /// @param client  TCP client to the ecatd daemon.
  /// @param parent  Parent QObject.
  explicit FirmwareUpdateService(EcatClient *client,
                                 QObject *parent = nullptr);

  /// @brief Check for firmware updates for a specific slave.
  /// @param position  Slave position on the bus.
  void checkForUpdates(int position);

  /// @brief Start a firmware update for a specific slave.
  /// @param position      Slave position on the bus.
  /// @param firmwarePath  Path to the firmware binary file.
  /// @return true if update was started successfully; currently always false
  ///         without backend firmware-update API support.
  bool startUpdate(int position, const QString &firmwarePath);

  /// @brief Cancel the current firmware update.
  void cancelUpdate();

  /// @brief Get the current firmware update progress.
  /// @return Progress percentage (0-100).
  int updateProgress() const;

  /// @brief Check if a firmware update is in progress.
  /// @return true if an update is currently in progress.
  bool isUpdating() const;

signals:
  /// @brief Emitted when a firmware update starts.
  /// @param position  Slave position being updated.
  void updateStarted(int position);

  /// @brief Emitted when firmware update progress changes.
  /// @param percent  Progress percentage (0-100).
  /// @param status   Human-readable status message.
  void updateProgressChanged(int percent, const QString &status);

  /// @brief Emitted when a firmware update completes successfully.
  /// @param position  Slave position that was updated.
  void updateCompleted(int position);

  /// @brief Emitted when a firmware update fails.
  /// @param position  Slave position where the update failed.
  /// @param error     Human-readable error message.
  void updateFailed(int position, const QString &error);

private:
  /// @brief Advance backend-driven progress once real firmware support exists.
  void advanceProgress();

  EcatClient *client_;              ///< TCP client to ecatd daemon.
  QTimer *progressTimer_ = nullptr; ///< Timer reserved for backend progress polling.
  bool updating_ = false;           ///< Whether an update is in progress.
  int targetPosition_ = -1;         ///< Target slave position for current update.
  int progress_ = 0;                ///< Current progress percentage (0-100).
  QString firmwarePath_;            ///< Path to firmware binary file.
};
