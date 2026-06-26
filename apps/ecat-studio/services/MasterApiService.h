#pragma once

// MasterApiService — low-level EtherCAT master API boundary.
//
// Provides master creation, activation, deactivation, and state monitoring.
// Until a real ecrt-backed lifecycle path is connected, lifecycle operations
// fail closed instead of synthesizing local success.
//
// This service provides low-level EtherCAT master API access. It handles:
//   - Master creation and initialization boundary
//   - Master activation and deactivation boundary
//   - Master state monitoring
//   - Slave configuration retrieval
//   - Direct ecrt API integration boundary
//
// Usage:
//   ServiceContainer *container = ...;
//   MasterApiService *masterApi = container->masterApi();
//   masterApi->createMaster();
//   masterApi->activateMaster();
//   MasterApiState state = masterApi->masterState();
//   SlaveApiConfig config = masterApi->slaveConfig(0);
//   masterApi->deactivateMaster();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - Master creation is O(1)
//   - Master activation is O(1)
//   - State monitoring is O(1)
//   - Slave config retrieval is O(1) per slave

#include <QObject>
#include <QString>

class EcatClient;

// Master API state structure.
struct MasterApiState {
  int slavesResponding = 0;  // Number of responding slaves
  int alStates = 0;          // Application layer states
  bool linkUp = false;       // Whether link is up
};

// Slave API configuration structure.
struct SlaveApiConfig {
  int position = -1;         // Slave position
  uint16_t alias = 0;        // Slave alias
  uint32_t vendorId = 0;     // Vendor ID
  uint32_t productCode = 0;  // Product code
  bool valid = false;        // Whether config is valid
};

class MasterApiService : public QObject {
  Q_OBJECT
public:
  explicit MasterApiService(EcatClient *client, QObject *parent = nullptr);

  // Create the EtherCAT master.
  // @return true if master was created successfully
  bool createMaster();

  // Activate the EtherCAT master.
  // @return true if master was activated successfully
  bool activateMaster();

  // Deactivate the EtherCAT master.
  // @return true if master was deactivated successfully
  bool deactivateMaster();

  // Get the current master state.
  // @return MasterApiState structure
  MasterApiState masterState() const;

  // Get slave configuration for a specific position.
  // @param position  Slave position
  // @return SlaveApiConfig structure
  SlaveApiConfig slaveConfig(int position) const;

signals:
  // Emitted when master is created.
  void masterCreated();

  // Emitted when master is activated.
  void masterActivated();

  // Emitted when master is deactivated.
  void masterDeactivated();

  // Emitted when an error occurs.
  // @param message  Human-readable error message
  void error(const QString &message);

private:
  bool backendReady() const;

  EcatClient *client_;        // TCP client to ecatd daemon
  bool created_ = false;      // Whether master is created
  bool active_ = false;       // Whether master is active
  MasterApiState state_;      // Current master state
};
