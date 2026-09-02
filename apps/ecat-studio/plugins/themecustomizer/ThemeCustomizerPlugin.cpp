#include "ThemeCustomizerPlugin.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>
#include <QVBoxLayout>

ThemeCustomizerPlugin::ThemeCustomizerPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
    populateColorTable();
    populateFontCombo();
}

QString ThemeCustomizerPlugin::id() const {
    return "themecustomizer";
}
QString ThemeCustomizerPlugin::displayName() const {
    return "Theme Customizer";
}
QString ThemeCustomizerPlugin::displayNameZh() const {
    return QStringLiteral("主题定制器");
}
QIcon ThemeCustomizerPlugin::icon() const {
    return QIcon::fromTheme("preferences-desktop-theme");
}
int ThemeCustomizerPlugin::defaultOrder() const {
    return 165;
}
bool ThemeCustomizerPlugin::visible() const {
    return false;
}
QWidget* ThemeCustomizerPlugin::widget() {
    return container_;
}

void ThemeCustomizerPlugin::activate() {
    updatePreview();
}
void ThemeCustomizerPlugin::deactivate() {}

void ThemeCustomizerPlugin::buildUi() {
    container_ = new QWidget;
    auto* mainLayout = new QHBoxLayout(container_);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    auto* leftPanel = new QVBoxLayout;
    leftPanel->setSpacing(10);

    auto* nameRow = new QHBoxLayout;
    nameRow->addWidget(new QLabel(tr("Theme Name:")));
    themeNameEdit_ = new QLineEdit;
    themeNameEdit_->setPlaceholderText(tr("Custom Theme"));
    nameRow->addWidget(themeNameEdit_);
    leftPanel->addLayout(nameRow);

    auto* selectorRow = new QHBoxLayout;
    selectorRow->addWidget(new QLabel(tr("Base Theme:")));
    themeSelector_ = new QComboBox;
    themeSelector_->addItems({"Light", "Dark", "High Contrast", "Monokai", "Solarized"});
    selectorRow->addWidget(themeSelector_);
    leftPanel->addLayout(selectorRow);

    auto* colorGroup = new QGroupBox(tr("Theme Colors"));
    auto* colorLayout = new QVBoxLayout(colorGroup);
    colorTable_ = new QTableWidget;
    colorTable_->setColumnCount(3);
    colorTable_->setHorizontalHeaderLabels({tr("Element"), tr("Color"), tr("Pick")});
    colorTable_->horizontalHeader()->setStretchLastSection(true);
    colorTable_->verticalHeader()->setVisible(false);
    colorTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    colorTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    colorLayout->addWidget(colorTable_);
    leftPanel->addWidget(colorGroup, 1);

    auto* fontGroup = new QGroupBox(tr("Fonts"));
    auto* fontLayout = new QHBoxLayout(fontGroup);
    fontLayout->addWidget(new QLabel(tr("Font:")));
    fontCombo_ = new QComboBox;
    fontLayout->addWidget(fontCombo_);
    fontLayout->addWidget(new QLabel(tr("Size:")));
    fontSizeCombo_ = new QComboBox;
    fontSizeCombo_->addItems({"8", "9", "10", "11", "12", "14", "16", "18", "20"});
    fontSizeCombo_->setCurrentText("11");
    fontLayout->addWidget(fontSizeCombo_);
    leftPanel->addWidget(fontGroup);

    auto* btnRow = new QHBoxLayout;
    applyBtn_ = new QPushButton(tr("Apply Preview"));
    saveBtn_ = new QPushButton(tr("Save"));
    loadBtn_ = new QPushButton(tr("Load"));
    exportBtn_ = new QPushButton(tr("Export"));
    importBtn_ = new QPushButton(tr("Import"));
    resetBtn_ = new QPushButton(tr("Reset"));
    btnRow->addWidget(applyBtn_);
    btnRow->addWidget(saveBtn_);
    btnRow->addWidget(loadBtn_);
    btnRow->addWidget(exportBtn_);
    btnRow->addWidget(importBtn_);
    btnRow->addWidget(resetBtn_);
    leftPanel->addLayout(btnRow);

    mainLayout->addLayout(leftPanel, 2);

    auto* rightPanel = new QVBoxLayout;
    rightPanel->setSpacing(8);
    previewLabel_ = new QLabel(tr("Live Preview"));
    previewLabel_->setStyleSheet("font-weight: bold; font-size: 14px;");
    rightPanel->addWidget(previewLabel_);
    previewPane_ = new QPlainTextEdit;
    previewPane_->setReadOnly(true);
    previewPane_->setPlainText(tr("Theme Preview\n\n"
                                  "This pane shows a live preview of your theme changes.\n\n"
                                  "Sample table header:  Index | Name      | Value\n"
                                  "Sample table row:     0x1000 | Device Type | 0x00000001\n\n"
                                  "Colors, fonts, and spacing will update when you\n"
                                  "click \"Apply Preview\".\n"));
    rightPanel->addWidget(previewPane_, 1);
    mainLayout->addLayout(rightPanel, 1);

    connect(applyBtn_, &QPushButton::clicked, this, &ThemeCustomizerPlugin::applyPreview);
    connect(saveBtn_, &QPushButton::clicked, this, &ThemeCustomizerPlugin::saveTheme);
    connect(loadBtn_, &QPushButton::clicked, this, &ThemeCustomizerPlugin::loadTheme);
    connect(exportBtn_, &QPushButton::clicked, this, &ThemeCustomizerPlugin::exportTheme);
    connect(importBtn_, &QPushButton::clicked, this, &ThemeCustomizerPlugin::importTheme);
    connect(resetBtn_, &QPushButton::clicked, this, &ThemeCustomizerPlugin::resetToDefaults);
}

