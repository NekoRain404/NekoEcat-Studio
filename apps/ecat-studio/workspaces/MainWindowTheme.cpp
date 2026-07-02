// MainWindowTheme.cpp — Theme application for NekoEcat Studio.
//
// Theme QSS strings are now stored in external .qss resource files
// under themes/ and loaded at runtime by ThemeManager.
// See ThemeManager.h for the public API.

#include "MainWindowIncludes.h"
#include "themes/ThemeManager.h"
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"

#include <QApplication>
#include <QFont>
#include <QTranslator>

/* ── Theme application ───────────────────────────────────────────────
   Delegates to ThemeManager which loads .qss from Qt resources. */
void MainWindow::applyTheme()
{
    ThemeManager::applyTheme(this, settings_.theme);
}

/* ── Apply all user settings ─────────────────────────────────────────
   Theme, font scale, language, master selector, action availability, status bar. */
void MainWindow::applySettings()
{
    applyTheme();
    QFont font = qApp->font();
    font.setPointSizeF(10.0 * settings_.scale);
    qApp->setFont(font);
    applyLanguage(settings_.language);
    refreshMasterSelector();
    updateActionAvailability();
    updateStatusBar();
}

/* ── Dynamic language switching ─────────────────────────────────────
   Unloads the old .qm, loads the new one, and triggers a full workspace
   rebuild so every plugin picks up the new tr() translations. */
void MainWindow::applyLanguage(const QString &localeCode)
{
    // Map locale code to Language enum
    Language lang = LanguageManager::instance().fromLocaleCode(localeCode);

    // Remove old translator
    if (translator_) {
        qApp->removeTranslator(translator_);
        delete translator_;
        translator_ = nullptr;
    }

    // Load new .qm if not English
    if (lang != Language::English) {
        translator_ = new QTranslator(this);
        QString qmFile = QStringLiteral("nekoecat_%1").arg(localeCode);
        if (translator_->load(qmFile, QStringLiteral(":/translations"))) {
            qApp->installTranslator(translator_);
        } else {
            delete translator_;
            translator_ = nullptr;
        }
    }

    // Notify LanguageManager (this triggers languageChanged signal)
    LanguageManager::instance().setCurrentLanguage(lang);

    // Qt posts QEvent::LanguageChange to all top-level widgets when a
    // new translator is installed.  Plugins that override changeEvent()
    // will retranslate automatically.  We also refresh the workspace
    // boundary and tab badges for immediate effect.
    updateWorkspaceBoundary();
    updateTabBadges();
}

/* ── Rebuild the workbench ──────────────────────────────────────────
   Destroys and recreates the workspace tab pages so every plugin
   widget picks up the new tr() translations.  Called on language
   switch and when the plugin registry changes. */
void MainWindow::rebuildWorkbench()
{
    // Clear existing workspace pages
    int count = tabs_->count();
    for (int i = count - 1; i >= 0; --i) {
        QWidget *page = tabs_->widget(i);
        tabs_->removeTab(i);
        page->deleteLater();
    }

    // Re-populate from plugin registry
    const auto plugins = pluginRegistry_->visiblePlugins();
    for (auto *plugin : plugins) {
        QWidget *w = plugin->widget();
        if (w) {
            tabs_->addTab(w, plugin->icon(), plugin->displayName());
        }
    }
    updateWorkspaceBoundary();
    updateTabBadges();
}
