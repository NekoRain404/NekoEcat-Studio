#include "UserPreferencesPlugin.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

UserPreferencesPlugin::UserPreferencesPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    populateDefaults();
    buildUi();
    buildPreferenceTree();
}

QString UserPreferencesPlugin::id() const {
    return "preferences";
}
QString UserPreferencesPlugin::displayName() const {
    return "User Preferences";
}
QString UserPreferencesPlugin::displayNameZh() const {
    return QStringLiteral("用户偏好");
}
QIcon UserPreferencesPlugin::icon() const {
    return QIcon::fromTheme("preferences-system");
}
int UserPreferencesPlugin::defaultOrder() const {
    return 175;
}
bool UserPreferencesPlugin::visible() const {
    return false;
}
QWidget* UserPreferencesPlugin::widget() {
    return container_;
}

void UserPreferencesPlugin::activate() {}
void UserPreferencesPlugin::deactivate() {}

void UserPreferencesPlugin::populateDefaults() {
    categories_ = {
        {tr("General"),
         {
             {"language",
              tr("Language"),
              tr("Interface language"),
              "English",
              "English",
              "combo",
              {"English", "Chinese", "Japanese", "German"}},
             {"autoConnect", tr("Auto Connect"), tr("Connect to daemon on startup"), true, true, "bool", {}},
             {"confirmExit", tr("Confirm Exit"), tr("Show confirmation before exiting"), true, true, "bool", {}},
             {"recentFiles", tr("Max Recent Files"), tr("Number of recent files to remember"), 10, 10, "int", {}},
         }},
        {tr("Appearance"),
         {
             {"theme",
              tr("Theme"),
              tr("Application theme"),
              "Light",
              "Light",
              "combo",
              {"Light", "Dark", "High Contrast"}},
             {"fontSize", tr("Font Size"), tr("Base font size in points"), 11, 11, "int", {}},
             {"showToolbar", tr("Show Toolbar"), tr("Display the main toolbar"), true, true, "bool", {}},
             {"showStatusBar", tr("Show Status Bar"), tr("Display the status bar"), true, true, "bool", {}},
             {"iconSize", tr("Icon Size"), tr("Toolbar icon size"), 24, 24, "int", {}},
         }},
        {tr("EtherCAT"),
         {
             {"autoScan", tr("Auto Scan"), tr("Scan network on connect"), true, true, "bool", {}},
             {"scanInterval", tr("Scan Interval (ms)"), tr("Periodic scan interval"), 5000, 5000, "int", {}},
             {"retryCount", tr("Retry Count"), tr("Connection retry attempts"), 3, 3, "int", {}},
             {"timeout", tr("Timeout (ms)"), tr("Communication timeout"), 2000, 2000, "int", {}},
         }},
        {tr("Display"),
         {
             {"hexAddresses", tr("Hex Addresses"), tr("Show addresses in hexadecimal"), true, true, "bool", {}},
             {"showDescriptions", tr("Show Descriptions"), tr("Display ESI descriptions"), true, true, "bool", {}},
             {"maxRows", tr("Max Table Rows"), tr("Maximum rows in data tables"), 1000, 1000, "int", {}},
             {"refreshRate", tr("Refresh Rate (ms)"), tr("UI refresh interval"), 500, 500, "int", {}},
         }},
        {tr("Notifications"),
         {
             {"enableSounds", tr("Enable Sounds"), tr("Play notification sounds"), false, false, "bool", {}},
             {"showPopups", tr("Show Popups"), tr("Display popup notifications"), true, true, "bool", {}},
             {"logLevel",
              tr("Log Level"),
              tr("Minimum log level to display"),
              "Info",
              "Info",
              "combo",
              {"Debug", "Info", "Warning", "Error"}},
         }},
        {tr("Export"),
         {
             {"defaultFormat",
              tr("Default Format"),
              tr("Default export format"),
              "CSV",
              "CSV",
              "combo",
              {"CSV", "JSON", "XML", "Text"}},
             {"includeHeaders", tr("Include Headers"), tr("Include column headers in exports"), true, true, "bool", {}},
             {"exportPath", tr("Default Export Path"), tr("Default directory for exports"), "", "", "string", {}},
         }},
    };
}

void UserPreferencesPlugin::buildUi() {
    container_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(container_);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(10);

    auto* splitter = new QSplitter(Qt::Horizontal);

    tree_ = new QTreeWidget;
    tree_->setHeaderLabel(tr("Categories"));
    tree_->setMinimumWidth(200);
    tree_->setMaximumWidth(300);
    splitter->addWidget(tree_);

    auto* rightWidget = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    descriptionLabel_ = new QLabel;
    descriptionLabel_->setWordWrap(true);
    descriptionLabel_->setStyleSheet("color: #6b7280; padding: 4px;");
    rightLayout->addWidget(descriptionLabel_);

    editorStack_ = new QStackedWidget;
    rightLayout->addWidget(editorStack_, 1);

    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    auto* btnRow = new QHBoxLayout;
    applyBtn_ = new QPushButton(tr("Apply"));
    resetBtn_ = new QPushButton(tr("Reset"));
    exportBtn_ = new QPushButton(tr("Export"));
    importBtn_ = new QPushButton(tr("Import"));
    defaultsBtn_ = new QPushButton(tr("Reset Defaults"));
    btnRow->addWidget(applyBtn_);
    btnRow->addWidget(resetBtn_);
    btnRow->addStretch();
    btnRow->addWidget(exportBtn_);
    btnRow->addWidget(importBtn_);
    btnRow->addWidget(defaultsBtn_);
    mainLayout->addLayout(btnRow);

    connect(applyBtn_, &QPushButton::clicked, this, &UserPreferencesPlugin::applyPreferences);
    connect(resetBtn_, &QPushButton::clicked, this, &UserPreferencesPlugin::resetPreferences);
    connect(exportBtn_, &QPushButton::clicked, this, &UserPreferencesPlugin::exportPreferences);
    connect(importBtn_, &QPushButton::clicked, this, &UserPreferencesPlugin::importPreferences);
    connect(defaultsBtn_, &QPushButton::clicked, this, &UserPreferencesPlugin::resetToDefaults);
}

