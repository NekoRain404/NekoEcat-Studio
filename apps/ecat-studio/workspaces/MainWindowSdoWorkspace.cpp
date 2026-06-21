// SDO inspector, target panel, evidence trail, and history.

#include "MainWindowIncludes.h"
QStringList MainWindow::sdoWriteImpactDetails(int position,
                                              const QString &index,
                                              const QString &subIndex,
                                              const QString &targetValue,
                                              const QString &type) const {
  QStringList details;
  // Normalize hex address for consistent comparison
  const QString normalizedIndex = normalizeHexText(index, 4);
  // Normalize hex address for consistent comparison
  const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
  const QString target = targetValue.trimmed();

  QString slaveName = uiText("unknown slave", "未知从站");
  QString slaveState = uiText("unknown", "未知");
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      slaveName =
          slave.name.trimmed().isEmpty() ? slaveName : slave.name.trimmed();
      slaveState =
          slave.state.trimmed().isEmpty() ? slaveState : slave.state.trimmed();
      break;
    }
  }
  details << uiText("Slave context: #%1 %2, state %3",
                    "从站上下文：#%1 %2，状态 %3")
                 .arg(position)
                 .arg(slaveName, slaveState);

  QString objectText;
  QString accessText;
  QString nameText;
  QString bitsText;
  if (sdo_->sdoTable && selectedPosition() == position &&
      loadedSdoPosition_ == position) {
    const SdoDictionaryRow dictionary = sdoDictionaryRowForTarget(
        sdo_->sdoTable, normalizedIndex, normalizedSubIndex);
    objectText = dictionary.object;
    accessText = dictionary.access;
    bitsText = dictionary.bits;
    nameText = dictionary.name;
  }

  const QString category =
      sdoObjectCategory(normalizedIndex, nameText, objectText, QString());
  details << uiText("Object class: %1", "对象类别：%1").arg(category);
  if (!nameText.isEmpty() || !accessText.isEmpty() || !bitsText.isEmpty()) {
    QStringList objectFacts;
    if (!nameText.isEmpty()) {
      objectFacts << nameText;
    }
    if (!accessText.isEmpty()) {
      objectFacts << uiText("access %1", "权限 %1").arg(accessText);
    }
    if (!bitsText.isEmpty()) {
      objectFacts << uiText("%1 bit", "%1 位").arg(bitsText);
    }
    details << uiText("Dictionary: %1", "对象字典：%1")
                   .arg(objectFacts.join(" | "));
  }

  const QStringList cachedEvidence =
      sdoEvidence_.value(sdoEvidenceKey(position, index, subIndex));
  const QVector<SdoEvidenceItem> localEvidence =
      sdoLocalEvidenceItemsFromTables(
          position, normalizedIndex, normalizedSubIndex,
          sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString(), cachedEvidence.value(0),
          sdoInspector_->sdoValue &&
              isCurrentSdoTarget(position, normalizedIndex, normalizedSubIndex),
          sdo_->sdoTable && loadedSdoPosition_ == position,
          {.dictionaryTable = sdo_->sdoTable,
           .watchTable = watch_->watchTable,
           .startupTable = startupSdoTable_,
           .bookmarkTable = bookmark_->objectBookmarkTable},
          {.read = uiText("Read", "读回"),
           .watchPrefix = uiText("Watch", "Watch"),
           .dictionary = uiText("OD", "OD"),
           .startupPrefix = uiText("Startup", "Startup"),
           .bookmarkPrefix = uiText("Bookmark", "书签")});

  const auto evidenceGroups = groupSdoEvidence(localEvidence);
  if (evidenceGroups.isEmpty()) {
    details << uiText("Evidence Set: no local comparable evidence",
                      "证据集：无可比较的本地证据");
  } else {
    QStringList groupFacts;
    for (const auto &group : evidenceGroups) {
      groupFacts << QString("%1=%2").arg(group.sources.join("/"), group.value);
    }
    details << (evidenceGroups.size() == 1
                    ? uiText("Evidence Set: consistent %1", "证据集：一致 %1")
                          .arg(groupFacts.join("; "))
                    : uiText("Evidence Set conflict: %1", "证据集冲突：%1")
                          .arg(groupFacts.join("; ")));

    if (!target.isEmpty()) {
      const SdoWriteDeltaReview review =
          reviewSdoWriteDelta(localEvidence, target);
      QStringList differingFacts;
      for (const auto &group : review.differingGroups) {
        differingFacts << QString("%1=%2").arg(group.sources.join("/"),
                                               group.value);
      }
      if (differingFacts.isEmpty()) {
        details << uiText("Write target: matches all local evidence",
                          "写入目标：匹配全部本地证据");
      } else if (!review.matchingSources.isEmpty()) {
        details << uiText("Write target: matches %1, differs from %2",
                          "写入目标：匹配 %1，不同于 %2")
                       .arg(review.matchingSources.join("/"),
                            differingFacts.join("; "));
      } else {
        details << uiText("Write target: differs from local evidence %1",
                          "写入目标：不同于本地证据 %1")
                       .arg(differingFacts.join("; "));
      }
    }
  }

  QString currentValue;
  QString currentSource;
  if (watch_->watchTable) {
    for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
      const QString value = tableText(watch_->watchTable, row, 4);
      if (tableObjectAddressMatches(watch_->watchTable, row, position, normalizedIndex,
                                    normalizedSubIndex, 1, 2, 3) &&
          !value.isEmpty()) {
        currentValue = value;
        currentSource = uiText("Watch", "Watch");
        break;
      }
    }
  }
  if (currentValue.isEmpty()) {
    const QStringList evidence =
        sdoEvidence_.value(sdoEvidenceKey(position, index, subIndex));
    if (!evidence.value(0).trimmed().isEmpty()) {
      currentValue = evidence.value(0).trimmed();
      currentSource = uiText("Object Dictionary evidence", "对象字典证据");
      const QString status = evidence.value(1).trimmed();
      const QString time = evidence.value(3).trimmed();
      if (!status.isEmpty()) {
        details << uiText("Last OD status: %1%2", "最后 OD 状态：%1%2")
                       .arg(status, time.isEmpty() ? QString() : "  " + time);
      }
    }
  }

  if (!currentValue.isEmpty()) {
    const bool same = normalizeComparableValue(currentValue) ==
                      normalizeComparableValue(target);
    details << (same ? uiText(
                           "Change preview: current %1 already matches target",
                           "变更预览：当前值 %1 已匹配目标值")
                           .arg(currentValue)
                     : uiText("Change preview: %1 -> %2", "变更预览：%1 -> %2")
                           .arg(currentValue, target));
    details << uiText("Current evidence source: %1", "当前证据来源：%1")
                   .arg(currentSource);
  } else {
    details << uiText("Current evidence: no Watch or OD value available",
                      "当前证据：没有可用的 Watch 或 OD 值");
  }

  if (startupSdoTable_) {
    int startupMatches = 0;
    QString startupExpected;
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
      if (!tableObjectAddressMatches(startupSdoTable_, row, position,
                                     normalizedIndex, normalizedSubIndex, 0, 1,
                                     2)) {
        continue;
      }
      ++startupMatches;
      startupExpected = tableText(startupSdoTable_, row, 3);
    }
    if (startupMatches > 0) {
      details << uiText("Startup expectation: %1 (%2 matching startup row(s))",
                        "Startup 期望值：%1（%2 条匹配启动行）")
                     .arg(startupExpected)
                     .arg(startupMatches);
    }
  }

  const QString riskKey = QString("%1 %2 %3 %4")
                              .arg(normalizedIndex, nameText, objectText, type)
                              .toLower();
  QStringList riskHints;
  if (riskKey.contains("0x6040") || riskKey.contains("0x6060") ||
      riskKey.contains("controlword") || riskKey.contains("mode")) {
    riskHints << uiText("drive command/mode object; confirm actuator safety",
                        "驱动命令/模式对象；确认执行机构安全");
  }
  if (riskKey.contains("0x160") || riskKey.contains("0x1a0") ||
      riskKey.contains("0x1c1") || riskKey.contains("0x1c2") ||
      riskKey.contains("pdo") || riskKey.contains("mapping")) {
    riskHints << uiText(
        "PDO mapping object; verify state and process-image impact",
        "PDO 映射对象；确认状态和过程映像影响");
  }
  if (riskKey.contains("0x1010") || riskKey.contains("0x1011") ||
      riskKey.contains("store") || riskKey.contains("save") ||
      riskKey.contains("restore")) {
    riskHints << uiText("persistent storage object; change may survive reboot",
                        "持久化存储对象；变更可能跨重启保留");
  }
  if (!riskHints.isEmpty()) {
    details
        << uiText("Risk flags: %1", "风险标记：%1").arg(riskHints.join("; "));
  }

  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before writing",
                      "拓扑基线：%1 个问题；写入前请复核")
                   .arg(topologyIssues.size());
  }
  return details;
}

