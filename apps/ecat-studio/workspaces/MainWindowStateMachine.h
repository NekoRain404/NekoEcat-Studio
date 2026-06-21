// MainWindowStateMachine.h — State machine method declarations (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

// ── State Machine ─────────────────────────────────────────────
QStringList stateTransitionImpactDetails(int position,
                                         const QString &requestedState) const;
QString recommendedEthercatState(const SlaveInfo &slave) const;
void updateStateMachineView();
void updateStateMachineRowDetail();

// ── Slave Operations ──────────────────────────────────────────
void requestSlaveStateWithConfirmation(int position, const QString &state);
void requestAllSlaveState(const QString &state);
