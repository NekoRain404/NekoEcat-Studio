// Main application window: workspace tabs, toolbars, wiring, and all workspace methods.
#include "MainWindow.h"
#include "Cia402DriveModel.h"
#include "CommissioningWorkflowModel.h"
#include "CommissioningWorkflowStepDetailUiState.h"
#include "CommissioningWorkflowTableAdapter.h"
#include "CommissioningWorkflowUiState.h"
#include "ConsistencyDetailUiState.h"
#include "ConsistencyEvidenceRouteModel.h"
#include "ConsistencyGateModel.h"
#include "ConsistencyTableAdapter.h"
#include "DiagnosticsEventUiState.h"
#include "EvidenceStatusModel.h"
#include "FreeRunEntryDetailUiState.h"
#include "HostHealthUiState.h"
#include "IoVariableBulkNamingModel.h"
#include "IoVariableDetailUiState.h"
#include "IoVariableFilterModel.h"
#include "IoVariableHandoffModel.h"
#include "NextBestActionModel.h"
#include "NextBestActionUiState.h"
#include "ObjectBookmarkDetailUiState.h"
#include "PdoMapDetailUiState.h"
#include "ProcessDataRowModel.h"
#include "ProcessDataTableAdapter.h"
#include "SdoDictionaryTableAdapter.h"
#include "SdoEvidenceModel.h"
#include "SdoEvidenceTableAdapter.h"
#include "SdoHistoryRowDetailUiState.h"
#include "SdoTargetPanelRouteModel.h"
#include "SdoTargetTrailDetailUiState.h"
#include "SelectedDriveSummaryUiState.h"
#include "SelectedSlaveEvidenceSummaryUiState.h"
#include "SessionBriefModel.h"
#include "SessionBriefTableAdapter.h"
#include "SessionBriefUiState.h"
#include "SlaveEvidenceModel.h"
#include "SlaveEvidenceTableAdapter.h"
#include "SlaveEvidenceUiState.h"
#include "StartupSdoRowDetailUiState.h"
#include "StateMachineRowDetailUiState.h"
#include "StateMachineTableAdapter.h"
#include "StateRecommendationModel.h"
#include "StudioDocumentation.h"
#include "StudioTableHelpers.h"
#include "StudioTextHelpers.h"
#include "StudioUiHelpers.h"
#include "TopologyBaselineModel.h"
#include "TopologyChangeModel.h"
#include "WatchRowDetailUiState.h"
#include "WatchStartupModel.h"
#include "WatchStartupTableAdapter.h"
#include "WatchStartupUiState.h"
#include "WorkspaceBoundaryUiState.h"
#include "WorkspaceTabBadgeTableAdapter.h"
#include "WorkspaceTabBadgeUiState.h"

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

void MainWindow::log(const QString &message) {
  rawText_->logText->appendPlainText(QString("[%1] %2").arg(
      QDateTime::currentDateTime().toString("HH:mm:ss"), message));
}

// Return the currently selected slave position, or -1 if none
int MainWindow::selectedPosition() const {
  auto *item = topologyTree_->currentItem();
  return item ? item->data(0, Qt::UserRole).toInt() : -1;
}

// Return the currently selected Object Dictionary row indices
QVector<int> MainWindow::selectedDictionaryRows() const {
  if (!sdo_->sdoTable || selectedPosition() < 0 ||
      loadedSdoPosition_ != selectedPosition()) {
    return {};
  }
  return selectedTableRows(sdo_->sdoTable);
}

// Return the currently selected SDO history row indices
QVector<int> MainWindow::selectedSdoHistoryRows() const {
  return selectedTableRows(sdoHistoryTable_);
}

// Return the currently selected Startup SDO row indices
QVector<int> MainWindow::selectedStartupSdoRows() const {
  return selectedTableRows(startupSdoTable_, true);
}

// Check if a watch table row has a non-empty value
bool MainWindow::watchRowHasValue(int row) const {
  return watch_->watchTable && row >= 0 && row < watch_->watchTable->rowCount() &&
         !watch_->watchTable->isRowHidden(row) && watch_->watchTable->item(row, 4) &&
         !watch_->watchTable->item(row, 4)->text().trimmed().isEmpty();
}

// Check if any of the selected watch rows contain a value
bool MainWindow::selectedWatchRowsHaveValue() const {
  if (!watch_->watchTable) {
    return false;
  }
  for (const int row : selectedTableRows(watch_->watchTable, true)) {
    if (watchRowHasValue(row)) {
      return true;
    }
  }
  return false;
}

// Highlight a slave in the topology tree and update dependent panels
void MainWindow::setSelectedSlave(int position) {
  if (position < 0) {
    selectedLabel_->setText(activeMasterName());
    beginSelectedSlaveOnlineLoad(-1);
    filterWatchTable();
    updateSelectedSlavePanel();
    updateActionAvailability();
    updateCommissioningWorkflow();
    updateIoVariableTable();
    updateStateMachineView();
    return;
  }
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      selectedLabel_->setText(
          QString("#%1  %2").arg(slave.position).arg(slave.name));
      beginSelectedSlaveOnlineLoad(position);
      client_.slaveInfo(position);
      client_.pdos(position);
      client_.sdos(position);
      client_.xml(position);
      filterWatchTable();
      updateSelectedSlavePanel();
      updateActionAvailability();
      updateCommissioningWorkflow();
      updateIoVariableTable();
      updateStateMachineView();
      return;
    }
  }
}

