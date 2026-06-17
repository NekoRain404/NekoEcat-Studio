// Topology baseline capture and diagnostics panel.

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


// — Snapshot the current slave list as the offline topology baseline
void MainWindow::captureTopologyBaseline() {
  if (slaves_.isEmpty()) {
    updateDiagnostics("Warning", "Topology",
                      uiText("Cannot capture baseline: no slaves detected",
                             "无法捕获基线：未检测到从站"));
    return;
  }
  topologyBaseline_ = slaves_;
  consistencyFresh_ = false;
  updateTopologyBaselineSummary();
  updateActionAvailability();
  updateDiagnostics("Info", "Topology",
                    QString("Captured topology baseline: %1 slave(s)")
                        .arg(topologyBaseline_.size()));
}


// — Discard the stored topology baseline and mark consistency stale
void MainWindow::clearTopologyBaseline() {
  topologyBaseline_.clear();
  consistencyFresh_ = false;
  updateTopologyBaselineSummary();
  updateActionAvailability();
  updateDiagnostics("Info", "Topology", "Topology baseline cleared");
}


// — Write a Markdown diagnostics report combining all evidence to a user-chosen file
void MainWindow::exportDiagnosticsReport() {
  const QString path = QFileDialog::getSaveFileName(
      this, "Export Diagnostics Report",
      QDir::home().absoluteFilePath(
          QString("ethercat-report-%1.md")
              .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"))),
      "Markdown (*.md);;Text (*.txt)");
  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Export failed", file.errorString());
    return;
  }

  QTextStream out(&file);
  int hostErrors = 0;
  int hostWarnings = 0;
  int hostOk = 0;
  int odEvidenceRows = 0;
  int odFailedRows = 0;
  if (hostHealthTable_) {
    for (int row = 0; row < hostHealthTable_->rowCount(); ++row) {
      const QString level = hostHealthTable_->item(row, 0)
                                ? hostHealthTable_->item(row, 0)->text()
                                : QString();
      if (level == "Error") {
        ++hostErrors;
      } else if (level == "Warning") {
        ++hostWarnings;
      } else if (!level.isEmpty()) {
        ++hostOk;
      }
    }
  }
  if (sdoTable_) {
    for (int row = 0; row < sdoTable_->rowCount(); ++row) {
      const SdoDictionaryRow dictionary =
          sdoDictionaryRowFromTable(sdoTable_, row);
      if (!dictionary.value.isEmpty() || !dictionary.status.isEmpty()) {
        ++odEvidenceRows;
      }
      if (dictionary.status.contains("failed", Qt::CaseInsensitive) ||
          dictionary.status.contains("失败")) {
        ++odFailedRows;
      }
    }
  }
  updateIoVariableTable();

  out << "# NekoEcat Studio Diagnostics\n\n";
  out << "Generated: " << QDateTime::currentDateTime().toString(Qt::ISODate)
      << "\n\n";
  out << "## Summary\n\n";
  out << "- Runtime: " << (client_.isConnected() ? "connected" : "offline")
      << "\n";
  out << "- Active Master: " << activeMasterName() << "\n";
  out << "- Slaves: " << slaves_.size() << "\n";
  out << "- Topology Baseline: "
      << (topologyBaseline_.isEmpty()
              ? "not captured"
              : QString("%1 slave(s)").arg(topologyBaseline_.size()))
      << "\n";
  out << "- Watch Items: " << (watchTable_ ? watchTable_->rowCount() : 0)
      << "\n";
  out << "- I/O Variables: "
      << (ioVariableTable_ ? ioVariableTable_->rowCount() : 0) << "\n";
  out << "- Startup SDO Rows: "
      << (startupSdoTable_ ? startupSdoTable_->rowCount() : 0) << "\n";
  out << "- Object Dictionary Evidence: " << odEvidenceRows << " row(s), "
      << odFailedRows << " failed\n";
  out << "- Free Run: " << lastFreeRunStatus_ << "\n";
  out << "- Host Health: " << hostErrors << " error(s), " << hostWarnings
      << " warning(s), " << hostOk << " OK\n\n";
  out << "## Host Health\n\n";
  writeMarkdownTable(out, hostHealthTable_);
  out << "## Diagnostics Events\n\n";
  writeMarkdownTable(out, diagnosticsTable_);
  out << "## Slaves\n\n";
  for (const auto &slave : slaves_) {
    out << "- #" << slave.position << " " << slave.state << " " << slave.flags
        << " " << slave.name << "\n";
  }
  out << "\n## Topology Baseline\n\n";
  if (topologyBaseline_.isEmpty()) {
    out << "_No topology baseline captured._\n\n";
  } else {
    for (const auto &slave : topologyBaseline_) {
      out << "- #" << slave.position << " " << slave.state << " " << slave.flags
          << " " << slave.name << "\n";
    }
    const QStringList issues = topologyBaselineIssues();
    out << "\nBaseline Check: "
        << (issues.isEmpty() ? "matches current topology" : issues.join("; "))
        << "\n\n";
  }
  out << "## Watch\n\n";
  writeMarkdownTable(out, watchTable_);
  out << "## I/O Variables\n\n";
  writeMarkdownTable(out, ioVariableTable_);
  updateConsistencyView();
  out << "## Consistency Check\n\n";
  writeMarkdownTable(out, consistency_->consistencyTable);
  out << "## Startup SDO\n\n";
  writeMarkdownTable(out, startupSdoTable_);
  out << "## SDO History\n\n";
  writeMarkdownTable(out, sdoHistoryTable_);
  out << "## Object Dictionary Evidence\n\n";
  if (!sdoTable_ || sdoTable_->rowCount() <= 0) {
    out << "_No Object Dictionary table is loaded._\n\n";
  } else if (odEvidenceRows <= 0) {
    out << "_No Object Dictionary evidence recorded._\n\n";
  } else {
    const QList<int> columns = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    out << "|";
    for (const int column : columns) {
      const auto *header = sdoTable_->horizontalHeaderItem(column);
      out << " "
          << markdownCell(header ? header->text()
                                 : QString("Column %1").arg(column + 1))
          << " |";
    }
    out << "\n|";
    for (int i = 0; i < columns.size(); ++i) {
      out << " --- |";
    }
    out << "\n";
    for (int row = 0; row < sdoTable_->rowCount(); ++row) {
      const SdoDictionaryRow dictionary =
          sdoDictionaryRowFromTable(sdoTable_, row);
      if (dictionary.value.isEmpty() && dictionary.status.isEmpty()) {
        continue;
      }
      out << "|";
      for (const int column : columns) {
        const auto *item = sdoTable_->item(row, column);
        out << " " << markdownCell(item ? item->text() : QString()) << " |";
      }
      out << "\n";
    }
    out << "\n";
  }
  out << "## Free Run Summary\n\n";
  writeMarkdownTable(out, freeRunTable_);
  out << "## Free Run Entries\n\n";
  writeMarkdownTable(out, freeRunEntryTable_);
  out << "## Project Notes\n\n";
  out << "```text\n"
      << (projectNotes_ ? projectNotes_->toPlainText() : QString())
      << "\n```\n\n";
  out << "\n## Master\n\n```text\n" << lastMasterText_ << "\n```\n\n";
  out << "## Selected Slave\n\n```text\n" << lastSlaveInfoText_ << "\n```\n\n";
  out << "## PDO\n\n```text\n" << lastPdoText_ << "\n```\n\n";
  out << "## SDO\n\n```text\n" << lastSdoText_.left(20000) << "\n```\n\n";
  out << "## Runtime Log\n\n```text\n" << logText_->toPlainText() << "\n```\n";
  log("Diagnostics report exported: " + path);
}


