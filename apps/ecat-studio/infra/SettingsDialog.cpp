// SettingsDialog — comprehensive tabbed settings dialog.
// Sections: Appearance, EtherCAT, Timing, Free Run, Display, Notifications, Export.

#include "LanguageManager.h"
#include "SettingsDialog.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStyle>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {

// Create a styled section header label.
QLabel *makeSection(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName("sectionTitle");
    return label;
}

// Create a form row helper — adds label + widget to form layout.
void addFormRow(QFormLayout *form, const QString &label, QWidget *widget)
{
    form->addRow(label, widget);
}

// Build a combo box with fixed refresh interval options.
QComboBox *makeRefreshCombo(int currentMs, const QVector<QPair<int, QString>> &options)
{
    auto *combo = new QComboBox;
    for (const auto &opt : options) {
        combo->addItem(opt.second, opt.first);
        if (opt.first == currentMs) combo->setCurrentIndex(combo->count() - 1);
    }
    return combo;
}

} // namespace

// ── Constructor ──────────────────────────────────────────────────────
// Builds the tabbed settings dialog from current AppSettings values.
SettingsDialog::SettingsDialog(const AppSettings &settings, QWidget *parent)
    : QDialog(parent)
{
    const bool zh = settings.language == QStringLiteral("简体中文") || settings.language == QStringLiteral("繁體中文");
    setObjectName("settingsDialog");
    setWindowTitle(zh ? QStringLiteral("设置") : QStringLiteral("Settings"));
    setModal(true);
    resize(720, 580);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 14);
    layout->setSpacing(12);

    auto *title = new QLabel(zh ? QStringLiteral("工作站设置") : QStringLiteral("Workspace Settings"));
    title->setObjectName("dialogTitle");
    layout->addWidget(title);

    // ── Tab widget ────────────────────────────────────────────────
    tabWidget_ = new QTabWidget;
    tabWidget_->addTab(buildAppearanceTab(settings, zh),
                       zh ? QStringLiteral("外观") : QStringLiteral("Appearance"));
    tabWidget_->addTab(buildEthercatTab(settings, zh),
                       zh ? QStringLiteral("EtherCAT") : QStringLiteral("EtherCAT"));
    tabWidget_->addTab(buildTimingTab(settings, zh),
                       zh ? QStringLiteral("定时") : QStringLiteral("Timing"));
    tabWidget_->addTab(buildFreeRunTab(settings, zh),
                       zh ? QStringLiteral("Free Run") : QStringLiteral("Free Run"));
    tabWidget_->addTab(buildDisplayTab(settings, zh),
                       zh ? QStringLiteral("显示") : QStringLiteral("Display"));
    tabWidget_->addTab(buildNotificationTab(settings, zh),
                       zh ? QStringLiteral("通知") : QStringLiteral("Notifications"));
    tabWidget_->addTab(buildExportTab(settings, zh),
                       zh ? QStringLiteral("导出") : QStringLiteral("Export"));
    layout->addWidget(tabWidget_, 1);

    // ── Dialog buttons ────────────────────────────────────────────
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    buttons->button(QDialogButtonBox::Cancel)->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

// ── Appearance tab ───────────────────────────────────────────────────
// Theme, language, and UI scale controls.
QWidget *SettingsDialog::buildAppearanceTab(const AppSettings &s, bool zh)
{
    auto *tab = new QWidget;
    auto *form = new QFormLayout(tab);
    form->setContentsMargins(12, 12, 12, 8);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(10);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    themeCombo_ = new QComboBox;
    themeCombo_->addItems({QStringLiteral("Dark"), QStringLiteral("Light"), QStringLiteral("Nord"), QStringLiteral("Catppuccin"), QStringLiteral("Dracula"), QStringLiteral("Solarized"), QStringLiteral("Gruvbox"), QStringLiteral("Tokyo Night"), QStringLiteral("One Dark"), QStringLiteral("Monokai"), QStringLiteral("Cyberpunk")});
    themeCombo_->setCurrentText(s.theme);
    addFormRow(form, zh ? QStringLiteral("主题") : QStringLiteral("Theme"), themeCombo_);

    /* Emit live preview signal when user selects a different theme. */
    connect(themeCombo_, &QComboBox::currentTextChanged,
            this, &SettingsDialog::themePreviewRequested);

    languageCombo_ = new QComboBox;
    for (const auto &lang : LanguageManager::instance().languages()) {
        languageCombo_->addItem(lang.displayName);
    }
    languageCombo_->setCurrentText(s.language);
    addFormRow(form, zh ? QStringLiteral("语言") : QStringLiteral("Language"), languageCombo_);

    scaleSpin_ = new QDoubleSpinBox;
    scaleSpin_->setRange(0.75, 1.75);
    scaleSpin_->setSingleStep(0.05);
    scaleSpin_->setDecimals(2);
    scaleSpin_->setValue(s.scale);
    addFormRow(form, zh ? QStringLiteral("界面缩放") : QStringLiteral("UI Scale"), scaleSpin_);

    return tab;
}

