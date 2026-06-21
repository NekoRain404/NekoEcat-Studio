// MainWindowConsistency.h — Consistency method declarations (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

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
