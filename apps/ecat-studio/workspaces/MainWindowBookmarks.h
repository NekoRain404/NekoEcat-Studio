// MainWindowBookmarks.h — Object bookmark method declarations (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

// ── Object Bookmarks ──────────────────────────────────────────
void ensureObjectBookmarkTable();
void updateObjectBookmarkRowDetail();
QVector<int> selectedObjectBookmarkRows() const;
bool selectSlaveForLocalEvidence(int position);
bool selectObjectBookmarkSlave(int position);
void addCurrentSdoBookmark();
void addSelectedDictionaryRowsToBookmarks();
void addDictionaryRowsToBookmarks(const QVector<int> &rows,
                                  const QString &sourceLabel);
void addObjectBookmark(int position, const QString &index,
                       const QString &subIndex, const QString &access,
                       const QString &type, const QString &bits,
                       const QString &name, const QString &lastValue,
                       const QString &source);
void applySdoSelectionFromBookmark(int row, bool readAfterFill);

// ── Bookmark Actions ──────────────────────────────────────────
void addSelectedObjectBookmarksToWatch();
void addObjectBookmarkRowsToWatch(const QVector<int> &rows);
void addSelectedObjectBookmarksToStartupSdo();
void addObjectBookmarkRowsToStartupSdo(const QVector<int> &rows);
void removeSelectedObjectBookmarks();
