// I/O variable table, PLC handoff, and export functions.

#include "MainWindow.h"

#include "models/Cia402DriveModel.h"
#include "models/CommissioningWorkflowModel.h"
#include "detail/CommissioningWorkflowStepDetail.h"
#include "adapters/CommissioningWorkflowTableAdapter.h"
#include "detail/CommissioningWorkflowDetail.h"
#include "detail/ConsistencyDetail.h"
#include "models/ConsistencyEvidenceRouteModel.h"
#include "models/ConsistencyGateModel.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "detail/DiagnosticsEventDetail.h"
#include "models/EvidenceStatusModel.h"
#include "detail/FreeRunEntryDetail.h"
#include "detail/HostHealthDetail.h"
#include "models/IoVariableBulkNamingModel.h"
#include "detail/IoVariableDetail.h"
#include "models/IoVariableFilterModel.h"
#include "models/IoVariableHandoffModel.h"
#include "models/NextBestActionModel.h"
#include "detail/NextBestActionDetail.h"
#include "detail/ObjectBookmarkDetail.h"
#include "detail/PdoMapDetail.h"
#include "models/ProcessDataRowModel.h"
#include "adapters/ProcessDataTableAdapter.h"
#include "adapters/SdoDictionaryTableAdapter.h"
#include "models/SdoEvidenceModel.h"
#include "adapters/SdoEvidenceTableAdapter.h"
#include "detail/SdoHistoryRowDetail.h"
#include "models/SdoTargetPanelRouteModel.h"
#include "detail/SdoTargetTrailDetail.h"
#include "detail/SelectedDriveSummaryDetail.h"
#include "detail/SelectedSlaveEvidenceSummaryDetail.h"
#include "models/SessionBriefModel.h"
#include "adapters/SessionBriefTableAdapter.h"
#include "detail/SessionBriefDetail.h"
#include "models/SlaveEvidenceModel.h"
#include "adapters/SlaveEvidenceTableAdapter.h"
#include "detail/SlaveEvidenceDetail.h"
#include "detail/StartupSdoRowDetail.h"
#include "detail/StateMachineRowDetail.h"
#include "adapters/StateMachineTableAdapter.h"
#include "models/StateRecommendationModel.h"
#include "helpers/StudioDocumentation.h"
#include "helpers/StudioTableHelpers.h"
#include "helpers/StudioTextHelpers.h"
#include "helpers/StudioUiHelpers.h"
#include "models/TopologyBaselineModel.h"
#include "models/TopologyChangeModel.h"
#include "detail/WatchRowDetail.h"
#include "models/WatchStartupModel.h"
#include "adapters/WatchStartupTableAdapter.h"
#include "detail/WatchStartupDetail.h"
#include "detail/WorkspaceBoundaryDetail.h"
#include "adapters/WorkspaceTabBadgeTableAdapter.h"
#include "detail/WorkspaceTabBadgeDetail.h"
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
    // Serialize/deserialize JSON data
#include <QJsonArray>
#include <QJsonDocument>
    // Serialize/deserialize JSON data
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
    // Schedule deferred or periodic execution
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QXmlStreamReader>


// — Return the unique key for an I/O variable table row
QString MainWindow::ioVariableRowKey(int row) const {
  return ioVariableTableRowKey(ioVar_->ioVariableTable, row);
}


// — Return the list of currently selected I/O variable row indices
QVector<int> MainWindow::selectedIoVariableRows(bool visibleOnly) const {
  return selectedIoVariableTableRows(ioVar_->ioVariableTable, visibleOnly);
}


// — Return the list of non-hidden I/O variable row indices
QVector<int> MainWindow::visibleIoVariableRows() const {
  return visibleIoVariableTableRows(ioVar_->ioVariableTable);
}


// — Map an internal handoff issue key to a localized display label
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


