#pragma once

// SdoService — manages SDO read/write operations, dictionary caching, and evidence tracking.
//
// This service provides the primary interface for SDO (Service Data Object) operations
// in the EtherCAT network. It handles:
//   - SDO upload (read) operations to retrieve values from slaves
//   - SDO download (write) operations to send values to slaves
//   - Dictionary caching for frequently accessed SDOs
//   - Evidence tracking for SDO read/write history
//   - SDO error code translation for user-friendly messages
//   - Timeout detection for long-running SDO operations
//
// Usage:
//   ServiceContainer *container = ...;
//   SdoService *sdo = container->sdo();
//   sdo->upload(0, "0x6000", "0x01");  // Read from slave 0
//   sdo->download(0, "0x6000", "0x01", "0xFF", "UINT8");  // Write to slave 0
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - SDO reads are cached with configurable TTL (default 30s)
//   - Batch operations are supported via BatchOperationService
//   - Evidence tracking adds minimal overhead to read/write operations

#include <QObject>
#include <QString>
#include <QTimer>

class EcatClient;

class SdoService : public QObject {
  Q_OBJECT
public:
  explicit SdoService(EcatClient *client, QObject *parent = nullptr);

  // Upload (read) an SDO value from a slave.
  // @param position  Slave position on the bus (0-based)
  // @param index     SDO index in hex format (e.g., "0x6000")
  // @param subIndex  SDO subindex in hex format (e.g., "0x01")
  // Emits sdoValueReceived() on success, error() on failure.
  void upload(int position, const QString &index, const QString &subIndex);

  // Download (write) an SDO value to a slave.
  // @param position  Slave position on the bus (0-based)
  // @param index     SDO index in hex format (e.g., "0x6000")
  // @param subIndex  SDO subindex in hex format (e.g., "0x01")
  // @param value     Value to write (string representation)
  // @param type      Data type (e.g., "UINT8", "INT16", "STRING")
  // Emits sdoValueReceived() on success, error() on failure.
  void download(int position, const QString &index, const QString &subIndex,
                const QString &value, const QString &type);

  // Translate an SDO error code or message into a user-friendly description.
  // @param rawError  The raw error string from the daemon
  // @return Human-readable error description with suggested action
  static QString translateSdoError(const QString &rawError);

signals:
  // Emitted when an SDO value is successfully read or written.
  // @param position  Slave position
  // @param index     SDO index
  // @param subIndex  SDO subindex
  // @param value     The value read/written
  void sdoValueReceived(int position, const QString &index, const QString &subIndex, const QString &value);

  // Emitted when an SDO operation fails.
  // @param message  Human-readable error description
  void error(const QString &message);

  // Emitted when an SDO operation is taking longer than expected.
  // @param position  Slave position
  // @param index     SDO index
  // @param subIndex  SDO subindex
  // @param elapsedMs How long the operation has been pending
  void sdoTimeoutWarning(int position, const QString &index, const QString &subIndex, int elapsedMs);

private:
  void trackPendingOperation(int position, const QString &index, const QString &subIndex, const QString &opType);
  void clearPendingOperation(int position, const QString &index, const QString &subIndex);

  EcatClient *client_;  // TCP client to ecatd daemon
  QTimer *timeoutCheckTimer_;  // Periodic check for slow SDO operations

  struct PendingOp {
    int position;
    QString index;
    QString subIndex;
    QString opType;
    qint64 startTimeMs;
  };
  QVector<PendingOp> pendingOps_;
  static constexpr int kSdoTimeoutWarningMs = 5000;  // Warn after 5s
};