// Compare previous and current slave lists; emit diagnostics for topology changes
void MainWindow::reportTopologyChanges(const QVector<SlaveInfo> &previous,
                                       const QVector<SlaveInfo> &current) {
  const QVector<TopologyChange> changes =
      detectTopologyChanges(previous, current);
  if (changes.isEmpty()) {
    return;
  }

  for (const auto &change : changes) {
    switch (change.kind) {
    case TopologyChangeKind::Added:
      updateDiagnostics("Warning", "Topology",
                        QString("Slave #%1 added: %2 (%3)")
                            .arg(change.position)
                            .arg(change.current.name, change.current.state));
      break;
    case TopologyChangeKind::NameChanged:
      updateDiagnostics("Warning", "Topology",
                        QString("Slave #%1 identity changed: %2 -> %3")
                            .arg(change.position)
                            .arg(change.previous.name, change.current.name));
      break;
    case TopologyChangeKind::StateChanged: {
      const QString level = change.current.state == "OP" ? "Info" : "Warning";
      updateDiagnostics(level, "Topology",
                        QString("Slave #%1 state changed: %2 -> %3 (%4)")
                            .arg(change.position)
                            .arg(change.previous.state, change.current.state,
                                 change.current.name));
      break;
    }
    case TopologyChangeKind::FlagsChanged:
      updateDiagnostics("Info", "Topology",
                        QString("Slave #%1 flags changed: %2 -> %3")
                            .arg(change.position)
                            .arg(change.previous.flags, change.current.flags));
      break;
    case TopologyChangeKind::Removed:
      updateDiagnostics("Error", "Topology",
                        QString("Slave #%1 removed: %2 (%3)")
                            .arg(change.position)
                            .arg(change.previous.name, change.previous.state));
      break;
    }
  }
  consistencyFresh_ = false;
}

// Process a fresh slave scan result — update topology tree and detect changes
void MainWindow::updateSlaves(const QVector<SlaveInfo> &slaves) {
  const int previous = selectedPosition();
  const QVector<SlaveInfo> previousSlaves = slaves_;
  reportTopologyChanges(previousSlaves, slaves);

  // Skip full tree rebuild if slave list is identical (avoids UI flicker)
  if (slaves == slaves_) {
    return;
  }
  slaves_ = slaves;
  topologyTree_->setUpdatesEnabled(false);
  topologyTree_->clear();

  auto *master = new QTreeWidgetItem(
      topologyTree_,
      {activeMasterName(),
       QString("%1 %2").arg(slaves.size()).arg(uiText("slaves", "从站"))});
  master->setData(0, Qt::UserRole, -1);
  master->setExpanded(true);

  for (const auto &slave : slaves) {
    auto *item = new QTreeWidgetItem(
        master,
        {QString("#%1  %2").arg(slave.position).arg(slave.name), slave.state});
    item->setData(0, Qt::UserRole, slave.position);
    if (slave.state == "OP") {
      item->setForeground(1, QColor("#8ff0c8"));
    } else if (slave.state == "PREOP") {
      item->setForeground(1, QColor("#ffd27b"));
    }
  }

  topologyTree_->expandAll();
  setMetricCard(slaveCountLabel_, uiText("Slaves", "从站"),
                QString::number(slaves.size()));
  log(QString("Scan complete: %1 slaves").arg(slaves.size()));
  updateDiagnostics("Info", "Scan",
                    QString("%1 slave(s) detected").arg(slaves.size()));
  updateSelectedSlavePanel();
  updateTopologyBaselineSummary();
  updateActionAvailability();
  updateCommissioningWorkflow();
  updateIoVariableTable();
  updateStateMachineView();
  updateStatusBar();

  if (!slaves.isEmpty()) {
    QTreeWidgetItem *target = master->child(0);
    for (int i = 0; i < master->childCount(); ++i) {
      if (master->child(i)->data(0, Qt::UserRole).toInt() == previous) {
        target = master->child(i);
        break;
      }
    }
    topologyTree_->setCurrentItem(target);
  }
  topologyTree_->setUpdatesEnabled(true);
}

// Parse the daemon's master status text and update the overview metric cards
void MainWindow::updateMasterSummary(const QString &text) {
  const QString phase = capture(text, R"(^\s*Phase:\s*(.+)$)");
  const QString slaves = capture(text, R"(^\s*Slaves:\s*(.+)$)");
  const QString link = capture(text, R"(^\s*Link:\s*(.+)$)");
  const QString loss = capture(text, R"(^\s*Lost frames:\s*(.+)$)");
  setMetricCard(masterStateLabel_, uiText("Master", "主站"),
                phase.isEmpty() ? uiText("Unknown", "未知") : phase);
  if (!slaves.isEmpty()) {
    setMetricCard(slaveCountLabel_, uiText("Slaves", "从站"), slaves);
  }
  setMetricCard(linkStateLabel_, uiText("Link", "链路"),
                link.isEmpty() ? uiText("Unknown", "未知") : link);
  setMetricCard(lossLabel_, uiText("Frame Loss", "丢帧"),
                loss.isEmpty() ? "0" : loss);

  QList<QStringList> rows;
  const auto lines = text.split('\n');
  for (const auto &line : lines) {
    const auto idx = line.indexOf(':');
    if (idx > 0) {
      rows.append({line.left(idx).trimmed(), line.mid(idx + 1).trimmed()});
    }
  }
  setTableRows(metricTable_, {"Metric", "Value"}, rows);
  updateCommissioningWorkflow();
}

// Parse identity text from the daemon and populate the identity/port/mailbox tables
void MainWindow::updateSlaveInfo(const QString &text) {
  QList<QStringList> identity;
  for (const QString &key :
       {"Vendor Id", "Product code", "Revision number", "Serial number",
        "Group", "Order number", "Device name"}) {
    const QString value = capture(
        text,
        QString(R"(^\s*%1:\s*(.+)$)").arg(QRegularExpression::escape(key)));
    if (!value.isEmpty()) {
      identity.append({key, value});
    }
  }
  setTableRows(identityTable_, {"Field", "Value"}, identity);

  QList<QStringList> ports;
  bool inPortTable = false;
  for (const auto &line : text.split('\n')) {
    if (line.startsWith("Port  Type")) {
      inPortTable = true;
      continue;
    }
    if (inPortTable) {
      const auto parts = line.simplified().split(' ');
      if (parts.size() >= 5 && parts.first().at(0).isDigit()) {
        ports.append({parts.value(0), parts.value(1), parts.value(2),
                      parts.value(3), parts.value(4)});
      } else if (!line.trimmed().isEmpty()) {
        inPortTable = false;
      }
    }
  }
  setTableRows(portTable_, {"Port", "Type", "Link", "Loop", "Signal"}, ports);

  QList<QStringList> mailboxes;
  for (const QString &prefix :
       {"Bootstrap", "Standard", "Supported protocols"}) {
    const QString value = capture(
        text,
        QString(R"(^\s*%1\s*(.*)$)").arg(QRegularExpression::escape(prefix)));
    if (!value.isEmpty()) {
      QString cleaned = value;
      cleaned.remove(':');
      mailboxes.append({prefix, cleaned.trimmed()});
    }
  }
  setTableRows(mailboxTable_, {"Mailbox", "Value"}, mailboxes);
  updateCommissioningWorkflow();
  updateStateMachineView();
}

