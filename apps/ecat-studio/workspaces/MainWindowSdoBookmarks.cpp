// Object Bookmarks: CRUD operations, cross-workspace export to Watch/Startup.
// SDO inspector, target panel, evidence trail, and history.

#include "MainWindowIncludes.h"
void MainWindow::ensureObjectBookmarkTable() {
    // Ensure the object bookmark table exists (lazy initialization).
    if (!bookmark_->objectBookmarkTable) {
        return;
    }
    if (bookmark_->objectBookmarkTable->columnCount() != 10) {
        bookmark_->objectBookmarkTable->setColumnCount(10);
    }
    // Define column headers for the table
    bookmark_->objectBookmarkTable->setHorizontalHeaderLabels(
        {uiText("Slave", "从站"), uiText("Slave Name", "从站名称"), uiText("Index", "索引"), uiText("Sub", "子项"),
         uiText("Access", "权限"), uiText("Type", "类型"), uiText("Bits", "位宽"), uiText("Name", "名称"),
         uiText("Last Value", "最后值"), uiText("Source", "来源")});
    bookmark_->objectBookmarkTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    bookmark_->objectBookmarkTable->horizontalHeader()->setStretchLastSection(true);
}


// — Refresh the object bookmark detail strip for the focused row
void MainWindow::updateObjectBookmarkRowDetail() {
    if (!objectBookmarkDetailLabel_) {
        return;
    }
    const ObjectBookmarkDetailTexts texts = objectBookmarkDetailTexts();
    // Lambda to push UI state changes to the label widget
    auto applyState = [this](const ObjectBookmarkDetailState& state) {
        // Update the object bookmark row detail panel.
        objectBookmarkDetailLabel_->setText(state.text);
        // Set severity property for styling/theming
        objectBookmarkDetailLabel_->setProperty("severity", state.severityKey);
        objectBookmarkDetailLabel_->setToolTip(state.tooltip);
        repolish(objectBookmarkDetailLabel_); // force QSS re-evaluation after property change
    };

    if (!bookmark_->objectBookmarkTable) {
        applyState(objectBookmarkDetailUnavailableState(texts));
        return;
    }

    const int row = bookmark_->objectBookmarkTable->currentRow();
    if (row < 0 || row >= bookmark_->objectBookmarkTable->rowCount() ||
        bookmark_->objectBookmarkTable->isRowHidden(row)) {
        applyState(objectBookmarkDetailNoSelectionState(texts));
        return;
    }

    applyState(
        buildObjectBookmarkDetailState(sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, row), texts));
}


// — Return selected object bookmark rows
QVector<int> MainWindow::selectedObjectBookmarkRows() const {
    return selectedTableRows(bookmark_->objectBookmarkTable);
}


// — Check whether select object bookmark slave
bool MainWindow::selectObjectBookmarkSlave(int position) {
    if (position < 0) {
        return false;
    }
    if (!topologyTree_) {
        return false;
    }
    for (int top = 0; top < topologyTree_->topLevelItemCount(); ++top) {
        auto* masterItem = topologyTree_->topLevelItem(top);
        if (!masterItem) {
            continue;
        }
        for (int child = 0; child < masterItem->childCount(); ++child) {
            auto* slaveItem = masterItem->child(child);
            if (slaveItem && slaveItem->data(0, Qt::UserRole).toInt() == position) {
                QSignalBlocker blocker(topologyTree_); // prevent recursive signal updates
                topologyTree_->setCurrentItem(slaveItem);
                selectedLabel_->setText(slaveItem->text(0));
                filterWatchTable();
                updateSelectedSlavePanel();
                // Select the slave in the topology tree for a bookmark position.
                updateActionAvailability();
                updateCommissioningWorkflow();
                updateIoVariableTable();
                updateStateMachineView();
                return true;
            }
        }
    }
    return false;
}