void ThemeCustomizerPlugin::populateColorTable() {
    const QStringList elements = {tr("Background"),      tr("Foreground"),      tr("Primary"),       tr("Secondary"),
                                  tr("Accent"),          tr("Error"),           tr("Warning"),       tr("Success"),
                                  tr("Table Header BG"), tr("Table Header FG"), tr("Table Row Alt"), tr("Selection BG"),
                                  tr("Selection FG"),    tr("Border"),          tr("Disabled Text"), tr("Link")};
    const QStringList defaults = {"#ffffff", "#1a1a2e", "#3b82f6", "#6b7280", "#8b5cf6", "#ef4444",
                                  "#f59e0b", "#22c55e", "#f3f4f6", "#111827", "#f9fafb", "#3b82f6",
                                  "#ffffff", "#d1d5db", "#9ca3af", "#2563eb"};
    colorTable_->setRowCount(elements.size());
    for (int i = 0; i < elements.size(); ++i) {
        colorTable_->setItem(i, 0, new QTableWidgetItem(elements[i]));
        auto* colorItem = new QTableWidgetItem(defaults[i]);
        colorItem->setBackground(QColor(defaults[i]));
        colorTable_->setItem(i, 1, colorItem);
        auto* pickBtn = new QPushButton(tr("Pick"));
        colorTable_->setCellWidget(i, 2, pickBtn);
    }
}

void ThemeCustomizerPlugin::populateFontCombo() {
    fontCombo_->addItems(
        {"Segoe UI", "Arial", "Helvetica", "Consolas", "Courier New", "Ubuntu", "Noto Sans", "Source Sans Pro"});
    fontCombo_->setCurrentText("Segoe UI");
}

void ThemeCustomizerPlugin::updatePreview() {
    QString preview;
    preview += tr("Theme: %1\n")
                   .arg(themeNameEdit_->text().isEmpty() ? themeSelector_->currentText() : themeNameEdit_->text());
    preview += tr("Font: %1 %2pt\n\n").arg(fontCombo_->currentText(), fontSizeCombo_->currentText());
    preview += tr("Color palette:\n");
    for (int i = 0; i < colorTable_->rowCount(); ++i) {
        if (colorTable_->item(i, 0) && colorTable_->item(i, 1))
            preview += QString("  %1: %2\n").arg(colorTable_->item(i, 0)->text(), colorTable_->item(i, 1)->text());
    }
    previewPane_->setPlainText(preview);
}

