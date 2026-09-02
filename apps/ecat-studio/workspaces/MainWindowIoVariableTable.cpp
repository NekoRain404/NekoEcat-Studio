// I/O variable table, PLC handoff, and export functions.

#include "MainWindowIncludes.h"
void MainWindow::updateIoVariableTable() {
    if (!ioVar_->ioVariableTable) {
        return;
    }
    consistencyFresh_ = false;

    const int previousRow = ioVar_->ioVariableTable->currentRow();
    const int verticalScroll =
        ioVar_->ioVariableTable->verticalScrollBar() ? ioVar_->ioVariableTable->verticalScrollBar()->value() : 0;
    const int horizontalScroll =
        ioVar_->ioVariableTable->horizontalScrollBar() ? ioVar_->ioVariableTable->horizontalScrollBar()->value() : 0;

    const QStringList headers = {
        uiText("Slave", "从站"),   uiText("Dir", "方向"),     uiText("Symbol", "符号"),  uiText("Index", "索引"),
        uiText("Sub", "子项"),     uiText("Bits", "位宽"),    uiText("PDO", "PDO"),      uiText("Source", "来源"),
        uiText("Raw", "原始值"),   uiText("Decoded", "解码"), uiText("Meaning", "含义"), uiText("Watch", "Watch"),
        uiText("Startup", "启动"), uiText("Map", "映射"),     uiText("Changed", "变化"), uiText("PLC", "PLC"),
        uiText("Alias", "别名"),   uiText("Tags", "标签"),    uiText("Note", "备注")};

    auto directionText = [this](QString text) {
        const QString normalized = text.trimmed().toLower();
        if (normalized.contains("rx") || normalized.contains("output") || normalized == "out") {
            return uiText("Rx Output", "Rx 输出");
        }
        if (normalized.contains("tx") || normalized.contains("input") || normalized == "in") {
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
    if (watch_->watchTable) {
        ensureWatchTable();
        for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
            bool ok = false;
            const int position = tableText(watch_->watchTable, row, 1).toInt(&ok);
            const QString index = tableText(watch_->watchTable, row, 2);
            const QString subIndex = tableText(watch_->watchTable, row, 3);
            if (!ok || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
                continue;
            }
            const QString key = ioVariableTableObjectKey(position, index, subIndex);
            const bool changed = watchChangedKeys_.contains(key);
            watchByObject.insert(
                // ── Table Rebuild ──────────────────────────────────────────────────
                key, {tableText(watch_->watchTable, row, 4), tableText(watch_->watchTable, row, 5),
                      tableText(watch_->watchTable, row, 6), tableText(watch_->watchTable, row, 7),
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
            startupByObject.insert(ioVariableTableObjectKey(position, index, subIndex),
                                   {tableText(startupSdoTable_, row, 3), tableText(startupSdoTable_, row, 8)});
        }
    }

    QList<QStringList> rows;
    QSet<QString> coveredObjects;
    auto appendRow = [&](int position, const QString& direction, const QString& symbol, const QString& index,
                         const QString& subIndex, const QString& bits, const QString& pdo, const QString& source,
                         QString raw, QString decoded, QString meaning, const QString& mapStatus, bool changed) {
        if (position < 0 || index.trimmed().isEmpty() || subIndex.trimmed().isEmpty()) {
            return;
        }
        // Normalize hex address for consistent comparison
        const QString normalizedIndex = normalizeHexText(index, 4);
        // Normalize hex address for consistent comparison
        const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
        const QString key = ioVariableTableObjectKey(position, normalizedIndex, normalizedSubIndex);
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
        const QString type = watch.value(2).isEmpty() ? inferredType(bits) : watch.value(2);
        const QStringList metadata = ioVariableMetadata_.value(key);
        QString startupText;
        if (!startup.isEmpty()) {
            startupText = startup.value(1).isEmpty() ? startup.value(0)
                                                     : QString("%1 | %2").arg(startup.value(0), startup.value(1));
        }
        const bool changedEvidence = changed || !watch.value(4).isEmpty();
        const QString sourceText = type.isEmpty() ? source : QString("%1 | %2").arg(source, type);
        rows.append(
            {QString::number(position), directionText(direction),
             symbol.trimmed().isEmpty() ? QString("%1:%2").arg(normalizedIndex, normalizedSubIndex) : symbol.trimmed(),
             normalizedIndex, normalizedSubIndex, bits.trimmed(), pdo.trimmed(), sourceText, raw.trimmed(),
             decoded.trimmed(), meaning.trimmed(), watchValue, startupText, mapStatus.trimmed(),
             changedEvidence ? uiText("Yes", "是") : QString(), QString(), metadata.value(0), metadata.value(1),
             metadata.value(2)});
        coveredObjects.insert(key);
    };

    if (freeRunWidgets_->freeRunEntryTable) {
        for (int row = 0; row < freeRunWidgets_->freeRunEntryTable->rowCount(); ++row) {
            bool ok = false;
            const int position = tableText(freeRunWidgets_->freeRunEntryTable, row, 0).toInt(&ok);
            if (!ok) {
                continue;
            }
            const auto* changedItem = freeRunWidgets_->freeRunEntryTable->item(row, 0);
            appendRow(position, tableText(freeRunWidgets_->freeRunEntryTable, row, 2),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 9),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 4),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 5),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 6),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 3), uiText("Process", "过程"),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 10),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 11),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 12),
                      tableText(freeRunWidgets_->freeRunEntryTable, row, 13),
                      changedItem && changedItem->data(Qt::UserRole).toBool());
        }
    }

    if (loadedPdoPosition_ == selectedPosition() && sdo_->pdoTable) {
        for (int row = 0; row < sdo_->pdoTable->rowCount(); ++row) {
            const int position = selectedPosition();
            const QString key = ioVariableTableObjectKey(position, tableText(sdo_->pdoTable, row, 2),
                                                         tableText(sdo_->pdoTable, row, 3));
            if (coveredObjects.contains(key)) {
                continue;
            }
            appendRow(position, tableText(sdo_->pdoTable, row, 1), tableText(sdo_->pdoTable, row, 5),
                      tableText(sdo_->pdoTable, row, 2), tableText(sdo_->pdoTable, row, 3),
                      tableText(sdo_->pdoTable, row, 4), tableText(sdo_->pdoTable, row, 1), uiText("PDO", "PDO"),
                      QString(), QString(), QString(), uiText("PDO loaded", "PDO 已加载"), false);
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
        appendRow(position, uiText("SDO", "SDO"), QString(), parts.value(1), parts.value(2), QString(), QString(),
                  uiText("Watch", "Watch"), it.value().value(0), it.value().value(1), it.value().value(3), QString(),
                  !it.value().value(4).isEmpty());
    }

    setTableRows(ioVar_->ioVariableTable, headers, rows);
    const QColor changedBackground =
        // Define color for visual feedback
        settings_.theme == "Light" ? QColor("#fff7cc") : QColor("#3a2f16");
    const QColor warningBackground = changedBackground;
    const QColor diffBackground =
        // Define color for visual feedback
        settings_.theme == "Light" ? QColor("#fee2e2") : QColor("#3a1218");
    const QColor okColor("#22c55e");
    const QColor warnColor("#f59e0b");
    const QColor errorColor("#ef4444");
    const QSet<QString> duplicateSymbols = duplicateIoVariablePlcSymbols();
    // Iterate all rows and apply active filter predicates
    for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
        const IoVariableTableRow variable = ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
        const QString plcQuality = ioVariablePlcQuality(row, &duplicateSymbols, nullptr);
        const bool plcReady = plcQuality == uiText("Ready", "就绪");
        const bool hasAlias = !variable.alias.isEmpty();
        if (auto* item = ioVar_->ioVariableTable->item(row, 7)) {
            item->setForeground(ioVariableTableRowHasProcessSource(variable) ? okColor
                                                                             : (ioVariableTableRowHasPdoSource(variable)
                                                                                    // Define color for visual feedback
                                                                                    ? QColor("#60a5fa")
                                                                                    : warnColor));
        }
        if (auto* item = ioVar_->ioVariableTable->item(row, 12)) {
            if (ioVariableTableRowHasStartupDiff(variable)) {
                item->setBackground(diffBackground);
                item->setForeground(errorColor);
            }
        }
        if (auto* item = ioVar_->ioVariableTable->item(row, 13)) {
            item->setForeground(ioVariableTableRowHasPdoMapIssue(variable) ? warnColor : okColor);
        }
        if (ioVariableTableRowHasChangedValue(variable)) {
            if (auto* item = ioVar_->ioVariableTable->item(row, 14)) {
                item->setBackground(changedBackground);
                // Define color for visual feedback
                item->setForeground(settings_.theme == "Light" ? QColor("#854d0e")
                                                               // Define color for visual feedback
                                                               : QColor("#fde68a"));
            }
        }
        if (auto* item = ioVar_->ioVariableTable->item(row, 15)) {
            item->setText(plcQuality);
            item->setForeground(plcReady ? okColor : warnColor);
            if (!plcReady) {
                item->setBackground(warningBackground);
            }
        }
        if (hasAlias) {
            if (auto* item = ioVar_->ioVariableTable->item(row, 16)) {
                // Define color for visual feedback
                item->setForeground(QColor("#22c55e"));
            }
        }
        if (auto* item = ioVar_->ioVariableTable->item(row, 17)) {
            if (!item->text().trimmed().isEmpty()) {
                // Define color for visual feedback
                item->setForeground(QColor("#60a5fa"));
            }
        }
    }
    ioVar_->ioVariableTable->resizeColumnsToContents(); // auto-fit column widths
    if (previousRow >= 0 && previousRow < ioVar_->ioVariableTable->rowCount()) {
        ioVar_->ioVariableTable->setCurrentCell(previousRow, 0);
    }
    // ── Filtering ──────────────────────────────────────────────────────
    if (ioVar_->ioVariableTable->verticalScrollBar()) {
        ioVar_->ioVariableTable->verticalScrollBar()->setValue(verticalScroll);
    }
    if (ioVar_->ioVariableTable->horizontalScrollBar()) {
        ioVar_->ioVariableTable->horizontalScrollBar()->setValue(horizontalScroll);
    }
    filterIoVariableTable();
}


