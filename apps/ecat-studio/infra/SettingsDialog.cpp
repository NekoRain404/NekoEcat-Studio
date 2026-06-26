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
#include <QFocusEvent>
#include <QHeaderView>
#include <QKeyEvent>
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
    tabWidget_->addTab(buildShortcutsTab(settings, zh),
                       zh ? QStringLiteral("快捷键") : QStringLiteral("Shortcuts"));
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

    // ── Network Adapter section ──────────────────────────────────────
    layout->addSpacing(8);
    layout->addWidget(makeSection(zh ? QStringLiteral("网络适配器") : QStringLiteral("Network Adapter")));

    auto *adapterRow = new QHBoxLayout;
    adapterRow->setSpacing(8);
    adapterCombo_ = new QComboBox;
    adapterCombo_->setMinimumWidth(280);
    adapterCombo_->addItem(zh ? QStringLiteral("(自动检测中...)") : QStringLiteral("(Detecting...)"), QString());
    adapterRow->addWidget(adapterCombo_);

    auto *refreshAdapterBtn = new QPushButton(zh ? QStringLiteral("刷新") : QStringLiteral("Refresh"));
    refreshAdapterBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    adapterRow->addWidget(refreshAdapterBtn);
    adapterRow->addStretch(1);
    layout->addLayout(adapterRow);

    // Pre-select the current adapter if set.
    if (!s.networkAdapter.isEmpty()) {
        const int idx = adapterCombo_->findData(s.networkAdapter);
        if (idx >= 0) adapterCombo_->setCurrentIndex(idx);
    }

    connect(refreshAdapterBtn, &QPushButton::clicked, this, [this]() {
        emit themePreviewRequested(QStringLiteral("__refresh_adapters__"));
    });

    // ── Backend Mode section ────────────────────────────────────────────
    layout->addSpacing(8);
    layout->addWidget(makeSection(zh ? QStringLiteral("后端模式") : QStringLiteral("Backend Mode")));

    auto *backendRow = new QHBoxLayout;
    backendRow->setSpacing(8);
    backendModeCombo_ = new QComboBox;
    backendModeCombo_->setMinimumWidth(280);
    backendModeCombo_->addItem(zh ? QStringLiteral("自动 (推荐)") : QStringLiteral("Auto (Recommended)"), QStringLiteral("auto"));
    backendModeCombo_->addItem(zh ? QStringLiteral("IgH 原生 API") : QStringLiteral("IgH Native API"), QStringLiteral("native"));
    backendModeCombo_->addItem(zh ? QStringLiteral("IgH 命令行") : QStringLiteral("IgH CLI"), QStringLiteral("cli"));
    backendModeCombo_->setCurrentIndex(
        s.backendMode == QStringLiteral("native") ? 1 :
        s.backendMode == QStringLiteral("cli") ? 2 : 0);
    backendRow->addWidget(backendModeCombo_);
    backendRow->addStretch(1);
    layout->addLayout(backendRow);

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
// ── KeySequenceEdit — a line edit that captures key presses as a shortcut.
// When focused, any key combination is recorded and displayed as text.
// Pressing Escape clears the shortcut; Backspace resets to default.
class KeySequenceEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit KeySequenceEdit(const QString &defaultKey, QWidget *parent = nullptr)
        : QLineEdit(parent), defaultKey_(defaultKey)
    {
        setReadOnly(true);
        setPlaceholderText(defaultKey);
        setMinimumWidth(140);
        setMaximumWidth(180);
        setStyleSheet("QLineEdit { padding: 2px 6px; }");
    }

    void setKeySequence(const QKeySequence &seq)
    {
        seq_ = seq;
        setText(seq.isEmpty() ? QString() : seq.toString(QKeySequence::NativeText));
    }

    QKeySequence keySequence() const { return seq_; }
    QString defaultKey() const { return defaultKey_; }

signals:
    void keySequenceChanged(const QKeySequence &seq);

protected:
    void keyPressEvent(QKeyEvent *e) override
    {
        int key = e->key();
        if (key == Qt::Key_Escape) {
            /* Escape clears the custom shortcut. */
            setKeySequence(QKeySequence());
            emit keySequenceChanged(QKeySequence());
            return;
        }
        if (key == Qt::Key_Backspace || key == Qt::Key_Delete) {
            /* Backspace/Delete resets to default. */
            setKeySequence(QKeySequence(defaultKey_));
            emit keySequenceChanged(QKeySequence(defaultKey_));
            return;
        }
        /* Ignore bare modifier key presses — wait for a real key. */
        if (key == Qt::Key_Control || key == Qt::Key_Shift ||
            key == Qt::Key_Alt || key == Qt::Key_Meta) {
            return;
        }
        /* Build the key sequence including held modifiers. */
        const int mods = e->modifiers().toInt() &
                         (static_cast<int>(Qt::ControlModifier) |
                          static_cast<int>(Qt::ShiftModifier) |
                          static_cast<int>(Qt::AltModifier) |
                          static_cast<int>(Qt::MetaModifier));
        QKeySequence seq(mods | key);
        setKeySequence(seq);
        emit keySequenceChanged(seq);
    }

    void focusInEvent(QFocusEvent *e) override
    {
        QLineEdit::focusInEvent(e);
        setStyleSheet("QLineEdit { padding: 2px 6px; border: 2px solid #3b82f6; }");
    }

    void focusOutEvent(QFocusEvent *e) override
    {
        QLineEdit::focusOutEvent(e);
        setStyleSheet("QLineEdit { padding: 2px 6px; }");
    }

