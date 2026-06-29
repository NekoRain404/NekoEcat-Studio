#pragma once

// EsiService — manages ESI (EtherCAT Slave Information) XML repository.
// Parses ESI XML files, stores device info, and provides lookup/match/export.
//
// This service provides ESI XML management capabilities for the EtherCAT
// network. It handles:
//   - ESI XML file import and parsing
//   - Device information storage and indexing
//   - Device matching by vendor/product ID
//   - PDO mapping lookup from ESI data
//   - ESI export for specific devices
//   - Device count and listing
//
// Usage:
//   ServiceContainer *container = ...;
//   EsiService *esi = container->esi();
//   esi->importEsi("/path/to/device.xml");
//   EsiDeviceInfo device = esi->matchDevice(0x0002, 0x0001);
//   QVector<EsiDeviceInfo> devices = esi->listDevices();
//   esi->exportEsi("device_id", "/path/to/export.xml");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. ESI parsing
//   is synchronous and blocks the calling thread.
//
// Performance:
//   - ESI import is O(n) where n is XML file size
//   - Device lookup is O(1) by vendor/product ID
//   - PDO mapping lookup is O(n) where n is number of PDOs

#include <QObject>
#include <QVector>
#include <QString>
#include <QMap>

/// @brief Represents a single PDO entry in an ESI descriptor.
struct EsiPdoEntry {
  QString index;       ///< PDO index in hex format.
  QString subIndex;    ///< PDO subindex in hex format.
  QString name;        ///< PDO entry name.
  QString type;        ///< Data type (e.g., "UINT8", "INT16").
  int bitSize = 0;     ///< Bit size of the entry.
};

/// @brief Represents a PDO assignment (mapping) in an ESI descriptor.
struct EsiPdoAssignment {
  QString index;                    ///< PDO index in hex format.
  QString name;                     ///< PDO name.
  QVector<EsiPdoEntry> entries;     ///< PDO entries in this assignment.
};

/// @brief Represents a sync manager in an ESI descriptor.
struct EsiSyncManager {
  int index = 0;        ///< Sync manager index.
  QString name;         ///< Sync manager name.
  QString direction;    ///< Direction ("input" or "output").
  int pdos = 0;         ///< Number of PDOs assigned to this sync manager.
};

/// @brief Represents device information parsed from an ESI XML file.
struct EsiDeviceInfo {
  QString deviceId;                           ///< Unique device identifier.
  QString name;                               ///< Device name.
  QString type;                               ///< Device type.
  QString description;                        ///< Device description.
  int vendorId = 0;                           ///< Vendor ID.
  int productCode = 0;                        ///< Product code.
  int revisionNo = 0;                         ///< Revision number.
  QVector<EsiPdoAssignment> rxPdos;           ///< Receive PDO assignments.
  QVector<EsiPdoAssignment> txPdos;           ///< Transmit PDO assignments.
  QVector<EsiSyncManager> syncManagers;       ///< Sync managers.
};

/// @brief Manages the ESI (EtherCAT Slave Information) XML repository.
///
/// Parses ESI XML files, stores device information, and provides lookup,
/// match, and export capabilities for EtherCAT slave device descriptors.
class EsiService : public QObject {
  Q_OBJECT
public:
  /// @brief Construct the ESI service.
  /// @param parent  Parent QObject.
  explicit EsiService(QObject *parent = nullptr);

  /// @brief Import an ESI XML file into the repository.
  /// @param filePath  Path to the ESI XML file.
  /// @return true if import was successful.
  bool importEsi(const QString &filePath);

  /// @brief Match a device by vendor ID and product code.
  /// @param vendorId     Vendor ID to match.
  /// @param productCode  Product code to match.
  /// @return EsiDeviceInfo structure (empty if not found).
  EsiDeviceInfo matchDevice(int vendorId, int productCode) const;

  /// @brief List all imported devices.
  /// @return Vector of EsiDeviceInfo structures.
  QVector<EsiDeviceInfo> listDevices() const;

  /// @brief Export a specific device's ESI data to an XML file.
  /// @param deviceId    Device identifier to export.
  /// @param outputPath  Output file path.
  /// @return true if export was successful.
  bool exportEsi(const QString &deviceId, const QString &outputPath) const;

  /// @brief Get the number of imported devices.
  /// @return Number of devices in the repository.
  int deviceCount() const;

  /// @brief Clear all imported ESI data from the repository.
  void clear();

signals:
  /// @brief Emitted when ESI data is successfully imported.
  /// @param deviceCount  Number of devices imported from the file.
  void esiImported(int deviceCount);

  /// @brief Emitted when an ESI operation error occurs.
  /// @param msg  Human-readable error message.
  void error(const QString &msg);

private:
  /// @brief Parse a hex (0x-prefixed) or decimal string to integer.
  /// @param s  String to parse.
  /// @return Parsed integer value.
  static int parseHexOrDec(const QString &s);

  QVector<EsiDeviceInfo> devices_;      ///< Imported device descriptors.
  QMap<QString, int> deviceIndex_;      ///< Device ID to index mapping for O(1) lookup.
};
