// MainWindowTopology.h — Topology-related method declarations (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

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