// Parse PDO map text from the daemon and populate the PDO table with direction detection
void MainWindow::updatePdoTable(const QString &text) {
  QList<QStringList> rows;
  QString sm;
  QString pdo;
  const QRegularExpression smRe(R"(^SM(\d+):.*DefaultSize\s+(\d+).*)");
  const QRegularExpression pdoRe(
      R"(^\s+(RxPDO|TxPDO)\s+(0x[0-9a-fA-F]+)\s+\"(.+)\")");
  const QRegularExpression entryRe(
      R"(^\s+PDO entry\s+(0x[0-9a-fA-F]+):([0-9a-fA-F]+),\s+(\d+)\s+bit,\s+\"(.+)\")");
  for (const auto &line : text.split('\n')) {
    auto m = smRe.match(line);
    if (m.hasMatch()) {
      sm = QString("SM%1 (%2 bytes)").arg(m.captured(1), m.captured(2));
      continue;
    }
    m = pdoRe.match(line);
    if (m.hasMatch()) {
      pdo =
          QString("%1 %2 %3").arg(m.captured(1), m.captured(2), m.captured(3));
      continue;
    }
    m = entryRe.match(line);
    if (m.hasMatch()) {
      rows.append({sm, pdo, m.captured(1), m.captured(2), m.captured(3),
                   m.captured(4)});
    }
  }
  setTableRows(sdo_->pdoTable, {"SM", "PDO", "Index", "Sub", "Bits", "Name"}, rows);
  filterPdoTable();
  updateIoVariableTable();
  updateStateMachineView();
}

// Apply text filter to the PDO map table, hiding non-matching rows
void MainWindow::filterPdoTable() {
  if (!sdo_->pdoTable) {
    return;
  }
  const QString needle = sdo_->pdoFilter ? sdo_->pdoFilter->text().trimmed() : QString();
  int visible = 0;
  for (int row = 0; row < sdo_->pdoTable->rowCount(); ++row) {
    bool match = needle.isEmpty();
    for (int column = 0; column < sdo_->pdoTable->columnCount() && !match;
         ++column) {
      const auto *item = sdo_->pdoTable->item(row, column);
      match = item && item->text().contains(needle, Qt::CaseInsensitive);
    }
    sdo_->pdoTable->setRowHidden(row, !match);
    if (match) {
      ++visible;
    }
  }
  if (sdo_->pdoSummaryLabel) {
    const int total = sdo_->pdoTable->rowCount();
    sdo_->pdoSummaryLabel->setText(
        total > 0 ? uiText("%1/%2 PDO entries", "%1/%2 个 PDO 条目")
                        .arg(visible)
                        .arg(total)
                  : uiText("No PDO entries", "暂无 PDO 条目"));
  }
  updatePdoRowDetail();
  updateActionAvailability();
  updateCommissioningWorkflow();
}

// Update the detail strip below the PDO map table for the current row
void MainWindow::updatePdoRowDetail() {
  if (!sdo_->pdoDetailLabel) {
    return;
  }
  const PdoMapDetailTexts texts = {
      .unavailableText = uiText("PDO Map evidence is not available.",
                                "当前没有可用的 PDO 映射证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select a visible PDO entry to review Sync Manager, PDO, object "
          "address, bit width, name, inferred type, and operation boundary.",
          "选择一条可见 PDO 条目，以复核 Sync "
          "Manager、PDO、对象地址、位宽、名称、"
          "推断类型和操作边界。"),
      .noSelectionTip = uiText(
          "Selecting rows, filtering, and reading this detail strip are local "
          "review actions after PDO data is loaded. Loading or refreshing PDO "
          "Map is the explicit online PDO evidence path; double-clicking a row "
          "fills and reads the SDO through the normal read path.",
          "PDO "
          "数据加载后，选择行、筛选和查看此详情条都是本地审阅动作；加载或刷新 "
          "PDO Map 才是显式在线 PDO "
          "证据路径；双击行会通过普通读取路径填充并读取 "
          "SDO。"),
      .directionRxOutput = uiText("Rx output", "Rx 输出"),
      .directionTxInput = uiText("Tx input", "Tx 输入"),
      .directionUnknown = uiText("PDO direction unknown", "PDO 方向未知"),
      .roleRxOutput =
          uiText("Output/process command candidate", "输出/过程命令候选"),
      .roleTxInput =
          uiText("Input/process feedback candidate", "输入/过程反馈候选"),
      .roleGeneric = uiText("Generic process-data entry", "通用过程数据条目"),
      .typeFallback = uiText("type?", "类型?"),
      .unnamed = uiText("Unnamed PDO entry", "未命名 PDO 条目"),
      .cia402Candidate = uiText("CiA 402 candidate", "CiA 402 候选"),
      .genericEntry = uiText("Generic PDO entry", "通用 PDO 条目"),
      .summaryPattern = uiText("%1 | %2:%3 | %4 bit %5 | %6 | %7",
                               "%1 | %2:%3 | %4 bit %5 | %6 | %7"),
      .selectedTitle = uiText("Selected PDO Map row", "选中的 PDO 映射行"),
      .slaveLabel = uiText("Slave", "从站"),
      .syncManagerLabel = uiText("Sync Manager", "Sync Manager"),
      .pdoLabel = uiText("PDO", "PDO"),
      .objectLabel = uiText("Object", "对象"),
      .bitsLabel = uiText("Bits", "位宽"),
      .inferredTypeLabel = uiText("Inferred SDO Type", "推断 SDO 类型"),
      .nameLabel = uiText("Name", "名称"),
      .directionLabel = uiText("Direction", "方向"),
      .roleLabel = uiText("Process Role", "过程角色"),
      .driveEvidenceLabel = uiText("Drive Evidence", "驱动证据"),
      .localBoundary = uiText(
          "Local preview boundary: after PDO Map data is loaded, selecting "
          "this row, filtering PDO entries, and reading this detail strip do "
          "not read SDOs, write SDOs, change state, toggle Free Run, "
          "rescan/connect, or run Host Health.",
          "本地预览边界：PDO Map 数据加载后，选择此行、筛选 PDO 条目和查看此"
          "详情条都不读取 SDO、不写 SDO、不切换状态、不改变 Free Run、不重扫/"
          "连接，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Load/refresh PDO Map is explicit online PDO "
          "evidence loading; Fill SDO only prepares the local target; "
          "double-click or Fill and Read uses the normal explicit SDO read "
          "path; Add Selected to Watch creates Watch rows without immediate "
          "reads.",
          "执行边界：加载/刷新 PDO Map 是显式在线 PDO 证据加载；填充 SDO "
          "只准备本地目标；双击或填充并读取会走普通显式 SDO "
          "读取路径；选中项加入 Watch 只创建 Watch 行，不立即读取。"),
  };

  auto applyState = [this](const PdoMapDetailUiState &state) {
    sdo_->pdoDetailLabel->setText(state.text);
    sdo_->pdoDetailLabel->setProperty("severity", state.severityKey);
    sdo_->pdoDetailLabel->setToolTip(state.tooltip);
    repolish(sdo_->pdoDetailLabel);
  };

  if (!sdo_->pdoTable) {
    applyState(pdoMapDetailUnavailableState(texts));
    return;
  }

  const int row = sdo_->pdoTable->currentRow();
  if (row < 0 || row >= sdo_->pdoTable->rowCount() || sdo_->pdoTable->isRowHidden(row)) {
    applyState(pdoMapDetailNoSelectionState(texts));
    return;
  }

  applyState(buildPdoMapDetailUiState(pdoMapTableRowFromTable(sdo_->pdoTable, row),
                                      selectedPosition(), texts));
}

