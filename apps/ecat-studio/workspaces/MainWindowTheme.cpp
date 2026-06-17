// Light/dark QSS theme application and settings integration.
// Light/dark QSS theme application.
#include "LanguageManager.h"

#include "MainWindow.h"

#include "models/Cia402DriveModel.h"
#include "models/CommissioningWorkflowModel.h"
#include "detail/CommissioningWorkflowStepDetail.h"
#include "adapters/CommissioningWorkflowTableAdapter.h"
#include "detail/CommissioningWorkflowDetail.h"
#include "detail/ConsistencyDetail.h"
#include "models/ConsistencyModel.h"
#include "models/ConsistencyModel.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "detail/DiagnosticsEventDetail.h"
#include "models/EvidenceModel.h"
#include "detail/FreeRunEntryDetail.h"
#include "detail/HostHealthDetail.h"
#include "models/IoVariableBulkNamingModel.h"
#include "detail/IoVariableDetail.h"
#include "models/IoVariableFilterModel.h"
#include "models/IoVariableHandoffModel.h"
#include "models/NextBestActionModel.h"
#include "detail/NextBestActionDetail.h"
#include "detail/ObjectBookmarkDetail.h"
#include "detail/PdoMapDetail.h"
#include "models/ProcessDataRowModel.h"
#include "adapters/ProcessDataTableAdapter.h"
#include "adapters/SdoDictionaryTableAdapter.h"
#include "models/SdoEvidenceModel.h"
#include "adapters/SdoEvidenceTableAdapter.h"
#include "detail/SdoHistoryRowDetail.h"
#include "models/SdoTargetPanelRouteModel.h"
#include "detail/SdoTargetTrailDetail.h"
#include "detail/SelectedDriveSummaryDetail.h"
#include "detail/SelectedSlaveEvidenceSummaryDetail.h"
#include "models/SessionBriefModel.h"
#include "adapters/SessionBriefTableAdapter.h"
#include "detail/SessionBriefDetail.h"
#include "models/SlaveEvidenceModel.h"
#include "adapters/SlaveEvidenceTableAdapter.h"
#include "detail/SlaveEvidenceDetail.h"
#include "detail/StartupSdoRowDetail.h"
#include "detail/StateMachineRowDetail.h"
#include "adapters/StateMachineTableAdapter.h"
#include "models/EvidenceModel.h"
#include "utils/Documentation.h"
#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"
#include "utils/UiHelpers.h"
#include "models/TopologyModel.h"
#include "models/TopologyModel.h"
#include "detail/WatchRowDetail.h"
#include "models/WatchStartupModel.h"
#include "adapters/WatchStartupTableAdapter.h"
#include "detail/WatchStartupDetail.h"
#include "detail/WorkspaceBoundaryDetail.h"
#include "adapters/WorkspaceTabBadgeTableAdapter.h"
#include "detail/WorkspaceTabBadgeDetail.h"
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
    // Serialize/deserialize JSON data
#include <QJsonArray>
#include <QJsonDocument>
    // Serialize/deserialize JSON data
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
    // Schedule deferred or periodic execution
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QXmlStreamReader>

// ── Dark Theme ─────────────────────────────────────────────────────
// Apply the dark theme QSS to all widgets

