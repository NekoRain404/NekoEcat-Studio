#pragma once

// SettingsDialog — comprehensive modal dialog for application preferences.
//
// Organized into tabbed sections: Appearance, EtherCAT, Timing, Display,
// Notifications, and Export. Changes are applied immediately and persisted
// via QSettings.

#include <QDialog>
#include <QMap>
#include <QString>
#include <QVector>

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;
class QTableWidget;
class QTabWidget;

// ── Master profile ───────────────────────────────────────────────────
// Named IgH master selector: display name + numeric target (e.g. "0", "1").
struct MasterProfile {
    QString name = "Master 0";
    QString target = "0";
};

// ── Application settings ─────────────────────────────────────────────
// Persisted workspace preferences covering appearance, behavior, timing,
// display, notifications, and export paths.
struct AppSettings {
    // ── Appearance ────────────────────────────────────────────────
    QString theme = "Dark";
    QString language = "English";
    double scale = 1.0;

    // ── EtherCAT Masters ──────────────────────────────────────────
    QVector<MasterProfile> masters = {MasterProfile{}};
    QString activeMaster = "0";

    // ── Refresh & Timing ──────────────────────────────────────────
    int watchAutoRefreshMs = 0;        // 0=off, 250, 500, 1000, 2000
    int overviewAutoRefreshMs = 0;     // 0=off, 1000, 2000, 5000
    int sdoReadTimeoutMs = 3000;       // SDO read timeout
    int sdoWriteTimeoutMs = 5000;      // SDO write timeout
    int topologyPollIntervalMs = 0;    // 0=off, 5000, 10000, 30000

    // ── Free Run ──────────────────────────────────────────────────
    int freeRunCycleUs = 1000;         // cycle time in microseconds
    bool freeRunAutoName = true;       // auto-name entries from OD
    bool freeRunHighlightChanges = true;

    // ── Display ───────────────────────────────────────────────────
    bool showRawTabs = false;          // show Master/Slave/PDO/SDO Raw tabs
    bool showColumnGrid = false;       // show grid lines in tables
    int detailPanelWidth = 360;        // detail panel width in pixels
    int tableRowHeight = 28;           // table row height in pixels
    bool alternatingRowColors = true;
    bool compactMode = false;          // tighter spacing for small screens
    int maxHistoryEntries = 200;       // SDO history max entries

    // ── Notifications ─────────────────────────────────────────────
    bool notifyOnStateChange = true;   // notify when slave state changes
    bool notifyOnError = true;         // notify on errors
    bool notifyOnWatchDrift = false;   // notify when watch values drift
    bool soundEnabled = false;         // play sound on critical events
    int toastDurationMs = 3000;        // toast notification duration

    // ── Export & Paths ────────────────────────────────────────────
    QString defaultExportDir;          // default directory for exports
    QString esiRepositoryPath;         // ESI file repository path
    bool exportIncludeTimestamp = true; // include timestamp in exports
    bool exportIncludeMetadata = true; // include metadata in exports
    QString csvDelimiter = ",";        // CSV delimiter: "," or ";"

    // ── Custom Shortcuts ────────────────────────────────────────
    QMap<QString, QString> customShortcuts;  // actionId → key sequence string
};

// ── Settings Dialog ──────────────────────────────────────────────────
// Modal dialog for editing all workspace preferences.
// Organized into tabbed sections for clarity.
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(const AppSettings &settings, QWidget *parent = nullptr);
    AppSettings settings() const;

signals:
    // Emitted when the user selects a different theme in the combo box.
    // MainWindow connects to this for live preview before confirmation.
    void themePreviewRequested(const QString &themeName);

private:
    // ── Tab builders ──────────────────────────────────────────────
    QWidget *buildAppearanceTab(const AppSettings &s, bool zh);
    QWidget *buildEthercatTab(const AppSettings &s, bool zh);
    QWidget *buildTimingTab(const AppSettings &s, bool zh);
    QWidget *buildFreeRunTab(const AppSettings &s, bool zh);
    QWidget *buildDisplayTab(const AppSettings &s, bool zh);
    QWidget *buildNotificationTab(const AppSettings &s, bool zh);
    QWidget *buildExportTab(const AppSettings &s, bool zh);
    QWidget *buildShortcutsTab(const AppSettings &s, bool zh);

    // ── Appearance widgets ────────────────────────────────────────
    QComboBox *themeCombo_ = nullptr;
    QComboBox *languageCombo_ = nullptr;
    QDoubleSpinBox *scaleSpin_ = nullptr;

    // ── EtherCAT widgets ──────────────────────────────────────────
    QTableWidget *masterTable_ = nullptr;

    // ── Timing widgets ────────────────────────────────────────────
    QComboBox *watchRefreshCombo_ = nullptr;
    QComboBox *overviewRefreshCombo_ = nullptr;
    QSpinBox *sdoReadTimeoutSpin_ = nullptr;
    QSpinBox *sdoWriteTimeoutSpin_ = nullptr;
    QComboBox *topologyPollCombo_ = nullptr;

    // ── Free Run widgets ──────────────────────────────────────────
    QSpinBox *freeRunCycleSpin_ = nullptr;
    QCheckBox *freeRunAutoNameCheck_ = nullptr;
    QCheckBox *freeRunHighlightCheck_ = nullptr;

    // ── Display widgets ───────────────────────────────────────────
    QCheckBox *showRawTabsCheck_ = nullptr;
    QCheckBox *showGridCheck_ = nullptr;
    QSpinBox *detailWidthSpin_ = nullptr;
    QSpinBox *rowHeightSpin_ = nullptr;
    QCheckBox *alternatingRowsCheck_ = nullptr;
    QCheckBox *compactModeCheck_ = nullptr;
    QSpinBox *maxHistorySpin_ = nullptr;

    // ── Notification widgets ──────────────────────────────────────
    QCheckBox *notifyStateCheck_ = nullptr;
    QCheckBox *notifyErrorCheck_ = nullptr;
    QCheckBox *notifyDriftCheck_ = nullptr;
    QCheckBox *soundCheck_ = nullptr;
    QSpinBox *toastDurationSpin_ = nullptr;

    // ── Export widgets ────────────────────────────────────────────
    QLineEdit *exportDirEdit_ = nullptr;
    QLineEdit *esiPathEdit_ = nullptr;
    QCheckBox *exportTimestampCheck_ = nullptr;
    QCheckBox *exportMetadataCheck_ = nullptr;
    QComboBox *csvDelimiterCombo_ = nullptr;

    QTableWidget *shortcutsTable_ = nullptr;
    QTabWidget *tabWidget_ = nullptr;
};
