// Application settings dialog: theme, master target, refresh interval.
#include "LanguageManager.h"
#include "SettingsDialog.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

// Factory for section headers used to visually group dialog areas.
QLabel *makeDialogSection(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName("sectionTitle");
    return label;
}

}

// Build the settings form: appearance (theme/language/scale) and a master profile table.
// The master table lets users name IgH masters and assign numeric selectors.
SettingsDialog::SettingsDialog(const AppSettings &settings, QWidget *parent)
    : QDialog(parent)
{
    const bool zh = settings.language == "简体中文";
    setObjectName("settingsDialog");
    setWindowTitle(zh ? "设置" : "Settings");
    setModal(true);
    resize(680, 520);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 18);
    layout->setSpacing(14);

    auto *title = new QLabel(zh ? "工作站设置" : "Workspace Settings");
    title->setObjectName("dialogTitle");
    layout->addWidget(title);

    auto *form = new QFormLayout;
    form->setContentsMargins(0, 0, 0, 4);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(12);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    themeCombo_ = new QComboBox;
    themeCombo_->addItems({"Dark", "Light"});
    themeCombo_->setCurrentText(settings.theme);

    languageCombo_ = new QComboBox;
    // Iterate over collection
    for (const auto &lang : LanguageManager::instance().languages()) { languageCombo_->addItem(lang.displayName); }
    languageCombo_->setCurrentText(settings.language);

    scaleSpin_ = new QDoubleSpinBox;
    scaleSpin_->setRange(0.75, 1.75);
    scaleSpin_->setSingleStep(0.05);
    scaleSpin_->setDecimals(2);
    scaleSpin_->setValue(settings.scale);

    form->addRow(zh ? "主题" : "Theme", themeCombo_);
    form->addRow(zh ? "语言" : "Language", languageCombo_);
    form->addRow(zh ? "界面缩放" : "UI scale", scaleSpin_);
    layout->addLayout(form);

    layout->addWidget(makeDialogSection(zh ? "EtherCAT 主站" : "EtherCAT masters"));
    masterTable_ = new QTableWidget;
    masterTable_->setColumnCount(2);
    masterTable_->setHorizontalHeaderLabels({zh ? "名称" : "Name", zh ? "IgH 主站选择器" : "IgH master selector"});
    masterTable_->horizontalHeader()->setStretchLastSection(true);
    masterTable_->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    masterTable_->verticalHeader()->setVisible(false);
    masterTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    masterTable_->setAlternatingRowColors(true);
    masterTable_->setShowGrid(false);
    masterTable_->setWordWrap(false);
    masterTable_->setCornerButtonEnabled(false);
    masterTable_->setMinimumHeight(230);
    masterTable_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    masterTable_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    masterTable_->verticalHeader()->setDefaultSectionSize(30);
    masterTable_->setProperty("activeMaster", settings.activeMaster);
    const QVector<MasterProfile> masters = settings.masters.isEmpty() ? QVector<MasterProfile>{MasterProfile{}} : settings.masters;
    masterTable_->setRowCount(masters.size());
    // Iterate over collection
    for (int row = 0; row < masters.size(); ++row) {
        masterTable_->setItem(row, 0, new QTableWidgetItem(masters[row].name));
        masterTable_->setItem(row, 1, new QTableWidgetItem(masters[row].target));
    }
    masterTable_->resizeColumnsToContents();
    layout->addWidget(masterTable_);

    auto *masterButtons = new QHBoxLayout;
    masterButtons->setSpacing(8);
    auto *addMaster = new QPushButton(zh ? "添加主站" : "Add master");
    auto *removeMaster = new QPushButton(zh ? "移除所选" : "Remove selected");
    addMaster->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    removeMaster->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    masterButtons->addWidget(addMaster);
    masterButtons->addWidget(removeMaster);
    masterButtons->addStretch(1);
    layout->addLayout(masterButtons);

    connect(addMaster, &QPushButton::clicked, this, [this, zh] {
        const int row = masterTable_->rowCount();
        masterTable_->insertRow(row);
        masterTable_->setItem(row, 0, new QTableWidgetItem(QString("%1 %2").arg(zh ? "主站" : "Master").arg(row)));
        masterTable_->setItem(row, 1, new QTableWidgetItem(QString::number(row)));
        masterTable_->setCurrentCell(row, 0);
    });
    connect(removeMaster, &QPushButton::clicked, this, [this] {
        if (masterTable_->rowCount() <= 1) {
            return;
        }
        const int row = masterTable_->currentRow();
        if (row >= 0) {
            masterTable_->removeRow(row);
        }
    });

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    buttons->button(QDialogButtonBox::Cancel)->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

// Collect current form values into an AppSettings struct.
// Preserves the previously active master across edits by matching on target.
// Ensures at least one default master profile exists.
AppSettings SettingsDialog::settings() const
{
    AppSettings result;
    result.theme = themeCombo_->currentText();
    result.language = languageCombo_->currentText();
    result.scale = scaleSpin_->value();
    result.masters.clear();

    // Iterate over collection
    for (int row = 0; row < masterTable_->rowCount(); ++row) {
        MasterProfile profile;
        profile.name = masterTable_->item(row, 0) ? masterTable_->item(row, 0)->text().trimmed() : QString();
        profile.target = masterTable_->item(row, 1) ? masterTable_->item(row, 1)->text().trimmed() : QString();
        if (profile.name.isEmpty() && profile.target.isEmpty()) {
            continue;
        }
        if (profile.target.isEmpty()) {
            profile.target = QString::number(result.masters.size());
        }
        if (profile.name.isEmpty()) {
            profile.name = QString("Master %1").arg(profile.target);
        }
        result.masters.append(profile);
    }
    if (result.masters.isEmpty()) {
        result.masters.append(MasterProfile{});
    }

    const QString previousActive = masterTable_->property("activeMaster").toString();
    result.activeMaster = result.masters.first().target;
    // Iterate over collection
    for (const auto &profile : result.masters) {
        if (profile.target == previousActive) {
            result.activeMaster = profile.target;
            break;
        }
    }
    return result;
}