// Bookmark the currently selected SDO object for quick access in the Object Bookmarks panel
void MainWindow::addCurrentSdoBookmark() {
    const int position = selectedPosition();
    const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
    const QString subIndex = sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
    if (position < 0 || index.isEmpty() || subIndex.isEmpty()) {
        updateDiagnostics("Info", "Object Bookmarks",
                          uiText("Select a slave and SDO target before bookmarking", "收藏前请先选择从站和 SDO 目标"));
        return;
    }
    const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
    const QString lastValue = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
    addObjectBookmark(position, index, subIndex, selectedSdoWritable_ ? uiText("rw", "读写") : uiText("ro", "只读"),
                      type, QString(), uiText("Manual SDO target", "手动 SDO 目标"), lastValue,
                      uiText("Current SDO", "当前 SDO"));
}


// — Add selected dictionary rows to bookmarks
void MainWindow::addSelectedDictionaryRowsToBookmarks() {
    if (selectedPosition() < 0 || !sdo_->sdoTable || loadedSdoPosition_ != selectedPosition()) {
        return;
    }
    // Add the current SDO target as a bookmark.
    const QVector<int> rows = selectedDictionaryRows();
    if (rows.isEmpty()) {
        return;
    }
    addDictionaryRowsToBookmarks(rows, uiText("selected Object Dictionary row(s)", "选中对象字典行"));
}


// — Add dictionary rows to bookmarks
void MainWindow::addDictionaryRowsToBookmarks(const QVector<int>& rows, const QString& sourceLabel) {
    if (selectedPosition() < 0 || !sdo_->sdoTable || loadedSdoPosition_ != selectedPosition() || rows.isEmpty()) {
        return;
    }
    int addedOrUpdated = 0;
    int skipped = 0;
    for (const int row : rows) {
        if (row < 0 || row >= sdo_->sdoTable->rowCount() || sdo_->sdoTable->isRowHidden(row)) {
            ++skipped;
            continue;
        }
        const SdoDictionaryRow dictionary = sdoDictionaryRowFromTable(sdo_->sdoTable, row);
        if (!sdoDictionaryRowHasTarget(dictionary)) {
            ++skipped;
            continue;
        }
        // Add selected dictionary rows to bookmarks.
        addObjectBookmark(selectedPosition(), dictionary.index, dictionary.subIndex, dictionary.access, dictionary.type,
                          dictionary.bits, dictionary.name, dictionary.value, uiText("Object Dictionary", "对象字典"));
        ++addedOrUpdated;
    }
    updateDiagnostics("Info", "Object Bookmarks",
                      uiText("Bookmarked/reused %1 %2%3", "已收藏/复用 %1 条%2%3")
                          .arg(addedOrUpdated)
                          .arg(sourceLabel)
                          .arg(skipped > 0 ? uiText(", skipped %1", "，跳过 %1").arg(skipped) : QString()));
    updateActionAvailability();
}


// — Check whether select slave for local evidence
bool MainWindow::selectSlaveForLocalEvidence(int position) {
    if (position < 0 || !topologyTree_) {
        return false;
    }
    for (int top = 0; top < topologyTree_->topLevelItemCount(); ++top) {
        auto* masterItem = topologyTree_->topLevelItem(top);
        if (!masterItem) {
            continue;
        }
        for (int child = 0; child < masterItem->childCount(); ++child) {
            auto* slaveItem = masterItem->child(child);
            if (!slaveItem || slaveItem->data(0, Qt::UserRole).toInt() != position) {
                // Add dictionary rows to bookmarks with source label.
                continue;
            }
            QSignalBlocker blocker(topologyTree_); // prevent recursive signal updates
            topologyTree_->setCurrentItem(slaveItem);
            selectedLabel_->setText(slaveItem->text(0));
            filterWatchTable();
            updateSelectedSlavePanel();
            updateActionAvailability();
            updateCommissioningWorkflow();
            updateIoVariableTable();
            updateStateMachineView();
            updateDiagnostics("Info", "Navigator",
                              uiText("Selected slave #%1 from local evidence without online load.",
                                     "已从本地证据选择从站 #%1，未触发在线加载。")
                                  .arg(position));
            return true;
        }
    }
    return false;
}