void ThemeCustomizerPlugin::applyPreview() {
    updatePreview();
}

void ThemeCustomizerPlugin::saveTheme() {
    QSettings settings("NekoEcatStudio", "ThemeCustomizer");
    settings.setValue("themeName", themeNameEdit_->text());
    settings.setValue("baseTheme", themeSelector_->currentIndex());
    settings.setValue("font", fontCombo_->currentText());
    settings.setValue("fontSize", fontSizeCombo_->currentText());
    QStringList colors;
    for (int i = 0; i < colorTable_->rowCount(); ++i) {
        if (colorTable_->item(i, 1))
            colors << colorTable_->item(i, 1)->text();
    }
    settings.setValue("colors", colors);
    QMessageBox::information(container_, tr("Save"), tr("Theme saved successfully."));
}

void ThemeCustomizerPlugin::loadTheme() {
    QSettings settings("NekoEcatStudio", "ThemeCustomizer");
    themeNameEdit_->setText(settings.value("themeName").toString());
    themeSelector_->setCurrentIndex(settings.value("baseTheme", 0).toInt());
    fontCombo_->setCurrentText(settings.value("font", "Segoe UI").toString());
    fontSizeCombo_->setCurrentText(settings.value("fontSize", "11").toString());
    QStringList colors = settings.value("colors").toStringList();
    for (int i = 0; i < qMin(colors.size(), colorTable_->rowCount()); ++i) {
        if (colorTable_->item(i, 1)) {
            colorTable_->item(i, 1)->setText(colors[i]);
            colorTable_->item(i, 1)->setBackground(QColor(colors[i]));
        }
    }
    updatePreview();
}

void ThemeCustomizerPlugin::exportTheme() {
    QString path =
        QFileDialog::getSaveFileName(container_, tr("Export Theme"), "", tr("Theme Files (*.theme);;All (*)"));
    if (path.isEmpty())
        return;
    QSettings file(path, QSettings::IniFormat);
    file.setValue("themeName", themeNameEdit_->text());
    file.setValue("baseTheme", themeSelector_->currentIndex());
    file.setValue("font", fontCombo_->currentText());
    file.setValue("fontSize", fontSizeCombo_->currentText());
    QStringList colors;
    for (int i = 0; i < colorTable_->rowCount(); ++i) {
        if (colorTable_->item(i, 1))
            colors << colorTable_->item(i, 1)->text();
    }
    file.setValue("colors", colors);
    QMessageBox::information(container_, tr("Export"), tr("Theme exported to %1").arg(path));
}

void ThemeCustomizerPlugin::importTheme() {
    QString path =
        QFileDialog::getOpenFileName(container_, tr("Import Theme"), "", tr("Theme Files (*.theme);;All (*)"));
    if (path.isEmpty())
        return;
    QSettings file(path, QSettings::IniFormat);
    themeNameEdit_->setText(file.value("themeName").toString());
    themeSelector_->setCurrentIndex(file.value("baseTheme", 0).toInt());
    fontCombo_->setCurrentText(file.value("font", "Segoe UI").toString());
    fontSizeCombo_->setCurrentText(file.value("fontSize", "11").toString());
    QStringList colors = file.value("colors").toStringList();
    for (int i = 0; i < qMin(colors.size(), colorTable_->rowCount()); ++i) {
        if (colorTable_->item(i, 1)) {
            colorTable_->item(i, 1)->setText(colors[i]);
            colorTable_->item(i, 1)->setBackground(QColor(colors[i]));
        }
    }
    updatePreview();
}

void ThemeCustomizerPlugin::resetToDefaults() {
    themeNameEdit_->clear();
    themeSelector_->setCurrentIndex(0);
    fontCombo_->setCurrentText("Segoe UI");
    fontSizeCombo_->setCurrentText("11");
    populateColorTable();
    updatePreview();
}
