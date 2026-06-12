#include "MainWindow.h"

#include "MainWindow.h"
#include "models/Cia402DriveModel.h"
#include "models/CommissioningWorkflowModel.h"
#include "ui_state/CommissioningWorkflowStepDetailUiState.h"
#include "adapters/CommissioningWorkflowTableAdapter.h"
#include "ui_state/CommissioningWorkflowUiState.h"
#include "ui_state/ConsistencyDetailUiState.h"
#include "models/ConsistencyEvidenceRouteModel.h"
#include "models/ConsistencyGateModel.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "ui_state/DiagnosticsEventUiState.h"
#include "models/EvidenceStatusModel.h"
#include "ui_state/FreeRunEntryDetailUiState.h"
#include "ui_state/HostHealthUiState.h"
#include "models/IoVariableBulkNamingModel.h"
#include "ui_state/IoVariableDetailUiState.h"
#include "models/IoVariableFilterModel.h"
#include "models/IoVariableHandoffModel.h"
#include "models/NextBestActionModel.h"
#include "ui_state/NextBestActionUiState.h"
#include "ui_state/ObjectBookmarkDetailUiState.h"
#include "ui_state/PdoMapDetailUiState.h"
#include "models/ProcessDataRowModel.h"
#include "adapters/ProcessDataTableAdapter.h"
#include "adapters/SdoDictionaryTableAdapter.h"
#include "models/SdoEvidenceModel.h"
#include "adapters/SdoEvidenceTableAdapter.h"
#include "ui_state/SdoHistoryRowDetailUiState.h"
#include "models/SdoTargetPanelRouteModel.h"
#include "ui_state/SdoTargetTrailDetailUiState.h"
#include "ui_state/SelectedDriveSummaryUiState.h"
#include "ui_state/SelectedSlaveEvidenceSummaryUiState.h"
#include "models/SessionBriefModel.h"
#include "adapters/SessionBriefTableAdapter.h"
#include "ui_state/SessionBriefUiState.h"
#include "models/SlaveEvidenceModel.h"
#include "adapters/SlaveEvidenceTableAdapter.h"
#include "ui_state/SlaveEvidenceUiState.h"
#include "ui_state/StartupSdoRowDetailUiState.h"
#include "ui_state/StateMachineRowDetailUiState.h"
#include "adapters/StateMachineTableAdapter.h"
#include "models/StateRecommendationModel.h"
#include "helpers/StudioDocumentation.h"
#include "helpers/StudioTableHelpers.h"
#include "helpers/StudioTextHelpers.h"
#include "helpers/StudioUiHelpers.h"
#include "models/TopologyBaselineModel.h"
#include "models/TopologyChangeModel.h"
#include "ui_state/WatchRowDetailUiState.h"
#include "models/WatchStartupModel.h"
#include "adapters/WatchStartupTableAdapter.h"
#include "ui_state/WatchStartupUiState.h"
#include "ui_state/WorkspaceBoundaryUiState.h"
#include "adapters/WorkspaceTabBadgeTableAdapter.h"
#include "ui_state/WorkspaceTabBadgeUiState.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QCheckBox>
#include <QClipboard>
#include <QColor>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QDockWidget>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHash>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSignalBlocker>
#include <QSize>
#include <QSizePolicy>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextStream>
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QXmlStreamReader>

QString MainWindow::ioVariableRowKey(int row) const {
  return ioVariableTableRowKey(ioVariableTable_, row);
}

QVector<int> MainWindow::selectedIoVariableRows(bool visibleOnly) const {
  return selectedIoVariableTableRows(ioVariableTable_, visibleOnly);
}

QVector<int> MainWindow::visibleIoVariableRows() const {
  return visibleIoVariableTableRows(ioVariableTable_);
}

QString MainWindow::ioVariableHandoffIssueLabel(const QString &key) const {
  if (key == QStringLiteral("missingAlias")) {
    return uiText("Missing Alias", "缺少 Alias");
  }
  if (key == QStringLiteral("autoName")) {
    return uiText("Auto Name", "自动命名");
  }
  if (key == QStringLiteral("noTags")) {
    return uiText("No Tags", "无标签");
  }
  if (key == QStringLiteral("duplicateSymbol")) {
    return uiText("Duplicate Symbol", "符号重复");
  }
  return key;
}

QStringList
MainWindow::ioVariableHandoffIssueLabels(const QStringList &keys) const {
  QStringList labels;
  for (const QString &key : keys) {
    labels << ioVariableHandoffIssueLabel(key);
  }
  return labels;
}

QString MainWindow::ioVariablePlcQuality(int row,
                                         const QSet<QString> *duplicateSymbols,
                                         QString *symbol) const {
  if (!ioVariableTable_ || row < 0 || row >= ioVariableTable_->rowCount()) {
    return QString();
  }

  const IoVariableTableRow variable =
      ioVariableTableRowFromTable(ioVariableTable_, row);
  const IoVariableHandoffName name = ioVariableHandoffName(variable);
  if (symbol) {
    *symbol = name.symbol;
  }

  const QStringList keys = ioVariableHandoffIssueKeys(
      ioVariableHandoffIssues(variable, duplicateSymbols));
  return keys.isEmpty() ? uiText("Ready", "就绪")
                        : ioVariableHandoffIssueLabels(keys).join(" | ");
}

QSet<QString> MainWindow::duplicateIoVariablePlcSymbols() const {
  if (!ioVariableTable_) {
    return {};
  }
  QVector<IoVariableTableRow> rows;
  rows.reserve(ioVariableTable_->rowCount());
  for (int row = 0; row < ioVariableTable_->rowCount(); ++row) {
    rows.append(ioVariableTableRowFromTable(ioVariableTable_, row));
  }
  return duplicateIoVariableHandoffSymbols(rows);
}

QVector<int> MainWindow::plcHandoffIssueRows(const QVector<int> &rows) const {
  QVector<int> issueRows;
  if (!ioVariableTable_) {
    return issueRows;
  }

  const QSet<QString> duplicateSymbols = duplicateIoVariablePlcSymbols();
  for (const int row : rows) {
    if (row < 0 || row >= ioVariableTable_->rowCount()) {
      continue;
    }
    const QString quality =
        ioVariablePlcQuality(row, &duplicateSymbols, nullptr);
    if (!quality.isEmpty() && quality != uiText("Ready", "就绪")) {
      issueRows.append(row);
    }
  }
  return issueRows;
}