// ── EtherCAT tab ─────────────────────────────────────────────────────
// Master profile management: add, remove, rename IgH master selectors.
QWidget *SettingsDialog::buildEthercatTab(const AppSettings &s, bool zh)
{
    auto *tab = new QWidget;
    auto *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(12, 12, 12, 8);
    layout->setSpacing(10);

    layout->addWidget(makeSection(zh ? QStringLiteral("EtherCAT 主站") : QStringLiteral("EtherCAT Masters")));

    masterTable_ = new QTableWidget;
    masterTable_->setColumnCount(2);
    masterTable_->setHorizontalHeaderLabels({zh ? QStringLiteral("名称") : QStringLiteral("Name"),
                                             zh ? QStringLiteral("IgH 主站选择器") : QStringLiteral("IgH Master Selector")});
    masterTable_->horizontalHeader()->setStretchLastSection(true);
    masterTable_->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    masterTable_->verticalHeader()->setVisible(false);
    masterTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    masterTable_->setAlternatingRowColors(true);
    masterTable_->setShowGrid(false);
    masterTable_->setWordWrap(false);
    masterTable_->setCornerButtonEnabled(false);
    masterTable_->setMinimumHeight(200);
    masterTable_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    masterTable_->verticalHeader()->setDefaultSectionSize(28);
    masterTable_->setProperty("activeMaster", s.activeMaster);

    const QVector<MasterProfile> masters = s.masters.isEmpty() ? QVector<MasterProfile>{MasterProfile{}} : s.masters;
    masterTable_->setRowCount(masters.size());
    for (int row = 0; row < masters.size(); ++row) {
        masterTable_->setItem(row, 0, new QTableWidgetItem(masters[row].name));
        masterTable_->setItem(row, 1, new QTableWidgetItem(masters[row].target));
    }
    masterTable_->resizeColumnsToContents();
    layout->addWidget(masterTable_);

    auto *btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);
    auto *addBtn = new QPushButton(zh ? QStringLiteral("添加主站") : QStringLiteral("Add Master"));
    auto *removeBtn = new QPushButton(zh ? QStringLiteral("移除所选") : QStringLiteral("Remove Selected"));
    addBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    removeBtn->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    btnRow->addWidget(addBtn);
    btnRow->addWidget(removeBtn);
    btnRow->addStretch(1);
    layout->addLayout(btnRow);

    connect(addBtn, &QPushButton::clicked, this, [this, zh] {
        const int row = masterTable_->rowCount();
        masterTable_->insertRow(row);
        masterTable_->setItem(row, 0, new QTableWidgetItem(
            QString(QStringLiteral("%1 %2")).arg(zh ? QStringLiteral("主站") : QStringLiteral("Master")).arg(row)));
        masterTable_->setItem(row, 1, new QTableWidgetItem(QString::number(row)));
        masterTable_->setCurrentCell(row, 0);
    });
    connect(removeBtn, &QPushButton::clicked, this, [this] {
        if (masterTable_->rowCount() <= 1) return;
        const int row = masterTable_->currentRow();
        if (row >= 0) masterTable_->removeRow(row);
    });

    return tab;
}

