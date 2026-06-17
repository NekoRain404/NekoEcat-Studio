// Project file I/O, ESI repository, and master profiles.

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


// — Reset all project state to a blank untitled project
void MainWindow::newProject() {
  projectPath_.clear();
  projectName_ = "Untitled";
  ioVariableMetadata_.clear();
  if (bookmark_->objectBookmarkTable) {
    bookmark_->objectBookmarkTable->clearContents();
    bookmark_->objectBookmarkTable->setRowCount(0);
    ensureObjectBookmarkTable();
    updateObjectBookmarkRowDetail();
  }
  if (sdoTargetTrailTable_) {
    sdoTargetTrailTable_->clearContents();
    sdoTargetTrailTable_->setRowCount(0);
    rememberedSdoTargetTrailKeys_.clear();
    ensureSdoTargetTrailTable();
    updateSdoTargetTrailRowDetail();
  }
  clearOnlineViews();
  setWindowTitle(
      uiText("NekoEcat Studio - Untitled", "NekoEcat Studio - 未命名"));
  updateDiagnostics("Info", "Project", "New project created");
  updateStatusBar();
}


// — Prompt the user for a .ecatproj file and load it
void MainWindow::openProject() {
  const QString path = QFileDialog::getOpenFileName(
      this, uiText("Open Project", "打开工程"), QDir::homePath(),
      "EtherCAT Project (*.ecatproj);;JSON (*.json)");
  if (path.isEmpty()) {
    return;
  }
  if (!readProjectFile(path)) {
    QMessageBox::warning(this, uiText("Open Project", "打开工程"),
                         uiText("Failed to open project.", "工程打开失败。"));
  }
}


// — Save the current project to its existing file path, or prompt for a new one
void MainWindow::saveProject() {
  if (projectPath_.isEmpty()) {
    saveProjectAs();
    return;
  }
  if (!writeProjectFile(projectPath_)) {
    QMessageBox::warning(this, uiText("Save Project", "保存工程"),
                         uiText("Failed to save project.", "工程保存失败。"));
  }
}


// — Prompt for a new file path and save the current project there
void MainWindow::saveProjectAs() {
  const QString path = QFileDialog::getSaveFileName(
      this, uiText("Save Project As", "工程另存为"),
      QDir::home().absoluteFilePath(projectName_ + ".ecatproj"),
      "EtherCAT Project (*.ecatproj);;JSON (*.json)");
  if (path.isEmpty()) {
    return;
  }
  if (!writeProjectFile(path)) {
    QMessageBox::warning(this, uiText("Save Project As", "工程另存为"),
                         uiText("Failed to save project.", "工程保存失败。"));
  }
}