QStringList MainWindow::plcHandoffIssueDetails(const QVector<int> &rows,
                                               int previewLimit,
                                               int totalRows) const {
  QStringList details;
  if (!ioVariableTable_ || rows.isEmpty()) {
    return details;
  }

  const QSet<QString> duplicateSymbols = duplicateIoVariablePlcSymbols();
  int missingAlias = 0;
  int autoName = 0;
  int noTags = 0;
  int duplicateSymbol = 0;
  QStringList preview;
  const int limit = std::max(0, previewLimit);
  for (const int row : rows) {
    if (row < 0 || row >= ioVariableTable_->rowCount()) {
      continue;
    }
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable_, row);
    const QVector<IoVariableHandoffIssue> issues =
        ioVariableHandoffIssues(variable, &duplicateSymbols);
    const QStringList issueKeys = ioVariableHandoffIssueKeys(issues);
    const QString quality =
        issueKeys.isEmpty()
            ? uiText("Ready", "就绪")
            : ioVariableHandoffIssueLabels(issueKeys).join(" | ");
    const QString symbol = ioVariableHandoffName(variable).symbol;
    if (ioVariableHandoffHasIssue(issues,
                                  IoVariableHandoffIssue::MissingAlias)) {
      ++missingAlias;
    }
    if (ioVariableHandoffHasIssue(issues, IoVariableHandoffIssue::AutoName)) {
      ++autoName;
    }
    if (ioVariableHandoffHasIssue(issues, IoVariableHandoffIssue::NoTags)) {
      ++noTags;
    }
    if (ioVariableHandoffHasIssue(issues,
                                  IoVariableHandoffIssue::DuplicateSymbol)) {
      ++duplicateSymbol;
    }
    if (preview.size() < limit) {
      preview << QString("#%1 %2:%3  %4  -> %5  [%6]")
                     .arg(variable.positionValid
                              ? QString::number(variable.position)
                              : QStringLiteral("?"),
                          variable.index, variable.subIndex, symbol, quality);
    }
  }

  details << uiText("Rows with PLC handoff issues: %1 / %2",
                    "存在 PLC 交接问题的行：%1 / %2")
                 .arg(rows.size())
                 .arg(totalRows >= 0 ? totalRows
                                     : ioVariableTable_->rowCount());
  QStringList issueCounts;
  if (missingAlias > 0) {
    issueCounts
        << uiText("missing alias %1", "缺少 Alias %1").arg(missingAlias);
  }
  if (autoName > 0) {
    issueCounts << uiText("auto name %1", "自动命名 %1").arg(autoName);
  }
  if (noTags > 0) {
    issueCounts << uiText("no tags %1", "无 Tags %1").arg(noTags);
  }
  if (duplicateSymbol > 0) {
    issueCounts
        << uiText("duplicate symbol %1", "符号重复 %1").arg(duplicateSymbol);
  }
  if (!issueCounts.isEmpty()) {
    details << uiText("Issue types: %1", "问题类型：%1")
                   .arg(issueCounts.join("; "));
  }
  details << uiText("This is project-local handoff metadata only; it does not "
                    "read or write the EtherCAT bus.",
                    "这只涉及工程内交接元数据，不会读取或写入 EtherCAT 总线。");
  details.append(preview);
  if (rows.size() > limit) {
    details << uiText("...and %1 more issue row(s)", "...另有 %1 条问题行")
                   .arg(rows.size() - limit);
  }
  return details;
}

QString MainWindow::plcDeclarationBlock(const QVector<int> &rows) const {
  if (!ioVariableTable_ || rows.isEmpty()) {
    return QString();
  }

  const QSet<QString> duplicateSymbols = duplicateIoVariablePlcSymbols();
  QVector<IoVariableTableRow> variables;
  QVector<QStringList> qualityLabelsByRow;
  variables.reserve(rows.size());
  qualityLabelsByRow.reserve(rows.size());
  for (const int row : rows) {
    if (row < 0 || row >= ioVariableTable_->rowCount()) {
      continue;
    }

    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable_, row);
    const QString quality = ioVariablePlcQuality(row, &duplicateSymbols);
    variables << variable;
    qualityLabelsByRow << (quality == uiText("Ready", "就绪")
                               ? QStringList()
                               : quality.split(" | "));
  }
  return ioVariableHandoffDeclarationBlock(variables, qualityLabelsByRow);
}

void MainWindow::editSelectedIoVariableMetadata() {
  if (!ioVariableTable_) {
    return;
  }
  const QVector<int> rows = selectedIoVariableRows(false);
  const int row = rows.isEmpty() ? -1 : rows.first();
  const QString key = ioVariableRowKey(row);
  if (key.isEmpty()) {
    updateDiagnostics(
        "Warning", "I/O Variables",
        uiText("Select an I/O variable row first", "请先选择一条 I/O 变量"));
    return;
  }

  const QStringList metadata = ioVariableMetadata_.value(key);
  QDialog dialog(this);
  dialog.setWindowTitle(uiText("I/O Variable Alias", "I/O 变量别名"));
  dialog.setModal(true);
  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(10);
  const IoVariableTableRow variable =
      ioVariableTableRowFromTable(ioVariableTable_, row);
  auto *context = new QLabel(
      QString("#%1 %2:%3  %4")
          .arg(variable.positionValid ? QString::number(variable.position)
                                      : QStringLiteral("?"),
               variable.index, variable.subIndex, variable.symbol));
  context->setObjectName("diagnosticsSummary");
  context->setWordWrap(true);
  layout->addWidget(context);

  auto *form = new QGridLayout;
  form->setHorizontalSpacing(10);
  form->setVerticalSpacing(8);
  auto *aliasEdit = new QLineEdit(metadata.value(0));
  aliasEdit->setPlaceholderText(
      uiText("Engineering name, for example Axis X Statusword",
             "工程名，例如 X 轴状态字"));
  auto *tagsEdit = new QLineEdit(metadata.value(1));
  tagsEdit->setPlaceholderText(
      uiText("Comma-separated tags, for example axis-x, cia402, safety",
             "逗号分隔标签，例如 axis-x, cia402, safety"));
  auto *noteEdit = new QPlainTextEdit;
  noteEdit->setPlainText(metadata.value(2));
  noteEdit->setPlaceholderText(
      uiText("Optional commissioning note", "可选调试备注"));
  noteEdit->setFixedHeight(86);
  form->addWidget(new QLabel(uiText("Alias", "别名")), 0, 0);
  form->addWidget(aliasEdit, 0, 1);
  form->addWidget(new QLabel(uiText("Tags", "标签")), 1, 0);
  form->addWidget(tagsEdit, 1, 1);
  form->addWidget(new QLabel(uiText("Note", "备注")), 2, 0);
  form->addWidget(noteEdit, 2, 1);
  layout->addLayout(form);

  auto *buttons =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
  layout->addWidget(buttons);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  const QString alias = aliasEdit->text().trimmed();
  const QString tags = tagsEdit->text().trimmed();
  const QString note = noteEdit->toPlainText().trimmed();
  if (alias.isEmpty() && tags.isEmpty() && note.isEmpty()) {
    ioVariableMetadata_.remove(key);
  } else {
    ioVariableMetadata_.insert(key, {alias, tags, note});
  }
  updateIoVariableTable();
  updateDiagnostics("Info", "I/O Variables",
                    uiText("Updated I/O variable metadata for %1",
                           "已更新 I/O 变量元数据：%1")
                        .arg(key));
}