private:
    QKeySequence seq_;
    QString defaultKey_;
};


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

    // Adapter
    if (adapterCombo_)
        r.networkAdapter = adapterCombo_->currentData().toString();

    // Backend Mode
    if (backendModeCombo_)
        r.backendMode = backendModeCombo_->currentData().toString();

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

    // Custom Shortcuts
    if (shortcutsTable_) {
        for (int row = 0; row < shortcutsTable_->rowCount(); ++row) {
            const QString id = shortcutsTable_->item(row, 0)
                ? shortcutsTable_->item(row, 0)->data(Qt::UserRole).toString()
                : QString();
            if (id.isEmpty()) continue;
            auto *edit = qobject_cast<KeySequenceEdit *>(shortcutsTable_->cellWidget(row, 2));
            if (!edit) continue;
            const QKeySequence custom = edit->keySequence();
            const QKeySequence def(edit->defaultKey());
            if (custom != def && !custom.isEmpty()) {
                r.customShortcuts[id] = custom.toString();
            } else {
                r.customShortcuts.remove(id);
            }
        }
    }

    return r;
}

// ── Shortcut entry definition ──────────────────────────────────────────
// Each entry defines a configurable action with its default key binding.
struct ShortcutDef {
    QString id;           // unique key for QSettings persistence
    QString displayName;  // label shown in the settings table
    QString defaultKey;   // platform default key sequence string
};

// Master list of all configurable shortcuts.
static const QVector<ShortcutDef> &shortcutDefinitions()
{
    static const QVector<ShortcutDef> defs = {
        {"newProject",       "New Project",              "Ctrl+N"},
        {"openProject",      "Open Project",             "Ctrl+O"},
        {"saveProject",      "Save Project",             "Ctrl+S"},
        {"saveProjectAs",    "Save Project As",          "Ctrl+Shift+S"},
        {"connect",          "Connect to Daemon",        "Ctrl+K"},
        {"refresh",          "Refresh Online Data",      "F5"},
        {"rescan",           "Rescan Bus",               "Ctrl+Shift+R"},
        {"commandPalette",   "Command Palette",          "Ctrl+P"},
        {"settings",         "Open Settings",            "Ctrl+,"},
        {"manual",           "User Manual",              "F1"},
        {"showLog",          "Toggle Runtime Log",       "Ctrl+`"},
        {"workspaceBack",    "Workspace Back",           "Alt+Left"},
        {"workspaceForward", "Workspace Forward",        "Alt+Right"},
        {"filterFocus",      "Focus Filter",             "Ctrl+F"},
        {"tab1",             "Switch to Tab 1",          "Ctrl+1"},
        {"tab2",             "Switch to Tab 2",          "Ctrl+2"},
        {"tab3",             "Switch to Tab 3",          "Ctrl+3"},
        {"tab4",             "Switch to Tab 4",          "Ctrl+4"},
        {"tab5",             "Switch to Tab 5",          "Ctrl+5"},
        {"tab6",             "Switch to Tab 6",          "Ctrl+6"},
        {"tab7",             "Switch to Tab 7",          "Ctrl+7"},
        {"tab8",             "Switch to Tab 8",          "Ctrl+8"},
        {"tab9",             "Switch to Tab 9",          "Ctrl+9"},
        {"nextTab",          "Next Tab",                 "Ctrl+Tab"},
        {"prevTab",          "Previous Tab",             "Ctrl+Shift+Tab"},
    };
    return defs;
}