// — Serialize all workspace tables and settings to a .ecatproj JSON file
bool MainWindow::writeProjectFile(const QString &path) {
  QJsonArray slaveArray;
  for (const auto &slave : slaves_) {
    slaveArray.append(QJsonObject{
        {"position", slave.position},
        {"state", slave.state},
        {"flags", slave.flags},
        {"name", slave.name},
        {"rawLine", slave.rawLine},
    });
  }
  QJsonArray baselineArray;
  for (const auto &slave : topologyBaseline_) {
    baselineArray.append(QJsonObject{
        {"position", slave.position},
        {"state", slave.state},
        {"flags", slave.flags},
        {"name", slave.name},
        {"rawLine", slave.rawLine},
    });
  }
  QJsonArray watchArray;
  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    QJsonArray rowArray;
    for (int column = 0; column < watch_->watchTable->columnCount(); ++column) {
      rowArray.append(watch_->watchTable->item(row, column)
                          ? watch_->watchTable->item(row, column)->text()
                          : QString());
    }
    watchArray.append(rowArray);
  }
  QJsonArray startupArray;
  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    startupArray.append(QJsonObject{
        {"position", startupSdoTable_->item(row, 0)
                         ? startupSdoTable_->item(row, 0)->text().toInt()
                         : 0},
        {"index", startupSdoTable_->item(row, 1)
                      ? startupSdoTable_->item(row, 1)->text()
                      : QString()},
        {"subIndex", startupSdoTable_->item(row, 2)
                         ? startupSdoTable_->item(row, 2)->text()
                         : QString()},
        {"value", startupSdoTable_->item(row, 3)
                      ? startupSdoTable_->item(row, 3)->text()
                      : QString()},
        {"type", startupSdoTable_->item(row, 4)
                     ? startupSdoTable_->item(row, 4)->text()
                     : QString()},
    });
  }
  QJsonArray sdoTargetTrailArray;
  if (sdoTargetTrailTable_) {
    ensureSdoTargetTrailTable();
    for (int row = 0; row < sdoTargetTrailTable_->rowCount(); ++row) {
      const SdoTargetTrailRow trail =
          sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
      if (trail.index.trimmed().isEmpty() ||
          trail.subIndex.trimmed().isEmpty()) {
        continue;
      }
      sdoTargetTrailArray.append(QJsonObject{
          {"time", trail.time},
          {"position", trail.position},
          {"index", trail.index},
          {"subIndex", trail.subIndex},
          {"type", trail.type},
          {"source", trail.source},
          {"value", trail.value},
          {"writeValue", trail.writeValue},
          {"detail", trail.detail},
      });
    }
  }
  QJsonArray bookmarkArray;
  if (bookmark_->objectBookmarkTable) {
    for (int row = 0; row < bookmark_->objectBookmarkTable->rowCount(); ++row) {
      const SdoObjectBookmarkRow bookmark =
          sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, row);
      if (bookmark.index.trimmed().isEmpty() ||
          bookmark.subIndex.trimmed().isEmpty()) {
        continue;
      }
      bookmarkArray.append(QJsonObject{
          {"position", bookmark.position},
          {"slave", bookmark.slaveName},
          {"index", bookmark.index},
          {"subIndex", bookmark.subIndex},
          {"access", bookmark.access},
          {"type", bookmark.type},
          {"bits", bookmark.bits},
          {"name", bookmark.name},
          {"lastValue", bookmark.lastValue},
          {"source", bookmark.source},
      });
    }
  }
  QJsonArray masterArray;
  for (const auto &profile : settings_.masters) {
    masterArray.append(QJsonObject{
        {"name", profile.name},
        {"target", profile.target},
    });
  }
  QJsonArray sdoEvidenceArray;
  QStringList sdoEvidenceKeys = sdoEvidence_.keys();
  sdoEvidenceKeys.sort();
  for (const QString &key : sdoEvidenceKeys) {
    const QStringList evidence = sdoEvidence_.value(key);
    sdoEvidenceArray.append(QJsonObject{
        {"key", key},
        {"value", evidence.value(0)},
        {"status", evidence.value(1)},
        {"detail", evidence.value(2)},
        {"time", evidence.value(3)},
    });
  }
  QJsonArray ioVariableArray;
  QStringList ioVariableKeys = ioVariableMetadata_.keys();
  ioVariableKeys.sort();
  for (const QString &key : ioVariableKeys) {
    const QStringList parts = key.split('|');
    const QStringList metadata = ioVariableMetadata_.value(key);
    if (parts.size() != 3 || (metadata.value(0).trimmed().isEmpty() &&
                              metadata.value(1).trimmed().isEmpty() &&
                              metadata.value(2).trimmed().isEmpty())) {
      continue;
    }
    ioVariableArray.append(QJsonObject{
        {"position", parts.value(0).toInt()},
        {"index", parts.value(1)},
        {"subIndex", parts.value(2)},
        {"alias", metadata.value(0)},
        {"tags", metadata.value(1)},
        {"note", metadata.value(2)},
    });
  }

  const QJsonObject root{
      {"format", "NekoEcatStudioProject"},
      {"version", 1},
      {"name", QFileInfo(path).completeBaseName()},
      {"savedAt", QDateTime::currentDateTime().toString(Qt::ISODate)},
      {"selectedPosition", selectedPosition()},
      {"activeMaster", settings_.activeMaster},
      {"masters", masterArray},
      {"sdoIndex", sdoInspector_->sdoIndex->text()},
      {"sdoSubIndex", sdoInspector_->sdoSubIndex->text()},
      {"sdoEvidence", sdoEvidenceArray},
      {"slaves", slaveArray},
      {"topologyBaseline", baselineArray},
      {"watch", watchArray},
      {"sdoTargetTrail", sdoTargetTrailArray},
      {"objectBookmarks", bookmarkArray},
      {"ioVariables", ioVariableArray},
      {"startupSdos", startupArray},
      {"notes", rawText_->projectNotes ? rawText_->projectNotes->toPlainText() : QString()},
      {"snapshots",
       QJsonObject{
           {"master", lastMasterText_},
           {"slaveInfo", lastSlaveInfoText_},
           {"slaveInfoPosition", loadedSlaveInfoPosition_},
           {"pdo", lastPdoText_},
           {"pdoPosition", loadedPdoPosition_},
           {"sdo", lastSdoText_},
           {"sdoPosition", loadedSdoPosition_},
           {"xml", lastXmlText_},
           {"xmlPosition", loadedXmlPosition_},
       }},
  };

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return false;
  }
  file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
  projectPath_ = path;
  projectName_ = QFileInfo(path).completeBaseName();
  setWindowTitle(QString("NekoEcat Studio - %1").arg(projectName_));
  updateDiagnostics("Info", "Project", "Saved project: " + path);
  return true;
}


