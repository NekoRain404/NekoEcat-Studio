// MainWindowConsistency.h — Consistency checking workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Evaluates cross-referenced consistency across SDO values, I/O variables,
// topology, and evidence data. Provides a tabular view of consistency issues
// (errors, warnings, infos) with filtering, detail display, and gate checks
// that block commissioning operations until issues are resolved.

// ── Consistency Workspace ─────────────────────────────────────
void updateConsistencyView();
void filterConsistencyTable();
void updateConsistencyRowDetail();
void openConsistencyView();
void focusEvidenceFromConsistency(int row = -1);
void focusIoVariablesFromConsistency(int row = -1);
int firstConsistencyBlockingIssueRow() const;
int firstConsistencyIoIssueRow() const;
bool consistencyCheckAvailable() const;
void consistencyIssueCounts(int *errors, int *warnings, int *infos,
                            int *ready) const;
QStringList consistencyGateDetails(const QString &operation) const;
