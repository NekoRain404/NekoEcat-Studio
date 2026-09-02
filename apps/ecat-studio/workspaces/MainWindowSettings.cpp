// MainWindowSettings.cpp — Settings, master selection, shortcuts, and recent projects.
// Extracted from MainWindow.cpp to reduce its size.

#include "MainWindowIncludes.h"

#include "infra/SettingsDialog.h"
#include "infra/TranslationRegistry.h"
#include "themes/ThemeManager.h"
#include "utils/ConfirmDialogBuilder.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>

void MainWindow::addToRecentProjects(const QString& path) {
    if (path.isEmpty())
        return;
    recentProjectPaths_.removeAll(path);
    recentProjectPaths_.prepend(path);
    while (recentProjectPaths_.size() > kMaxRecentProjects)
        recentProjectPaths_.removeLast();
    updateRecentProjectsMenu();
    saveSettings();
}

void MainWindow::updateRecentProjectsMenu() {
    if (!recentProjectsMenu_)
        return;
    recentProjectsMenu_->clear();
    recentProjectsMenu_->setEnabled(!recentProjectPaths_.isEmpty());
    for (int i = 0; i < recentProjectPaths_.size(); ++i) {
        const QString& p = recentProjectPaths_[i];
        const QString label = QString("&%1 %2").arg(i + 1).arg(QFileInfo(p).fileName());
        auto* action = recentProjectsMenu_->addAction(label);
        action->setToolTip(p);
        action->setData(p);
        connect(action, &QAction::triggered, this, [this, p]() {
            if (!readProjectFile(p)) {
                QMessageBox::warning(this, uiText("Open Project", "打开工程"),
                                     uiText("Failed to open project.", "工程打开失败。"));
            } else {
                addToRecentProjects(p);
            }
        });
    }
    if (!recentProjectPaths_.isEmpty()) {
        recentProjectsMenu_->addSeparator();
        auto* clearAction = recentProjectsMenu_->addAction(uiText("Clear Recent Projects", "清除最近工程"));
        connect(clearAction, &QAction::triggered, this, [this]() {
            recentProjectPaths_.clear();
            updateRecentProjectsMenu();
            saveSettings();
        });
    }
}

void MainWindow::applyCustomShortcuts() {
    if (settings_.customShortcuts.isEmpty())
        return;

    const QMap<QString, QString> idToObj = {
        {"newProject", "newProjectAction"},
        {"openProject", "openProjectAction"},
        {"saveProject", "saveProjectAction"},
        {"saveProjectAs", "saveProjectAsAction"},
        {"connect", "menuConnectAction"},
        {"refresh", "menuRefreshAction"},
        {"rescan", "menuRescanAction"},
        {"commandPalette", "commandPaletteAction"},
        {"settings", "settingsAction"},
        {"manual", "manualAction"},
        {"showLog", "showLogAction"},
        {"workspaceBack", "workspaceBackAction"},
        {"workspaceForward", "workspaceForwardAction"},
        {"filterFocus", "filterFocusAction"},
    };
    for (auto it = settings_.customShortcuts.constBegin(); it != settings_.customShortcuts.constEnd(); ++it) {
        const QString objName = idToObj.value(it.key());
        if (!objName.isEmpty()) {
            auto* action = findChild<QAction*>(objName);
            if (action)
                action->setShortcut(QKeySequence(it.value()));
            continue;
        }
        if (it.key().startsWith("tab") || it.key() == "nextTab" || it.key() == "prevTab") {
            int idx = -1;
            if (it.key() == "nextTab")
                idx = tabSwitchShortcuts_.size() - 2;
            else if (it.key() == "prevTab")
                idx = tabSwitchShortcuts_.size() - 1;
            else {
                bool ok = false;
                int num = it.key().mid(3).toInt(&ok);
                if (ok && num >= 1 && num <= 9)
                    idx = num - 1;
            }
            if (idx >= 0 && idx < tabSwitchShortcuts_.size()) {
                tabSwitchShortcuts_[idx]->setKey(QKeySequence(it.value()));
            }
        }
    }
}