// Parse Object Dictionary text from the daemon, merge with evidence, and populate the SDO table
void MainWindow::updateSdoTable(const QString &text) {
  QList<QStringList> rows;
  QString object;
  auto normalizeHex = [](QString text, int minimumDigits) {
    text = text.trimmed();
    if (text.isEmpty()) {
      return text;
    }
    QString digits = text;
    if (digits.startsWith("0x", Qt::CaseInsensitive)) {
      digits = digits.mid(2);
    }
    bool ok = false;
    const quint64 parsed = digits.toULongLong(&ok, 16);
    if (!ok) {
      return text.toLower();
    }
    return QString("0x%1")
        .arg(parsed, minimumDigits, 16, QLatin1Char('0'))
        .toLower();
  };
  const QRegularExpression objRe(R"(^SDO\s+(0x[0-9a-fA-F]+),\s+\"(.+)\")");
  const QRegularExpression entryRe(
      R"(^\s+(0x[0-9a-fA-F]+):([0-9a-fA-F]+),\s+(\S+),\s+([^,]+),\s+(\d+)\s+bit,\s+\"(.+)\")");
  for (const auto &line : text.split('\n')) {
    auto m = objRe.match(line);
    if (m.hasMatch()) {
      object = QString("%1 %2").arg(m.captured(1), m.captured(2));
      continue;
    }
    m = entryRe.match(line);
    if (m.hasMatch()) {
      const QString index = m.captured(1);
      const QString subIndex = m.captured(2);
      const QString key =
          QString("%1|%2|%3")
              .arg(selectedPosition())
              .arg(normalizeHex(index, 4), normalizeHex(subIndex, 2));
      const QStringList evidence = sdoEvidence_.value(key);
      rows.append(
          {object, index, subIndex, m.captured(3), m.captured(4).trimmed(),
           m.captured(5), m.captured(6), evidence.value(0),
           evidence.isEmpty()
               ? QString()
               : QString("%1  %2").arg(evidence.value(1), evidence.value(3))});
    }
  }
  setTableRows(sdo_->sdoTable,
               {"Object", "Index", "Sub", "Access", "Type", "Bits", "Name",
                "Last Value", "Last Status"},
               rows);
  for (int row = 0; row < sdo_->sdoTable->rowCount(); ++row) {
    const QString lastStatus =
        sdo_->sdoTable->item(row, 8) ? sdo_->sdoTable->item(row, 8)->text() : QString();
    if (lastStatus.isEmpty()) {
      continue;
    }
    const QColor color =
        lastStatus.contains(uiText("Complete", "完成")) ||
                lastStatus.contains(uiText("OK", "成功")) ||
                lastStatus.contains(uiText("Write OK", "写入完成"))
            ? QColor("#22c55e")
            : (lastStatus.contains(uiText("Failed", "失败"))
                   ? QColor("#ef4444")
                   : QColor("#f59e0b"));
    if (auto *statusItem = sdo_->sdoTable->item(row, 8)) {
      statusItem->setForeground(color);
    }
    if (auto *valueItem = sdo_->sdoTable->item(row, 7)) {
      valueItem->setBackground(settings_.theme == "Light" ? QColor("#eef2ff")
                                                          : QColor("#172036"));
    }
  }
  filterSdoTable(sdo_->sdoFilter ? sdo_->sdoFilter->text() : QString());
  updateCommissioningWorkflow();
  updateStateMachineView();
}

