#pragma once

#include <QDialog>
#include <QString>
#include <QVector>

class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QTableWidget;

struct MasterProfile {
    QString name = "Master 0";
    QString target = "0";
};

struct AppSettings {
    QString theme = "Dark";
    QString language = "English";
    double scale = 1.0;
    QVector<MasterProfile> masters = {MasterProfile{}};
    QString activeMaster = "0";
};

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
