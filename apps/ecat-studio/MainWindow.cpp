// MainWindow.cpp — Core implementation: constructor, destructor, daemon lifecycle,
// online state management, and incremental refresh.
//
// This file covers the fundamental lifecycle methods of MainWindow. The remaining
// 789+ methods are split across 31 partial-class .cpp files in workspaces/, each
// covering a different workspace or concern (OD, Watch, Startup SDO, Free Run,
// Topology, State Machine, Diagnostics, I/O Variables, Consistency, Session,
// Export, Wiring, etc.). All partials share MainWindowIncludes.h as their
// precompiled header.
#include "detail/RealtimeChartDialog.h"
#include "MainWindow.h"
#include "infra/TranslationRegistry.h"
#include "infra/LanguageManager.h"
#include "Cia402DriveModel.h"
#include "CommissioningWorkflowModel.h"
#include "detail/WorkflowStepDetail.h"
#include "WorkflowTableAdapter.h"
#include "detail/CommissioningWorkflowDetail.h"
#include "detail/ConsistencyDetail.h"
#include "ConsistencyModel.h"
#include "ConsistencyTableAdapter.h"
#include "detail/DiagnosticsEventDetail.h"
#include "EvidenceModel.h"
#include "detail/FreeRunEntryDetail.h"
#include "detail/HostHealthDetail.h"
#include "detail/IoVariableDetail.h"
#include "IoVariableModel.h"
#include "NextBestActionModel.h"
#include "detail/NextBestActionDetail.h"
#include "detail/ObjectBookmarkDetail.h"
#include "detail/PdoMapDetail.h"
#include "ProcessDataRowModel.h"
#include "ProcessDataTableAdapter.h"
#include "SdoDictionaryTableAdapter.h"
#include "SdoEvidenceModel.h"
#include "SdoEvidenceTableAdapter.h"
#include "detail/SdoHistoryRowDetail.h"
#include "detail/SdoTargetTrailDetail.h"
#include "detail/SelectedDriveSummaryDetail.h"
#include "detail/SlaveEvidenceSummaryDetail.h"
#include "SessionBriefModel.h"
#include "SessionBriefTableAdapter.h"
#include "detail/SessionBriefDetail.h"
#include "SlaveEvidenceModel.h"
#include "SlaveEvidenceTableAdapter.h"
#include "detail/SlaveEvidenceDetail.h"
#include "detail/StartupSdoRowDetail.h"
#include "detail/StateMachineRowDetail.h"
#include "StateMachineTableAdapter.h"
#include "utils/Documentation.h"
#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"
#include "utils/UiHelpers.h"
#include "TopologyModel.h"
#include "detail/WatchRowDetail.h"
#include "WatchStartupModel.h"
#include "WatchStartupTableAdapter.h"
#include "detail/WatchStartupDetail.h"
#include "detail/WorkspaceBoundaryDetail.h"
#include "WorkspaceTabBadgeTableAdapter.h"
#include "detail/WorkspaceTabBadgeDetail.h"
#include "plugins/PluginRegistry.h"
#include "plugins/esi/EsiBrowserPlugin.h"
#include "plugins/busstats/BusStatsPlugin.h"
#include "plugins/oscilloscope/OscilloscopePlugin.h"
#include "plugins/protocol/ProtocolAnalyzerPlugin.h"
#include "plugins/project/ProjectPlugin.h"
// AlarmPlugin excised for AC1 volume/tabs cleanup (include + reg removed)
#include "plugins/eoe/EoEPlugin.h"
#include "plugins/soe/SoEPlugin.h"
#include "plugins/eni/EniExportPlugin.h"
#include "plugins/chart/ChartPlugin.h"
#ifdef ECAT_SCRIPTING_ENABLED
#include "plugins/automation/AutomationPlugin.h"
#endif
#include "plugins/themecustomizer/ThemeCustomizerPlugin.h"
#include "plugins/shortcuts/KeyboardShortcutsPlugin.h"
#include "plugins/preferences/UserPreferencesPlugin.h"
#include "plugins/pdomapping/PdoMappingEditorPlugin.h"
#include "plugins/dcsyncprecision/DcSyncPrecisionPlugin.h"
#include "plugins/onlinediagnostics/OnlineDiagnosticsPlugin.h"
#include "plugins/realtimeperf/RealtimePerformancePlugin.h"