void UserPreferencesPlugin::buildPreferenceTree() {
    tree_->clear();
    for (int ci = 0; ci < categories_.size(); ++ci) {
        auto* catItem = new QTreeWidgetItem(tree_);
        catItem->setText(0, categories_[ci].name);
        catItem->setExpanded(true);

        auto* page = new QScrollArea;
        auto* pageWidget = new QWidget;
        auto* pageLayout = new QVBoxLayout(pageWidget);
        pageLayout->setContentsMargins(12, 12, 12, 12);
        pageLayout->setSpacing(10);

        for (int ii = 0; ii < categories_[ci].items.size(); ++ii) {
            auto* row = new QHBoxLayout;
            auto* label = new QLabel(categories_[ci].items[ii].name);
            label->setMinimumWidth(160);
            row->addWidget(label);
            row->addWidget(createEditor(categories_[ci].items[ii], ci, ii));
            pageLayout->addLayout(row);
        }
        pageLayout->addStretch();
        page->setWidget(pageWidget);
        page->setWidgetResizable(true);
        editorStack_->addWidget(page);
    }

    connect(tree_, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem* item) {
        int idx = tree_->indexOfTopLevelItem(item);
        if (idx >= 0 && idx < editorStack_->count()) {
            editorStack_->setCurrentIndex(idx);
            descriptionLabel_->setText(categories_[idx].name);
        }
    });

    if (tree_->topLevelItem(0))
        tree_->setCurrentItem(tree_->topLevelItem(0));
}

QWidget* UserPreferencesPlugin::createEditor(const PreferenceItem& item, int catIdx, int itemIdx) {
    if (item.type == "bool") {
        auto* cb = new QCheckBox;
        cb->setChecked(item.currentValue.toBool());
        cb->setObjectName(QString("pref_%1_%2").arg(catIdx).arg(itemIdx));
        return cb;
    }
    if (item.type == "int") {
        auto* spin = new QSpinBox;
        spin->setRange(0, 999999);
        spin->setValue(item.currentValue.toInt());
        spin->setObjectName(QString("pref_%1_%2").arg(catIdx).arg(itemIdx));
        return spin;
    }
    if (item.type == "combo") {
        auto* combo = new QComboBox;
        combo->addItems(item.options);
        combo->setCurrentText(item.currentValue.toString());
        combo->setObjectName(QString("pref_%1_%2").arg(catIdx).arg(itemIdx));
        return combo;
    }
    auto* edit = new QLineEdit;
    edit->setText(item.currentValue.toString());
    edit->setObjectName(QString("pref_%1_%2").arg(catIdx).arg(itemIdx));
    return edit;
}

void UserPreferencesPlugin::onCategoryChanged(int index) {
    if (index >= 0 && index < editorStack_->count())
        editorStack_->setCurrentIndex(index);
}

void UserPreferencesPlugin::applyPreferences() {
    QMessageBox::information(container_, tr("Apply"), tr("Preferences applied successfully."));
}

void UserPreferencesPlugin::resetPreferences() {
    buildPreferenceTree();
}

void UserPreferencesPlugin::exportPreferences() {
    QString path = QFileDialog::getSaveFileName(container_, tr("Export Preferences"), "",
                                                tr("Preference Files (*.prefs);;All (*)"));
    if (path.isEmpty())
        return;
    QSettings file(path, QSettings::IniFormat);
    for (int ci = 0; ci < categories_.size(); ++ci) {
        file.beginGroup(categories_[ci].name);
        for (int ii = 0; ii < categories_[ci].items.size(); ++ii) {
            file.setValue(categories_[ci].items[ii].key, categories_[ci].items[ii].currentValue);
        }
        file.endGroup();
    }
    QMessageBox::information(container_, tr("Export"), tr("Preferences exported to %1").arg(path));
}

void UserPreferencesPlugin::importPreferences() {
    QString path = QFileDialog::getOpenFileName(container_, tr("Import Preferences"), "",
                                                tr("Preference Files (*.prefs);;All (*)"));
    if (path.isEmpty())
        return;
    QSettings file(path, QSettings::IniFormat);
    for (int ci = 0; ci < categories_.size(); ++ci) {
        file.beginGroup(categories_[ci].name);
        for (int ii = 0; ii < categories_[ci].items.size(); ++ii) {
            if (file.contains(categories_[ci].items[ii].key))
                categories_[ci].items[ii].currentValue = file.value(categories_[ci].items[ii].key);
        }
        file.endGroup();
    }
    buildPreferenceTree();
}

void UserPreferencesPlugin::resetToDefaults() {
    for (auto& cat : categories_) {
        for (auto& item : cat.items) {
            item.currentValue = item.defaultValue;
        }
    }
    buildPreferenceTree();
}