void MainWindow::loadSettings() {
    QSettings s("NekoEcatStudio", "NekoEcatStudio");

    settings_.theme = s.value("preferences/theme", "Dark").toString();
    settings_.language = s.value("preferences/language", "English").toString();
    settings_.scale = s.value("preferences/scale", 1.0).toDouble();

    settings_.masters.clear();
    const int count = s.beginReadArray("preferences/masters");
    for (int i = 0; i < count; ++i) {
        s.setArrayIndex(i);
        MasterProfile profile;
        profile.name = s.value("name", QString("Master %1").arg(i)).toString();
        profile.target = s.value("target", QString::number(i)).toString().trimmed();
        if (!profile.target.isEmpty())
            settings_.masters.append(profile);
    }
    s.endArray();
    if (settings_.masters.isEmpty())
        settings_.masters.append(MasterProfile{});
    settings_.activeMaster = s.value("preferences/activeMaster", settings_.masters.first().target).toString().trimmed();
    if (settings_.activeMaster.isEmpty())
        settings_.activeMaster = settings_.masters.first().target;
    bool known = false;
    for (const auto& p : settings_.masters) {
        if (p.target == settings_.activeMaster) {
            known = true;
            break;
        }
    }
    if (!known)
        settings_.masters.prepend(
            MasterProfile{QString("Master %1").arg(settings_.activeMaster), settings_.activeMaster});

    settings_.watchAutoRefreshMs = s.value("timing/watchAutoRefreshMs", 0).toInt();
    settings_.overviewAutoRefreshMs = s.value("timing/overviewAutoRefreshMs", 0).toInt();
    settings_.sdoReadTimeoutMs = s.value("timing/sdoReadTimeoutMs", 3000).toInt();
    settings_.sdoWriteTimeoutMs = s.value("timing/sdoWriteTimeoutMs", 5000).toInt();
    settings_.topologyPollIntervalMs = s.value("timing/topologyPollIntervalMs", 0).toInt();

    settings_.freeRunCycleUs = s.value("freerun/cycleUs", 1000).toInt();
    settings_.freeRunAutoName = s.value("freerun/autoName", true).toBool();
    settings_.freeRunHighlightChanges = s.value("freerun/highlightChanges", true).toBool();

    settings_.showRawTabs = s.value("display/showRawTabs", false).toBool();
    settings_.showColumnGrid = s.value("display/showColumnGrid", false).toBool();
    settings_.detailPanelWidth = s.value("display/detailPanelWidth", 360).toInt();
    settings_.tableRowHeight = s.value("display/tableRowHeight", 28).toInt();
    settings_.alternatingRowColors = s.value("display/alternatingRowColors", true).toBool();
    settings_.compactMode = s.value("display/compactMode", false).toBool();
    settings_.maxHistoryEntries = s.value("display/maxHistoryEntries", 200).toInt();

    settings_.notifyOnStateChange = s.value("notifications/onStateChange", true).toBool();
    settings_.notifyOnError = s.value("notifications/onError", true).toBool();
    settings_.notifyOnWatchDrift = s.value("notifications/onWatchDrift", false).toBool();
    settings_.soundEnabled = s.value("notifications/soundEnabled", false).toBool();
    settings_.toastDurationMs = s.value("notifications/toastDurationMs", 3000).toInt();

    settings_.defaultExportDir = s.value("export/defaultDir", "").toString();
    settings_.esiRepositoryPath = s.value("export/esiPath", "").toString();
    settings_.exportIncludeTimestamp = s.value("export/includeTimestamp", true).toBool();
    settings_.exportIncludeMetadata = s.value("export/includeMetadata", true).toBool();
    settings_.csvDelimiter = s.value("export/csvDelimiter", ",").toString();

    settings_.backendMode = s.value("ethercat/backendMode", "auto").toString();

    recentProjectPaths_ = s.value("recentProjects/paths").toStringList();

    settings_.customShortcuts.clear();
    const int scCount = s.beginReadArray("shortcuts/custom");
    for (int i = 0; i < scCount; ++i) {
        s.setArrayIndex(i);
        const QString id = s.value("id").toString();
        const QString seq = s.value("sequence").toString();
        if (!id.isEmpty() && !seq.isEmpty())
            settings_.customShortcuts[id] = seq;
    }
    s.endArray();
}

