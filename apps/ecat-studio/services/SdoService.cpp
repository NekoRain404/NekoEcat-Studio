#include "SdoService.h"
#include "EcatClient.h"

#include <QDateTime>

// SdoService.cpp — SDO upload/download with error translation and timeout tracking
//
// Implementation notes:
//   - Delegates upload/download calls directly to EcatClient
//   - Relays sdoValue and errorMessage signals from the client layer
//   - Translates raw SDO errors into user-friendly messages
//   - Tracks pending operations and warns on timeouts

SdoService::SdoService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
  connect(client_, &EcatClient::sdoValue, this,
          [this](int pos, const QString &idx, const QString &sub, const QString &val) {
            clearPendingOperation(pos, idx, sub);
            emit sdoValueReceived(pos, idx, sub, val);
          });
  connect(client_, &EcatClient::errorMessage, this,
          [this](const QString &msg) { emit error(translateSdoError(msg)); });

  // Periodic timeout check — warn if SDO ops are stuck for >5s.
  timeoutCheckTimer_ = new QTimer(this);
  timeoutCheckTimer_->setInterval(1000);
  connect(timeoutCheckTimer_, &QTimer::timeout, this, [this]() {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    for (const auto &op : std::as_const(pendingOps_)) {
      const int elapsed = static_cast<int>(now - op.startTimeMs);
      if (elapsed > kSdoTimeoutWarningMs) {
        emit sdoTimeoutWarning(op.position, op.index, op.subIndex, elapsed);
      }
    }
  });
  timeoutCheckTimer_->start();
}

void SdoService::upload(int position, const QString &index, const QString &subIndex) {
  trackPendingOperation(position, index, subIndex, "upload");
  client_->upload(position, index, subIndex);
}

void SdoService::download(int position, const QString &index, const QString &subIndex,
                          const QString &value, const QString &type) {
  Q_UNUSED(value);
  Q_UNUSED(type);
  trackPendingOperation(position, index, subIndex, "download");
  client_->download(position, index, subIndex, value, type);
}

void SdoService::trackPendingOperation(int position, const QString &index,
                                        const QString &subIndex, const QString &opType) {
  clearPendingOperation(position, index, subIndex);
  pendingOps_.append({position, index, subIndex, opType, QDateTime::currentMSecsSinceEpoch()});
}

void SdoService::clearPendingOperation(int position, const QString &index,
                                        const QString &subIndex) {
  pendingOps_.removeIf([&](const PendingOp &op) {
    return op.position == position && op.index == index && op.subIndex == subIndex;
  });
}

// Translate raw SDO/daemon error strings into user-friendly messages with suggested actions.
QString SdoService::translateSdoError(const QString &rawError) {
  if (rawError.isEmpty()) return rawError;

  const QString lower = rawError.toLower();

  // IgH CLI SDO abort codes (0x05030000–0x05040000 range)
  if (lower.contains("0x05030000") || lower.contains("toggle bit"))
    return QObject::tr("SDO protocol error: toggle bit not alternated. "
                       "The slave may be unresponsive. Try again or power-cycle the slave.");
  if (lower.contains("0x05040000") || lower.contains("timeout") || lower.contains("sdo timeout"))
    return QObject::tr("SDO timeout: the slave did not respond in time. "
                       "Check that the slave is in PREOP or SAFEOP state and the index/subindex exists.");
  if (lower.contains("0x05040001") || lower.contains("not found"))
    return QObject::tr("SDO timeout: command specifier not valid or unknown. "
                       "Verify the SDO index and subindex are correct for this slave type.");
  if (lower.contains("0x05040005") || lower.contains("out of memory"))
    return QObject::tr("SDO error: slave out of memory. The slave cannot process this request.");
  if (lower.contains("0x06010000") || lower.contains("unsupported"))
    return QObject::tr("SDO access error: unsupported access to this object. "
                       "The SDO may be read-only or write-only.");
  if (lower.contains("0x06010001") || lower.contains("read only"))
    return QObject::tr("SDO access error: this object is read-only. "
                       "Use upload (read) instead of download (write).");
  if (lower.contains("0x06010002") || lower.contains("write only"))
    return QObject::tr("SDO access error: this object is write-only. "
                       "Use download (write) instead of upload (read).");
  if (lower.contains("0x06020000") || lower.contains("does not exist"))
    return QObject::tr("SDO error: object does not exist in the slave's dictionary. "
                       "Verify the index is valid for this slave's ESI/XML.");
  if (lower.contains("0x06040041") || lower.contains("pdo mapping"))
    return QObject::tr("SDO error: object cannot be mapped to a PDO. "
                       "The object may be too large or have an incompatible data type.");
  if (lower.contains("0x06040042") || lower.contains("pdo length"))
    return QObject::tr("SDO error: PDO length exceeded. "
                       "Reduce the number of mapped objects or use smaller data types.");
  if (lower.contains("0x06040043") || lower.contains("parameter"))
    return QObject::tr("SDO error: general parameter incompatibility. "
                       "Check the data type and value range for this SDO.");
  if (lower.contains("0x06040047") || lower.contains("abort"))
    return QObject::tr("SDO error: general internal incompatibility in the slave. "
                       "The slave firmware may need an update.");
  if (lower.contains("0x06060000") || lower.contains("hardware error"))
    return QObject::tr("SDO error: access failed due to a hardware error. "
                       "Check the slave's physical connection and power supply.");
  if (lower.contains("0x06070012") || lower.contains("too long"))
    return QObject::tr("SDO error: data type does not match, value too long. "
                       "Verify the data type matches the object dictionary entry.");
  if (lower.contains("0x06070013") || lower.contains("too short"))
    return QObject::tr("SDO error: data type does not match, value too short. "
                       "Verify the data type matches the object dictionary entry.");
  if (lower.contains("0x06090011") || lower.contains("subindex does not exist"))
    return QObject::tr("SDO error: subindex does not exist. "
                       "Check the slave's object dictionary for valid subindices.");
  if (lower.contains("0x06090030") || lower.contains("value range"))
    return QObject::tr("SDO error: value out of range. "
                       "The requested value exceeds the min/max for this parameter.");
  if (lower.contains("0x06090031") || lower.contains("too large"))
    return QObject::tr("SDO error: value too large for this parameter. "
                       "Reduce the value to fit within the allowed range.");
  if (lower.contains("0x06090032") || lower.contains("too small"))
    return QObject::tr("SDO error: value too small for this parameter. "
                       "Increase the value to fit within the allowed range.");
  if (lower.contains("0x08000000") || lower.contains("general error"))
    return QObject::tr("SDO error: general error in the slave. "
                       "Check slave diagnostics and try again.");

  // Daemon/connection-level errors
  if (lower.contains("not connected") || lower.contains("ecatd is not connected"))
    return QObject::tr("Cannot perform SDO operation: daemon is not connected. "
                       "Wait for reconnection or restart the application.");
  if (lower.contains("connection refused"))
    return QObject::tr("SDO operation failed: connection to daemon was refused. "
                       "Ensure ecatd is running on port 5877.");
  if (lower.contains("timed out") || lower.contains("timeout"))
    return QObject::tr("SDO operation timed out. The slave may be unresponsive "
                       "or the bus may be experiencing high load.");
  if (lower.contains("no such file") || lower.contains("not found"))
    return QObject::tr("SDO operation failed: the specified slave or master was not found. "
                       "Verify the bus is scanned and the slave position is correct.");

  // Return original with a prefix if no translation matched
  return QObject::tr("SDO error: %1").arg(rawError);
}