#include "services/PerformanceMonitorService.h"
#include "services/ServiceContainer.h"
#include "plugins/overview/OverviewPlugin.h"
#include "plugins/od/OdPlugin.h"
#include "plugins/watch/WatchPlugin.h"
#include "plugins/freerun/FreeRunPlugin.h"
#include "plugins/iovariable/IoVariablePlugin.h"
#include "plugins/startupsdo/StartupSdoPlugin.h"
#include "plugins/session/SessionPlugin.h"
#include "plugins/consistency/ConsistencyPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "plugins/topology/TopologyPlugin.h"
#include "plugins/diagnostics/DiagnosticsPlugin.h"
#include "plugins/alevent/AlEventPlugin.h"
#include "plugins/dcsync/DcSyncPlugin.h"
#include "plugins/signal/SignalPlugin.h"
#include "plugins/rttest/RtTestPlugin.h"
#include "plugins/export/ExportPlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "plugins/onlinediagnostics/OnlineDiagnosticsPlugin.h"
#include "plugins/realtimeperf/RealtimePerformancePlugin.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProcess>
#include <QSettings>
#include <QStyle>
#include <QTableWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QElapsedTimer>

namespace {

/// Locate the ecatd binary relative to the application executable.
/// Checks common paths: same directory, ../ecatd/ecatd, then falls back to PATH.
QString ecatdPath() {
  const QFileInfo app(QCoreApplication::applicationFilePath());
  const QStringList candidates = {
      app.dir().absoluteFilePath("ecatd"),
      app.dir().absoluteFilePath("../ecatd/ecatd"),
  };
  for (const QString &candidate : candidates) {
    if (QFileInfo::exists(candidate)) {
      return QFileInfo(candidate).canonicalFilePath();
    }
  }
  return "ecatd";
}

/// Map a NextBestActionIconKey enum to a QStyle standard pixmap for the action button.
QStyle::StandardPixmap
nextBestActionStandardPixmap(NextBestActionIconKey icon) {
  switch (icon) {
  case NextBestActionIconKey::DriveNet:
    return QStyle::SP_DriveNetIcon;
  case NextBestActionIconKey::Warning:
    return QStyle::SP_MessageBoxWarning;
  case NextBestActionIconKey::DetailedView:
    return QStyle::SP_FileDialogDetailedView;
  case NextBestActionIconKey::ListView:
    return QStyle::SP_FileDialogListView;
  case NextBestActionIconKey::NewFolder:
    return QStyle::SP_FileDialogNewFolder;
  case NextBestActionIconKey::MediaPlay:
    return QStyle::SP_MediaPlay;
  case NextBestActionIconKey::ContentsView:
    return QStyle::SP_FileDialogContentsView;
  }
  return QStyle::SP_FileDialogContentsView;
}

/// Map a color key string ("error", "warning", default green) to a QColor for host health rows.
QColor hostHealthColorForKey(const QString &colorKey) {
  if (colorKey == QStringLiteral("error")) {
    return QColor("#ef4444");
  }
  if (colorKey == QStringLiteral("warning")) {
    return QColor("#f59e0b");
  }
  return QColor("#22c55e");
}

/// Map a color key string ("error", "warning", default blue) to a QColor for diagnostics event rows.
QColor diagnosticsEventColorForKey(const QString &colorKey) {
  if (colorKey == QStringLiteral("error")) {
    return QColor("#ef4444");
  }
  if (colorKey == QStringLiteral("warning")) {
    return QColor("#f59e0b");
  }
  return QColor("#60a5fa");
}

} // namespace

