// SDO inspector, target panel, evidence trail, and history.

#include "MainWindowIncludes.h"
SdoEvidenceCandidates MainWindow::currentSdoEvidenceCandidates() const {
    const int position = selectedPosition();
    const SdoEvidenceTableRows rows = sdoEvidenceTableRowsForTarget(
        {.dictionaryTable = sdo_->sdoTable,
         .watchTable = watch_->watchTable,
         .startupTable = startupSdoTable_,
         .bookmarkTable = bookmark_->objectBookmarkTable,
         .targetTrailTable = sdoTargetTrailTable_},
        {.position = position,
         .index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString(),
         .subIndex = sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString(),
         .dictionaryLoadedForPosition = loadedSdoPosition_ == position});
    return sdoEvidenceCandidatesFromTables(sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString(),
                                           {.dictionaryTable = sdo_->sdoTable,
                                            .watchTable = watch_->watchTable,
                                            .startupTable = startupSdoTable_,
                                            .bookmarkTable = bookmark_->objectBookmarkTable,
                                            .targetTrailTable = sdoTargetTrailTable_},
                                           rows,
                                           {.readValue = uiText("Read Value", "读回值"),
                                            .watch = uiText("Watch", "Watch"),
                                            .dictionary = uiText("OD Evidence", "OD 证据"),
                                            .startup = uiText("Startup SDO", "Startup SDO"),
                                            .bookmark = uiText("Object Bookmark", "对象书签"),
                                            .targetTrailWrite = uiText("Target Trail Write", "目标轨迹写入"),
                                            .targetTrail = uiText("Target Trail", "目标轨迹")});
}


// — Check whether current sdo evidence has conflict
bool MainWindow::currentSdoEvidenceHasConflict() const {
    return sdoEvidenceHasConflict(currentSdoEvidenceCandidates());
}


// — Check whether current sdo write delta review available
bool MainWindow::currentSdoWriteDeltaReviewAvailable() const {
    if (selectedPosition() < 0 || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
        return false;
    }
    if (currentSdoEvidenceHasConflict()) {
        return true;
    }
    if (!sdoInspector_->sdoWriteValue || sdoInspector_->sdoWriteValue->text().trimmed().isEmpty()) {
        return false;
    }
    return sdoWriteDeltaReviewEvidenceAvailable(sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString(),
                                                {.dictionaryTable = sdo_->sdoTable,
                                                 .watchTable = watch_->watchTable,
                                                 .startupTable = startupSdoTable_,
                                                 .bookmarkTable = bookmark_->objectBookmarkTable,
                                                 .targetTrailTable = sdoTargetTrailTable_},
                                                {.dictionaryRow = currentSdoDictionaryRow(),
                                                 .watchRow = currentSdoWatchRow(),
                                                 .startupRow = currentSdoStartupRow(),
                                                 .bookmarkRow = currentSdoBookmarkRow(),
                                                 .targetTrailRow = currentSdoTargetTrailRow()});
}


// — Compare the current SDO target value against the last read value
void MainWindow::reviewCurrentSdoWriteDelta() {
    if (!currentSdoWriteDeltaReviewAvailable()) {
        updateDiagnostics("Info", "Navigation",
                          uiText("No local Write Delta evidence for current SDO "
                                 "target",
                                 "当前 SDO 目标没有可审阅的本地写入差异证据"));
        return;
    }

    const QString writeValue =
        sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text().trimmed() : QString();
    const QString normalizedWrite = normalizeComparableValue(writeValue);
    auto differsFromWrite = [&normalizedWrite](const QString& value) {
        const QString normalized = normalizeComparableValue(value);
        return !normalized.isEmpty() && normalized != normalizedWrite;
    };
    const auto candidates = currentSdoEvidenceCandidates();
    QString baselineEvidence;
    auto conflictsWithBaseline = [&baselineEvidence](const QString& value) {
        const QString normalized = normalizeComparableValue(value);
        if (normalized.isEmpty()) {
            return false;
        }
        if (baselineEvidence.isEmpty()) {
            baselineEvidence = normalized;
            return false;
        }
        return normalized != baselineEvidence;
    };
    if (writeValue.isEmpty()) {
        for (const auto& candidate : candidates) {
            conflictsWithBaseline(candidate.second);
        }
    }
    auto shouldReviewValue = [&writeValue, &differsFromWrite, &conflictsWithBaseline](const QString& value) {
        return writeValue.isEmpty() ? conflictsWithBaseline(value) : differsFromWrite(value);
    };

    const int watchRow = currentSdoWatchRow();
    if (watchRow >= 0 && shouldReviewValue(tableText(watch_->watchTable, watchRow, 4))) {
        openCurrentSdoWatchLink();
        updateDiagnostics("Info", "Navigation",
                          writeValue.isEmpty()
                              ? uiText("Reviewing Evidence Set conflict in Watch", "正在 Watch 中审阅证据集冲突")
                              : uiText("Reviewing Write Delta in Watch evidence", "正在 Watch 证据中审阅写入差异"));
        return;
    }

    const int startupRow = currentSdoStartupRow();
    if (startupRow >= 0 && shouldReviewValue(tableText(startupSdoTable_, startupRow, 3))) {
        openCurrentSdoStartupLink();
        updateDiagnostics("Info", "Navigation",
                          writeValue.isEmpty() ? uiText("Reviewing Evidence Set conflict in Startup "
                                                        "SDO",
                                                        "正在 Startup SDO 中审阅证据集冲突")
                                               : uiText("Reviewing Write Delta in Startup SDO "
                                                        "evidence",
                                                        "正在 Startup SDO 证据中审阅写入差异"));
        return;
    }

    const int bookmarkRow = currentSdoBookmarkRow();
    const SdoObjectBookmarkRow bookmark = sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, bookmarkRow);
    if (bookmarkRow >= 0 && shouldReviewValue(bookmark.lastValue)) {
        openCurrentSdoBookmarkLink();
        updateDiagnostics("Info", "Navigation",
                          writeValue.isEmpty() ? uiText("Reviewing Evidence Set conflict in Object "
                                                        "Bookmark",
                                                        "正在对象书签中审阅证据集冲突")
                                               : uiText("Reviewing Write Delta in Object Bookmark "
                                                        "evidence",
                                                        "正在对象书签证据中审阅写入差异"));
        return;
    }

    const int trailRow = currentSdoTargetTrailRow();
    const SdoTargetTrailRow trail = sdoTargetTrailRowFromTable(sdoTargetTrailTable_, trailRow);
    if (trailRow >= 0 && (shouldReviewValue(trail.writeValue) || shouldReviewValue(trail.value))) {
        openCurrentSdoTargetTrailLink();
        updateDiagnostics("Info", "Navigation",
                          writeValue.isEmpty() ? uiText("Reviewing Evidence Set conflict in SDO "
                                                        "Target Trail",
                                                        "正在 SDO 目标轨迹中审阅证据集冲突")
                                               : uiText("Reviewing Write Delta in SDO Target Trail "
                                                        "evidence",
                                                        "正在 SDO 目标轨迹证据中审阅写入差异"));
        return;
    }

    if (sdoInspector_->sdoValue && shouldReviewValue(sdoInspector_->sdoValue->text())) {
        activateWorkspaceTab(objectDictionaryTabIndex_);
        sdoInspector_->sdoValue->setFocus();
        sdoInspector_->sdoValue->selectAll();
        updateDiagnostics("Info", "Navigation",
                          writeValue.isEmpty() ? uiText("Reviewing Evidence Set conflict in current "
                                                        "read-back field",
                                                        "正在当前读回字段中审阅证据集冲突")
                                               : uiText("Reviewing Write Delta in current read-back "
                                                        "field",
                                                        "正在当前读回字段中审阅写入差异"));
        return;
    }

    const int dictionaryRow = currentSdoDictionaryRow();
    if (dictionaryRow >= 0) {
        activateObjectDictionaryPaneFor(sdo_->sdoTable);
        if (sdo_->sdoFilter && sdo_->sdoTable->isRowHidden(dictionaryRow)) {
            sdo_->sdoFilter->clear();
            filterSdoTable(QString());
        }
        sdo_->sdoTable->clearSelection();
        selectAndFocusTableRow(sdo_->sdoTable, dictionaryRow, 7);
        updateDiagnostics("Info", "Navigation",
                          uiText("Reviewing Write Delta in Object Dictionary "
                                 "evidence",
                                 "正在对象字典证据中审阅写入差异"));
        return;
    }

    activateWorkspaceTab(objectDictionaryTabIndex_);
    if (sdoInspector_->sdoValue) {
        sdoInspector_->sdoValue->setFocus();
        sdoInspector_->sdoValue->selectAll();
    }
    updateDiagnostics("Info", "Navigation",
                      uiText("Reviewing Write Delta in current read-back field", "正在当前读回字段中审阅写入差异"));
}


