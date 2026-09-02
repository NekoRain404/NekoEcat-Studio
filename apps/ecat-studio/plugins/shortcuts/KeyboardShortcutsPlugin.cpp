#include "KeyboardShortcutsPlugin.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>
#include <QVBoxLayout>

KeyboardShortcutsPlugin::KeyboardShortcutsPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    populateDefaults();
    buildUi();
    refreshTable();
}

QString KeyboardShortcutsPlugin::id() const {
    return "shortcuts";
}
QString KeyboardShortcutsPlugin::displayName() const {
    return "Keyboard Shortcuts";
}
QString KeyboardShortcutsPlugin::displayNameZh() const {
    return QStringLiteral("键盘快捷键");
}
QIcon KeyboardShortcutsPlugin::icon() const {
    return QIcon::fromTheme("input-keyboard");
}
int KeyboardShortcutsPlugin::defaultOrder() const {
    return 170;
}
bool KeyboardShortcutsPlugin::visible() const {
    return false;
}
QWidget* KeyboardShortcutsPlugin::widget() {
    return container_;
}

void KeyboardShortcutsPlugin::activate() {
    refreshTable();
}
void KeyboardShortcutsPlugin::deactivate() {}

int KeyboardShortcutsPlugin::shortcutCount() const {
    return shortcuts_.size();
}

QString KeyboardShortcutsPlugin::shortcutKey(int index) const {
    if (index < 0 || index >= shortcuts_.size())
        return {};
    return shortcuts_[index].currentKey;
}

void KeyboardShortcutsPlugin::buildUi() {
    container_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(container_);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(10);

    auto* filterRow = new QHBoxLayout;
    filterRow->addWidget(new QLabel(tr("Search:")));
    searchEdit_ = new QLineEdit;
    searchEdit_->setPlaceholderText(tr("Filter shortcuts..."));
    filterRow->addWidget(searchEdit_);
    filterRow->addWidget(new QLabel(tr("Category:")));
    categoryFilter_ = new QComboBox;
    categoryFilter_->addItem(tr("All"));
    categoryFilter_->addItems({tr("File"), tr("Edit"), tr("View"), tr("Navigation"), tr("Tools"), tr("Help")});
    filterRow->addWidget(categoryFilter_);
    mainLayout->addLayout(filterRow);

    table_ = new QTableWidget;
    table_->setColumnCount(4);
    table_->setHorizontalHeaderLabels({tr("Action"), tr("Default"), tr("Current"), tr("Category")});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->verticalHeader()->setVisible(false);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setShowGrid(false);
    table_->setAlternatingRowColors(true);
    mainLayout->addWidget(table_, 1);

    auto* btnRow = new QHBoxLayout;
    addBtn_ = new QPushButton(tr("Add"));
    editBtn_ = new QPushButton(tr("Edit"));
    deleteBtn_ = new QPushButton(tr("Delete"));
    exportBtn_ = new QPushButton(tr("Export"));
    importBtn_ = new QPushButton(tr("Import"));
    resetBtn_ = new QPushButton(tr("Reset Defaults"));
    btnRow->addWidget(addBtn_);
    btnRow->addWidget(editBtn_);
    btnRow->addWidget(deleteBtn_);
    btnRow->addStretch();
    btnRow->addWidget(exportBtn_);
    btnRow->addWidget(importBtn_);
    btnRow->addWidget(resetBtn_);
    mainLayout->addLayout(btnRow);

    connect(searchEdit_, &QLineEdit::textChanged, this, &KeyboardShortcutsPlugin::searchShortcuts);
    connect(addBtn_, &QPushButton::clicked, this, &KeyboardShortcutsPlugin::addShortcut);
    connect(editBtn_, &QPushButton::clicked, this, &KeyboardShortcutsPlugin::editShortcut);
    connect(deleteBtn_, &QPushButton::clicked, this, &KeyboardShortcutsPlugin::deleteShortcut);
    connect(exportBtn_, &QPushButton::clicked, this, &KeyboardShortcutsPlugin::exportShortcuts);
    connect(importBtn_, &QPushButton::clicked, this, &KeyboardShortcutsPlugin::importShortcuts);
    connect(resetBtn_, &QPushButton::clicked, this, &KeyboardShortcutsPlugin::resetToDefaults);
}

void KeyboardShortcutsPlugin::populateDefaults() {
    shortcuts_ = {
        {tr("New Project"), "Ctrl+N", "Ctrl+N", tr("File")},
        {tr("Open Project"), "Ctrl+O", "Ctrl+O", tr("File")},
        {tr("Save Project"), "Ctrl+S", "Ctrl+S", tr("File")},
        {tr("Close Project"), "Ctrl+W", "Ctrl+W", tr("File")},
        {tr("Quit"), "Ctrl+Q", "Ctrl+Q", tr("File")},
        {tr("Undo"), "Ctrl+Z", "Ctrl+Z", tr("Edit")},
        {tr("Redo"), "Ctrl+Y", "Ctrl+Y", tr("Edit")},
        {tr("Cut"), "Ctrl+X", "Ctrl+X", tr("Edit")},
        {tr("Copy"), "Ctrl+C", "Ctrl+C", tr("Edit")},
        {tr("Paste"), "Ctrl+V", "Ctrl+V", tr("Edit")},
        {tr("Find"), "Ctrl+F", "Ctrl+F", tr("Edit")},
        {tr("Select All"), "Ctrl+A", "Ctrl+A", tr("Edit")},
        {tr("Refresh"), "F5", "F5", tr("View")},
        {tr("Zoom In"), "Ctrl+=", "Ctrl+=", tr("View")},
        {tr("Zoom Out"), "Ctrl+-", "Ctrl+-", tr("View")},
        {tr("Toggle Fullscreen"), "F11", "F11", tr("View")},
        {tr("Next Tab"), "Ctrl+Tab", "Ctrl+Tab", tr("Navigation")},
        {tr("Previous Tab"), "Ctrl+Shift+Tab", "Ctrl+Shift+Tab", tr("Navigation")},
        {tr("Go to Overview"), "Ctrl+1", "Ctrl+1", tr("Navigation")},
        {tr("Go to Object Dictionary"), "Ctrl+2", "Ctrl+2", tr("Navigation")},
        {tr("Go to Watch"), "Ctrl+3", "Ctrl+3", tr("Navigation")},
        {tr("Open Settings"), "Ctrl+,", "Ctrl+,", tr("Tools")},
        {tr("Command Palette"), "Ctrl+Shift+P", "Ctrl+Shift+P", tr("Tools")},
        {tr("Start/Stop Free Run"), "Ctrl+R", "Ctrl+R", tr("Tools")},
        {tr("Show Manual"), "F1", "F1", tr("Help")},
    };
}

