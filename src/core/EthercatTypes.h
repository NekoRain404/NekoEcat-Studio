#pragma once

// Shared EtherCAT domain types: SlaveInfo, master state, port info.
//
// These types form the common data model shared between the daemon (ecatd),
// the GUI client (ecat-studio), and the backend adapters (CLI/native).
// Serialization to/from JSON is provided for wire transport over TCP.

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

/// @brief Represents a single EtherCAT slave discovered on the bus.
///
/// Populated by EcatService::scanSlaves(). The `position` field corresponds
/// to the slave's bus address. `rawLine` preserves the original CLI output
/// for debugging when the CLI backend is used.
struct SlaveInfo {
    int position = -1;   ///< Bus position (address) of the slave.
    QString state;       ///< Current state: "OP", "PREOP", "SAFEOP", "INIT", etc.
    QString flags;       ///< Device flags (e.g. "A----", "SR----").
    QString name;        ///< Product name from the ESI/XML description.
    QString rawLine;     ///< Raw CLI output line for debugging.

    bool operator==(const SlaveInfo &other) const = default;
};
Q_DECLARE_METATYPE(SlaveInfo)
Q_DECLARE_METATYPE(QVector<SlaveInfo>)

/// @brief Raw master information for text-mode display.
struct MasterInfo {
    QString rawText;  ///< Raw `ethercat master` output (timing, DC info, topology).
};

/// @brief Serialize a single SlaveInfo to JSON.
QJsonObject toJson(const SlaveInfo &slave);

/// @brief Serialize a vector of SlaveInfo to a JSON array.
QJsonArray toJson(const QVector<SlaveInfo> &slaves);

/// @brief Deserialize a SlaveInfo from a JSON object.
/// @return SlaveInfo with fields populated; position defaults to -1 on missing data.
SlaveInfo slaveFromJson(const QJsonObject &object);

/// @brief Deserialize a vector of SlaveInfo from a JSON array.
QVector<SlaveInfo> slavesFromJson(const QJsonArray &array);
