#pragma once

// PdoConfigurationService — Professional PDO configuration management for OP state.
//
// This service manages the complete PDO configuration pipeline required to
// transition EtherCAT slaves into Operational (OP) state. It handles:
//   - PDO mapping configuration (index, subIndex, bitSize, dataType)
//   - PDO assignment configuration (mapping PDOs to sync managers)
//   - Sync manager configuration (direction, watchdog, enable)
//   - DC sync configuration (assign activate, cycle times)
//   - Applying the complete configuration to a slave position
//
// The configuration order matters for OP state entry:
//   1. Configure PDO mappings (CoE objects 0x1600/0x1A00)
//   2. Assign PDOs to sync managers (CoE objects 0x1C12/0x1C13)
//   3. Configure sync managers (physical SM registers)
//   4. Configure DC sync (0x1C32/0x1C33 or register 0x0920+)
//
// Usage:
//   ServiceContainer *container = ...;
//   PdoConfigurationService *pdoConfig = container->pdoConfiguration();
//
//   PdoMappingConfig mapping;
//   mapping.index = "0x6000";
//   mapping.subIndex = "0x01";
//   mapping.name = "ActualPosition";
//   mapping.bitSize = 32;
//   mapping.dataType = "INT32";
//   mapping.direction = PdoConfigDirection::Input;
//   pdoConfig->configurePdoMapping(0, mapping);
//
//   PdoAssignmentConfig assignment;
//   assignment.smIndex = 3;
//   assignment.pdoIndices = {"0x6000"};
//   pdoConfig->configurePdoAssignment(0, assignment);
//
//   pdoConfig->applyConfiguration(0);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread.

#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>
#include <QDateTime>

enum class PdoConfigDirection { Input, Output };

struct PdoMappingConfig {
  QString index;
  QString subIndex;
  QString name;
  int bitSize = 0;
  QString dataType;
  PdoConfigDirection direction = PdoConfigDirection::Input;
};

struct PdoAssignmentConfig {
  int smIndex = 0;
  QVector<QString> pdoIndices;
  bool enabled = true;
};

struct PdoSyncManagerConfig {
  int smIndex = 0;
  PdoConfigDirection direction = PdoConfigDirection::Input;
  int startAddress = 0;
  int length = 0;
  bool enable = true;
  bool watchdog = false;
};

struct DcSyncConfig {
  bool assignActivate = false;
  int sync0CycleTime = 0;
  int sync1CycleTime = 0;
  int sync0ShiftTime = 0;
  int sync1ShiftTime = 0;
};

struct PdoConfigurationStatus {
  int position = -1;
  bool mappingConfigured = false;
  bool assignmentConfigured = false;
  bool syncManagerConfigured = false;
  bool dcSyncConfigured = false;
  QDateTime lastApplied;
  QString lastError;
};

class PdoConfigurationService : public QObject {
  Q_OBJECT
public:
  explicit PdoConfigurationService(QObject *parent = nullptr);

  bool configurePdoMapping(int position, const PdoMappingConfig &config);
  bool configurePdoAssignment(int position, const PdoAssignmentConfig &config);
  bool configureSyncManager(int position, const PdoSyncManagerConfig &config);
  bool configureDcSync(int position, const DcSyncConfig &config);
  bool applyConfiguration(int position);

  QVector<PdoMappingConfig> pdoMappings(int position) const;
  QVector<PdoAssignmentConfig> pdoAssignments(int position) const;
  QVector<PdoSyncManagerConfig> syncManagers(int position) const;
  DcSyncConfig dcSyncConfig(int position) const;
  PdoConfigurationStatus configurationStatus(int position) const;

signals:
  void configurationApplied(int position);
  void configurationError(int position, const QString &message);

private:
  struct SlaveConfig {
    QVector<PdoMappingConfig> mappings;
    QVector<PdoAssignmentConfig> assignments;
    QVector<PdoSyncManagerConfig> smConfigs;
    DcSyncConfig dcSync;
    PdoConfigurationStatus status;
  };

  SlaveConfig &ensureSlave(int position);
  bool validateMapping(const PdoMappingConfig &config, QString &error) const;
  bool validateAssignment(const PdoAssignmentConfig &config, QString &error) const;

  QHash<int, SlaveConfig> configs_;
};