// — Return the io variable plc quality
QString MainWindow::ioVariablePlcQuality(int row,
                                         const QSet<QString> *duplicateSymbols,
                                         QString *symbol) const {
  if (!ioVar_->ioVariableTable || row < 0 || row >= ioVar_->ioVariableTable->rowCount()) {
    return QString();
  }

  const IoVariableTableRow variable =
      ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
  const IoVariableHandoffName name = ioVariableHandoffName(variable);
  if (symbol) {
    *symbol = name.symbol;
  }

  const QStringList keys = ioVariableHandoffIssueKeys(
      ioVariableHandoffIssues(variable, duplicateSymbols));
  return keys.isEmpty() ? uiText("Ready", "就绪")
                        : ioVariableHandoffIssueLabels(keys).join(" | ");
}


// — Duplicate io variable plc symbols
QSet<QString> MainWindow::duplicateIoVariablePlcSymbols() const {
  if (!ioVar_->ioVariableTable) {
    return {};
  }
  QVector<IoVariableTableRow> rows;
  rows.reserve(ioVar_->ioVariableTable->rowCount());
  // Iterate all rows and apply active filter predicates
  for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
    rows.append(ioVariableTableRowFromTable(ioVar_->ioVariableTable, row));
  }
  return duplicateIoVariableHandoffSymbols(rows);
}