// — Set the application stylesheet for the current theme (Light or Dark)
void MainWindow::applyTheme() {
  if (settings_.theme == "Light") {
    // Apply QSS stylesheet to widget
    qApp->setStyleSheet(R"QSS(
    // QSS rule for QWidget
            QWidget {
                color: #172033;
                selection-background-color: #2563eb;
                selection-color: #ffffff;
            }
    // QSS rule for QMainWindow, QDialog, QMessageBox, QDockWidget
            QMainWindow, QDialog, QMessageBox, QDockWidget {
                background: #f4f7fb;
            }
    // QSS rule for QMenuBar
            QMenuBar {
                background: #ffffff;
                border-bottom: 1px solid #d9e1ec;
                padding: 3px 8px;
            }
    // QSS rule for QMenuBar::item
            QMenuBar::item {
                background: transparent;
                border-radius: 7px;
                padding: 6px 10px;
                margin: 2px;
            }
    // QSS rule for QMenuBar::item:selected
            QMenuBar::item:selected {
                background: #eef4fb;
                color: #0f172a;
            }
    // QSS rule for QMenu
            QMenu {
                background: #ffffff;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 6px;
            }
    // QSS rule for QMenu::item
            QMenu::item {
                border-radius: 6px;
                padding: 7px 30px 7px 12px;
            }
    // QSS rule for QMenu::item:selected
            QMenu::item:selected {
                background: #e8f1ff;
                color: #12376f;
            }
    // QSS rule for QMenu::separator
            QMenu::separator {
                height: 1px;
                background: #d9e1ec;
                margin: 5px 8px;
            }
    // QSS rule for QToolBar
            QToolBar {
                background: #ffffff;
                border: 0;
                border-bottom: 1px solid #d9e1ec;
                spacing: 6px;
                padding: 8px 10px;
            }
    // QSS rule for QToolBar::separator
            QToolBar::separator {
                background: #d9e1ec;
                width: 1px;
                margin: 6px 5px;
            }
    // QSS rule for QLabel#toolbarLabel
            QLabel#toolbarLabel {
                color: #64748b;
                font-size: 12px;
                font-weight: 700;
                padding: 0 2px 0 10px;
            }
    // QSS rule for QToolButton, QPushButton
            QToolButton, QPushButton {
                background: #f7f9fc;
                color: #172033;
                border: 1px solid #d4deeb;
                border-radius: 8px;
                padding: 7px 12px;
                min-height: 28px;
            }
    // QSS rule for QToolButton:hover, QPushButton:hover
            QToolButton:hover, QPushButton:hover {
                background: #edf4ff;
                border-color: #9bb9e8;
            }
// ── Dark Theme QSS ───────────────────────────────────────────────────
            QToolButton:pressed, QPushButton:pressed {
                background: #dbeafe;
                border-color: #2563eb;
            }
    // QSS rule for QToolButton:checked
            QToolButton:checked {
                background: #2563eb;
                border-color: #2563eb;
                color: #ffffff;
            }
    // QSS rule for QPushButton#nextBestActionButton
            QPushButton#nextBestActionButton {
                background: #2563eb;
                color: #ffffff;
                border-color: #1d4ed8;
                font-weight: 700;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="ok"]
            QPushButton#nextBestActionButton[severity="ok"] {
                background: #16a34a;
                border-color: #15803d;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="action"]
            QPushButton#nextBestActionButton[severity="action"] {
                background: #2563eb;
                border-color: #1d4ed8;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="warning"]
            QPushButton#nextBestActionButton[severity="warning"] {
                background: #f59e0b;
                color: #111827;
                border-color: #d97706;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="error"]
            QPushButton#nextBestActionButton[severity="error"] {
                background: #dc2626;
                border-color: #b91c1c;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="neutral"]
            QPushButton#nextBestActionButton[severity="neutral"] {
                background: #475569;
                border-color: #334155;
            }
    // QSS rule for QPushButton#nextBestActionButton:hover
            QPushButton#nextBestActionButton:hover {
                background: #1d4ed8;
                border-color: #1e40af;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="ok"]:hover
            QPushButton#nextBestActionButton[severity="ok"]:hover {
                background: #15803d;
                border-color: #166534;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="action"]:hover
            QPushButton#nextBestActionButton[severity="action"]:hover {
                background: #1d4ed8;
                border-color: #1e40af;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="warning"]:hover
            QPushButton#nextBestActionButton[severity="warning"]:hover {
                background: #d97706;
                border-color: #b45309;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="error"]:hover
            QPushButton#nextBestActionButton[severity="error"]:hover {
                background: #b91c1c;
                border-color: #991b1b;
            }
    // QSS rule for QPushButton#nextBestActionButton[severity="neutral"]:hover
            QPushButton#nextBestActionButton[severity="neutral"]:hover {
                background: #334155;
                border-color: #1e293b;
            }
    // QSS rule for QPushButton:disabled, QToolButton:disabled
            QPushButton:disabled, QToolButton:disabled {
                background: #eef2f7;
                color: #94a3b8;
                border-color: #d9e1ec;
            }
    // QSS rule for QComboBox, QLineEdit, QDoubleSpinBox
            QComboBox, QLineEdit, QDoubleSpinBox {
                background: #ffffff;
                color: #172033;
                border: 1px solid #d4deeb;
                border-radius: 8px;
                padding: 6px 10px;
                min-height: 28px;
            }
    // QSS rule for QComboBox:hover, QLineEdit:hover, QDoubleSpinBox:hover
            QComboBox:hover, QLineEdit:hover, QDoubleSpinBox:hover {
                border-color: #a8b8cf;
            }
    // QSS rule for QComboBox:focus, QLineEdit:focus, QDoubleSpinBox:focus
            QComboBox:focus, QLineEdit:focus, QDoubleSpinBox:focus {
                border-color: #2563eb;
                background: #ffffff;
            }
    // QSS rule for QLineEdit:read-only
            QLineEdit:read-only {
                background: #f8fafc;
                color: #64748b;
            }
    // QSS rule for QComboBox::drop-down, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button
            QComboBox::drop-down, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                border: 0;
                width: 28px;
            }
    // QSS rule for QComboBox QAbstractItemView
            QComboBox QAbstractItemView {
                background: #ffffff;
                border: 1px solid #d4deeb;
                border-radius: 8px;
                padding: 4px;
                outline: 0;
                selection-background-color: #e8f1ff;
                selection-color: #12376f;
            }
    // QSS rule for QLabel#connectionPill
            QLabel#connectionPill {
                border-radius: 999px;
                padding: 6px 12px;
                font-weight: 700;
            }
    // QSS rule for QLabel#connectionPill[state="pending"]
            QLabel#connectionPill[state="pending"] {
                background: #fff7ed;
                color: #9a3412;
                border: 1px solid #fed7aa;
            }
    // QSS rule for QLabel#connectionPill[state="connected"]
            QLabel#connectionPill[state="connected"] {
                background: #ecfdf5;
                color: #047857;
                border: 1px solid #a7f3d0;
            }
    // QSS rule for QLabel#connectionPill[state="disconnected"]
            QLabel#connectionPill[state="disconnected"] {
                background: #fef2f2;
                color: #b91c1c;
                border: 1px solid #fecaca;
            }
    // QSS rule for QLabel#heroTitle
            QLabel#heroTitle {
                color: #0f172a;
                font-size: 24px;
                font-weight: 700;
                padding: 6px 2px 4px 2px;
            }
    // QSS rule for QLabel#paneTitle, QLabel#sectionTitle
            QLabel#paneTitle, QLabel#sectionTitle {
                color: #667085;
                font-size: 12px;
                font-weight: 700;
                padding: 2px 0;
            }
    // QSS rule for QLabel#dialogTitle
            QLabel#dialogTitle {
                color: #0f172a;
                font-size: 22px;
                font-weight: 700;
                padding: 2px 0 8px 0;
            }
    // QSS rule for QFrame#metricCard
            QFrame#metricCard {
                background: #ffffff;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
            }
    // QSS rule for QFrame#metricCard:hover
            QFrame#metricCard:hover {
                background: #fbfdff;
                border-color: #b6c8df;
            }
    // QSS rule for QFrame#metricCard[health="ready"]
            QFrame#metricCard[health="ready"] {
                border-color: #86efac;
            }
    // QSS rule for QFrame#metricCard[health="warning"]
            QFrame#metricCard[health="warning"] {
                border-color: #fbbf24;
            }
    // QSS rule for QFrame#metricCard[health="error"]
            QFrame#metricCard[health="error"] {
                border-color: #fca5a5;
            }
    // QSS rule for QFrame#metricCard[health="pending"]
            QFrame#metricCard[health="pending"] {
                border-color: #cbd5e1;
            }
    // QSS rule for QLabel#metricTitle
            QLabel#metricTitle {
                color: #667085;
                font-size: 11px;
                font-weight: 700;
            }
    // QSS rule for QLabel#metricValue
            QLabel#metricValue {
                color: #0f172a;
                font-size: 22px;
                font-weight: 700;
            }
    // QSS rule for QLabel#selectedSlaveName
            QLabel#selectedSlaveName {
                color: #0f172a;
                font-size: 15px;
                font-weight: 700;
            }
    // QSS rule for QLabel#sdoInspector
            QLabel#sdoInspector {
                background: #f7f9fc;
                color: #475569;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 9px 12px;
                font-weight: 600;
            }
    // QSS rule for QLabel#sdoInspector[state="ready"]
            QLabel#sdoInspector[state="ready"] {
                background: #ecfdf5;
                color: #047857;
                border-color: #a7f3d0;
            }
    // QSS rule for QLabel#sdoInspector[state="warning"]
            QLabel#sdoInspector[state="warning"] {
                background: #fff7ed;
                color: #9a3412;
                border-color: #fed7aa;
            }
    // QSS rule for QLabel#sdoInspector[state="blocked"]
            QLabel#sdoInspector[state="blocked"] {
                background: #fef2f2;
                color: #b91c1c;
                border-color: #fecaca;
            }
    // QSS rule for QTabWidget::pane
            QTabWidget::pane {
                background: #ffffff;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                top: -1px;
            }
    // QSS rule for QTabBar::tab
            QTabBar::tab {
                background: transparent;
                color: #667085;
                border: 1px solid transparent;
                border-bottom: 0;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                padding: 8px 11px;
                margin-right: 3px;
            }
    // QSS rule for QTabBar::tab:hover
            QTabBar::tab:hover {
                background: #eef4fb;
                color: #1f2937;
            }
    // QSS rule for QTabBar::tab:selected
            QTabBar::tab:selected {
                background: #ffffff;
                color: #0f172a;
                border-color: #d9e1ec;
            }
    // QSS rule for QTabBar QToolButton
            QTabBar QToolButton {
                background: #f8fafc;
                color: #475569;
                border: 1px solid #d9e1ec;
                border-radius: 6px;
                margin: 2px;
            }
    // QSS rule for QTabBar QToolButton:hover
            QTabBar QToolButton:hover {
                background: #eef4fb;
                color: #0f172a;
            }
    // QSS rule for QTreeWidget, QTableWidget, QPlainTextEdit
            QTreeWidget, QTableWidget, QPlainTextEdit {
                background: #ffffff;
                color: #172033;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                alternate-background-color: #f7f9fc;
                gridline-color: #edf1f7;
            }
    // QSS rule for QPlainTextEdit
            QPlainTextEdit {
                font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace;
                padding: 8px;
            }
    // QSS rule for QTableWidget::item, QTreeWidget::item
            QTableWidget::item, QTreeWidget::item {
                padding: 6px 8px;
            }
    // QSS rule for QTableWidget::item:hover, QTreeWidget::item:hover
            QTableWidget::item:hover, QTreeWidget::item:hover {
                background: #eef4fb;
            }
    // QSS rule for QTableWidget::item:selected, QTreeWidget::item:selected
            QTableWidget::item:selected, QTreeWidget::item:selected {
                background: #dbeafe;
                color: #0f172a;
            }
    // QSS rule for QListWidget#commandList
            QListWidget#commandList {
                background: #ffffff;
                color: #172033;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 6px;
                outline: 0;
            }
    // QSS rule for QListWidget#commandList::item
            QListWidget#commandList::item {
                border-radius: 7px;
                padding: 10px;
                margin: 2px;
            }
    // QSS rule for QListWidget#commandList::item:hover
            QListWidget#commandList::item:hover {
                background: #eef4fb;
            }
    // QSS rule for QListWidget#commandList::item:selected
            QListWidget#commandList::item:selected {
                background: #dbeafe;
                color: #0f172a;
            }
    // QSS rule for QListWidget#commandList::item:disabled
            QListWidget#commandList::item:disabled {
                color: #94a3b8;
            }
    // QSS rule for QLabel#commandStats
            QLabel#commandStats {
                color: #475569;
                background: #f8fafc;
                border: 1px solid #e2e8f0;
                border-radius: 8px;
                padding: 8px 10px;
                font-weight: 700;
            }
    // QSS rule for QLabel#commandPreview
            QLabel#commandPreview {
                color: #334155;
                background: #f8fafc;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 10px 12px;
                font-weight: 700;
            }
    // QSS rule for QLabel#commandPreview[safety="local"]
            QLabel#commandPreview[safety="local"] {
                color: #166534;
                background: #f0fdf4;
                border-color: #bbf7d0;
            }
    // QSS rule for QLabel#commandPreview[safety="online"]
            QLabel#commandPreview[safety="online"] {
                color: #9a3412;
