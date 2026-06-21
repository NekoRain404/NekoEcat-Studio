// MainWindowWiring.h — Signal wiring method declarations (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

// ── Signal Wiring & Daemon ────────────────────────────────────
void wire();
QAction *findAction(const char *name) const;
void wireMenuActions();
void wireTopologyAndStateButtons();
void wireSdoInspector();
void wireStartupSdoButtons();
void wireWatchAndBookmarkButtons();
void wireSessionAndWorkflow();
void wireStateMachine();
void wireHostDiagnostics();
void wireIoVariableWorkspace();
void wireConsistencyWorkspace();
void wireDiagnosticsWorkspace();
void wireClearButtonsAndEventFilters();
void wireClientSignals();
void wireTimers();
void startEmbeddedDaemon();
void requestRefresh();