// — Populate the SDO read/write controls from the selected target panel row
void MainWindow::ensureSdoTargetTrailTable() {
    if (!sdoTargetTrailTable_) {
        return;
    }
    if (sdoTargetTrailTable_->columnCount() != 9) {
        sdoTargetTrailTable_->setColumnCount(9);
    }
    // Define column headers for the table
    sdoTargetTrailTable_->setHorizontalHeaderLabels(
        {uiText("Time", "时间"), uiText("Slave", "从站"), uiText("Index", "索引"), uiText("Sub", "子项"),
         uiText("Type", "类型"), uiText("Source", "来源"), uiText("Value", "值"), uiText("Write", "写入值"),
         uiText("Detail", "详情")});
    sdoTargetTrailTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    sdoTargetTrailTable_->horizontalHeader()->setStretchLastSection(true);
}


// — Refresh the SDO target trail detail strip for the focused row
void MainWindow::updateSdoTargetTrailRowDetail() {
    if (!sdoTargetTrailDetailLabel_) {
        return;
    }
    const SdoTargetTrailDetailTexts texts = sdoTargetTrailDetailTexts();
    // Lambda to push UI state changes to the label widget
    auto applyState = [this](const SdoTargetTrailDetailState& state) {
        sdoTargetTrailDetailLabel_->setText(state.text);
        // Set severity property for styling/theming
        sdoTargetTrailDetailLabel_->setProperty("severity", state.severityKey);
        sdoTargetTrailDetailLabel_->setToolTip(state.tooltip);
        repolish(sdoTargetTrailDetailLabel_); // force QSS re-evaluation after property change
    };

    if (!sdoTargetTrailTable_) {
        applyState(sdoTargetTrailDetailUnavailableState(texts));
        return;
    }

    const int row = sdoTargetTrailTable_->currentRow();
    if (row < 0 || row >= sdoTargetTrailTable_->rowCount() || sdoTargetTrailTable_->isRowHidden(row)) {
        applyState(sdoTargetTrailDetailNoSelectionState(texts));
        return;
    }

    const SdoTargetTrailRow trail = sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
    applyState(buildSdoTargetTrailDetailState(trail, sdoTargetTrailRowCanCreateStartup(row), texts));
}