/// MainWindow constructor — full initialization sequence:
///
/// 1. Load persisted settings (QSettings) and configure the EcatClient target
/// 2. Create EventBus and ServiceContainer for shared runtime services
/// 3. Build the main UI layout (tabs, toolbars, panels, metric cards)
/// 4. Apply settings and custom keyboard shortcuts
/// 5. Create PluginRegistry and register workspace plugins:
///    - Core plugins: ESI, BusStats, Oscilloscope, Protocol, Project, Alarm, Chart, Dashboard
///    - Automation: Scripting (if ECAT_SCRIPTING_ENABLED), Batch operations
///    - Network: NetworkDiagnostics, MasterManager, DC Sync, SDO Cache
///    - Customization: Theme, Keyboard Shortcuts, User Preferences
///    - Analysis: Trace, Logic Analyzer, Diagram, Formula, Script Library
///    - Engineering: Simulation, Calibration, Documentation, Wizard, Template
///    - Reporting: Report, Dashboard Designer, Alarm Manager, Data Logger
///    - DevOps: Workflow Designer, Test Suite, Deployment, Config Editor
///    - Enterprise: Security, Compliance, Certification, Optimization, Monitoring, Analytics
/// 6. Wire all signal/slot connections between UI widgets, client, and timers
/// 7. Restore window geometry and state from QSettings
/// 8. Start the embedded ecatd daemon process
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QElapsedTimer startupTimer;
  startupTimer.start();

  loadSettings();
  client_.setMasterTarget(settings_.activeMaster);

  qint64 serviceInitMs = startupTimer.elapsed();
  startupTimer.restart();

  buildUi();
  applySettings();
  applyCustomShortcuts();

  qint64 uiBuildMs = startupTimer.elapsed();
  startupTimer.restart();

  pluginRegistry_ = new PluginRegistry(this);
  connect(pluginRegistry_, &PluginRegistry::registrationFailed, this,
          [this](const QString &reason, const QString &pluginId) {
            log(QString("Plugin registration failed: %1 (id: %2)")
                    .arg(reason, pluginId.isEmpty() ? "unknown" : pluginId));
            updateDiagnostics("Warning", "Plugins",
                              QString("Plugin registration failed: %1").arg(reason));
          });
  eventBus_ = new EventBus(this);
  container_ = new ServiceContainer(&client_, eventBus_, this);
  connect(container_, &ServiceContainer::serviceInitFailed, this,
          [this](const QString &serviceName, const QString &reason) {
            log(QString("Service initialization failed: %1 — %2")
                    .arg(serviceName, reason));
            updateDiagnostics("Error", "Services",
                              QString("Service '%1' failed to initialize: %2")
                                  .arg(serviceName, reason));
          });
  if (!container_->isInitialized()) {
    const QStringList errors = container_->initializationErrors();
    log(QString("ServiceContainer has %1 initialization error(s): %2")
            .arg(errors.size())
            .arg(errors.join(", ")));
    updateDiagnostics("Warning", "Services",
                      QString("Some services failed to initialize: %1")
                          .arg(errors.join(", ")));
  }
  pluginRegistry_->registerPlugin(new EsiBrowserPlugin(container_->esi(), this));
  pluginRegistry_->registerPlugin(new BusStatsPlugin(container_->busStats(), this));
  pluginRegistry_->registerPlugin(new OscilloscopePlugin(container_->oscilloscope(), this));
  pluginRegistry_->registerPlugin(new ProtocolAnalyzerPlugin(container_->protocolAnalyzer(), this));
  pluginRegistry_->registerPlugin(new EoEPlugin(container_->eoe(), container_->eventBus(), this));
  pluginRegistry_->registerPlugin(new SoEPlugin(container_->client(), container_->eventBus(), this));
  pluginRegistry_->registerPlugin(new EniExportPlugin(container_->client(), container_->eventBus(), this));

  pluginRegistry_->registerPlugin(new ProjectPlugin(container_->projectManager(), container_->configuration(), this));
  // Alarm disabled for tab/volume reduction (AC1); visible=false + no reg line

  pluginRegistry_->registerPlugin(new ChartPlugin(container_->chart(), this));

#ifdef ECAT_SCRIPTING_ENABLED
  pluginRegistry_->registerPlugin(new AutomationPlugin(container_->scripting(), this));
