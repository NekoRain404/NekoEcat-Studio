// MainWindowTopology.h — Topology and data updates (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Manages EtherCAT network topology data: capturing and clearing topology
// baselines for change detection, computing baseline issue summaries, and
// processing live topology updates. Also serves as the central data-ingest
// point that propagates slave list, master summary, slave info text, and
// PDO table updates from the EtherCAT master into the MainWindow model
// layer.

// ── Topology Baseline ─────────────────────────────────────────
void captureTopologyBaseline();
void clearTopologyBaseline();

// ── Topology Summary ──────────────────────────────────────────
void updateTopologyBaselineSummary();
QStringList topologyBaselineIssues() const;

// ── Topology & Data Updates ───────────────────────────────────
void reportTopologyChanges(const QVector<SlaveInfo> &previous,
                           const QVector<SlaveInfo> &current);
void updateSlaves(const QVector<SlaveInfo> &slaves);
void updateMasterSummary(const QString &text);
void updateSlaveInfo(const QString &text);
void updatePdoTable(const QString &text);