void MainWindow::bulkNameIoVariables() {
  if (!ioVariableTable_ || ioVariableTable_->rowCount() <= 0) {
    QMessageBox::information(
        this, uiText("Bulk Name I/O Variables", "批量命名 I/O 变量"),
        uiText("No I/O variables are available.",
               "当前没有可命名的 I/O 变量。"));
    return;
  }

  const QVector<int> selectedRows = selectedIoVariableRows(true);
  const QVector<int> visibleRows = visibleIoVariableRows();
  if (selectedRows.isEmpty() && visibleRows.isEmpty()) {
    updateDiagnostics("Warning", "I/O Variables",
                      uiText("No visible I/O variable rows are available",
                             "当前没有可见 I/O 变量行"));
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle(uiText("Bulk Name I/O Variables", "批量命名 I/O 变量"));
  dialog.setModal(true);
  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(10);

  auto *summary = new QLabel(
      uiText("Generate project-local aliases and tags. This does not read or "
             "write the EtherCAT bus.",
             "生成工程内 Alias 和 Tags。此操作不会读取或写入 EtherCAT 总线。"));
  summary->setObjectName("diagnosticsSummary");
  summary->setWordWrap(true);
  layout->addWidget(summary);

  auto *form = new QGridLayout;
  form->setHorizontalSpacing(10);
  form->setVerticalSpacing(8);
  auto *scopeCombo = new QComboBox;
  if (!selectedRows.isEmpty()) {
    scopeCombo->addItem(
        uiText("Selected rows (%1)", "所选行 (%1)").arg(selectedRows.size()),
        "selected");
  }
  scopeCombo->addItem(
      uiText("Visible rows (%1)", "可见行 (%1)").arg(visibleRows.size()),
      "visible");
  auto *prefixEdit = new QLineEdit;
  prefixEdit->setPlaceholderText(
      uiText("Optional prefix, for example MachineA or Line1",
             "可选前缀，例如 MachineA 或 Line1"));
  auto *tagsEdit = new QLineEdit(uiText("plc,handoff", "plc,交接"));
  tagsEdit->setPlaceholderText(
      uiText("Tags appended to every row", "追加到每行的标签"));
  auto *includeAddress = new QCheckBox(uiText(
      "Include slave/index in generated alias", "生成 Alias 时包含从站和索引"));
  includeAddress->setChecked(true);
  auto *protectExisting =
      new QCheckBox(uiText("Keep existing aliases", "保留已有 Alias"));
  protectExisting->setChecked(true);
  auto *addDirectionTags =
      new QCheckBox(uiText("Add direction/type tags", "添加方向/类型标签"));
  addDirectionTags->setChecked(true);
  form->addWidget(new QLabel(uiText("Rows", "行范围")), 0, 0);
  form->addWidget(scopeCombo, 0, 1);
  form->addWidget(new QLabel(uiText("Prefix", "前缀")), 1, 0);
  form->addWidget(prefixEdit, 1, 1);
  form->addWidget(new QLabel(uiText("Tags", "标签")), 2, 0);
  form->addWidget(tagsEdit, 2, 1);
  form->addWidget(includeAddress, 3, 1);
  form->addWidget(protectExisting, 4, 1);
  form->addWidget(addDirectionTags, 5, 1);
  layout->addLayout(form);

  auto *buttons =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttons->button(QDialogButtonBox::Ok)->setText(uiText("Apply", "应用"));
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
  layout->addWidget(buttons);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  QVector<int> rows = scopeCombo->currentData().toString() == "selected"
                          ? selectedRows
                          : visibleRows;
  if (rows.isEmpty()) {
    return;
  }
  const bool keepExisting = protectExisting->isChecked();

  QVector<IoVariableTableRow> allRows;
  allRows.reserve(ioVariableTable_->rowCount());
  for (int row = 0; row < ioVariableTable_->rowCount(); ++row) {
    allRows << ioVariableTableRowFromTable(ioVariableTable_, row);
  }
  QHash<int, IoVariableTableRow> rowsByNumber;
  rowsByNumber.reserve(allRows.size());
  for (const IoVariableTableRow &variable : allRows) {
    rowsByNumber.insert(variable.row, variable);
  }
  QVector<IoVariableTableRow> namingRows;
  namingRows.reserve(rows.size());
  for (const int row : rows) {
    if (rowsByNumber.contains(row)) {
      namingRows << rowsByNumber.value(row);
    }
  }
  const int existingAliases =
      countIoVariableBulkNamingExistingAliases(namingRows);
  if (!keepExisting && existingAliases > 0) {
    const QStringList details = {
        uiText("%1 existing alias(es) will be replaced.",
               "%1 个已有 Alias 将被覆盖。")
            .arg(existingAliases),
        uiText("The change is project-local metadata only.",
               "此更改只影响工程内元数据。")};
    if (!confirmDangerousOperation(
            uiText("Replace Existing I/O Aliases", "覆盖已有 I/O Alias"),
            uiText("Bulk naming is about to replace existing aliases.",
                   "批量命名将覆盖已有 Alias。"),
            details, uiText("Replace Aliases", "覆盖 Alias"))) {
      return;
    }
  }

  IoVariableBulkNamingOptions options;
  options.prefix = prefixEdit->text();
  options.requestedTags = tagsEdit->text().split(',', Qt::SkipEmptyParts);
  options.includeAddress = includeAddress->isChecked();
  options.keepExistingAliases = keepExisting;
  options.addDirectionTags = addDirectionTags->isChecked();

  const IoVariableBulkNamingResult result = buildIoVariableBulkNamingPlan(
      allRows, rows, ioVariableMetadata_, options);
  for (auto it = result.metadataUpdates.cbegin();
       it != result.metadataUpdates.cend(); ++it) {
    ioVariableMetadata_.insert(it.key(), it.value());
  }

  consistencyFresh_ = false;
  updateIoVariableTable();
  updateDiagnostics(
      "Info", "I/O Variables",
      uiText("Bulk named %1 I/O variable row(s), skipped %2 existing alias(es)",
             "已批量命名 %1 条 I/O 变量，跳过 %2 个已有 Alias")
          .arg(result.updated)
          .arg(result.skippedExistingAliases));
}

void MainWindow::reviewPlcHandoffIssues() {
  updateIoVariableTable();
  if (!ioVariableTable_ || ioVariableTable_->rowCount() <= 0) {
    QMessageBox::information(
        this, uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"),
        uiText("No I/O variables are available.",
               "当前没有可审阅的 I/O 变量。"));
    return;
  }

  const QVector<int> visibleRows = visibleIoVariableRows();
  if (visibleRows.isEmpty()) {
    QMessageBox::information(
        this, uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"),
        uiText("No visible I/O variables match the current filter.",
               "当前筛选条件下没有可见 I/O 变量。"));
    return;
  }
  const QVector<int> issueRows = plcHandoffIssueRows(visibleRows);
  focusPlcHandoffIssueRows(issueRows, true);

  updateDiagnostics("Info", "I/O Variables",
                    uiText("PLC handoff review opened: %1 issue row(s)",
                           "已打开 PLC 交接复核：%1 条问题行")
                        .arg(issueRows.size()));
}

void MainWindow::focusPlcHandoffIssueRows(const QVector<int> &issueRows,
                                          bool showReadyMessage) {
  if (!ioVariableTable_) {
    return;
  }
  if (ioVariableScopeFilter_) {
    const int index = ioVariableScopeFilter_->findData("plcIssues");
    if (index >= 0) {
      ioVariableScopeFilter_->setCurrentIndex(index);
    }
  }
  if (ioVariableFilter_) {
    ioVariableFilter_->clear();
  }
  filterIoVariableTable();
  activateWorkspaceTab(ioVariableTabIndex_);

  int selectedRow = -1;
  for (const int row : issueRows) {
    if (row >= 0 && row < ioVariableTable_->rowCount() &&
        !ioVariableTable_->isRowHidden(row)) {
      selectedRow = row;
      break;
    }
  }
  if (selectedRow >= 0) {
    ioVariableTable_->setCurrentCell(selectedRow, 15);
    ioVariableTable_->selectRow(selectedRow);
    ioVariableTable_->scrollToItem(ioVariableTable_->item(selectedRow, 15),
                                   QAbstractItemView::PositionAtCenter);
  } else if (showReadyMessage) {
    QMessageBox::information(
        this, uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"),
        uiText("All visible PLC handoff rows are ready.",
               "当前 I/O 变量没有 PLC 交接质量问题。"));
  }
}

void MainWindow::copyIoVariablePlcDeclarations(bool selectedOnly) {
  updateIoVariableTable();
  if (!ioVariableTable_ || ioVariableTable_->rowCount() <= 0) {
    QMessageBox::information(this,
                             uiText("Copy PLC Declarations", "复制 PLC 声明"),
                             uiText("No I/O variables are available.",
                                    "当前没有可复制的 I/O 变量。"));
    return;
  }

  const QVector<int> rows =
      selectedOnly ? selectedIoVariableRows(true) : visibleIoVariableRows();
  if (rows.isEmpty()) {
    QMessageBox::information(
        this, uiText("Copy PLC Declarations", "复制 PLC 声明"),
        selectedOnly
            ? uiText("No selected visible I/O variables are available.",
                     "当前没有选中的可见 I/O 变量。")
            : uiText("No visible I/O variables match the current filter.",
                     "当前筛选条件下没有可见 I/O 变量。"));
    return;
  }
  if (!confirmPlcHandoffOperation(
          rows, uiText("copy PLC declarations", "复制 PLC 声明"),
          uiText("Continue Copy", "继续复制"))) {
    return;
  }

  QApplication::clipboard()->setText(plcDeclarationBlock(rows));
  updateDiagnostics(
      "Info", "I/O Variables",
      uiText("Copied PLC declaration block for %1 I/O variable row(s)",
             "已复制 %1 条 I/O 变量的 PLC 声明块")
          .arg(rows.size()));
}

void MainWindow::clearSelectedIoVariableMetadata() {
  if (!ioVariableTable_) {
    return;
  }
  const QVector<int> rows = selectedIoVariableRows(false);
  int cleared = 0;
  for (const int row : rows) {
    const QString key = ioVariableRowKey(row);
    if (!key.isEmpty() && ioVariableMetadata_.remove(key) > 0) {
      ++cleared;
    }
  }
  if (cleared > 0) {
    updateIoVariableTable();
  }
  updateDiagnostics("Info", "I/O Variables",
                    uiText("Cleared metadata for %1 I/O variable(s)",
                           "已清除 %1 条 I/O 变量元数据")
                        .arg(cleared));
}

void MainWindow::exportIoVariablesCsv() {
  updateIoVariableTable();
  if (!ioVariableTable_ || ioVariableTable_->rowCount() <= 0) {
    QMessageBox::information(this,
                             uiText("Export I/O Variables", "导出 I/O 变量"),
                             uiText("No I/O variables are available to export.",
                                    "当前没有可导出的 I/O 变量。"));
    return;
  }
  const QString path = QFileDialog::getSaveFileName(
      this, uiText("Export I/O Variables CSV", "导出 I/O 变量 CSV"),
      QDir::home().absoluteFilePath(
          QString("ethercat-io-variables-%1.csv")
              .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"))),
      "CSV (*.csv);;Text (*.txt)");
  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, uiText("Export failed", "导出失败"),
                         file.errorString());
    return;
  }
  QTextStream out(&file);
  QStringList headers;
  for (int column = 0; column < ioVariableTable_->columnCount(); ++column) {
    const auto *header = ioVariableTable_->horizontalHeaderItem(column);
    headers.append(csvCell(header ? header->text()
                                  : QString("Column %1").arg(column + 1)));
  }
  out << headers.join(',') << '\n';
  int exported = 0;
  for (int row = 0; row < ioVariableTable_->rowCount(); ++row) {
    if (ioVariableTable_->isRowHidden(row)) {
      continue;
    }
    QStringList cells;
    for (int column = 0; column < ioVariableTable_->columnCount(); ++column) {
      cells.append(csvCell(tableText(ioVariableTable_, row, column)));
    }
    out << cells.join(',') << '\n';
    ++exported;
  }
  updateDiagnostics("Info", "I/O Variables",
                    uiText("Exported %1 visible I/O variable row(s): %2",
                           "已导出 %1 条可见 I/O 变量：%2")
                        .arg(exported)
                        .arg(path));
}

