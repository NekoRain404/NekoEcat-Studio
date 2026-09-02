#pragma once
// ThemeManager — loads and applies QSS themes from resource files.
#include <QString>
#include <QStringList>

class QWidget;

class ThemeManager {
public:
    static QStringList availableThemes();
    static QString loadTheme(const QString& name);
    static void applyTheme(QWidget* root, const QString& name);
};