// — Store the current SDO target address in the target trail history
void MainWindow::rememberCurrentSdoTarget(const QString& source, const QString& detail) {
    if (!sdoTargetTrailTable_) {
        return;
    }
    const int position = selectedPosition();
    const QString index =
        // Normalize hex address for consistent comparison
        normalizeHexText(sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString(), 4);
    // Normalize hex address for consistent comparison
    const QString subIndex =
        normalizeHexText(sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString(), 2);
    if (position < 0 || index.isEmpty() || subIndex.isEmpty()) {
        return;
    }
    ensureSdoTargetTrailTable();

    const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
    const QString value = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
    const QString writeValue =
        sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text().trimmed() : QString();
    const QString sourceText = source.trimmed().isEmpty() ? uiText("Manual fields", "手动字段") : source.trimmed();
    const QString detailText = detail.trimmed();
    const QString key = sdoTargetTrailRowKey(position, index, subIndex, type, sourceText, detailText);

    for (int row = 0; row < sdoTargetTrailTable_->rowCount(); ++row) {
        if (sdoTargetTrailRowKeyFromTable(sdoTargetTrailTable_, row) != key) {
            continue;
        }
        sdoTargetTrailTable_->removeRow(row);
        break;
    }
    rememberedSdoTargetTrailKeys_.insert(key);

    sdoTargetTrailTable_->insertRow(0);
    const QStringList values = {
        QDateTime::currentDateTime().toString("HH:mm:ss"),
        QString::number(position),
        index,
        subIndex,
        type,
        sourceText,
        value,
        writeValue,
        detailText,
    };
    for (int column = 0; column < values.size(); ++column) {
        // Create table cell
        auto* item = new QTableWidgetItem(values.at(column));
        item->setToolTip(values.at(column));
        if (column == 5) {
            // Define color for visual feedback
            item->setForeground(settings_.theme == "Light" ? QColor("#1d4ed8")
                                                           // Define color for visual feedback
                                                           : QColor("#93c5fd"));
        }
        sdoTargetTrailTable_->setItem(0, column, item);
    }
    while (sdoTargetTrailTable_->rowCount() > 40) {
        const int lastRow = sdoTargetTrailTable_->rowCount() - 1;
        rememberedSdoTargetTrailKeys_.remove(sdoTargetTrailRowKeyFromTable(sdoTargetTrailTable_, lastRow));
        sdoTargetTrailTable_->removeRow(lastRow);
    }
    sdoTargetTrailTable_->resizeColumnsToContents(); // auto-fit column widths
    updateSdoTargetTrailRowDetail();
    updateActionAvailability();
}


// — Check whether prepare sdo target trail row
bool MainWindow::prepareSdoTargetTrailRow(int row, bool reportRestoreSuccess) {
    if (!sdoTargetTrailTable_ || row < 0 || row >= sdoTargetTrailTable_->rowCount()) {
        updateDiagnostics("Info", "SDO Target Trail",
                          uiText("Select a target trail row first", "请先选择一条目标轨迹"));
        return false;
    }

    const SdoTargetTrailRow trail = sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
    if (!sdoTargetTrailRowHasTarget(trail)) {
        return false;
    }

    if (!selectSlaveForLocalEvidence(trail.position)) {
        updateDiagnostics("Warning", "SDO Target Trail",
                          uiText("Target slave #%1 is not in the current topology", "目标从站 #%1 不在当前拓扑中")
                              .arg(trail.position));
        return false;
    }

    {
        const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex);       // prevent recursive signal updates
        const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
        const QSignalBlocker typeBlocker(sdoInspector_->sdoType);         // prevent recursive signal updates
        const QSignalBlocker valueBlocker(sdoInspector_->sdoValue);       // prevent recursive signal updates
        const QSignalBlocker writeBlocker(sdoInspector_->sdoWriteValue);  // prevent recursive signal updates
        if (sdoInspector_->sdoIndex) {
            sdoInspector_->sdoIndex->setText(trail.index);
        }
        if (sdoInspector_->sdoSubIndex) {
            sdoInspector_->sdoSubIndex->setText(trail.subIndex);
        }
        if (sdoInspector_->sdoType && !trail.type.isEmpty()) {
            const QString normalized = trail.type.toLower().replace(' ', "_");
            const int typeIndex = sdoInspector_->sdoType->findText(normalized, Qt::MatchFixedString);
            sdoInspector_->sdoType->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
        }
        if (sdoInspector_->sdoValue) {
            sdoInspector_->sdoValue->setText(trail.value);
            sdoInspector_->sdoValue->setPlaceholderText(trail.value.isEmpty()
                                                            ? uiText("No trail value", "轨迹暂无值")
                                                            : uiText("Value from target trail", "来自目标轨迹的值"));
        }
    }

    selectedSdoWritable_ = true;
    const int dictionaryRow = currentSdoDictionaryRow();
    if (dictionaryRow >= 0) {
        selectedSdoWritable_ = sdoDictionaryRowIsWritable(sdoDictionaryRowFromTable(sdo_->sdoTable, dictionaryRow));
    } else {
        const int bookmarkRow = currentSdoBookmarkRow();
        if (bookmarkRow >= 0) {
            selectedSdoWritable_ = !sdoObjectAccessIsReadOnly(
                sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, bookmarkRow).access,
                uiText("只读", "只读"));
        }
    }
    if (sdoInspector_->sdoWriteValue) {
        sdoInspector_->sdoWriteValue->setEnabled(selectedSdoWritable_);
        sdoInspector_->sdoWriteValue->setText(selectedSdoWritable_ ? trail.writeValue : QString());
        sdoInspector_->sdoWriteValue->setPlaceholderText(
            selectedSdoWritable_
                ? (trail.writeValue.isEmpty() ? uiText("Value to write", "写入值")
                                              : uiText("Write value from target trail", "来自目标轨迹的写入值"))
                : uiText("Read-only object", "只读对象"));
    }

    sdoTargetTrailTable_->selectRow(row);
    updateSdoInspector(uiText("SDO Target Trail", "SDO 目标轨迹"),
                       trail.source.isEmpty() ? trail.detail : QString("%1 | %2").arg(trail.source, trail.detail));
    if (reportRestoreSuccess) {
        updateDiagnostics("Info", "SDO Target Trail",
                          uiText("Restored local SDO target #%1 %2:%3 without bus "
                                 "access",
                                 "已本地恢复 SDO 目标 #%1 %2:%3，未访问总线")
                              .arg(trail.position)
                              .arg(sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : trail.index,
                                   sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : trail.subIndex));
    }
    updateActionAvailability();
    return true;
}


// — Return the sdo target trail row startup value
QString MainWindow::sdoTargetTrailRowStartupValue(int row) const {
    return sdoTargetTrailStartupValueFromTable(sdoTargetTrailTable_, row);
}