#endif

  pluginRegistry_->registerPlugin(new ThemeCustomizerPlugin(this));
  pluginRegistry_->registerPlugin(new KeyboardShortcutsPlugin(this));
  pluginRegistry_->registerPlugin(new UserPreferencesPlugin(this));

  pluginRegistry_->registerPlugin(new PdoMappingEditorPlugin(container_->pdoMapping(), this));
  pluginRegistry_->registerPlugin(new DcSyncPrecisionPlugin(&client_, container_->eventBus(), this));

  pluginRegistry_->registerPlugin(new OverviewPlugin(container_, this));
  pluginRegistry_->registerPlugin(new OdPlugin(container_, this));
  pluginRegistry_->registerPlugin(new WatchPlugin(container_, this));
  pluginRegistry_->registerPlugin(new FreeRunPlugin(container_, this));
  pluginRegistry_->registerPlugin(new IoVariablePlugin(container_, this));
  pluginRegistry_->registerPlugin(new StartupSdoPlugin(container_, this));
  pluginRegistry_->registerPlugin(new SessionPlugin(container_, this));
  pluginRegistry_->registerPlugin(new ConsistencyPlugin(container_, this));
  pluginRegistry_->registerPlugin(new StateMachinePlugin(container_, this));
  pluginRegistry_->registerPlugin(new TopologyPlugin(container_->eventBus(), this));
  pluginRegistry_->registerPlugin(new DiagnosticsPlugin(container_, this));
  auto *alEventService = container_->alEvent();
  pluginRegistry_->registerPlugin(new AlEventPlugin(container_->eventBus(), alEventService, this));
  auto *dcSyncService = container_->dcSync();
  pluginRegistry_->registerPlugin(new DcSyncPlugin(container_->eventBus(), dcSyncService, this));
  auto *signalService = container_->signal();
  pluginRegistry_->registerPlugin(new SignalPlugin(signalService, this));
  pluginRegistry_->registerPlugin(new RtTestPlugin(container_, this));
  pluginRegistry_->registerPlugin(new ExportPlugin(container_, this));
  pluginRegistry_->registerPlugin(new NotesPlugin(this));
  auto *perfMonitorService = container_->perfMonitor();
  pluginRegistry_->registerPlugin(new OnlineDiagnosticsPlugin(container_->onlineDiagnostics(), this));
  pluginRegistry_->registerPlugin(new RealtimePerformancePlugin(container_->realtimePerformance(), this));

  qint64 pluginLoadMs = startupTimer.elapsed();

  // Record startup timing
  perfMonitorService->beginStartup();
  perfMonitorService->recordStartupPhase("serviceInit", serviceInitMs);
  perfMonitorService->recordStartupPhase("uiBuild", uiBuildMs);
  perfMonitorService->recordStartupPhase("pluginLoad", pluginLoadMs);
  perfMonitorService->endStartup();

  wire();
  setMinimumSize(1120, 720);
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  const QByteArray geometry = settings.value("geometry").toByteArray();
  if (geometry.isEmpty() || !restoreGeometry(geometry)) {
    resize(1440, 900);
  }
  restoreState(settings.value("windowState").toByteArray());
  startEmbeddedDaemon();
  // If a non-English UI language was persisted, applySettings() already set the
  // active language but the UI was built in English above.  Rebuild once the
  // constructor has fully wired everything so the saved language shows on launch.
  if (LanguageManager::instance().currentLanguage() != Language::English) {
    QTimer::singleShot(0, this, [this] {
      rebuildUi();
      applyCustomShortcuts();
    });
  }
}

/// MainWindow destructor — cleanup sequence:
/// 1. Persist current settings and window geometry to QSettings
/// 2. Terminate the embedded ecatd daemon (with graceful shutdown timeout)
/// 3. If termination times out after 1.2s, force-kill the daemon process
MainWindow::~MainWindow() {
  saveSettings();
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  if (daemon_.state() != QProcess::NotRunning) {
    daemon_.terminate();
    if (!daemon_.waitForFinished(1200)) {
      daemon_.kill();
    }
  }
}