void KeyboardShortcutsPlugin::refreshTable() {
    const QString search = searchEdit_ ? searchEdit_->text().toLower() : QString();
    const QString catFilter = categoryFilter_ ? categoryFilter_->currentText() : tr("All");

    QVector<ShortcutEntry> filtered;
    for (const auto& entry : shortcuts_) {
        if (!catFilter.isEmpty() && catFilter != tr("All") && entry.category != catFilter)
            continue;
        if (!search.isEmpty() && !entry.action.toLower().contains(search) &&
            !entry.currentKey.toLower().contains(search))
            continue;
        filtered.append(entry);
    }

    table_->setRowCount(filtered.size());
    for (int i = 0; i < filtered.size(); ++i) {
        table_->setItem(i, 0, new QTableWidgetItem(filtered[i].action));
        table_->setItem(i, 1, new QTableWidgetItem(filtered[i].defaultKey));
        table_->setItem(i, 2, new QTableWidgetItem(filtered[i].currentKey));
        table_->setItem(i, 3, new QTableWidgetItem(filtered[i].category));
    }
}

void KeyboardShortcutsPlugin::addShortcut() {
    bool ok;
    QString action =
        QInputDialog::getText(container_, tr("Add Shortcut"), tr("Action name:"), QLineEdit::Normal, "", &ok);
    if (!ok || action.isEmpty())
        return;
    QString key =
        QInputDialog::getText(container_, tr("Add Shortcut"), tr("Keyboard shortcut:"), QLineEdit::Normal, "", &ok);
    if (!ok || key.isEmpty())
        return;
    shortcuts_.append({action, key, key, tr("Tools")});
    refreshTable();
}

void KeyboardShortcutsPlugin::editShortcut() {
    int row = table_->currentRow();
    if (row < 0)
        return;
    bool ok;
    QString key = QInputDialog::getText(container_, tr("Edit Shortcut"),
                                        tr("New shortcut for \"%1\":").arg(table_->item(row, 0)->text()),
                                        QLineEdit::Normal, table_->item(row, 2)->text(), &ok);
    if (!ok || key.isEmpty())
        return;
    QString action = table_->item(row, 0)->text();
    for (auto& entry : shortcuts_) {
        if (entry.action == action) {
            entry.currentKey = key;
            break;
        }
    }
    refreshTable();
}

void KeyboardShortcutsPlugin::deleteShortcut() {
    int row = table_->currentRow();
    if (row < 0)
        return;
    QString action = table_->item(row, 0)->text();
    for (int i = 0; i < shortcuts_.size(); ++i) {
        if (shortcuts_[i].action == action) {
            shortcuts_.removeAt(i);
            break;
        }
    }
    refreshTable();
}

void KeyboardShortcutsPlugin::searchShortcuts(const QString&) {
    refreshTable();
}

void KeyboardShortcutsPlugin::exportShortcuts() {
    QString path = QFileDialog::getSaveFileName(container_, tr("Export Shortcuts"), "",
                                                tr("Shortcut Files (*.shortcuts);;All (*)"));
    if (path.isEmpty())
        return;
    QSettings file(path, QSettings::IniFormat);
    file.beginWriteArray("shortcuts");
    for (int i = 0; i < shortcuts_.size(); ++i) {
        file.setArrayIndex(i);
        file.setValue("action", shortcuts_[i].action);
        file.setValue("defaultKey", shortcuts_[i].defaultKey);
        file.setValue("currentKey", shortcuts_[i].currentKey);
        file.setValue("category", shortcuts_[i].category);
    }
    file.endArray();
    QMessageBox::information(container_, tr("Export"), tr("Shortcuts exported to %1").arg(path));
}

void KeyboardShortcutsPlugin::importShortcuts() {
    QString path = QFileDialog::getOpenFileName(container_, tr("Import Shortcuts"), "",
                                                tr("Shortcut Files (*.shortcuts);;All (*)"));
    if (path.isEmpty())
        return;
    QSettings file(path, QSettings::IniFormat);
    int size = file.beginReadArray("shortcuts");
    shortcuts_.clear();
    for (int i = 0; i < size; ++i) {
        file.setArrayIndex(i);
        shortcuts_.append({file.value("action").toString(), file.value("defaultKey").toString(),
                           file.value("currentKey").toString(), file.value("category").toString()});
    }
    file.endArray();
    refreshTable();
}

void KeyboardShortcutsPlugin::resetToDefaults() {
    populateDefaults();
    refreshTable();
}