// ── Timing tab ───────────────────────────────────────────────────────
// Auto-refresh intervals, SDO timeouts, topology polling.
QWidget *SettingsDialog::buildTimingTab(const AppSettings &s, bool zh)
{
    auto *tab = new QWidget;
    auto *form = new QFormLayout(tab);
    form->setContentsMargins(12, 12, 12, 8);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(10);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Watch auto-refresh
    watchRefreshCombo_ = makeRefreshCombo(s.watchAutoRefreshMs, {
        {0, zh ? QStringLiteral("关闭") : QStringLiteral("Off")},
        {250, QStringLiteral("250 ms")}, {500, QStringLiteral("500 ms")},
        {1000, QStringLiteral("1 s")}, {2000, QStringLiteral("2 s")}
    });
    addFormRow(form, zh ? QStringLiteral("Watch 自动刷新") : QStringLiteral("Watch Auto-Refresh"), watchRefreshCombo_);

    // Overview auto-refresh
    overviewRefreshCombo_ = makeRefreshCombo(s.overviewAutoRefreshMs, {
        {0, zh ? QStringLiteral("关闭") : QStringLiteral("Off")},
        {1000, QStringLiteral("1 s")}, {2000, QStringLiteral("2 s")}, {5000, QStringLiteral("5 s")}
    });
    addFormRow(form, zh ? QStringLiteral("总览自动刷新") : QStringLiteral("Overview Auto-Refresh"), overviewRefreshCombo_);

    // SDO read timeout
    sdoReadTimeoutSpin_ = new QSpinBox;
    sdoReadTimeoutSpin_->setRange(500, 30000);
    sdoReadTimeoutSpin_->setSingleStep(500);
    sdoReadTimeoutSpin_->setSuffix(QStringLiteral(" ms"));
    sdoReadTimeoutSpin_->setValue(s.sdoReadTimeoutMs);
    addFormRow(form, zh ? QStringLiteral("SDO 读取超时") : QStringLiteral("SDO Read Timeout"), sdoReadTimeoutSpin_);

    // SDO write timeout
    sdoWriteTimeoutSpin_ = new QSpinBox;
    sdoWriteTimeoutSpin_->setRange(500, 60000);
    sdoWriteTimeoutSpin_->setSingleStep(500);
    sdoWriteTimeoutSpin_->setSuffix(QStringLiteral(" ms"));
    sdoWriteTimeoutSpin_->setValue(s.sdoWriteTimeoutMs);
    addFormRow(form, zh ? QStringLiteral("SDO 写入超时") : QStringLiteral("SDO Write Timeout"), sdoWriteTimeoutSpin_);

    // Topology poll
    topologyPollCombo_ = makeRefreshCombo(s.topologyPollIntervalMs, {
        {0, zh ? QStringLiteral("关闭") : QStringLiteral("Off")},
        {5000, QStringLiteral("5 s")}, {10000, QStringLiteral("10 s")}, {30000, QStringLiteral("30 s")}
    });
    addFormRow(form, zh ? QStringLiteral("拓扑轮询间隔") : QStringLiteral("Topology Poll Interval"), topologyPollCombo_);

    return tab;
}

// ── Free Run tab ─────────────────────────────────────────────────────
// Free Run cycle time, auto-name, change highlighting.
QWidget *SettingsDialog::buildFreeRunTab(const AppSettings &s, bool zh)
{
    auto *tab = new QWidget;
    auto *form = new QFormLayout(tab);
    form->setContentsMargins(12, 12, 12, 8);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(10);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    freeRunCycleSpin_ = new QSpinBox;
    freeRunCycleSpin_->setRange(100, 100000);
    freeRunCycleSpin_->setSingleStep(100);
    freeRunCycleSpin_->setSuffix(QStringLiteral(" µs"));
    freeRunCycleSpin_->setValue(s.freeRunCycleUs);
    addFormRow(form, zh ? QStringLiteral("周期时间") : QStringLiteral("Cycle Time"), freeRunCycleSpin_);

    freeRunAutoNameCheck_ = new QCheckBox(zh ? QStringLiteral("从对象字典自动命名") : QStringLiteral("Auto-name from Object Dictionary"));
    freeRunAutoNameCheck_->setChecked(s.freeRunAutoName);
    form->addRow(QStringLiteral(""), freeRunAutoNameCheck_);

    freeRunHighlightCheck_ = new QCheckBox(zh ? QStringLiteral("高亮变化值") : QStringLiteral("Highlight changed values"));
    freeRunHighlightCheck_->setChecked(s.freeRunHighlightChanges);
    form->addRow(QStringLiteral(""), freeRunHighlightCheck_);

    return tab;
}