// Clear all cached online state — called on disconnect or master switch
void MainWindow::clearOnlineViews() {
  slaves_.clear();
  lastMasterText_.clear();
  lastSlaveInfoText_.clear();
  lastPdoText_.clear();
  lastSdoText_.clear();
  lastXmlText_.clear();
  loadedSlaveInfoPosition_ = -1;
  loadedPdoPosition_ = -1;
  loadedSdoPosition_ = -1;
  loadedXmlPosition_ = -1;
  freeRunEntryNames_.clear();
  freeRunObjectNames_.clear();
  freeRunEntryValues_.clear();
  freeRun_ = false;
  consistencyFresh_ = false;
  watchValues_.clear();
  sdoEvidence_.clear();
  watchChangedKeys_.clear();
  pendingSdoReads_.clear();
  pendingSdoReadTypes_.clear();
  pendingSdoWrites_.clear();
  pendingSdoVerifications_.clear();
  pendingStartupSdoChecks_.clear();
  topologyBaseline_.clear();
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  if (topologyTree_) {
    topologyTree_->clear();
  }
  for (auto *table :
       {metricTable_, workflow_->workflowTable, stateMachine_->stateMachineTable, identityTable_,
        slaveEvidence_->slaveEvidenceMatrixTable, portTable_, mailboxTable_, sdo_->pdoTable,
        sdo_->sdoTable, sdoHistoryTable_, freeRunWidgets_->freeRunTable, freeRunWidgets_->freeRunEntryTable,
        ioVar_->ioVariableTable, watch_->watchTable}) {
    if (table) {
      table->clear();
      table->setRowCount(0);
      table->setColumnCount(0);
    }
  }
  ensureWatchTable();
  ensureSdoHistoryTable();
  updateSdoHistoryRowDetail();
  updateSdoTargetTrailRowDetail();
  updateObjectBookmarkRowDetail();
  for (auto *editor : {rawText_->masterText, rawText_->infoText, rawText_->pdoText, rawText_->sdoText, rawText_->xmlText}) {
    if (editor) {
      editor->clear();
    }
  }
  if (selectedLabel_) {
    selectedLabel_->setText(activeMasterName());
  }
  setMetricCard(masterStateLabel_, uiText("Master", "主站"),
                uiText("Idle", "空闲"));
  setMetricCard(slaveCountLabel_, uiText("Slaves", "从站"), "0");
  setMetricCard(linkStateLabel_, uiText("Link", "链路"),
                uiText("Unknown", "未知"));
  setMetricCard(lossLabel_, uiText("Frame Loss", "丢帧"), "0");
  setMetricCard(freeRunLabel_, uiText("Free Run", "自由运行"),
                freeRun_ ? uiText("On", "开启") : uiText("Off", "关闭"));
  updateWatchAutoRefresh();
  updateIoVariableTable();
  updateSelectedSlavePanel();
  updateTopologyBaselineSummary();
  updateCommissioningWorkflow();
  updateStateMachineView();
  updateStatusBar();
}

// Launch the embedded ecatd process and start the connection-retry timer
void MainWindow::startEmbeddedDaemon() {
  connect(&daemon_, &QProcess::readyReadStandardError, this, [this] {
    const QString text =
        QString::fromLocal8Bit(daemon_.readAllStandardError()).trimmed();
    if (!text.isEmpty()) {
      log("ecatd: " + text);
    }
  });
  connect(&daemon_, &QProcess::readyReadStandardOutput, this, [this] {
    const QString text =
        QString::fromLocal8Bit(daemon_.readAllStandardOutput()).trimmed();
    if (!text.isEmpty()) {
      log("ecatd: " + text);
    }
  });
  connect(&daemon_, &QProcess::errorOccurred, this,
          [this](QProcess::ProcessError error) {
            QString message;
            switch (error) {
            case QProcess::FailedToStart:
              message = "ecatd failed to start — binary not found or permission denied. "
                        "Trying to connect to an external runtime.";
              break;
            case QProcess::Crashed:
              message = "ecatd process crashed unexpectedly. "
                        "Check system logs for details.";
              break;
            case QProcess::Timedout:
              message = "ecatd process timed out during startup.";
              break;
            case QProcess::WriteError:
              message = "ecatd write error — daemon may have exited.";
              break;
            case QProcess::ReadError:
              message = "ecatd read error — daemon output stream broken.";
              break;
            default:
              message = "Failed to launch embedded ecatd; trying to connect to an "
                        "external runtime";
              break;
            }
            log(message);
            updateDiagnostics("Warning", "Runtime", message);
          });
  connect(&daemon_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::CrashExit) {
              const QString message = QString("Embedded ecatd crashed (exit code %1)")
                                          .arg(exitCode);
              log(message);
              updateDiagnostics("Error", "Runtime", message);
            } else if (exitCode != 0) {
              const QString message = QString("Embedded ecatd exited with code %1")
                                          .arg(exitCode);
              log(message);
              updateDiagnostics("Warning", "Runtime", message);
            } else {
              log("Embedded ecatd exited normally");
            }
          });
  const QString path = ecatdPath();
  log("Starting embedded ecatd: " + path);
  daemon_.start(path, {"--foreground"});
  if (!daemon_.waitForStarted(3000)) {
    const QString message = "ecatd did not start within 3 seconds. "
                            "Ensure the binary exists at: " + path;
    log(message);
    updateDiagnostics("Warning", "Runtime", message);
  }
}

// Trigger an incremental refresh of all visible workspace data
void MainWindow::requestRefresh() {
  if (!client_.isConnected()) {
    return;
  }
  client_.ping();
  client_.freeRunStatus();
  if (!freeRun_) {
    client_.master();
    client_.scan();
  }
  if (rtTestRunning_) {
    client_.rtTestStatus();
  }
}