// — Check whether sdo target trail row can create startup
bool MainWindow::sdoTargetTrailRowCanCreateStartup(int row) const {
    const SdoTargetTrailRow trail = sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
    if (!sdoTargetTrailRowHasTarget(trail) || ::sdoTargetTrailRowStartupValue(trail).isEmpty()) {
        return false;
    }

    if (sdo_->sdoTable && loadedSdoPosition_ == trail.position) {
        for (int dictionaryRow = 0; dictionaryRow < sdo_->sdoTable->rowCount(); ++dictionaryRow) {
            if (tableObjectIndexMatches(sdo_->sdoTable, dictionaryRow, trail.index, trail.subIndex, 1, 2)) {
                return sdoDictionaryRowIsWritable(sdoDictionaryRowFromTable(sdo_->sdoTable, dictionaryRow));
            }
        }
    }

    if (bookmark_->objectBookmarkTable) {
        for (int bookmarkRow = 0; bookmarkRow < bookmark_->objectBookmarkTable->rowCount(); ++bookmarkRow) {
            if (!tableObjectAddressMatches(bookmark_->objectBookmarkTable, bookmarkRow, trail.position, trail.index,
                                           trail.subIndex, 0, 2, 3)) {
                continue;
            }
            return !sdoObjectAccessIsReadOnly(
                sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, bookmarkRow).access,
                uiText("只读", "只读"));
        }
    }

    return true;
}


// — Restore the SDO target panel from a historical trail entry
void MainWindow::restoreSdoTargetTrailRow(int row) {
    prepareSdoTargetTrailRow(row, true);
}


// — Add sdo target trail row to watch
void MainWindow::addSdoTargetTrailRowToWatch() {
    const int row = sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1;
    if (!prepareSdoTargetTrailRow(row, false)) {
        return;
    }
    const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
    const QString subIndex = sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
    const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
    const QString value = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
    addCurrentSdoToWatch(false);
    if (watch_->watchTable && watch_->watchTable->currentRow() >= 0) {
        const int watchRow = watch_->watchTable->currentRow();
        if (!value.isEmpty()) {
            // Create table cell
            watch_->watchTable->setItem(watchRow, 4, new QTableWidgetItem(value));
            watchValues_[QString("%1|%2|%3").arg(selectedPosition()).arg(index, subIndex)] = value;
        }
        watch_->watchTable->setItem(
            watchRow, 5,
            // Create table cell
            new QTableWidgetItem(decodeWatchValue(index, subIndex, type,
                                                  value.isEmpty() ? tableText(watch_->watchTable, watchRow, 4) : value,
                                                  uiText("Target Trail", "目标轨迹"))));
        // Create table cell
        watch_->watchTable->setItem(watchRow, 6, new QTableWidgetItem(type));
        watch_->watchTable->setItem(
            // Create table cell
            watchRow, 7, new QTableWidgetItem(uiText("Target Trail", "目标轨迹")));
        updateWatchBaselineDelta(watchRow);
        updateWatchStartupDelta(watchRow);
    }
    if (watch_->watchTable) {
        watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
    }
    updateDiagnostics("Info", "SDO Target Trail",
                      uiText("Added selected target trail row to Watch without "
                             "immediate reads",
                             "已将所选目标轨迹行加入 Watch，未立即读取"));
    activateWorkspaceTab(watchTabIndex_);
}


// — Bookmark sdo target trail row
void MainWindow::bookmarkSdoTargetTrailRow() {
    const int row = sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1;
    if (!prepareSdoTargetTrailRow(row, false)) {
        return;
    }
    const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
    const QString subIndex = sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
    const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
    const QString value = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
    const SdoTargetTrailRow trail = sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
    addObjectBookmark(selectedPosition(), index, subIndex,
                      selectedSdoWritable_ ? uiText("rw", "读写") : uiText("ro", "只读"), type, QString(),
                      trail.detail.isEmpty() ? trail.source : trail.detail, value,
                      uiText("SDO Target Trail", "SDO 目标轨迹"));
    updateDiagnostics("Info", "SDO Target Trail",
                      uiText("Bookmarked selected target trail row without bus "
                             "access",
                             "已收藏所选目标轨迹行，未访问总线"));
}


// — Add sdo target trail row to startup
void MainWindow::addSdoTargetTrailRowToStartup() {
    const int row = sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1;
    if (!sdoTargetTrailRowCanCreateStartup(row)) {
        updateDiagnostics("Warning", "SDO Target Trail",
                          uiText("Selected target trail row has no writable value for Startup "
                                 "SDO creation",
                                 "所选目标轨迹行没有可用于创建 Startup SDO 的可写值"));
        return;
    }
    const QString value = sdoTargetTrailRowStartupValue(row);
    if (!prepareSdoTargetTrailRow(row, false)) {
        return;
    }
    if (!selectedSdoWritable_) {
        updateDiagnostics("Warning", "SDO Target Trail",
                          uiText("Selected target trail row resolves to a "
                                 "read-only object",
                                 "所选目标轨迹行被识别为只读对象"));
        return;
    }
    if (sdoInspector_->sdoWriteValue) {
        sdoInspector_->sdoWriteValue->setEnabled(true);
        sdoInspector_->sdoWriteValue->setText(value);
        sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value from target trail", "来自目标轨迹的值"));
    }
    addStartupSdo();
    if (startupSdoTable_ && startupSdoTable_->currentRow() >= 0) {
        const int startupRow = startupSdoTable_->currentRow();
        if (startupSdoTable_->columnCount() > 5) {
            startupSdoTable_->setItem(startupRow, 5,
                                      // Create table cell
                                      new QTableWidgetItem(uiText("From Target Trail", "来自目标轨迹")));
        }
        if (startupSdoTable_->columnCount() > 6) {
            startupSdoTable_->setItem(
                startupRow, 6,
                // Create table cell
                new QTableWidgetItem(
                    uiText("Created from SDO Target Trail row %1", "由 SDO 目标轨迹第 %1 行创建").arg(row + 1)));
        }
    }
    updateDiagnostics("Info", "Startup SDO",
                      uiText("Created a local Startup SDO candidate from the "
                             "selected target trail row",
                             "已从所选目标轨迹行创建本地 Startup SDO 候选"));
    activateWorkspaceTab(startupSdoTabIndex_);
}


// — Remove selected sdo target trail rows
void MainWindow::removeSelectedSdoTargetTrailRows() {
    if (!sdoTargetTrailTable_ || !sdoTargetTrailTable_->selectionModel()) {
        return;
    }
    QVector<int> rows = selectedTableRows(sdoTargetTrailTable_);
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (const int row : rows) {
        if (row >= 0 && row < sdoTargetTrailTable_->rowCount()) {
            sdoTargetTrailTable_->removeRow(row);
        }
    }
    rememberedSdoTargetTrailKeys_ = sdoTargetTrailKeysFromTable(sdoTargetTrailTable_);
    updateDiagnostics("Info", "SDO Target Trail",
                      uiText("Removed selected local target trail row(s)", "已移除所选本地目标轨迹行"));
    updateSdoTargetTrailRowDetail();
    updateActionAvailability();
}