// ── I/O Variable Table Filter ────────────────────────────────────────
// — Apply text and scope filters to the I/O variable table
void MainWindow::filterIoVariableTable() {
    if (!ioVar_->ioVariableTable) {
        return;
    }
    const QString needle = ioVar_->ioVariableFilter ? ioVar_->ioVariableFilter->text().trimmed() : QString();
    const QString scope =
        ioVar_->ioVariableScopeFilter ? ioVar_->ioVariableScopeFilter->currentData().toString() : QStringLiteral("all");
    const int selected = selectedPosition();
    IoVariableFilterStats stats;
    const QString readyText = uiText("Ready", "就绪");

    // Iterate all rows and apply active filter predicates
    for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
        const IoVariableTableRow variable = ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
        QStringList cells;
        cells.reserve(ioVar_->ioVariableTable->columnCount());
        for (int column = 0; column < ioVar_->ioVariableTable->columnCount(); ++column) {
            const auto* item = ioVar_->ioVariableTable->item(row, column);
            cells << (item ? item->text() : QString());
        }
        const IoVariableFilterDecision decision =
            evaluateIoVariableFilterRow(variable, cells, scope, needle, selected, readyText);
        ioVar_->ioVariableTable->setRowHidden(row, !decision.visible); // show/hide based on filter match
        accumulateIoVariableFilterStats(&stats, decision);
    }

    if (ioVar_->ioVariableSummaryLabel) {
        const QString scopeLabel =
            ioVar_->ioVariableScopeFilter ? ioVar_->ioVariableScopeFilter->currentText() : uiText("All", "全部");
        const QString summaryPattern = uiText("%1/%2 | %3 | process %4 | watch %5 | startup diff %6 | missing %7 | "
                                              "changed %8 | plc issues %9",
                                              "%1/%2 | %3 | 过程 %4 | Watch %5 | 启动偏差 %6 | 缺失 %7 | 变化 %8 | "
                                              "PLC 问题 %9");
        ioVar_->ioVariableSummaryLabel->setText(ioVariableFilterSummaryText(stats, scopeLabel, summaryPattern));
        ioVar_->ioVariableSummaryLabel->setToolTip(
            uiText("I/O Variables merges PDO Map, Free Run process image, Watch values, "
                   "and Startup SDO expectations into one engineering signal table.",
                   "I/O 变量把 PDO 映射、Free Run 过程映像、Watch 值和 Startup SDO 期望"
                   "合并到一张工程信号表。"));
    }
    updateIoVariableRowDetail();
    updateActionAvailability();
}