// — Apply level filter and text search to the diagnostics table
void MainWindow::filterDiagnosticsTable() {
  if (!diagnosticsTable_) {
    return;
  }
  const QString needle =
      diagnosticsFilter_ ? diagnosticsFilter_->text().trimmed() : QString();
  const QString level = diagnosticsLevelFilter_
                            ? diagnosticsLevelFilter_->currentData().toString()
                            : QString();

  for (int row = 0; row < diagnosticsTable_->rowCount(); ++row) {
    const QString rowLevel = diagnosticsTable_->item(row, 1)
                                 ? diagnosticsTable_->item(row, 1)->text()
                                 : QString();
    bool match = level.isEmpty() || rowLevel == level;
    if (match && !needle.isEmpty()) {
      match = false;
      for (int column = 0; column < diagnosticsTable_->columnCount() && !match;
           ++column) {
        const auto *item = diagnosticsTable_->item(row, column);
        match = item && item->text().contains(needle, Qt::CaseInsensitive);
      }
    }
    diagnosticsTable_->setRowHidden(row, !match); // show/hide based on filter match
  }
  updateDiagnosticsSummary();
}


// — Refresh the selected-slave summary label from identity, OD, and PDO evidence
void MainWindow::updateSelectedSlavePanel() {
  if (!selectedSlaveNameLabel_ || !selectedSlaveStateLabel_ ||
      !selectedSlaveFlagsLabel_ || !selectedSlaveHintLabel_) {
    return;
  }

  const int position = selectedPosition();
  if (position < 0) {
    selectedSlaveNameLabel_->setText(
        uiText("No slave selected", "尚未选择从站"));
    selectedSlaveStateLabel_->setText(uiText("State: none", "状态：无"));
    selectedSlaveFlagsLabel_->setText(uiText("Flags: none", "标志：无"));
    if (selectedDriveSummaryLabel_) {
      selectedDriveSummaryLabel_->setText(
          uiText("Drive: no Watch evidence", "驱动：暂无监视证据"));
      selectedDriveSummaryLabel_->setProperty("severity", "neutral");
      repolish(selectedDriveSummaryLabel_); // force QSS re-evaluation after property change
    }
    updateSelectedSlaveEvidenceSummary();
    updateDriveNextButton();
    selectedSlaveHintLabel_->setText(
        uiText("Select a slave in the I/O tree to enable contextual actions.",
               "在 I/O 树中选择从站后启用上下文操作。"));
    return;
  }

  SlaveInfo selected;
  bool found = false;
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      selected = slave;
      found = true;
      break;
    }
  }
  if (!found) {
    selectedSlaveNameLabel_->setText(QString("#%1").arg(position));
    selectedSlaveStateLabel_->setText(uiText("State: unknown", "状态：未知"));
    selectedSlaveFlagsLabel_->setText(uiText("Flags: unknown", "标志：未知"));
    updateSelectedDriveSummary();
    updateSelectedSlaveEvidenceSummary();
    selectedSlaveHintLabel_->setText(
        uiText("Refresh online data to recover the selected slave context.",
               "刷新在线数据以恢复选中从站上下文。"));
    return;
  }

  const QString name = selected.name.trimmed().isEmpty()
                           ? uiText("Unnamed slave", "未命名从站")
                           : selected.name.trimmed();
  selectedSlaveNameLabel_->setText(
      QString("#%1  %2").arg(selected.position).arg(name));
  selectedSlaveStateLabel_->setText(uiText("State: %1", "状态：%1")
                                        .arg(selected.state.trimmed().isEmpty()
                                                 ? uiText("Unknown", "未知")
                                                 : selected.state.trimmed()));
  selectedSlaveFlagsLabel_->setText(uiText("Flags: %1", "标志：%1")
                                        .arg(selected.flags.trimmed().isEmpty()
                                                 ? uiText("none", "无")
                                                 : selected.flags.trimmed()));

  const QString state = selected.state.trimmed().toUpper();
  QString hint;
  if (!client_.isConnected()) {
    hint = uiText("Runtime is offline. Connect before reading SDO, PDO, or "
                  "changing states.",
                  "运行时离线。读取 SDO/PDO 或切换状态前请先连接。");
  } else if (state == "OP") {
    hint = uiText(
        "Operational. Prefer Watch and Free Run; review outputs before "
        "writing.",
        "已进入 OP。优先使用 Watch 和 Free Run；写入前复核输出相关对象。");
  } else if (state == "SAFEOP") {
    hint = uiText("Safe-Operational. Good point to validate PDO and move to OP "
                  "when ready.",
                  "处于 SAFEOP。适合核对 PDO，确认后再进入 OP。");
  } else if (state == "PREOP") {
    hint = uiText("Pre-Operational. Good point for Object Dictionary and "
                  "Startup SDO work.",
                  "处于 PREOP。适合对象字典和 Startup SDO 调试。");
  } else if (state == "INIT") {
    hint = uiText("Init. Move to PREOP before mailbox-based SDO workflows.",
                  "处于 INIT。执行基于邮箱的 SDO 工作流前建议切到 PREOP。");
  } else {
    hint =
        uiText("Review diagnostics and raw slave output before changing state.",
               "切换状态前先查看诊断和从站原始输出。");
  }
  selectedSlaveHintLabel_->setText(hint);
  updateSelectedDriveSummary();
  updateSelectedSlaveEvidenceSummary();
}