// ── Startup SDO Impact ──────────────────────────────────────────────

// — Return the startup sdo impact line
QString MainWindow::startupSdoImpactLine(int row) const {
  if (!startupSdoTable_ || row < 0 || row >= startupSdoTable_->rowCount()) {
    return QString();
  }
  const QString position = tableText(startupSdoTable_, row, 0);
  const QString index = tableText(startupSdoTable_, row, 1);
  const QString subIndex = tableText(startupSdoTable_, row, 2);
  const QString target = tableText(startupSdoTable_, row, 3);
  const QString type = tableText(startupSdoTable_, row, 4);
  const QString watchValue = tableText(startupSdoTable_, row, 7);
  const QString watchDelta = tableText(startupSdoTable_, row, 8);

  bool positionOk = false;
  const int numericPosition = position.toInt(&positionOk);
  QString current = watchValue;
  QString source = uiText("Watch", "Watch");
  if (current.isEmpty() && positionOk) {
    const QStringList evidence =
        sdoEvidence_.value(sdoEvidenceKey(numericPosition, index, subIndex));
    current = evidence.value(0).trimmed();
    source = uiText("OD evidence", "OD 证据");
  }
  if (current.isEmpty()) {
    current = uiText("unknown current", "当前未知");
    source = uiText("no evidence", "无证据");
  }

  QString line =
      uiText("Row %1: #%2 %3:%4 %5 -> %6", "第 %1 行：#%2 %3:%4 %5 -> %6")
          .arg(row + 1)
          .arg(position, index, subIndex, current, target);
  QStringList suffix;
  suffix << source;
  if (!watchDelta.isEmpty()) {
    suffix << uiText("Watch delta %1", "Watch 偏差 %1").arg(watchDelta);
  }
  if (!type.isEmpty()) {
    suffix << uiText("type %1", "类型 %1").arg(type);
  }
  return QString("%1 (%2)").arg(line, suffix.join(" | "));
}