// ── Display tab ──────────────────────────────────────────────────────
// Table appearance, raw tabs, compact mode, detail panel width.
QWidget *SettingsDialog::buildDisplayTab(const AppSettings &s, bool zh)
{
    auto *tab = new QWidget;
    auto *form = new QFormLayout(tab);
    form->setContentsMargins(12, 12, 12, 8);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(10);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    showRawTabsCheck_ = new QCheckBox(zh ? QStringLiteral("显示原始数据选项卡") : QStringLiteral("Show raw data tabs"));
    showRawTabsCheck_->setChecked(s.showRawTabs);
    form->addRow(QStringLiteral(""), showRawTabsCheck_);

    showGridCheck_ = new QCheckBox(zh ? QStringLiteral("显示表格网格线") : QStringLiteral("Show table grid lines"));
    showGridCheck_->setChecked(s.showColumnGrid);
    form->addRow(QStringLiteral(""), showGridCheck_);

    alternatingRowsCheck_ = new QCheckBox(zh ? QStringLiteral("交替行颜色") : QStringLiteral("Alternating row colors"));
    alternatingRowsCheck_->setChecked(s.alternatingRowColors);
    form->addRow(QStringLiteral(""), alternatingRowsCheck_);

    compactModeCheck_ = new QCheckBox(zh ? QStringLiteral("紧凑模式") : QStringLiteral("Compact mode"));
    compactModeCheck_->setChecked(s.compactMode);
    form->addRow(QStringLiteral(""), compactModeCheck_);

    detailWidthSpin_ = new QSpinBox;
    detailWidthSpin_->setRange(200, 600);
    detailWidthSpin_->setSingleStep(20);
    detailWidthSpin_->setSuffix(QStringLiteral(" px"));
    detailWidthSpin_->setValue(s.detailPanelWidth);
    addFormRow(form, zh ? QStringLiteral("详情面板宽度") : QStringLiteral("Detail Panel Width"), detailWidthSpin_);

    rowHeightSpin_ = new QSpinBox;
    rowHeightSpin_->setRange(20, 48);
    rowHeightSpin_->setSingleStep(2);
    rowHeightSpin_->setSuffix(QStringLiteral(" px"));
    rowHeightSpin_->setValue(s.tableRowHeight);
    addFormRow(form, zh ? QStringLiteral("表格行高") : QStringLiteral("Table Row Height"), rowHeightSpin_);

    maxHistorySpin_ = new QSpinBox;
    maxHistorySpin_->setRange(50, 2000);
    maxHistorySpin_->setSingleStep(50);
    maxHistorySpin_->setValue(s.maxHistoryEntries);
    addFormRow(form, zh ? QStringLiteral("历史记录上限") : QStringLiteral("Max History Entries"), maxHistorySpin_);

    return tab;
}

// ── Notification tab ─────────────────────────────────────────────────
// Notification preferences for state changes, errors, watch drift.
QWidget *SettingsDialog::buildNotificationTab(const AppSettings &s, bool zh)
{
    auto *tab = new QWidget;
    auto *form = new QFormLayout(tab);
    form->setContentsMargins(12, 12, 12, 8);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(10);

    notifyStateCheck_ = new QCheckBox(zh ? QStringLiteral("从站状态变化时通知") : QStringLiteral("Notify on slave state change"));
    notifyStateCheck_->setChecked(s.notifyOnStateChange);
    form->addRow(QStringLiteral(""), notifyStateCheck_);

    notifyErrorCheck_ = new QCheckBox(zh ? QStringLiteral("错误发生时通知") : QStringLiteral("Notify on errors"));
    notifyErrorCheck_->setChecked(s.notifyOnError);
    form->addRow(QStringLiteral(""), notifyErrorCheck_);

    notifyDriftCheck_ = new QCheckBox(zh ? QStringLiteral("Watch 值偏移时通知") : QStringLiteral("Notify on watch value drift"));
    notifyDriftCheck_->setChecked(s.notifyOnWatchDrift);
    form->addRow(QStringLiteral(""), notifyDriftCheck_);

    soundCheck_ = new QCheckBox(zh ? QStringLiteral("关键事件提示音") : QStringLiteral("Sound on critical events"));
    soundCheck_->setChecked(s.soundEnabled);
    form->addRow(QStringLiteral(""), soundCheck_);

    toastDurationSpin_ = new QSpinBox;
    toastDurationSpin_->setRange(1000, 10000);
    toastDurationSpin_->setSingleStep(500);
    toastDurationSpin_->setSuffix(QStringLiteral(" ms"));
    toastDurationSpin_->setValue(s.toastDurationMs);
    addFormRow(form, zh ? QStringLiteral("提示持续时间") : QStringLiteral("Toast Duration"), toastDurationSpin_);

    return tab;
}

