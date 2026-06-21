#pragma once

// PdoMappingService — PDO mapping management, validation, and export.
//
// This service provides Process Data Object (PDO) mapping management
// for the EtherCAT network. It handles:
//   - PDO mapping discovery from slaves
//   - PDO mapping configuration
//   - PDO mapping validation (bit size, direction)
//   - PDO mapping export/import
//   - Current mapping tracking per slave
//
// Usage:
//   ServiceContainer *container = ...;
//   PdoMappingService *pdo = container->pdoMapping();
//   QVector<PdoMapping> mappings = pdo->discoverMappings(0);
//   PdoMapping mapping;
//   mapping.index = "0x6000";
//   mapping.subIndex = "0x01";
//   mapping.name = "Velocity";
//   mapping.dataType = "INT16";
//   mapping.bitSize = 16;
//   mapping.direction = PdoDirection::Input;
//   mapping.slavePosition = 0;
//   PdoValidationResult result = pdo->validateMapping(mapping);
//   if (result.valid) {
//     pdo->configureMapping(0, mapping);
//   }
//   pdo->exportMapping(0, "/path/to/mapping.json");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. PDO operations
//   are synchronous and block the calling thread.
//
// Performance:
//   - PDO discovery is O(n) where n is number of PDOs
//   - PDO validation is O(1)
//   - PDO export/import is O(n) where n is number of mappings

#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>

// PDO direction enumeration.
enum class PdoDirection { 
  Input,   // Process data input (from slave to master)
  Output   // Process data output (from master to slave)
};

// Represents a single PDO mapping entry.
struct PdoMapping {
  QString index;                                      // PDO index in hex format
  QString subIndex;                                   // PDO subindex in hex format
  QString name;                                       // PDO name
  QString dataType;                                   // Data type (e.g., "INT16", "UINT8")
  int bitSize = 0;                                    // Bit size of the PDO
  PdoDirection direction = PdoDirection::Input;       // PDO direction
  int slavePosition = -1;                             // Slave position
  bool enabled = true;                                // Whether PDO is enabled
};

// Layout of a single PDO entry in the visual mapping.
struct PdoEntryLayout {
  QString index;
  QString subIndex;
  QString name;
  QString dataType;
  int bitSize = 0;
  PdoDirection direction = PdoDirection::Input;
  bool enabled = true;
};

// Layout of a Sync Manager in the visual mapping.
struct SyncManagerLayout {
  int smIndex = 0;
  PdoDirection direction = PdoDirection::Input;
  QVector<PdoEntryLayout> pdoEntries;
  int size = 0;
  bool enabled = true;
};

// Complete mapping layout for visual display.
struct MappingLayout {
  QVector<SyncManagerLayout> syncManagers;
  int totalSize = 0;
  PdoDirection direction = PdoDirection::Input;
  bool valid = true;
  QStringList errors;
};

// PDO validation result.
struct PdoValidationResult {
  bool valid = true;        // Whether PDO is valid
  QString errorMessage;     // Error message (if invalid)
  int totalBitSize = 0;     // Total bit size of all PDOs
  int maxBitSize = 0;       // Maximum allowed bit size
};

class PdoMappingService : public QObject {
  Q_OBJECT
public:
  explicit PdoMappingService(QObject *parent = nullptr);

  // Discover PDO mappings from a slave.
  // @param position  Slave position
  // @return Vector of discovered PdoMapping structures
  QVector<PdoMapping> discoverMappings(int position);

  // Configure a PDO mapping for a slave.
  // @param position  Slave position
  // @param mapping   PdoMapping structure to configure
  // @return true if configuration was successful
  bool configureMapping(int position, const PdoMapping &mapping);

  // Validate a PDO mapping.
  // @param mapping  PdoMapping structure to validate
  // @return PdoValidationResult with validation status
  PdoValidationResult validateMapping(const PdoMapping &mapping) const;

  // Export PDO mappings for a slave to a file.
  // @param position  Slave position
  // @param filePath  Output file path
  // @return true if export was successful
  bool exportMapping(int position, const QString &filePath) const;

  // Import PDO mappings for a slave from a file.
  // @param position  Slave position
  // @param filePath  Input file path
  // @return true if import was successful
  bool importMapping(int position, const QString &filePath);

  // Get current PDO mappings for a slave.
  // @param position  Slave position
  // @return Vector of current PdoMapping structures
  QVector<PdoMapping> currentMappings(int position) const;

  // Get visual mapping layout for a slave position.
  // @param position  Slave position
  // @return MappingLayout with SyncManager and PDO entry details
  MappingLayout getMappingLayout(int position) const;

  // Validate a complete mapping layout.
  // @param layout  MappingLayout to validate
  // @return PdoValidationResult with validation status
  PdoValidationResult validateMappingLayout(const MappingLayout &layout) const;

  // Apply a mapping layout to a slave position.
  // @param position  Slave position
  // @param layout    MappingLayout to apply
  // @return true if application was successful
  bool applyMappingLayout(int position, const MappingLayout &layout);

signals:
  // Emitted when a PDO mapping changes.
  // @param position  Slave position
  // @param mapping   Updated PdoMapping structure
  void mappingChanged(int position, const PdoMapping &mapping);

  // Emitted when a mapping layout changes.
  // @param position  Slave position
  // @param layout    Updated MappingLayout
  void mappingLayoutChanged(int position, const MappingLayout &layout);

  // Emitted when an error occurs.
  // @param message  Human-readable error message
  void error(const QString &message);

private:
  QHash<int, QVector<PdoMapping>> mappings_;  // Per-slave PDO mappings
};