void MainWindow::exportIoVariablesPlcCsv() {
  updateIoVariableTable();
  if (!ioVariableTable_ || ioVariableTable_->rowCount() <= 0) {
    QMessageBox::information(this,
                             uiText("Export PLC Symbols", "导出 PLC 符号"),
                             uiText("No I/O variables are available to export.",
                                    "当前没有可导出的 I/O 变量。"));
    return;
  }
  const QVector<int> rows = visibleIoVariableRows();
  if (rows.isEmpty()) {
    QMessageBox::information(
        this, uiText("Export PLC Symbols", "导出 PLC 符号"),
        uiText("No visible I/O variables match the current filter.",
               "当前筛选条件下没有可见 I/O 变量。"));
    return;
  }
  if (!confirmPlcHandoffOperation(rows,
                                  uiText("export PLC symbols", "导出 PLC 符号"),
                                  uiText("Continue Export", "继续导出"))) {
    return;
  }

  const QString path = QFileDialog::getSaveFileName(
      this, uiText("Export PLC Symbols CSV", "导出 PLC 符号 CSV"),
      QDir::home().absoluteFilePath(
          QString("ethercat-plc-symbols-%1.csv")
              .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"))),
      "CSV (*.csv);;Text (*.txt)");
  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, uiText("Export failed", "导出失败"),
                         file.errorString());
    return;
  }

  QTextStream out(&file);
  QStringList headerCells;
  for (const QString &header : ioVariableHandoffCsvHeaders()) {
    headerCells << csvCell(header);
  }
  out << headerCells.join(',') << '\n';

  const QString exportedAt = QDateTime::currentDateTime().toString(Qt::ISODate);
  int exported = 0;
  QSet<QString> usedSymbols;
  for (const int row : rows) {
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable_, row);
    const QStringList values =
        ioVariableHandoffCsvRow(variable, &usedSymbols, exportedAt).values;
    QStringList cells;
    for (const QString &value : values) {
      cells << csvCell(value);
    }
    out << cells.join(',') << '\n';
    ++exported;
  }

  updateDiagnostics("Info", "I/O Variables",
                    uiText("Exported %1 visible PLC symbol row(s): %2",
                           "已导出 %1 条可见 PLC 符号：%2")
                        .arg(exported)
                        .arg(path));
}