// ── Light Theme ────────────────────────────────────────────────────
// Apply the light theme QSS to all widgets
                background: #fff7ed;
                border-color: #fed7aa;
            }
    // QSS rule for QLabel#commandPreview[safety="danger"]
            QLabel#commandPreview[safety="danger"] {
                color: #991b1b;
                background: #fef2f2;
                border-color: #fecaca;
            }
    // QSS rule for QLabel#commandPreview[safety="host"]
            QLabel#commandPreview[safety="host"] {
                color: #1d4ed8;
                background: #eff6ff;
                border-color: #bfdbfe;
            }
    // QSS rule for QLabel#commandPreview[safety="file"]
            QLabel#commandPreview[safety="file"] {
                color: #475569;
                background: #f1f5f9;
                border-color: #cbd5e1;
            }
    // QSS rule for QLabel#commandPreview[safety="empty"]
            QLabel#commandPreview[safety="empty"] {
                color: #64748b;
                background: #f8fafc;
                border-color: #e2e8f0;
            }
    // QSS rule for QHeaderView::section
            QHeaderView::section {
                background: #f0f4f9;
                color: #475569;
                border: 0;
                border-right: 1px solid #d9e1ec;
                border-bottom: 1px solid #d9e1ec;
                padding: 8px 10px;
                font-weight: 700;
            }
    // QSS rule for QTableCornerButton::section
            QTableCornerButton::section {
                background: #f0f4f9;
                border: 0;
                border-right: 1px solid #d9e1ec;
                border-bottom: 1px solid #d9e1ec;
            }
    // QSS rule for QSplitter::handle
            QSplitter::handle {
                background: #d9e1ec;
            }
    // QSS rule for QSplitter::handle:horizontal
            QSplitter::handle:horizontal {
                width: 1px;
                margin: 12px 3px;
            }
    // QSS rule for QSplitter::handle:hover
            QSplitter::handle:hover {
                background: #60a5fa;
            }
    // QSS rule for QDockWidget
            QDockWidget {
                color: #172033;
                titlebar-close-icon: none;
                titlebar-normal-icon: none;
            }
    // QSS rule for QDockWidget::title
            QDockWidget::title {
                background: #ffffff;
                border-top: 1px solid #d9e1ec;
                border-bottom: 1px solid #d9e1ec;
                color: #667085;
                padding: 8px 10px;
                text-align: left;
                font-weight: 700;
            }
    // QSS rule for QStatusBar
            QStatusBar {
                background: #ffffff;
                border-top: 1px solid #d9e1ec;
                color: #475569;
            }
    // QSS rule for QLabel#statusSummary
            QLabel#statusSummary {
                color: #475569;
                padding: 4px 8px;
            }
    // QSS rule for QLabel#statusSummary[severity="neutral"]
            QLabel#statusSummary[severity="neutral"] {
                color: #475569;
                background: #f1f5f9;
                border: 1px solid #d9e1ec;
                border-radius: 6px;
            }
    // QSS rule for QLabel#statusSummary[severity="ok"]
            QLabel#statusSummary[severity="ok"] {
                color: #047857;
                background: #ecfdf5;
                border: 1px solid #a7f3d0;
                border-radius: 6px;
            }
    // QSS rule for QLabel#statusSummary[severity="action"]
            QLabel#statusSummary[severity="action"] {
                color: #1d4ed8;
                background: #eff6ff;
                border: 1px solid #bfdbfe;
                border-radius: 6px;
            }
    // QSS rule for QLabel#statusSummary[severity="warning"]
            QLabel#statusSummary[severity="warning"] {
                color: #9a3412;
                background: #fff7ed;
                border: 1px solid #fed7aa;
                border-radius: 6px;
            }
    // QSS rule for QLabel#statusSummary[severity="error"]
            QLabel#statusSummary[severity="error"] {
                color: #b91c1c;
                background: #fef2f2;
                border: 1px solid #fecaca;
                border-radius: 6px;
            }
    // QSS rule for QLabel#diagnosticsSummary
            QLabel#diagnosticsSummary {