// — Return plc handoff issue rows
QVector<int> MainWindow::plcHandoffIssueRows(const QVector<int> &rows) const {
  QVector<int> issueRows;
  if (!ioVar_->ioVariableTable) {
    return issueRows;
  }

  const QSet<QString> duplicateSymbols = duplicateIoVariablePlcSymbols();
  for (const int row : rows) {
    if (row < 0 || row >= ioVar_->ioVariableTable->rowCount()) {
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


// — Return a list of plc handoff issue details
QStringList MainWindow::plcHandoffIssueDetails(const QVector<int> &rows,
                                               int previewLimit,
                                               int totalRows) const {
  QStringList details;
  if (!ioVar_->ioVariableTable || rows.isEmpty()) {
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
    if (row < 0 || row >= ioVar_->ioVariableTable->rowCount()) {
      continue;
    }
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
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
                                     : ioVar_->ioVariableTable->rowCount());
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


// — Return the plc declaration block
QString MainWindow::plcDeclarationBlock(const QVector<int> &rows) const {
  if (!ioVar_->ioVariableTable || rows.isEmpty()) {
    return QString();
  }

  const QSet<QString> duplicateSymbols = duplicateIoVariablePlcSymbols();
  QVector<IoVariableTableRow> variables;
  QVector<QStringList> qualityLabelsByRow;
  variables.reserve(rows.size());
  qualityLabelsByRow.reserve(rows.size());
  for (const int row : rows) {
    if (row < 0 || row >= ioVar_->ioVariableTable->rowCount()) {
      continue;
    }

    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
    const QString quality = ioVariablePlcQuality(row, &duplicateSymbols);
    variables << variable;
    qualityLabelsByRow << (quality == uiText("Ready", "就绪")
                               ? QStringList()
                               : quality.split(" | "));
  }
  return ioVariableHandoffDeclarationBlock(variables, qualityLabelsByRow);
}


// — Edit selected io variable metadata
void MainWindow::editSelectedIoVariableMetadata() {
  if (!ioVar_->ioVariableTable) {
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
      ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
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
    // Connect QDialogButtonBox::accepted signal to handler
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept); // wire signal to slot
    // Connect QDialogButtonBox::rejected signal to handler
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); // wire signal to slot
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


// Apply bulk naming rules to rename selected I/O variables using the configured pattern
void MainWindow::bulkNameIoVariables() {
  if (!ioVar_->ioVariableTable || ioVar_->ioVariableTable->rowCount() <= 0) {
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
    // Connect QDialogButtonBox::accepted signal to handler
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept); // wire signal to slot
    // Connect QDialogButtonBox::rejected signal to handler
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); // wire signal to slot
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
  allRows.reserve(ioVar_->ioVariableTable->rowCount());
  // Iterate all rows and apply active filter predicates
  for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
    allRows << ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
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
    // Safety gate: require explicit confirmation before bus write
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


// — Review plc handoff issues
void MainWindow::reviewPlcHandoffIssues() {
  updateIoVariableTable();
  if (!ioVar_->ioVariableTable || ioVar_->ioVariableTable->rowCount() <= 0) {
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


// — Focus plc handoff issue rows
void MainWindow::focusPlcHandoffIssueRows(const QVector<int> &issueRows,
                                          bool showReadyMessage) {
  if (!ioVar_->ioVariableTable) {
    return;
  }
  if (ioVar_->ioVariableScopeFilter) {
    const int index = ioVar_->ioVariableScopeFilter->findData("plcIssues");
    if (index >= 0) {
      ioVar_->ioVariableScopeFilter->setCurrentIndex(index);
    }
  }
  if (ioVar_->ioVariableFilter) {
    ioVar_->ioVariableFilter->clear();
  }
  filterIoVariableTable();
  activateWorkspaceTab(ioVariableTabIndex_);

  int selectedRow = -1;
  for (const int row : issueRows) {
    if (row >= 0 && row < ioVar_->ioVariableTable->rowCount() &&
        !ioVar_->ioVariableTable->isRowHidden(row)) {
      selectedRow = row;
      break;
    }
  }
  if (selectedRow >= 0) {
    ioVar_->ioVariableTable->setCurrentCell(selectedRow, 15);
    ioVar_->ioVariableTable->selectRow(selectedRow);
    ioVar_->ioVariableTable->scrollToItem(ioVar_->ioVariableTable->item(selectedRow, 15),
                                   QAbstractItemView::PositionAtCenter);
  } else if (showReadyMessage) {
    QMessageBox::information(
        this, uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"),
        uiText("All visible PLC handoff rows are ready.",
               "当前 I/O 变量没有 PLC 交接质量问题。"));
  }
}


// — Copy IEC 61131-3 style declarations for selected I/O variables
void MainWindow::copyIoVariablePlcDeclarations(bool selectedOnly) {
  updateIoVariableTable();
  if (!ioVar_->ioVariableTable || ioVar_->ioVariableTable->rowCount() <= 0) {
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

  QApplication::clipboard()->setText(plcDeclarationBlock(rows)); // copy to system clipboard
  updateDiagnostics(
      "Info", "I/O Variables",
      uiText("Copied PLC declaration block for %1 I/O variable row(s)",
             "已复制 %1 条 I/O 变量的 PLC 声明块")
          .arg(rows.size()));
}


// — Clear selected io variable metadata
void MainWindow::clearSelectedIoVariableMetadata() {
  if (!ioVar_->ioVariableTable) {
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


// Export the I/O variable table to a CSV file for external tooling
void MainWindow::exportIoVariablesCsv() {
  updateIoVariableTable();
  if (!ioVar_->ioVariableTable || ioVar_->ioVariableTable->rowCount() <= 0) {
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
  for (int column = 0; column < ioVar_->ioVariableTable->columnCount(); ++column) {
    const auto *header = ioVar_->ioVariableTable->horizontalHeaderItem(column);
    headers.append(csvCell(header ? header->text()
                                  : QString("Column %1").arg(column + 1)));
  }
  out << headers.join(',') << '\n';
  int exported = 0;
  // Iterate all rows and apply active filter predicates
  for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
    if (ioVar_->ioVariableTable->isRowHidden(row)) {
      continue;
    }
    QStringList cells;
    for (int column = 0; column < ioVar_->ioVariableTable->columnCount(); ++column) {
      cells.append(csvCell(tableText(ioVar_->ioVariableTable, row, column)));
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


// — Export io variables plc csv
void MainWindow::exportIoVariablesPlcCsv() {
  updateIoVariableTable();
  if (!ioVar_->ioVariableTable || ioVar_->ioVariableTable->rowCount() <= 0) {
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
        ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
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


// — Export io variables plc declarations st
void MainWindow::exportIoVariablesPlcDeclarationsSt() {
  updateIoVariableTable();
  if (!ioVar_->ioVariableTable || ioVar_->ioVariableTable->rowCount() <= 0) {
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


// — Check whether confirm plc handoff operation
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