// Insert a new object bookmark entry with the given slave position, index, and sub-index
void MainWindow::addObjectBookmark(int position, const QString& index, const QString& subIndex, const QString& access,
                                   const QString& type, const QString& bits, const QString& name,
                                   const QString& lastValue, const QString& source) {
    if (position < 0 || index.trimmed().isEmpty() || subIndex.trimmed().isEmpty()) {
        return;
    }
    ensureObjectBookmarkTable();
    // Normalize hex address for consistent comparison
    const QString normalizedIndex = normalizeHexText(index, 4);
    // Normalize hex address for consistent comparison
    const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
    int row = tableRowForObjectAddress(bookmark_->objectBookmarkTable, position, normalizedIndex, normalizedSubIndex, 0,
                                       2, 3);
    if (row < 0) {
        row = bookmark_->objectBookmarkTable->rowCount();
        bookmark_->objectBookmarkTable->insertRow(row);
    }

    QString slaveName;
    for (const auto& slave : slaves_) {
        if (slave.position == position) {
            slaveName = slave.name;
            break;
        }
    }
    const QStringList values = {
        QString::number(position),
        slaveName,
        normalizedIndex,
        normalizedSubIndex,
        access,
        // Select a slave for local evidence display.
        type,
        bits,
        name,
        lastValue,
        source.trimmed().isEmpty() ? uiText("Project", "工程") : source,
    };
    for (int column = 0; column < values.size(); ++column) {
        auto* item = bookmark_->objectBookmarkTable->item(row, column);
        if (!item) {
            // Create table cell
            item = new QTableWidgetItem;
            bookmark_->objectBookmarkTable->setItem(row, column, item);
        }
        item->setText(values.at(column));
    }
    bookmark_->objectBookmarkTable->resizeColumnsToContents(); // auto-fit column widths
    bookmark_->objectBookmarkTable->selectRow(row);
    updateObjectBookmarkRowDetail();
}


// — Fill the SDO target panel from the selected object bookmark row
void MainWindow::applySdoSelectionFromBookmark(int row, bool readAfterFill) {
    if (!bookmark_->objectBookmarkTable || row < 0 || row >= bookmark_->objectBookmarkTable->rowCount()) {
        return;
    }
    const SdoObjectBookmarkRow bookmark = sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, row);
    if (!sdoObjectBookmarkRowHasTarget(bookmark)) {
        return;
    }

    const bool slaveSelected =
        readAfterFill ? selectObjectBookmarkSlave(bookmark.position) : selectSlaveForLocalEvidence(bookmark.position);
    if (!slaveSelected) {
        updateDiagnostics("Warning", "Object Bookmarks",
                          uiText("Bookmark slave #%1 is not in the current topology; rescan or "
                                 // Add an object bookmark entry with full metadata.
                                 "open the matching project before filling SDO fields",
                                 "书签从站 #%1 不在当前拓扑中；请先重新扫描或打开匹配工程，再回填 "
                                 "SDO 字段")
                              .arg(bookmark.position));
        return;
    }
    {
        const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex);       // prevent recursive signal updates
        const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
        const QSignalBlocker typeBlocker(sdoInspector_->sdoType);         // prevent recursive signal updates
        const QSignalBlocker valueBlocker(sdoInspector_->sdoValue);       // prevent recursive signal updates
        sdoInspector_->sdoIndex->setText(bookmark.index);
        sdoInspector_->sdoSubIndex->setText(bookmark.subIndex);
        if (sdoInspector_->sdoType) {
            const QString normalized = bookmark.type.toLower().replace(' ', "_");
            const int typeIndex = sdoInspector_->sdoType->findText(normalized, Qt::MatchFixedString);
            sdoInspector_->sdoType->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
        }
        if (sdoInspector_->sdoValue) {
            sdoInspector_->sdoValue->setText(bookmark.lastValue);
            sdoInspector_->sdoValue->setPlaceholderText(bookmark.lastValue.isEmpty()
                                                            ? uiText("No bookmark value", "书签暂无值")
                                                            : uiText("Value from object bookmark", "来自对象书签的值"));
        }
    }

    selectedSdoWritable_ = !sdoObjectAccessIsReadOnly(bookmark.access, uiText("只读", "只读"));
    if (sdoInspector_->sdoWriteValue) {
        sdoInspector_->sdoWriteValue->setEnabled(selectedSdoWritable_);
        sdoInspector_->sdoWriteValue->setPlaceholderText(selectedSdoWritable_ ? uiText("Value to write", "写入值")
                                                                              : uiText("Read-only object", "只读对象"));
        if (!selectedSdoWritable_) {
            sdoInspector_->sdoWriteValue->clear();
        }
    }
    updateSdoInspector(uiText("Object Bookmark", "对象书签"),
                       QString("%1 %2 bit %3").arg(bookmark.name, bookmark.bits, bookmark.type));
    rememberCurrentSdoTarget(uiText("Object Bookmark", "对象书签"),
                             QString("%1 %2 bit %3").arg(bookmark.name, bookmark.bits, bookmark.type));
    updateDiagnostics("Info", "Object Bookmarks",
                      uiText("Filled SDO target from bookmark #%1 %2:%3", "已从书签回填 SDO 目标 #%1 %2:%3")
                          .arg(bookmark.position)
                          .arg(sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text()));
    if (readAfterFill && client_.isConnected()) {
        requestSdoRead(bookmark.position, sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text(),
                       uiText("Object Bookmark", "对象书签"),
                       sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString());
    }
    updateActionAvailability();
}


