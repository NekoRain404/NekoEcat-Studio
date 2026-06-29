// MainWindowPdoMap.h — PDO mapping workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Handles the Process Data Object (PDO) mapping table display. Provides
// filtering across PDO entries and updating the detail panel when the user
// selects a row. The table shows mapped RxPdo/TxPdo entries with their
// index, sub-index, name, and byte offset for the current slave.

// ── PDO Map Workspace ─────────────────────────────────────────
void filterPdoTable();
void updatePdoRowDetail();