void MainWindow::saveSettings() {
    QSettings s("NekoEcatStudio", "NekoEcatStudio");

    s.setValue("preferences/theme", settings_.theme);
    s.setValue("preferences/language", settings_.language);
    s.setValue("preferences/scale", settings_.scale);
    s.setValue("preferences/activeMaster", settings_.activeMaster);
    s.beginWriteArray("preferences/masters");
    for (int i = 0; i < settings_.masters.size(); ++i) {
        s.setArrayIndex(i);
        s.setValue("name", settings_.masters[i].name);
        s.setValue("target", settings_.masters[i].target);
    }
    s.endArray();

    s.setValue("timing/watchAutoRefreshMs", settings_.watchAutoRefreshMs);
    s.setValue("timing/overviewAutoRefreshMs", settings_.overviewAutoRefreshMs);
    s.setValue("timing/sdoReadTimeoutMs", settings_.sdoReadTimeoutMs);
    s.setValue("timing/sdoWriteTimeoutMs", settings_.sdoWriteTimeoutMs);
    s.setValue("timing/topologyPollIntervalMs", settings_.topologyPollIntervalMs);

    s.setValue("freerun/cycleUs", settings_.freeRunCycleUs);
    s.setValue("freerun/autoName", settings_.freeRunAutoName);
    s.setValue("freerun/highlightChanges", settings_.freeRunHighlightChanges);

    s.setValue("display/showRawTabs", settings_.showRawTabs);
    s.setValue("display/showColumnGrid", settings_.showColumnGrid);
    s.setValue("display/detailPanelWidth", settings_.detailPanelWidth);
    s.setValue("display/tableRowHeight", settings_.tableRowHeight);
    s.setValue("display/alternatingRowColors", settings_.alternatingRowColors);
    s.setValue("display/compactMode", settings_.compactMode);
    s.setValue("display/maxHistoryEntries", settings_.maxHistoryEntries);

    s.setValue("notifications/onStateChange", settings_.notifyOnStateChange);
    s.setValue("notifications/onError", settings_.notifyOnError);
    s.setValue("notifications/onWatchDrift", settings_.notifyOnWatchDrift);
    s.setValue("notifications/soundEnabled", settings_.soundEnabled);
    s.setValue("notifications/toastDurationMs", settings_.toastDurationMs);

    s.setValue("export/defaultDir", settings_.defaultExportDir);
    s.setValue("export/esiPath", settings_.esiRepositoryPath);
    s.setValue("export/includeTimestamp", settings_.exportIncludeTimestamp);
    s.setValue("export/includeMetadata", settings_.exportIncludeMetadata);
    s.setValue("export/csvDelimiter", settings_.csvDelimiter);

    s.setValue("ethercat/backendMode", settings_.backendMode);

    s.setValue("recentProjects/paths", recentProjectPaths_);

    s.remove("shortcuts/custom");
    s.beginWriteArray("shortcuts/custom");
    int scIdx = 0;
    for (auto it = settings_.customShortcuts.constBegin(); it != settings_.customShortcuts.constEnd(); ++it, ++scIdx) {
        s.setArrayIndex(scIdx);
        s.setValue("id", it.key());
        s.setValue("sequence", it.value());
    }
    s.endArray();
}