// — Add selected object bookmarks to watch
void MainWindow::addSelectedObjectBookmarksToWatch() {
    const QVector<int> rows = selectedObjectBookmarkRows();
    if (rows.isEmpty()) {
        return;
    }
    addObjectBookmarkRowsToWatch(rows);
}


// — Add object bookmark rows to watch
void MainWindow::addObjectBookmarkRowsToWatch(const QVector<int>& rows) {
    if (!bookmark_->objectBookmarkTable || rows.isEmpty()) {
        return;
    }
    ensureWatchTable();
    int added = 0;
    int reused = 0;
    int skipped = 0;
    for (const int bookmarkRow : rows) {
        const SdoObjectBookmarkRow bookmark =
            sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, bookmarkRow);
        if (!sdoObjectBookmarkRowHasTarget(bookmark)) {
            ++skipped;
            continue;
        }
        int watchRow = -1;
        for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
            const bool match = tableObjectAddressMatches(watch_->watchTable, row, bookmark.position, bookmark.index,
                                                         bookmark.subIndex, 1, 2, 3);
            if (match) {
                watchRow = row;
                break;
            }
        }
        if (watchRow < 0) {
            watchRow = watch_->watchTable->rowCount();
            watch_->watchTable->insertRow(watchRow);
            watch_->watchTable->setItem(watchRow, 0,
                                        // Create table cell
                                        new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss")));
            watch_->watchTable->setItem(watchRow, 1,
                                        // Create table cell
                                        new QTableWidgetItem(QString::number(bookmark.position)));
            // Create table cell
            watch_->watchTable->setItem(watchRow, 2, new QTableWidgetItem(bookmark.index));
            watch_->watchTable->setItem(watchRow, 3,
                                        // Create table cell
                                        new QTableWidgetItem(bookmark.subIndex));
            ++added;
        } else {
            ++reused;
        }
        // Create table cell
        watch_->watchTable->setItem(watchRow, 4, new QTableWidgetItem(bookmark.lastValue));
        watch_->watchTable->setItem(
            watchRow, 5,
            // Create table cell
            new QTableWidgetItem(decodeWatchValue(bookmark.index, bookmark.subIndex, bookmark.type, bookmark.lastValue,
                                                  "Object Bookmark")));
        // Create table cell
        watch_->watchTable->setItem(watchRow, 6, new QTableWidgetItem(bookmark.type));
        watch_->watchTable->setItem(watchRow, 7,
                                    // Create table cell
                                    new QTableWidgetItem(bookmark.name.isEmpty()
                                                             // Add selected bookmarks to Watch list.
                                                             ? "Object Bookmark"
                                                             : bookmark.name));
        for (int column = 8; column < 12; ++column) {
            if (!watch_->watchTable->item(watchRow, column)) {
                // Create table cell
                watch_->watchTable->setItem(watchRow, column, new QTableWidgetItem);
            }
        }
        updateWatchBaselineDelta(watchRow);
        updateWatchStartupDelta(watchRow);
    }
    watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
    filterWatchTable();
    updateWatchAutoRefresh();
    activateWorkspaceTab(watchTabIndex_);
    updateDiagnostics(
        "Info", "Object Bookmarks",
        uiText("Watch from bookmarks: added %1, reused %2, skipped %3", "从书签加入 Watch：新增 %1，复用 %2，跳过 %3")
            .arg(added)
            .arg(reused)
            // Add bookmark rows to Watch list.
            .arg(skipped));
}