// — Deserialize a .ecatproj JSON file into all workspace tables and settings
bool MainWindow::readProjectFile(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }
  const auto document = QJsonDocument::fromJson(file.readAll());
  if (!document.isObject()) {
    return false;
  }
  const auto root = document.object();
  const QString format = root.value("format").toString();
  if (format != "NekoEcatStudioProject" && format != "EtherCATStudioProject") {
    return false;
  }

  if (root.contains("masters")) {
    QVector<MasterProfile> loadedMasters;
    for (const auto &value : root.value("masters").toArray()) {
      const auto object = value.toObject();
      MasterProfile profile;
      profile.name = object.value("name").toString();
      profile.target = object.value("target").toString().trimmed();
      if (profile.name.isEmpty()) {
        profile.name = QString("Master %1").arg(profile.target);
      }
      if (!profile.target.isEmpty()) {
        loadedMasters.append(profile);
      }
    }
    if (!loadedMasters.isEmpty()) {
      settings_.masters = loadedMasters;
    }
  }
  if (root.contains("activeMaster")) {
    settings_.activeMaster =
        root.value("activeMaster").toString(settings_.activeMaster).trimmed();
    if (settings_.activeMaster.isEmpty()) {
      settings_.activeMaster = "0";
    }
    client_.setMasterTarget(settings_.activeMaster);
    refreshMasterSelector();
    saveSettings();
  }

  QVector<SlaveInfo> loaded;
  for (const auto &value : root.value("slaves").toArray()) {
    const auto object = value.toObject();
    SlaveInfo slave;
    slave.position = object.value("position").toInt();
    slave.state = object.value("state").toString();
    slave.flags = object.value("flags").toString();
    slave.name = object.value("name").toString();
    slave.rawLine = object.value("rawLine").toString();
    loaded.append(slave);
  }
  updateSlaves(loaded);
  const int savedPosition = root.value("selectedPosition").toInt(-1);
  if (savedPosition >= 0 && topologyTree_) {
    bool restoredSelection = false;
    for (int top = 0;
         top < topologyTree_->topLevelItemCount() && !restoredSelection;
         ++top) {
      auto *masterItem = topologyTree_->topLevelItem(top);
      if (!masterItem) {
        continue;
      }
      for (int child = 0; child < masterItem->childCount(); ++child) {
        auto *slaveItem = masterItem->child(child);
        if (slaveItem &&
            slaveItem->data(0, Qt::UserRole).toInt() == savedPosition) {
          topologyTree_->setCurrentItem(slaveItem);
          restoredSelection = true;
          break;
        }
      }
    }
  }

  topologyBaseline_.clear();
  for (const auto &value : root.value("topologyBaseline").toArray()) {
    const auto object = value.toObject();
    SlaveInfo slave;
    slave.position = object.value("position").toInt();
    slave.state = object.value("state").toString();
    slave.flags = object.value("flags").toString();
    slave.name = object.value("name").toString();
    slave.rawLine = object.value("rawLine").toString();
    topologyBaseline_.append(slave);
  }
  updateTopologyBaselineSummary();

  const auto snapshots = root.value("snapshots").toObject();
  lastMasterText_ = snapshots.value("master").toString();
  lastSlaveInfoText_ = snapshots.value("slaveInfo").toString();
  lastPdoText_ = snapshots.value("pdo").toString();
  lastSdoText_ = snapshots.value("sdo").toString();
  lastXmlText_ = snapshots.value("xml").toString();
  const auto snapshotPosition = [&snapshots, savedPosition](const char *key) {
    return snapshots.contains(QLatin1String(key))
               ? snapshots.value(QLatin1String(key)).toInt(-1)
               : savedPosition;
  };
  const int currentSelectedPosition = selectedPosition();
  const int slaveInfoSnapshotPosition = snapshotPosition("slaveInfoPosition");
  const int pdoSnapshotPosition = snapshotPosition("pdoPosition");
  const int sdoSnapshotPosition = snapshotPosition("sdoPosition");
  const int xmlSnapshotPosition = snapshotPosition("xmlPosition");
  loadedSlaveInfoPosition_ =
      !lastSlaveInfoText_.isEmpty() &&
              slaveInfoSnapshotPosition == currentSelectedPosition
          ? slaveInfoSnapshotPosition
          : -1;
  loadedPdoPosition_ =
      !lastPdoText_.isEmpty() && pdoSnapshotPosition == currentSelectedPosition
          ? pdoSnapshotPosition
          : -1;
  loadedSdoPosition_ =
      !lastSdoText_.isEmpty() && sdoSnapshotPosition == currentSelectedPosition
          ? sdoSnapshotPosition
          : -1;
  loadedXmlPosition_ =
      !lastXmlText_.isEmpty() && xmlSnapshotPosition == currentSelectedPosition
          ? xmlSnapshotPosition
          : -1;
  sdoEvidence_.clear();
  for (const auto &value : root.value("sdoEvidence").toArray()) {
    const auto object = value.toObject();
    const QString key = object.value("key").toString().trimmed();
    if (key.isEmpty()) {
      continue;
    }
    sdoEvidence_.insert(key, {object.value("value").toString(),
                              object.value("status").toString(),
                              object.value("detail").toString(),
                              object.value("time").toString()});
  }
  ioVariableMetadata_.clear();
  for (const auto &value : root.value("ioVariables").toArray()) {
    const auto object = value.toObject();
    const int position = object.value("position").toInt(-1);
    const QString key =
        ioVariableTableObjectKey(position, object.value("index").toString(),
                                 object.value("subIndex").toString());
    const QString alias = object.value("alias").toString().trimmed();
    const QString tags = object.value("tags").toString().trimmed();
    const QString note = object.value("note").toString().trimmed();
    if (position >= 0 && !key.isEmpty() &&
        (!alias.isEmpty() || !tags.isEmpty() || !note.isEmpty())) {
      ioVariableMetadata_.insert(key, {alias, tags, note});
    }
  }
  if (bookmark_->objectBookmarkTable) {
    ensureObjectBookmarkTable();
    bookmark_->objectBookmarkTable->clearContents();
    bookmark_->objectBookmarkTable->setRowCount(0);
    const auto bookmarkArray = root.value("objectBookmarks").toArray();
    for (const auto &value : bookmarkArray) {
      const auto object = value.toObject();
      const QString index = object.value("index").toString().trimmed();
      const QString subIndex = object.value("subIndex").toString().trimmed();
      if (index.isEmpty() || subIndex.isEmpty()) {
        continue;
      }
      addObjectBookmark(
          object.value("position").toInt(-1), index, subIndex,
          object.value("access").toString(), object.value("type").toString(),
          object.value("bits").toString(), object.value("name").toString(),
          object.value("lastValue").toString(),
          object.value("source").toString());
    }
    updateObjectBookmarkRowDetail();
  }
  if (sdoTargetTrailTable_) {
    ensureSdoTargetTrailTable();
    sdoTargetTrailTable_->clearContents();
    sdoTargetTrailTable_->setRowCount(0);
    rememberedSdoTargetTrailKeys_.clear();
    const auto trailArray = root.value("sdoTargetTrail").toArray();
    sdoTargetTrailTable_->setRowCount(trailArray.size());
    int targetRow = 0;
    for (const auto &value : trailArray) {
      const auto object = value.toObject();
      const QString index = object.value("index").toString().trimmed();
      const QString subIndex = object.value("subIndex").toString().trimmed();
      if (index.isEmpty() || subIndex.isEmpty()) {
        continue;
      }
      const QStringList values = {
          object.value("time").toString(),
          QString::number(object.value("position").toInt(-1)),
          // Normalize hex address for consistent comparison
          normalizeHexText(index, 4),
          // Normalize hex address for consistent comparison
          normalizeHexText(subIndex, 2),
          object.value("type").toString(),
          object.value("source").toString(),
          object.value("value").toString(),
          object.value("writeValue").toString(),
          object.value("detail").toString(),
      };
      for (int column = 0; column < values.size(); ++column) {
        sdoTargetTrailTable_->setItem(targetRow, column,
                                      new QTableWidgetItem(values.at(column)));
      }
      rememberedSdoTargetTrailKeys_.insert(
          sdoTargetTrailRowKeyFromTable(sdoTargetTrailTable_, targetRow));
      ++targetRow;
    }
    sdoTargetTrailTable_->setRowCount(targetRow);
    sdoTargetTrailTable_->resizeColumnsToContents(); // auto-fit column widths
    updateSdoTargetTrailRowDetail();
  }
  rawText_->masterText->setPlainText(lastMasterText_);
  rawText_->infoText->setPlainText(lastSlaveInfoText_);
  rawText_->pdoText->setPlainText(lastPdoText_);
  rawText_->sdoText->setPlainText(lastSdoText_);
  rawText_->xmlText->setPlainText(lastXmlText_);
  updateMasterSummary(lastMasterText_);
  updateSlaveInfo(lastSlaveInfoText_);
  updatePdoTable(lastPdoText_);
  updateSdoTable(lastSdoText_);

  const auto watchArray = root.value("watch").toArray();
  if (!watchArray.isEmpty()) {
    ensureWatchTable();
    watch_->watchTable->setRowCount(watchArray.size());
    for (int row = 0; row < watchArray.size(); ++row) {
      const auto rowArray = watchArray.at(row).toArray();
      // Migrate older 6/7-column watch formats to current 12-column layout
      const bool legacySixColumnWatch = rowArray.size() == 6;
      // Migrate older watch formats to current layout
      const bool legacySevenColumnWatch = rowArray.size() == 7;
      QStringList migrated;
      // Migrate older 6/7-column watch formats to current 12-column layout
      if (legacySixColumnWatch) {
        migrated = {rowArray.at(0).toString(),
                    rowArray.at(1).toString(),
                    rowArray.at(2).toString(),
                    rowArray.at(3).toString(),
                    rowArray.at(4).toString(),
                    QString(),
                    QString(),
                    rowArray.at(5).toString(),
                    QString(),
                    QString()};
      // Migrate older watch formats to current layout
      } else if (legacySevenColumnWatch) {
        migrated = {rowArray.at(0).toString(),
                    rowArray.at(1).toString(),
                    rowArray.at(2).toString(),
                    rowArray.at(3).toString(),
                    rowArray.at(4).toString(),
                    QString(),
                    rowArray.at(5).toString(),
                    rowArray.at(6).toString(),
                    QString(),
                    QString()};
      } else {
        for (int column = 0; column < rowArray.size(); ++column) {
          migrated.append(rowArray.at(column).toString());
        }
      }
      for (int column = 0; column < 12; ++column) {
        watch_->watchTable->setItem(row, column,
                             new QTableWidgetItem(migrated.value(column)));
      }
    }
    updateWatchBaselineDeltas();
    updateWatchStartupDeltas();
    watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
  }
  ensureWatchTable();
  const auto startupArray = root.value("startupSdos").toArray();
  if (!startupArray.isEmpty()) {
    ensureStartupSdoTable();
    startupSdoTable_->setRowCount(startupArray.size());
    for (int row = 0; row < startupArray.size(); ++row) {
      const auto item = startupArray.at(row).toObject();
      startupSdoTable_->setItem(row, 0,
                                new QTableWidgetItem(QString::number(
                                    item.value("position").toInt())));
      startupSdoTable_->setItem(
          row, 1, new QTableWidgetItem(item.value("index").toString()));
      startupSdoTable_->setItem(
          row, 2, new QTableWidgetItem(item.value("subIndex").toString()));
      startupSdoTable_->setItem(
          row, 3, new QTableWidgetItem(item.value("value").toString()));
      startupSdoTable_->setItem(
          row, 4, new QTableWidgetItem(item.value("type").toString()));
      startupSdoTable_->setItem(
          row, 5, new QTableWidgetItem(uiText("Pending", "待应用")));
      startupSdoTable_->setItem(row, 6, new QTableWidgetItem);
    }
    startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
    updateWatchStartupDeltas();
    updateStartupSdoWatchEvidence();
  }
  if (rawText_->projectNotes) {
    rawText_->projectNotes->setPlainText(root.value("notes").toString());
  }

  sdoInspector_->sdoIndex->setText(root.value("sdoIndex").toString("0x1000"));
  sdoInspector_->sdoSubIndex->setText(root.value("sdoSubIndex").toString("0x00"));
  projectPath_ = path;
  projectName_ =
      root.value("name").toString(QFileInfo(path).completeBaseName());
  setWindowTitle(QString("NekoEcat Studio - %1").arg(projectName_));
  updateDiagnostics("Info", "Project", "Opened project: " + path);
  updateActionAvailability();
  updateSdoTargetTrailRowDetail();
  updateObjectBookmarkRowDetail();
  updateTopologyBaselineSummary();
  return true;
}