// Process real-time Free Run telemetry — update signals table and entry table with PDO map cross-reference
void MainWindow::updateFreeRunTelemetry(const QJsonObject &telemetry) {
  const QList<QStringList> rows = {
      {"Running", telemetry.value("running").toBool() ? "Yes" : "No"},
      {"Status", telemetry.value("status").toString()},
      {"Cycle Count", telemetry.value("cycles").toString()},
      {"Configured Slaves",
       QString::number(telemetry.value("configuredSlaves").toInt())},
      {"PDO Entries", QString::number(telemetry.value("pdoEntries").toInt())},
      {"Slaves Responding",
       QString::number(telemetry.value("slavesResponding").toInt())},
      {"AL State", telemetry.value("alStateText").toString()},
      {"AL State Mask",
       QString("0x%1").arg(telemetry.value("alStates").toInt(), 0, 16)},
      {"Link Up", telemetry.value("linkUp").toBool() ? "Yes" : "No"},
      {"Working Counter",
       QString::number(telemetry.value("workingCounter").toInt())},
      {"WKC State", telemetry.value("wcStateText").toString()},
      {"WKC State Code", QString::number(telemetry.value("wcState").toInt())},
      {"Redundancy Active",
       QString::number(telemetry.value("redundancyActive").toInt())},
  };
  setTableRows(freeRunWidgets_->freeRunTable, {"Signal", "Value"}, rows);

  QList<QStringList> entries;
  const auto array = telemetry.value("entries").toArray();
  auto directionClass = [](QString text) {
    text = text.trimmed().toLower();
    if (text.contains("rx") || text.contains("output") || text == "out") {
      return QStringLiteral("rx");
    }
    if (text.contains("tx") || text.contains("input") || text == "in") {
      return QStringLiteral("tx");
    }
    return QString();
  };
  auto pdoMapEvidence = [this, &directionClass](
                            int slave, const QString &direction,
                            const QString &index, const QString &subIndex,
                            int bits, QString *mappedName,
                            QString *mappedDetail) {
    if (mappedName) {
      mappedName->clear();
    }
    if (mappedDetail) {
      mappedDetail->clear();
    }
    if (!sdo_->pdoTable || sdo_->pdoTable->rowCount() <= 0) {
      return uiText("No PDO map", "无 PDO 映射");
    }
    if (slave != selectedPosition()) {
      return uiText("Map not loaded for slave", "未加载该从站映射");
    }

    const QString normalizedIndex = normalizeHexText(index, 4);
    const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
    const QString runtimeDirection = directionClass(direction);
    for (int row = 0; row < sdo_->pdoTable->rowCount(); ++row) {
      const QString mapIndex =
          normalizeHexText(tableText(sdo_->pdoTable, row, 2), 4);
      const QString mapSubIndex =
          normalizeHexText(tableText(sdo_->pdoTable, row, 3), 2);
      if (mapIndex != normalizedIndex || mapSubIndex != normalizedSubIndex) {
        continue;
      }

      const QString mapPdo = tableText(sdo_->pdoTable, row, 1);
      const QString mapBitsText = tableText(sdo_->pdoTable, row, 4);
      const int mapBits = mapBitsText.toInt();
      const QString mapDirection = directionClass(mapPdo);
      if (mappedName) {
        *mappedName = tableText(sdo_->pdoTable, row, 5);
      }
      if (mappedDetail) {
        *mappedDetail = QString("%1 %2 bit").arg(mapPdo, mapBitsText);
      }
      QStringList issues;
      if (!runtimeDirection.isEmpty() && !mapDirection.isEmpty() &&
          runtimeDirection != mapDirection) {
        issues << uiText("direction mismatch", "方向不一致");
      }
      if (bits > 0 && mapBits > 0 && bits != mapBits) {
        issues << uiText("bit mismatch %1/%2", "位宽不一致 %1/%2")
                      .arg(bits)
                      .arg(mapBits);
      }
      return issues.isEmpty() ? uiText("Mapped", "已映射")
                              : uiText("Mapped warning: %1", "映射警告：%1")
                                    .arg(issues.join("; "));
    }
    return uiText("Missing in PDO map", "PDO 映射缺失");
  };
  for (const auto &value : array) {
    const auto object = value.toObject();
    const QString slave = QString::number(object.value("slave").toInt());
    const QString sync = QString::number(object.value("sync").toInt());
    const QString direction = object.value("direction").toString();
    const QString pdo = object.value("pdo").toString();
    const QString index = object.value("index").toString();
    const QString subIndex = object.value("subindex").toString();
    const int bits = object.value("bits").toInt();
    QString mappedName;
    QString mappedDetail;
    const QString mapEvidence =
        pdoMapEvidence(object.value("slave").toInt(), direction, index,
                       subIndex, bits, &mappedName, &mappedDetail);
    const QString nameKey =
        QString("%1|%2|%3|%4|%5|%6")
            .arg(slave, sync, direction, pdo, index, subIndex);
    const QString objectNameKey = ioVariableTableObjectKey(
        object.value("slave").toInt(), index, subIndex);
    const QString aliasName = ioVariableMetadata_.value(objectNameKey).value(0);
    QString displayName = aliasName.trimmed();
    QString displayNameSource =
        displayName.isEmpty() ? QString() : uiText("I/O alias", "I/O 别名");
    auto takeDisplayName = [&](const QString &candidate,
                               const QString &source) {
      if (!displayName.isEmpty() || candidate.trimmed().isEmpty()) {
        return;
      }
      displayName = candidate.trimmed();
      displayNameSource = source;
    };
    takeDisplayName(object.value("displayName").toString(),
                    uiText("runtime display name", "运行时显示名"));
    takeDisplayName(object.value("name").toString(),
                    uiText("runtime name", "运行时名称"));
    takeDisplayName(mappedName, uiText("PDO map", "PDO 映射"));
    takeDisplayName(freeRunObjectNames_.value(objectNameKey),
                    uiText("object cache", "对象缓存"));
    takeDisplayName(freeRunEntryNames_.value(nameKey),
                    uiText("entry cache", "条目缓存"));
    if (displayName.isEmpty()) {
      displayName = QString("%1 %2:%3").arg(direction, index, subIndex);
      displayNameSource = uiText("address fallback", "地址回退");
    } else {
      freeRunEntryNames_.insert(nameKey, displayName);
      if (!objectNameKey.isEmpty()) {
        freeRunObjectNames_.insert(objectNameKey, displayName);
      }
    }
    const QString rawValue = object.value("rawValue").toString();
    const QString decodedValue = object.value("decodedValue").toString();
    QString meaning = object.value("meaning").toString();
    if (meaning.trimmed().isEmpty()) {
      meaning = decodeWatchValue(index, subIndex, QString(),
                                 rawValue.isEmpty() ? decodedValue : rawValue,
                                 "CiA 402");
    }
    entries.append({
        slave,
        sync,
        direction,
        pdo,
        index,
        subIndex,
        QString::number(bits),
        QString::number(object.value("offset").toInt()),
        QString::number(object.value("bit").toInt()),
        displayName,
        rawValue,
        decodedValue,
        meaning,
        mapEvidence,
        mappedDetail.isEmpty()
            ? uiText("Name source: %1", "名称来源：%1").arg(displayNameSource)
            : QString("%1 | %2").arg(mappedDetail,
                                     uiText("Name source: %1", "名称来源：%1")
                                         .arg(displayNameSource)),
    });
  }
  updateFreeRunEntryTable(entries);
}