// ── Export tab ────────────────────────────────────────────────────────
// Default export directory, ESI path, CSV format, metadata options.
QWidget *SettingsDialog::buildExportTab(const AppSettings &s, bool zh)
{
    auto *tab = new QWidget;
    auto *form = new QFormLayout(tab);
    form->setContentsMargins(12, 12, 12, 8);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(10);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Export directory with browse button
    auto *exportDirRow = new QHBoxLayout;
    exportDirEdit_ = new QLineEdit(s.defaultExportDir);
    exportDirEdit_->setPlaceholderText(zh ? QStringLiteral("默认使用项目目录") : QStringLiteral("Defaults to project directory"));
    auto *browseBtn = new QPushButton(zh ? QStringLiteral("浏览") : QStringLiteral("Browse"));
    browseBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    exportDirRow->addWidget(exportDirEdit_, 1);
    exportDirRow->addWidget(browseBtn);
    connect(browseBtn, &QPushButton::clicked, this, [this, zh] {
        QString dir = QFileDialog::getExistingDirectory(
            this, zh ? QStringLiteral("选择导出目录") : QStringLiteral("Select Export Directory"), exportDirEdit_->text());
        if (!dir.isEmpty()) exportDirEdit_->setText(dir);
    });
    addFormRow(form, zh ? QStringLiteral("默认导出目录") : QStringLiteral("Default Export Directory"), new QWidget);
    form->itemAt(form->count() - 1)->widget()->setLayout(exportDirRow);

    // ESI repository path
    auto *esiRow = new QHBoxLayout;
    esiPathEdit_ = new QLineEdit(s.esiRepositoryPath);
    esiPathEdit_->setPlaceholderText(zh ? QStringLiteral("ESI 文件搜索路径") : QStringLiteral("ESI file search path"));
    auto *esiBrowseBtn = new QPushButton(zh ? QStringLiteral("浏览") : QStringLiteral("Browse"));
    esiBrowseBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    esiRow->addWidget(esiPathEdit_, 1);
    esiRow->addWidget(esiBrowseBtn);
    connect(esiBrowseBtn, &QPushButton::clicked, this, [this, zh] {
        QString dir = QFileDialog::getExistingDirectory(
            this, zh ? QStringLiteral("选择 ESI 仓库路径") : QStringLiteral("Select ESI Repository Path"), esiPathEdit_->text());
        if (!dir.isEmpty()) esiPathEdit_->setText(dir);
    });
    addFormRow(form, zh ? QStringLiteral("ESI 仓库路径") : QStringLiteral("ESI Repository Path"), new QWidget);
    form->itemAt(form->count() - 1)->widget()->setLayout(esiRow);

    exportTimestampCheck_ = new QCheckBox(zh ? QStringLiteral("导出文件包含时间戳") : QStringLiteral("Include timestamp in exports"));
    exportTimestampCheck_->setChecked(s.exportIncludeTimestamp);
    form->addRow(QStringLiteral(""), exportTimestampCheck_);

    exportMetadataCheck_ = new QCheckBox(zh ? QStringLiteral("导出文件包含元数据") : QStringLiteral("Include metadata in exports"));
    exportMetadataCheck_->setChecked(s.exportIncludeMetadata);
    form->addRow(QStringLiteral(""), exportMetadataCheck_);

    csvDelimiterCombo_ = new QComboBox;
    csvDelimiterCombo_->addItem(QStringLiteral(" ,  (CSV)"), QStringLiteral(","));
    csvDelimiterCombo_->addItem(QStringLiteral(" ;  (CSV EU)"), QStringLiteral(";"));
    csvDelimiterCombo_->addItem(QStringLiteral("\\t (TSV)"), QStringLiteral("\t"));
    csvDelimiterCombo_->setCurrentIndex(s.csvDelimiter == QStringLiteral(";") ? 1 : s.csvDelimiter == QStringLiteral("\t") ? 2 : 0);
    addFormRow(form, zh ? QStringLiteral("CSV 分隔符") : QStringLiteral("CSV Delimiter"), csvDelimiterCombo_);

    return tab;
}