// — Prompt the user to select ESI XML files and add them to the repository
void MainWindow::importEsiFiles() {
  const QStringList files = QFileDialog::getOpenFileNames(
      this, uiText("Import ESI XML", "导入 ESI XML"), QDir::homePath(),
      "ESI XML (*.xml);;XML (*.xml);;All files (*)");
  if (files.isEmpty()) {
    return;
  }
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  QStringList repository = settings.value("esi/files").toStringList();
  for (const auto &file : files) {
    if (!repository.contains(file)) {
      repository << file;
    }
  }
  settings.setValue("esi/files", repository);
  updateDiagnostics("Info", "ESI",
                    QString("Imported %1 ESI file(s)").arg(files.size()));
  refreshEsiRepository();
}


// — Parse all stored ESI XML paths and populate the ESI repository table
void MainWindow::refreshEsiRepository() {
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  const QStringList repository = settings.value("esi/files").toStringList();
  QList<QStringList> rows;
  for (const auto &path : repository) {
    QFile file(path);
    QString vendor;
    QString name;
    QString type;
    QString product;
    QString revision;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QXmlStreamReader xml(&file);
      while (!xml.atEnd()) {
        xml.readNext();
        if (!xml.isStartElement()) {
          continue;
        }
        const QString n = xml.name().toString();
        if (n == "Vendor" && vendor.isEmpty()) {
          while (!xml.atEnd() &&
                 !(xml.isEndElement() && xml.name() == "Vendor")) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == "Id") {
              vendor = xml.readElementText().trimmed();
            }
          }
        } else if (n == "Type" && type.isEmpty()) {
          product = xml.attributes().value("ProductCode").toString();
          revision = xml.attributes().value("RevisionNo").toString();
          type = xml.readElementText().trimmed();
        } else if (n == "Name" && name.isEmpty()) {
          name = xml.readElementText().trimmed();
        }
        if (!vendor.isEmpty() && !name.isEmpty() && !type.isEmpty()) {
          break;
        }
      }
    }
    rows.append({QFileInfo(path).fileName(), vendor, product, revision, type,
                 name, path});
  }
  setTableRows(
      esiTable_,
      {"File", "Vendor", "Product", "Revision", "Type", "Name", "Path"}, rows);
}

