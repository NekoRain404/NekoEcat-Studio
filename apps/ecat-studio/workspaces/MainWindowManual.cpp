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


namespace {


// — Locate the ecatd binary path relative to the application
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

} // namespace


// — Open the built-in user manual dialog with full HTML documentation
void MainWindow::showManual() {
  QDialog dialog(this);
  dialog.setObjectName("manualDialog");
  dialog.setWindowTitle(
      uiText("NekoEcat Studio User Manual", "NekoEcat Studio 使用说明书"));
  dialog.setModal(true);
  dialog.resize(980, 760);

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(18, 18, 18, 16);
  layout->setSpacing(12);

  auto *title = new QLabel(
      uiText("NekoEcat Studio User Manual", "NekoEcat Studio 使用说明书"));
  title->setObjectName("dialogTitle");

  auto *subtitle = new QLabel(
      uiText("Detailed operating guide for commissioning, diagnostics, SDO, "
             "PDO, Watch, Startup SDO, and Free Run workflows.",
             "面向调试、诊断、SDO、PDO、监视、启动 SDO "
             "与自由运行工作流的详细操作说明。"));
  subtitle->setObjectName("statusSummary");
  subtitle->setWordWrap(true);

  auto *browser =
      makeDocumentationBrowser("manualBrowser", settings_.theme == "Light");
  const ManualSearchControls manualTools = makeManualSearchControls(
      &dialog, browser, style(), uiText("Search manual text", "搜索说明书内容"),
      uiText("Previous", "上一个"), uiText("Next", "下一个"),
      uiText("Contents", "目录"));

  const QString english = QStringLiteral(R"HTML(
<!doctype html>
<html>
<head>
<style>
body { font-family: "Inter", "Segoe UI", "Noto Sans", sans-serif; font-size: 14px; line-height: 1.62; color: #172033; }
h1 { font-size: 26px; margin: 0 0 8px 0; color: #0f172a; }
h2 { font-size: 20px; margin: 26px 0 8px 0; color: #12376f; }
h3 { font-size: 16px; margin: 18px 0 6px 0; color: #1f2937; }
p { margin: 7px 0; }
ul, ol { margin-top: 6px; margin-bottom: 10px; padding-left: 22px; }
li { margin: 4px 0; }
a { color: #2563eb; text-decoration: none; }
.toc { background: #f7f9fc; border: 1px solid #d9e1ec; border-radius: 8px; padding: 12px 14px; }
.note { background: #eef4fb; border-left: 4px solid #2563eb; padding: 9px 12px; margin: 10px 0; }
.warn { background: #fff7ed; border-left: 4px solid #f97316; padding: 9px 12px; margin: 10px 0; }
.danger { background: #fef2f2; border-left: 4px solid #ef4444; padding: 9px 12px; margin: 10px 0; }
code { font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace; background: #eef2f7; border-radius: 5px; padding: 1px 5px; }
table { border-collapse: collapse; width: 100%; margin: 10px 0 16px 0; }
th, td { border: 1px solid #d9e1ec; padding: 7px 9px; vertical-align: top; }
th { background: #f0f4f9; color: #475569; }
</style>
</head>
<body>
<h1>NekoEcat Studio User Manual</h1>
<p>NekoEcat Studio is a modern EtherCAT engineering workstation for Linux systems using IgH EtherCAT Master. It combines project management, online bus inspection, Object Dictionary SDO workflows, PDO map analysis, cyclic Free Run telemetry, I/O variable engineering, host diagnostics, ESI repository management, and runtime logging in one application. Overview is reserved for bus and selected-slave context; host checks and diagnostic evidence are handled in the Diagnostics tab.</p>
<div class="toc" id="contents">
<b>Contents</b>
<ol>
<li><a href="#concept">Product Concept and Architecture</a></li>
<li><a href="#first-run">First Start and Runtime Connection</a></li>
<li><a href="#prerequisites">Host Prerequisites</a></li>
<li><a href="#masters">Multi-Master Configuration</a></li>
<li><a href="#topology">Topology and Slave Selection</a></li>
<li><a href="#overview">Overview Tab</a></li>
<li><a href="#state-machine">EtherCAT State Model</a></li>
<li><a href="#od">Object Dictionary and SDO</a></li>
<li><a href="#sdo-history">SDO History and Verification</a></li>
<li><a href="#pdo">PDO Map</a></li>
<li><a href="#watch">Watch</a></li>
<li><a href="#startup">Startup SDO</a></li>
<li><a href="#freerun">Free Run Telemetry</a></li>
<li><a href="#io-variables">I/O Variables</a></li>
<li><a href="#consistency">Consistency Check</a></li>
<li><a href="#diagnostics">Diagnostics and Host Health</a></li>
<li><a href="#esi">ESI Repository and XML</a></li>
<li><a href="#project">Projects, Notes, and Reports</a></li>
<li><a href="#shortcuts">Shortcuts and Fast Operations</a></li>
<li><a href="#trouble">Troubleshooting</a></li>
<li><a href="#safety">Safety Checklist</a></li>
</ol>
</div>

<h2 id="concept">1. Product Concept and Architecture</h2>
<p>The application is split into two parts: the GUI workstation and the <code>ecatd</code> runtime daemon. The GUI talks to <code>ecatd</code> over localhost TCP, and the daemon drives IgH command-line tools plus the ecrt-based Free Run path. This separation keeps the GUI responsive while online operations execute in the runtime layer.</p>
<table>
<tr><th>Layer</th><th>Responsibility</th></tr>
<tr><td>GUI</td><td>Project files, settings, topology tree, tabs, filters, command palette, dialogs, and presentation of online data.</td></tr>
<tr><td><code>ecatd</code></td><td>Connection endpoint, master target routing, command execution, host checks, SDO/PDO operations, Free Run process image telemetry.</td></tr>
<tr><td>IgH Master</td><td>Kernel/user-space EtherCAT master, slave scan, states, mailbox access, PDO/SDO information, process data exchange.</td></tr>
</table>
<p>Default runtime endpoint: <code>127.0.0.1:5877</code>. Active master: <code>%2</code>. Runtime binary: <code>%3</code>.</p>

<h2 id="first-run">2. First Start and Runtime Connection</h2>
<ol>
<li>Start NekoEcat Studio. The application attempts to launch or connect to <code>ecatd</code> automatically.</li>
<li>Check the connection pill in the upper area. Connected means the GUI has reached the runtime daemon, not necessarily that every slave is operational.</li>
<li>Click <b>Connect</b> when the daemon is not connected. Click <b>Refresh</b> to read master, slave, PDO, SDO, and summary data. Click <b>Rescan</b> when cables, power, or devices changed.</li>
<li>Use the master selector in the toolbar when more than one master profile exists.</li>
</ol>
<div class="note">A successful GUI connection can still show zero slaves if the EtherCAT service, NIC binding, link state, or slave power is wrong. Use the Diagnostics tab for host checks.</div>

<h2 id="prerequisites">3. Host Prerequisites</h2>
<p>Before commissioning hardware, confirm the Linux host has a working IgH EtherCAT Master installation, a configured master interface, the expected network driver, a usable device node, and permissions for the current user. These checks live in the Diagnostics tab.</p>
<table>
<tr><th>Prerequisite</th><th>What to Confirm</th></tr>
<tr><td>EtherCAT service</td><td>The master service starts cleanly after boot and exposes the expected master index.</td></tr>
<tr><td>Network interface</td><td>The NIC connected to the EtherCAT line is the one configured for the master and is not managed as a normal IP network link.</td></tr>
<tr><td>USB Ethernet adapters</td><td>Realtek USB adapters should use the intended <code>r8152</code>/<code>rt8152</code> driver path, including DKMS when required.</td></tr>
<tr><td>Blacklist and firmware</td><td>Required modules and firmware must not be blocked by blacklist files after reboot.</td></tr>
<tr><td>Device node</td><td><code>/dev/EtherCAT0</code> should exist, and the current user or group should have appropriate access.</td></tr>
</table>

<h2 id="masters">4. Multi-Master Configuration</h2>
<p>Open <b>Tools / Settings</b> to configure language, theme, scale, and master profiles. A master profile contains a display name and a runtime target such as master index <code>0</code>. The active master is shown in the toolbar and saved with preferences.</p>
<p>When switching masters, online views are cleared and subsequent commands target the selected master. This prevents accidentally applying state changes or SDO writes to a stale bus.</p>

<h2 id="topology">5. Topology and Slave Selection</h2>
<p>The topology tree is the primary selection surface. Selecting a slave updates the selected slave label and makes SDO, state transition, Watch, Startup SDO, PDO, and Free Run actions operate on that slave.</p>
<p>The Selected Slave panel includes an evidence score for the current slave. It summarizes identity, OD, PDO, Watch values, Startup SDO mismatches, Free Run process-image evidence, PDO-map issues, and topology-baseline issues so missing evidence is visible before a write, state transition, or output-related workflow.</p>
<p>Use <b>Snapshot</b> in the Selected Slave panel when you want a read-only evidence bundle for the current slave. It refreshes identity, Object Dictionary, PDO Map, ESI XML, and CiA 402 Watch evidence. It does not request state transitions and does not write SDOs or process outputs.</p>
<p>Typical sequence: select a slave, inspect Overview, read Object Dictionary entries, check PDO Map, add important values to Watch, then move to Free Run or Startup SDO if needed.</p>

<h2 id="overview">6. Overview Tab</h2>
<p>Overview is the default first tab. It shows high-frequency bus and selected-slave commissioning information: master metrics, identity information, port/link details, mailbox support, Session Brief, the multi-slave Slave Evidence Matrix, and workflow navigation. It intentionally does not run host diagnostics; use the Diagnostics tab for host checks, repair commands, and event evidence.</p>
<p><b>Session Brief</b> is a read-only decision layer assembled from already loaded UI evidence. It summarizes the current target, stale-aware Consistency gate, OD/PDO mapping readiness, current SDO evidence groups and pending write delta, Watch/Startup/Free Run runtime evidence, and the next workflow action. <b>Copy Row</b>, row-menu <b>Copy Row Evidence</b>, and Command Palette <b>Copy Session Brief Row Evidence</b> copy the selected brief row with status, evidence, next action, route key, current Next Best Action, row tooltip detail, and the local boundary for handoff. Double-clicking a brief row, pressing Enter, using <b>Open Local Evidence</b> from the row menu, or running <b>Open Session Brief Evidence</b> from the Command Palette opens the matching local evidence surface, such as the topology tree, Consistency table, Object Dictionary, Watch, Startup SDO, Free Run, or the next workflow row. Row copy and local evidence navigation do not read the bus, write SDOs, change state, toggle Free Run, or run Host Health.</p>
<p>The <b>Slave Evidence Matrix</b> compares every scanned slave with the same loaded-evidence model: priority, state, OD/PDO readiness, Watch values, Startup diffs, process-image rows, PDO map issues, risk text, and the next evidence action. Rows are automatically ordered as a commissioning queue: P0 Fault, P1 Risk, P2 Action, then P3 Ready. The Overview tab badge, boundary tooltip, matrix summary, and P0/P1/P2/P3 triage buttons expose live priority counts, and <b>Next Best Action</b> can route to the highest-priority matrix issue once stronger online prerequisites have been handled. Use the priority buttons or scope selector to focus All, P0 Fault, P1 Risk, P2 Action, P3 Ready, Risk, Action, Ready, Missing OD, Missing PDO, Missing Watch, Startup Diff, or Process Missing rows, and use the search box to find a slave, priority, state, risk, or next action across large buses. <b>Review First</b> opens the first visible highest-priority Risk or Action row, and <b>Review Next</b> advances from the current row to the next visible issue with wraparound, so a filtered list becomes a keyboard-friendly punch list. <b>Copy Row</b>, <b>Copy Matrix Row Evidence</b>, and <b>Copy Slave Matrix Row Evidence</b> copy the selected row's priority, readiness, risk, next action, detailed tooltip evidence, active scope, and local boundary to the clipboard for shift handoff or issue reports. Double-clicking a row, using <code>Alt+Enter</code>, choosing <b>Open Matrix Evidence</b>, <b>Review First Matrix Issue</b>, <b>Review Next Matrix Issue</b>, or <b>Copy Matrix Row Evidence</b> from the row menu, or running <b>Open Slave Matrix Evidence</b>, <b>Review First Slave Matrix Issue</b>, <b>Review Next Slave Matrix Issue</b>, or <b>Copy Slave Matrix Row Evidence</b> from the Command Palette selects that slave locally, opens the most relevant loaded evidence table, or copies the row digest. Filtering, navigation, and row copying do not call the runtime, load OD/PDO/ESI data, change state, toggle Free Run, or run Host Health.</p>
<p>The Commissioning Workflow card is an engineering workflow board rather than a static checklist. Each row is grouped by phase and shows status, step, risk, evidence, and next action, so missing topology, OD, PDO, Watch, Startup, Consistency, or process-image evidence is visible before a risky operation. Selecting a row updates the workflow detail strip with the row number, phase, status, action boundary, risk, evidence, and next action; hover it to see whether explicit execution will stay local, connect or rescan online, load OD/PDO data, add a local Watch row, open Consistency evidence, or toggle Free Run telemetry. The Scope selector focuses <b>All</b>, <b>Open</b>, <b>Blocked</b>, <b>Action</b>, <b>Ready</b>, <b>Risk</b>, or <b>Evidence Gap</b> steps, while the search box finds phase, status, step, risk, evidence, or next action text. The same workflow scopes are available from the Command Palette for keyboard-driven review. <b>Review First</b> selects the first visible non-ready workflow issue, and <b>Review Next</b> advances to the next visible issue with wraparound; the row menu and Command Palette expose the same review actions. <b>Run Next</b> executes the first workflow step that can move the session forward, while double-clicking a specific row runs or opens that row's suggested action. <b>Copy Step</b>, <b>Copy Step Evidence</b> from the row menu, <code>Alt+Enter</code>, or Command Palette <b>Copy Workflow Step Evidence</b> copies the selected phase, status, step, risk, evidence, next action, readiness summary, tooltip detail, current Next Best Action, and local boundary to the clipboard for handoff or pre-action review. Its readiness capsule shows an overall commissioning percentage, the next action, the number of open workflow items, and local filter counts; hover it to see each unresolved phase, status, risk, evidence, suggested action, and visible workflow scope statistics. The workflow treats Consistency as a stale-aware read-only gate before Free Run and later state progression: if online evidence changes, the gate must be refreshed before it is considered passed. The status bar also exposes <b>Next Best Action</b>, a global one-click navigator that can connect, rescan, focus slave selection, load OD/PDO data, add Watch rows, refresh or review Consistency, start Free Run, or jump to Diagnostics when existing errors need review. Workflow selection, filtering, issue review, detail review, and step copying are local UI/clipboard operations and do not read the bus, load OD/PDO/ESI, write SDOs, change state, toggle Free Run, or run Host Health. This keeps the overview useful as an operating surface without mixing host-health checks into it.</p>
<table>
<tr><th>Area</th><th>Use</th></tr>
<tr><td>Master Metrics</td><td>Confirm master state, slave count, link status, packet loss, and Free Run status.</td></tr>
<tr><td>Identity</td><td>Check vendor ID, product code, revision, serial number, type, and name.</td></tr>
<tr><td>Ports</td><td>Inspect physical link ports and topology hints.</td></tr>
<tr><td>Mailbox</td><td>Confirm CoE/SoE/FoE/EoE mailbox capabilities where available.</td></tr>
</table>
<p>The Selected Slave card also summarizes CiA 402 drive evidence from Watch rows for the current slave. When Watch contains statusword, mode display, error code, or controlword objects, the Drive line shows the decoded state and flags without moving host diagnostics into Overview. The Drive line is color-coded from neutral/action to warning/error/ok so fault and quick-stop evidence stands out. When a clear next controlword can be derived from the watched statusword, <b>Drive Next</b> becomes available and uses the normal confirmed SDO write path. If Object Dictionary evidence contains failed SDO rows, the workflow and global <b>Next Best Action</b> route directly to the failed OD evidence filter and select the first failed object for review.</p>

<h2 id="state-machine">7. EtherCAT State Model</h2>
<p>The <b>State Machine</b> tab is the dedicated EtherCAT state workspace. It sits after Free Run and before Diagnostics, so slave state decisions stay separate from host environment checks. Selecting a row updates a local transition detail strip with the slave, current state, recommended state, evidence, drive status, Startup evidence, PDO/process evidence, risk, and confirmation boundary. Selection and preview do not read the bus or request a state change; <b>Send Recommended</b>, PREOP, SAFEOP, OP, all-slave buttons, and row double-click remain the explicit request paths and keep the normal confirmation dialog. Host Health remains in Diagnostics only.</p>
<p>The state matrix lists every detected slave with current state, recommended next state, identity/OD/Watch evidence, decoded CiA 402 drive evidence, Startup SDO diff status, PDO/Free Run evidence, risk notes, and the next action. Recommendations are conservative: INIT can move to PREOP; PREOP only recommends SAFEOP when PDO and Watch evidence are present; SAFEOP only recommends OP when process-image evidence exists, Startup/PDO map risks are clear, and the stale-aware Consistency gate is passed. If evidence is missing or stale, the recommendation stays empty and the row asks you to review evidence instead of sending a misleading state request.</p>
<p>Use <b>Send Recommended</b> or double-click a matrix row to send the row's recommendation. Use PREOP, SAFEOP, or OP row buttons for an explicit selected-slave request. <b>All PREOP</b> and <b>All SAFEOP</b> request the state for every detected slave. All of these paths reuse the normal safety confirmation flow.</p>
<p>State buttons operate on the selected slave, while the Online menu and state-machine bulk buttons can request state transitions for all slaves. Treat state changes as online operations, not UI-only actions.</p>
<p>State-change confirmation is evidence-driven and grouped by risk tier. The confirmation review separates <b>Critical Impact</b>, <b>Review Before Confirming</b>, <b>Evidence</b>, and <b>Target Context</b> so high-impact output, drive, stale Consistency, topology, and missing-evidence concerns are visible before the request is sent. For a selected slave it includes the current state, requested state, selected-slave evidence score, CiA 402 Watch evidence where available, Startup SDO mismatches, Free Run PDO-map issues, topology-baseline warnings, and Consistency gate status. OP and SAFEOP confirmations explicitly call out missing PDO, Watch, process-image, Startup, or stale/failed Consistency evidence before the request is sent.</p>
<table>
<tr><th>State</th><th>Typical Use</th><th>Common Caution</th></tr>
<tr><td>INIT</td><td>Reset or lowest communication state.</td><td>Mailbox access is normally not available.</td></tr>
<tr><td>PREOP</td><td>Good state for Object Dictionary and Startup SDO work.</td><td>Process data is not fully active.</td></tr>
<tr><td>SAFEOP</td><td>Validate PDO mapping and input process data before OP.</td><td>Outputs may remain constrained by device rules.</td></tr>
<tr><td>OP</td><td>Operational process data exchange.</td><td>Outputs and drive behavior can become active; confirm machine safety first.</td></tr>
</table>

<h2 id="od">8. Object Dictionary and SDO</h2>
<p>The <b>Selected Object</b> table now includes an <b>Action</b> column. It makes each row's local intent visible before interaction: open Watch, open Startup, open Bookmark, open Trail, review evidence, review delta, focus OD, copy row, or copy the full digest. <b>Run Row Action</b> changes its label to the selected row action, such as <b>Run: Open Watch</b>, then executes that local action without requiring users to remember double-click, <code>Alt+Enter</code>, or the row menu. <b>Copy Row</b> also follows the selected field, such as <b>Copy: Target</b>, and copies only that row's evidence bundle to the clipboard. These row actions are local evidence/navigation actions and do not read the bus, write SDOs, change state, toggle Free Run, or run Host Health.</p>
<p>The Object Dictionary tab is the central SDO workspace. Select a row in the dictionary table to automatically fill the SDO command fields: slave position, object index, subindex, and data type. The table keeps <b>Last Value</b> and <b>Last Status</b> evidence columns, so recent SDO reads and write/read-back results remain visible in the dictionary itself. The <b>Selected Object</b> panel acts as a target workbench: it shows the active target, source, category, object name, access mode, type/bits, read value, write value, table evidence, whether the same object is already present in Watch, Startup SDO, Object Bookmarks, or SDO Target Trail, and a recommended next action. <b>Evidence Set</b> summarizes whether local Read, Watch, OD, Startup, Bookmark, and Target Trail values agree before you type a write value. <b>Use Evidence</b> copies the best local value into the write field without bus access; <b>Pick Evidence</b> opens the same local candidates so you can choose the exact value when evidence conflicts. <b>Write Delta</b> compares the pending write value with local evidence before the write confirmation: green means the value already matches evidence, amber means the pending write differs from the available evidence, and red means the local evidence itself conflicts. <b>Review Delta</b> opens the most relevant local evidence row behind that comparison, including matching Target Trail evidence; when the write field is empty it reviews Evidence Set conflicts, and when a write value exists it reviews the Write Delta. <b>Copy Evidence</b> copies a local digest of the current target, Selected Object review rows, evidence candidates, evidence links, and the local-only boundary for handoff or pre-write review. Double-clicking or pressing <code>Alt+Enter</code> on a Selected Object row turns the panel into a local evidence router: Watch, Startup, Bookmark, and Target Trail rows open their matching evidence; Evidence Set and Write Delta review the relevant conflict or delta; Target/Read/OD rows focus the Object Dictionary context; summary rows copy the evidence digest. Right-clicking a Selected Object row exposes explicit <b>Open Row Evidence</b>, <b>Copy Row Evidence</b>, and <b>Copy Full Evidence Digest</b> actions, so the same local evidence routing is discoverable without memorizing shortcuts. The SDO write confirmation repeats the same local Evidence Set for the exact slave/index/subindex being written, then states whether the target value matches all evidence, only part of it, or none of it. The <b>Open Watch</b>, <b>Open Startup</b>, <b>Open Bookmark</b>, and <b>Open Trail</b> buttons jump to the matching evidence row for the same slave/index/subindex without reading or writing the bus. The panel also states the safety boundary: panel updates, Evidence Set, Use Evidence, Pick Evidence, Write Delta, Review Delta, Copy Evidence, row evidence copy, and evidence jumps only organize local context; reads, Watch additions, writes, and Startup changes still require explicit button actions. Local buttons can read the target, write through the normal confirmation path, add the object to Watch, bookmark it, or create a Startup SDO row from the current write value. Double-click a dictionary row to fill the fields and immediately issue an SDO read. Select multiple rows and use <b>Read Selected</b>, <b>Watch Selected</b>, or <b>Startup Evidence</b> to inspect, monitor, or turn already-read Last Value evidence into Startup SDO candidates without repeating single-row actions.</p>
<p>Use the semantic filter chips to move faster on large dictionaries: <b>Writable</b>, <b>Readable</b>, <b>CiA 402</b>, <b>Identity</b>, <b>PDO</b>, <b>Errors</b>, <b>Evidence</b>, and <b>Failed</b> apply tagged filters such as <code>tag:cia402</code>, <code>tag:evidence</code>, or <code>tag:failed</code>. The OD summary shows visible, writable, evidence, and failed counts for the current table. Normal free-text filtering still works across object, index, access, type, bits, name, Last Value, and Last Status columns. After filtering, <b>Read Visible</b> reads the visible result set with confirmation for large batches, <b>Retry Failed</b> reads only rows whose latest SDO evidence failed, while <b>Watch Visible</b> turns the same result set into Watch rows without issuing immediate mailbox reads. The Object Dictionary context menu exposes the same visible-result read/watch actions, failed-evidence retry, Evidence/Failed filters, <b>Bookmark Object</b>, <b>Bookmark Selected Objects</b>, <b>Create Startup SDOs from Selected Evidence</b>, and <b>Copy Last Evidence</b>, so row evidence can be reused without leaving the table.</p>
<p><b>Object Bookmarks</b> is a project-local shortlist for objects you repeatedly touch during commissioning, such as drive controlword/statusword, mode objects, error registers, vendor parameters, or known startup candidates. <b>Bookmark</b> saves the current SDO target, and <b>Bookmark Selected</b> saves selected OD rows. Selecting a bookmark updates a local detail strip with slave, object address, access, type, bits, name, saved Last Value, source, reuse readiness, and operation boundary. Double-clicking a bookmark fills the SDO fields only; it does not read, write, change state, or run host diagnostics. Use <b>Watch Bookmark</b> to add bookmarked objects to Watch without immediate reads. Use <b>Create Startup</b> to create or update Startup SDO rows from selected bookmarks using their saved Last Value; this is a project-table change only and does not access the bus. Remove bookmarks when the project cleanup is done. Bookmarks are saved in the <code>.ecatproj</code> file.</p>
<p>The current SDO fields are also exposed through the Command Palette. Press <code>Ctrl+P</code> to read the current object, write it through the normal validation and confirmation path, use the read value, best local evidence, or a manually chosen evidence candidate for the write field, add it to Watch, review Write Delta evidence, copy the current SDO evidence digest, open or copy the currently selected <b>Selected Object</b> row evidence, open matching Watch/Startup/Bookmark/Target Trail evidence, create a Startup SDO row, create Startup SDO candidates from selected OD evidence, Object Bookmarks, or a selected SDO Target Trail row, restore a Target Trail row, add a Target Trail row to Watch, bookmark it, or copy its address/value from any tab. The <b>Active SDO Inspector</b> and <b>Selected Object</b> panel summarize the active master, slave, object, type, read value, write value, source table, write permission, object category, validation status, Watch link, Startup link, bookmark link, Target Trail link, Write Delta, and next action so the target and value change are visible before every read or write. The <b>SDO Target Trail</b> records recent local targets from Object Dictionary, PDO Map, Watch, Free Run, I/O Variables, SDO History, Startup SDO, Object Bookmarks, CiA 402 helpers, and manual fields. Selecting a trail row updates a local detail strip with time, slave, object address, type, source, value, write value, detail, reuse readiness, and operation boundary. Matching trail rows now participate in Evidence Set, Use Evidence, Pick Evidence, and Write Delta review. Double-clicking or restoring a trail row refills the SDO target locally; the Watch, Bookmark, and Startup buttons reuse the selected trail row locally, preserving its saved value when available and using the write value or last value for Startup SDO candidates. The trail is saved with the project, and these reuse actions do not read the bus, write SDOs, change state, toggle Free Run, or run Host Health.</p>
<h3>Read Workflow</h3>
<ol>
<li>Select the target slave in the topology tree.</li>
<li>Open <b>Object Dictionary</b> and refresh online data if needed.</li>
<li>Click an object entry. The SDO form is populated automatically. The read value field is cleared unless that exact row already has Last Value evidence.</li>
<li>Click <b>Read SDO</b>, <b>Read Selected</b>, or <b>Read Visible</b>. The value field is updated only when the daemon returns the upload result for the currently selected slave/index/subindex; other batch or Watch responses update Object Dictionary evidence, Watch, and history without overwriting the active target value. Batch reads and Watch refreshes preserve the source row type where available, so SDO History and Watch decoding do not silently inherit the manual type selector. Each Object Dictionary row records Last Value and Last Status evidence.</li>
<li>For writable objects, click <b>Use Read Value</b> to copy the returned value into the write field, <b>Use Evidence</b> to copy the best available local Read/Watch/OD/Startup/Bookmark/Target Trail value, or <b>Pick Evidence</b> to choose the exact candidate before tuning or creating a Startup SDO row.</li>
</ol>
<h3>Write Workflow</h3>
<ol>
<li>Verify the entry is writable. Read-only entries disable the write field to reduce accidental writes.</li>
<li>Enter the value in the write field, or start from <b>Use Read Value</b> and edit only the part that should change. Review <b>Write Delta</b> in the Selected Object panel before committing the write; it highlights whether the typed value is unchanged, differs from local evidence, or conflicts with mixed evidence from Read/OD/Watch/Startup/Bookmarks/Target Trail.</li>
<li>Click <b>Write SDO</b>. Before the request is sent, the confirmation review groups the impact preview into Critical Impact, Review Before Confirming, Evidence, and Target Context. It includes slave state/name, object class, local Evidence Set agreement or conflict across Read, Watch, OD, Startup, Bookmarks, and Target Trail, target-to-evidence match status, current-to-target change, Startup expectation, topology-baseline warnings, and risk flags for drive control, PDO mapping, or persistent storage objects.</li>
<li>After the download completes, NekoEcat Studio automatically reads the same object back and compares the actual value with the expected write value. Matching rows are marked OK; mismatches or read-back failures are recorded as failed Object Dictionary evidence and can be reviewed through <b>Next Best Action</b> or the Failed filter.</li>
</ol>
<p>Before a write is submitted, the GUI validates the object address and value format: index must fit <code>0x0000..0xffff</code>, subindex must fit <code>0x00..0xff</code>, numeric types must parse as numbers and fit the selected integer width where applicable, and unsupported or empty types are surfaced as warnings.</p>
<h3>Value and Type Guidelines</h3>
<table>
<tr><th>Type</th><th>Input Guidance</th></tr>
<tr><td>Unsigned integers</td><td>Use decimal or hexadecimal when supported by the runtime, for example <code>0</code>, <code>1</code>, or <code>0x6040</code>.</td></tr>
<tr><td>Signed integers</td><td>Use the range expected by the device manual. Negative values must match the selected type width.</td></tr>
<tr><td>Float / Double</td><td>Use normal decimal notation and verify unit scaling in the manual.</td></tr>
<tr><td>String / Octet string</td><td>Confirm whether the device expects ASCII text, raw bytes, or vendor-specific formatting.</td></tr>
</table>
<h3>SDO Target Trail Reuse</h3>
<p>Use <b>Restore Target</b> when you want to bring a recent object back into the SDO fields without reselecting it in the source table. The selected-row detail strip shows whether the row has a saved value, a write value, a usable Startup candidate, and which actions stay local. Use <b>Watch</b> to add that trail row to Watch without an immediate SDO read, <b>Bookmark</b> to save it as a project Object Bookmark, and <b>Startup</b> to create a local Startup SDO candidate. Startup creation prefers the saved Write value and falls back to the saved Value column. When the current target matches a trail row, the trail value is also part of the Selected Object Evidence Set and can be opened with <b>Open Trail</b>. These actions only update local UI/project tables and are also available from the Target Trail context menu and Command Palette.</p>
<div class="warn">SDO writes can change drive parameters, mode settings, calibration values, or persistent device configuration. Always confirm the selected slave, index, subindex, type, and expected range before writing.</div>

<h2 id="sdo-history">9. SDO History and Verification</h2>
<p>The SDO Operation History table records read, write, and verification results during the current session. It is intended as an audit trail for commissioning decisions and handoff review. Failed rows keep the returned detail so that type mismatch, access rights, object-not-found, timeout, and mailbox-state problems can be reviewed later.</p>
<p>Selecting a history row updates a local detail strip with the operation time, action, slave, index/subindex, type, value, status, detail, reuse readiness, and operation boundary. Selection, detail review, filling SDO fields from history, adding selected rows to Watch, and creating Startup SDO candidates from completed value rows are local audit/reuse actions. Right-click a history row to fill the SDO fields again, fill and read the same object, add the object to Watch, or create a Startup SDO from the recorded value. Select multiple history rows and use <b>Watch Selected</b> or the right-click bulk action to create a Watch set without issuing immediate mailbox reads. Use <b>Create Startup</b> or the selected-history context action to turn rows with recorded values into Startup SDO rows; failed, pending, address-less, or empty-value rows are skipped. Double-click and Fill and Read explicitly read the selected SDO through the normal SDO read path.</p>
<p>This turns successful field tests into reusable engineering configuration. A typical flow is: manually tune or read a device object, verify the accepted value in history, select the relevant history rows, create Startup SDO rows, then use Startup SDO preflight and verification before applying the sequence again.</p>
<p>After an SDO write, the application requests a read-back verification where possible. Use this result before turning a manual value into a Startup SDO row. The confirmation path is evidence-driven and risk-grouped: it compares the target against the local Read/Watch/OD/Startup/Bookmark/Target Trail Evidence Set, shows Startup expectations, and highlights topology-baseline or high-impact object risks before the final confirmation button can be accepted. The history can be cleared for a new test pass without clearing the project notes or diagnostics.</p>

<h2 id="pdo">10. PDO Map</h2>
<p>PDO Map shows process data entries parsed from online PDO information. It supports filtering and quick actions. Use it to understand what each cyclic process-data byte represents before enabling Free Run or interpreting decoded telemetry.</p>
<p>Selecting a PDO row updates a local detail strip with Sync Manager, PDO, direction, index/subindex, bit width, inferred SDO type, name, process role, CiA 402 candidate evidence, and the operation boundary. After PDO Map data is loaded, selection, filtering, and this detail preview are local review actions. Loading or refreshing PDO Map is the explicit online PDO evidence path. Double-click or right-click a PDO entry to fill the SDO command fields; double-click also reads the object through the normal SDO read path. You can also add relevant entries to Watch. Select multiple PDO rows and use <b>Add Selected to Watch</b> or the right-click bulk action to create a Watch set without duplicating existing rows. Bulk add does not immediately read every object; use <b>Refresh Watch</b> when you are ready so large selections do not overload mailbox traffic.</p>
<p>Filtering by index, subindex, name, direction, or data type helps isolate RxPDO or TxPDO entries on large drives before adding a focused set to Watch.</p>

<h2 id="watch">11. Watch</h2>
<p>Watch is a focused SDO monitor. Add an SDO from the current Object Dictionary, PDO Map, Free Run entry, I/O Variables, or manual SDO fields. Each row stores time, slave, index, subindex, raw value, decoded meaning, type, mode, baseline, baseline delta, Startup SDO expected value, and Startup delta.</p>
<p>For CiA 402 drives, use <b>CiA 402 Preset</b> or the Command Palette to add common drive objects in one action: controlword, statusword, operation mode, mode display, error code, actual position, actual velocity, actual torque, target position, target velocity, and target torque. Existing Watch rows are reused, so the preset can be applied repeatedly during commissioning without duplicating objects. The Decoded column translates statusword values into drive states such as Switch on disabled, Ready to switch on, Operation enabled, Quick stop active, or Fault, and annotates useful flags such as warning, remote, target reached, and internal limit.</p>
<p>The Command Palette also provides CiA 402 controlword actions for shutdown, switch on, enable operation, quick stop, and fault reset. When the current slave has a decoded statusword in Watch, the palette adds a recommended next controlword based on that state. These actions prepare <code>0x6040:0x00</code> as <code>uint16</code> and then use the same validated and confirmed SDO write path as the manual Write button.</p>
<p>Right-click a Watch row to fill the SDO fields, read the object, remove the row, create a Startup SDO from the current watched value, or sync the watched value into Startup SDO. Select multiple Watch rows and use <b>Create Startup</b> to append accepted live values as new startup rows, or <b>Sync Startup</b> to update existing matching Startup SDO rows by slave/index/subindex. Sync creates missing rows only when no matching startup row exists, asks for confirmation, and changes only the Startup SDO table; it does not write to the bus.</p>
<p>Use <b>Refresh Watch</b> for manual reads. Enable <b>Auto</b> to poll at 250 ms, 500 ms, 1 s, or 2 s. Use <b>Capture Baseline</b> after a known-good state to store current values, then watch <b>Delta</b> for numeric drift or text changes. Watch also compares each matching object against Startup SDO rows, showing the configured Startup value and whether the live value matches it. <b>Clear Baseline</b> removes the saved baseline and drift result. The Scope selector turns Watch into an engineering review surface: <b>All</b>, <b>Selected Slave</b>, <b>Changed</b>, <b>Baseline Drift</b>, <b>Startup Diff</b>, <b>Missing Value</b>, and <b>CiA 402</b> isolate the rows that matter during commissioning. The same scopes are available from the Command Palette for keyboard-driven reviews. The summary shows item count, active scope, refresh mode, recent value changes, baseline drift, Startup mismatches, and missing values. Value changes, baseline deltas, and Startup deltas are highlighted to make live tuning easier. After tuning a parameter online, refresh Watch, confirm the value, then use <b>Sync Startup</b> to keep the repeatable startup list aligned without duplicating rows.</p>
<p>Decoded CiA 402 Watch evidence is also reflected in the Selected Slave card as a compact Drive summary, so Overview can show selected-slave drive state while Diagnostics remains the only host-health workspace. The <b>Drive Next</b> button appears when that evidence supports a recommended controlword, changes its label to the exact next action, and shows the target <code>0x6040:0x00 uint16</code> value in its tooltip before the normal write confirmation.</p>

<h2 id="startup">12. Startup SDO</h2>
<p>Startup SDO stores configuration writes that should be applied as a repeatable commissioning sequence. Add rows from the current SDO fields, edit value/type as needed, and click <b>Apply Startup SDO</b>. The page summary reports total startup rows, Watch matches, Watch diffs, pending comparisons, missing Watch evidence, and how many rows <b>Apply Diffs</b> will write. The table also shows <b>Watch Value</b> and <b>Watch Delta</b> when matching Watch rows exist, so the startup list itself shows whether the configured value already matches current device evidence. Selecting a row updates a local detail strip with the row number, slave, index/subindex, type, expected value, status, Watch value, Watch comparison, detailed tooltip evidence, and the write boundary. Row selection, <b>Diffs Only</b>, and this detail preview are local review actions; <b>Verify</b> reads target SDOs, while <b>Apply Row</b>, <b>Apply Selected</b>, <b>Apply Diffs</b>, and <b>Apply Startup</b> write SDO values only through the normal confirmation flow. Before applying all rows, selected rows, or Watch-diff rows, the confirmation review refreshes Watch evidence and groups match/diff/no-watch counts, risk rows, topology-baseline warnings, and current-to-target previews into the same Critical Impact, Review, Evidence, and Target Context sections.</p>
<p>Use <b>Review Diffs</b> to open the Startup SDO page, enable <b>Diffs Only</b>, and select the first startup row whose expected value differs from current Watch evidence. This creates a review step before writing: the engineer can inspect exactly which rows would be touched, then decide whether to sync Watch into Startup, edit the expected value, verify selected rows, or apply only the diffs. The global <b>Next Best Action</b> also points to this review surface when Watch evidence shows Startup mismatches.</p>
<p>Right-click a Startup SDO row to fill the SDO fields, fill and read the object, add it to Watch, verify only that row, apply only that row, review or filter Watch diffs, move rows, remove selected rows, or copy the object address/value. Select multiple rows and use <b>Verify Selected</b> or <b>Apply Selected</b> to test or write only a focused subset while leaving the rest of the startup sequence untouched. Double-click a row to fill and read immediately. This makes the startup list a live commissioning surface instead of a passive export table.</p>
<p>Use <b>Move Up</b> and <b>Move Down</b> to control execution order. Some devices require mode, mapping, limit, and vendor-specific objects to be written in a strict sequence, so the row order should match the vendor manual and your commissioning notes.</p>
<p>Use <b>Preflight</b> before writing. Preflight checks for empty fields, invalid object address ranges, value/type format problems, invalid slave positions, missing online slaves, duplicate objects, and topology baseline mismatches. Duplicate objects with the same value are warnings; duplicate objects with conflicting values are errors. Rows with Preflight Error must be fixed before Apply Startup SDO continues.</p>
<p>Use <b>Verify Startup</b> to read every listed object and compare the current device value with the expected startup value. Matching rows become Verified; mismatches show the actual read-back value in Detail. Use <b>Verify Selected</b> when only a small group changed and you do not want to poll the whole list. When Watch already contains current values for matching startup objects, <b>Apply Diffs</b> writes only Startup SDO rows whose expected value differs from the current Watch value. This is useful after tuning or verification because it avoids rewriting matching rows and keeps the confirmation scope small.</p>
<p>The table contains per-row status and detail columns. During application rows move through applying, applied, or failed states. Failed rows include returned detail so you can distinguish type mismatch, mailbox failure, device state restrictions, and invalid object access. Use <b>Apply Selected</b> for a controlled partial retry or when only a few parameters changed; use <b>Apply Diffs</b> when Watch evidence shows exactly which startup values are still different.</p>
<div class="warn">Apply Startup SDO only after the selected bus and slave positions are stable. If the topology changes, review every row before applying the sequence again.</div>

<h2 id="freerun">13. Free Run Telemetry</h2>
<p>Free Run starts cyclic process-image telemetry through the runtime daemon. The Free Run tab shows raw status and a structured process-image table. It is useful when you need online process data without a full PLC runtime. Before Free Run starts, the confirmation review groups a process-image impact preview: selected-slave context, bus state mix, PDO map evidence, RxPDO/output counts and examples, previous Free Run cache, decoded drive Watch evidence, topology-baseline issues, Consistency gate status, and the faster GUI refresh cadence.</p>
<p>Use the filter box to focus by name, index, subindex, direction, value, or map status. Enable <b>Changed only</b> to keep only recently changed entries visible. Selecting a process-image row updates the Free Run detail strip with the slave, direction, object address, name, value, bit location, PDO map status, name source, change state, and input/output boundary. Selection, filtering, and the detail strip are local review actions; toggling Free Run remains the explicit online action. Raw and decoded values highlight when they change. Entry names are stabilized locally: I/O variable aliases are preferred, then runtime names, PDO-map names, object-level cache, and entry-level cache before falling back to the object address, so intermittent runtime payloads do not cause names to disappear. <b>Map Detail</b> also shows the name source. The <b>Map Status</b> and <b>Map Detail</b> columns compare each selected-slave process-image entry against the currently loaded PDO Map, showing mapped rows, missing map evidence, direction mismatch, or bit-length mismatch. When CiA 402 objects such as <code>0x6040</code>, <code>0x6041</code>, <code>0x6060</code>, <code>0x6061</code>, or <code>0x603f</code> appear in process data, the Meaning column uses the same drive-state decoder as Watch unless the runtime already supplied a more specific meaning.</p>
<div class="danger">Free Run can exchange process data with devices. Verify machine state, drive enable behavior, output mapping, and the impact preview before using it on hardware connected to actuators.</div>

<h2 id="io-variables">14. I/O Variables</h2>
<p>The <b>I/O Variables</b> tab is an engineering signal table. It merges the loaded PDO Map, Free Run process-image rows, Watch values, Startup SDO expectations, map evidence, and recent value changes into one view. Use it when you want to work with signals instead of jumping between raw PDO, Watch, Startup, and Free Run tables. It is not a host-diagnostics page; Linux service, driver, DKMS, blacklist, and device-node checks remain in Diagnostics.</p>
<table>
<tr><th>Source</th><th>How It Is Used</th></tr>
<tr><td>Free Run process image</td><td>Provides live raw/decoded values, direction, change highlighting, map status, and CiA 402 meaning when cyclic telemetry is available.</td></tr>
<tr><td>PDO Map</td><td>Backfills signal names, index/subindex, direction, bit length, and PDO mapping evidence when Free Run data is not yet present or lacks metadata.</td></tr>
<tr><td>Watch</td><td>Adds last SDO value, decoded meaning, changed status, baseline drift, missing-value evidence, and CiA 402 drive interpretation.</td></tr>
<tr><td>Startup SDO</td><td>Shows expected startup values and flags objects whose live or watched values differ from the configured startup value.</td></tr>
</table>
<p>The Scope selector turns the table into focused review modes: <b>All</b>, <b>Selected Slave</b>, <b>Process Image</b>, <b>PDO Only</b>, <b>Watch Evidence</b>, <b>Startup Diff</b>, <b>Missing Value</b>, <b>Rx Outputs</b>, <b>Tx Inputs</b>, <b>CiA 402</b>, and <b>Changed</b>. The summary reports the current visible set so the table can be used as a checklist before state transitions, Free Run, or Startup SDO application. Selecting a row updates a local detail strip with the signal name or alias, object address, direction, source, Raw/Decoded/Meaning, Watch value, Startup comparison, PDO map status, changed flag, PLC quality, tags, note, and operation boundary. Scope changes, filtering, selection, detail review, declaration copy, visible-row export, and Alias/Tags/Note edits use only the current table and project evidence.</p>
<p>Select or double-click a row to fill the SDO fields with slave, index, subindex, and inferred type. <b>Fill SDO</b> only prepares the target; <b>Read SDO</b> and double-click explicitly issue the normal SDO read for that object. <b>Watch Selected</b> and <b>Watch Visible</b> create or reuse Watch rows without immediate reads, so large signal sets do not unexpectedly flood mailbox traffic. <b>Startup Selected</b> and <b>Startup Visible</b> create or update Startup SDO candidates from I/O variable evidence, preferring Watch values and falling back to Raw values without reading or writing the bus. Refresh Watch when you are ready to poll those objects, and use Startup Apply only after the normal confirmation review.</p>
<p>Use <b>Alias</b> or the row context menu to assign a project-local engineering name, comma-separated tags, and a note to any variable. These fields are keyed by slave position, index, and subindex, are saved in the <code>.ecatproj</code> file, participate in filtering, and do not write anything to the bus. <b>Bulk Name</b> generates aliases and tags for selected or visible rows in one pass, defaults to preserving existing aliases, and asks for confirmation before replacing them. Use this to turn raw PDO names into PLC-ready engineering names before handoff.</p>
<p>Use <b>Export CSV</b> or the Project menu to export the currently visible I/O Variables table for commissioning records. Use <b>PLC CSV</b> or <b>Export PLC Symbols CSV</b> to export a normalized symbol-planning file for visible rows: Symbol, Direction, IEC-style Type, Slave, Index, SubIndex, PDO, source evidence, value evidence, PLC Quality, Alias, Tags, Note, and object address. Use <b>Export PLC Declarations ST</b> to write the same visible rows as an IEC-style <code>VAR_GLOBAL</code> block into a <code>.st</code> file for PLC handoff. Before PLC CSV or ST export is saved, NekoEcat Studio checks the visible export scope for handoff quality issues and offers <b>Review Issues</b>, <b>Continue Export</b>, or cancel. From the Command Palette or I/O Variables context menu, <b>Copy Selected PLC Declarations</b> and <b>Copy Visible PLC Declarations</b> copy the same declaration block to the clipboard using the same symbol, type, address, direction, and quality evidence; copying runs the same quality gate and offers <b>Continue Copy</b> when unresolved issues remain. Review opens I/O Variables on the PLC Issues scope and focuses the relevant rows; continuing keeps Quality evidence attached to the handoff output for downstream cleanup. Exports and declaration copies derive from the current evidence table only; they do not read the bus, write SDOs, change Free Run, or run host diagnostics.</p>
<p>The <b>PLC</b> quality column marks whether each signal is ready for PLC handoff or still has missing alias, generated-name, missing-tag, or duplicate-symbol issues. Use <b>Review PLC</b>, <b>Review PLC Handoff Issues</b> from the Project menu, the <b>PLC Issues</b> scope, or the Command Palette to review only those rows before exporting or copying declarations. The table highlights Startup mismatches, changed values, missing map evidence, direction or bit-length mismatch, PLC quality, Alias rows, tags, and CiA 402 drive objects. Right-click actions support copying the object address/value, copying PLC declarations, filling or reading SDO, setting or clearing Alias/Tags, bulk naming visible or selected rows, reviewing PLC handoff issues, exporting visible engineering rows, exporting PLC symbol rows, and adding selected or visible signals to Watch. This makes I/O Variables the main bridge between process data, SDO operations, startup configuration, PLC handoff, and the State Machine evidence model.</p>

<h2 id="consistency">15. Consistency Check</h2>
<p>The <b>Consistency</b> tab is a read-only online/offline engineering review surface. It compares evidence that is already loaded in the current session and project: topology baseline versus online slaves, Startup SDO expectations versus Watch and I/O variable evidence, I/O Variables map/value completeness, stale Alias/Tags metadata, and missing engineering aliases. It does not issue bus reads, bus writes, state requests, Free Run changes, or host checks.</p>
<table>
<tr><th>Scope</th><th>What It Checks</th></tr>
<tr><td>Topology</td><td>Captured topology baseline against current online slave positions, names, and states.</td></tr>
<tr><td>Startup</td><td>Startup SDO rows with invalid addresses, missing Watch comparison, or expected values that differ from Watch evidence.</td></tr>
<tr><td>I/O Variables</td><td>Missing process values, incomplete PDO/process-map evidence, stale project metadata, and variables that still need Alias/Tags for handoff.</td></tr>
</table>
<p>Click <b>Refresh Check</b> to rebuild the table from current evidence. The scope selector can focus <b>All</b>, <b>Errors</b>, <b>Warnings</b>, <b>Topology</b>, <b>Startup</b>, <b>I/O Variables</b>, or <b>Ready</b> rows. The columns are Level, Scope, Target, Evidence, Expected, Actual, and Action. Selecting a row updates a local detail strip with the severity, scope, target, evidence, expected/actual values, recommended action, best evidence route, and read-only gate boundary. <b>Open Evidence</b>, row double-click, the row context menu, and the Command Palette navigate to the best loaded evidence table without bus access: topology rows open State Machine, Startup rows open the matching Startup SDO row, Watch evidence opens the matching Watch row, and remaining I/O issues open I/O Variables with the relevant scope such as PLC Issues, Startup Diff, Missing Value, or PDO evidence.</p>
<p>Use Consistency before applying Startup SDO, requesting OP, exporting handoff records, or comparing a saved project with the current bus. Overview's Run Next and the status-bar Next Best Action route through this read-only gate after Startup/Watch evidence is clean and before Free Run process-image validation. If the gate already has Error or Warning rows, Next Best Action can jump directly into the first blocking row's evidence target instead of leaving you on the summary table. If Watch, Startup, PDO, Free Run, topology, or I/O variable metadata changes, the previous Consistency result is treated as stale and must be refreshed. The Command Palette can open the gate and can also open the selected row's evidence target. Exported diagnostics reports include the Consistency Check table. Host diagnostics remain in the Diagnostics tab only.</p>

<h2 id="diagnostics">16. Diagnostics and Host Health</h2>
<p>The Diagnostics tab is the only place for host environment checks and diagnostic review. It contains Host Health and Event Stream. Host Health checks kernel modules, EtherCAT configuration, NIC state, firmware blacklist hints, DKMS driver status, runtime service availability, device node permissions, and common repair commands. Event Stream records user actions, refreshes, state requests, Watch operations, Startup SDO results, explicit Command Palette activations, and runtime errors.</p>
<p>Select a Host Health row and use <b>Copy Command</b> to copy the suggested repair command. Use <b>Export Diagnostics Report</b> from the Project menu to write a Markdown report with host health, diagnostics events, current slaves, topology baseline, Watch, Startup SDO, SDO history, Object Dictionary evidence rows, Free Run tables, I/O Variables, Consistency Check, project notes, and raw command snapshots.</p>

<h2 id="esi">17. ESI Repository and XML</h2>
<p>Use <b>Tools / Import ESI XML</b> to add EtherCAT Slave Information files to the local repository. The ESI Repository tab lists file, vendor, product code, revision, type, name, and path. The ESI XML tab shows the currently loaded XML text for inspection.</p>
<p>When a slave name or object description is incomplete online, importing the correct ESI file helps engineering review and documentation.</p>

<h2 id="project">18. Projects, Notes, and Reports</h2>
<p>Project files preserve the working context: notes, Watch rows, Startup SDO rows, Object Bookmarks, I/O variable aliases/tags/notes, selected master/slave context, raw online snapshots, and Object Dictionary evidence snapshots including Last Value, Last Status, detail, and time. Host Health remains a Diagnostics workspace concern and is not moved into Overview. Use <b>New Project</b>, <b>Open Project</b>, <b>Save Project</b>, <b>Save Project As</b>, <b>Export I/O Variables CSV</b>, <b>Export PLC Symbols CSV</b>, <b>Export PLC Declarations ST</b>, and <b>Export Diagnostics Report</b> from the Project menu.</p>
<p>The Notes tab is intended for commissioning records: device serials, cabling notes, parameter decisions, observed faults, and test procedures. Exported diagnostics reports are useful before driver changes, kernel updates, or field troubleshooting.</p>

<h2 id="shortcuts">19. Shortcuts and Fast Operations</h2>
<table>
<tr><th>Action</th><th>Shortcut / Entry</th></tr>
<tr><td>Command Palette</td><td><code>Ctrl+P</code>; inside palette: <code>Alt+A/L/O/D/H/F</code> filters All, Local, Online, Danger, Host, File; <code>Alt+P</code> pins or unpins the selected command</td></tr>
<tr><td>Workspace switcher</td><td><code>Ctrl+Alt+1</code> Overview, <code>Ctrl+Alt+2</code> Object Dictionary, <code>Ctrl+Alt+3</code> PDO Map, <code>Ctrl+Alt+4</code> Watch, <code>Ctrl+Alt+5</code> Startup SDO, <code>Ctrl+Alt+6</code> Free Run, <code>Ctrl+Alt+7</code> I/O Variables, <code>Ctrl+Alt+8</code> Consistency, <code>Ctrl+Alt+9</code> State Machine, <code>Ctrl+Alt+0</code> Diagnostics</td></tr>
<tr><td>Workspace history</td><td><code>Alt+Left</code> Back, <code>Alt+Right</code> Forward</td></tr>
<tr><td>Local evidence action</td><td><code>Alt+Enter</code> on Commissioning Workflow, Session Brief, Slave Evidence Matrix, Consistency, Selected Object rows, Object Dictionary, PDO Map, Watch, Free Run entries, I/O Variables, SDO History, Startup SDO, Object Bookmarks, or SDO Target Trail. It copies workflow step evidence, opens local evidence, copies Session Brief or matrix row evidence, routes a matrix slave to the best loaded evidence table, reviews/copies Selected Object evidence, or fills the SDO target only; it does not read the bus, write SDOs, change state, toggle Free Run, or run Host Health.</td></tr>
<tr><td>Connect Runtime</td><td><code>Ctrl+K</code></td></tr>
<tr><td>Refresh</td><td>Standard refresh key</td></tr>
<tr><td>Rescan Bus</td><td><code>Ctrl+Shift+R</code></td></tr>
<tr><td>Toggle Free Run</td><td><code>Ctrl+F</code></td></tr>
<tr><td>Selected slave INIT/PREOP/SAFEOP/OP</td><td><code>Alt+1</code>, <code>Alt+2</code>, <code>Alt+3</code>, <code>Alt+4</code></td></tr>
<tr><td>I/O Variables</td><td>Command palette or tab</td></tr>
<tr><td>Consistency Check</td><td>Command palette or tab</td></tr>
<tr><td>Export I/O Variables CSV</td><td>Project menu, I/O Variables tab, or command palette</td></tr>
<tr><td>Export PLC Symbols CSV</td><td>Project menu, I/O Variables tab, command palette, or context menu</td></tr>
<tr><td>Export PLC Declarations ST</td><td>Project menu, command palette, or I/O Variables context menu</td></tr>
<tr><td>Next Best Action</td><td>Status bar button</td></tr>
<tr><td>User Manual</td><td>Help menu or command palette</td></tr>
</table>
<p>The status bar <b>Boundary</b> pill changes with the active workspace: Local Gate, Engineering, Online PDO, Watch Reads, SDO, Process Data, Startup Danger, State Danger, Host, File/ESI, Project, or Raw Evidence. It is an always-visible safety label, not an action button; hover it to see which operations are local, which ones use online SDO/PDO/process-data access, and which ones keep dangerous confirmation flows.</p>
<p><b>Workspace shortcuts</b>, workspace history, <b>Alt+Enter</b> local evidence actions, and Command Palette <b>Go to Workspace</b> entries only activate pages, copy workflow step evidence, open loaded evidence, copy Session Brief or matrix row evidence, route a matrix slave to the best loaded evidence table, review/copy Selected Object evidence, or fill the SDO target from the selected table row. They do not read the bus, write SDOs, change slave state, toggle Free Run, or run Host Health checks. Use action commands such as Load OD, Refresh Watch, Toggle Free Run, or Run Host Check when you intentionally want online activity. Navigation follows actual workspace pages even after tabs are reordered.</p>
<p><b>Next Best Action</b> changes with the current session: Connect, Rescan, Select Slave, Load OD, Load PDO, Add Watch, Review Startup Diffs, Run Consistency, open blocking Consistency evidence, Free Run, Review Diagnostics, or Commands. Its status-bar button uses semantic color: blue for normal next actions, amber for evidence review, red for diagnostics errors, green when ready, and neutral gray for the command palette. High-frequency tab titles also carry local badges: row counts for active evidence and <b>!</b> counts for Watch/Startup/Consistency/I/O/State/Diagnostics risks, with tooltips explaining the counts. These badges are computed from loaded tables only and do not read the bus. Next Best Action does not display host checks in Overview; when diagnostics need attention it navigates to the Diagnostics workspace. When Consistency already has blocking rows, it opens the first row's best evidence target instead of leaving you on a summary-only table.</p>
<p>The Command Palette includes stable <b>Go to Workspace</b> entries, plus contextual current-SDO actions: run the next commissioning workflow step, switch workflow scopes, review first/next workflow issues, copy the selected workflow step evidence, open or copy the selected Session Brief evidence row, open or copy the selected Slave Evidence Matrix row, filter the Object Dictionary by semantic groups, add visible Object Dictionary results to Watch, capture or clear Watch baselines, read, write, use read value, use best evidence, pick a specific evidence value, review Write Delta evidence, copy the current SDO evidence digest, open or copy the selected <b>Selected Object</b> row evidence, open matching Watch/Startup/Bookmark/Target Trail evidence, restore/reuse Target Trail rows, add to Watch, add the CiA 402 Watch preset, write common or recommended CiA 402 controlwords, add to Startup SDO, sync selected Watch values to Startup SDO, review only Startup SDO rows that differ from Watch values, apply only those Watch-diff rows, open I/O Variables, switch I/O Variables scopes, edit or bulk-name I/O variable aliases, review PLC handoff issues, copy selected or visible PLC declarations, export I/O Variables CSV, export PLC Symbols CSV, export PLC Declarations ST, add selected or visible I/O Variables to Watch, open Consistency Check, open selected Consistency evidence, copy address, and copy object/value. The palette marks each command as <b>Local</b>, <b>Online</b>, <b>Danger</b>, <b>Host</b>, or <b>File</b> and can filter by that action type, so navigation and project-table commands are visually separated from runtime reads, host checks, SDO writes, and state changes. Use <code>Alt+A</code>, <code>Alt+L</code>, <code>Alt+O</code>, <code>Alt+D</code>, <code>Alt+H</code>, and <code>Alt+F</code> inside the palette to switch All, Local, Online, Danger, Host, and File filters without leaving the keyboard. Use <code>Alt+P</code> or the row context menu to pin high-frequency commands above recent commands; pinned commands are stored in local settings and never auto-run. Commands explicitly activated from the palette are remembered and shown with a <b>Recent</b> marker on later palette opens; this only changes ordering and never auto-runs a command. The result summary shows how many matching commands are executable, pinned, recent, and visible by Local/Online/Danger/Host/File type. The selected command also shows a fixed preview panel with the action type, boundary, command description, pinned/recent markers, and disabled state when applicable. Each explicit palette activation writes a <b>Command Palette</b> Event Stream row with the action type; dangerous activations are logged as warnings and still use the existing confirmation paths before any write or state change can proceed. Most online object tables also support <code>Alt+Enter</code> for the local fill/open-evidence path, plus right-click actions for copying rows, copying an object address such as <code>#3 0x6040:0x00</code>, copying an object value such as <code>#3 0x6040:0x00 uint16 = 0x0006</code>, filling SDO fields, reading objects, bookmarking objects, or adding items to Watch. The Selected Object panel has its own right-click menu for row evidence, row copy, full digest copy, delta review, and autosizing, all without bus access.</p>

<h2 id="trouble">20. Troubleshooting</h2>
<h3>Runtime cannot connect</h3>
<ul>
<li>Open Diagnostics and run Host Check.</li>
<li>Confirm <code>ecatd</code> is running and listening on <code>127.0.0.1:5877</code>.</li>
<li>Check runtime log for startup errors and permission failures.</li>
</ul>
<h3>No slaves after reboot</h3>
<ul>
<li>Check the EtherCAT service status and master configuration.</li>
<li>Confirm the selected NIC is bound to the EtherCAT master and not used by NetworkManager.</li>
<li>For USB Ethernet adapters, verify whether the firmware/module is blacklisted and whether the expected <code>r8152</code>/<code>rt8152</code> DKMS driver is loaded.</li>
<li>Check cable, slave power, link LEDs, and bus order before changing software configuration.</li>
</ul>
<h3>SDO read or write fails</h3>
<ul>
<li>Confirm the slave is in a state that allows mailbox access, usually PREOP or SAFEOP.</li>
<li>Verify object index, subindex, data type, and access rights.</li>
<li>Check whether the selected object exists in the current device revision.</li>
</ul>
<h3>OP transition fails</h3>
<ul>
<li>Inspect the event stream and raw slave output.</li>
<li>Check PDO mapping, Sync Manager configuration, distributed clock requirements, watchdog settings, and device-specific enable prerequisites.</li>
<li>Try PREOP to SAFEOP first, then SAFEOP to OP after resolving reported errors.</li>
</ul>

<h2 id="safety">21. Safety Checklist</h2>
<ul>
<li>Always confirm the selected master and slave before state changes or writes.</li>
<li>Use read-back after SDO writes.</li>
<li>Do not apply Startup SDO rows after a topology change without review.</li>
<li>Do not enable Free Run outputs on actuators unless the machine is in a safe condition.</li>
<li>Keep diagnostics reports before changing kernel modules, DKMS drivers, or EtherCAT service configuration.</li>
</ul>
</body>
</html>
)HTML");

  const QString chinese = QStringLiteral(R"HTML(
<!doctype html>
<html>
<head>
<style>
body { font-family: "Inter", "Microsoft YaHei UI", "Noto Sans CJK SC", "Segoe UI", sans-serif; font-size: 14px; line-height: 1.72; color: #172033; }
h1 { font-size: 26px; margin: 0 0 8px 0; color: #0f172a; }
h2 { font-size: 20px; margin: 26px 0 8px 0; color: #12376f; }
h3 { font-size: 16px; margin: 18px 0 6px 0; color: #1f2937; }
p { margin: 7px 0; }
ul, ol { margin-top: 6px; margin-bottom: 10px; padding-left: 22px; }
li { margin: 4px 0; }
a { color: #2563eb; text-decoration: none; }
.toc { background: #f7f9fc; border: 1px solid #d9e1ec; border-radius: 8px; padding: 12px 14px; }
.note { background: #eef4fb; border-left: 4px solid #2563eb; padding: 9px 12px; margin: 10px 0; }
.warn { background: #fff7ed; border-left: 4px solid #f97316; padding: 9px 12px; margin: 10px 0; }
.danger { background: #fef2f2; border-left: 4px solid #ef4444; padding: 9px 12px; margin: 10px 0; }
code { font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace; background: #eef2f7; border-radius: 5px; padding: 1px 5px; }
table { border-collapse: collapse; width: 100%; margin: 10px 0 16px 0; }
th, td { border: 1px solid #d9e1ec; padding: 7px 9px; vertical-align: top; }
th { background: #f0f4f9; color: #475569; }
</style>
</head>
<body>
<h1>NekoEcat Studio 使用说明书</h1>
<p>NekoEcat Studio 是一个面向 Linux + IgH EtherCAT Master 的现代 EtherCAT 工程工作站。它把工程文件、主站选择、拓扑扫描、对象字典、SDO 读写、PDO 映射、Watch 监视、Startup SDO、Free Run 周期遥测、I/O 变量工程表、主机健康检查、ESI 仓库和运行日志组织在一个界面内，目标是在调试效率和现场排障能力上对齐并逐步超越传统工程软件。总览页只负责总线和当前从站上下文；主机环境检查、修复建议和诊断证据统一放在诊断页。</p>
<div class="toc" id="contents">
<b>目录</b>
<ol>
<li><a href="#concept">软件定位与运行架构</a></li>
<li><a href="#first-run">第一次启动与运行时连接</a></li>
<li><a href="#prerequisites">主机前置条件</a></li>
<li><a href="#masters">多主站配置与切换</a></li>
<li><a href="#topology">拓扑树与从站选择</a></li>
<li><a href="#overview">总览页</a></li>
<li><a href="#state-machine">EtherCAT 状态模型</a></li>
<li><a href="#od">对象字典与 SDO 读写</a></li>
<li><a href="#sdo-history">SDO 历史与写入校验</a></li>
<li><a href="#pdo">PDO 映射页</a></li>
<li><a href="#watch">Watch 监视页</a></li>
<li><a href="#startup">Startup SDO 启动参数页</a></li>
<li><a href="#freerun">Free Run 自由运行页</a></li>
<li><a href="#io-variables">I/O 变量页</a></li>
<li><a href="#consistency">一致性检查页</a></li>
<li><a href="#diagnostics">诊断与主机健康检查</a></li>
<li><a href="#esi">ESI 仓库与 XML 查看</a></li>
<li><a href="#project">工程文件、备注与诊断报告</a></li>
<li><a href="#shortcuts">快捷键、右键菜单与命令面板</a></li>
<li><a href="#workflow">推荐调试流程</a></li>
<li><a href="#trouble">常见故障排查</a></li>
<li><a href="#safety">安全操作清单</a></li>
</ol>
</div>

<h2 id="concept">1. 软件定位与运行架构</h2>
<p>本软件由两个部分组成：图形工作站和 <code>ecatd</code> 运行时守护进程。图形工作站负责工程组织、界面交互、过滤、表格、命令面板和说明书；<code>ecatd</code> 负责和 IgH EtherCAT Master 交互，并把主站、从站、SDO、PDO、Free Run、诊断等能力通过本地 TCP 接口暴露给界面。</p>
<table>
<tr><th>层级</th><th>职责</th><th>你在界面中看到的结果</th></tr>
<tr><td>GUI 工作站</td><td>工程管理、主站选择、拓扑树、选项卡、筛选器、命令面板、对话框、运行日志展示。</td><td>菜单、工具栏、左侧拓扑树、右侧功能选项卡、底部 Runtime Log。</td></tr>
<tr><td><code>ecatd</code> 运行时</td><td>监听 <code>127.0.0.1:5877</code>，执行 IgH CLI 命令，维护主站目标，处理 SDO/PDO/状态切换/主机检查/Free Run。</td><td>连接状态、刷新结果、诊断事件、Free Run 遥测。</td></tr>
<tr><td>IgH EtherCAT Master</td><td>内核模块和用户态工具，完成从站扫描、状态机、邮箱访问、PDO/SDO 信息读取、周期数据交换。</td><td>Master Raw、Slave Raw、PDO Raw、SDO Raw、对象字典和 PDO 映射。</td></tr>
</table>
<p>当前版本：<code>%1</code>。默认通信端点：<code>127.0.0.1:5877</code>。当前主站：<code>%2</code>。运行时路径：<code>%3</code>。</p>
<div class="note">界面显示“已连接”表示 GUI 已连接到 <code>ecatd</code>，不等于总线完全正常。总线是否正常还需要看主站状态、从站数量、链路、从站状态和诊断页。</div>

<h2 id="first-run">2. 第一次启动与运行时连接</h2>
<ol>
<li>启动 NekoEcat Studio 后，软件会尝试启动或连接同目录的 <code>ecatd</code>。</li>
<li>观察顶部连接状态。如果处于等待或断开状态，点击工具栏 <b>连接</b>。</li>
<li>连接成功后点击 <b>刷新</b>。刷新会读取主站摘要、从站列表、对象字典、PDO、ESI/原始输出和部分统计信息。</li>
<li>如果刚插拔网线、给从站上电、切换主站网卡、重启 EtherCAT 服务，点击 <b>重新扫描</b>，再点击 <b>刷新</b>。</li>
<li>左侧拓扑树出现从站后，单击一个从站作为当前操作目标。右侧大部分 SDO、状态切换、Watch、Startup SDO 和 Free Run 操作都会以当前从站为上下文。</li>
</ol>
<div class="warn">如果连接成功但从站数量为 0，通常不是 GUI 问题。优先检查 EtherCAT 服务、主站配置、网卡绑定、从站供电、链路灯和网线顺序。</div>

<h2 id="prerequisites">3. 主机前置条件</h2>
<p>开始调试硬件前，先确认 Linux 主机具备可用的 IgH EtherCAT Master、正确的主站网卡、预期的网卡驱动、可访问的设备节点，以及当前用户权限。相关检查统一在 <b>诊断</b> 页完成。</p>
<table>
<tr><th>前置项</th><th>需要确认的内容</th></tr>
<tr><td>EtherCAT 服务</td><td>系统重启后主站服务能正常启动，并暴露预期的 master index。</td></tr>
<tr><td>网卡接口</td><td>连接 EtherCAT 总线的网卡就是主站配置中的网卡，且没有被普通 IP 网络服务占用。</td></tr>
<tr><td>USB 以太网卡</td><td>Realtek USB 网卡应使用预期的 <code>r8152</code>/<code>rt8152</code> 驱动路径，必要时包括 DKMS 驱动。</td></tr>
<tr><td>黑名单与固件</td><td>重启后需要的模块和 firmware 没有被 blacklist 配置屏蔽。</td></tr>
<tr><td>设备节点</td><td><code>/dev/EtherCAT0</code> 存在，并且当前用户或用户组有合适访问权限。</td></tr>
</table>

<h2 id="masters">4. 多主站配置与切换</h2>
<p>进入 <b>工具 / 设置</b> 可以配置语言、主题、缩放和主站列表。每个主站配置包含显示名称和运行目标，例如主站索引 <code>0</code>。工具栏的主站下拉框用于选择当前活跃主站。</p>
<p>切换主站时，软件会清空在线视图并把后续命令发送到新的目标。这可以避免你在 A 总线上选中的从站信息残留到 B 总线，导致误读、误写或错误状态切换。</p>
<p>建议命名规则：使用“设备用途 + 网卡/工位”的形式，例如 <code>Servo Bench [0]</code>、<code>IO Rack [1]</code>。如果现场有多个 USB/PCIe 网卡，主站名应包含物理位置或线缆标签。</p>

<h2 id="topology">5. 拓扑树与从站选择</h2>
<p>左侧拓扑树是在线操作的主入口。选中从站后，顶部 Selected Slave 会更新，右侧对象字典、PDO、Watch、Startup SDO 和状态切换按钮都以该从站为目标。</p>
<p>Selected Slave 面板会显示当前从站的证据完整度，汇总身份、OD、PDO、Watch 有值项、Startup SDO 偏差、Free Run 过程映像、PDO 映射问题和拓扑基线问题。这样在写入、切状态或做输出相关操作前，可以直接看到当前从站还缺哪些证据。</p>
<p>Selected Slave 面板中的 <b>准备快照</b> 用于为当前从站建立只读证据包：刷新身份信息、对象字典、PDO 映射、ESI XML，并加入/刷新 CiA 402 Watch 证据。它不会请求状态切换，也不会写 SDO 或过程输出。</p>
<p>右键拓扑树通常用于快速复制、定位或触发和该从站相关的操作。使用任何写入类操作之前，先确认拓扑树中的 position、名称、Vendor/Product/Revision 和实际设备一致。</p>
<p>推荐的基本顺序是：先选从站，再看总览页确认身份和状态，再进入对象字典或 PDO 映射，最后才执行写 SDO、Startup SDO、OP 切换或 Free Run。</p>

<h2 id="overview">6. 总览页</h2>
<p>总览页是默认第一个选项卡，也是最高频的总线/从站概览页面。它把主站和当前从站的关键状态集中展示，适合每次刷新后快速判断当前总线和选中从站是否处于预期状态。总览页不承担主机诊断；主机环境检查、修复命令和诊断证据统一在 <b>诊断</b> 页处理。</p>
<p><b>会话简报</b> 是只读决策层，只使用已经加载到界面里的证据。它会汇总当前目标、一致性门禁、OD/PDO 映射就绪度、当前 SDO 的本地证据组和待写值差异、Watch/Startup/Free Run 运行证据，以及下一条工作流动作。<b>复制本行</b>、右键 <b>复制本行证据</b> 和命令面板 <b>复制会话简报本行证据</b> 会把当前行的状态、依据、下一步、本地路由、当前下一最佳动作、行提示详情和本地边界复制到剪贴板，便于交接。双击简报行、按 Enter、在右键菜单中使用 <b>打开本地证据</b>，或在命令面板执行 <b>打开会话简报证据</b>，都会打开匹配的本地证据界面，例如拓扑树、一致性表、对象字典、Watch、Startup SDO、Free Run 或下一条工作流行。行复制和本地证据导航都不会读取总线、写 SDO、切换状态、启动 Free Run 或运行 Host Health。</p>
<p><b>从站证据矩阵</b> 会把每个扫描到的从站放在同一张表里横向比较：优先级、当前状态、OD/PDO 就绪度、Watch 有值数量、Startup 偏差、过程映像行、PDO 映射问题、风险和下一步证据动作都能直接看到。矩阵会自动按调试队列排序：P0 故障、P1 风险、P2 待执行、P3 就绪；Overview 选项卡徽标、边界提示、矩阵摘要和 P0/P1/P2/P3 快速 triage 按钮会显示实时优先级数量，强优先级在线前置条件处理完后，<b>下一最佳动作</b> 也可以直接路由到优先级最高的矩阵问题。优先级按钮或范围下拉框可以聚焦全部、P0 故障、P1 风险、P2 待执行、P3 就绪、风险、待执行、就绪、缺 OD、缺 PDO、缺 Watch、Startup 偏差或缺过程证据行；搜索框可以在大型总线里按从站、优先级、状态、风险或下一步动作快速定位。<b>审阅首个问题</b> 会打开当前可见列表里优先级最高的风险或待执行行，<b>审阅下个问题</b> 会从当前矩阵行继续打开下一个可见问题并在末尾回绕，让过滤后的列表变成可连续处理的问题清单。<b>复制本行</b>、右键 <b>复制矩阵本行证据</b> 和命令面板 <b>复制从站矩阵本行证据</b> 会把当前行的优先级、就绪度、风险、下一步、详细提示证据、当前过滤范围和本地边界复制到剪贴板，便于现场交接或问题报告。双击矩阵行、按 <code>Alt+Enter</code>、在右键菜单选择 <b>打开矩阵证据</b>、<b>审阅首个矩阵问题</b>、<b>审阅下个矩阵问题</b> 或 <b>复制矩阵本行证据</b>，或在命令面板执行 <b>打开从站矩阵证据</b>、<b>审阅首个从站矩阵问题</b>、<b>审阅下个从站矩阵问题</b> 或 <b>复制从站矩阵本行证据</b>，都会本地选中该从站、打开最相关的已加载证据表，或复制本行摘要。过滤、导航和本行复制都不会调用运行时、不会加载 OD/PDO/ESI、不会切状态、不会启动 Free Run，也不会运行 Host Health。</p>
<p>调试工作流不是静态提示表，而是工程流程看板：每一行都按阶段组织，并同时显示状态、步骤、风险、依据和下一步动作，让拓扑、OD、PDO、Watch、Startup、一致性和过程映像证据缺口在高风险操作前就能被看见。选中某一行后，工作流详情条会显示行号、阶段、状态、动作边界、风险、依据和下一步；悬停可看到显式执行会停留在本地、连接/重扫在线、加载 OD/PDO、加入本地 Watch 行、打开一致性证据，还是切换 Free Run 遥测。范围选择器可以聚焦 <b>全部</b>、<b>未完成</b>、<b>受阻</b>、<b>待执行</b>、<b>就绪</b>、<b>风险</b> 或 <b>证据缺口</b> 步骤，搜索框可以按阶段、状态、步骤、风险、依据或下一步动作查找；同样的工作流范围也可以从命令面板切换，适合键盘驱动复核。<b>审阅首个</b> 会选择当前可见的首个未就绪工作流问题，<b>审阅下个</b> 会继续选择下一个可见问题并在末尾回绕；右键菜单和命令面板也提供同样的审阅动作。<b>执行下一步</b> 会自动运行当前第一条可推进步骤，双击某一行则执行或打开该行对应动作。<b>复制步骤</b>、右键 <b>复制步骤证据</b>、<code>Alt+Enter</code> 或命令面板 <b>复制工作流步骤证据</b> 会把当前步骤的阶段、状态、风险、依据、下一步、就绪度摘要、提示详情、当前下一最佳动作和本地边界复制到剪贴板，便于交接或执行前复核。工作流会把一致性检查作为 Free Run 和后续状态推进前的只读门禁；Watch、Startup、PDO、Free Run、拓扑或 I/O 变量元数据变化后，旧的一致性结果会被视为过期，必须重新刷新才算通过。状态栏还提供 <b>下一最佳动作</b> 全局入口，会跟随调试状态一键执行连接、重扫、选择从站、加载 OD/PDO、加入 Watch、运行或审阅一致性、启动 Free Run，或在已有错误需要复核时跳转到诊断页。工作流选择、过滤、问题审阅、详情复核和步骤复制都是本地界面/剪贴板操作，不读取总线、不加载 OD/PDO/ESI、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。这样总览页可以承担高频操作引导，但仍不混入主机健康检查。</p>
<table>
<tr><th>区域</th><th>用途</th><th>异常时的判断</th></tr>
<tr><td>主站指标卡</td><td>显示连接、主站状态、从站数量、链路、丢包和 Free Run。</td><td>从站数为 0 先看链路、从站供电、线缆顺序和主站配置；主机环境问题进入诊断页。</td></tr>
<tr><td>Identity</td><td>查看 Vendor ID、Product Code、Revision、Serial、Name、Type。</td><td>型号或 Revision 不匹配时，先确认 ESI 和实际硬件版本。</td></tr>
<tr><td>Ports</td><td>查看端口链路、拓扑方向和物理连接提示。</td><td>端口链路异常优先检查网线、供电、从站顺序和端接。</td></tr>
<tr><td>Mailbox</td><td>查看 CoE/SoE/FoE/EoE 等邮箱能力。</td><td>SDO 失败时确认设备是否支持 CoE，且状态允许邮箱访问。</td></tr>
</table>
<p>调试工作流卡片是可操作的：选中某一行会先刷新详情条，显示当前步骤的风险、依据和执行边界；<b>执行下一步</b> 会执行第一条能推进会话的工作流步骤，双击某一行则执行或打开该行建议动作；<b>复制步骤</b>、右键菜单、<code>Alt+Enter</code> 和命令面板则复制该步骤证据摘要，不访问总线。就绪度胶囊会显示整体调试就绪百分比、下一步动作和未完成项数量；悬停后可查看每条未完成步骤所属阶段、状态、风险、依据和建议动作。Selected Slave 卡片还会汇总当前从站 Watch 行中的 CiA 402 驱动证据。如果 Watch 中包含状态字、模式显示、错误码或控制字，Drive 行会显示解析后的驱动状态和关键标志；这仍然是从站上下文信息，不会把主机诊断放入总览页。Drive 行会按 neutral/action/warning/error/ok 变色，让 Fault、Quick stop、warning 等证据更醒目。当状态字能推导出明确的下一步控制字时，<b>驱动下一步</b> 按钮会启用，并继续走普通 SDO 写入确认流程。如果对象字典中存在失败 SDO 证据，总览工作流和全局 <b>下一最佳动作</b> 会直接跳到失败 OD 证据过滤，并选中第一条失败对象。</p>

<h2 id="state-machine">7. EtherCAT 状态模型</h2>
<p><b>状态机</b> 页是专用的 EtherCAT 从站状态工作区，位于 Free Run 之后、诊断页之前。它只处理从站状态、工程证据和切换风险，不承担主机环境检查。选中某一行会刷新本地状态切换详情条，显示从站、当前状态、推荐状态、证据、驱动状态、Startup 证据、PDO/过程证据、风险和确认边界。选择和预览不会读取总线，也不会请求状态切换；<b>发送推荐状态</b>、PREOP、SAFEOP、OP、全部从站按钮和双击行仍然是显式请求路径，并继续走常规确认对话框。Host Health 仍然只在诊断页。</p>
<p>状态矩阵会按从站列出当前状态、推荐下一状态、身份/OD/Watch 证据、CiA 402 驱动解析证据、Startup SDO 偏差、PDO/Free Run 过程证据、风险提示和下一步动作。推荐逻辑是保守的：INIT 可以推荐 PREOP；PREOP 只有在 PDO 和 Watch 证据存在时才推荐 SAFEOP；SAFEOP 只有在已有过程映像证据、Startup/PDO 映射风险清理且 stale-aware 一致性门禁通过后才推荐 OP。证据不足或一致性过期时推荐状态为空，行内动作会提示复核证据，而不是制造一个看似可执行但依据不足的状态请求。</p>
<p>点击 <b>发送推荐状态</b> 或双击状态矩阵行，会发送该行的推荐状态。PREOP、SAFEOP、OP 行级按钮用于显式请求当前矩阵行的状态。<b>全部 PREOP</b> 和 <b>全部 SAFEOP</b> 会对所有检测到的从站请求状态。所有路径都会复用原有安全确认流程。</p>
<p>状态按钮作用于当前选中从站，Online 菜单和状态机页中的全部状态切换作用于所有从站。状态切换是真实在线操作，不是只改变界面显示。</p>
<p>状态切换确认是证据驱动并按风险分组的。确认审阅会把 <b>关键影响</b>、<b>确认前复核</b>、<b>证据</b> 和 <b>目标上下文</b> 分开显示，让输出、驱动、一致性过期、拓扑和缺失证据等高影响项先被看到。单从站切换会显示当前状态、目标状态、当前从站证据完整度、可用的 CiA 402 Watch 证据、Startup SDO 偏差、Free Run PDO 映射问题、拓扑基线警告和一致性门禁状态。切到 OP 或 SAFEOP 时，如果缺少 PDO、Watch、过程映像、Startup 证据，或一致性未运行/已过期/存在 Error/Warning，确认框会在发送请求前直接标出。</p>
<table>
<tr><th>状态</th><th>典型用途</th><th>常见注意事项</th></tr>
<tr><td>INIT</td><td>复位或最低通信状态。</td><td>通常不能进行邮箱访问。</td></tr>
<tr><td>PREOP</td><td>适合对象字典和 Startup SDO 调试。</td><td>过程数据尚未完全工作。</td></tr>
<tr><td>SAFEOP</td><td>进入 OP 前核对 PDO 映射和输入过程数据。</td><td>输出行为仍受设备规则限制。</td></tr>
<tr><td>OP</td><td>进行运行态过程数据交换。</td><td>输出和驱动行为可能变为有效，必须先确认设备安全状态。</td></tr>
</table>

<h2 id="od">8. 对象字典与 SDO 读写</h2>
<p><b>选中对象</b> 表现在包含 <b>动作</b> 列，会在交互前直接显示每一行的本地意图：打开 Watch、打开 Startup、打开书签、打开轨迹、审阅证据、审阅差异、聚焦 OD、复制本行或复制完整摘要。<b>执行本行动作</b> 会随当前行变成 <b>执行：打开 Watch</b> 这类具体标签，然后执行当前行可见的本地动作，不需要记住双击、<code>Alt+Enter</code> 或右键菜单。<b>复制本行</b> 也会跟随所选字段变成 <b>复制：目标</b> 这类标签，并只把该行证据包复制到剪贴板。这些行级动作都是本地证据/导航动作，不读取总线、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。</p>
<p>对象字典页用于浏览当前从站的 Object Dictionary，并进行 SDO 上传/下载。对象字典表格支持筛选，并保留 <b>Last Value</b> 与 <b>Last Status</b> 证据列，让最近一次读取、写入完成和读回校验结果直接留在对象字典表中。单击某一行会自动把 Index、SubIndex、Type 等信息填入下方 SDO 指令区；<b>选中对象</b> 面板现在是目标工作台，会立即展示当前目标、来源、类别、对象名、访问权限、类型/位宽、读回值、写入值、表格证据、同一对象是否已经存在于 Watch、Startup SDO、对象书签或 SDO 目标轨迹、校验状态和推荐下一步。<b>证据集</b> 会在输入写入值之前汇总本地读回、Watch、OD、Startup、书签和目标轨迹值是否一致。<b>使用证据</b> 会把最佳本地值复制到写入框，不访问总线；<b>选择证据</b> 会打开同一组本地候选，证据冲突时可以明确选择要回填的值。<b>写入差异</b> 会在写入确认前，把待写值与本地证据进行比较：绿色表示已匹配本地证据，黄色表示待写值不同于已有证据，红色表示本地证据之间已经互相冲突。<b>审阅差异</b> 会打开比较背后最相关的本地证据行，包括匹配的目标轨迹证据；写入框为空时审阅证据集冲突，已有写入值时审阅写入差异。<b>复制证据</b> 会把当前目标、选中对象复核行、本地证据候选、证据链接和本地边界复制到剪贴板，便于交接或写入前复核。在选中对象面板行上双击或按 <code>Alt+Enter</code>，该面板会变成本地证据路由器：Watch、Startup、书签和目标轨迹行会打开匹配证据；证据集和写入差异行会审阅相关冲突或差异；目标/读回/OD 行会聚焦对象字典上下文；摘要行会复制证据摘要。右键选中对象行还会显示 <b>打开本行证据</b>、<b>复制本行证据</b> 和 <b>复制完整证据摘要</b>，不用记快捷键也能发现行级本地证据动作。SDO 写入确认会针对即将写入的同一从站/Index/SubIndex 再展示本地证据集，并说明目标值是匹配全部证据、只匹配部分证据，还是不匹配任何本地证据。<b>打开 Watch</b>、<b>打开 Startup</b>、<b>打开书签</b> 和 <b>打开轨迹</b> 会按同一个从站/Index/SubIndex 跳到匹配证据行，不读取或写入总线。它也会明确安全边界：面板更新、证据集、使用证据、选择证据、写入差异、审阅差异、复制证据、本行证据复制和证据跳转只整理本地上下文；读取、加入 Watch、写入和 Startup 变更仍然需要显式点击对应按钮。局部动作可以读取目标、按原有确认流程写入、加入 Watch、收藏为书签，或用当前写入值创建 Startup SDO。双击对象字典行会自动填充并立即读取。多选对象后可以使用 <b>读取所选</b>、<b>监视所选</b> 或 <b>证据启动项</b>，把已经读到 Last Value 的对象直接变成 Startup SDO 候选，减少重复单行操作。</p>
<p>对象字典提供语义过滤 chip：<b>可写</b>、<b>可读</b>、<b>CiA 402</b>、<b>身份</b>、<b>PDO</b>、<b>错误</b>、<b>有证据</b>、<b>失败</b> 会填入类似 <code>tag:cia402</code>、<code>tag:evidence</code>、<code>tag:failed</code> 的标签过滤。OD 摘要会显示当前表格的可见、可写、有证据、失败计数。普通自由文本过滤仍然会在 Object、Index、Access、Type、Bits、Name、Last Value、Last Status 等列中全文查找。过滤后点击 <b>读取可见项</b> 可以读取当前可见结果集，大批量读取会先确认；点击 <b>重试失败项</b> 只会读取最新 SDO 证据为失败的行；点击 <b>监视可见项</b> 可以把当前可见结果集直接加入 Watch，且不会立即发起大量 mailbox 读取。对象字典右键菜单也提供读取可见对象、重试失败对象、监视可见对象、有证据/失败快速筛选、<b>收藏对象</b>、<b>收藏所选对象</b>、<b>从所选证据创建 Startup SDO</b> 和 <b>复制最后证据</b>，不用离开表格就能复用当前行的证据。</p>
<p><b>对象书签</b> 是工程内的常用对象清单，适合保存调试中反复使用的控制字、状态字、模式对象、错误寄存器、厂家参数或 Startup 候选项。<b>书签</b> 会保存当前 SDO 目标，<b>收藏所选</b> 会保存选中的对象字典行。选中书签会刷新本地详情条，显示从站、对象地址、权限、类型、位宽、名称、保存的 Last Value、来源、复用就绪度和操作边界。双击书签只回填 SDO 字段，不读取、不写入、不切状态，也不运行主机诊断。点击 <b>监视书签</b> 可以把书签对象加入 Watch，且不会立即读取；点击 <b>创建启动项</b> 可以用所选书签保存的 Last Value 创建或更新 Startup SDO 行，这只是工程表格变更，不访问总线。整理完工程后可以移除书签。书签会保存到 <code>.ecatproj</code> 工程文件。</p>
<p>当前 SDO 字段也会暴露到命令面板。按 <code>Ctrl+P</code> 可以在任何页签直接读取当前对象、通过原有校验和确认流程写入、把读回值、最佳本地证据或手动选择的证据候选复制到写入框、加入 Watch、审阅写入差异证据、复制当前 SDO 证据摘要、打开或复制当前 <b>选中对象</b> 行证据、打开匹配的 Watch/Startup/书签/目标轨迹证据、创建 Startup SDO 行、从所选 OD 证据、对象书签或所选 SDO 目标轨迹行创建 Startup SDO 候选，恢复目标轨迹行、把目标轨迹行加入 Watch、收藏目标轨迹行，或者复制对象地址和值。指令区下方的 <b>Active SDO Inspector</b> 和 <b>选中对象</b> 面板会汇总当前主站、从站、对象、类型、读回值、写入值、来源表格、写入权限、对象类别、校验状态、Watch 关联、Startup 关联、书签关联、目标轨迹关联、写入差异和下一步建议，让每次读写前的目标与数值变化更清楚。<b>SDO 目标轨迹</b> 会记录最近从对象字典、PDO Map、Watch、Free Run、I/O 变量、SDO 历史、Startup SDO、对象书签、CiA 402 辅助动作和手动字段得到的本地目标；选中轨迹行会刷新本地详情条，显示时间、从站、对象地址、类型、来源、值、写入值、详情、复用就绪度和操作边界；匹配轨迹行现在会参与证据集、使用证据、选择证据和写入差异审阅。双击或点击恢复目标只会本地回填 SDO 字段，Watch、书签和 Startup 按钮可本地复用所选轨迹行，有保存值时会带入 Watch，创建 Startup 候选时优先使用写入值、缺失时使用最后值。目标轨迹会随工程保存；这些复用动作不读取总线、不写 SDO、不切状态、不改变 Free Run，也不运行 Host Health。</p>
<h3>读取一个对象</h3>
<ol>
<li>在左侧拓扑树选中目标从站。</li>
<li>打开 <b>对象字典</b> 页，必要时点击 <b>刷新</b> 更新在线数据。</li>
<li>在表格中单击对象条目，确认 Index、Sub、Type 已自动填入。读回值字段只显示当前行已有的 Last Value，没有当前行证据时会清空，避免误用上一个对象的值。</li>
<li>点击 <b>Read SDO</b>、<b>读取所选</b> 或 <b>读取可见项</b>。只有返回值匹配当前选中的从站、Index、Sub 时，Value 字段才会更新；批量读取、Watch 或其他对象的返回值只更新对象字典证据、Watch 和历史，不会覆盖当前目标读回框。批量读取和 Watch 刷新会尽量保留来源行 Type，避免 SDO 历史和 Watch 解码悄悄继承当前手动 Type。对象字典行会记录 Last Value 和 Last Status 证据，诊断事件也会记录操作。</li>
<li>如果对象可写，可以点击 <b>使用读回值</b>，把返回值复制到写入框，再用于微调或创建 Startup SDO 行。</li>
</ol>
<h3>写入一个对象</h3>
<ol>
<li>确认对象访问权限允许写入。只读对象会自动禁用写入框，减少误操作。</li>
<li>确认当前从站、Index、Sub、Type、写入值和单位范围。需要基于当前值微调时，先用 <b>使用读回值</b> 带入当前值，再只修改需要变化的部分。</li>
<li>在写入框输入值，点击 <b>Write SDO</b>。发送请求前，确认审阅会把影响预览分成关键影响、确认前复核、证据和目标上下文：从站状态/名称、对象类别、读回/Watch/OD/Startup/书签/目标轨迹本地证据集的一致或冲突状态、目标值与证据的匹配状态、当前值到目标值的变化、Startup 期望、拓扑基线警告，以及驱动控制、PDO 映射、持久化存储等高影响对象风险。</li>
<li>下载完成后，NekoEcat Studio 会自动读取同一对象并把实际值和期望写入值比较。匹配行会标记为成功；读回不匹配或读回失败会写入对象字典失败证据，并可通过 <b>下一最佳动作</b> 或 Failed 过滤直接审阅。</li>
</ol>
<p>提交写入前，界面会校验对象地址和值格式：Index 必须在 <code>0x0000..0xffff</code> 范围内，Sub 必须在 <code>0x00..0xff</code> 范围内，数值类型必须能解析为数字，并在适用时符合所选整数位宽范围；未知或空 Type 会作为警告提示。</p>
<h3>值和类型填写建议</h3>
<table>
<tr><th>类型</th><th>填写建议</th></tr>
<tr><td>无符号整数</td><td>按运行时支持使用十进制或十六进制，例如 <code>0</code>、<code>1</code>、<code>0x6040</code>。</td></tr>
<tr><td>有符号整数</td><td>使用设备手册允许的范围。负数必须和所选类型宽度匹配。</td></tr>
<tr><td>Float / Double</td><td>使用常规小数写法，并按设备手册确认单位和缩放。</td></tr>
<tr><td>String / Octet string</td><td>确认设备期望 ASCII 文本、原始字节，还是厂家自定义格式。</td></tr>
</table>
<h3>对象字典和其他页面联动</h3>
<ul>
<li>从 PDO Map、Free Run、Watch 右键或双击条目，也可以回填 SDO 指令区。</li>
<li>对象字典中选中的条目可以加入 Watch，便于反复读取。</li>
<li>手动修改 SDO 指令区会恢复写入模式，但仍要以设备手册为准确认访问权限。</li>
<li>SDO 目标轨迹行可以恢复、无需立即读取就加入 Watch、保存为对象书签，或转成本地 Startup SDO 候选。选中轨迹行会先显示值、写入值、来源、详情、复用就绪度和本地边界。当前目标匹配轨迹行时，轨迹值也会进入选中对象证据集，并可通过 <b>打开轨迹</b> 直接定位。Startup 创建优先使用轨迹中的写入值，没有写入值时使用轨迹值；这些轨迹复用动作只更新本地界面和工程表。</li>
</ul>
<div class="danger">SDO 写入可能改变驱动器模式、限位、增益、校准、保存参数或输出行为。对伺服、IO 输出、安全相关对象写入前，必须确认设备处于安全状态。</div>

<h2 id="sdo-history">9. SDO 历史与写入校验</h2>
<p>SDO 操作历史表会记录当前会话中的读取、写入和校验结果，用作调试决策和交接复核的审计记录。失败行会保留返回细节，便于区分类型不匹配、访问权限、对象不存在、超时和邮箱状态问题。</p>
<p>选中历史行会刷新本地详情条，显示操作时间、动作、从站、Index/Sub、类型、值、状态、详情、复用就绪度和操作边界。选择行、查看详情、从历史回填 SDO 字段、把所选行加入 Watch，以及用已完成且带值的历史行创建 Startup SDO 候选，都是本地审计/复用动作。右键历史行可以重新填充 SDO 字段、填充并读取同一对象、加入 Watch，或者用历史值创建 Startup SDO。多选历史行后，可以点击 <b>监视所选</b> 或使用右键批量动作创建 Watch 集合；这个批量加入不会立即发起大量 mailbox 读取。点击 <b>创建启动项</b> 或使用所选历史右键动作时，软件只会把带有记录值的历史行转成 Startup SDO，失败、仍在请求中、地址缺失或空值行会被跳过。双击和填充并读取会通过普通 SDO 读取路径显式读取所选对象。</p>
<p>这条路径用于把现场操作记录变成可复用工程配置：先手动读写和微调对象，在历史表中确认设备接受的值，再选中相关历史行生成 Startup SDO，最后用 Startup SDO 的预检查和校验流程复核后再应用。</p>
<p>SDO 写入后，软件会尽可能请求读回校验。把手动值固化为 Startup SDO 前，应先看历史表中的校验结果。写入确认路径会基于证据并按风险分组工作：比较目标值和读回/Watch/OD/Startup/书签/目标轨迹本地证据集，显示 Startup 期望，并在最终确认按钮可接受前突出拓扑基线或高影响对象风险。清空历史只会清理本次测试记录，不会清空工程备注或诊断事件。</p>

<h2 id="pdo">10. PDO 映射页</h2>
<p>PDO 映射页把在线 PDO 信息整理成结构化表格，用于理解周期过程数据的方向、索引、子项、位宽、偏移、名称和类型。它适合在启用 Free Run 前确认每个输入/输出字节代表什么。</p>
<p>选中 PDO 行会刷新本地详情条，显示 Sync Manager、PDO、方向、Index/Sub、位宽、推断 SDO 类型、名称、过程角色、CiA 402 候选证据和操作边界。PDO Map 数据加载后，选择行、筛选和详情预览都只是本地审阅动作；加载或刷新 PDO Map 才是显式在线 PDO 证据路径。筛选框可以按名称、Index、Sub、方向、类型等文本过滤。双击或右键条目可以把对应对象回填到 SDO 指令区；双击还会通过普通 SDO 读取路径读取对象。可以多选 PDO 行后点击 <b>选中项加入监视</b>，也可以在右键菜单中使用批量加入 Watch。软件会复用已有 Watch 行，不重复创建相同从站和对象。</p>
<p>批量加入不会立刻读取每个对象，准备好后再点击 <b>刷新监视</b>，避免一次性选择大量 PDO 时增加邮箱访问负载。对于大型伺服或复杂 IO 模块，建议先用筛选定位控制字、状态字、模式、位置、速度、电流、数字输入/输出等关键对象。</p>
<p>如果 PDO Map 显示不完整，先确认从站状态、ESI 文件、设备 Revision 和在线 PDO 输出。部分设备在不同状态或不同配置下暴露的映射可能不同。</p>

<h2 id="watch">11. Watch 监视页</h2>
<p>Watch 用于集中监视一组 SDO 对象，适合调试时反复读取控制字、状态字、模式、错误码、实际位置、温度、电压或自定义参数。对象可以来自对象字典、PDO Map、Free Run、I/O 变量或手动 SDO 字段。Watch 行包含时间、从站、Index、Sub、原始 Value、Decoded 解析、Type、Mode、Baseline 基线、Delta 偏差、Startup 启动值和 Startup Delta 启动偏差。选中某一行会刷新 Watch 详情条，把当前值、解析含义、基线偏离、Startup 对照、是否变化和 CiA 402 候选状态直接显示出来；选择、筛选和详情预览都只是本地证据动作，不读取 SDO、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。</p>
<p>对于 CiA 402 驱动器，可以点击 <b>CiA 402 预设</b> 或通过命令面板一次性加入常见驱动对象：控制字、状态字、运行模式、模式显示、错误码、实际位置、实际速度、实际转矩、目标位置、目标速度和目标转矩。已有 Watch 行会复用，因此调试过程中反复执行预设不会重复创建对象。Decoded 列会把状态字解析成 Switch on disabled、Ready to switch on、Operation enabled、Quick stop active、Fault 等驱动状态，并标出 warning、remote、target reached、internal limit 等关键标志。</p>
<p>命令面板还提供 CiA 402 控制字动作：shutdown、switch on、enable operation、quick stop 和 fault reset。如果当前从站 Watch 中已有解析后的状态字，命令面板还会基于该状态给出推荐的下一步控制字。这些动作会准备 <code>0x6040:0x00</code>、类型 <code>uint16</code> 和对应控制字值，然后继续走和手动 <b>Write</b> 按钮相同的校验与确认流程。</p>
<h3>添加 Watch 项</h3>
<ul>
<li>在对象字典中选择对象后点击 <b>添加当前 SDO</b>。</li>
<li>在 PDO Map、Free Run 或 Watch 相关表格中使用右键菜单加入 Watch。</li>
<li>手动填写 SDO 指令区后，也可以把当前对象加入 Watch。</li>
<li>右键 Watch 行可以用当前监视值创建 Startup SDO，也可以同步到已有 Startup SDO。多选 Watch 行后点击 <b>创建启动项</b> 会追加新启动行；点击 <b>同步启动</b> 会按从站、Index、SubIndex 更新已有启动行，没有匹配行时才创建新行。同步会弹出确认，只修改 Startup SDO 表，不会向总线写入。</li>
</ul>
<h3>刷新与变化观察</h3>
<ul>
<li><b>刷新监视</b>：立即读取全部 Watch 行。</li>
<li><b>自动</b>：按 250 ms、500 ms、1 s 或 2 s 周期读取。</li>
<li><b>捕获基线</b>：在设备处于已确认的正常状态后，把当前值保存到 Baseline，用于后续偏差比较。</li>
<li><b>清除基线</b>：移除 Baseline 和 Delta 结果。</li>
<li>Delta 会显示数值差异；非数值对象会显示匹配或已变化。</li>
<li>Watch 会把匹配的 Startup SDO 期望值显示到 Startup 列，并在 Startup Delta 中标出当前值是否和启动配置一致。</li>
<li><b>范围</b> 下拉框可以把 Watch 变成工程审阅视图：全部、当前从站、变化项、基线偏离、启动不一致、缺失值和 CiA 402。命令面板也提供同样的 Watch 范围切换，适合用键盘快速进入异常审阅。</li>
<li>值变化、基线偏离和 Startup 不一致会高亮，摘要中会显示监视项数量、当前范围、刷新模式、变化项数量、基线偏离数量、启动不一致数量和缺失值数量。</li>
<li>自动刷新默认尽量减少诊断噪声；手动刷新会记录操作事件。</li>
</ul>
<div class="note">Watch 本质上仍然是 SDO 邮箱访问。刷新频率过高可能增加邮箱负载。大量对象建议使用 1 s 或 2 s 周期，关键少量对象再使用 250 ms 或 500 ms。</div>
<p>解析后的 CiA 402 Watch 证据也会同步到 Selected Slave 卡片的 Drive 摘要中，因此总览页能显示当前从站驱动状态，而诊断页仍然是唯一的主机健康检查工作区。只有当 Watch 状态字支持推荐动作时，<b>驱动下一步</b> 才会启用；按钮文字会显示具体下一步动作，提示中会列出要写入的 <code>0x6040:0x00 uint16</code> 值，然后继续走普通写入确认。</p>

<h2 id="startup">12. Startup SDO 启动参数页</h2>
<p>Startup SDO 用于保存一组可重复执行的 SDO 写入步骤。它常用于调试阶段批量写入模式、映射、限位、滤波、同步或厂家特定参数。页面摘要会显示启动行总数、Watch 匹配数、不一致数、待比较数、无监视数，以及 <b>应用偏差</b> 将写入的行数。表格包含 Slave、Index、Sub、Value、Type、Status、Detail、Watch Value 和 Watch Delta；存在匹配 Watch 行时，Startup SDO 页会直接显示当前 Watch 值和是否与启动期望值一致。选中行会刷新本地详情条，显示行号、从站、Index/Sub、类型、期望值、状态、Watch 值、Watch 对照、tooltip 证据和写入边界。选择行、<b>只看偏差</b> 和此详情预览都是本地审阅动作；<b>校验</b> 会读取目标 SDO，<b>应用此行</b>、<b>应用所选</b>、<b>应用偏差</b> 和 <b>应用启动项</b> 只会通过普通确认流程写入 SDO 值。应用全部、所选或 Watch 偏差行前，确认审阅会刷新 Watch 证据，并把匹配/不一致/无监视统计、风险行、拓扑基线警告和前几行的当前值到目标值预览放入关键影响、复核项、证据和目标上下文分组。</p>
<p>点击 <b>审阅偏差</b> 会进入 Startup SDO 页，启用 <b>只看偏差</b>，并选中第一条期望启动值和当前 Watch 证据不一致的行。这样写入前会先形成一个明确审阅步骤：工程师可以确认到底哪些行会被触碰，再决定是把 Watch 同步回 Startup、手动修改期望值、校验所选行，还是只应用这些偏差。全局 <b>下一最佳动作</b> 在发现 Startup Watch 不一致时也会引导到这个审阅界面。</p>
<p>右键 Startup SDO 行可以回填 SDO 字段、填充并读取对象、加入 Watch、只校验当前行、只应用当前行、审阅或过滤 Watch 偏差、移动当前行、删除所选行，或者复制对象地址和值。也可以多选行后使用 <b>校验所选</b> 或 <b>应用所选</b>，只处理当前关注的一组启动项，不影响列表中其它参数。双击一行会立即回填并读取。这样启动参数表既是执行清单，也是现场复核和回归调试入口。</p>
<p>使用 <b>上移</b> 和 <b>下移</b> 控制执行顺序。部分设备要求模式、映射、限位和厂家对象按严格顺序写入，因此表格顺序应和设备手册及调试备注一致。</p>
<p>写入前使用 <b>预检查</b>。预检查会检查空字段、对象地址范围、值和类型格式、无效从站、当前拓扑缺失从站、重复对象和拓扑基线不匹配。重复对象写入相同值会作为警告；重复对象写入冲突值会作为错误。存在“预检查错误”的行必须先修复，<b>应用启动项</b> 才会继续。</p>
<p>使用 <b>校验启动项</b> 会逐行读取表中对象，并把当前设备值和期望启动值比较。匹配行显示“已校验”；不匹配行会在 Detail 中显示实际读回值。只改动少量参数时，可以使用 <b>校验所选</b>，避免轮询整张启动表。当 Watch 中已经有匹配启动对象的当前值时，<b>应用偏差</b> 只会写入期望启动值和当前 Watch 值不一致的 Startup SDO 行，适合在线调参后进行小范围修正，避免重复写入已经匹配的参数。</p>
<h3>创建步骤</h3>
<ol>
<li>先在对象字典或 SDO 指令区确认目标对象。</li>
<li>点击添加 Startup SDO，把当前从站、Index、Sub、Value、Type 记录到表格。</li>
<li>逐行检查值、类型和目标从站 position。</li>
<li>保存工程，使 Startup SDO 列表随工程保存。</li>
</ol>
<h3>应用步骤</h3>
<ol>
<li>确认当前总线拓扑没有变化，尤其是 slave position 没有移动。</li>
<li>确认设备处于允许写入的状态，通常为 PREOP 或 SAFEOP，具体以设备手册为准。</li>
<li>只需要重试或写入少量参数时，先选中这些行，再点击 <b>应用所选</b>。</li>
<li>点击 <b>Apply Startup SDO</b>。</li>
<li>观察 Status 和 Detail。Applied 表示该行成功；Failed 会显示失败原因或运行时返回信息。</li>
</ol>
<div class="warn">Startup SDO 是批量写入工具，不应在未确认拓扑和设备状态的情况下执行。更换设备、插拔从站、调整顺序后必须逐行复核。</div>

<h2 id="freerun">13. Free Run 自由运行页</h2>
<p>Free Run 通过运行时守护进程建立周期过程映像遥测，不依赖完整 PLC 工程即可观察过程数据。它适合快速查看 TxPDO/RxPDO 值、验证映射、检查变化趋势和定位设备状态。启动前，确认审阅会按分组显示过程映像影响预览：选中从站上下文、总线状态分布、PDO 映射证据、RxPDO/输出数量和示例、上次 Free Run 缓存、驱动 Watch 解析证据、拓扑基线问题、一致性门禁状态，以及启用后更快的 GUI 刷新节奏。</p>
<h3>使用方式</h3>
<ol>
<li>确认主站和从站在线，PDO Map 已能正常读取。</li>
<li>点击工具栏 <b>自由运行</b> 或使用 <code>Ctrl+F</code> 启动。</li>
<li>在 Free Run 页查看 Raw/Decoded、Direction、Index、Sub、Name、Bit Length 等字段。</li>
<li>使用筛选框按名称、Index、Sub、方向或值过滤。</li>
<li>启用 <b>Changed only</b> 只显示最近变化的条目。</li>
<li>选中某条过程映像行时，详情条会汇总从站、方向、对象地址、Name、Name 来源、Raw/Decoded/Meaning、位位置、PDO 映射状态、是否变化，以及输入/输出边界；选择、筛选和详情预览都不读取总线、不切换 Free Run，也不运行 Host Health。</li>
</ol>
<p>Free Run 表格会尽量稳定刷新，不会每次重建整表，因此滚动位置和选中项更容易保留。条目的 Name 会在本地稳定：优先使用 I/O 变量 Alias，其次使用运行时显示名、运行时名称、PDO Map 名称、对象级缓存和条目级缓存，最后才回退到对象地址，避免运行时某些周期 payload 缺失 name 时界面突然变空。<b>Map Detail</b> 也会显示当前 Name 的来源，方便判断它来自工程别名、运行时、映射证据还是缓存。<b>Map Status</b> 和 <b>Map Detail</b> 列会把当前选中从站的过程映像条目对照已加载 PDO Map，显示已映射、映射缺失、方向不一致或位宽不一致。当过程数据中出现 <code>0x6040</code>、<code>0x6041</code>、<code>0x6060</code>、<code>0x6061</code>、<code>0x603f</code> 等 CiA 402 对象时，如果运行时没有提供更具体 meaning，Meaning 列会复用 Watch 的驱动状态解析。</p>
<div class="danger">如果 PDO 中包含输出数据，Free Run 可能影响现场设备。连接伺服、阀岛、电机、加热、继电器等执行机构时，必须确认急停、使能、负载、人员安全和启动前影响预览。</div>

<h2 id="io-variables">14. I/O 变量页</h2>
<p><b>I/O 变量</b> 页是面向工程信号的汇总表。它把已加载的 PDO Map、Free Run 过程映像、Watch 当前值、Startup SDO 期望值、映射证据和最近变化合在同一张表里。需要从“对象/过程数据/启动参数”角度审阅信号时，不必在 PDO、Watch、Startup 和 Free Run 之间反复跳转。它不是主机诊断页；Linux 服务、网卡驱动、DKMS、blacklist、firmware 和设备节点检查仍然只在诊断页。</p>
<table>
<tr><th>来源</th><th>在 I/O 变量表中的作用</th></tr>
<tr><td>Free Run 过程映像</td><td>提供实时 Raw/Decoded、方向、变化高亮、映射状态和 CiA 402 meaning。</td></tr>
<tr><td>PDO Map</td><td>补齐变量名称、Index/SubIndex、方向、位宽和 PDO 映射证据；当 Free Run 尚未启动或 payload 缺少描述时也能形成工程信号列表。</td></tr>
<tr><td>Watch</td><td>带入最后一次 SDO 值、解码含义、变化状态、基线偏离、缺失值证据和 CiA 402 驱动解析。</td></tr>
<tr><td>Startup SDO</td><td>显示启动期望值，并标出当前值或 Watch 证据与启动配置不一致的对象。</td></tr>
</table>
<p><b>范围</b> 下拉框用于快速审阅：全部、当前从站、过程映像、仅 PDO、Watch 证据、Startup 不一致、缺失值、Rx 输出、Tx 输入、CiA 402 和变化项。摘要会显示当前可见集合，因此它可以作为切 SAFEOP/OP、启用 Free Run 或应用 Startup SDO 前的信号检查清单。选中行会刷新本地详情条，显示信号名或 Alias、对象地址、方向、来源、Raw/Decoded/Meaning、Watch 值、Startup 对照、PDO 映射状态、变化标记、PLC 质量、Tags、Note 和操作边界。切换范围、筛选、选择行、查看详情、复制声明、导出可见行以及编辑 Alias/Tags/Note 都只使用当前表格和工程证据。</p>
<p>选中或双击一行会把 Slave、Index、SubIndex 和推断 Type 回填到 SDO 指令区。<b>Fill SDO</b> 只准备目标；<b>Read SDO</b> 和双击才会显式执行普通 SDO 读取。<b>Watch Selected</b> 和 <b>Watch Visible</b> 会创建或复用 Watch 行，但不会立即读取，避免一次性选择大量信号时突然增加 mailbox 流量。<b>Startup Selected</b> 和 <b>Startup Visible</b> 会用 I/O 变量证据创建或更新 Startup SDO 候选，优先使用 Watch 值，缺失时用 Raw 值，且不读写总线。准备好后再进入 Watch 页刷新；真正应用 Startup 仍然走普通确认审阅。</p>
<p>点击 <b>别名</b> 或使用右键菜单，可以给变量设置工程内 Alias、逗号分隔 Tags 和备注。元数据按从站 position、Index、SubIndex 绑定，会保存到 <code>.ecatproj</code> 工程文件，也参与筛选；它只改变工程显示和导出内容，不会向总线写入。<b>批量命名</b> 可以一次为所选或可见行生成 Alias 和 Tags，默认保留已有 Alias；如果选择覆盖已有 Alias，会先弹出确认。这个动作适合把原始 PDO/对象名整理成 PLC 交接可用的工程命名。</p>
<p>点击 <b>导出 CSV</b> 或使用工程菜单，可以把当前可见 I/O 变量表导出给调试记录。点击 <b>PLC CSV</b> 或 <b>导出 PLC 符号 CSV</b> 会按当前可见行导出专门的符号规划文件，包含 Symbol、Direction、IEC 风格 Type、Slave、Index、SubIndex、PDO、来源证据、值证据、PLC Quality、Alias、Tags、Note 和对象地址。点击 <b>导出 PLC 声明 ST</b> 会把当前可见行写成 IEC 风格 <code>VAR_GLOBAL</code> 声明块并保存为 <code>.st</code> 文件，方便直接交给 PLC 工程。保存 PLC CSV 或 ST 导出文件前，软件会检查当前可见导出范围是否仍有交接质量问题，并提供 <b>审阅问题</b>、<b>继续导出</b> 或取消。命令面板和 I/O 变量右键菜单中的 <b>复制所选 PLC 声明</b> 与 <b>复制可见 PLC 声明</b> 会把同一声明块复制到剪贴板，复用相同的符号、类型、地址、方向和质量证据；复制声明也会经过同一交接质量门禁，仍有问题时可以选择 <b>继续复制</b>。审阅会打开 I/O 变量页的 PLC 交接问题范围并定位相关行，继续则把 Quality 证据保留在交接输出中供后续清理。导出和复制声明都只使用当前表格证据，不读取总线、不写 SDO、不改变 Free Run，也不运行主机诊断。</p>
<p><b>PLC</b> 质量列会标出每个信号是否已经适合 PLC 交接，或仍存在缺少 Alias、自动命名、缺少 Tags、符号重复等问题。可以使用 <b>审阅 PLC</b>、工程菜单中的 <b>审阅 PLC 交接问题</b>、页内范围下拉框或命令面板中的 <b>PLC 交接问题</b>，只审阅这些未就绪行后再导出或复制声明。表格会高亮 Startup 不一致、值变化、映射缺失、方向/位宽不一致、PLC 质量、已有别名/标签和 CiA 402 关键对象。右键菜单支持复制对象地址/值、复制 PLC 声明、回填 SDO、回填并读取、设置或清除 Alias/Tags、批量命名可见或所选行、审阅 PLC 交接问题、导出可见工程行、导出 PLC 符号行、把所选或可见信号加入 Watch，以及用所选或可见 I/O 证据生成 Startup SDO 候选。这个页面相当于过程数据、SDO 操作、启动参数、PLC 交接和状态机证据之间的桥。</p>

<h2 id="consistency">15. 一致性检查页</h2>
<p><b>一致性</b> 页是只读的 Online/Offline 工程审阅面。它只使用当前会话和工程中已经加载的证据：拓扑基线与当前在线从站、Startup SDO 期望与 Watch/I/O 变量证据、I/O 变量映射和值完整性、过期 Alias/Tags 元数据，以及缺少工程别名的变量。它不会发起总线读取、写入、状态切换、Free Run 改动或主机检查。</p>
<table>
<tr><th>范围</th><th>检查内容</th></tr>
<tr><td>拓扑</td><td>已捕获的拓扑基线和当前在线从站 position、名称、状态是否一致。</td></tr>
<tr><td>Startup</td><td>Startup SDO 行是否存在无效地址、缺少 Watch 对比，或期望值和 Watch 证据不一致。</td></tr>
<tr><td>I/O 变量</td><td>过程值是否缺失、PDO/过程映像映射证据是否完整、工程元数据是否过期、关键变量是否还缺少 Alias/Tags。</td></tr>
</table>
<p>点击 <b>刷新检查</b> 会基于当前证据重建表格。范围下拉框可聚焦全部、错误、警告、拓扑、Startup、I/O 变量或就绪行。表格列为级别、范围、目标、证据、期望、实际和建议动作。选中行会刷新本地详情条，显示级别、范围、目标、证据、期望/实际值、建议动作、最佳证据路由和只读门禁边界。<b>打开证据</b>、双击行、右键菜单和命令面板都会在不访问总线的前提下跳到最相关的已加载证据表：拓扑行打开状态机，Startup 行打开匹配的 Startup SDO 行，Watch 证据打开匹配 Watch 行，其余 I/O 问题打开 I/O 变量页并自动切到 PLC 交接问题、Startup 不一致、缺失值或 PDO 证据等相关范围。</p>
<p>建议在应用 Startup SDO、请求 OP、导出交接记录，或拿保存工程对比当前总线前使用一致性检查。总览页的 <b>执行下一步</b> 和状态栏 <b>下一最佳动作</b> 会在 Startup/Watch 证据清理后、Free Run 过程映像验证前引导到这个只读门禁。如果门禁已经发现 I/O 变量问题，下一最佳动作可以直接跳到聚焦后的 I/O 复核界面，而不是只停留在汇总表。Watch、Startup、PDO、Free Run、拓扑或 I/O 变量元数据变化后，旧结果会标记为过期。命令面板可以打开该页，也可以打开当前所选一致性行的证据目标；导出诊断报告也会包含 Consistency Check 表。主机环境诊断仍然只在诊断页。</p>

<h2 id="diagnostics">16. 诊断与主机健康检查</h2>
<p>诊断页是主机环境检查和诊断复核的唯一工作区，分为 Host Health 和 Event Stream。Host Health 关注 Linux 主机和 EtherCAT 环境，Event Stream 记录 GUI 和运行时操作。工程菜单中的导出诊断报告会写出主机健康、诊断事件、当前从站、拓扑基线、Watch、Startup SDO、SDO 历史、对象字典证据行、Free Run 表、I/O 变量表、一致性检查表、工程备注和原始命令快照。</p>
<table>
<tr><th>检查方向</th><th>说明</th></tr>
<tr><td>EtherCAT 服务</td><td>检查服务状态、运行时连接和主站可用性。</td></tr>
<tr><td>设备节点</td><td>检查 <code>/dev/EtherCAT0</code> 是否存在、权限是否允许当前用户访问。</td></tr>
<tr><td>网卡与驱动</td><td>检查网卡链路、驱动、USB 网卡、<code>r8152</code>/<code>rt8152</code> DKMS 相关提示。</td></tr>
<tr><td>黑名单与固件</td><td>检查常见 blacklist 配置，帮助定位重启后驱动或固件加载异常。</td></tr>
<tr><td>修复建议</td><td>每行可能给出建议动作和命令。选中行后可点击 <b>复制命令</b>。</td></tr>
</table>
<p>Event Stream 记录连接、刷新、重扫、状态切换、SDO 读写、Watch 刷新、Startup SDO 应用、Free Run 状态、命令面板显式触发、诊断检查和导出报告。出现错误时，先看最近几条 Error/Warning，再结合 Raw 输出确认底层返回。</p>

<h2 id="esi">17. ESI 仓库与 XML 查看</h2>
<p>通过 <b>工具 / 导入 ESI XML</b> 可以把设备描述文件加入仓库。ESI Repository 页显示文件名、Vendor、Product、Revision、Type、Name 和路径。ESI XML 页用于查看原始 XML。</p>
<p>ESI 文件不能替代在线设备返回，但它可以帮助识别设备名称、类型、对象描述和工程记录。遇到从站 Name 不完整、对象描述缺失或 Revision 难以确认时，应导入和硬件匹配的 ESI 文件。</p>

<h2 id="project">18. 工程文件、备注与诊断报告</h2>
<p>工程菜单提供新建、打开、保存、另存为、导出 I/O 变量 CSV、导出 PLC 符号 CSV、导出 PLC 声明 ST 和导出诊断报告。工程文件用于保存当前工作上下文，例如备注、Watch 行、Startup SDO 行、对象书签、I/O 变量 Alias/Tags/备注、当前主站/从站上下文、原始在线快照，以及对象字典证据快照，包括 Last Value、Last Status、细节和时间。Host Health 仍然只属于诊断页，不会移动到总览页。</p>
<p>备注页建议记录以下信息：设备型号和序列号、线缆连接、主站网卡、内核版本、驱动版本、参数修改原因、测试步骤、异常现象和最终处理结论。导出诊断报告适合在升级内核、切换 DKMS、调整 blacklist、修改 EtherCAT 配置前后留档。</p>

<h2 id="shortcuts">19. 快捷键、右键菜单与命令面板</h2>
<table>
<tr><th>功能</th><th>入口</th><th>用途</th></tr>
<tr><td>命令面板</td><td><code>Ctrl+P</code>；面板内 <code>Alt+A/L/O/D/H/F</code> 过滤类型，<code>Alt+P</code> 固定或取消固定命令</td><td>搜索命令、切换工作区、切换主站、打开说明书；可按全部、本地、在线、危险、主机和文件操作过滤，并把高频命令固定到最近命令前。</td></tr>
<tr><td>工作区直达</td><td><code>Ctrl+Alt+1</code> 总览、<code>Ctrl+Alt+2</code> 对象字典、<code>Ctrl+Alt+3</code> PDO 映射、<code>Ctrl+Alt+4</code> Watch、<code>Ctrl+Alt+5</code> Startup SDO、<code>Ctrl+Alt+6</code> Free Run、<code>Ctrl+Alt+7</code> I/O 变量、<code>Ctrl+Alt+8</code> 一致性、<code>Ctrl+Alt+9</code> 状态机、<code>Ctrl+Alt+0</code> 诊断</td><td>只切换工作区，不读取总线、不写 SDO、不切换状态、不启动 Free Run、不运行 Host Health。</td></tr>
<tr><td>工作区历史</td><td><code>Alt+Left</code> 后退、<code>Alt+Right</code> 前进</td><td>在最近访问的工作区之间往返；即使拖动重排页签，也按实际页面导航。</td></tr>
<tr><td>本地证据动作</td><td>在调试工作流、会话简报、从站证据矩阵、一致性、选中对象行、对象字典、PDO Map、Watch、Free Run 条目、I/O 变量、SDO 历史、Startup SDO、对象书签或 SDO 目标轨迹表中按 <code>Alt+Enter</code></td><td>只复制工作流步骤证据、打开本地证据、复制会话简报或矩阵本行证据、把矩阵从站路由到最相关的已加载证据表、审阅/复制选中对象证据或回填 SDO 目标，不读取总线、不写 SDO、不切换状态、不启动 Free Run、不运行 Host Health。</td></tr>
<tr><td>连接运行时</td><td><code>Ctrl+K</code></td><td>连接 <code>ecatd</code>。</td></tr>
<tr><td>刷新</td><td>标准刷新键或工具栏</td><td>读取最新主站、从站和对象信息。</td></tr>
<tr><td>重新扫描</td><td><code>Ctrl+Shift+R</code></td><td>总线拓扑变化后重新扫描。</td></tr>
<tr><td>自由运行</td><td><code>Ctrl+F</code></td><td>启动或停止 Free Run 遥测。</td></tr>
<tr><td>状态切换</td><td><code>Alt+1</code>/<code>Alt+2</code>/<code>Alt+3</code>/<code>Alt+4</code></td><td>把选中从站切换到 INIT/PREOP/SAFEOP/OP。</td></tr>
<tr><td>I/O 变量</td><td>命令面板或页签</td><td>打开 I/O 变量页、切换范围、把所选或可见变量加入 Watch。</td></tr>
<tr><td>一致性检查</td><td>命令面板或页签</td><td>打开只读 Online/Offline 一致性审阅表，不访问总线。</td></tr>
<tr><td>导出 I/O 变量 CSV</td><td>工程菜单、I/O 变量页或命令面板</td><td>导出当前可见工程信号表，不访问总线。</td></tr>
<tr><td>导出 PLC 符号 CSV</td><td>工程菜单、I/O 变量页、命令面板或右键菜单</td><td>导出当前可见变量的 PLC 符号规划记录，不访问总线。</td></tr>
<tr><td>导出 PLC 声明 ST</td><td>工程菜单、命令面板或 I/O 变量右键菜单</td><td>导出当前可见变量的 IEC <code>VAR_GLOBAL</code> 声明文件，不访问总线。</td></tr>
<tr><td>下一最佳动作</td><td>状态栏按钮</td><td>根据当前会话状态执行推荐下一步。</td></tr>
<tr><td>使用说明书</td><td>帮助菜单或命令面板</td><td>打开本说明书。</td></tr>
</table>
<p>状态栏的 <b>边界</b> 胶囊会跟随当前工作区变化，显示本地门禁、工程、在线 PDO、Watch 读取、SDO、过程数据、Startup 风险、状态风险、主机、文件/ESI、工程或原始证据等类型。它只是常驻安全标签，不是动作按钮；悬停后可以看到当前页面哪些操作只处理本地证据，哪些会使用在线 SDO/PDO/过程数据访问，哪些仍会走危险确认流程。</p>
<p><b>工作区快捷键</b>、工作区历史、<b>Alt+Enter</b> 本地证据动作和命令面板中的 <b>切换工作区</b> 命令都是纯导航/本地证据入口，只改变当前页、复制工作流步骤证据、打开已加载证据、复制会话简报或矩阵本行证据、把矩阵从站路由到最相关的证据表、审阅/复制选中对象证据，或从当前表格行回填 SDO 目标，不执行在线命令。需要读取 OD、刷新 Watch、切换 Free Run 或运行 Host Check 时，应使用对应动作命令。Host Health 仍然只属于诊断页。</p>
<p><b>下一最佳动作</b> 会随着会话状态切换为连接、重扫、选择从站、加载 OD、加载 PDO、加入 Watch、审阅 Startup 偏差、运行一致性、打开阻塞一致性证据、自由运行、查看诊断或打开命令。状态栏按钮使用语义颜色：蓝色表示普通下一步，琥珀色表示需要审阅证据，红色表示诊断错误，绿色表示就绪，灰色表示打开命令面板。高频页签标题也会显示本地徽标：普通数字表示当前证据行数，<b>!</b> 数字表示 Watch、Startup、Consistency、I/O、状态机或诊断风险，悬停可查看计数含义；这些徽标只来自已加载表格，不读取总线。它不会把主机检查放入总览页；当诊断需要处理时，只负责导航到诊断工作区。一致性已经有阻塞行时，它会打开第一条阻塞行最相关的证据目标，而不是只停留在汇总表。</p>
<p>命令面板包含稳定的 <b>切换工作区</b> 入口，以及当前 SDO 的上下文动作：执行调试下一步、切换工作流范围、审阅首个/下个工作流问题、复制所选工作流步骤证据、打开或复制所选会话简报证据行、打开或复制所选从站证据矩阵行、按语义组过滤对象字典、把可见对象字典结果加入 Watch、捕获或清除 Watch 基线、读取、写入、使用读回值、使用最佳证据、选择具体证据值、审阅写入差异证据、复制当前 SDO 证据摘要、打开或复制当前 <b>选中对象</b> 行证据、打开匹配的 Watch/Startup/书签/目标轨迹证据、恢复或复用目标轨迹行、加入 Watch、添加 CiA 402 监视预设、写入常用或推荐的 CiA 402 控制字、加入 Startup SDO、同步所选 Watch 值到 Startup SDO、审阅和 Watch 当前值不一致的 Startup SDO 行、只应用这些偏差行、打开 I/O 变量页、切换 I/O 变量范围、编辑或批量生成 I/O 变量别名、审阅 PLC 交接问题、复制所选或可见 PLC 声明、导出 I/O 变量 CSV、导出 PLC 符号 CSV、导出 PLC 声明 ST、把所选或可见 I/O 变量加入 Watch、打开一致性检查、打开所选一致性证据、复制地址、复制对象和值。每条命令都会标注 <b>本地</b>、<b>在线</b>、<b>危险</b>、<b>主机</b> 或 <b>文件</b>，也可以按操作类型过滤，这样导航和工程表格命令会与运行时读取、主机检查、SDO 写入和状态切换明显分开；在面板内可用 <code>Alt+A</code>、<code>Alt+L</code>、<code>Alt+O</code>、<code>Alt+D</code>、<code>Alt+H</code>、<code>Alt+F</code> 从键盘切换全部、本地、在线、危险、主机和文件过滤；用 <code>Alt+P</code> 或行右键菜单可以把高频命令固定到最近命令之前，固定列表保存在本地设置中且不会自动运行；从命令面板显式触发过的命令会在后续打开时排在前面并标记为 <b>最近</b>，这只改变排序，不会自动运行命令；结果摘要会显示当前搜索/过滤下有多少命令可执行、多少固定命令、多少最近命令，以及 Local/Online/Danger/Host/File 各自可见数量；当前选中的命令会在固定预览区显示操作类型、安全边界、命令说明、固定/最近标记以及当前上下文是否不可执行；每次从命令面板显式触发命令都会写入一条 <b>Command Palette</b> 事件流记录，危险类型会以 Warning 记录，真正写入 SDO 或切换状态前仍然继续走已有确认流程。对象和证据表格还支持 <code>Alt+Enter</code> 走本地回填/打开证据路径；右键菜单则继续提供复制行、复制对象地址（例如 <code>#3 0x6040:0x00</code>）、复制对象和值（例如 <code>#3 0x6040:0x00 uint16 = 0x0006</code>）、回填 SDO、读取对象、收藏对象、加入 Watch 等上下文操作。选中对象面板有专用右键菜单，可打开本行证据、复制本行证据、复制完整证据摘要、审阅差异并自动调整列宽，全部不访问总线。</p>

<h2 id="workflow">20. 推荐调试流程</h2>
<ol>
<li><b>主机确认</b>：打开诊断页运行 Host Check，确保服务、网卡、权限、驱动没有明显异常。</li>
<li><b>连接总线</b>：连接运行时，重新扫描，刷新。</li>
<li><b>确认身份</b>：在拓扑树选择从站，在总览页确认 Vendor/Product/Revision/Name。</li>
<li><b>确认状态</b>：检查当前状态和端口链路，必要时先切 PREOP 或 SAFEOP。</li>
<li><b>查看对象</b>：在对象字典读取关键对象，例如设备类型、错误码、状态字、模式、厂家参数。</li>
<li><b>查看 PDO</b>：在 PDO Map 中确认过程数据布局，定位控制字、状态字、模式、位置、速度等条目。</li>
<li><b>审阅 I/O 变量</b>：在 I/O 变量页合并查看 PDO、Free Run、Watch 和 Startup 证据，按当前从站、变化项、Startup 不一致或 CiA 402 范围快速定位信号，并给关键变量设置 Alias/Tags 方便交接和导出。</li>
<li><b>一致性检查</b>：在一致性页只读对比拓扑基线、Startup、Watch 和 I/O 变量证据，先处理 Error/Warning；若证据变化导致结果过期，重新刷新后再继续 Free Run、写入或切 OP。</li>
<li><b>加入 Watch</b>：把关键 SDO 加入 Watch，使用手动或低频自动刷新观察变化。</li>
<li><b>配置 Startup SDO</b>：把可重复参数写入整理为 Startup SDO，逐行验证。</li>
<li><b>启用 Free Run</b>：在安全条件下观察周期过程数据，使用过滤和 Changed only 定位变化。</li>
<li><b>导出报告</b>：保存工程并导出诊断报告，保留现场状态和问题证据。</li>
</ol>

<h2 id="trouble">21. 常见故障排查</h2>
<h3>GUI 连接不上 ecatd</h3>
<ul>
<li>查看底部 Runtime Log 是否有启动失败、端口占用或权限错误。</li>
<li>运行 Host Check，确认 <code>ecatd</code> 是否监听 <code>127.0.0.1:5877</code>。</li>
<li>如果端口被其他进程占用，先停止旧的运行时实例，再重新连接。</li>
</ul>
<h3>重启电脑后 EtherCAT 不可用</h3>
<ul>
<li>检查 EtherCAT 服务状态和 <code>/dev/EtherCAT0</code> 是否存在。</li>
<li>确认当前用户是否在 ethercat 组，设备节点权限是否允许访问。</li>
<li>检查主站配置中的网卡是否变化。USB 网卡重插后名称和 MAC 可能变化。</li>
<li>如果使用 Realtek USB 网卡，检查是否使用预期的 <code>r8152</code>/<code>rt8152</code> DKMS 驱动，是否被系统默认驱动抢占。</li>
<li>检查 firmware 或模块 blacklist，确认没有把需要的驱动或固件禁用。</li>
<li>运行 Host Check 后复制建议命令，逐项处理。</li>
</ul>
<h3>刷新后从站数量为 0</h3>
<ul>
<li>检查网线、从站供电、链路灯和端口方向。</li>
<li>确认 EtherCAT 主站绑定的是实际连接总线的网卡。</li>
<li>确认普通网络服务没有占用该网卡。</li>
<li>先重新扫描，再刷新；仍然为 0 时看 Master Raw 输出。</li>
</ul>
<h3>SDO 读取失败</h3>
<ul>
<li>确认从站支持 CoE mailbox。</li>
<li>确认从站状态允许邮箱访问，通常 PREOP/SAFEOP 更适合读写 SDO。</li>
<li>确认 Index/SubIndex 是否存在于该设备 Revision。</li>
<li>查看错误是否为 object not found、access denied、type mismatch 或 timeout。</li>
</ul>
<h3>SDO 写入失败</h3>
<ul>
<li>确认对象不是只读、当前状态允许写入。</li>
<li>确认 Type 和输入值格式匹配，例如整数、有符号数、十六进制、字符串等。</li>
<li>部分参数需要先停机、解除使能、进入 PREOP 或执行厂家指定解锁步骤。</li>
<li>写入后必须读回确认，不要只依赖按钮返回。</li>
</ul>
<h3>从站切不到 OP</h3>
<ul>
<li>先从 INIT 到 PREOP，再到 SAFEOP，再到 OP，观察哪一步失败。</li>
<li>检查 PDO Map、Sync Manager、Distributed Clock、Watchdog 和设备报警。</li>
<li>确认 Startup SDO 是否正确执行，是否有 Failed 行。</li>
<li>查看 Slave Raw 和 Event Stream 中最近的错误。</li>
</ul>
<h3>Free Run 中 Name 偶发丢失</h3>
<ul>
<li>软件会按 I/O 变量 Alias、运行时显示名、运行时名称、PDO Map 名称、对象级缓存、条目级缓存和对象地址回退的顺序稳定 Name；先查看 <b>Map Detail</b> 中的“名称来源”判断当前来源。</li>
<li>如果仍然只剩地址回退或描述不足，先刷新 PDO Map，确认 I/O 变量 Alias 是否保存，再重新启动 Free Run。</li>
<li>确认 ESI 文件和设备 Revision 匹配，必要时导入正确 ESI。</li>
<li>如果只有 Raw/Decoded 有值而 Name 为空，说明运行时数据可用，但描述信息不足。</li>
</ul>

<h2 id="safety">22. 安全操作清单</h2>
<ul>
<li>写 SDO、应用 Startup SDO、切换 OP、启用 Free Run 前，先确认当前主站和从站。</li>
<li>对伺服、阀岛、继电器、加热、机械执行机构操作前，确认急停、限位、负载和人员位置。</li>
<li>批量写入前保存工程，执行后导出诊断报告。</li>
<li>任何写入都建议读回确认，必要时断电重启设备验证参数是否持久。</li>
<li>驱动、DKMS、firmware blacklist、EtherCAT 服务配置属于主机级修改，修改前后都应保留诊断报告。</li>
<li>现场排障时一次只改一个变量：网卡、驱动、服务、线缆、从站顺序、参数写入不要同时改。</li>
</ul>
</body>
</html>
)HTML");

  const QString html = finalizeDocumentationHtml(
      uiText(english, chinese), QCoreApplication::applicationVersion(),
      activeMasterName(), ecatdPath(), settings_.theme == "Light");
  browser->setHtml(html);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); // wire signal to slot

  layout->addWidget(title);
  layout->addWidget(subtitle);
  layout->addLayout(manualTools.layout);
  layout->addWidget(browser, 1);
  layout->addWidget(buttons);
  dialog.exec();
}


// — Open the About dialog with version and build information
void MainWindow::showAbout() {
  QDialog dialog(this);
  dialog.setObjectName("aboutDialog");
  dialog.setWindowTitle(
      uiText("About NekoEcat Studio", "关于 NekoEcat Studio"));
  dialog.setModal(true);
  dialog.resize(860, 700);

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(20, 20, 20, 16);
  layout->setSpacing(12);

  auto *title = new QLabel("NekoEcat Studio");
  title->setObjectName("dialogTitle");
  auto *tagline = new QLabel(
      uiText("Modern EtherCAT engineering workstation for IgH EtherCAT Master.",
             "面向 IgH EtherCAT Master 的现代 EtherCAT 工程工作站。"));
  tagline->setObjectName("statusSummary");
  tagline->setWordWrap(true);

  auto *browser =
      makeDocumentationBrowser("aboutBrowser", settings_.theme == "Light");

  const QString english = QStringLiteral(R"HTML(
<!doctype html>
<html>
<head>
<style>
body { font-family: "Inter", "Segoe UI", "Noto Sans", sans-serif; font-size: 14px; line-height: 1.62; color: #172033; }
h2 { font-size: 18px; margin: 18px 0 8px 0; color: #12376f; }
ul { margin-top: 6px; padding-left: 22px; }
li { margin: 4px 0; }
code { font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace; background: #eef2f7; border-radius: 5px; padding: 1px 5px; }
table { border-collapse: collapse; width: 100%; margin: 8px 0 14px 0; }
th, td { border: 1px solid #d9e1ec; padding: 7px 9px; vertical-align: top; }
th { background: #f0f4f9; color: #475569; }
.warn { background: #fff7ed; border-left: 4px solid #f97316; padding: 9px 12px; margin: 10px 0; }
</style>
</head>
<body>
<p><b>NekoEcat Studio</b> is a focused EtherCAT commissioning, online inspection, and diagnostics workstation for Linux systems using IgH EtherCAT Master. It is designed for practical field work: scan the bus, inspect slaves, read and write Object Dictionary entries, analyze PDO maps, compose I/O variable signal tables, monitor SDO values, apply repeatable startup writes, run process-image telemetry, check the host environment, and export diagnostics.</p>
<table>
<tr><th>Item</th><th>Value</th></tr>
<tr><td>Version</td><td><code>%1</code></td></tr>
<tr><td>Runtime</td><td><code>ecatd</code></td></tr>
<tr><td>Runtime endpoint</td><td><code>127.0.0.1:5877</code></td></tr>
<tr><td>Active master</td><td><code>%2</code></td></tr>
<tr><td>Runtime binary</td><td><code>%3</code></td></tr>
<tr><td>Backend</td><td>IgH CLI + ecrt Free Run</td></tr>
</table>
<h2>Workspace Model</h2>
<table>
<tr><th>Workspace</th><th>Purpose</th></tr>
<tr><td>Overview</td><td>Default bus and selected-slave summary. It shows master metrics, identity, ports, mailboxes, a read-only Session Brief for target/gate/SDO/runtime evidence with row evidence copy, a multi-slave evidence matrix, and commissioning navigation. It does not run host diagnostics.</td></tr>
<tr><td>Object Dictionary</td><td>CoE/SDO workspace for selecting objects, filling SDO command fields, summarizing and reviewing local evidence agreement, copying the best local evidence or a chosen evidence candidate into the write field, comparing the pending write value against local Read/OD/Watch/Startup/Bookmark/Target Trail evidence, reviewing the evidence behind that Write Delta, jumping to matching evidence rows, bookmarking project-critical objects, reading, writing with automatic read-back verification, focusing failed evidence, and reviewing SDO history with a selected-row audit detail strip.</td></tr>
<tr><td>PDO Map / Free Run</td><td>Process-data interpretation and cyclic telemetry without a PLC project. PDO Map includes a selected-row local detail strip for direction, object address, bit width, inferred type, name, role, and boundary review.</td></tr>
<tr><td>I/O Variables</td><td>Engineering signal table merging PDO Map, Free Run process image, Watch values, Startup expectations, map evidence, change evidence, and project-local aliases/tags/notes. It includes a selected-signal local detail strip for value, Startup, map, PLC, metadata, and boundary review. It can fill/read SDO targets, add selected or visible signals to Watch without immediate reads, bulk-name selected or visible signals, review PLC handoff quality, export visible engineering rows as CSV, export normalized PLC symbol planning records, and export IEC <code>VAR_GLOBAL</code> declarations as ST.</td></tr>
<tr><td>Consistency</td><td>Read-only online/offline gate across topology baseline, Startup SDO, Watch evidence, I/O Variables, and project-local metadata. It includes a selected-row local detail strip for severity, evidence, expected/actual values, action, route, and boundary review. Overview, Next Best Action, Free Run confirmation, and state-change evidence route through it before Free Run/state progression; stale evidence requires a refresh. It does not read or write the bus and does not run host diagnostics.</td></tr>
<tr><td>State Machine</td><td>Slave-state matrix with evidence, risk, conservative recommendations, a selected-row transition detail strip, and confirmed state requests. Row selection is local preview only, and it does not run host diagnostics.</td></tr>
<tr><td>Diagnostics</td><td>The dedicated host and runtime diagnostics workspace. Host Health, repair commands, and Event Stream stay here.</td></tr>
</table>
<h2>Core Capabilities</h2>
<ul>
<li>Multi-master profiles with fast active-master switching.</li>
<li>Overview-first workspace for high-frequency bus, selected-slave, read-only Session Brief decision context with local evidence navigation and row evidence clipboard digests, a searchable and scope-filtered multi-slave Slave Evidence Matrix for OD/PDO/Watch/Startup/process readiness, matrix row evidence clipboard digests for handoff, a searchable and scope-filtered readiness-scored phase/risk/evidence/action commissioning workflow board with actionable Run Next, local Review First/Next issue selection, a selected-step detail strip that exposes the local/online/process-data execution boundary before Run Next or double-click, and workflow step evidence clipboard digests, global Next Best Action navigation, pure workspace switching through View / Workspaces, <code>Ctrl+Alt+1..0</code>, reorder-safe Back/Forward workspace history, and stable Command Palette Go to Workspace entries, semantic Next Best Action colors, high-frequency tab badges for local evidence/risk counts, color-coded Watch-derived drive-state checks, and confirmed Drive Next actions with visible recommended controlword targets.</li>
<li>Topology tree, slave identity, port, mailbox, raw online outputs, selected-slave evidence completeness scoring, and a read-only Snapshot operation that gathers identity, OD, PDO, ESI XML, and CiA 402 Watch evidence without state changes or writes.</li>
<li>Object Dictionary browsing with semantic quick filters including Evidence/Failed, visible/writable/evidence/failed summary counts, Last Value/Last Status evidence columns, direct OD-evidence-to-Startup candidate creation, project-local Object Bookmarks for repeated SDO targets with selected-row saved-evidence detail preview, bookmark-to-Watch and bookmark-to-Startup candidate creation, project-saved SDO Target Trail for recent local targets with selected-row reuse-boundary detail preview, restore, Watch, Bookmark, Startup reuse, Evidence Set participation, Use/Pick Evidence candidates, Write Delta review, and Open Trail navigation, target-safe read-back field updates, type-preserving batch reads and Watch refresh, Evidence Set local agreement summary and conflict navigation, best-local-evidence and chosen-evidence write-field fill, local Write Delta comparison and Review Delta evidence navigation before confirmation, local SDO Evidence Digest clipboard copy for handoff/pre-write review, confirmation-time Read/Watch/OD/Startup/Bookmark/Target Trail Evidence Set impact preview, automatic write read-back verification, failed-evidence Next Best Action routing, failed-row retry, visible-result read/watch actions, table context-menu evidence reuse, automatic SDO field fill, Selected Object target panel with a visible row Action column, dynamic Run Row Action label/execution, dynamic Copy Row evidence copy, row-level double-click/Alt+Enter local evidence routing, a dedicated row context menu for Open Row Evidence, Copy Row Evidence, Copy Full Evidence Digest, Review Delta, and autosizing, Active SDO Inspector context and validation summary, command-palette current-SDO actions including Selected Object row open/copy, and double-click read.</li>
<li>SDO History reuse with selected-row audit detail preview, local fill target reuse, selected-row Watch creation without immediate reads, and Startup SDO generation from accepted field values.</li>
<li>PDO Map filtering with selected-row local process-data detail preview, quick SDO actions, inferred type/name/role review, and bulk selected-row Watch creation without immediate reads.</li>
<li>I/O Variables workspace that merges PDO Map, Free Run, Watch, Startup SDO, map status, changed-value evidence, and project-local alias/tag/note metadata into one signal table with scopes for selected slave, process image, PDO-only rows, Watch evidence, Startup diff, missing values, Rx outputs, Tx inputs, CiA 402, changed rows, and PLC handoff issues, selected-row signal detail preview with local boundary labels, selected/visible I/O-evidence-to-Startup candidate creation, bulk signal naming, focused PLC handoff review, visible-row CSV export, IEC-style PLC declaration copy/export, and normalized PLC symbol CSV export with pre-export quality review.</li>
<li>Read-only stale-aware Consistency gate for online/offline review, including topology-baseline mismatches, Startup-versus-Watch differences, I/O variable map/value gaps, stale project metadata, missing engineering aliases, PLC handoff quality issues, selected-row consistency detail preview with local route and boundary labels, Open Evidence navigation from consistency rows into State Machine, Startup SDO, Watch, or I/O Variables without bus access, Next Best Action routing into the first blocking evidence target, workflow gating before Free Run/state progression, Free Run/state confirmation evidence, OP recommendation gating, and diagnostics-report export.</li>
<li>Watch list with manual or periodic SDO refresh, scope filters for selected slave, changed rows, baseline drift, Startup diff, missing values, and CiA 402 drive evidence, selected-row value/detail preview, baseline/delta drift checks, Startup SDO expected-value comparison, CiA 402 drive presets, recommended confirmed controlword actions, decoded drive states, bulk Watch-to-Startup creation, confirmed Watch-to-Startup synchronization that updates existing rows before creating missing ones, type retention, and value-change highlighting.</li>
<li>State Machine workspace with per-slave current state, recommended next state, evidence, decoded drive status, Startup diff, PDO/Free Run process evidence, risk notes, selected-row transition detail preview, row-level confirmed state requests, double-click recommended-state sending, and conservative PREOP/SAFEOP/OP progression.</li>
<li>Evidence-driven state-change confirmation for selected-slave and all-slave transitions, including selected-slave evidence score, drive evidence, Startup mismatches, PDO-map issues, topology-baseline warnings, and all-slave state mix.</li>
<li>Startup SDO sequence management with per-row status, selected-row local detail preview, selected-row verify/apply, Watch Value/Watch Delta evidence columns, Diffs Only review/focus, Watch-diff-only apply, batch impact preview with current-to-target evidence, and explicit read/write boundary labels.</li>
<li>Free Run process-image telemetry with startup impact preview, PDO-map evidence columns, filtering, changed-only view, selected-row process detail preview, stable name backfill from I/O aliases, runtime names, PDO-map names, object cache, and entry cache with visible name-source detail, output/input boundary labeling, and CiA 402 meaning decode.</li>
<li>Host diagnostics for EtherCAT service, device node, NIC driver, blacklist, DKMS, and repair hints.</li>
<li>ESI repository, project notes, project persistence for Object Dictionary evidence snapshots, Object Bookmarks, and I/O variable metadata, runtime log, visible I/O Variables CSV export, PLC declaration copy/export, PLC symbol CSV export, and Markdown diagnostics export with host, OD, Free Run, I/O variable, and consistency evidence.</li>
</ul>
<h2>Runtime Boundary</h2>
<p>The GUI is not the EtherCAT master. It connects to <code>ecatd</code> on localhost, and <code>ecatd</code> routes requests to IgH tools and the ecrt Free Run path. This keeps online operations explicit and allows the GUI to keep a clear audit trail for reads, writes, state changes, host checks, and exported reports.</p>
<h2>Design Goal</h2>
<p>The application is built as a modern engineering station rather than a raw command wrapper. Frequent bus operations stay near the front, online data is structured into task-oriented tabs, and diagnostics stay in the dedicated Diagnostics workspace so hardware, driver, and runtime issues can be reviewed without polluting the Overview page.</p>
<div class="warn"><b>Safety:</b> SDO writes, Startup SDO, OP transitions, and Free Run can affect real hardware. Confirm the selected master, selected slave, object index, subindex, value, and machine state before executing write or output-related operations.</div>
</body>
</html>
)HTML");

  const QString chinese = QStringLiteral(R"HTML(
<!doctype html>
<html>
<head>
<style>
body { font-family: "Inter", "Microsoft YaHei UI", "Noto Sans CJK SC", "Segoe UI", sans-serif; font-size: 14px; line-height: 1.72; color: #172033; }
h2 { font-size: 18px; margin: 18px 0 8px 0; color: #12376f; }
ul { margin-top: 6px; padding-left: 22px; }
li { margin: 4px 0; }
code { font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace; background: #eef2f7; border-radius: 5px; padding: 1px 5px; }
table { border-collapse: collapse; width: 100%; margin: 8px 0 14px 0; }
th, td { border: 1px solid #d9e1ec; padding: 7px 9px; vertical-align: top; }
th { background: #f0f4f9; color: #475569; }
.warn { background: #fff7ed; border-left: 4px solid #f97316; padding: 9px 12px; margin: 10px 0; }
</style>
</head>
<body>
<p><b>NekoEcat Studio</b> 是一个面向 Linux + IgH EtherCAT Master 的 EtherCAT 调试、在线检查和诊断工作站。它不是简单包一层命令行，而是把总线扫描、从站识别、对象字典、SDO 读写、PDO 映射、I/O 变量工程表、Watch 监视、Startup SDO、Free Run 过程映像、主机环境检查和诊断导出整合成一套连续工作流。</p>
<table>
<tr><th>项目</th><th>内容</th></tr>
<tr><td>版本</td><td><code>%1</code></td></tr>
<tr><td>运行时</td><td><code>ecatd</code></td></tr>
<tr><td>通信端点</td><td><code>127.0.0.1:5877</code></td></tr>
<tr><td>当前主站</td><td><code>%2</code></td></tr>
<tr><td>运行时路径</td><td><code>%3</code></td></tr>
<tr><td>后端</td><td>IgH CLI + ecrt Free Run</td></tr>
</table>
<h2>工作区划分</h2>
<table>
<tr><th>工作区</th><th>定位</th></tr>
<tr><td>总览</td><td>默认总线和当前从站概览页。展示主站指标、身份信息、端口、邮箱、目标/门禁/SDO/运行证据的只读会话简报、本行证据复制和调试导航，不运行主机诊断。</td></tr>
<tr><td>对象字典</td><td>CoE/SDO 工作区，用于选中对象、自动填充 SDO 指令、汇总和审阅本地证据一致性、把最佳证据或指定证据候选回填写入框、用读回/OD/Watch/Startup/书签/目标轨迹证据比较待写值、跳转匹配证据行、收藏工程关键对象、读取、写入、校验和查看带选中行审计详情条的 SDO 历史。</td></tr>
<tr><td>PDO 映射 / Free Run</td><td>用于理解过程数据，并在没有完整 PLC 工程时观察周期过程映像。PDO 映射包含选中行本地详情条，用于复核方向、对象地址、位宽、推断类型、名称、角色和边界。</td></tr>
<tr><td>I/O 变量</td><td>工程信号汇总表，合并 PDO Map、Free Run 过程映像、Watch 值、Startup 期望、映射证据、变化证据和工程内 Alias/Tags/备注；包含选中信号本地详情条，用于复核值、Startup、映射、PLC、元数据和操作边界；支持回填/读取 SDO，把所选或可见信号加入 Watch，加入时不立即读取，批量生成工程命名，复核 PLC 交接质量，导出可见变量 CSV，导出标准化 PLC 符号规划 CSV，并导出 IEC <code>VAR_GLOBAL</code> ST 声明文件。</td></tr>
<tr><td>一致性</td><td>只读 Online/Offline 门禁工作区，对比拓扑基线、Startup SDO、Watch 证据、I/O 变量和工程内元数据；包含选中行本地详情条，用于复核级别、证据、期望/实际值、动作、路由和边界；总览页、下一最佳动作、Free Run 确认和状态切换证据都会在 Free Run/状态推进前经过这里，证据过期后需要刷新；不读取或写入总线，也不运行主机诊断。</td></tr>
<tr><td>状态机</td><td>从站状态矩阵，展示证据、风险、保守推荐状态、选中行切换详情预览和带确认的状态请求；选中行只做本地预览，不运行主机诊断。</td></tr>
<tr><td>诊断</td><td>专用的主机和运行时诊断工作区。Host Health、修复命令和 Event Stream 都保留在这里。</td></tr>
</table>
<h2>核心能力</h2>
<ul>
<li>多主站配置和快速切换，适合多网卡、多工位、多总线场景。</li>
<li>总览页作为默认首页，优先展示最高频的连接、主站、从站、链路、当前从站状态、带本地证据导航和本行证据摘要复制的只读会话简报、可搜索和按范围过滤的多从站证据矩阵、用于交接的矩阵行证据摘要复制、可搜索和按范围过滤且带就绪度评分的阶段/风险/证据/动作调试工作流看板、本地审阅首个/下个工作流问题、可在执行下一步或双击前显示本地/在线/过程数据边界的选中步骤详情条、工作流步骤证据摘要复制、全局下一最佳动作入口，并支持通过视图/工作区菜单、<code>Ctrl+Alt+1..0</code>、可抵抗页签重排的工作区后退/前进历史和命令面板中稳定的“切换工作区”入口进行纯导航；同时提供带颜色的 Watch 派生驱动状态和带确认的驱动下一步动作。</li>
<li>拓扑树、从站身份、端口、邮箱能力和原始输出统一查看；Selected Slave 面板提供当前从站证据完整度评分；选中从站支持只读准备快照，一次采集身份、OD、PDO、ESI XML 和 CiA 402 Watch 证据，不切换状态也不写入。</li>
<li>对象字典支持包含有证据/失败在内的语义快速过滤、可见/可写/有证据/失败摘要、Last Value/Last Status 证据列、直接从 OD 证据生成 Startup 候选、带选中行保存证据详情预览的工程内对象书签、书签加入 Watch 和书签生成 Startup 候选、带选中行复用边界详情预览的随工程保存 SDO 目标轨迹，并可从轨迹行恢复目标、加入 Watch、保存书签、生成 Startup 候选、参与证据集、作为使用/选择证据候选、参与写入差异审阅并通过打开轨迹定位、可见结果一键读取/加入 Watch、证据集本地一致性摘要和冲突跳转、最佳本地证据或指定证据候选回填写入框、写入确认前的本地写入差异比较和审阅差异证据导航、本地 SDO 证据摘要复制用于交接或写入前复核、确认弹窗中的读回/Watch/OD/Startup/书签/目标轨迹证据集影响预览，选中对象自动填充 SDO 指令，选中对象面板显示目标上下文，并支持在行上双击或按 Alt+Enter 本地路由 Watch/Startup/书签/目标轨迹、证据集、写入差异和 OD 上下文，Active SDO Inspector 显示上下文和校验摘要，命令面板可执行当前 SDO 动作，双击对象自动读取。</li>
<li>SDO 历史支持选中行审计详情预览、本地回填目标复用、多选加入 Watch 且不立即读取，以及把已接受的带值历史行生成 Startup SDO。</li>
<li>PDO Map 支持过滤、选中行本地过程数据详情预览、右键回填 SDO、推断类型/名称/角色复核，以及选中多行批量加入 Watch 且不立即读取，便于理解过程数据。</li>
<li>I/O 变量工作区把 PDO Map、Free Run、Watch、Startup SDO、映射状态、变化证据和工程内 Alias/Tags/备注合并到同一张信号表，并提供当前从站、过程映像、仅 PDO、Watch 证据、Startup 不一致、缺失值、Rx 输出、Tx 输入、CiA 402、变化项和 PLC 交接问题等范围过滤、选中行信号详情预览和本地边界说明，以及所选/可见 I/O 证据生成 Startup 候选、批量工程命名、聚焦 PLC 交接复核、可见行 CSV 导出、IEC 风格 PLC 声明复制/导出和带导出前质量提示的 PLC 符号 CSV 导出。</li>
<li>一致性检查提供只读且 stale-aware 的 Online/Offline 门禁，覆盖拓扑基线不一致、Startup 和 Watch 偏差、I/O 变量映射/值缺口、PLC 交接质量问题、过期工程元数据、缺少工程别名，并提供选中行一致性详情预览、本地证据路由和边界说明，支持从一致性行打开状态机、Startup SDO、Watch 或 I/O 变量证据且不访问总线、下一最佳动作直达 I/O 问题复核、Free Run/状态推进前工作流门禁、Free Run/状态切换确认、OP 推荐约束和诊断报告。</li>
<li>Watch 支持 CiA 402 驱动预设、当前从站/变化项/基线偏离/启动不一致/缺失值/CiA 402 范围过滤、选中行数值详情预览、基线/偏差检查、Startup SDO 期望值对比、带确认的推荐控制字动作、驱动状态解析、多选批量生成 Startup SDO、带确认的 Watch 到 Startup 同步、优先更新已有启动行、缺失时才创建、类型保留、手动刷新、250 ms/500 ms/1 s/2 s 自动刷新和值变化高亮。</li>
<li>状态机工作区按从站展示当前状态、推荐下一状态、证据、驱动状态、Startup 偏差、PDO/Free Run 过程证据和风险提示，选中行会给出切换详情预览，支持行级确认状态请求、双击发送推荐状态，并采用保守的 PREOP/SAFEOP/OP 推进逻辑。</li>
<li>状态切换确认支持证据驱动：单从站切换显示证据完整度、驱动证据、Startup 偏差、PDO 映射问题和拓扑基线警告；全部从站切换显示当前状态分布和全局风险。</li>
<li>Startup SDO 支持批量参数写入、选中行本地详情预览、所选行校验/应用、Watch Value/Watch Delta 证据列、只看偏差审阅/聚焦、基于 Watch 偏差只应用不一致行、批量影响预览和当前值到目标值证据，并显示明确的读写边界、逐行应用状态和失败详情。</li>
<li>Free Run 支持过程映像遥测、启动影响预览、PDO 映射证据列、过滤、Changed only、选中行过程详情预览、Raw/Decoded 变化高亮、从 I/O Alias/运行时名称/PDO Map 名称/对象缓存/条目缓存稳定兜底 Name、显示名称来源、输入/输出边界标注，以及 CiA 402 meaning 解析。</li>
<li>Diagnostics 支持 EtherCAT 服务、设备节点、网卡驱动、firmware blacklist、DKMS 和建议命令检查。</li>
<li>ESI 仓库、工程备注、对象字典证据快照、对象书签和 I/O 变量元数据工程持久化、运行日志、可见 I/O 变量 CSV 导出、PLC 声明复制/导出、PLC 符号 CSV 导出，以及带主机、OD、Free Run、I/O 变量和一致性证据的 Markdown 诊断报告导出。</li>
</ul>
<h2>运行边界</h2>
<p>GUI 本身不是 EtherCAT 主站。它连接本机 <code>ecatd</code>，由 <code>ecatd</code> 把请求路由到 IgH 工具和 ecrt Free Run 路径。这样可以把在线读写、状态切换、主机检查、诊断导出等动作保留清晰的上下文和记录。</p>
<h2>产品目标</h2>
<p>NekoEcat Studio 的目标是成为比传统工具更顺手的现代工程站：高频总线操作排在前面，在线数据按任务组织，危险操作保留上下文，主机和运行时诊断统一放在诊断页。它服务于真实硬件调试，而不是只展示原始文本。</p>
<div class="warn"><b>安全边界：</b>写 SDO、应用 Startup SDO、切换 OP、启用 Free Run 都可能影响真实设备。执行前必须确认当前主站、选中从站、Index、SubIndex、类型、写入值和设备安全状态。</div>
</body>
</html>
)HTML");

  const QString html = finalizeDocumentationHtml(
      uiText(english, chinese), QCoreApplication::applicationVersion(),
      activeMasterName(), ecatdPath(), settings_.theme == "Light");
  browser->setHtml(html);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); // wire signal to slot

  layout->addWidget(title);
  layout->addWidget(tagline);
  layout->addWidget(browser, 1);
  layout->addWidget(buttons);
  dialog.exec();
}