// — Add selected object bookmarks to startup sdo
void MainWindow::addSelectedObjectBookmarksToStartupSdo() {
    const QVector<int> rows = selectedObjectBookmarkRows();
    if (rows.isEmpty()) {
        return;
    }
    addObjectBookmarkRowsToStartupSdo(rows);
}


// — Add object bookmark rows to startup sdo
void MainWindow::addObjectBookmarkRowsToStartupSdo(const QVector<int>& rows) {
    if (!bookmark_->objectBookmarkTable || rows.isEmpty()) {
        return;
    }
    ensureStartupSdoTable();
    if (!startupSdoTable_) {
        return;
    }

    auto ensureStartupCell = [this](int row, int column) {
        auto* item = startupSdoTable_->item(row, column);
        if (!item) {
            // Create table cell
            item = new QTableWidgetItem;
            startupSdoTable_->setItem(row, column, item);
        }
        return item;
    };

    struct Candidate {
        int bookmarkRow = -1;
        int position = -1;
        QString index;
        QString subIndex;
        QString value;
        QString type;
        QString name;
        QString source;
        QVector<int> startupRows;
    };

    QVector<int> uniqueRows = rows;
    std::sort(uniqueRows.begin(), uniqueRows.end());
    uniqueRows.erase(std::unique(uniqueRows.begin(), uniqueRows.end()), uniqueRows.end());

    QVector<Candidate> candidates;
    QSet<QString> processedKeys;
    int skipped = 0;
    int duplicateSkipped = 0;

    for (const int bookmarkRow : uniqueRows) {
        const SdoObjectBookmarkRow bookmark =
            sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, bookmarkRow);
        if (!sdoObjectBookmarkRowHasTarget(bookmark) || bookmark_->objectBookmarkTable->isRowHidden(bookmarkRow) ||
            // Add selected bookmarks to Startup SDO list.
            bookmark.lastValue.isEmpty()) {
            ++skipped;
            continue;
        }

        const QString key = QString("%1|%2|%3").arg(bookmark.position).arg(bookmark.index, bookmark.subIndex);
        if (processedKeys.contains(key)) {
            ++duplicateSkipped;
            continue;
        }
        processedKeys.insert(key);

        Candidate candidate;
        candidate.bookmarkRow = bookmarkRow;
        candidate.position = bookmark.position;
        candidate.index = bookmark.index;
        candidate.subIndex = bookmark.subIndex;
        candidate.value = bookmark.lastValue;
        // Add bookmark rows to Startup SDO list.
        candidate.type = bookmark.type;
        candidate.name = bookmark.name;
        candidate.source = bookmark.source;

        for (int startupRow = 0; startupRow < startupSdoTable_->rowCount(); ++startupRow) {
            if (tableObjectAddressMatches(startupSdoTable_, startupRow, bookmark.position, bookmark.index,
                                          bookmark.subIndex, 0, 1, 2)) {
                candidate.startupRows.append(startupRow);
            }
        }

        candidates.append(candidate);
    }

    if (candidates.isEmpty()) {
        updateDiagnostics(
            "Warning", "Object Bookmarks",
            uiText("Startup creation skipped: selected bookmarks have no saved "
                   "Last Value%1",
                   "创建启动项已跳过：所选书签没有保存的最后值%1")
                .arg(skipped > 0 ? uiText(", skipped %1 row(s)", "，跳过 %1 行").arg(skipped) : QString()));
        return;
    }

    int existingRowsAffected = 0;
    int newRowsPlanned = 0;
    for (const auto& candidate : candidates) {
        if (candidate.startupRows.isEmpty()) {
            ++newRowsPlanned;
        } else {
            existingRowsAffected += candidate.startupRows.size();
        }
    }

    QStringList details = {
        uiText("Master: %1", "主站：%1").arg(activeMasterName()),
        uiText("Accepted bookmark values: %1", "可使用书签值：%1").arg(candidates.size()),
        uiText("Existing Startup SDO rows to update: %1", "将更新的已有 Startup SDO 行：%1").arg(existingRowsAffected),
        uiText("Missing Startup SDO rows to create: %1", "将创建的缺失 Startup SDO 行：%1").arg(newRowsPlanned),
        uiText("This changes only the Startup SDO table and does not read or write "
               "the bus.",
               "此操作只修改 Startup SDO 表，不会读取或写入总线。"),
    };
    const int previewRows = std::min(static_cast<int>(candidates.size()), 6);
    for (int i = 0; i < previewRows; ++i) {
        const auto& candidate = candidates.at(i);
        QString target = candidate.startupRows.isEmpty()
                             ? uiText("create", "创建")
                             : uiText("update startup row(s) %1", "更新启动行 %1").arg([&candidate] {
                                   QStringList rowNumbers;
                                   for (const int startupRow : candidate.startupRows) {
                                       rowNumbers << QString::number(startupRow + 1);
                                   }
                                   return rowNumbers.join(", ");
                               }());
        details << QString("#%1  %2:%3 = %4  [%5]%6")
                       .arg(candidate.position)
                       .arg(candidate.index, candidate.subIndex, candidate.value, target,
                            candidate.name.isEmpty() ? QString() : QString("  %1").arg(candidate.name));
    }
    if (candidates.size() > previewRows) {
        details
            << uiText("...and %1 more bookmark value(s)", "...另有 %1 个书签值").arg(candidates.size() - previewRows);
    }
    // Remove selected bookmarks.
    if (skipped > 0) {
        details << uiText("Skipped bookmarks without address or saved value: %1", "已跳过缺少地址或保存值的书签：%1")
                       .arg(skipped);
    }
    if (duplicateSkipped > 0) {
        details << uiText("Skipped duplicate selected bookmark address(es): %1", "已跳过重复选中书签地址：%1")
                       .arg(duplicateSkipped);
    }

    // Safety gate: require explicit confirmation before bus write
    if (!confirmDangerousOperation(uiText("Confirm Startup from Bookmarks", "确认从书签创建 Startup SDO"),
                                   uiText("Create or update Startup SDO rows from selected object "
                                          "bookmarks.",
                                          "从所选对象书签创建或更新 Startup SDO 行。"),
                                   details, uiText("Create Startup", "创建启动项"))) {
        return;
    }

    const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    int updated = 0;
    int unchanged = 0;
    int created = 0;
    int lastTouchedRow = -1;

    for (const auto& candidate : candidates) {
        if (candidate.startupRows.isEmpty()) {
            const int startupRow = startupSdoTable_->rowCount();
            startupSdoTable_->insertRow(startupRow);
            startupSdoTable_->setItem(startupRow, 0,
                                      // Create table cell
                                      new QTableWidgetItem(QString::number(candidate.position)));
            startupSdoTable_->setItem(startupRow, 1,
                                      // Create table cell
                                      new QTableWidgetItem(candidate.index));
            startupSdoTable_->setItem(startupRow, 2,
                                      // Create table cell
                                      new QTableWidgetItem(candidate.subIndex));
            startupSdoTable_->setItem(startupRow, 3,
                                      // Create table cell
                                      new QTableWidgetItem(candidate.value));
            startupSdoTable_->setItem(startupRow, 4,
                                      // Create table cell
                                      new QTableWidgetItem(candidate.type));
            startupSdoTable_->setItem(startupRow, 5,
                                      // Create table cell
                                      new QTableWidgetItem(uiText("From Bookmark", "来自书签")));
            startupSdoTable_->setItem(
                startupRow, 6,
                // Create table cell
                new QTableWidgetItem(
                    uiText("Created from Object Bookmark row %1 at %2%3", "由对象书签第 %1 行在 %2 创建%3")
                        .arg(candidate.bookmarkRow + 1)
                        .arg(timestamp,
                             candidate.source.isEmpty() ? QString() : QString(" (%1)").arg(candidate.source))));
            for (int column = 7; column < startupSdoTable_->columnCount(); ++column) {
                // Create table cell
                startupSdoTable_->setItem(startupRow, column, new QTableWidgetItem);
            }
            ++created;
            lastTouchedRow = startupRow;
            continue;
        }

        for (const int startupRow : candidate.startupRows) {
            if (startupRow < 0 || startupRow >= startupSdoTable_->rowCount()) {
                continue;
            }
            const QString previousValue = tableText(startupSdoTable_, startupRow, 3);
            ensureStartupCell(startupRow, 3)->setText(candidate.value);
            if (!candidate.type.isEmpty()) {
                ensureStartupCell(startupRow, 4)->setText(candidate.type);
            }
            ensureStartupCell(startupRow, 5)->setText(uiText("From Bookmark", "来自书签"));
            ensureStartupCell(startupRow, 6)
                ->setText(previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0
                              ? uiText("Confirmed from Object Bookmark row %1 at %2", "由对象书签第 %1 行在 %2 确认")
                                    .arg(candidate.bookmarkRow + 1)
                                    .arg(timestamp)
                              : uiText("Updated from Object Bookmark row %1 at %2; "
                                       "previous value: %3",
                                       "由对象书签第 %1 行在 %2 更新；原值：%3")
                                    .arg(candidate.bookmarkRow + 1)
                                    .arg(timestamp, previousValue));
            if (previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0) {
                ++unchanged;
            } else {
                ++updated;
            }
            lastTouchedRow = startupRow;
        }
    }

    if (lastTouchedRow >= 0) {
        startupSdoTable_->selectRow(lastTouchedRow);
    }
    startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
    updateWatchStartupDeltas();
    filterStartupSdoTable();
    updateStartupSdoControls();
    updateActionAvailability();
    updateDiagnostics("Info", "Startup SDO",
                      uiText("Startup from bookmarks: %1 updated, %2 unchanged, %3 created%4%5",
                             "从书签生成 Startup：更新 %1，未变 %2，新建 %3%4%5")
                          .arg(updated)
                          .arg(unchanged)
                          .arg(created)
                          .arg(skipped > 0 ? uiText(", skipped %1", "，跳过 %1").arg(skipped) : QString())
                          .arg(duplicateSkipped > 0
                                   ? uiText(", duplicate selections %1", "，重复选择 %1").arg(duplicateSkipped)
                                   : QString()));
    if (created > 0 || updated > 0 || unchanged > 0) {
        activateWorkspaceTab(startupSdoTabIndex_);
    }
}


// — Remove selected object bookmarks
void MainWindow::removeSelectedObjectBookmarks() {
    QVector<int> rows = selectedObjectBookmarkRows();
    if (rows.isEmpty() || !bookmark_->objectBookmarkTable) {
        return;
    }
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (const int row : rows) {
        if (row >= 0 && row < bookmark_->objectBookmarkTable->rowCount()) {
            bookmark_->objectBookmarkTable->removeRow(row);
        }
    }
    updateDiagnostics("Info", "Object Bookmarks",
                      uiText("Removed %1 object bookmark(s)", "已移除 %1 个对象书签").arg(rows.size()));
    updateObjectBookmarkRowDetail();
    updateActionAvailability();
}


// — Read selected dictionary rows