// Rebuild the Free Run entry table from the processed rows
void MainWindow::updateFreeRunEntryTable(const QList<QStringList> &rows) {
  if (!freeRunWidgets_->freeRunEntryTable) {
    return;
  }

  const QStringList headers = {"Slave",   "SM",         "Dir",       "PDO",
                               "Index",   "Sub",        "Bits",      "Offset",
                               "Bit",     "Name",       "Raw",       "Decoded",
                               "Meaning", "Map Status", "Map Detail"};
  const int selectedRow = freeRunWidgets_->freeRunEntryTable->currentRow();
  const int verticalScroll =
      freeRunWidgets_->freeRunEntryTable->verticalScrollBar()
          ? freeRunWidgets_->freeRunEntryTable->verticalScrollBar()->value()
          : 0;
  const int horizontalScroll =
      freeRunWidgets_->freeRunEntryTable->horizontalScrollBar()
          ? freeRunWidgets_->freeRunEntryTable->horizontalScrollBar()->value()
          : 0;

  if (freeRunWidgets_->freeRunEntryTable->columnCount() != headers.size()) {
    freeRunWidgets_->freeRunEntryTable->setColumnCount(headers.size());
    freeRunWidgets_->freeRunEntryTable->setHorizontalHeaderLabels(headers);
  }
  if (freeRunWidgets_->freeRunEntryTable->rowCount() != rows.size()) {
    freeRunWidgets_->freeRunEntryTable->setRowCount(rows.size());
  }

  const QColor changedBackground =
      settings_.theme == "Light" ? QColor("#fff7cc") : QColor("#3a2f16");
  const QColor changedForeground =
      settings_.theme == "Light" ? QColor("#854d0e") : QColor("#fde68a");
  for (int row = 0; row < rows.size(); ++row) {
    const QStringList values = rows[row];
    const QString key =
        QString("%1|%2|%3|%4|%5|%6")
            .arg(values.value(0), values.value(1), values.value(2),
                 values.value(3), values.value(4), values.value(5));
    const QString valueSignature = values.value(10) + "|" + values.value(11);
    const bool changed = freeRunEntryValues_.contains(key) &&
                         freeRunEntryValues_.value(key) != valueSignature;
    freeRunEntryValues_.insert(key, valueSignature);

    for (int column = 0; column < headers.size(); ++column) {
      auto *item = freeRunWidgets_->freeRunEntryTable->item(row, column);
      if (!item) {
        item = new QTableWidgetItem;
        freeRunWidgets_->freeRunEntryTable->setItem(row, column, item);
      }
      item->setText(values.value(column));
      item->setData(Qt::UserRole, changed);
      if (changed && (column == 10 || column == 11)) {
        item->setBackground(changedBackground);
        item->setForeground(changedForeground);
      } else if (column == 13) {
        const QString status = values.value(column);
        const bool warning =
            status.contains(uiText("warning", "警告"), Qt::CaseInsensitive) ||
            status.contains(uiText("Missing", "缺失"), Qt::CaseInsensitive) ||
            status.contains("缺失") ||
            status.contains(uiText("No PDO map", "无 PDO 映射"),
                            Qt::CaseInsensitive);
        const bool mapped = status == uiText("Mapped", "已映射");
        item->setForeground(
            mapped ? QColor("#22c55e")
                   : (warning ? QColor("#f59e0b") : QColor("#64748b")));
      } else {
        item->setBackground(QBrush());
        item->setForeground(QBrush());
      }
    }
  }
  freeRunWidgets_->freeRunEntryTable->resizeColumnsToContents();
  if (selectedRow >= 0 && selectedRow < freeRunWidgets_->freeRunEntryTable->rowCount()) {
    freeRunWidgets_->freeRunEntryTable->selectRow(selectedRow);
  }
  if (freeRunWidgets_->freeRunEntryTable->verticalScrollBar()) {
    freeRunWidgets_->freeRunEntryTable->verticalScrollBar()->setValue(verticalScroll);
  }
  if (freeRunWidgets_->freeRunEntryTable->horizontalScrollBar()) {
    freeRunWidgets_->freeRunEntryTable->horizontalScrollBar()->setValue(horizontalScroll);
  }
  filterFreeRunEntryTable();
  updateIoVariableTable();
  updateStateMachineView();
}

// Apply text/scope filter to the Free Run entry table
void MainWindow::filterFreeRunEntryTable() {
  if (!freeRunWidgets_->freeRunEntryTable) {
    return;
  }
  const QString needle =
      freeRunWidgets_->freeRunFilter ? freeRunWidgets_->freeRunFilter->text().trimmed() : QString();
  const bool changedOnly =
      freeRunWidgets_->freeRunChangedOnly && freeRunWidgets_->freeRunChangedOnly->isChecked();
  int visibleRows = 0;
  int changedRows = 0;
  for (int row = 0; row < freeRunWidgets_->freeRunEntryTable->rowCount(); ++row) {
    const auto *firstItem = freeRunWidgets_->freeRunEntryTable->item(row, 0);
    const bool changed = firstItem && firstItem->data(Qt::UserRole).toBool();
    if (changed) {
      ++changedRows;
    }
    bool match = needle.isEmpty();
    for (int column = 0; column < freeRunWidgets_->freeRunEntryTable->columnCount() && !match;
         ++column) {
      const auto *item = freeRunWidgets_->freeRunEntryTable->item(row, column);
      match = item && item->text().contains(needle, Qt::CaseInsensitive);
    }
    match = match && (!changedOnly || changed);
    freeRunWidgets_->freeRunEntryTable->setRowHidden(row, !match);
    if (match) {
      ++visibleRows;
    }
  }
  if (freeRunWidgets_->freeRunEntrySummaryLabel) {
    freeRunWidgets_->freeRunEntrySummaryLabel->setText(
        uiText("%1 visible / %2 entries, %3 changed",
               "%1 可见 / %2 条目，%3 变化")
            .arg(visibleRows)
            .arg(freeRunWidgets_->freeRunEntryTable->rowCount())
            .arg(changedRows));
  }
  updateFreeRunEntryDetail();
  updateCommissioningWorkflow();
}

