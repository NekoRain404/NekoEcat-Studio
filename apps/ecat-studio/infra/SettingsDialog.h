#pragma once

// SettingsDialog — modal dialog for application preferences.
//
// Allows users to configure theme (Dark/Light), language (8 languages),
// UI scale, and EtherCAT master profiles. Changes are applied immediately
// and persisted via QSettings.

// Application settings dialog: theme, master target, refresh interval.


#include <QDialog>
#include <QString>
#include <QVector>

class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QTableWidget;

struct MasterProfile {
    // Named IgH master selector: display name + numeric target (e.g. "0", "1").
    QString name = "Master 0";
    QString target = "0";
};

// Persisted workspace preferences — theme, language, UI scale, and master list.
struct AppSettings {
    QString theme = "Dark";
    QString language = "English";
    double scale = 1.0;
    QVector<MasterProfile> masters = {MasterProfile{}};
    QString activeMaster = "0";
};

// Modal dialog for editing workspace preferences.
// Constructed with current settings; call settings() after accept() to retrieve changes.
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(const AppSettings &settings, QWidget *parent = nullptr);
    AppSettings settings() const;

private:
    QComboBox *themeCombo_ = nullptr;
    QComboBox *languageCombo_ = nullptr;
    QDoubleSpinBox *scaleSpin_ = nullptr;
    QTableWidget *masterTable_ = nullptr;
};