// — Return a list of startup sdo batch impact details
QStringList MainWindow::startupSdoBatchImpactDetails(const QVector<int> &rows,
                                                     int previewLimit) const {
  QStringList details;
  if (!startupSdoTable_ || rows.isEmpty()) {
    return details;
  }

  int watchMatch = 0;
  int watchDiff = 0;
  int pending = 0;
  int noWatch = 0;
  int driveRisk = 0;
  int pdoRisk = 0;
  int persistentRisk = 0;

  for (const int row : rows) {
    if (row < 0 || row >= startupSdoTable_->rowCount()) {
      continue;
    }
    const QString delta = tableText(startupSdoTable_, row, 8).toLower();
    if (delta == "match" || delta == "匹配") {
      ++watchMatch;
    } else if (delta == "diff" || delta == "不一致") {
      ++watchDiff;
    } else if (delta == "no watch" || delta == "无监视") {
      ++noWatch;
    } else {
      ++pending;
    }

    const QString riskKey =
        QString("%1 %2")
            // Normalize hex address for consistent comparison
            .arg(normalizeHexText(tableText(startupSdoTable_, row, 1), 4),
                 tableText(startupSdoTable_, row, 4))
            .toLower();
    if (riskKey.contains("0x6040") || riskKey.contains("0x6060")) {
      ++driveRisk;
    }
    if (riskKey.contains("0x160") || riskKey.contains("0x1a0") ||
        riskKey.contains("0x1c1") || riskKey.contains("0x1c2")) {
      ++pdoRisk;
    }
    if (riskKey.contains("0x1010") || riskKey.contains("0x1011")) {
      ++persistentRisk;
    }
  }

  details << uiText(
                 "Watch evidence: match %1, diff %2, pending %3, no watch %4",
                 "Watch 证据：匹配 %1，不一致 %2，待比较 %3，无监视 %4")
                 .arg(watchMatch)
                 .arg(watchDiff)
                 .arg(pending)
                 .arg(noWatch);

  QStringList riskFacts;
  if (driveRisk > 0) {
    riskFacts << uiText("drive/mode rows %1", "驱动/模式行 %1").arg(driveRisk);
  }
  if (pdoRisk > 0) {
    riskFacts << uiText("PDO mapping rows %1", "PDO 映射行 %1").arg(pdoRisk);
  }
  if (persistentRisk > 0) {
    riskFacts
        << uiText("persistent rows %1", "持久化行 %1").arg(persistentRisk);
  }
  if (!riskFacts.isEmpty()) {
    details
        << uiText("Risk summary: %1", "风险摘要：%1").arg(riskFacts.join("; "));
  }

  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before applying",
                      "拓扑基线：%1 个问题；应用前请复核")
                   .arg(topologyIssues.size());
  }

  const int limit =
      std::max(0, std::min(previewLimit, static_cast<int>(rows.size())));
  for (int i = 0; i < limit; ++i) {
    const QString line = startupSdoImpactLine(rows.at(i));
    if (!line.isEmpty()) {
      details << line;
    }
  }
  if (rows.size() > limit) {
    details << uiText("...and %1 more item(s)", "...另有 %1 项")
                   .arg(rows.size() - limit);
  }
  return details;
}
// ── SDO Write Mode ──────────────────────────────────────────────────