// ── Build the Shortcuts configuration tab ──────────────────────────────
QWidget *SettingsDialog::buildShortcutsTab(const AppSettings &s, bool zh)
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);

    auto *hint = new QLabel(zh
        ? QStringLiteral("点击快捷键列，按下新的按键组合。Esc 清除，Backspace 恢复默认。")
        : QStringLiteral("Click the shortcut column and press a new key combination. Esc clears, Backspace resets to default."));
    hint->setWordWrap(true);
    hint->setStyleSheet("color: palette(placeholder-text); font-size: 11px; margin-bottom: 6px;");
    layout->addWidget(hint);

    const auto &defs = shortcutDefinitions();
    shortcutsTable_ = new QTableWidget(defs.size(), 3);
    shortcutsTable_->setHorizontalHeaderLabels({
        zh ? QStringLiteral("操作") : QStringLiteral("Action"),
        zh ? QStringLiteral("默认") : QStringLiteral("Default"),
        zh ? QStringLiteral("自定义") : QStringLiteral("Custom"),
    });
    shortcutsTable_->horizontalHeader()->setStretchLastSection(true);
    shortcutsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    shortcutsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    shortcutsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    shortcutsTable_->verticalHeader()->setVisible(false);
    shortcutsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    shortcutsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    shortcutsTable_->setAlternatingRowColors(true);

    for (int i = 0; i < defs.size(); ++i) {
        const auto &def = defs[i];

        /* Column 0: action name */
        auto *nameItem = new QTableWidgetItem(def.displayName);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        nameItem->setData(Qt::UserRole, def.id);
        shortcutsTable_->setItem(i, 0, nameItem);

        /* Column 1: default shortcut (read-only) */
        auto *defaultItem = new QTableWidgetItem(
            QKeySequence(def.defaultKey).toString(QKeySequence::NativeText));
        defaultItem->setFlags(defaultItem->flags() & ~Qt::ItemIsEditable);
        defaultItem->setForeground(palette().color(QPalette::PlaceholderText));
        shortcutsTable_->setItem(i, 1, defaultItem);

        /* Column 2: custom shortcut (editable via KeySequenceEdit) */
        auto *edit = new KeySequenceEdit(def.defaultKey);

        /* Load custom value if present, otherwise show default. */
        const QString customStr = s.customShortcuts.value(def.id);
        if (!customStr.isEmpty()) {
            edit->setKeySequence(QKeySequence(customStr));
        } else {
            edit->setKeySequence(QKeySequence(def.defaultKey));
        }

        shortcutsTable_->setCellWidget(i, 2, edit);
        shortcutsTable_->setRowHeight(i, 32);
    }

    /* Filter row */
    auto *filterRow = new QHBoxLayout;
    auto *filterEdit = new QLineEdit;
    filterEdit->setPlaceholderText(zh ? QStringLiteral("搜索快捷键...") : QStringLiteral("Search shortcuts..."));
    filterRow->addWidget(filterEdit);
    layout->addLayout(filterRow);
    layout->addWidget(shortcutsTable_, 1);

    connect(filterEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        for (int r = 0; r < shortcutsTable_->rowCount(); ++r) {
            bool match = text.isEmpty();
            if (!match) {
                for (int c = 0; c < shortcutsTable_->columnCount(); ++c) {
                    auto *item = shortcutsTable_->item(r, c);
                    if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                        match = true;
                        break;
                    }
                }
            }
            shortcutsTable_->setRowHidden(r, !match);
        }
    });

    /* Reset all button */
    auto *resetRow = new QHBoxLayout;
    resetRow->addStretch(1);
    auto *resetAllBtn = new QPushButton(zh ? QStringLiteral("全部恢复默认") : QStringLiteral("Reset All to Defaults"));
    connect(resetAllBtn, &QPushButton::clicked, this, [this]() {
        for (int r = 0; r < shortcutsTable_->rowCount(); ++r) {
            auto *edit = qobject_cast<KeySequenceEdit *>(shortcutsTable_->cellWidget(r, 2));
            if (edit) {
                edit->setKeySequence(QKeySequence(edit->defaultKey()));
            }
        }
    });
    resetRow->addWidget(resetAllBtn);
    layout->addLayout(resetRow);

    return page;
}

// Include MOC for KeySequenceEdit (Q_OBJECT in .cpp requires this)
#include "SettingsDialog.moc"


// ── Adapter list population ──────────────────────────────────────────
// Called by MainWindow after querying the daemon for available NICs.
void SettingsDialog::setAvailableAdapters(const QStringList &adapters)
{
    if (!adapterCombo_) return;
    adapterCombo_->clear();
    adapterCombo_->addItem(tr("(None)"), QString());
    for (const QString &entry : adapters) {
        // Format: "name|mac|driver|linkStatus"
        const QStringList parts = entry.split(QLatin1Char('|'));
        if (parts.isEmpty()) continue;
        const QString name = parts.value(0);
        const QString mac = parts.value(1);
        const QString driver = parts.value(2);
        const QString link = parts.value(3);
        const QString label = QStringLiteral("%1  [%2]  %3  %4")
            .arg(name, mac, driver, link == QStringLiteral("Up") ? tr("Up") : tr("Down"));
        adapterCombo_->addItem(label, name);
    }
}
