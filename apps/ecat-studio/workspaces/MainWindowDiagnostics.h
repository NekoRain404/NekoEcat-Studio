// MainWindowDiagnostics.h — Diagnostics method declarations (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

// ── Diagnostics Workspace ─────────────────────────────────────
void filterDiagnosticsTable();
void updateSelectedSlavePanel();
void updateSlaveEvidenceSummary();
void updateSelectedDriveSummary();
void updateDriveNextButton();

// ── Diagnostics Export ────────────────────────────────────────
void exportDiagnosticsReport();

// ── Host Health & Diagnostics ─────────────────────────────────
void updateHostHealth(const QJsonArray &checks);
void updateDiagnostics(const QString &level, const QString &source,
                       const QString &message);
void updateDiagnosticsSummary();
void styleDiagnosticsRow(int row, const QString &level);

// ── Host Diagnostics ──────────────────────────────────────────
void runHostDiagnostics();
void copySelectedHostCommand();