// — Restore manual sdo write mode
void MainWindow::restoreManualSdoWriteMode() {
  if (selectedSdoWritable_) {
    updateSdoInspector(uiText("Manual edit", "手动编辑"));
    return;
  }
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(uiText("Manual edit", "手动编辑"));
  updateActionAvailability();
// ── Current SDO Target ──────────────────────────────────────────────
}


// — Check whether is current sdo target
bool MainWindow::isCurrentSdoTarget(int position, const QString &index,
                                    const QString &subIndex) const {
  if (position < 0 || selectedPosition() < 0 || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return false;
  }
  const QString currentIndex = sdoInspector_->sdoIndex->text().trimmed();
  const QString currentSubIndex = sdoInspector_->sdoSubIndex->text().trimmed();
  if (currentIndex.isEmpty() || currentSubIndex.isEmpty()) {
    return false;
  }
  return sdoEvidenceKey(position, index, subIndex) ==
         sdoEvidenceKey(selectedPosition(), currentIndex, currentSubIndex);
}


// — Return the current sdo dictionary row
int MainWindow::currentSdoDictionaryRow() const {
  const int position = selectedPosition();
  if (position < 0 || !sdo_->sdoTable || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex ||
      loadedSdoPosition_ != position) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.dictionaryTable = sdo_->sdoTable},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text(),
                                        .dictionaryLoadedForPosition = true})
      .dictionaryRow;
}


// — Return the current sdo watch row
int MainWindow::currentSdoWatchRow() const {
  const int position = selectedPosition();
  if (position < 0 || !watch_->watchTable || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.watchTable = watch_->watchTable},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text()})
      .watchRow;
}


// — Return the current sdo startup row
int MainWindow::currentSdoStartupRow() const {
  const int position = selectedPosition();
  if (position < 0 || !startupSdoTable_ || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.startupTable = startupSdoTable_},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text()})
      .startupRow;
}


// — Return the current sdo bookmark row
int MainWindow::currentSdoBookmarkRow() const {
  const int position = selectedPosition();
  if (position < 0 || !bookmark_->objectBookmarkTable || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.bookmarkTable = bookmark_->objectBookmarkTable},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text()})
      .bookmarkRow;
}


