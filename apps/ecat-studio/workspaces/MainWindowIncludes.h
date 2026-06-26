#pragma once

// MainWindowIncludes — shared precompiled include header for all MainWindow
// workspace partial implementations. Centralizes common repeated include
// blocks that would otherwise be duplicated across workspace .cpp files.
//
// Include this header at the top of any workspace partial .cpp file that
// implements MainWindow methods. It pulls in all model, adapter, detail,
// utility, infra, and Qt headers used across workspace implementations.

// ── MainWindow ───────────────────────────────────────────────────────
#include "MainWindow.h"

// ── Models ───────────────────────────────────────────────────────────
#include "models/Cia402DriveModel.h"
#include "models/CommissioningWorkflowModel.h"
#include "models/ConsistencyModel.h"
#include "models/EvidenceModel.h"
#include "models/IoVariableModel.h"
#include "models/NextBestActionModel.h"
#include "models/ProcessDataRowModel.h"
#include "models/SdoEvidenceModel.h"
#include "models/SessionBriefModel.h"
#include "models/SlaveEvidenceModel.h"
#include "models/TopologyModel.h"
#include "models/WatchStartupModel.h"

// ── Adapters ─────────────────────────────────────────────────────────
#include "adapters/ConsistencyTableAdapter.h"
#include "adapters/ProcessDataTableAdapter.h"
#include "adapters/SdoDictionaryTableAdapter.h"
#include "adapters/SdoEvidenceTableAdapter.h"
#include "adapters/SessionBriefTableAdapter.h"
#include "adapters/SlaveEvidenceTableAdapter.h"
#include "adapters/StateMachineTableAdapter.h"
#include "adapters/WatchStartupTableAdapter.h"
#include "adapters/WorkflowTableAdapter.h"
#include "adapters/WorkspaceTabBadgeTableAdapter.h"

// ── Detail panels ────────────────────────────────────────────────────
#include "detail/CommissioningWorkflowDetail.h"
#include "detail/ConsistencyDetail.h"
#include "detail/DiagnosticsEventDetail.h"
#include "detail/FreeRunEntryDetail.h"
#include "detail/HostHealthDetail.h"
#include "detail/IoVariableDetail.h"
#include "detail/NextBestActionDetail.h"
#include "detail/ObjectBookmarkDetail.h"
#include "detail/PdoMapDetail.h"
#include "detail/SdoHistoryRowDetail.h"
#include "detail/SdoTargetTrailDetail.h"
#include "detail/SelectedDriveSummaryDetail.h"
#include "detail/SessionBriefDetail.h"
#include "detail/SlaveEvidenceDetail.h"
#include "detail/SlaveEvidenceSummaryDetail.h"
#include "detail/StartupSdoRowDetail.h"
#include "detail/StateMachineRowDetail.h"
#include "detail/WatchRowDetail.h"
#include "detail/WatchStartupDetail.h"
#include "detail/WorkspaceBoundaryDetail.h"
#include "detail/WorkspaceTabBadgeDetail.h"
#include "detail/WorkflowStepDetail.h"
#include "detail/RealtimeChartDialog.h"


// ── Infrastructure ───────────────────────────────────────────────────
#include "infra/LanguageManager.h"
#include "services/SdoService.h"
#include "services/WatchService.h"
#include "services/TopologyService.h"
#include "services/ServiceContainer.h"
// ── Utilities ────────────────────────────────────────────────────────
#include "utils/Documentation.h"
#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"
#include "utils/UiHelpers.h"

// ── C++ standard library ─────────────────────────────────────────────
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>

// ── Qt Core ──────────────────────────────────────────────────────────
#include <QAbstractItemView>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QItemSelectionModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QKeySequence>
#include <QPoint>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QSignalBlocker>
#include <QSize>
#include <QSizePolicy>
#include <QStringList>
#include <QTimer>
#include <QVector>
#include <QXmlStreamReader>

// ── Qt Painting ───────────────────────────────────────────────────────
#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QResizeEvent>
// ── Qt Widgets ───────────────────────────────────────────────────────
#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QCheckBox>
#include <QClipboard>
#include <QColor>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QShortcut>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextStream>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
