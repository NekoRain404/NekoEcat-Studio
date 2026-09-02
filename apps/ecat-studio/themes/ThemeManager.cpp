#include "ThemeManager.h"
#include <QApplication>
#include <QFile>
#include <QWidget>

QStringList ThemeManager::availableThemes() {
    return {"Dark",    "Light",       "Nord",     "Catppuccin", "Dracula",  "Solarized",
            "Gruvbox", "Tokyo Night", "One Dark", "Monokai",    "Cyberpunk"};
}

QString ThemeManager::loadTheme(const QString& name) {
    /* Map display name to lowercase filename (no spaces). */
    QString key = name.toLower().replace(" ", "");
    QString path = ":/themes/" + key + ".qss";
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(f.readAll());
}

void ThemeManager::applyTheme(QWidget* /*root*/, const QString& name) {
    QString qss = loadTheme(name);
    if (!qss.isEmpty()) {
        qApp->setStyleSheet(qss);
    } else {
        /* Fallback: load the Dark theme so the app is never unstyled. */
        QString fallback = loadTheme(QStringLiteral("Dark"));
        if (!fallback.isEmpty())
            qApp->setStyleSheet(fallback);
    }
}