// ── Light Theme QSS ──────────────────────────────────────────────────
                color: #475569;
                padding: 2px 4px;
                font-weight: 700;
            }
    // QSS rule for QScrollBar:vertical
            QScrollBar:vertical {
                background: transparent;
                width: 10px;
                margin: 2px;
            }
    // QSS rule for QScrollBar::handle:vertical
            QScrollBar::handle:vertical {
                background: #c5cfdd;
                border-radius: 5px;
                min-height: 36px;
            }
    // QSS rule for QScrollBar::handle:vertical:hover
            QScrollBar::handle:vertical:hover {
                background: #9aa9bd;
            }
    // QSS rule for QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0;
            }
    // QSS rule for QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical
            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                background: transparent;
            }
    // QSS rule for QScrollBar:horizontal
            QScrollBar:horizontal {
                background: transparent;
                height: 10px;
                margin: 2px;
            }
    // QSS rule for QScrollBar::handle:horizontal
            QScrollBar::handle:horizontal {
                background: #c5cfdd;
                border-radius: 5px;
                min-width: 36px;
            }
    // QSS rule for QScrollBar::handle:horizontal:hover
            QScrollBar::handle:horizontal:hover {
                background: #9aa9bd;
            }
    // QSS rule for QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0;
            }
    // QSS rule for QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal
            QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
                background: transparent;
            }
        )QSS");
    return;
  }

    // Apply QSS stylesheet to widget
  qApp->setStyleSheet(R"QSS(
    // QSS rule for QWidget
        QWidget {
            color: #e6edf5;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
        }
    // QSS rule for QMainWindow, QDialog, QMessageBox, QDockWidget
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #0e1117;
        }
    // QSS rule for QMenuBar
        QMenuBar {
            background: #121722;
            border-bottom: 1px solid #263242;
            padding: 3px 8px;
        }
    // QSS rule for QMenuBar::item
        QMenuBar::item {
            background: transparent;
            border-radius: 7px;
            padding: 6px 10px;
            margin: 2px;
        }
    // QSS rule for QMenuBar::item:selected
        QMenuBar::item:selected {
            background: #1c2533;
            color: #f8fafc;
        }
    // QSS rule for QMenu
        QMenu {
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 6px;
        }
    // QSS rule for QMenu::item
        QMenu::item {
            border-radius: 6px;
            padding: 7px 30px 7px 12px;
        }
    // QSS rule for QMenu::item:selected
        QMenu::item:selected {
            background: #20324d;
            color: #f8fafc;
        }
    // QSS rule for QMenu::separator
        QMenu::separator {
            height: 1px;
            background: #2a3546;
            margin: 5px 8px;
        }
    // QSS rule for QToolBar
        QToolBar {
            background: #121722;
            border: 0;
            border-bottom: 1px solid #263242;
            spacing: 6px;
            padding: 8px 10px;
        }
    // QSS rule for QToolBar::separator
        QToolBar::separator {
            background: #2a3546;
            width: 1px;
            margin: 6px 5px;
        }
    // QSS rule for QLabel#toolbarLabel
        QLabel#toolbarLabel {
            color: #91a1b6;
            font-size: 12px;
            font-weight: 700;
            padding: 0 2px 0 10px;
        }
    // QSS rule for QToolButton, QPushButton
        QToolButton, QPushButton {
            background: #1a2230;
            color: #e6edf5;
            border: 1px solid #303c50;
            border-radius: 8px;
            padding: 7px 12px;
            min-height: 28px;
        }
    // QSS rule for QToolButton:hover, QPushButton:hover
        QToolButton:hover, QPushButton:hover {
            background: #243149;
            border-color: #4c6a95;
        }
    // QSS rule for QToolButton:pressed, QPushButton:pressed
        QToolButton:pressed, QPushButton:pressed {
            background: #1d4ed8;
            border-color: #60a5fa;
        }
    // QSS rule for QToolButton:checked
        QToolButton:checked {
            background: #2563eb;
            border-color: #60a5fa;
            color: #ffffff;
        }
    // QSS rule for QPushButton#nextBestActionButton
        QPushButton#nextBestActionButton {
            background: #2563eb;
            color: #ffffff;
            border-color: #60a5fa;
            font-weight: 700;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="ok"]
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #15803d;
            border-color: #22c55e;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="action"]
        QPushButton#nextBestActionButton[severity="action"] {
            background: #2563eb;
            border-color: #60a5fa;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="warning"]
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #d97706;
            color: #111827;
            border-color: #fbbf24;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="error"]
        QPushButton#nextBestActionButton[severity="error"] {
            background: #b91c1c;
            border-color: #f87171;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="neutral"]
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #334155;
            border-color: #64748b;
        }
    // QSS rule for QPushButton#nextBestActionButton:hover
        QPushButton#nextBestActionButton:hover {
            background: #1d4ed8;
            border-color: #93c5fd;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="ok"]:hover
        QPushButton#nextBestActionButton[severity="ok"]:hover {
            background: #166534;
            border-color: #86efac;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="action"]:hover
        QPushButton#nextBestActionButton[severity="action"]:hover {
            background: #1d4ed8;
            border-color: #93c5fd;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="warning"]:hover
        QPushButton#nextBestActionButton[severity="warning"]:hover {
            background: #b45309;
            border-color: #fdba74;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="error"]:hover
        QPushButton#nextBestActionButton[severity="error"]:hover {
            background: #991b1b;
            border-color: #fca5a5;
        }
    // QSS rule for QPushButton#nextBestActionButton[severity="neutral"]:hover
        QPushButton#nextBestActionButton[severity="neutral"]:hover {
            background: #1e293b;
            border-color: #94a3b8;
        }
    // QSS rule for QPushButton:disabled, QToolButton:disabled
        QPushButton:disabled, QToolButton:disabled {
            background: #161c25;
            color: #64748b;
            border-color: #263242;
        }
    // QSS rule for QComboBox, QLineEdit, QDoubleSpinBox
        QComboBox, QLineEdit, QDoubleSpinBox {
            background: #151b25;
            color: #e6edf5;
            border: 1px solid #303c50;
            border-radius: 8px;
            padding: 6px 10px;
            min-height: 28px;
        }
    // QSS rule for QComboBox:hover, QLineEdit:hover, QDoubleSpinBox:hover
        QComboBox:hover, QLineEdit:hover, QDoubleSpinBox:hover {
            border-color: #4c6a95;
        }
    // QSS rule for QComboBox:focus, QLineEdit:focus, QDoubleSpinBox:focus
        QComboBox:focus, QLineEdit:focus, QDoubleSpinBox:focus {
            border-color: #60a5fa;
            background: #151b25;
        }
    // QSS rule for QLineEdit:read-only
        QLineEdit:read-only {
            background: #111722;
            color: #91a1b6;
        }
    // QSS rule for QComboBox::drop-down, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button
        QComboBox::drop-down, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            border: 0;
            width: 28px;
        }
    // QSS rule for QComboBox QAbstractItemView
        QComboBox QAbstractItemView {
            background: #151b25;
            border: 1px solid #303c50;
            border-radius: 8px;
            padding: 4px;
            outline: 0;
            selection-background-color: #20324d;
            selection-color: #f8fafc;
        }
    // QSS rule for QLabel#connectionPill
        QLabel#connectionPill {
            border-radius: 999px;
            padding: 6px 12px;
            font-weight: 700;
        }
    // QSS rule for QLabel#connectionPill[state="pending"]
        QLabel#connectionPill[state="pending"] {
            background: #422006;
            color: #facc15;
            border: 1px solid #854d0e;
        }
    // QSS rule for QLabel#connectionPill[state="connected"]
        QLabel#connectionPill[state="connected"] {
            background: #052e24;
            color: #86efac;
            border: 1px solid #15803d;
        }
    // QSS rule for QLabel#connectionPill[state="disconnected"]
        QLabel#connectionPill[state="disconnected"] {
            background: #450a0a;
            color: #fca5a5;
            border: 1px solid #991b1b;
        }
    // QSS rule for QLabel#heroTitle
        QLabel#heroTitle {
            color: #f8fafc;
            font-size: 24px;
            font-weight: 700;
            padding: 6px 2px 4px 2px;
        }
    // QSS rule for QLabel#paneTitle, QLabel#sectionTitle
        QLabel#paneTitle, QLabel#sectionTitle {
            color: #91a1b6;
            font-size: 12px;
            font-weight: 700;
            padding: 2px 0;
        }
    // QSS rule for QLabel#dialogTitle
        QLabel#dialogTitle {
            color: #f8fafc;
            font-size: 22px;
            font-weight: 700;
            padding: 2px 0 8px 0;
        }
    // QSS rule for QFrame#metricCard
        QFrame#metricCard {
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
        }
    // QSS rule for QFrame#metricCard:hover
        QFrame#metricCard:hover {
            background: #182232;
            border-color: #3f5778;
        }
    // QSS rule for QFrame#metricCard[health="ready"]
        QFrame#metricCard[health="ready"] {
            border-color: #22c55e;
        }
    // QSS rule for QFrame#metricCard[health="warning"]
        QFrame#metricCard[health="warning"] {
            border-color: #f59e0b;
        }
    // QSS rule for QFrame#metricCard[health="error"]
        QFrame#metricCard[health="error"] {
            border-color: #ef4444;
        }
    // QSS rule for QFrame#metricCard[health="pending"]
        QFrame#metricCard[health="pending"] {
            border-color: #475569;
        }
    // QSS rule for QLabel#metricTitle
        QLabel#metricTitle {
            color: #91a1b6;
            font-size: 11px;
            font-weight: 700;
        }
    // QSS rule for QLabel#metricValue
        QLabel#metricValue {
            color: #f8fafc;
            font-size: 22px;
            font-weight: 700;
        }
    // QSS rule for QLabel#selectedSlaveName
        QLabel#selectedSlaveName {
            color: #f8fafc;
            font-size: 15px;
            font-weight: 700;
        }
    // QSS rule for QLabel#sdoInspector
        QLabel#sdoInspector {
            background: #111722;
            color: #b9c6d6;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 9px 12px;
            font-weight: 600;