// — Return the current sdo target trail row
int MainWindow::currentSdoTargetTrailRow() const {
  const int position = selectedPosition();
  if (position < 0 || !sdoTargetTrailTable_ || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget(
             {.targetTrailTable = sdoTargetTrailTable_},
             {.position = position,
              .index = sdoInspector_->sdoIndex->text(),
              .subIndex = sdoInspector_->sdoSubIndex->text()})
      .targetTrailRow;
}


// — Return the current sdo preferred evidence value
QString MainWindow::currentSdoPreferredEvidenceValue(QString *source) const {
  return preferredSdoEvidenceValue(currentSdoEvidenceCandidates(), source);
}

void MainWindow::handleSdoValueResponse(int position, const QString &index,
                                        const QString &subIndex,
                                        const QString &value) {
  log(QString("SDO upload #%1 %2:%3 = %4")
          .arg(position)
          .arg(index, subIndex, value));
  const QString key = sdoEvidenceKey(position, index, subIndex);
  const bool currentTarget = isCurrentSdoTarget(position, index, subIndex);
  if (currentTarget && sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(value);
    sdoInspector_->sdoValue->setPlaceholderText(uiText(
        "Read-back for current SDO target", "当前 SDO 目标的读回值"));
    updateActionAvailability();
  }
  const QString source = pendingSdoReads_.take(key);
  const QString readType = pendingSdoReadTypes_.take(key);
  updateSdoTableEvidence(position, index, subIndex, value,
                         uiText("Complete", "完成"),
                         source.isEmpty() ? uiText("Runtime response", "运行时返回")
                                          : source);
  const QStringList verification = pendingSdoVerifications_.take(key);
  const bool hasStartupCheck = pendingStartupSdoChecks_.contains(key);
  const QVector<int> startupCheckRows =
      hasStartupCheck ? pendingStartupSdoChecks_.take(key) : QVector<int>{};
  if (!verification.isEmpty()) {
    auto normalize = [](QString text) {
      return text.trimmed().remove(' ').toLower();
    };
    const QString expected = verification.value(4);
    const bool match = normalize(expected) == normalize(value);
    const QString verifyDetail =
        match ? uiText("Read-back matched expected value %1",
                       "读回值匹配期望值 %1")
                    .arg(expected)
              : uiText("Read-back mismatch, expected %1 got %2",
                       "读回不匹配，期望 %1，实际 %2")
                    .arg(expected, value);
    appendSdoHistory(uiText("Verify", "校验"), position, index, subIndex,
                     verification.value(3), value,
                     match ? uiText("OK", "成功") : uiText("Failed", "失败"),
                     verifyDetail);
    updateSdoTableEvidence(position, index, subIndex, value,
                           match ? uiText("OK", "成功")
                                 : uiText("Failed", "失败"),
                           verifyDetail);
    updateDiagnostics(
        match ? "Info" : "Error", "SDO",
        match ? QString("SDO write verified #%1 %2:%3 = %4")
                    .arg(position)
                    .arg(index, subIndex, value)
              : QString("SDO write verification failed #%1 "
                        "%2:%3 expected %4 got %5")
                    .arg(position)
                    .arg(index, subIndex, expected, value));
  }
  if (hasStartupCheck) {
    verifyStartupSdo(key, value, startupCheckRows);
  }
  appendSdoHistory(
      uiText("Read", "读取"), position, index, subIndex,
      !readType.isEmpty()
          ? readType
          : (currentTarget && sdoInspector_->sdoType
                 ? sdoInspector_->sdoType->currentText()
                 : QString()),
      value, uiText("Complete", "完成"),
      source.isEmpty() ? uiText("Runtime response", "运行时返回") : source);
  updateWatchTableFromSdo(position, index, subIndex, value, source, readType,
                          currentTarget, key);
  feedChartFromSdo(index, subIndex, value);
  watch_->watchTable->resizeColumnsToContents();
  updateWatchAutoRefresh();
  updateSelectedDriveSummary();
}

void MainWindow::updateWatchTableFromSdo(int position, const QString &index,
                                         const QString &subIndex,
                                         const QString &value,
                                         const QString &source,
                                         const QString &readType,
                                         bool currentTarget,
                                         const QString &key) {
  ensureWatchTable();
  int row = -1;
  for (int i = 0; i < watch_->watchTable->rowCount(); ++i) {
    const bool match =
        (watch_->watchTable->item(i, 1) &&
         watch_->watchTable->item(i, 1)->text().toInt() == position) &&
        (watch_->watchTable->item(i, 2) &&
         watch_->watchTable->item(i, 2)->text().compare(
             index, Qt::CaseInsensitive) == 0) &&
        (watch_->watchTable->item(i, 3) &&
         watch_->watchTable->item(i, 3)->text().compare(
             subIndex, Qt::CaseInsensitive) == 0);
    if (match) {
      row = i;
      break;
    }
  }
  if (row < 0) {
    row = watch_->watchTable->rowCount();
    watch_->watchTable->insertRow(row);
    watch_->watchTable->setItem(
        row, 1, new QTableWidgetItem(QString::number(position)));
    watch_->watchTable->setItem(row, 2, new QTableWidgetItem(index));
    watch_->watchTable->setItem(row, 3, new QTableWidgetItem(subIndex));
    watch_->watchTable->setItem(row, 8, new QTableWidgetItem);
    watch_->watchTable->setItem(row, 9, new QTableWidgetItem);
    watch_->watchTable->setItem(row, 10, new QTableWidgetItem);
    watch_->watchTable->setItem(row, 11, new QTableWidgetItem);
  }
  const bool changed =
      watchValues_.contains(key) && watchValues_.value(key) != value;
  watchValues_.insert(key, value);
  if (changed) {
    watchChangedKeys_.insert(key);
  }

  auto setCell = [this, row](int column, const QString &text) {
    auto *item = watch_->watchTable->item(row, column);
    if (!item) {
      item = new QTableWidgetItem;
      watch_->watchTable->setItem(row, column, item);
    }
    item->setText(text);
    return item;
  };

  setCell(0, QDateTime::currentDateTime().toString("HH:mm:ss"));
  auto *valueItem = setCell(4, value);
  const QString currentType = watch_->watchTable->item(row, 6)
                                  ? watch_->watchTable->item(row, 6)->text()
                                  : QString();
  const QString currentMode = watch_->watchTable->item(row, 7)
                                  ? watch_->watchTable->item(row, 7)->text()
                                  : QString();
  if (!watch_->watchTable->item(row, 6) ||
      watch_->watchTable->item(row, 6)->text().trimmed().isEmpty()) {
    const bool watchRefreshSource =
        source.contains("Watch", Qt::CaseInsensitive) ||
        source.contains("监视", Qt::CaseInsensitive);
    if (!watchRefreshSource && !readType.isEmpty()) {
      setCell(6, readType);
    } else if (!watchRefreshSource && currentTarget &&
               sdoInspector_->sdoType) {
      setCell(6, sdoInspector_->sdoType->currentText());
    }
  }
  const QString effectiveType = watch_->watchTable->item(row, 6)
                                    ? watch_->watchTable->item(row, 6)->text()
                                    : currentType;
  setCell(5, decodeWatchValue(index, subIndex, effectiveType, value,
                              currentMode));
  if (currentMode.trimmed().isEmpty()) {
    setCell(7, "Watch");
  }
  updateWatchBaselineDelta(row);
  updateWatchStartupDelta(row);
  if (changed) {
    valueItem->setBackground(settings_.theme == "Light" ? QColor("#fff7cc")
                                                        : QColor("#3a2f16"));
    valueItem->setForeground(settings_.theme == "Light" ? QColor("#854d0e")
                                                        : QColor("#fde68a"));
  } else {
    valueItem->setBackground(QBrush());
    valueItem->setForeground(QBrush());
  }
}

void MainWindow::feedChartFromSdo(const QString &index,
                                  const QString &subIndex,
                                  const QString &value) {
  const QString odPrefix = QString("od_%1_%2").arg(index, subIndex);
  for (auto *chart : openCharts_) {
    if (!chart) continue;
    if (chart->entryKey().startsWith(odPrefix)) {
      bool numOk = false;
      double v = value.toDouble(&numOk);
      if (!numOk) v = value.toULongLong(&numOk, 16);
      if (numOk) chart->feedValue(v);
    }
  }
}

void MainWindow::verifyStartupSdo(const QString &key, const QString &value,
                                  const QVector<int> &startupCheckRows) {
  auto normalize = [](QString text) {
    return text.trimmed().remove(' ').toLower();
  };
  for (const int startupCheckRow : startupCheckRows) {
    if (startupCheckRow < 0 ||
        startupCheckRow >= startupSdoTable_->rowCount()) {
      continue;
    }
    const QString expected =
        startupSdoTable_->item(startupCheckRow, 3)
            ? startupSdoTable_->item(startupCheckRow, 3)->text()
            : QString();
    const bool match = normalize(expected) == normalize(value);
    auto *status = startupSdoTable_->item(startupCheckRow, 5);
    if (!status) {
      status = new QTableWidgetItem;
      startupSdoTable_->setItem(startupCheckRow, 5, status);
    }
    auto *detail = startupSdoTable_->item(startupCheckRow, 6);
    if (!detail) {
      detail = new QTableWidgetItem;
      startupSdoTable_->setItem(startupCheckRow, 6, detail);
    }
    status->setText(match ? uiText("Verified", "已校验")
                          : uiText("Mismatch", "不匹配"));
    status->setForeground(match ? QColor("#22c55e") : QColor("#ef4444"));
    status->setBackground(
        match ? QBrush()
              : (settings_.theme == "Light" ? QBrush(QColor("#fef2f2"))
                                            : QBrush(QColor("#3a1218"))));
    detail->setText(
        match ? uiText("Read-back matched %1", "读回值匹配 %1").arg(expected)
              : uiText("Expected %1, got %2", "期望 %1，实际 %2")
                    .arg(expected, value));
    updateDiagnostics(
        match ? "Info" : "Error", "Startup SDO",
        match ? QString("Startup SDO verified row %1")
                    .arg(startupCheckRow + 1)
              : QString("Startup SDO mismatch row %1 expected %2 got %3")
                    .arg(startupCheckRow + 1)
                    .arg(expected, value));
  }
  startupSdoTable_->resizeColumnsToContents();
  updateWatchStartupDeltas();
}
