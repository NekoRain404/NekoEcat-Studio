// MainWindowUi.h — UI construction, theme, command palette, RT test, help (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.

// ── UI Construction ───────────────────────────────────────────
void buildUi();
void rebuildUi();
void applyTheme();
void applySettings();
void updateActionAvailability();

// ── Command Palette ───────────────────────────────────────────
void showCommandPalette();

// ── RT Stability Test ──────────────────────────────────────────
QWidget *buildRtTestPage();
void updateRtTestTelemetry(const QJsonObject &telemetry);
void appendRtTestTimeline(const QJsonArray &recent, double avgUsec);
void updateRtTestActionAvailability();
QString formatDuration(double seconds) const;

// ── Help ──────────────────────────────────────────────────────
void showManual();
void showAbout();

// ── Context Menus ─────────────────────────────────────────────
void showTopologyContextMenu(const QPoint &position);
void showTableContextMenu(QTableWidget *table, const QPoint &position);
void showSdoTargetPanelContextMenu(const QPoint &position);
bool runLocalEvidenceAction(QTableWidget *table);
void copyTableToClipboard(QTableWidget *table, bool selectedOnly);

// ── Online Data Lifecycle ─────────────────────────────────────
void clearOnlineViews();

// ── Generic Helpers ───────────────────────────────────────────
void setTableRows(QTableWidget *table, const QStringList &headers,
                  const QList<QStringList> &rows);
void setMetricCard(QLabel *label, const QString &title, const QString &value);

// ── Custom Shortcuts ──────────────────────────────────────────
void applyCustomShortcuts();