void MainWindow::exportIoVariablesPlcDeclarationsSt() {
  updateIoVariableTable();
  if (!ioVariableTable_ || ioVariableTable_->rowCount() <= 0) {
    QMessageBox::information(this,
                             uiText("Export PLC Declarations", "导出 PLC 声明"),
                             uiText("No I/O variables are available to export.",
                                    "当前没有可导出的 I/O 变量。"));
    return;
  }
  const QVector<int> rows = visibleIoVariableRows();
  if (rows.isEmpty()) {
    QMessageBox::information(
        this, uiText("Export PLC Declarations", "导出 PLC 声明"),
        uiText("No visible I/O variables match the current filter.",
               "当前筛选条件下没有可见 I/O 变量。"));
    return;
  }
  if (!confirmPlcHandoffOperation(
          rows, uiText("export PLC declarations", "导出 PLC 声明"),
          uiText("Continue Export", "继续导出"))) {
    return;
  }

  const QString path = QFileDialog::getSaveFileName(
      this, uiText("Export PLC Declarations ST", "导出 PLC 声明 ST"),
      QDir::home().absoluteFilePath(
          QString("ethercat-plc-declarations-%1.st")
              .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"))),
      "Structured Text (*.st);;Text (*.txt)");
  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, uiText("Export failed", "导出失败"),
                         file.errorString());
    return;
  }

  QTextStream out(&file);
  out << plcDeclarationBlock(rows) << '\n';
  updateDiagnostics("Info", "I/O Variables",
                    uiText("Exported PLC declaration ST for %1 visible row(s): "
                           "%2",
                           "已导出 %1 条可见行的 PLC 声明 ST：%2")
                        .arg(rows.size())
                        .arg(path));
}

bool MainWindow::confirmPlcHandoffOperation(const QVector<int> &rows,
                                            const QString &operation,
                                            const QString &continueText) {
  const QVector<int> issueRows = plcHandoffIssueRows(rows);
  if (issueRows.isEmpty()) {
    return true;
  }

  QMessageBox box(this);
  box.setIcon(QMessageBox::Warning);
  box.setWindowTitle(uiText("PLC Handoff Not Ready", "PLC 交接尚未就绪"));
  box.setText(uiText("The selected PLC handoff scope still contains quality "
                     "issues before %1.",
                     "%1 前，当前 PLC 交接范围仍存在质量问题。")
                  .arg(operation));
  QStringList details = plcHandoffIssueDetails(issueRows, 8, rows.size());
  details << uiText("Continue keeps Quality evidence attached to the handoff "
                    "output; Review Issues cancels this operation and opens "
                    "I/O Variables.",
                    "继续会把 Quality 证据保留在交接输出中；审阅问题会取消本次"
                    "操作并打开 I/O 变量页。");
  box.setInformativeText(details.join('\n'));
  box.setStandardButtons(QMessageBox::Cancel);
  auto *review = box.addButton(uiText("Review Issues", "审阅问题"),
                               QMessageBox::ActionRole);
  auto *continueOperation =
      box.addButton(continueText, QMessageBox::AcceptRole);
  box.setDefaultButton(QMessageBox::Cancel);
  box.exec();

  if (box.clickedButton() == continueOperation) {
    updateDiagnostics(
        "Warning", "I/O Variables",
        uiText("PLC handoff operation continued with %1 issue row(s): %2",
               "PLC 交接操作在 %1 条问题未清理时继续：%2")
            .arg(issueRows.size())
            .arg(operation));
    return true;
  }
  if (box.clickedButton() == review) {
    updateDiagnostics(
        "Info", "I/O Variables",
        uiText("PLC handoff operation paused for issue review: %1",
               "PLC 交接操作已暂停，进入问题复核：%1")
            .arg(operation));
    focusPlcHandoffIssueRows(issueRows, false);
    return false;
  }

  updateDiagnostics(
      "Info", "I/O Variables",
      uiText("PLC handoff operation cancelled: %1", "PLC 交接操作已取消：%1")
          .arg(operation));
  return false;
}