// ── I/O Variable Row Detail ──────────────────────────────────────────

// — Refresh the I/O variable detail strip for the currently focused row
void MainWindow::updateIoVariableRowDetail() {
    if (!ioVar_->ioVariableDetailLabel) {
        return;
    }
    const IoVariableDetailTexts texts = {
        .unavailableText = uiText("I/O variable evidence is not available.", "当前没有可用的 I/O 变量证据。"),
        .unavailableTip =
            uiText("This preview is local only and does not access the bus.", "此预览仅在本地工作，不访问总线。"),
        .noSelectionText = uiText("Select a visible I/O variable to review signal source, value "
                                  "evidence, Startup comparison, PDO map status, PLC quality, and "
                                  "operation boundary.",
                                  "选择一条可见 I/O 变量，以复核信号来源、值证据、Startup 对照、PDO "
                                  "映射"
                                  "状态、PLC 质量和操作边界。"),
        .noSelectionTip = uiText("Selecting rows, changing scopes, filtering, and reading this detail "
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
        .localBoundary = uiText("Local preview boundary: selecting this row, changing I/O scopes, "
                                "filtering, copying declarations, exporting visible rows, editing "
                                "Alias/Tags/Note, and reading this detail strip do not read SDOs, "
                                "write SDOs, change state, toggle Free Run, or run Host Health.",
                                "本地预览边界：选择此行、切换 I/O 范围、筛选、复制声明、导出可见行、"
                                "编辑别名/标签/备注和查看此详情条都不读取 SDO、不写 SDO、不切换状态、"
                                "不改变 Free Run，也不运行 Host Health。"),
        .executionBoundary = uiText("Execution boundary: Fill SDO only prepares the local target; Read "
                                    "SDO and double-click issue explicit SDO reads; Watch buttons create "
                                    "Watch rows without immediate reads; Startup buttons edit the "
                                    "Startup table until Startup Apply is confirmed.",
                                    "执行边界：填充 SDO 只准备本地目标；读取 SDO 和双击会显式读取 SDO；"
                                    "Watch 按钮只创建 Watch 行且不立即读取；Startup 按钮只编辑 Startup "
                                    "表，"
                                    "直到确认应用启动项。"),
    };

    // Lambda to push UI state changes to the label widget
    auto applyState = [this](const IoVariableDetailState& state) {
        ioVar_->ioVariableDetailLabel->setText(state.text);
        // Set severity property for styling/theming
        ioVar_->ioVariableDetailLabel->setProperty("severity", state.severityKey);
        ioVar_->ioVariableDetailLabel->setToolTip(state.tooltip);
        repolish(ioVar_->ioVariableDetailLabel); // force QSS re-evaluation after property change
    };

    if (!ioVar_->ioVariableTable) {
        applyState(ioVariableDetailUnavailableState(texts));
        return;
    }

    const int row = ioVar_->ioVariableTable->currentRow();
    if (row < 0 || row >= ioVar_->ioVariableTable->rowCount() || ioVar_->ioVariableTable->isRowHidden(row)) {
        applyState(ioVariableDetailNoSelectionState(texts));
        return;
    }

    const IoVariableTableRow variable = ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
    applyState(buildIoVariableDetailState(variable, texts));
}
