// MainWindowFreeRunWorkspace.cpp — Free Run workspace methods.
// Extracted from MainWindow.cpp to reduce its size.

#include "MainWindowIncludes.h"

#include "services/ImpactAnalysisService.h"

// Delegate to ImpactAnalysisService for state transition impact details
QStringList MainWindow::stateTransitionImpactDetails(int position, const QString& requestedState) const {
    auto textFn = [this](const char* en, const char* zh) { return uiText(en, zh); };
    return ImpactAnalysisService::stateTransitionImpactDetails(
        position, requestedState, loadedSlaveInfoPosition_, loadedSdoPosition_, loadedPdoPosition_, identityTable_,
        sdo_->sdoTable, sdo_->pdoTable, watch_->watchTable, startupSdoTable_, freeRunWidgets_->freeRunEntryTable,
        topologyBaselineIssues(), consistencyGateDetails(uiText("state transition", "状态切换")), textFn);
}

// Build the impact details for Free Run confirmation
void MainWindow::setFreeRun(bool enabled) {
    if (enabled && client_.isConnected()) {
        QStringList details = {
            uiText("Master: %1", "主站：%1").arg(activeMasterName()),
            uiText("Selected slave: #%1", "选中从站：#%1").arg(selectedPosition()),
            uiText("Detected slaves: %1", "检测到从站：%1").arg(slaves_.size()),
            uiText("Free Run exchanges cyclic process-image telemetry without a "
                   "PLC project.",
                   "自由运行会在没有 PLC 工程的情况下交换周期过程映像遥测。"),
            uiText("Review PDO outputs and actuator safety before continuing.",
                   "继续前请复核 PDO 输出和执行机构安全。"),
        };
        details << freeRunImpactDetails();
        if (!confirmDangerousOperation(
                uiText("Confirm Free Run Start", "确认启动自由运行"),
                uiText("This operation starts cyclic Free Run telemetry.", "此操作会启动周期自由运行遥测。"), details,
                uiText("Start Free Run", "启动自由运行"))) {
            if (auto* action = findChild<QAction*>("freeRunAction")) {
                QSignalBlocker blocker(action);
                action->setChecked(false);
            }
            freeRun_ = false;
            setMetricCard(freeRunLabel_, uiText("Free Run", "自由运行"), uiText("Off", "关闭"));
            updateStatusBar();
            return;
        }
    }
    freeRun_ = enabled;
    refreshTimer_->setInterval(enabled ? 500 : 3000);
    setMetricCard(freeRunLabel_, uiText("Free Run", "自由运行"),
                  enabled ? uiText("On", "开启") : uiText("Off", "关闭"));
    if (!client_.isConnected()) {
        log(enabled ? "Free Run armed; waiting for runtime" : "Free Run disabled");
        return;
    }

    if (enabled) {
        log("Free Run enabled: starting runtime PDO cycle and using 500 ms online "
            "refresh");
        client_.freeRunStart();
    } else {
        log("Free Run disabled: stopping runtime PDO cycle and restoring slow "
            "refresh");
        client_.freeRunStop();
    }
}

// Build the impact details for Free Run confirmation
QStringList MainWindow::freeRunImpactDetails() const {
    auto textFn = [this](const char* en, const char* zh) { return uiText(en, zh); };
    return ImpactAnalysisService::freeRunImpactDetails(
        selectedPosition(), slaves_, sdo_->pdoTable, freeRunWidgets_->freeRunEntryTable, watch_->watchTable,
        topologyBaselineIssues(), consistencyGateDetails(uiText("Free Run start", "启动自由运行")), textFn);
}