// Remove all entries from the SDO target trail history table
void MainWindow::clearSdoTargetTrail() {
    if (!sdoTargetTrailTable_) {
        return;
    }
    sdoTargetTrailTable_->clearContents();
    sdoTargetTrailTable_->setRowCount(0);
    rememberedSdoTargetTrailKeys_.clear();
    ensureSdoTargetTrailTable();
    updateDiagnostics("Info", "SDO Target Trail", uiText("Cleared local SDO target trail", "已清空本地 SDO 目标轨迹"));
    updateSdoTargetTrailRowDetail();
    updateActionAvailability();
}


// — Update sdo table evidence
void MainWindow::updateSdoTableEvidence(int position, const QString& index, const QString& subIndex,
                                        const QString& value, const QString& status, const QString& detail) {
    if (position < 0 || index.trimmed().isEmpty() || subIndex.trimmed().isEmpty()) {
        return;
    }

    // Normalize hex address for consistent comparison
    const QString normalizedIndex = normalizeHexText(index, 4);
    // Normalize hex address for consistent comparison
    const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
    const QString key = sdoEvidenceKey(position, index, subIndex);
    const QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    sdoEvidence_.insert(key, {value, status, detail, time});

    if (!sdo_->sdoTable || selectedPosition() != position) {
        return;
    }

    const QColor statusColor = status == uiText("Complete", "完成") || status == uiText("OK", "成功") ||
                                       status == uiText("Write OK", "写入完成")
                                   // Define color for visual feedback
                                   ? QColor("#22c55e")
                                   // Define color for visual feedback
                                   : (status == uiText("Failed", "失败") ? QColor("#ef4444")
                                                                         // Define color for visual feedback
                                                                         : QColor("#f59e0b"));
    const QColor valueBackground =
        // Define color for visual feedback
        settings_.theme == "Light" ? QColor("#eef2ff") : QColor("#172036");

    for (int row = 0; row < sdo_->sdoTable->rowCount(); ++row) {
        if (!tableObjectIndexMatches(sdo_->sdoTable, row, normalizedIndex, normalizedSubIndex, 1, 2)) {
            continue;
        }

        auto ensureItem = [this, row](int column) {
            auto* item = sdo_->sdoTable->item(row, column);
            if (!item) {
                // Create table cell
                item = new QTableWidgetItem;
                sdo_->sdoTable->setItem(row, column, item);
            }
            return item;
        };
        auto* valueItem = ensureItem(7);
        auto* statusItem = ensureItem(8);
        valueItem->setText(value);
        valueItem->setToolTip(detail.isEmpty() ? time : QString("%1\n%2").arg(time, detail));
        valueItem->setBackground(valueBackground);
        statusItem->setText(QString("%1  %2").arg(status, time));
        statusItem->setToolTip(detail);
        statusItem->setForeground(statusColor);
        break;
    }

    if (sdo_->sdoTable->currentRow() >= 0) {
        updateSdoInspector(uiText("OD evidence", "OD 证据"), detail);
    }
    updateActionAvailability();
}


// — Use read sdo value for write
void MainWindow::useReadSdoValueForWrite() {
    if (!sdoInspector_->sdoValue || !sdoInspector_->sdoWriteValue || !selectedSdoWritable_) {
        return;
    }
    const QString value = sdoInspector_->sdoValue->text().trimmed();
    if (value.isEmpty()) {
        return;
    }
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setText(value);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value copied from read-back", "已从读回值复制"));
    updateDiagnostics("Info", "SDO",
                      QString("Copied read value into write field %1:%2 = %3")
                          .arg(sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString(),
                               sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString(), value));
    updateSdoInspector(uiText("Read value", "读回值"),
                       uiText("Read value copied to write field", "读回值已复制到写入框"));
    updateActionAvailability();
}


// — Use preferred sdo evidence for write
void MainWindow::usePreferredSdoEvidenceForWrite() {
    if (!sdoInspector_->sdoWriteValue || !selectedSdoWritable_) {
        return;
    }
    QString source;
    const QString value = currentSdoPreferredEvidenceValue(&source);
    if (value.isEmpty()) {
        updateDiagnostics("Info", "SDO",
                          uiText("No local SDO evidence value is available for the "
                                 "current target",
                                 "当前目标没有可用的本地 SDO 证据值"));
        return;
    }
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setText(value);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value copied from %1", "已从 %1 复制数值").arg(source));
    updateDiagnostics("Info", "SDO",
                      uiText("Copied %1 evidence into write field %2:%3 = %4", "已把 %1 证据复制到写入框 %2:%3 = %4")
                          .arg(source, sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString(),
                               sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString(), value));
    updateSdoInspector(uiText("Local evidence", "本地证据"),
                       uiText("%1 copied to write field", "已把 %1 复制到写入框").arg(source));
    updateActionAvailability();
}