// Apply text/scope/changed-only filter to the Watch table
void MainWindow::filterWatchTable() {
  if (!watch_->watchTable) {
    return;
  }
  const QString needle =
      watch_->watchFilter ? watch_->watchFilter->text().trimmed() : QString();
  const QString scope =
      watch_->watchScopeFilter ? watch_->watchScopeFilter->currentData().toString() : "all";
  const bool changedOnly = watch_->watchChangedOnly && watch_->watchChangedOnly->isChecked();
  const int selected = selectedPosition();
  int visible = 0;
  int changedRows = 0;
  int driftRows = 0;
  int startupDriftRows = 0;
  int missingValueRows = 0;

  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    const int position = watch_->watchTable->item(row, 1)
                             ? watch_->watchTable->item(row, 1)->text().toInt()
                             : -1;
    const QString index = watch_->watchTable->item(row, 2)
                              ? watch_->watchTable->item(row, 2)->text().trimmed()
                              : QString();
    const QString subIndex = watch_->watchTable->item(row, 3)
                                 ? watch_->watchTable->item(row, 3)->text().trimmed()
                                 : QString();
    const QString key = QString("%1|%2|%3").arg(position).arg(index, subIndex);
    const bool changed = watchChangedKeys_.contains(key);
    if (changed) {
      ++changedRows;
    }
    const QString value = watch_->watchTable->item(row, 4)
                              ? watch_->watchTable->item(row, 4)->text().trimmed()
                              : QString();
    const bool missingValue = value.isEmpty();
    if (missingValue) {
      ++missingValueRows;
    }
    const QString delta = watch_->watchTable->item(row, 9)
                              ? watch_->watchTable->item(row, 9)->text().trimmed()
                              : QString();
    const QString normalizedDelta = delta.toLower();
    const bool baselineDrift = !delta.isEmpty() && normalizedDelta != "0" &&
                               normalizedDelta != "match" && delta != "匹配";
    if (baselineDrift) {
      ++driftRows;
    }
    const QString startupDelta =
        watch_->watchTable->item(row, 11)
            ? watch_->watchTable->item(row, 11)->text().trimmed()
            : QString();
    const QString normalizedStartupDelta = startupDelta.toLower();
    const bool startupDrift =
        !startupDelta.isEmpty() && normalizedStartupDelta != "match" &&
        startupDelta != "匹配" && normalizedStartupDelta != "pending" &&
        startupDelta != "待比较";
    if (startupDrift) {
      ++startupDriftRows;
    }
    const QString mode = watch_->watchTable->item(row, 7)
                             ? watch_->watchTable->item(row, 7)->text().trimmed()
                             : QString();
    const QString normalizedMode = mode.toLower();
    const QString normalizedIndex = normalizeHexText(index, 4);
    const bool cia402 =
        normalizedMode.contains("cia 402") ||
        normalizedMode.contains("cia402") || normalizedIndex == "0x6040" ||
        normalizedIndex == "0x6041" || normalizedIndex == "0x6060" ||
        normalizedIndex == "0x6061" || normalizedIndex == "0x603f" ||
        normalizedIndex == "0x6064" || normalizedIndex == "0x606c" ||
        normalizedIndex == "0x6077" || normalizedIndex == "0x607a" ||
        normalizedIndex == "0x60ff" || normalizedIndex == "0x6071";
    const bool selectedSlave = selected >= 0 && position == selected;

    bool match = needle.isEmpty();
    for (int column = 0; column < watch_->watchTable->columnCount() && !match;
         ++column) {
      const auto *item = watch_->watchTable->item(row, column);
      match = item && item->text().contains(needle, Qt::CaseInsensitive);
    }
    bool scopeMatch = true;
    if (scope == "selected") {
      scopeMatch = selectedSlave;
    } else if (scope == "changed") {
      scopeMatch = changed;
    } else if (scope == "baselineDrift") {
      scopeMatch = baselineDrift;
    } else if (scope == "startupDiff") {
      scopeMatch = startupDrift;
    } else if (scope == "missingValue") {
      scopeMatch = missingValue;
    } else if (scope == "cia402") {
      scopeMatch = cia402;
    }
    match = match && scopeMatch;
    match = match && (!changedOnly || changed);
    watch_->watchTable->setRowHidden(row, !match);
    if (match) {
      ++visible;
    }
  }

  if (watch_->watchSummaryLabel) {
    const bool autoEnabled =
        watch_->watchAutoRefresh && watch_->watchAutoRefresh->isChecked();
    const int interval = watch_->watchRefreshInterval
                             ? watch_->watchRefreshInterval->currentData().toInt()
                             : 1000;
    const QString mode = autoEnabled ? uiText("auto %1 ms", "自动 %1 ms")
                                           .arg(interval > 0 ? interval : 1000)
                                     : uiText("manual", "手动");
    const QString scopeLabel = watch_->watchScopeFilter
                                   ? watch_->watchScopeFilter->currentText()
                                   : uiText("All", "全部");
    const QString summary =
        uiText("%1/%2 | %3 | %4 | changed %5 | drift %6 | startup %7 | missing "
               "%8",
               "%1/%2 | %3 | %4 | 变化 %5 | 偏离 %6 | 启动 %7 | 缺失 %8")
            .arg(visible)
            .arg(watch_->watchTable->rowCount())
            .arg(scopeLabel, mode)
            .arg(changedRows)
            .arg(driftRows)
            .arg(startupDriftRows)
            .arg(missingValueRows);
    watch_->watchSummaryLabel->setText(summary);
    watch_->watchSummaryLabel->setToolTip(
        uiText(
            "Visible rows: %1/%2\nScope: %3\nRefresh: %4\nChanged rows: "
            "%5\nBaseline drift: %6\nStartup diff: %7\nMissing values: %8",
            "可见行：%1/%2\n范围：%3\n刷新：%4\n变化项：%5\n基线偏离：%6\n启动"
            "不一致：%7\n缺失值：%8")
            .arg(visible)
            .arg(watch_->watchTable->rowCount())
            .arg(scopeLabel, mode)
            .arg(changedRows)
            .arg(driftRows)
            .arg(startupDriftRows)
            .arg(missingValueRows));
  }
  updateWatchRowDetail();
  updateCommissioningWorkflow();
}