void MainWindow::openSettings() {
    const QString previousLanguage = settings_.language;
    const QString previousMaster = settings_.activeMaster;
    const QString previousTheme = settings_.theme;
    const QString previousBackendMode = settings_.backendMode;

    SettingsDialog dialog(settings_, this);

    connect(&dialog, &SettingsDialog::themePreviewRequested, this, [this](const QString& theme) {
        // Defensively reject sentinel/non-theme values so a misrouted signal can
        // never persist a bogus theme name into settings_.
        if (theme.startsWith("__") || !ThemeManager::availableThemes().contains(theme)) {
            return;
        }
        settings_.theme = theme;
        applyTheme();
    });

    auto populateAdapters = [&dialog](const QJsonObject& data) {
        QStringList entries;
        const auto adapters = data.value("adapters").toArray();
        for (const auto& value : adapters) {
            const auto a = value.toObject();
            const QString link = a.value("linkUp").toBool() ? QStringLiteral("Up") : QStringLiteral("Down");
            entries << QStringLiteral("%1|%2|%3|%4")
                           .arg(a.value("name").toString(), a.value("mac").toString(), a.value("driver").toString(),
                                link);
        }
        dialog.setAvailableAdapters(entries);
    };
    connect(&client_, &EcatClient::adaptersListResult, &dialog,
            [populateAdapters](const QJsonObject& data) { populateAdapters(data); });
    connect(&dialog, &SettingsDialog::adaptersRefreshRequested, &client_, [this] {
        if (client_.isConnected()) {
            client_.listAdapters();
        }
    });
    // Populate the adapter list once when the dialog opens.
    if (client_.isConnected()) {
        client_.listAdapters();
    }

    if (dialog.exec() != QDialog::Accepted) {
        settings_.theme = previousTheme;
        applyTheme();
        return;
    }

    settings_ = dialog.settings();
    if (settings_.activeMaster != previousMaster) {
        clearOnlineViews();
    }
    client_.setMasterTarget(settings_.activeMaster);
    if (settings_.backendMode != previousBackendMode && client_.isConnected()) {
        client_.setBackendMode(settings_.backendMode);
    }
    saveSettings();

    if (settings_.language != previousLanguage) {
        applySettings();
        rebuildUi();
        applyCustomShortcuts();
        QMessageBox::information(this, uiText("Settings", "设置"), uiText("Language was applied.", "语言已应用。"));
        return;
    }
    applySettings();
    applyCustomShortcuts();
    QMessageBox::information(this, uiText("Settings", "设置"), uiText("Settings were applied.", "设置已应用。"));
}

QString MainWindow::uiText(const QString& english, const QString& /*zh*/) const {
    const Language lang = LanguageManager::instance().currentLanguage();
    if (lang == Language::English) {
        return english;
    }
    const QString translated = TranslationRegistry::instance().translate(english, lang);
    return translated.isEmpty() ? english : translated;
}

QString MainWindow::activeMasterName() const {
    for (const auto& profile : settings_.masters) {
        if (profile.target == settings_.activeMaster) {
            return QString("%1  [%2]").arg(profile.name, profile.target);
        }
    }
    return QString("Master %1").arg(settings_.activeMaster);
}

void MainWindow::refreshMasterSelector() {
    if (!masterCombo_) {
        return;
    }
    QSignalBlocker blocker(masterCombo_);
    masterCombo_->clear();
    const QVector<MasterProfile> masters =
        settings_.masters.isEmpty() ? QVector<MasterProfile>{MasterProfile{}} : settings_.masters;
    int activeIndex = 0;
    for (int i = 0; i < masters.size(); ++i) {
        const QString label = QString("%1  [%2]").arg(masters[i].name, masters[i].target);
        masterCombo_->addItem(label, masters[i].target);
        if (masters[i].target == settings_.activeMaster) {
            activeIndex = i;
        }
    }
    masterCombo_->setCurrentIndex(activeIndex);
}

void MainWindow::setActiveMaster(const QString& target) {
    const QString next = target.trimmed().isEmpty() ? "0" : target.trimmed();
    if (next == settings_.activeMaster) {
        return;
    }
    if (freeRun_ && client_.isConnected()) {
        client_.freeRunStop();
    }
    settings_.activeMaster = next;
    bool known = false;
    for (const auto& profile : settings_.masters) {
        if (profile.target == next) {
            known = true;
            break;
        }
    }
    if (!known) {
        settings_.masters.append(MasterProfile{QString("Master %1").arg(next), next});
    }
    saveSettings();
    refreshMasterSelector();
    clearOnlineViews();
    client_.setMasterTarget(next);
    requestRefresh();
}

bool MainWindow::confirmDangerousOperation(const QString& title, const QString& summary, const QStringList& details,
                                           const QString& confirmText) {
    const bool accepted = ConfirmDialogBuilder::confirm(this, title, summary, details, confirmText, settings_.theme);
    updateDiagnostics(
        accepted ? "Warning" : "Info", "Safety",
        QString("%1: %2").arg(title, accepted ? uiText("confirmed", "已确认") : uiText("cancelled", "已取消")));
    return accepted;
}