// ── Common Theme Elements ───────────────────────────────────────────
// Update theme-aware widget properties after theme change
        }
    // QSS rule for QLabel#sdoInspector[state="ready"]
        QLabel#sdoInspector[state="ready"] {
            background: #052e24;
            color: #86efac;
            border-color: #15803d;
        }
    // QSS rule for QLabel#sdoInspector[state="warning"]
        QLabel#sdoInspector[state="warning"] {
            background: #422006;
            color: #facc15;
            border-color: #854d0e;
        }
    // QSS rule for QLabel#sdoInspector[state="blocked"]
        QLabel#sdoInspector[state="blocked"] {
            background: #450a0a;
            color: #fca5a5;
            border-color: #991b1b;
        }
    // QSS rule for QTabWidget::pane
        QTabWidget::pane {
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            top: -1px;
        }
    // QSS rule for QTabBar::tab
        QTabBar::tab {
            background: transparent;
            color: #91a1b6;
            border: 1px solid transparent;
            border-bottom: 0;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            padding: 8px 11px;
            margin-right: 3px;
        }
    // QSS rule for QTabBar::tab:hover
        QTabBar::tab:hover {
            background: #1c2533;
            color: #f8fafc;
        }
    // QSS rule for QTabBar::tab:selected
        QTabBar::tab:selected {
            background: #151b25;
            color: #ffffff;
            border-color: #2a3546;
        }
    // QSS rule for QTabBar QToolButton
        QTabBar QToolButton {
            background: #151b25;
            color: #b9c6d6;
            border: 1px solid #2a3546;
            border-radius: 6px;
            margin: 2px;
        }
    // QSS rule for QTabBar QToolButton:hover
        QTabBar QToolButton:hover {
            background: #1c2533;
            color: #ffffff;
        }
    // QSS rule for QTreeWidget, QTableWidget, QPlainTextEdit
        QTreeWidget, QTableWidget, QPlainTextEdit {
            background: #151b25;
            color: #e6edf5;
            border: 1px solid #2a3546;
            border-radius: 8px;
            alternate-background-color: #111722;
            gridline-color: #263242;
        }
    // QSS rule for QPlainTextEdit
        QPlainTextEdit {
            font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace;
            padding: 8px;
        }
    // QSS rule for QTableWidget::item, QTreeWidget::item
        QTableWidget::item, QTreeWidget::item {
            padding: 6px 8px;
        }
    // QSS rule for QTableWidget::item:hover, QTreeWidget::item:hover
        QTableWidget::item:hover, QTreeWidget::item:hover {
            background: #1c2533;
        }
    // QSS rule for QTableWidget::item:selected, QTreeWidget::item:selected
        QTableWidget::item:selected, QTreeWidget::item:selected {
            background: #20324d;
            color: #f8fafc;
        }
    // QSS rule for QListWidget#commandList
        QListWidget#commandList {
            background: #151b25;
            color: #e6edf5;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 6px;
            outline: 0;
        }
    // QSS rule for QListWidget#commandList::item
        QListWidget#commandList::item {
            border-radius: 7px;
            padding: 10px;
            margin: 2px;
        }
    // QSS rule for QListWidget#commandList::item:hover
        QListWidget#commandList::item:hover {
            background: #1c2533;
        }
    // QSS rule for QListWidget#commandList::item:selected
        QListWidget#commandList::item:selected {
            background: #20324d;
            color: #f8fafc;
        }
    // QSS rule for QListWidget#commandList::item:disabled
        QListWidget#commandList::item:disabled {
            color: #64748b;
        }
    // QSS rule for QLabel#commandStats
        QLabel#commandStats {
            color: #cbd5e1;
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 8px 10px;
// ── Common Theme Elements (shared between dark and light) ────────────
            font-weight: 700;
        }
    // QSS rule for QLabel#commandPreview
        QLabel#commandPreview {
            color: #dbeafe;
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 10px 12px;
            font-weight: 700;
        }
    // QSS rule for QLabel#commandPreview[safety="local"]
        QLabel#commandPreview[safety="local"] {
            color: #86efac;
            background: #10251b;
            border-color: #166534;
        }
    // QSS rule for QLabel#commandPreview[safety="online"]
        QLabel#commandPreview[safety="online"] {
            color: #fdba74;
            background: #2b1d0f;
            border-color: #9a3412;
        }
    // QSS rule for QLabel#commandPreview[safety="danger"]
        QLabel#commandPreview[safety="danger"] {
            color: #fca5a5;
            background: #2b1014;
            border-color: #991b1b;
        }
    // QSS rule for QLabel#commandPreview[safety="host"]
        QLabel#commandPreview[safety="host"] {
            color: #93c5fd;
            background: #10213d;
            border-color: #1d4ed8;
        }
    // QSS rule for QLabel#commandPreview[safety="file"]
        QLabel#commandPreview[safety="file"] {
            color: #cbd5e1;
            background: #1a2230;
            border-color: #334155;
        }
    // QSS rule for QLabel#commandPreview[safety="empty"]
        QLabel#commandPreview[safety="empty"] {
            color: #94a3b8;
            background: #151b25;
            border-color: #2a3546;
        }
    // QSS rule for QHeaderView::section
        QHeaderView::section {
            background: #1a2230;
            color: #b9c6d6;
            border: 0;
            border-right: 1px solid #2a3546;
            border-bottom: 1px solid #2a3546;
            padding: 8px 10px;
            font-weight: 700;
        }
    // QSS rule for QTableCornerButton::section
        QTableCornerButton::section {
            background: #1a2230;
            border: 0;
            border-right: 1px solid #2a3546;
            border-bottom: 1px solid #2a3546;
        }
    // QSS rule for QSplitter::handle
        QSplitter::handle {
            background: #263242;
        }
    // QSS rule for QSplitter::handle:horizontal
        QSplitter::handle:horizontal {
            width: 1px;
            margin: 12px 3px;
        }
    // QSS rule for QSplitter::handle:hover
        QSplitter::handle:hover {
            background: #60a5fa;
        }
    // QSS rule for QDockWidget
        QDockWidget {
            color: #e6edf5;
            titlebar-close-icon: none;
            titlebar-normal-icon: none;
        }
    // QSS rule for QDockWidget::title
        QDockWidget::title {
            background: #121722;
            border-top: 1px solid #263242;
            border-bottom: 1px solid #263242;
            color: #91a1b6;
            padding: 8px 10px;
            text-align: left;
            font-weight: 700;
        }
    // QSS rule for QStatusBar
        QStatusBar {
            background: #121722;
            border-top: 1px solid #263242;
            color: #b9c6d6;
        }
    // QSS rule for QLabel#statusSummary
        QLabel#statusSummary {
            color: #b9c6d6;
            padding: 4px 8px;
        }
    // QSS rule for QLabel#statusSummary[severity="neutral"]
        QLabel#statusSummary[severity="neutral"] {
            color: #cbd5e1;
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 6px;
        }
    // QSS rule for QLabel#statusSummary[severity="ok"]
        QLabel#statusSummary[severity="ok"] {
            color: #86efac;
            background: #10251b;
            border: 1px solid #166534;
            border-radius: 6px;
        }
