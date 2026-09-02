// MainWindowUi.h — UI construction, theming, command palette, RT test, and
//   help (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Builds and rebuilds the entire MainWindow widget tree, applies the visual
// theme and user settings, and manages the command palette overlay. Also
// provides the RT stability test page (chart, telemetry, timeline), help
// dialogs (manual, about), context menus for tables and topology, generic
// table/metric-card helpers, custom keyboard shortcuts, and online-view
// lifecycle (clear).

// ── UI Construction ───────────────────────────────────────────
void buildUi();
void rebuildUi();
void applyTheme();
void applySettings();
void updateActionAvailability();

// ── Command Palette ───────────────────────────────────────────
void showCommandPalette();

// ── RT Stability Test ──────────────────────────────────────────
QWidget* buildRtTestPage();
void updateRtTestTelemetry(const QJsonObject& telemetry);
void appendRtTestTimeline(const QJsonArray& recent, double avgUsec);
void updateRtTestActionAvailability();
QString formatDuration(double seconds) const;

// ── Help ──────────────────────────────────────────────────────
void showManual();
void showAbout();

// ── Context Menus ─────────────────────────────────────────────
void showTopologyContextMenu(const QPoint& position);
void showTableContextMenu(QTableWidget* table, const QPoint& position);
void showSdoTargetPanelContextMenu(const QPoint& position);
bool runLocalEvidenceAction(QTableWidget* table);
void copyTableToClipboard(QTableWidget* table, bool selectedOnly);

// ── Online Data Lifecycle ─────────────────────────────────────
void clearOnlineViews();

// ── Generic Helpers ───────────────────────────────────────────
void setTableRows(QTableWidget* table, const QStringList& headers, const QList<QStringList>& rows);
void setMetricCard(QLabel* label, const QString& title, const QString& value);

// ── Custom Shortcuts ──────────────────────────────────────────
void applyCustomShortcuts();
