// MainWindowTheme.cpp — Theme application for NekoEcat Studio.
//
// Theme QSS strings are now stored in external .qss resource files
// under themes/ and loaded at runtime by ThemeManager.
// See ThemeManager.h for the public API.

#include "MainWindowIncludes.h"
#include "themes/ThemeManager.h"

#include <QFont>

/* ── Theme application ───────────────────────────────────────────────
   Delegates to ThemeManager which loads .qss from Qt resources. */
void MainWindow::applyTheme()
{
    ThemeManager::applyTheme(this, settings_.theme);
}

/* ── Apply all user settings ─────────────────────────────────────────
   Theme, font scale, master selector, action availability, status bar. */
void MainWindow::applySettings()
{
    LanguageManager::instance().setCurrentLanguage(settings_.language);
    applyTheme();
    QFont font = qApp->font();
    font.setPointSizeF(10.0 * settings_.scale);
    qApp->setFont(font);
    refreshMasterSelector();
    updateActionAvailability();
    updateStatusBar();
}
