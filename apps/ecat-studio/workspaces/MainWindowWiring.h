// MainWindowWiring.h — Signal wiring and embedded daemon lifecycle (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Connects all Qt signals and slots across MainWindow's UI components.
// Organized into focused wiring functions per workspace area (menu actions,
// topology/state buttons, SDO inspector, startup SDOs, watch/bookmarks,
// session/workflow, state machine, host diagnostics, I/O variables,
// consistency, diagnostics). Also starts the embedded EtherCAT daemon and
// provides the generic requestRefresh entry point.

// ── Signal Wiring & Daemon ────────────────────────────────────
void wire();
QAction* findAction(const char* name) const;
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