void MainWindow::updateIoVariableTable() {
  if (!ioVariableTable_) {
    return;
  }
  consistencyFresh_ = false;

  const int previousRow = ioVariableTable_->currentRow();
  const int verticalScroll =
      ioVariableTable_->verticalScrollBar()
          ? ioVariableTable_->verticalScrollBar()->value()
          : 0;
  const int horizontalScroll =
      ioVariableTable_->horizontalScrollBar()
          ? ioVariableTable_->horizontalScrollBar()->value()
          : 0;

  const QStringList headers = {
      uiText("Slave", "从站"),   uiText("Dir", "方向"),
      uiText("Symbol", "符号"),  uiText("Index", "索引"),
      uiText("Sub", "子项"),     uiText("Bits", "位宽"),
      uiText("PDO", "PDO"),      uiText("Source", "来源"),
      uiText("Raw", "原始值"),   uiText("Decoded", "解码"),
      uiText("Meaning", "含义"), uiText("Watch", "Watch"),
      uiText("Startup", "启动"), uiText("Map", "映射"),
      uiText("Changed", "变化"), uiText("PLC", "PLC"),
      uiText("Alias", "别名"),   uiText("Tags", "标签"),
      uiText("Note", "备注")};

  auto directionText = [this](QString text) {
    const QString normalized = text.trimmed().toLower();
    if (normalized.contains("rx") || normalized.contains("output") ||
        normalized == "out") {
      return uiText("Rx Output", "Rx 输出");
    }
    if (normalized.contains("tx") || normalized.contains("input") ||
        normalized == "in") {
      return uiText("Tx Input", "Tx 输入");
    }
    return text.trimmed().isEmpty() ? uiText("SDO", "SDO") : text.trimmed();
  };
  auto inferredType = [](QString bitsText) {
    bool ok = false;
    const int bits = bitsText.trimmed().toInt(&ok);
    if (!ok || bits <= 0) {
      return QString();
    }
    if (bits == 1) {
      return QStringLiteral("bool");
    }
    if (bits <= 8) {
      return QStringLiteral("uint8");
    }
    if (bits <= 16) {
      return QStringLiteral("uint16");
    }
    if (bits <= 32) {
      return QStringLiteral("uint32");
    }
    return QStringLiteral("uint64");
  };

  QHash<QString, QStringList> watchByObject;
  if (watchTable_) {
    ensureWatchTable();
    for (int row = 0; row < watchTable_->rowCount(); ++row) {
      bool ok = false;
      const int position = tableText(watchTable_, row, 1).toInt(&ok);
      const QString index = tableText(watchTable_, row, 2);
      const QString subIndex = tableText(watchTable_, row, 3);
      if (!ok || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
        continue;
      }
      const QString key = ioVariableTableObjectKey(position, index, subIndex);
      const bool changed = watchChangedKeys_.contains(key);
      watchByObject.insert(
          key, {tableText(watchTable_, row, 4), tableText(watchTable_, row, 5),
                tableText(watchTable_, row, 6), tableText(watchTable_, row, 7),
                changed ? uiText("Yes", "是") : QString()});
    }
  }

  QHash<QString, QStringList> startupByObject;
  if (startupSdoTable_) {
    ensureStartupSdoTable();
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
      bool ok = false;
      const int position = tableText(startupSdoTable_, row, 0).toInt(&ok);
      const QString index = tableText(startupSdoTable_, row, 1);
      const QString subIndex = tableText(startupSdoTable_, row, 2);
      if (!ok || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
        continue;
      }
      startupByObject.insert(
          ioVariableTableObjectKey(position, index, subIndex),
          {tableText(startupSdoTable_, row, 3),
           tableText(startupSdoTable_, row, 8)});
    }
  }

  QList<QStringList> rows;
  QSet<QString> coveredObjects;
  auto appendRow = [&](int position, const QString &direction,
                       const QString &symbol, const QString &index,
                       const QString &subIndex, const QString &bits,
                       const QString &pdo, const QString &source, QString raw,
                       QString decoded, QString meaning,
                       const QString &mapStatus, bool changed) {
    if (position < 0 || index.trimmed().isEmpty() ||
        subIndex.trimmed().isEmpty()) {
      return;
    }
    const QString normalizedIndex = normalizeHexText(index, 4);
    const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
    const QString key =
        ioVariableTableObjectKey(position, normalizedIndex, normalizedSubIndex);
    const QStringList watch = watchByObject.value(key);
    const QStringList startup = startupByObject.value(key);
    const QString watchValue = watch.value(0);
    if (raw.trimmed().isEmpty() && source == uiText("Watch", "Watch")) {
      raw = watchValue;
    }
    if (decoded.trimmed().isEmpty()) {
      decoded = watch.value(1);
    }
    if (meaning.trimmed().isEmpty()) {
      meaning = decoded;
    }
    const QString type =
        watch.value(2).isEmpty() ? inferredType(bits) : watch.value(2);
    const QStringList metadata = ioVariableMetadata_.value(key);
    QString startupText;
    if (!startup.isEmpty()) {
      startupText =
          startup.value(1).isEmpty()
              ? startup.value(0)
              : QString("%1 | %2").arg(startup.value(0), startup.value(1));
    }
    const bool changedEvidence = changed || !watch.value(4).isEmpty();
    const QString sourceText =
        type.isEmpty() ? source : QString("%1 | %2").arg(source, type);
    rows.append({QString::number(position), directionText(direction),
                 symbol.trimmed().isEmpty()
                     ? QString("%1:%2").arg(normalizedIndex, normalizedSubIndex)
                     : symbol.trimmed(),
                 normalizedIndex, normalizedSubIndex, bits.trimmed(),
                 pdo.trimmed(), sourceText, raw.trimmed(), decoded.trimmed(),
                 meaning.trimmed(), watchValue, startupText,
                 mapStatus.trimmed(),
                 changedEvidence ? uiText("Yes", "是") : QString(), QString(),
                 metadata.value(0), metadata.value(1), metadata.value(2)});
    coveredObjects.insert(key);
  };

  if (freeRunEntryTable_) {
    for (int row = 0; row < freeRunEntryTable_->rowCount(); ++row) {
      bool ok = false;
      const int position = tableText(freeRunEntryTable_, row, 0).toInt(&ok);
      if (!ok) {
        continue;
      }
      const auto *changedItem = freeRunEntryTable_->item(row, 0);
      appendRow(position, tableText(freeRunEntryTable_, row, 2),
                tableText(freeRunEntryTable_, row, 9),
                tableText(freeRunEntryTable_, row, 4),
                tableText(freeRunEntryTable_, row, 5),
                tableText(freeRunEntryTable_, row, 6),
                tableText(freeRunEntryTable_, row, 3),
                uiText("Process", "过程"),
                tableText(freeRunEntryTable_, row, 10),
                tableText(freeRunEntryTable_, row, 11),
                tableText(freeRunEntryTable_, row, 12),
                tableText(freeRunEntryTable_, row, 13),
                changedItem && changedItem->data(Qt::UserRole).toBool());
    }
  }

  if (loadedPdoPosition_ == selectedPosition() && pdoTable_) {
    for (int row = 0; row < pdoTable_->rowCount(); ++row) {
      const int position = selectedPosition();
      const QString key = ioVariableTableObjectKey(
          position, tableText(pdoTable_, row, 2), tableText(pdoTable_, row, 3));
      if (coveredObjects.contains(key)) {
        continue;
      }
      appendRow(position, tableText(pdoTable_, row, 1),
                tableText(pdoTable_, row, 5), tableText(pdoTable_, row, 2),
                tableText(pdoTable_, row, 3), tableText(pdoTable_, row, 4),
                tableText(pdoTable_, row, 1), uiText("PDO", "PDO"), QString(),
                QString(), QString(), uiText("PDO loaded", "PDO 已加载"),
                false);
    }
  }

  for (auto it = watchByObject.cbegin(); it != watchByObject.cend(); ++it) {
    const QStringList parts = it.key().split('|');
    if (parts.size() != 3 || coveredObjects.contains(it.key())) {
      continue;
    }
    bool ok = false;
    const int position = parts.value(0).toInt(&ok);
    if (!ok) {
      continue;
    }
    appendRow(position, uiText("SDO", "SDO"), QString(), parts.value(1),
              parts.value(2), QString(), QString(), uiText("Watch", "Watch"),
              it.value().value(0), it.value().value(1), it.value().value(3),
              QString(), !it.value().value(4).isEmpty());
  }

  setTableRows(ioVariableTable_, headers, rows);
  const QColor changedBackground =
      settings_.theme == "Light" ? QColor("#fff7cc") : QColor("#3a2f16");
  const QColor warningBackground = changedBackground;
  const QColor diffBackground =
      settings_.theme == "Light" ? QColor("#fee2e2") : QColor("#3a1218");
  const QColor okColor("#22c55e");
  const QColor warnColor("#f59e0b");
  const QColor errorColor("#ef4444");
  const QSet<QString> duplicateSymbols = duplicateIoVariablePlcSymbols();
  for (int row = 0; row < ioVariableTable_->rowCount(); ++row) {
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable_, row);
    const QString plcQuality =
        ioVariablePlcQuality(row, &duplicateSymbols, nullptr);
    const bool plcReady = plcQuality == uiText("Ready", "就绪");
    const bool hasAlias = !variable.alias.isEmpty();
    if (auto *item = ioVariableTable_->item(row, 7)) {
      item->setForeground(ioVariableTableRowHasProcessSource(variable)
                              ? okColor
                              : (ioVariableTableRowHasPdoSource(variable)
                                     ? QColor("#60a5fa")
                                     : warnColor));
    }
    if (auto *item = ioVariableTable_->item(row, 12)) {
      if (ioVariableTableRowHasStartupDiff(variable)) {
        item->setBackground(diffBackground);
        item->setForeground(errorColor);
      }
    }
    if (auto *item = ioVariableTable_->item(row, 13)) {
      item->setForeground(ioVariableTableRowHasPdoMapIssue(variable) ? warnColor
                                                                     : okColor);
    }
    if (ioVariableTableRowHasChangedValue(variable)) {
      if (auto *item = ioVariableTable_->item(row, 14)) {
        item->setBackground(changedBackground);
        item->setForeground(settings_.theme == "Light" ? QColor("#854d0e")
                                                       : QColor("#fde68a"));
      }
    }
    if (auto *item = ioVariableTable_->item(row, 15)) {
      item->setText(plcQuality);
      item->setForeground(plcReady ? okColor : warnColor);
      if (!plcReady) {
        item->setBackground(warningBackground);
      }
    }
    if (hasAlias) {
      if (auto *item = ioVariableTable_->item(row, 16)) {
        item->setForeground(QColor("#22c55e"));
      }
    }
    if (auto *item = ioVariableTable_->item(row, 17)) {
      if (!item->text().trimmed().isEmpty()) {
        item->setForeground(QColor("#60a5fa"));
      }
    }
  }
  ioVariableTable_->resizeColumnsToContents();
  if (previousRow >= 0 && previousRow < ioVariableTable_->rowCount()) {
    ioVariableTable_->setCurrentCell(previousRow, 0);
  }
  if (ioVariableTable_->verticalScrollBar()) {
    ioVariableTable_->verticalScrollBar()->setValue(verticalScroll);
  }
  if (ioVariableTable_->horizontalScrollBar()) {
    ioVariableTable_->horizontalScrollBar()->setValue(horizontalScroll);
  }
  filterIoVariableTable();
}