// ── Widget-Specific Overrides ────────────────────────────────────────
        QLabel#statusSummary[severity="action"] {
            color: #93c5fd;
            background: #10213d;
            border: 1px solid #1d4ed8;
            border-radius: 6px;
        }
    // QSS rule for QLabel#statusSummary[severity="warning"]
        QLabel#statusSummary[severity="warning"] {
            color: #fdba74;
            background: #2b1d0f;
            border: 1px solid #9a3412;
            border-radius: 6px;
        }
    // QSS rule for QLabel#statusSummary[severity="error"]
        QLabel#statusSummary[severity="error"] {
            color: #fca5a5;
            background: #2b1014;
            border: 1px solid #991b1b;
            border-radius: 6px;
        }
    // QSS rule for QLabel#diagnosticsSummary
        QLabel#diagnosticsSummary {
            color: #b9c6d6;
            padding: 2px 4px;
            font-weight: 700;
        }
    // QSS rule for QScrollBar:vertical
        QScrollBar:vertical {
            background: transparent;
            width: 10px;
            margin: 2px;
        }
    // QSS rule for QScrollBar::handle:vertical
        QScrollBar::handle:vertical {
            background: #3a4658;
            border-radius: 5px;
            min-height: 36px;
        }
    // QSS rule for QScrollBar::handle:vertical:hover
        QScrollBar::handle:vertical:hover {
            background: #53647a;
        }
    // QSS rule for QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
    // QSS rule for QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
    // QSS rule for QScrollBar:horizontal
        QScrollBar:horizontal {
            background: transparent;
            height: 10px;
            margin: 2px;
        }
    // QSS rule for QScrollBar::handle:horizontal
        QScrollBar::handle:horizontal {
            background: #3a4658;
            border-radius: 5px;
            min-width: 36px;
        }
    // QSS rule for QScrollBar::handle:horizontal:hover
        QScrollBar::handle:horizontal:hover {
            background: #53647a;
        }
    // QSS rule for QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
    // QSS rule for QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: transparent;
        }
    )QSS");
}


// — Apply all user settings: theme, font scale, and refresh the status bar
void MainWindow::applySettings() {
  LanguageManager::instance().setCurrentLanguage(settings_.language);
  applyTheme();
  QFont font = qApp->font();
    // Configure font for visual hierarchy
  font.setPointSizeF(10.0 * settings_.scale);
  qApp->setFont(font);
  refreshMasterSelector();
  updateActionAvailability();
  updateStatusBar();
}

