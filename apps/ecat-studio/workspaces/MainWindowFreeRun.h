// MainWindowFreeRun.h — FreeRun-related method declarations (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

// ── Free Run Workspace ────────────────────────────────────────
QStringList freeRunImpactDetails() const;
void updateFreeRunEntryDetail();
void setFreeRun(bool enabled);

// ── Free Run Real-time Chart ─────────────────────────────────
void openFreeRunChart();
void addSelectedOdToFreeRunChart();

// ── Free Run Entry Table ──────────────────────────────────────
void filterFreeRunEntryTable();

// ── Free Run Telemetry ────────────────────────────────────────
void updateFreeRunTelemetry(const QJsonObject &telemetry);
void updateFreeRunEntryTable(const QList<QStringList> &rows);
