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
| `MainWindowSdoWorkspace` | SDO inspector, target panel, trail, evidence |
| `MainWindowWatchWorkspace` | Watch list, add-to-watch, baseline |
| `MainWindowStartupSdoWorkspace` | Startup SDO management |
| `MainWindowIoVariableWorkspace` | I/O variable table, PLC handoff, export |
| `MainWindowConsistencyCommissioning` | Consistency checks, commissioning workflow |
| `MainWindowDiagnosticsTopology` | Topology baseline, diagnostics, slave panel |
| `MainWindowSessionWorkspace` | Session brief, next-best-action, badges |
| `MainWindowManual` | User manual and about dialogs |
| `MainWindowTexts` | UI text generation for detail panels |
