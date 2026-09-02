// MainWindowFreeRun.h — Free-run monitoring workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Manages the free-run mode where the EtherCAT master continuously reads
// selected OD entries in real time. Covers enabling/disabling free-run,
// updating the live entry table with telemetry data, filtering entries,
// charting selected objects, and computing impact details for active reads.

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
void updateFreeRunTelemetry(const QJsonObject& telemetry);
void updateFreeRunEntryTable(const QList<QStringList>& rows);
