// MainWindowStateMachine.h — EtherCAT state machine workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Displays per-slave EtherCAT state machine recommendations and allows the
// user to request state transitions. Methods compute impact details for a
// requested state change, recommend the next state based on slave capabilities
// (e.g., CIA 402 drives), update the state machine table and detail panel,
// and issue single-slave or broadcast state-change requests with confirmation
// dialogs.

// ── State Machine ─────────────────────────────────────────────
QStringList stateTransitionImpactDetails(int position, const QString& requestedState) const;
QString recommendedEthercatState(const SlaveInfo& slave) const;
void updateStateMachineView();
void updateStateMachineRowDetail();

// ── Slave Operations ──────────────────────────────────────────
void requestSlaveStateWithConfirmation(int position, const QString& state);
void requestAllSlaveState(const QString& state);