void MainWindow::filterIoVariableTable() {
  if (!ioVariableTable_) {
    return;
  }
  const QString needle =
      ioVariableFilter_ ? ioVariableFilter_->text().trimmed() : QString();
  const QString scope = ioVariableScopeFilter_
                            ? ioVariableScopeFilter_->currentData().toString()
                            : QStringLiteral("all");
  const int selected = selectedPosition();
  IoVariableFilterStats stats;
  const QString readyText = uiText("Ready", "就绪");

  for (int row = 0; row < ioVariableTable_->rowCount(); ++row) {
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable_, row);
    QStringList cells;
    cells.reserve(ioVariableTable_->columnCount());
    for (int column = 0; column < ioVariableTable_->columnCount(); ++column) {
      const auto *item = ioVariableTable_->item(row, column);
      cells << (item ? item->text() : QString());
    }
    const IoVariableFilterDecision decision = evaluateIoVariableFilterRow(
        variable, cells, scope, needle, selected, readyText);
    ioVariableTable_->setRowHidden(row, !decision.visible);
    accumulateIoVariableFilterStats(&stats, decision);
  }

  if (ioVariableSummaryLabel_) {
    const QString scopeLabel = ioVariableScopeFilter_
                                   ? ioVariableScopeFilter_->currentText()
                                   : uiText("All", "全部");
    const QString summaryPattern = uiText(
        "%1/%2 | %3 | process %4 | watch %5 | startup diff %6 | missing %7 | "
        "changed %8 | plc issues %9",
        "%1/%2 | %3 | 过程 %4 | Watch %5 | 启动偏差 %6 | 缺失 %7 | 变化 %8 | "
        "PLC 问题 %9");
    ioVariableSummaryLabel_->setText(
        ioVariableFilterSummaryText(stats, scopeLabel, summaryPattern));
    ioVariableSummaryLabel_->setToolTip(uiText(
        "I/O Variables merges PDO Map, Free Run process image, Watch values, "
        "and Startup SDO expectations into one engineering signal table.",
        "I/O 变量把 PDO 映射、Free Run 过程映像、Watch 值和 Startup SDO 期望"
        "合并到一张工程信号表。"));
  }
  updateIoVariableRowDetail();
  updateActionAvailability();
}

