// MainWindowDiagnostics.h — Diagnostics and host health workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Handles diagnostics event logging, host health monitoring, and diagnostic
// report export. Methods update the diagnostics table, display per-slave
// evidence summaries and drive status, style rows by severity, and provide
// host-level diagnostic command execution and clipboard export.

// ── Diagnostics Workspace ─────────────────────────────────────
void filterDiagnosticsTable();
void updateSelectedSlavePanel();
void updateSlaveEvidenceSummary();
void updateSelectedDriveSummary();
void updateDriveNextButton();

// ── Diagnostics Export ────────────────────────────────────────
void exportDiagnosticsReport();

// ── Host Health & Diagnostics ─────────────────────────────────
void updateHostHealth(const QJsonArray& checks);
void updateDiagnostics(const QString& level, const QString& source, const QString& message);
void updateDiagnosticsSummary();
void styleDiagnosticsRow(int row, const QString& level);

// ── Host Diagnostics ──────────────────────────────────────────
void runHostDiagnostics();
void copySelectedHostCommand();
