// Light/dark QSS theme application.
#include "LanguageManager.h"

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


// — Set the application stylesheet for the current theme (Light or Dark)
void MainWindow::applyTheme() {
  if (settings_.theme == "Light") {
    qApp->setStyleSheet(R"QSS(
            QWidget {
                color: #172033;
                selection-background-color: #2563eb;
                selection-color: #ffffff;
            }
            QMainWindow, QDialog, QMessageBox, QDockWidget {
                background: #f4f7fb;
            }
            QMenuBar {
                background: #ffffff;
                border-bottom: 1px solid #d9e1ec;
                padding: 3px 8px;
            }
            QMenuBar::item {
                background: transparent;
                border-radius: 7px;
                padding: 6px 10px;
                margin: 2px;
            }
            QMenuBar::item:selected {
                background: #eef4fb;
                color: #0f172a;
            }
            QMenu {
                background: #ffffff;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 6px;
            }
            QMenu::item {
                border-radius: 6px;
                padding: 7px 30px 7px 12px;
            }
            QMenu::item:selected {
                background: #e8f1ff;
                color: #12376f;
            }
            QMenu::separator {
                height: 1px;
                background: #d9e1ec;
                margin: 5px 8px;
            }
            QToolBar {
                background: #ffffff;
                border: 0;
                border-bottom: 1px solid #d9e1ec;
                spacing: 6px;
                padding: 8px 10px;
            }
            QToolBar::separator {
                background: #d9e1ec;
                width: 1px;
                margin: 6px 5px;
            }
            QLabel#toolbarLabel {
                color: #64748b;
                font-size: 12px;
                font-weight: 700;
                padding: 0 2px 0 10px;
            }
            QToolButton, QPushButton {
                background: #f7f9fc;
                color: #172033;
                border: 1px solid #d4deeb;
                border-radius: 8px;
                padding: 7px 12px;
                min-height: 28px;
            }
            QToolButton:hover, QPushButton:hover {
                background: #edf4ff;
                border-color: #9bb9e8;
            }
            QToolButton:pressed, QPushButton:pressed {
                background: #dbeafe;
                border-color: #2563eb;
            }
            QToolButton:checked {
                background: #2563eb;
                border-color: #2563eb;
                color: #ffffff;
            }
            QPushButton#nextBestActionButton {
                background: #2563eb;
                color: #ffffff;
                border-color: #1d4ed8;
                font-weight: 700;
            }
            QPushButton#nextBestActionButton[severity="ok"] {
                background: #16a34a;
                border-color: #15803d;
            }
            QPushButton#nextBestActionButton[severity="action"] {
                background: #2563eb;
                border-color: #1d4ed8;
            }
            QPushButton#nextBestActionButton[severity="warning"] {
                background: #f59e0b;
                color: #111827;
                border-color: #d97706;
            }
            QPushButton#nextBestActionButton[severity="error"] {
                background: #dc2626;
                border-color: #b91c1c;
            }
            QPushButton#nextBestActionButton[severity="neutral"] {
                background: #475569;
                border-color: #334155;
            }
            QPushButton#nextBestActionButton:hover {
                background: #1d4ed8;
                border-color: #1e40af;
            }
            QPushButton#nextBestActionButton[severity="ok"]:hover {
                background: #15803d;
                border-color: #166534;
            }
            QPushButton#nextBestActionButton[severity="action"]:hover {
                background: #1d4ed8;
                border-color: #1e40af;
            }
            QPushButton#nextBestActionButton[severity="warning"]:hover {
                background: #d97706;
                border-color: #b45309;
            }
            QPushButton#nextBestActionButton[severity="error"]:hover {
                background: #b91c1c;
                border-color: #991b1b;
            }
            QPushButton#nextBestActionButton[severity="neutral"]:hover {
                background: #334155;
                border-color: #1e293b;
            }
            QPushButton:disabled, QToolButton:disabled {
                background: #eef2f7;
                color: #94a3b8;
                border-color: #d9e1ec;
            }
            QComboBox, QLineEdit, QDoubleSpinBox {
                background: #ffffff;
                color: #172033;
                border: 1px solid #d4deeb;
                border-radius: 8px;
                padding: 6px 10px;
                min-height: 28px;
            }
            QComboBox:hover, QLineEdit:hover, QDoubleSpinBox:hover {
                border-color: #a8b8cf;
            }
            QComboBox:focus, QLineEdit:focus, QDoubleSpinBox:focus {
                border-color: #2563eb;
                background: #ffffff;
            }
            QLineEdit:read-only {
                background: #f8fafc;
                color: #64748b;
            }
            QComboBox::drop-down, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                border: 0;
                width: 28px;
            }
            QComboBox QAbstractItemView {
                background: #ffffff;
                border: 1px solid #d4deeb;
                border-radius: 8px;
                padding: 4px;
                outline: 0;
                selection-background-color: #e8f1ff;
                selection-color: #12376f;
            }
            QLabel#connectionPill {
                border-radius: 999px;
                padding: 6px 12px;
                font-weight: 700;
            }
            QLabel#connectionPill[state="pending"] {
                background: #fff7ed;
                color: #9a3412;
                border: 1px solid #fed7aa;
            }
            QLabel#connectionPill[state="connected"] {
                background: #ecfdf5;
                color: #047857;
                border: 1px solid #a7f3d0;
            }
            QLabel#connectionPill[state="disconnected"] {
                background: #fef2f2;
                color: #b91c1c;
                border: 1px solid #fecaca;
            }
            QLabel#heroTitle {
                color: #0f172a;
                font-size: 24px;
                font-weight: 700;
                padding: 6px 2px 4px 2px;
            }
            QLabel#paneTitle, QLabel#sectionTitle {
                color: #667085;
                font-size: 12px;
                font-weight: 700;
                padding: 2px 0;
            }
            QLabel#dialogTitle {
                color: #0f172a;
                font-size: 22px;
                font-weight: 700;
                padding: 2px 0 8px 0;
            }
            QFrame#metricCard {
                background: #ffffff;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
            }
            QFrame#metricCard:hover {
                background: #fbfdff;
                border-color: #b6c8df;
            }
            QFrame#metricCard[health="ready"] {
                border-color: #86efac;
            }
            QFrame#metricCard[health="warning"] {
                border-color: #fbbf24;
            }
            QFrame#metricCard[health="error"] {
                border-color: #fca5a5;
            }
            QFrame#metricCard[health="pending"] {
                border-color: #cbd5e1;
            }
            QLabel#metricTitle {
                color: #667085;
                font-size: 11px;
                font-weight: 700;
            }
            QLabel#metricValue {
                color: #0f172a;
                font-size: 22px;
                font-weight: 700;
            }
            QLabel#selectedSlaveName {
                color: #0f172a;
                font-size: 15px;
                font-weight: 700;
            }
            QLabel#sdoInspector {
                background: #f7f9fc;
                color: #475569;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 9px 12px;
                font-weight: 600;
            }
            QLabel#sdoInspector[state="ready"] {
                background: #ecfdf5;
                color: #047857;
                border-color: #a7f3d0;
            }
            QLabel#sdoInspector[state="warning"] {
                background: #fff7ed;
                color: #9a3412;
                border-color: #fed7aa;
            }
            QLabel#sdoInspector[state="blocked"] {
                background: #fef2f2;
                color: #b91c1c;
                border-color: #fecaca;
            }
            QTabWidget::pane {
                background: #ffffff;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                top: -1px;
            }
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
            QTabBar::tab:hover {
                background: #eef4fb;
                color: #1f2937;
            }
            QTabBar::tab:selected {
                background: #ffffff;
                color: #0f172a;
                border-color: #d9e1ec;
            }
            QTabBar QToolButton {
                background: #f8fafc;
                color: #475569;
                border: 1px solid #d9e1ec;
                border-radius: 6px;
                margin: 2px;
            }
            QTabBar QToolButton:hover {
                background: #eef4fb;
                color: #0f172a;
            }
            QTreeWidget, QTableWidget, QPlainTextEdit {
                background: #ffffff;
                color: #172033;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                alternate-background-color: #f7f9fc;
                gridline-color: #edf1f7;
            }
            QPlainTextEdit {
                font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace;
                padding: 8px;
            }
            QTableWidget::item, QTreeWidget::item {
                padding: 6px 8px;
            }
            QTableWidget::item:hover, QTreeWidget::item:hover {
                background: #eef4fb;
            }
            QTableWidget::item:selected, QTreeWidget::item:selected {
                background: #dbeafe;
                color: #0f172a;
            }
            QListWidget#commandList {
                background: #ffffff;
                color: #172033;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 6px;
                outline: 0;
            }
            QListWidget#commandList::item {
                border-radius: 7px;
                padding: 10px;
                margin: 2px;
            }
            QListWidget#commandList::item:hover {
                background: #eef4fb;
            }
            QListWidget#commandList::item:selected {
                background: #dbeafe;
                color: #0f172a;
            }
            QListWidget#commandList::item:disabled {
                color: #94a3b8;
            }
            QLabel#commandStats {
                color: #475569;
                background: #f8fafc;
                border: 1px solid #e2e8f0;
                border-radius: 8px;
                padding: 8px 10px;
                font-weight: 700;
            }
            QLabel#commandPreview {
                color: #334155;
                background: #f8fafc;
                border: 1px solid #d9e1ec;
                border-radius: 8px;
                padding: 10px 12px;
                font-weight: 700;
            }
            QLabel#commandPreview[safety="local"] {
                color: #166534;
                background: #f0fdf4;
                border-color: #bbf7d0;
            }
            QLabel#commandPreview[safety="online"] {
                color: #9a3412;
                background: #fff7ed;
                border-color: #fed7aa;
            }
            QLabel#commandPreview[safety="danger"] {
                color: #991b1b;
                background: #fef2f2;
                border-color: #fecaca;
            }
            QLabel#commandPreview[safety="host"] {
                color: #1d4ed8;
                background: #eff6ff;
                border-color: #bfdbfe;
            }
            QLabel#commandPreview[safety="file"] {
                color: #475569;
                background: #f1f5f9;
                border-color: #cbd5e1;
            }
            QLabel#commandPreview[safety="empty"] {
                color: #64748b;
                background: #f8fafc;
                border-color: #e2e8f0;
            }
            QHeaderView::section {
                background: #f0f4f9;
                color: #475569;
                border: 0;
                border-right: 1px solid #d9e1ec;
                border-bottom: 1px solid #d9e1ec;
                padding: 8px 10px;
                font-weight: 700;
            }
            QTableCornerButton::section {
                background: #f0f4f9;
                border: 0;
                border-right: 1px solid #d9e1ec;
                border-bottom: 1px solid #d9e1ec;
            }
            QSplitter::handle {
                background: #d9e1ec;
            }
            QSplitter::handle:horizontal {
                width: 1px;
                margin: 12px 3px;
            }
            QSplitter::handle:hover {
                background: #60a5fa;
            }
            QDockWidget {
                color: #172033;
                titlebar-close-icon: none;
                titlebar-normal-icon: none;
            }
            QDockWidget::title {
                background: #ffffff;
                border-top: 1px solid #d9e1ec;
                border-bottom: 1px solid #d9e1ec;
                color: #667085;
                padding: 8px 10px;
                text-align: left;
                font-weight: 700;
            }
            QStatusBar {
                background: #ffffff;
                border-top: 1px solid #d9e1ec;
                color: #475569;
            }
            QLabel#statusSummary {
                color: #475569;
                padding: 4px 8px;
            }
            QLabel#statusSummary[severity="neutral"] {
                color: #475569;
                background: #f1f5f9;
                border: 1px solid #d9e1ec;
                border-radius: 6px;
            }
            QLabel#statusSummary[severity="ok"] {
                color: #047857;
                background: #ecfdf5;
                border: 1px solid #a7f3d0;
                border-radius: 6px;
            }
            QLabel#statusSummary[severity="action"] {
                color: #1d4ed8;
                background: #eff6ff;
                border: 1px solid #bfdbfe;
                border-radius: 6px;
            }
            QLabel#statusSummary[severity="warning"] {
                color: #9a3412;
                background: #fff7ed;
                border: 1px solid #fed7aa;
                border-radius: 6px;
            }
            QLabel#statusSummary[severity="error"] {
                color: #b91c1c;
                background: #fef2f2;
                border: 1px solid #fecaca;
                border-radius: 6px;
            }
            QLabel#diagnosticsSummary {
                color: #475569;
                padding: 2px 4px;
                font-weight: 700;
            }
            QScrollBar:vertical {
                background: transparent;
                width: 10px;
                margin: 2px;
            }
            QScrollBar::handle:vertical {
                background: #c5cfdd;
                border-radius: 5px;
                min-height: 36px;
            }
            QScrollBar::handle:vertical:hover {
                background: #9aa9bd;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0;
            }
            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                background: transparent;
            }
            QScrollBar:horizontal {
                background: transparent;
                height: 10px;
                margin: 2px;
            }
            QScrollBar::handle:horizontal {
                background: #c5cfdd;
                border-radius: 5px;
                min-width: 36px;
            }
            QScrollBar::handle:horizontal:hover {
                background: #9aa9bd;
            }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0;
            }
            QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
                background: transparent;
            }
        )QSS");
    return;
  }

  qApp->setStyleSheet(R"QSS(
        QWidget {
            color: #e6edf5;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
        }
        QMainWindow, QDialog, QMessageBox, QDockWidget {
            background: #0e1117;
        }
        QMenuBar {
            background: #121722;
            border-bottom: 1px solid #263242;
            padding: 3px 8px;
        }
        QMenuBar::item {
            background: transparent;
            border-radius: 7px;
            padding: 6px 10px;
            margin: 2px;
        }
        QMenuBar::item:selected {
            background: #1c2533;
            color: #f8fafc;
        }
        QMenu {
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 6px;
        }
        QMenu::item {
            border-radius: 6px;
            padding: 7px 30px 7px 12px;
        }
        QMenu::item:selected {
            background: #20324d;
            color: #f8fafc;
        }
        QMenu::separator {
            height: 1px;
            background: #2a3546;
            margin: 5px 8px;
        }
        QToolBar {
            background: #121722;
            border: 0;
            border-bottom: 1px solid #263242;
            spacing: 6px;
            padding: 8px 10px;
        }
        QToolBar::separator {
            background: #2a3546;
            width: 1px;
            margin: 6px 5px;
        }
        QLabel#toolbarLabel {
            color: #91a1b6;
            font-size: 12px;
            font-weight: 700;
            padding: 0 2px 0 10px;
        }
        QToolButton, QPushButton {
            background: #1a2230;
            color: #e6edf5;
            border: 1px solid #303c50;
            border-radius: 8px;
            padding: 7px 12px;
            min-height: 28px;
        }
        QToolButton:hover, QPushButton:hover {
            background: #243149;
            border-color: #4c6a95;
        }
        QToolButton:pressed, QPushButton:pressed {
            background: #1d4ed8;
            border-color: #60a5fa;
        }
        QToolButton:checked {
            background: #2563eb;
            border-color: #60a5fa;
            color: #ffffff;
        }
        QPushButton#nextBestActionButton {
            background: #2563eb;
            color: #ffffff;
            border-color: #60a5fa;
            font-weight: 700;
        }
        QPushButton#nextBestActionButton[severity="ok"] {
            background: #15803d;
            border-color: #22c55e;
        }
        QPushButton#nextBestActionButton[severity="action"] {
            background: #2563eb;
            border-color: #60a5fa;
        }
        QPushButton#nextBestActionButton[severity="warning"] {
            background: #d97706;
            color: #111827;
            border-color: #fbbf24;
        }
        QPushButton#nextBestActionButton[severity="error"] {
            background: #b91c1c;
            border-color: #f87171;
        }
        QPushButton#nextBestActionButton[severity="neutral"] {
            background: #334155;
            border-color: #64748b;
        }
        QPushButton#nextBestActionButton:hover {
            background: #1d4ed8;
            border-color: #93c5fd;
        }
        QPushButton#nextBestActionButton[severity="ok"]:hover {
            background: #166534;
            border-color: #86efac;
        }
        QPushButton#nextBestActionButton[severity="action"]:hover {
            background: #1d4ed8;
            border-color: #93c5fd;
        }
        QPushButton#nextBestActionButton[severity="warning"]:hover {
            background: #b45309;
            border-color: #fdba74;
        }
        QPushButton#nextBestActionButton[severity="error"]:hover {
            background: #991b1b;
            border-color: #fca5a5;
        }
        QPushButton#nextBestActionButton[severity="neutral"]:hover {
            background: #1e293b;
            border-color: #94a3b8;
        }
        QPushButton:disabled, QToolButton:disabled {
            background: #161c25;
            color: #64748b;
            border-color: #263242;
        }
        QComboBox, QLineEdit, QDoubleSpinBox {
            background: #151b25;
            color: #e6edf5;
            border: 1px solid #303c50;
            border-radius: 8px;
            padding: 6px 10px;
            min-height: 28px;
        }
        QComboBox:hover, QLineEdit:hover, QDoubleSpinBox:hover {
            border-color: #4c6a95;
        }
        QComboBox:focus, QLineEdit:focus, QDoubleSpinBox:focus {
            border-color: #60a5fa;
            background: #151b25;
        }
        QLineEdit:read-only {
            background: #111722;
            color: #91a1b6;
        }
        QComboBox::drop-down, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            border: 0;
            width: 28px;
        }
        QComboBox QAbstractItemView {
            background: #151b25;
            border: 1px solid #303c50;
            border-radius: 8px;
            padding: 4px;
            outline: 0;
            selection-background-color: #20324d;
            selection-color: #f8fafc;
        }
        QLabel#connectionPill {
            border-radius: 999px;
            padding: 6px 12px;
            font-weight: 700;
        }
        QLabel#connectionPill[state="pending"] {
            background: #422006;
            color: #facc15;
            border: 1px solid #854d0e;
        }
        QLabel#connectionPill[state="connected"] {
            background: #052e24;
            color: #86efac;
            border: 1px solid #15803d;
        }
        QLabel#connectionPill[state="disconnected"] {
            background: #450a0a;
            color: #fca5a5;
            border: 1px solid #991b1b;
        }
        QLabel#heroTitle {
            color: #f8fafc;
            font-size: 24px;
            font-weight: 700;
            padding: 6px 2px 4px 2px;
        }
        QLabel#paneTitle, QLabel#sectionTitle {
            color: #91a1b6;
            font-size: 12px;
            font-weight: 700;
            padding: 2px 0;
        }
        QLabel#dialogTitle {
            color: #f8fafc;
            font-size: 22px;
            font-weight: 700;
            padding: 2px 0 8px 0;
        }
        QFrame#metricCard {
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
        }
        QFrame#metricCard:hover {
            background: #182232;
            border-color: #3f5778;
        }
        QFrame#metricCard[health="ready"] {
            border-color: #22c55e;
        }
        QFrame#metricCard[health="warning"] {
            border-color: #f59e0b;
        }
        QFrame#metricCard[health="error"] {
            border-color: #ef4444;
        }
        QFrame#metricCard[health="pending"] {
            border-color: #475569;
        }
        QLabel#metricTitle {
            color: #91a1b6;
            font-size: 11px;
            font-weight: 700;
        }
        QLabel#metricValue {
            color: #f8fafc;
            font-size: 22px;
            font-weight: 700;
        }
        QLabel#selectedSlaveName {
            color: #f8fafc;
            font-size: 15px;
            font-weight: 700;
        }
        QLabel#sdoInspector {
            background: #111722;
            color: #b9c6d6;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 9px 12px;
            font-weight: 600;
        }
        QLabel#sdoInspector[state="ready"] {
            background: #052e24;
            color: #86efac;
            border-color: #15803d;
        }
        QLabel#sdoInspector[state="warning"] {
            background: #422006;
            color: #facc15;
            border-color: #854d0e;
        }
        QLabel#sdoInspector[state="blocked"] {
            background: #450a0a;
            color: #fca5a5;
            border-color: #991b1b;
        }
        QTabWidget::pane {
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            top: -1px;
        }
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
        QTabBar::tab:hover {
            background: #1c2533;
            color: #f8fafc;
        }
        QTabBar::tab:selected {
            background: #151b25;
            color: #ffffff;
            border-color: #2a3546;
        }
        QTabBar QToolButton {
            background: #151b25;
            color: #b9c6d6;
            border: 1px solid #2a3546;
            border-radius: 6px;
            margin: 2px;
        }
        QTabBar QToolButton:hover {
            background: #1c2533;
            color: #ffffff;
        }
        QTreeWidget, QTableWidget, QPlainTextEdit {
            background: #151b25;
            color: #e6edf5;
            border: 1px solid #2a3546;
            border-radius: 8px;
            alternate-background-color: #111722;
            gridline-color: #263242;
        }
        QPlainTextEdit {
            font-family: "JetBrains Mono", "Cascadia Mono", "Consolas", monospace;
            padding: 8px;
        }
        QTableWidget::item, QTreeWidget::item {
            padding: 6px 8px;
        }
        QTableWidget::item:hover, QTreeWidget::item:hover {
            background: #1c2533;
        }
        QTableWidget::item:selected, QTreeWidget::item:selected {
            background: #20324d;
            color: #f8fafc;
        }
        QListWidget#commandList {
            background: #151b25;
            color: #e6edf5;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 6px;
            outline: 0;
        }
        QListWidget#commandList::item {
            border-radius: 7px;
            padding: 10px;
            margin: 2px;
        }
        QListWidget#commandList::item:hover {
            background: #1c2533;
        }
        QListWidget#commandList::item:selected {
            background: #20324d;
            color: #f8fafc;
        }
        QListWidget#commandList::item:disabled {
            color: #64748b;
        }
        QLabel#commandStats {
            color: #cbd5e1;
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 8px 10px;
            font-weight: 700;
        }
        QLabel#commandPreview {
            color: #dbeafe;
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 8px;
            padding: 10px 12px;
            font-weight: 700;
        }
        QLabel#commandPreview[safety="local"] {
            color: #86efac;
            background: #10251b;
            border-color: #166534;
        }
        QLabel#commandPreview[safety="online"] {
            color: #fdba74;
            background: #2b1d0f;
            border-color: #9a3412;
        }
        QLabel#commandPreview[safety="danger"] {
            color: #fca5a5;
            background: #2b1014;
            border-color: #991b1b;
        }
        QLabel#commandPreview[safety="host"] {
            color: #93c5fd;
            background: #10213d;
            border-color: #1d4ed8;
        }
        QLabel#commandPreview[safety="file"] {
            color: #cbd5e1;
            background: #1a2230;
            border-color: #334155;
        }
        QLabel#commandPreview[safety="empty"] {
            color: #94a3b8;
            background: #151b25;
            border-color: #2a3546;
        }
        QHeaderView::section {
            background: #1a2230;
            color: #b9c6d6;
            border: 0;
            border-right: 1px solid #2a3546;
            border-bottom: 1px solid #2a3546;
            padding: 8px 10px;
            font-weight: 700;
        }
        QTableCornerButton::section {
            background: #1a2230;
            border: 0;
            border-right: 1px solid #2a3546;
            border-bottom: 1px solid #2a3546;
        }
        QSplitter::handle {
            background: #263242;
        }
        QSplitter::handle:horizontal {
            width: 1px;
            margin: 12px 3px;
        }
        QSplitter::handle:hover {
            background: #60a5fa;
        }
        QDockWidget {
            color: #e6edf5;
            titlebar-close-icon: none;
            titlebar-normal-icon: none;
        }
        QDockWidget::title {
            background: #121722;
            border-top: 1px solid #263242;
            border-bottom: 1px solid #263242;
            color: #91a1b6;
            padding: 8px 10px;
            text-align: left;
            font-weight: 700;
        }
        QStatusBar {
            background: #121722;
            border-top: 1px solid #263242;
            color: #b9c6d6;
        }
        QLabel#statusSummary {
            color: #b9c6d6;
            padding: 4px 8px;
        }
        QLabel#statusSummary[severity="neutral"] {
            color: #cbd5e1;
            background: #151b25;
            border: 1px solid #2a3546;
            border-radius: 6px;
        }
        QLabel#statusSummary[severity="ok"] {
            color: #86efac;
            background: #10251b;
            border: 1px solid #166534;
            border-radius: 6px;
        }
        QLabel#statusSummary[severity="action"] {
            color: #93c5fd;
            background: #10213d;
            border: 1px solid #1d4ed8;
            border-radius: 6px;
        }
        QLabel#statusSummary[severity="warning"] {
            color: #fdba74;
            background: #2b1d0f;
            border: 1px solid #9a3412;
            border-radius: 6px;
        }
        QLabel#statusSummary[severity="error"] {
            color: #fca5a5;
            background: #2b1014;
            border: 1px solid #991b1b;
            border-radius: 6px;
        }
        QLabel#diagnosticsSummary {
            color: #b9c6d6;
            padding: 2px 4px;
            font-weight: 700;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 10px;
            margin: 2px;
        }
        QScrollBar::handle:vertical {
            background: #3a4658;
            border-radius: 5px;
            min-height: 36px;
        }
        QScrollBar::handle:vertical:hover {
            background: #53647a;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
        QScrollBar:horizontal {
            background: transparent;
            height: 10px;
            margin: 2px;
        }
        QScrollBar::handle:horizontal {
            background: #3a4658;
            border-radius: 5px;
            min-width: 36px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #53647a;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
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
  font.setPointSizeF(10.0 * settings_.scale);
  qApp->setFont(font);
  refreshMasterSelector();
  updateActionAvailability();
  updateStatusBar();
}