void MainWindow::updateIoVariableRowDetail() {
  if (!ioVariableDetailLabel_) {
    return;
  }
  const IoVariableDetailTexts texts = {
      .unavailableText = uiText("I/O variable evidence is not available.",
                                "当前没有可用的 I/O 变量证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select a visible I/O variable to review signal source, value "
          "evidence, Startup comparison, PDO map status, PLC quality, and "
          "operation boundary.",
          "选择一条可见 I/O 变量，以复核信号来源、值证据、Startup 对照、PDO "
          "映射"
          "状态、PLC 质量和操作边界。"),
      .noSelectionTip = uiText(
          "Selecting rows, changing scopes, filtering, and reading this detail "
          "strip are local review actions. Read SDO is the explicit SDO read "
          "path; Watch and Startup buttons only edit local tables until their "
          "own explicit actions are used.",
          "选择行、切换范围、筛选和查看此详情条都是本地审阅动作；读取 SDO 才是"
          "显式 SDO 读取路径；Watch 和 Startup 按钮只编辑本地表格，直到使用各自"
          "的显式动作。"),
      .readyText = uiText("Ready", "就绪"),
      .noValue = uiText("No value", "无值"),
      .directionFallback = uiText("Direction?", "方向?"),
      .unnamedSignal = uiText("Unnamed signal", "未命名信号"),
      .noComparison = uiText("No comparison", "无对照"),
      .noMapEvidence = uiText("No map evidence", "无映射证据"),
      .notReviewed = uiText("Not reviewed", "未审阅"),
      .startupMismatch = uiText("Startup mismatch", "Startup 不一致"),
      .mapIssue = uiText("Map issue", "映射问题"),
      .plcReview = uiText("PLC review", "PLC 待审阅"),
      .missingValue = uiText("Missing value", "缺失值"),
      .changed = uiText("Changed", "已变化"),
      .readyEvidence = uiText("Ready evidence", "证据就绪"),
      .summaryPattern = uiText("#%1 %2:%3 | %4 | %5 | Value: %6 | Startup: "
                               "%7 | Map: %8 | PLC: %9",
                               "#%1 %2:%3 | %4 | %5 | 值：%6 | Startup：%7 | "
                               "映射：%8 | PLC：%9"),
      .selectedTitle = uiText("Selected I/O variable", "选中的 I/O 变量"),
      .slaveLabel = uiText("Slave", "从站"),
      .directionLabel = uiText("Direction", "方向"),
      .symbolLabel = uiText("Symbol", "符号"),
      .aliasLabel = uiText("Alias", "别名"),
      .objectLabel = uiText("Object", "对象"),
      .bitsLabel = uiText("Bits", "位宽"),
      .pdoLabel = uiText("PDO", "PDO"),
      .sourceLabel = uiText("Source", "来源"),
      .rawLabel = uiText("Raw", "原始值"),
      .decodedLabel = uiText("Decoded", "解码"),
      .meaningLabel = uiText("Meaning", "含义"),
      .watchLabel = uiText("Watch", "Watch"),
      .startupLabel = uiText("Startup", "启动"),
      .mapLabel = uiText("Map", "映射"),
      .changedLabel = uiText("Changed", "变化"),
      .plcLabel = uiText("PLC", "PLC"),
      .tagsLabel = uiText("Tags", "标签"),
      .noteLabel = uiText("Note", "备注"),
      .signalStateLabel = uiText("Signal State", "信号状态"),
      .localBoundary = uiText(
          "Local preview boundary: selecting this row, changing I/O scopes, "
          "filtering, copying declarations, exporting visible rows, editing "
          "Alias/Tags/Note, and reading this detail strip do not read SDOs, "
          "write SDOs, change state, toggle Free Run, or run Host Health.",
          "本地预览边界：选择此行、切换 I/O 范围、筛选、复制声明、导出可见行、"
          "编辑别名/标签/备注和查看此详情条都不读取 SDO、不写 SDO、不切换状态、"
          "不改变 Free Run，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Fill SDO only prepares the local target; Read "
          "SDO and double-click issue explicit SDO reads; Watch buttons create "
          "Watch rows without immediate reads; Startup buttons edit the "
          "Startup table until Startup Apply is confirmed.",
          "执行边界：填充 SDO 只准备本地目标；读取 SDO 和双击会显式读取 SDO；"
          "Watch 按钮只创建 Watch 行且不立即读取；Startup 按钮只编辑 Startup "
          "表，"
          "直到确认应用启动项。"),
  };

  auto applyState = [this](const IoVariableDetailUiState &state) {
    ioVariableDetailLabel_->setText(state.text);
    ioVariableDetailLabel_->setProperty("severity", state.severityKey);
    ioVariableDetailLabel_->setToolTip(state.tooltip);
    repolish(ioVariableDetailLabel_);
  };

  if (!ioVariableTable_) {
    applyState(ioVariableDetailUnavailableState(texts));
    return;
  }

  const int row = ioVariableTable_->currentRow();
  if (row < 0 || row >= ioVariableTable_->rowCount() ||
      ioVariableTable_->isRowHidden(row)) {
    applyState(ioVariableDetailNoSelectionState(texts));
    return;
  }

  const IoVariableTableRow variable =
      ioVariableTableRowFromTable(ioVariableTable_, row);
  applyState(buildIoVariableDetailUiState(variable, texts));
}

