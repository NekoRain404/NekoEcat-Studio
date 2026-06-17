# workspaces/ — MainWindow Partial Implementations

Each file implements a group of `MainWindow` methods for a specific
workspace or concern. They all include `MainWindow.h` and access
private members directly (Qt partial-class pattern).

## Files

| File | Scope |
|------|-------|
| `MainWindowUiBuild` | `buildUi()` + `rebuildUi()` — full UI construction |
| `MainWindowTheme` | Light/dark QSS theme application |
| `MainWindowCommandPalette` | Command palette dialog |
| `MainWindowContextMenus` | Topology, table, SDO target context menus |
| `MainWindowProjectIo` | Project file I/O, ESI repository |
| `MainWindowSdoWorkspace` | SDO inspector helpers, evidence lookup |
| `MainWindowSdoTargetPanel` | SDO target panel, inspector, evidence digest |
| `MainWindowSdoBookmarks` | Object bookmark CRUD, cross-workspace export |
| `MainWindowSdoWrite` | SDO write operations, evidence trail, history |
| `MainWindowSdoSelection` | Cross-workspace SDO selection (OD, PDO, FreeRun, Watch) |
| `MainWindowWatchWorkspace` | Watch list management, add-to-watch, baseline |
| `MainWindowWatchSync` | Watch→Startup SDO sync operations |
| `MainWindowStartupSdoWorkspace` | Startup SDO management |
| `MainWindowIoVariableWorkspace` | I/O variable helpers, PLC handoff, export |
| `MainWindowIoVariableTable` | I/O variable table rebuild, filtering, detail |
| `MainWindowConsistencyCommissioning` | Consistency check view |
| `MainWindowCommissioning` | Commissioning workflow steps |
| `MainWindowSlaveEvidence` | Slave evidence matrix |
| `MainWindowDiagnosticsTopology` | Topology baseline, diagnostics, slave panel |
| `MainWindowSessionWorkspace` | Session brief, next-best-action, badges |
| `MainWindowStateMachine` | CiA 402 state machine display |
| `MainWindowTopologyUi` | Topology tree rendering |
| `MainWindowRtTestWorkspace` | Real-time test workspace |
| `MainWindowManual` | User manual and about dialogs |
| `MainWindowTexts` | UI text generation for detail panels |