// — Refresh the slave evidence summary label from all loaded evidence tables
void MainWindow::updateSelectedSlaveEvidenceSummary() {
  if (!selectedSlaveEvidenceLabel_) {
    return;
  }
  const SelectedSlaveEvidenceSummaryTexts texts =
      selectedSlaveEvidenceSummaryTexts();
  // Lambda to push UI state changes to the label widget
  auto applyState = [this](const SelectedSlaveEvidenceSummaryUiState &state) {
    selectedSlaveEvidenceLabel_->setText(state.text);
    selectedSlaveEvidenceLabel_->setToolTip(state.tooltip);
    selectedSlaveEvidenceLabel_->setProperty("severity", state.severityKey);
    repolish(selectedSlaveEvidenceLabel_); // force QSS re-evaluation after property change
  };

  const int position = selectedPosition();
  if (position < 0) {
    applyState(selectedSlaveEvidenceNoSelectionState(texts));
    return;
  }

  SlaveEvidenceInput input;
  input.position = position;
  applyLoadedSlaveEvidence(
      &input,
      {.identityPosition = loadedSlaveInfoPosition_,
       .identityRows = identityTable_ ? identityTable_->rowCount() : 0,
       .odPosition = loadedSdoPosition_,
       .odRows = sdoTable_ ? sdoTable_->rowCount() : 0,
       .pdoPosition = loadedPdoPosition_,
       .pdoRows = pdoTable_ ? pdoTable_->rowCount() : 0},
      {.watchTable = watchTable_,
       .startupTable = startupSdoTable_,
       .processTable = freeRunEntryTable_});
  const QStringList topologyIssues = topologyBaselineIssues();
  applyState(buildSelectedSlaveEvidenceSummaryUiState(
      input, topologyIssues.size(), texts));
}