// — Pick sdo evidence for write
void MainWindow::pickSdoEvidenceForWrite() {
    if (!sdoInspector_->sdoWriteValue || !selectedSdoWritable_) {
        return;
    }
    const auto candidates = currentSdoEvidenceCandidates();
    if (candidates.isEmpty()) {
        updateDiagnostics("Info", "SDO",
                          uiText("No local SDO evidence value is available for the "
                                 "current target",
                                 "当前目标没有可用的本地 SDO 证据值"));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(uiText("Choose SDO Evidence", "选择 SDO 证据"));
    auto* layout = new QVBoxLayout(&dialog);
    auto* summary = new QLabel(uiText("Choose one local evidence value for the write field. This does "
                                      "not read or write the bus.",
                                      "选择一个本地证据值填入写入框；不会读取或写入总线。"));
    summary->setWordWrap(true);
    layout->addWidget(summary);

    auto* list = new QListWidget;
    list->setObjectName("sdoEvidenceCandidateList");
    for (int i = 0; i < candidates.size(); ++i) {
        const auto& candidate = candidates.at(i);
        auto* item = new QListWidgetItem(QString("%1  =  %2").arg(candidate.first, candidate.second));
        item->setData(Qt::UserRole, candidate.second);
        item->setData(Qt::UserRole + 1, candidate.first);
        if (i == 0) {
            item->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
        }
        list->addItem(item);
    }
    list->setCurrentRow(0);
    layout->addWidget(list);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(uiText("Use Evidence", "使用证据"));
    buttons->button(QDialogButtonBox::Cancel)->setText(uiText("Cancel", "取消"));
    layout->addWidget(buttons);
    // Connect QDialogButtonBox::accepted signal to handler
    connect(buttons, &QDialogButtonBox::accepted, &dialog,
            &QDialog::accept); // wire signal to slot
                               // Connect QDialogButtonBox::rejected signal to handler
    connect(buttons, &QDialogButtonBox::rejected, &dialog,
            &QDialog::reject); // wire signal to slot
                               // Connect QListWidget::itemDoubleClicked signal to handler
    connect(list, &QListWidget::itemDoubleClicked, &dialog, &QDialog::accept); // wire signal to slot

    if (dialog.exec() != QDialog::Accepted || !list->currentItem()) {
        return;
    }

    const QString value = list->currentItem()->data(Qt::UserRole).toString();
    const QString source = list->currentItem()->data(Qt::UserRole + 1).toString();
    if (value.trimmed().isEmpty()) {
        return;
    }
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setText(value.trimmed());
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value chosen from %1", "已从 %1 选择数值").arg(source));
    updateDiagnostics("Info", "SDO",
                      uiText("Selected %1 evidence for write field %2:%3 = %4", "已选择 %1 证据作为写入值 %2:%3 = %4")
                          .arg(source, sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString(),
                               sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString(),
                               value.trimmed()));
    updateSdoInspector(uiText("Local evidence", "本地证据"),
                       uiText("%1 chosen for write field", "已选择 %1 作为写入值").arg(source));
    updateActionAvailability();
}


// Execute an SDO write operation for the currently selected object with the typed value
void MainWindow::writeCurrentSdo() {
    if (!selectedSdoWritable_) {
        updateDiagnostics("Warning", "SDO",
                          uiText("Write blocked: selected object is read-only", "写入已阻止：所选对象为只读"));
        updateActionAvailability();
        return;
    }
    if (selectedPosition() < 0) {
        return;
    }

    QStringList validationErrors;
    QStringList validationWarnings;
    validateSdoAddressAndValue(sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text(),
                               sdoInspector_->sdoWriteValue->text(), sdoInspector_->sdoType->currentText(),
                               &validationErrors, &validationWarnings);
    if (!validationErrors.isEmpty()) {
        updateDiagnostics(
            "Error", "SDO",
            uiText("Write blocked by validation: %1", "写入已被校验阻止：%1").arg(validationErrors.join("; ")));
        QMessageBox::warning(this, uiText("SDO Validation Failed", "SDO 校验失败"), validationErrors.join("\n"));
        return;
    }

    QStringList details = {
        uiText("Master: %1", "主站：%1").arg(activeMasterName()),
        uiText("Slave: #%1", "从站：#%1").arg(selectedPosition()),
        uiText("Object: %1:%2", "对象：%1:%2").arg(sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text()),
        uiText("Type: %1", "类型：%1")
            .arg(sdoInspector_->sdoType->currentText().isEmpty() ? uiText("default", "默认")
                                                                 : sdoInspector_->sdoType->currentText()),
        uiText("Value: %1", "值：%1").arg(sdoInspector_->sdoWriteValue->text()),
        uiText("SDO writes can change persistent parameters or output behavior.",
               "SDO 写入可能改变持久参数或输出行为。"),
    };
    details << sdoWriteImpactDetails(selectedPosition(), sdoInspector_->sdoIndex->text(),
                                     sdoInspector_->sdoSubIndex->text(), sdoInspector_->sdoWriteValue->text(),
                                     sdoInspector_->sdoType->currentText());
    if (!validationWarnings.isEmpty()) {
        details << uiText("Validation warning: %1", "校验警告：%1").arg(validationWarnings.join("; "));
    }
    // Safety gate: require explicit confirmation before bus write
    if (!confirmDangerousOperation(
            uiText("Confirm SDO Write", "确认 SDO 写入"),
            uiText("This operation writes a value to the selected slave object.", "此操作会向选中从站对象写入数值。"),
            details, uiText("Write SDO", "写入 SDO"))) {
        return;
    }

    const QString writeKey =
        sdoEvidenceKey(selectedPosition(), sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text());
    pendingSdoWrites_.insert(writeKey, {QString::number(selectedPosition()), sdoInspector_->sdoIndex->text(),
                                        sdoInspector_->sdoSubIndex->text(), sdoInspector_->sdoType->currentText(),
                                        sdoInspector_->sdoWriteValue->text()});
    updateSdoTableEvidence(selectedPosition(), sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text(),
                           sdoInspector_->sdoWriteValue->text(), uiText("Write Pending", "写入待确认"),
                           uiText("Waiting for runtime download completion; read-back verification "
                                  "will run automatically.",
                                  "等待运行时写入完成；随后会自动读回校验。"));
    appendSdoHistory(uiText("Write", "写入"), selectedPosition(), sdoInspector_->sdoIndex->text(),
                     sdoInspector_->sdoSubIndex->text(), sdoInspector_->sdoType->currentText(),
                     sdoInspector_->sdoWriteValue->text(), uiText("Requested", "已请求"),
                     uiText("Manual SDO write; automatic read-back verification "
                            "will follow",
                            "手动 SDO 写入；随后自动读回校验"));
    client_.download(selectedPosition(), sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text(),
                     sdoInspector_->sdoWriteValue->text(), sdoInspector_->sdoType->currentText());
    updateSdoInspector(uiText("SDO write requested", "SDO 写入已请求"),
                       uiText("Automatic read-back verification will run after download "
                              "completion",
                              "写入完成后会自动读回校验"));
    updateDiagnostics("Info", "SDO",
                      QString("Download requested %1:%2 = %3; automatic "
                              "read-back verification will follow")
                          .arg(sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text(),
                               sdoInspector_->sdoWriteValue->text()));
}


// — Prepare cia 402 controlword
void MainWindow::prepareCia402Controlword(const QString& label, const QString& value) {
    if (selectedPosition() < 0) {
        return;
    }
    activateObjectDictionaryPaneFor(sdo_->sdoTable);
    {
        const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex);       // prevent recursive signal updates
        const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
        const QSignalBlocker typeBlocker(sdoInspector_->sdoType);         // prevent recursive signal updates
        if (sdoInspector_->sdoIndex) {
            sdoInspector_->sdoIndex->setText("0x6040");
        }
        if (sdoInspector_->sdoSubIndex) {
            sdoInspector_->sdoSubIndex->setText("0x00");
        }
        if (sdoInspector_->sdoType) {
            sdoInspector_->sdoType->setCurrentText("uint16");
        }
    }
    selectedSdoWritable_ = true;
    if (sdoInspector_->sdoWriteValue) {
        sdoInspector_->sdoWriteValue->setEnabled(true);
        sdoInspector_->sdoWriteValue->setText(value);
        sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("CiA 402 controlword", "CiA 402 控制字"));
    }
    updateSdoInspector(uiText("CiA 402", "CiA 402"), uiText("Prepared controlword %1", "已准备控制字 %1").arg(label));
    rememberCurrentSdoTarget(uiText("CiA 402", "CiA 402"),
                             uiText("Prepared controlword %1", "已准备控制字 %1").arg(label));
    updateDiagnostics("Info", "SDO",
                      QString("Prepared CiA 402 controlword %1 for slave #%2: 0x6040:0x00 = %3")
                          .arg(label)
                          .arg(selectedPosition())
                          .arg(value));
    updateActionAvailability();
    writeCurrentSdo();
}