// Update the detail strip below the Free Run entry table
void MainWindow::updateFreeRunEntryDetail() {
    if (!freeRunWidgets_->freeRunEntryDetailLabel) {
        return;
    }
    const FreeRunEntryDetailTexts texts = {
        .unavailableText = uiText("Process-image evidence is not available.", "当前没有可用的过程映像证据。"),
        .unavailableTip =
            uiText("This preview is local only and does not access the bus.", "此预览仅在本地工作，不访问总线。"),
        .noSelectionText = uiText("Select a visible process-image row to review name source, value, "
                                  "PDO map evidence, and output boundary.",
                                  "选择一条可见过程映像行，以复核名称来源、数值、PDO "
                                  "映射证据和输出边界。"),
        .noSelectionTip = uiText("Filtering, row selection, and this detail preview are local "
                                 "UI actions. Toggling Free Run remains the explicit online "
                                 "action.",
                                 "过滤、选择行和此详情预览都是本地界面动作；切换 Free Run "
                                 "仍是显式在线动作。"),
        .unknown = uiText("Unknown", "未知"),
        .directionFallback = uiText("Dir?", "方向?"),
        .unnamed = uiText("Unnamed", "未命名"),
        .emptyValue = uiText("Empty", "空"),
        .noMapEvidence = uiText("No map evidence", "无映射证据"),
        .outputBoundary = uiText("Output-like process data", "输出类过程数据"),
        .inputBoundary = uiText("Input/telemetry process data", "输入/遥测过程数据"),
        .mappedText = uiText("Mapped", "已映射"),
        .summaryPattern = uiText("#%1 %2 %3:%4 | %5 | %6 bit @ %7.%8 | %9 | Value: "
                                 "%10 | Map: %11",
                                 "#%1 %2 %3:%4 | %5 | %6 bit @ %7.%8 | %9 | 值：%10 | "
                                 "映射：%11"),
        .nameSourceMarkers = {uiText("Name source:", "名称来源："), QStringLiteral("name source:")},
        .selectedTitle = uiText("Selected Free Run process-image entry", "选中的 Free Run 过程映像条目"),
        .slaveLabel = uiText("Slave", "从站"),
        .syncManagerLabel = uiText("Sync Manager", "同步管理器"),
        .directionLabel = uiText("Direction", "方向"),
        .pdoLabel = uiText("PDO", "PDO"),
        .objectLabel = uiText("Object", "对象"),
        .nameLabel = uiText("Name", "名称"),
        .nameSourceLabel = uiText("Name Source", "名称来源"),
        .locationLabel = uiText("Location", "位置"),
        .rawLabel = uiText("Raw", "原始值"),
        .decodedLabel = uiText("Decoded", "解码值"),
        .meaningLabel = uiText("Meaning", "含义"),
        .mapStatusLabel = uiText("Map Status", "映射状态"),
        .mapDetailLabel = uiText("Map Detail", "映射详情"),
        .changedLabel = uiText("Changed", "是否变化"),
        .yesText = uiText("Yes", "是"),
        .noText = uiText("No", "否"),
        .boundaryLabel = uiText("Boundary", "边界"),
        .localBoundary = uiText("Local preview boundary: selecting this row, filtering rows, and "
                                "reading this detail strip do not read SDOs, write SDOs, change "
                                "state, toggle Free Run, or run Host Health.",
                                "本地预览边界：选择此行、过滤行和查看此详情条都不读取 SDO、不写 SDO、"
                                "不切换状态、不改变 Free Run，也不运行 Host Health。"),
        .executionBoundary = uiText("Execution boundary: Toggle Free Run remains the explicit online "
                                    "action; Fill SDO Fields and Alt+Enter only refill local SDO target "
                                    "fields unless you explicitly choose Fill and Read.",
                                    "执行边界：切换 Free Run 仍是显式在线动作；填充 SDO 字段和 Alt+Enter "
                                    "只会本地回填 SDO 目标，除非明确选择填充并读取。"),
    };

    auto applyState = [this](const FreeRunEntryDetailState& state) {
        freeRunWidgets_->freeRunEntryDetailLabel->setText(state.text);
        freeRunWidgets_->freeRunEntryDetailLabel->setProperty("severity", state.severityKey);
        freeRunWidgets_->freeRunEntryDetailLabel->setToolTip(state.tooltip);
        repolish(freeRunWidgets_->freeRunEntryDetailLabel);
    };

    if (!freeRunWidgets_->freeRunEntryTable) {
        applyState(freeRunEntryDetailUnavailableState(texts));
        return;
    }

    const int row = freeRunWidgets_->freeRunEntryTable->currentRow();
    if (row < 0 || row >= freeRunWidgets_->freeRunEntryTable->rowCount() ||
        freeRunWidgets_->freeRunEntryTable->isRowHidden(row)) {
        applyState(freeRunEntryDetailNoSelectionState(texts));
        return;
    }

    applyState(
        buildFreeRunEntryDetailState(freeRunEntryTableRowFromTable(freeRunWidgets_->freeRunEntryTable, row), texts));
}
