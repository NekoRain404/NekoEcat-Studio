// MainWindowWatch.h — Watch workspace (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Manages the live SDO polling (Watch) list. Methods handle adding entries
// from various sources (SDO dictionary, bookmarks, PDO map, history), bulk
// read operations, value decoding, baseline snapshot for change detection,
// delta tracking against startup SDOs, auto-refresh scheduling, CIA 402
// watch presets, table filtering, and the watch detail panel.

// ── Watch Workspace ───────────────────────────────────────────
void ensureWatchTable();
QString decodeWatchValue(const QString &index, const QString &subIndex,
                         const QString &type, const QString &value,
                         const QString &mode) const;
void addCurrentSdoToWatch(bool requestRead = true);
void addSelectedDictionaryRowsToWatch();
void addVisibleDictionaryRowsToWatch();
void addDictionaryRowsToWatch(const QVector<int> &rows,
                              const QString &sourceLabel);
void addSelectedPdoEntriesToWatch();
void addSelectedHistoryRowsToWatch();
void readSelectedDictionaryRows();
void readVisibleDictionaryRows();
void readFailedDictionaryRows();
void readDictionaryRows(const QVector<int> &rows, const QString &sourceLabel,
                        bool confirmLargeBatch);
void addCia402WatchPreset();
void refreshWatchList(bool quiet = false);
void captureWatchBaseline();
void clearWatchBaseline();
void updateWatchBaselineDelta(int row);
void updateWatchBaselineDeltas();
void updateWatchStartupDelta(int row);
void updateWatchStartupDeltas();
void updateWatchAutoRefresh();
void clearWatchList();

// ── Watch Table ───────────────────────────────────────────────
void filterWatchTable();
void updateWatchRowDetail();

// ── Watch Status Helpers ──────────────────────────────────────
bool watchRowHasValue(int row) const;
bool selectedWatchRowsHaveValue() const;
