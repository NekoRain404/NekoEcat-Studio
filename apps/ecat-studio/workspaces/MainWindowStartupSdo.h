// MainWindowStartupSdo.h — Startup SDO workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Manages the startup SDO list: the ordered set of SDO writes that will be
// applied automatically when the EtherCAT master enters operational state.
// Methods here handle adding rows from Watch/history/dictionary, reordering,
// filtering, applying individual or batch writes, verifying written values
// against the live device, detecting mismatches with the Watch baseline, and
// preflight checks before commissioning.

// ── Startup SDO Workspace ─────────────────────────────────────
void addStartupSdoFromWatchRow(int row);
void addStartupSdoFromSelectedWatchRows();
void syncWatchRowsToStartupSdo(const QVector<int> &rows);
void syncSelectedWatchRowsToStartupSdo();
void addSelectedHistoryRowsToStartupSdo();
void addSelectedDictionaryEvidenceToStartupSdo();
void addDictionaryEvidenceRowsToStartupSdo(const QVector<int> &rows);
void ensureStartupSdoTable();

// ── Startup SDO Operations ────────────────────────────────────
void updateStartupSdoWatchEvidence();
void filterStartupSdoTable();
void updateStartupSdoRowDetail();
void focusStartupSdoWatchDiffs();
void addStartupSdo();
void removeStartupSdo();
void moveStartupSdoRow(int delta);
void applyStartupSdoList();
void applyStartupSdoRow(int row);
void applySelectedStartupSdoRows();
void applyStartupSdoRows(const QVector<int> &rows,
                         const QString &operationLabel,
                         const QString &summary, const QString &confirmText);
QVector<int> startupSdoRowsWithWatchDiffs() const;
void applyStartupSdoWatchDiffRows();
void verifyStartupSdoList();
void verifyStartupSdoRow(int row);
void verifySelectedStartupSdoRows();
void addStartupSdoRowToWatch(int row);
bool preflightStartupSdoList(bool showSuccess);
void updateStartupSdoControls();