// Update the detail strip below the Watch table for the current row
void MainWindow::updateWatchRowDetail() {
  if (!watch_->watchDetailLabel) {
    return;
  }
  const WatchRowDetailTexts texts = {
      .unavailableText = uiText("Watch evidence is not available.",
                                "当前没有可用的 Watch 证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select a visible Watch row to review value, decoded meaning, "
          "baseline drift, Startup comparison, and read boundary.",
          "选择一条可见 Watch 行，以复核数值、解析含义、基线偏离、Startup "
          "对照和读取边界。"),
      .noSelectionTip = uiText(
          "Selecting rows, filtering scopes, and reading this detail strip are "
          "local UI actions. Refresh Watch and Auto polling are the explicit "
          "SDO read paths.",
          "选择行、筛选范围和查看此详情条都是本地界面动作；刷新 Watch "
          "和自动轮询才是显式 SDO 读取路径。"),
      .emptyValue = uiText("Empty", "空"),
      .typeFallback = uiText("Type?", "类型?"),
      .noBaseline = uiText("No baseline", "无基线"),
      .noComparison = uiText("No comparison", "无对照"),
      .startupMismatch = uiText("Startup mismatch", "Startup 不一致"),
      .baselineDrift = uiText("Baseline drift", "基线偏离"),
      .changed = uiText("Changed", "已变化"),
      .stableEvidence = uiText("Stable evidence", "证据稳定"),
      .cia402Candidate = uiText("CiA 402 candidate", "CiA 402 候选"),
      .genericSdo = uiText("Generic SDO", "通用 SDO"),
      .matchText = uiText("Match", "匹配"),
      .pendingText = uiText("Pending", "待比较"),
      .summaryPattern =
          uiText("#%1 %2:%3 | %4 | Value: %5 | Baseline: %6 | Startup: %7 | %8",
                 "#%1 %2:%3 | %4 | 值：%5 | 基线：%6 | Startup：%7 | %8"),
      .selectedTitle = uiText("Selected Watch row", "选中的 Watch 行"),
      .timeLabel = uiText("Time", "时间"),
      .slaveLabel = uiText("Slave", "从站"),
      .objectLabel = uiText("Object", "对象"),
      .typeLabel = uiText("Type", "类型"),
      .modeLabel = uiText("Mode", "模式"),
      .valueLabel = uiText("Value", "值"),
      .decodedLabel = uiText("Decoded", "解析"),
      .baselineLabel = uiText("Baseline", "基线"),
      .baselineDeltaLabel = uiText("Baseline Delta", "基线偏差"),
      .startupLabel = uiText("Startup", "启动值"),
      .startupDeltaLabel = uiText("Startup Delta", "启动偏差"),
      .changedLabel = uiText("Changed", "是否变化"),
      .yesText = uiText("Yes", "是"),
      .noText = uiText("No", "否"),
      .driveEvidenceLabel = uiText("Drive Evidence", "驱动证据"),
      .localBoundary = uiText(
          "Local preview boundary: selecting this row, filtering Watch scopes, "
          "and reading this detail strip do not read SDOs, write SDOs, change "
          "state, toggle Free Run, or run Host Health.",
          "本地预览边界：选择此行、筛选 Watch 范围和查看此详情条都不读取 SDO、"
          "不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Refresh Watch and Auto polling read SDO "
          "objects; Create Startup and Sync Startup only edit the Startup "
          "table until Apply is used.",
          "执行边界：刷新 Watch 和自动轮询会读取 SDO 对象；创建 Startup "
          "和同步 Startup 只编辑 Startup 表，直到使用应用动作。"),
  };

  auto applyState = [this](const WatchRowDetailUiState &state) {
    watch_->watchDetailLabel->setText(state.text);
    watch_->watchDetailLabel->setProperty("severity", state.severityKey);
    watch_->watchDetailLabel->setToolTip(state.tooltip);
    repolish(watch_->watchDetailLabel);
  };

  if (!watch_->watchTable) {
    applyState(watchRowDetailUnavailableState(texts));
    return;
  }

  const int row = watch_->watchTable->currentRow();
  if (row < 0 || row >= watch_->watchTable->rowCount() ||
      watch_->watchTable->isRowHidden(row)) {
    applyState(watchRowDetailNoSelectionState(texts));
    return;
  }

  applyState(buildWatchRowDetailUiState(
      watchStartupWatchRow(watch_->watchTable, row, watchChangedKeys_), texts));
}

// Helper: bulk-set table rows with headers in a single operation
void MainWindow::setTableRows(QTableWidget *table, const QStringList &headers,
                              const QList<QStringList> &rows) {
  table->setUpdatesEnabled(false);
  table->clear();
  table->setColumnCount(headers.size());
  table->setHorizontalHeaderLabels(headers);
  table->setRowCount(rows.size());
  for (int row = 0; row < rows.size(); ++row) {
    for (int column = 0; column < headers.size(); ++column) {
      table->setItem(row, column,
                     new QTableWidgetItem(rows[row].value(column)));
    }
  }
  table->resizeColumnsToContents();
  table->setUpdatesEnabled(true);
}

// Helper: update a metric card label's title and value
void MainWindow::setMetricCard(QLabel *label, const QString &title,
                               const QString &value) {
  if (!label) {
    return;
  }
  if (auto *card = label->parentWidget()) {
    if (auto *titleLabel = card->findChild<QLabel *>("metricTitle")) {
      titleLabel->setText(title);
    }
  }
  label->setText(value);
  label->setToolTip(value);
}