// ── Collect settings ─────────────────────────────────────────────────
// Reads all widget states into a new AppSettings struct.
AppSettings SettingsDialog::settings() const
{
    AppSettings r;

    // Appearance
    r.theme = themeCombo_->currentText();
    r.language = languageCombo_->currentText();
    r.scale = scaleSpin_->value();

    // Masters
    r.masters.clear();
    for (int row = 0; row < masterTable_->rowCount(); ++row) {
        MasterProfile p;
        p.name = masterTable_->item(row, 0) ? masterTable_->item(row, 0)->text().trimmed() : QString();
        p.target = masterTable_->item(row, 1) ? masterTable_->item(row, 1)->text().trimmed() : QString();
        if (p.name.isEmpty() && p.target.isEmpty()) continue;
        if (p.target.isEmpty()) p.target = QString::number(r.masters.size());
        if (p.name.isEmpty()) p.name = QString(QStringLiteral("Master %1")).arg(p.target);
        r.masters.append(p);
    }
    if (r.masters.isEmpty()) r.masters.append(MasterProfile{});

    const QString prev = masterTable_->property("activeMaster").toString();
    r.activeMaster = r.masters.first().target;
    for (const auto &p : r.masters) {
        if (p.target == prev) { r.activeMaster = p.target; break; }
    }

    // Timing
    r.watchAutoRefreshMs = watchRefreshCombo_->currentData().toInt();
    r.overviewAutoRefreshMs = overviewRefreshCombo_->currentData().toInt();
    r.sdoReadTimeoutMs = sdoReadTimeoutSpin_->value();
    r.sdoWriteTimeoutMs = sdoWriteTimeoutSpin_->value();
    r.topologyPollIntervalMs = topologyPollCombo_->currentData().toInt();

    // Free Run
    r.freeRunCycleUs = freeRunCycleSpin_->value();
    r.freeRunAutoName = freeRunAutoNameCheck_->isChecked();
    r.freeRunHighlightChanges = freeRunHighlightCheck_->isChecked();

    // Display
    r.showRawTabs = showRawTabsCheck_->isChecked();
    r.showColumnGrid = showGridCheck_->isChecked();
    r.detailPanelWidth = detailWidthSpin_->value();
    r.tableRowHeight = rowHeightSpin_->value();
    r.alternatingRowColors = alternatingRowsCheck_->isChecked();
    r.compactMode = compactModeCheck_->isChecked();
    r.maxHistoryEntries = maxHistorySpin_->value();

    // Notifications
    r.notifyOnStateChange = notifyStateCheck_->isChecked();
    r.notifyOnError = notifyErrorCheck_->isChecked();
    r.notifyOnWatchDrift = notifyDriftCheck_->isChecked();
    r.soundEnabled = soundCheck_->isChecked();
    r.toastDurationMs = toastDurationSpin_->value();

    // Export
    r.defaultExportDir = exportDirEdit_->text().trimmed();
    r.esiRepositoryPath = esiPathEdit_->text().trimmed();
    r.exportIncludeTimestamp = exportTimestampCheck_->isChecked();
    r.exportIncludeMetadata = exportMetadataCheck_->isChecked();
    r.csvDelimiter = csvDelimiterCombo_->currentData().toString();

    return r;
}