// — Check whether recommended cia 402 controlword
bool MainWindow::recommendedCia402Controlword(QString* label, QString* value, QString* reason) const {
    const int position = selectedPosition();
    if (position < 0 || !watch_->watchTable) {
        return false;
    }

    const Cia402ControlwordRecommendation recommendation =
        selectedDriveControlwordRecommendation(watchStartupWatchRows(watch_->watchTable), position);
    if (recommendation.label.isEmpty() || recommendation.value.isEmpty()) {
        return false;
    }

    if (label) {
        *label = recommendation.label;
    }
    if (value) {
        *value = recommendation.value;
    }
    if (reason) {
        *reason = recommendation.reason;
    }
    return true;
}


// — Check whether validate sdo address and value
bool MainWindow::validateSdoAddressAndValue(const QString& index, const QString& subIndex, const QString& value,
                                            const QString& type, QStringList* errors, QStringList* warnings) const {
    auto addError = [errors](const QString& message) {
        if (errors) {
            errors->append(message);
        }
    };
    auto addWarning = [warnings](const QString& message) {
        if (warnings) {
            warnings->append(message);
        }
    };
    // Parse a numeric string to double for delta comparison
    auto parseNumber = [](QString text, int* baseOut = nullptr) {
        text = text.trimmed();
        int base = 10;
        if (text.startsWith("0x", Qt::CaseInsensitive)) {
            base = 16;
            text = text.mid(2);
        }
        bool ok = false;
        const quint64 result = text.toULongLong(&ok, base);
        if (baseOut) {
            *baseOut = base;
        }
        return QPair<bool, quint64>{ok, result};
    };

    const auto indexParsed = parseNumber(index);
    if (index.trimmed().isEmpty()) {
        addError(uiText("empty index", "索引为空"));
    } else if (!indexParsed.first || indexParsed.second > 0xffff) {
        addError(
            uiText("index must be 0x0000..0xffff or decimal 0..65535", "索引必须是 0x0000..0xffff 或十进制 0..65535"));
    }

    const auto subParsed = parseNumber(subIndex);
    if (subIndex.trimmed().isEmpty()) {
        addError(uiText("empty subindex", "子项为空"));
    } else if (!subParsed.first || subParsed.second > 0xff) {
        addError(uiText("subindex must be 0x00..0xff or decimal 0..255", "子项必须是 0x00..0xff 或十进制 0..255"));
    }

    const QString trimmedValue = value.trimmed();
    if (trimmedValue.isEmpty()) {
        addError(uiText("empty value", "写入值为空"));
    }

    static const QSet<QString> knownTypes = {"",       "bool",   "int8",   "int16", "int32",  "int64",  "uint8",
                                             "uint16", "uint32", "uint64", "float", "double", "string", "octet_string"};
    const QString normalizedType = type.trimmed().toLower().replace(' ', "_");
    if (!knownTypes.contains(normalizedType)) {
        addWarning(uiText("unknown type %1", "未知类型 %1").arg(type));
    }
    if (trimmedValue.isEmpty()) {
        return errors ? errors->isEmpty() : true;
    }

    if (normalizedType == "bool") {
        const QString normalizedValue = trimmedValue.toLower();
        if (!QStringList{"0", "1", "true", "false", "yes", "no"}.contains(normalizedValue)) {
            addError(uiText("bool value must be 0/1/true/false", "bool 值必须是 0/1/true/false"));
        }
    } else if (normalizedType.startsWith("uint")) {
        const auto parsed = parseNumber(trimmedValue);
        if (!parsed.first) {
            addError(uiText("unsigned integer value is not numeric", "无符号整数值不是有效数字"));
        } else {
            bool bitsOk = false;
            const int bits = normalizedType.mid(4).toInt(&bitsOk);
            if (bitsOk && bits > 0 && bits < 64) {
                const quint64 maxValue = (quint64{1} << bits) - 1;
                if (parsed.second > maxValue) {
                    addError(uiText("%1 value exceeds range 0..%2", "%1 值超出范围 0..%2")
                                 .arg(normalizedType)
                                 .arg(maxValue));
                }
            }
        }
    } else if (normalizedType.startsWith("int")) {
        QString numericText = trimmedValue;
        bool negative = false;
        if (numericText.startsWith('-')) {
            negative = true;
            numericText = numericText.mid(1);
        }
        const auto parsed = parseNumber(numericText);
        if (!parsed.first) {
            addError(uiText("signed integer value is not numeric", "有符号整数值不是有效数字"));
        } else {
            bool bitsOk = false;
            const int bits = normalizedType.mid(3).toInt(&bitsOk);
            if (bitsOk && bits > 1 && bits <= 64) {
                const qint64 minValue = bits == 64 ? std::numeric_limits<qint64>::min() : -(qint64{1} << (bits - 1));
                const quint64 positiveLimit =
                    bits == 64 ? quint64{9223372036854775807ULL} : (quint64{1} << (bits - 1)) - 1;
                if (negative) {
                    const quint64 maxMagnitude =
                        bits == 64 ? quint64{9223372036854775808ULL} : (quint64{1} << (bits - 1));
                    if (parsed.second > maxMagnitude) {
                        addError(uiText("%1 value is below range minimum %2", "%1 值低于范围下限 %2")
                                     .arg(normalizedType)
                                     .arg(minValue));
                    }
                } else if (parsed.second > positiveLimit) {
                    addError(uiText("%1 value exceeds range maximum %2", "%1 值超出范围上限 %2")
                                 .arg(normalizedType)
                                 .arg(positiveLimit));
                }
            }
            if (negative && numericText.startsWith("0x", Qt::CaseInsensitive)) {
                addWarning(
                    uiText("negative hexadecimal values depend on device parsing", "负十六进制值依赖设备解析方式"));
            }
        }
    } else if (normalizedType == "float" || normalizedType == "double") {
        bool ok = false;
        trimmedValue.toDouble(&ok);
        if (!ok) {
            addError(uiText("floating-point value is not numeric", "浮点值不是有效数字"));
        }
    } else if (normalizedType.isEmpty()) {
        addWarning(
            uiText("empty type, runtime will infer or use default behavior", "类型为空，运行时将推断或使用默认行为"));
    }

    return errors ? errors->isEmpty() : true;
}


