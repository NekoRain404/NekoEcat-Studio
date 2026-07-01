#pragma once

// Abstract interface for EtherCAT backend operations.
// Decouples the daemon dispatch layer from the CLI implementation.
// Allows alternative backends (ecrt, mock, etc.) without changing the daemon.
//
// Implementations:
//   - EthercatCliBackend: shells out to `ethercat` CLI (slow, requires IgH CLI)
//   - EthercatNativeBackend: uses ecrt API directly (fast, 10-100x performance)
//   - MockEcatClient: test double for unit tests
//
// Thread safety: implementations must be thread-safe for concurrent requests
// from the daemon's JSON-RPC dispatcher.

#include "EthercatTypes.h"

#include <QJsonArray>
#include <QString>

class EcatService {
public:
    virtual ~EcatService() = default;

    /// @brief Query master information text (timing, DC info, topology).
    /// @param master Target master index (e.g. "0").
    /// @param error Optional error output; non-null on failure.
    /// @return Raw master info text for display.
    virtual QString masterText(const QString &master, QString *error = nullptr) const = 0;

    /// @brief Scan all slaves on the bus and return structured info.
    /// @param master Target master index.
    /// @param error Optional error output.
    /// @return List of discovered SlaveInfo structs.
    virtual QVector<SlaveInfo> scanSlaves(const QString &master, QString *error = nullptr) const = 0;

    /// @brief Get detailed information for a specific slave.
    /// @param master Target master index.
    /// @param position Slave position on the bus.
    /// @param error Optional error output.
    /// @return Raw slave info text.
    virtual QString slaveInfo(const QString &master, int position, QString *error = nullptr) const = 0;

    /// @brief Get the ESI/XML description for a specific slave.
    /// @param master Target master index.
    /// @param position Slave position on the bus.
    /// @param error Optional error output.
    /// @return Raw XML content string.
    virtual QString slaveXml(const QString &master, int position, QString *error = nullptr) const = 0;

    /// @brief List Process Data Objects (PDOs) mapped to a slave.
    /// @param master Target master index.
    /// @param position Slave position on the bus.
    /// @param error Optional error output.
    /// @return Raw PDO mapping text.
    virtual QString pdos(const QString &master, int position, QString *error = nullptr) const = 0;

    /// @brief List Service Data Objects (SDOs) available on a slave.
    /// @param master Target master index.
    /// @param position Slave position on the bus.
    /// @param error Optional error output.
    /// @return Raw SDO catalog text.
    virtual QString sdos(const QString &master, int position, QString *error = nullptr) const = 0;

    /// @brief Upload (read) an SDO value from a slave.
    /// @param master Target master index.
    /// @param position Slave position on the bus.
    /// @param index SDO index (hex string, e.g. "0x1000").
    /// @param subIndex SDO sub-index (hex string, e.g. "0x00").
    /// @param type Optional data type hint (e.g. "uint32").
    /// @param error Optional error output.
    /// @return The uploaded value as a string.
    virtual QString upload(const QString &master, int position, const QString &index,
                           const QString &subIndex, const QString &type = QString(),
                           QString *error = nullptr) const = 0;

    /// @brief Download (write) an SDO value to a slave.
    /// @param master Target master index.
    /// @param position Slave position on the bus.
    /// @param index SDO index (hex string).
    /// @param subIndex SDO sub-index (hex string).
    /// @param value Value to write (string representation).
    /// @param type Data type (e.g. "uint32", "int8", "string").
    /// @param error Optional error output.
    /// @return true on success, false on failure.
    virtual bool download(const QString &master, int position, const QString &index,
                          const QString &subIndex, const QString &value,
                          const QString &type, QString *error = nullptr) const = 0;

    /// @brief Set the operational state of a single slave.
    /// @param master Target master index.
    /// @param position Slave position on the bus.
    /// @param state Target state (e.g. "OP", "PREOP", "SAFEOP", "INIT").
    /// @param error Optional error output.
    /// @return true on success.
    virtual bool setState(const QString &master, int position, const QString &state,
                          QString *error = nullptr) const = 0;

    /// @brief Set the operational state of all slaves on the bus.
    /// @param master Target master index.
    /// @param state Target state for all slaves.
    /// @param error Optional error output.
    /// @return true if all slaves transitioned successfully.
    virtual bool setAllStates(const QString &master, const QString &state,
                              QString *error = nullptr) const = 0;

    /// @brief Force a bus rescan to rediscover slaves.
    /// @param master Target master index.
    /// @param error Optional error output.
    /// @return true on success.
    virtual bool rescan(const QString &master, QString *error = nullptr) const = 0;

    /// @brief Run host-level diagnostics (network, CPU, IgH driver status).
    /// @param error Optional error output.
    /// @return JSON array of diagnostic check results.
    virtual QJsonArray hostDiagnostics(QString *error = nullptr) const = 0;

    /// @brief Whether this backend uses the native ecrt API (vs CLI).
    /// @return true for native backend, false for CLI backend.
    virtual bool isNative() const { return false; }

    /// @brief Whether the last operation fell back to CLI due to ecrt API limitations.
    /// @return true if the last operation used CLI fallback.
    virtual bool lastOperationWasFallback() const { return false; }

    /// @brief Human-readable reason for the last CLI fallback.
    /// @return Explanation string, empty if no fallback occurred.
    virtual QString lastFallbackReason() const { return {}; }
};