// — Refresh the CiA 402 drive summary label from Watch evidence
void MainWindow::updateSelectedDriveSummary() {
  if (!selectedDriveSummaryLabel_) {
    return;
  }
  // Lambda to push UI state changes to the label widget
  auto applyState = [this](const SelectedDriveSummaryUiState &state) {
    selectedDriveSummaryLabel_->setText(state.text);
    selectedDriveSummaryLabel_->setProperty("severity", state.severityKey);
    repolish(selectedDriveSummaryLabel_); // force QSS re-evaluation after property change
  };

  const SelectedDriveSummaryTexts texts = selectedDriveSummaryTexts();
  const int position = selectedPosition();
  if (position < 0 || !watchTable_) {
    applyState(selectedDriveNoWatchEvidenceState(texts));
    updateDriveNextButton();
    return;
  }

  applyState(buildSelectedDriveSummaryUiState(
      watchStartupWatchRows(watchTable_), position, texts));
  updateDriveNextButton();
}


// — Enable or disable the 'Drive Next' button based on recommended controlword
void MainWindow::updateDriveNextButton() {
  auto *button = findChild<QPushButton *>("contextDriveNext");
  if (!button) {
    return;
  }

  QString label;
  QString value;
  QString reason;
  const bool hasRecommendation =
      client_.isConnected() && selectedPosition() >= 0 &&
      recommendedCia402Controlword(&label, &value, &reason);
  button->setEnabled(hasRecommendation);
  if (hasRecommendation) {
    button->setText(uiText("Next: %1", "下一步：%1").arg(label));
    button->setToolTip(uiText("Recommended from watched statusword: %1\nWrites "
                              "0x6040:0x00 uint16 = "
                              "%2 after the normal SDO confirmation.",
                              "根据监视状态字推荐：%1\n普通 SDO 确认后写入 "
                              "0x6040:0x00 uint16 = %2")
                           .arg(reason, value));
    return;
  }

  button->setText(uiText("Drive Next", "驱动下一步"));
  button->setToolTip(
      uiText("Add or refresh the CiA 402 Watch preset for the selected slave "
             "to derive "
             "a recommended controlword.",
             "为选中从站添加或刷新 CiA 402 监视预设后，才能推导推荐控制字。"));
}