// — Create the SDO history table columns if not yet initialized
void MainWindow::ensureSdoHistoryTable() {
    if (!sdoHistoryTable_) {
        return;
    }
    if (sdoHistoryTable_->columnCount() != 9) {
        sdoHistoryTable_->setColumnCount(9);
    }
    // Define column headers for the table
    sdoHistoryTable_->setHorizontalHeaderLabels({uiText("Time", "时间"), uiText("Action", "动作"),
                                                 uiText("Slave", "从站"), uiText("Index", "索引"),
                                                 uiText("Sub", "子项"), uiText("Type", "类型"), uiText("Value", "值"),
                                                 uiText("Status", "状态"), uiText("Detail", "详情")});
}


// Add a timestamped entry to the SDO history table with action, status, and detail
void MainWindow::appendSdoHistory(const QString& action, int position, const QString& index, const QString& subIndex,
                                  const QString& type, const QString& value, const QString& status,
                                  const QString& detail) {
    if (!sdoHistoryTable_) {
        return;
    }
    ensureSdoHistoryTable();
    const int row = sdoHistoryTable_->rowCount();
    sdoHistoryTable_->insertRow(row);
    const QStringList values = {
        QDateTime::currentDateTime().toString("HH:mm:ss"),
        action,
        position >= 0 ? QString::number(position) : QString(),
        index,
        subIndex,
        type,
        value,
        status,
        detail,
    };
    const QColor color = status == uiText("Failed", "失败")
                             // Define color for visual feedback
                             ? QColor("#ef4444")
                             : (status == uiText("Complete", "完成") || status == uiText("OK", "成功")
                                    // Define color for visual feedback
                                    ? QColor("#22c55e")
                                    // Define color for visual feedback
                                    : QColor("#f59e0b"));
    for (int column = 0; column < values.size(); ++column) {
        // Create table cell
        auto* item = new QTableWidgetItem(values.at(column));
        if (column == 7) {
            item->setForeground(color);
        }
        sdoHistoryTable_->setItem(row, column, item);
    }
    if (sdoHistoryTable_->rowCount() > 300) {
        sdoHistoryTable_->removeRow(0);
    }
    sdoHistoryTable_->resizeColumnsToContents(); // auto-fit column widths
    updateSdoHistoryRowDetail();
}


// — Refresh the SDO history detail strip for the focused row
void MainWindow::updateSdoHistoryRowDetail() {
    if (!sdoHistoryDetailLabel_) {
        return;
    }
    const SdoHistoryRowDetailTexts texts = sdoHistoryRowDetailTexts();
    // Lambda to push UI state changes to the label widget
    auto applyState = [this](const SdoHistoryRowDetailState& state) {
        sdoHistoryDetailLabel_->setText(state.text);
        // Set severity property for styling/theming
        sdoHistoryDetailLabel_->setProperty("severity", state.severityKey);
        sdoHistoryDetailLabel_->setToolTip(state.tooltip);
        repolish(sdoHistoryDetailLabel_); // force QSS re-evaluation after property change
    };

    if (!sdoHistoryTable_) {
        applyState(sdoHistoryRowDetailUnavailableState(texts));
        return;
    }

    const int row = sdoHistoryTable_->currentRow();
    if (row < 0 || row >= sdoHistoryTable_->rowCount() || sdoHistoryTable_->isRowHidden(row)) {
        applyState(sdoHistoryRowDetailNoSelectionState(texts));
        return;
    }

    applyState(buildSdoHistoryRowDetailState(sdoHistoryRowFromTable(sdoHistoryTable_, row), texts));
}


// — Send an SDO read request to ecatd for the given object address
void MainWindow::requestSdoRead(int position, const QString& index, const QString& subIndex, const QString& source,
                                const QString& type) {
    const QString trimmedIndex = index.trimmed();
    const QString trimmedSub = subIndex.trimmed();
    if (position < 0 || trimmedIndex.isEmpty() || trimmedSub.isEmpty()) {
        return;
    }
    const QString key = sdoEvidenceKey(position, trimmedIndex, trimmedSub);
    const QString readType =
        !type.trimmed().isEmpty()
            ? type.trimmed()
            : (sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString());
    pendingSdoReads_.insert(key, source);
    pendingSdoReadTypes_.insert(key, readType);
    appendSdoHistory(uiText("Read", "读取"), position, trimmedIndex, trimmedSub, readType, QString(),
                     uiText("Requested", "已请求"), source);
    client_.upload(position, trimmedIndex, trimmedSub);
}