// — Return a list of human-readable issues comparing baseline to current slaves
QStringList MainWindow::topologyBaselineIssues() const {
  QStringList issues;
  // Collect issue details from the evidence model
  for (const auto &issue :
       compareTopologyBaseline(topologyBaseline_, slaves_)) {
    // Map each baseline deviation type to a human-readable warning row
    switch (issue.kind) {
    case TopologyBaselineIssueKind::MissingSlave:
      issues << uiText("Missing slave #%1 %2", "缺失从站 #%1 %2")
                    .arg(issue.position)
                    .arg(issue.baseline.name);
      break;
    case TopologyBaselineIssueKind::NameChanged:
      issues << uiText("Slave #%1 name changed: %2 -> %3",
                       "从站 #%1 名称变化：%2 -> %3")
                    .arg(issue.position)
                    .arg(issue.baseline.name, issue.current.name);
      break;
    case TopologyBaselineIssueKind::StateChanged:
      issues << uiText("Slave #%1 state differs: %2 -> %3",
                       "从站 #%1 状态不同：%2 -> %3")
                    .arg(issue.position)
                    .arg(issue.baseline.state, issue.current.state);
      break;
    case TopologyBaselineIssueKind::UnexpectedSlave:
      issues << uiText("Unexpected slave #%1 %2", "额外从站 #%1 %2")
                    .arg(issue.position)
                    .arg(issue.current.name);
      break;
    }
  }

  return issues;
}


// — Update the baseline label with current issue count and slave count
void MainWindow::updateTopologyBaselineSummary() {
  if (!topologyBaselineLabel_) {
    return;
  }
  if (topologyBaseline_.isEmpty()) {
    topologyBaselineLabel_->setText(
        uiText("No topology baseline", "未设置拓扑基线"));
    return;
  }
  const QStringList issues = topologyBaselineIssues();
  topologyBaselineLabel_->setText(
      issues.isEmpty()
          ? uiText("Baseline: %1 slave(s), current topology matches",
                   "基线：%1 个从站，当前拓扑匹配")
                .arg(topologyBaseline_.size())
          : uiText("Baseline: %1 issue(s) against %2 expected slave(s)",
                   "基线：发现 %1 个问题，期望 %2 个从站")
                .arg(issues.size())
                .arg(topologyBaseline_.size()));
}

