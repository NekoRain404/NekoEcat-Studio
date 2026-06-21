# StartupSdoPlugin & Constructor Unification Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create a `StartupSdoPlugin` class that extracts Startup SDO UI from `MainWindow`, and unify `DcSyncPlugin`, `AlEventPlugin`, `SignalPlugin` constructors to accept `ServiceContainer*` instead of individual service pointers.

**Architecture:** Follow the established plugin pattern (see `OdPlugin`, `OverviewPlugin`) where each plugin owns its UI widgets and receives a `ServiceContainer*` at construction. MainWindow delegates to plugins via accessors. The `StartupSdoPlugin` will own the `startupSdoTable_` widget and provide accessors for MainWindow's business logic methods.

**Tech Stack:** C++17, Qt6 (Widgets), CMake

---

## File Structure

### New Files
- `plugins/startupsdo/StartupSdoPlugin.h` — Plugin header with `ServiceContainer*` constructor, table accessors, and UI helper methods
- `plugins/startupsdo/StartupSdoPlugin.cpp` — Plugin implementation: UI construction, table management, control state updates

### Modified Files
- `plugins/dcsync/DcSyncPlugin.h` — Change constructor to accept `ServiceContainer*`
- `plugins/dcsync/DcSyncPlugin.cpp` — Extract services from container in constructor
- `plugins/alevent/AlEventPlugin.h` — Change constructor to accept `ServiceContainer*`
- `plugins/alevent/AlEventPlugin.cpp` — Extract services from container in constructor
- `plugins/signal/SignalPlugin.h` — Change constructor to accept `ServiceContainer*`
- `plugins/signal/SignalPlugin.cpp` — Extract services from container in constructor
- `MainWindow.h` — Add `StartupSdoPlugin*` member, remove `startupSdoTable_` member
- `MainWindow.cpp` — Register `StartupSdoPlugin`, update constructor calls for unified plugins
- `workspaces/MainWindowStartupSdoWorkspace.cpp` — Delegate table access to plugin
- `workspaces/MainWindowUiBuild.cpp` — Update startup SDO page construction to use plugin
- `CMakeLists.txt` — Add `StartupSdoPlugin.h/cpp` to build

---

## Task 1: Unify DcSyncPlugin Constructor to ServiceContainer*

**Files:**
- Modify: `plugins/dcsync/DcSyncPlugin.h:18-19`
- Modify: `plugins/dcsync/DcSyncPlugin.cpp:13-22`
- Modify: `MainWindow.cpp:223`

- [ ] **Step 1: Update DcSyncPlugin.h constructor signature**

```cpp
// plugins/dcsync/DcSyncPlugin.h — change lines 18-19
class QTableWidget;
class ServiceContainer;
class EventBus;
class DcSyncService;

class DcSyncPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DcSyncPlugin(ServiceContainer *container,
                        QObject *parent = nullptr);
```

- [ ] **Step 2: Update DcSyncPlugin.cpp constructor implementation**

```cpp
// plugins/dcsync/DcSyncPlugin.cpp — change lines 1-22
#include "DcSyncPlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "services/DcSyncService.h"

#include <QHeaderView>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QVBoxLayout>

static constexpr int kColCount = 8;

DcSyncPlugin::DcSyncPlugin(ServiceContainer *container, QObject *parent)
    : bus_(container->eventBus()), service_(container->dcSync()) {
  if (parent) setParent(parent);
  buildUi();

  connect(bus_, &EventBus::dcSyncUpdate, this,
          &DcSyncPlugin::handleDcSyncUpdate);
}
```

- [ ] **Step 3: Update MainWindow.cpp registration call**

```cpp
// MainWindow.cpp line 223 — change from:
pluginRegistry_.registerPlugin(new DcSyncPlugin(container_->eventBus(), container_->dcSync(), this));
// to:
pluginRegistry_.registerPlugin(new DcSyncPlugin(container_, this));
```

- [ ] **Step 4: Build and verify compilation**

Run: `cd build && cmake --build . --target ecat-studio 2>&1 | tail -20`
Expected: No errors related to DcSyncPlugin

- [ ] **Step 5: Commit**

```bash
git add apps/ecat-studio/plugins/dcsync/DcSyncPlugin.h apps/ecat-studio/plugins/dcsync/DcSyncPlugin.cpp apps/ecat-studio/MainWindow.cpp
git commit -m "refactor: unify DcSyncPlugin constructor to use ServiceContainer*"
```

---

## Task 2: Unify AlEventPlugin Constructor to ServiceContainer*

**Files:**
- Modify: `plugins/alevent/AlEventPlugin.h:20-21`
- Modify: `plugins/alevent/AlEventPlugin.cpp:24-33`
- Modify: `MainWindow.cpp:224`

- [ ] **Step 1: Update AlEventPlugin.h constructor signature**

```cpp
// plugins/alevent/AlEventPlugin.h — change forward declarations and constructor
class QTableWidget;
class QComboBox;
class ServiceContainer;
class EventBus;
class AlEventService;

class AlEventPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit AlEventPlugin(ServiceContainer *container,
                         QObject *parent = nullptr);
```

- [ ] **Step 2: Update AlEventPlugin.cpp constructor implementation**

```cpp
// plugins/alevent/AlEventPlugin.cpp — change lines 1-33
#include "AlEventPlugin.h"
#include "services/ServiceContainer.h"
#include "services/AlEventService.h"
#include "services/EventBus.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

static constexpr int kColTime        = 0;
static constexpr int kColSlave       = 1;
static constexpr int kColName        = 2;
static constexpr int kColCode        = 3;
static constexpr int kColSeverity    = 4;
static constexpr int kColDescription = 5;
static constexpr int kColCount       = 6;

AlEventPlugin::AlEventPlugin(ServiceContainer *container, QObject *parent)
    : bus_(container->eventBus()), service_(container->alEvent()) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &AlEventService::alEventUpdate, this,
          &AlEventPlugin::handleAlEventUpdate);
}
```

- [ ] **Step 3: Update MainWindow.cpp registration call**

```cpp
// MainWindow.cpp line 224 — change from:
pluginRegistry_.registerPlugin(new AlEventPlugin(container_->eventBus(), container_->alEvent(), this));
// to:
pluginRegistry_.registerPlugin(new AlEventPlugin(container_, this));
```

- [ ] **Step 4: Build and verify compilation**

Run: `cd build && cmake --build . --target ecat-studio 2>&1 | tail -20`
Expected: No errors related to AlEventPlugin

- [ ] **Step 5: Commit**

```bash
git add apps/ecat-studio/plugins/alevent/AlEventPlugin.h apps/ecat-studio/plugins/alevent/AlEventPlugin.cpp apps/ecat-studio/MainWindow.cpp
git commit -m "refactor: unify AlEventPlugin constructor to use ServiceContainer*"
```

---

## Task 3: Unify SignalPlugin Constructor to ServiceContainer*

**Files:**
- Modify: `plugins/signal/SignalPlugin.h:18`
- Modify: `plugins/signal/SignalPlugin.cpp:18-32`
- Modify: `MainWindow.cpp:225`

- [ ] **Step 1: Update SignalPlugin.h constructor signature**

```cpp
// plugins/signal/SignalPlugin.h — change forward declaration and constructor
class QListWidget;
class QComboBox;
class QLabel;
class SignalChartWidget;
class ServiceContainer;
class SignalService;

class SignalPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit SignalPlugin(ServiceContainer *container,
                        QObject *parent = nullptr);
```

- [ ] **Step 2: Update SignalPlugin.cpp constructor implementation**

```cpp
// plugins/signal/SignalPlugin.cpp — change lines 1-32
#include "SignalPlugin.h"
#include "SignalChartWidget.h"
#include "services/ServiceContainer.h"
#include "services/SignalService.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

SignalPlugin::SignalPlugin(ServiceContainer *container, QObject *parent)
    : service_(container->signal()) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &SignalService::channelDataUpdated,
          this, &SignalPlugin::refreshChart);
  connect(service_, &SignalService::channelDataUpdated,
          this, &SignalPlugin::updateStatsOverlay);
  connect(service_, &SignalService::channelAdded,
          this, [this](int) { refreshChart(); });
  connect(service_, &SignalService::channelRemoved,
          this, [this](int) { refreshChart(); });
}
```

- [ ] **Step 3: Update MainWindow.cpp registration call**

```cpp
// MainWindow.cpp line 225 — change from:
pluginRegistry_.registerPlugin(new SignalPlugin(container_->signal(), this));
// to:
pluginRegistry_.registerPlugin(new SignalPlugin(container_, this));
```

- [ ] **Step 4: Build and verify compilation**

Run: `cd build && cmake --build . --target ecat-studio 2>&1 | tail -20`
Expected: No errors related to SignalPlugin

- [ ] **Step 5: Commit**

```bash
git add apps/ecat-studio/plugins/signal/SignalPlugin.h apps/ecat-studio/plugins/signal/SignalPlugin.cpp apps/ecat-studio/MainWindow.cpp
git commit -m "refactor: unify SignalPlugin constructor to use ServiceContainer*"
```

---

## Task 4: Create StartupSdoPlugin Header

**Files:**
- Create: `plugins/startupsdo/StartupSdoPlugin.h`

- [ ] **Step 1: Create the StartupSdoPlugin header**

```cpp
#pragma once

// StartupSdoPlugin — workspace plugin for the Startup SDO management page.
// Extracted from MainWindowStartupSdoWorkspace.cpp. Owns the startup SDO table
// and provides accessors for MainWindow's business logic methods.

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QTableWidget;
class QPushButton;
class QCheckBox;
class ServiceContainer;

class StartupSdoPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit StartupSdoPlugin(ServiceContainer *container,
                            QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  // ── Table Accessors ──────────────────────────────────────────────
  QTableWidget *startupSdoTable() const;

  // ── Control Widgets ──────────────────────────────────────────────
  QCheckBox *startupWatchDiffsOnly() const;
  QLabel *startupWatchSummaryLabel() const;
  QLabel *startupSdoDetailLabel() const;

  // ── Table Management ─────────────────────────────────────────────
  void ensureStartupSdoTable();
  void updateStartupSdoControls(bool connected);
  void filterStartupSdoTable(bool diffsOnly);

signals:
  void startupSdoTableSelectionChanged();

private:
  void buildUi();
  void buildToolbar(QWidget *parent);
  void buildTable(QWidget *parent);

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;

  // Toolbar
  QCheckBox *startupWatchDiffsOnly_ = nullptr;
  QLabel *startupWatchSummaryLabel_ = nullptr;
  QLabel *startupSdoDetailLabel_ = nullptr;

  // Table
  QTableWidget *startupSdoTable_ = nullptr;
};
```

- [ ] **Step 2: Commit the header**

```bash
git add apps/ecat-studio/plugins/startupsdo/StartupSdoPlugin.h
git commit -m "feat: add StartupSdoPlugin header"
```

---

## Task 5: Create StartupSdoPlugin Implementation

**Files:**
- Create: `plugins/startupsdo/StartupSdoPlugin.cpp`

- [ ] **Step 1: Create the StartupSdoPlugin implementation**

```cpp
#include "StartupSdoPlugin.h"
#include "services/ServiceContainer.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

StartupSdoPlugin::StartupSdoPlugin(ServiceContainer *container,
                                   QObject *parent)
    : container_(container) {
  if (parent) setParent(parent);
  buildUi();
}

// ── Identity ──────────────────────────────────────────────────────────
QString StartupSdoPlugin::id() const { return "startupsdo"; }
QString StartupSdoPlugin::displayName() const { return "Startup SDO"; }
QString StartupSdoPlugin::displayNameZh() const {
  return QStringLiteral("启动SDO");
}
int StartupSdoPlugin::defaultOrder() const { return 35; }
bool StartupSdoPlugin::visible() const { return true; }

QWidget *StartupSdoPlugin::widget() { return containerWidget_; }

// ── Accessors ─────────────────────────────────────────────────────────
QTableWidget *StartupSdoPlugin::startupSdoTable() const {
  return startupSdoTable_;
}

QCheckBox *StartupSdoPlugin::startupWatchDiffsOnly() const {
  return startupWatchDiffsOnly_;
}

QLabel *StartupSdoPlugin::startupWatchSummaryLabel() const {
  return startupWatchSummaryLabel_;
}

QLabel *StartupSdoPlugin::startupSdoDetailLabel() const {
  return startupSdoDetailLabel_;
}

// ── UI construction ───────────────────────────────────────────────────
void StartupSdoPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *toolbarWidget = new QWidget;
  buildToolbar(toolbarWidget);
  mainLayout->addWidget(toolbarWidget);

  auto *tableWidget = new QWidget;
  buildTable(tableWidget);
  mainLayout->addWidget(tableWidget, 1);

  connect(startupSdoTable_, &QTableWidget::currentCellChanged, this,
          &StartupSdoPlugin::startupSdoTableSelectionChanged);
}

void StartupSdoPlugin::buildToolbar(QWidget *parent) {
  auto *layout = new QHBoxLayout(parent);
  layout->setContentsMargins(4, 2, 4, 2);

  startupWatchDiffsOnly_ = new QCheckBox(tr("Diffs Only"));
  layout->addWidget(startupWatchDiffsOnly_);

  startupWatchSummaryLabel_ = new QLabel;
  layout->addWidget(startupWatchSummaryLabel_);

  layout->addStretch();

  startupSdoDetailLabel_ = new QLabel;
  startupSdoDetailLabel_->setWordWrap(true);
  layout->addWidget(startupSdoDetailLabel_);
}

void StartupSdoPlugin::buildTable(QWidget *parent) {
  auto *layout = new QVBoxLayout(parent);
  layout->setContentsMargins(0, 0, 0, 0);

  startupSdoTable_ = new QTableWidget;
  startupSdoTable_->setColumnCount(9);
  startupSdoTable_->setHorizontalHeaderLabels(
      {tr("Slave"), tr("Index"), tr("Sub"), tr("Value"), tr("Type"),
       tr("Status"), tr("Detail"), tr("Watch Value"), tr("Watch Delta")});
  startupSdoTable_->horizontalHeader()->setStretchLastSection(true);
  startupSdoTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  startupSdoTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  startupSdoTable_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  layout->addWidget(startupSdoTable_);
}

// ── Table Management ──────────────────────────────────────────────────
void StartupSdoPlugin::ensureStartupSdoTable() {
  if (!startupSdoTable_) return;
  if (startupSdoTable_->columnCount() != 9) {
    startupSdoTable_->setColumnCount(9);
  }
  startupSdoTable_->setHorizontalHeaderLabels(
      {tr("Slave"), tr("Index"), tr("Sub"), tr("Value"), tr("Type"),
       tr("Status"), tr("Detail"), tr("Watch Value"), tr("Watch Delta")});
  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    for (int col = 0; col < startupSdoTable_->columnCount(); ++col) {
      if (!startupSdoTable_->item(row, col)) {
        startupSdoTable_->setItem(row, col, new QTableWidgetItem);
      }
    }
  }
}

void StartupSdoPlugin::updateStartupSdoControls(bool connected) {
  if (!startupSdoTable_) return;
  const int rows = startupSdoTable_->rowCount();
  const int row = startupSdoTable_->currentRow();
  const bool hasCurrentVisibleRow =
      row >= 0 && row < rows && !startupSdoTable_->isRowHidden(row);

  QVector<int> selectedRows;
  for (int r = 0; r < rows; ++r) {
    if (startupSdoTable_->selectionModel()->isRowSelected(r, QModelIndex())) {
      selectedRows.append(r);
    }
  }
  const bool hasSelectedRows = !selectedRows.isEmpty();

  auto setEnabled = [this](const char *name, bool enabled) {
    if (auto *button = containerWidget_->findChild<QPushButton *>(name)) {
      button->setEnabled(enabled);
    }
  };
  setEnabled("removeStartupSdo", hasSelectedRows);
  setEnabled("moveStartupSdoUp", hasCurrentVisibleRow && row > 0);
  setEnabled("moveStartupSdoDown", hasCurrentVisibleRow && row < rows - 1);
  setEnabled("preflightStartupSdo", rows > 0);
  setEnabled("verifyStartupSdo", connected && rows > 0);
  setEnabled("verifySelectedStartupSdo", connected && hasSelectedRows);
  setEnabled("applyStartupSdo", connected && rows > 0);
  setEnabled("applySelectedStartupSdo", connected && hasSelectedRows);

  if (startupWatchDiffsOnly_) {
    startupWatchDiffsOnly_->setEnabled(rows > 0);
  }
}

void StartupSdoPlugin::filterStartupSdoTable(bool diffsOnly) {
  if (!startupSdoTable_) return;
  // The actual filtering logic remains in MainWindow since it depends on
  // watch data. This method is called by MainWindow after filtering.
  Q_UNUSED(diffsOnly);
}
```

- [ ] **Step 2: Commit the implementation**

```bash
git add apps/ecat-studio/plugins/startupsdo/StartupSdoPlugin.cpp
git commit -m "feat: add StartupSdoPlugin implementation"
```

---

## Task 6: Register StartupSdoPlugin in CMakeLists.txt and MainWindow

**Files:**
- Modify: `CMakeLists.txt:14-31`
- Modify: `MainWindow.h` — Add `StartupSdoPlugin*` forward declaration and member
- Modify: `MainWindow.cpp` — Register plugin and store pointer

- [ ] **Step 1: Add StartupSdoPlugin to CMakeLists.txt**

```cmake
# CMakeLists.txt — add after line 30 (plugins/od/OdPlugin.h plugins/od/OdPlugin.cpp)
    plugins/startupsdo/StartupSdoPlugin.h        plugins/startupsdo/StartupSdoPlugin.cpp
```

- [ ] **Step 2: Add StartupSdoPlugin forward declaration to MainWindow.h**

```cpp
// MainWindow.h — add after line 33 (class NotesPlugin;)
class StartupSdoPlugin;
```

- [ ] **Step 3: Add StartupSdoPlugin member to MainWindow.h**

```cpp
// MainWindow.h — add after line 802 (NotesPlugin *notesPlugin_ = nullptr;)
  StartupSdoPlugin *startupSdoPlugin_ = nullptr;
```

- [ ] **Step 4: Register StartupSdoPlugin in MainWindow.cpp**

```cpp
// MainWindow.cpp — add after line 226 (pluginRegistry_.registerPlugin(new NotesPlugin(this));)
  pluginRegistry_.registerPlugin(new StartupSdoPlugin(container_, this));
```

- [ ] **Step 5: Store StartupSdoPlugin pointer in MainWindow.cpp**

```cpp
// MainWindow.cpp — add after line 242 (notesPlugin_ = qobject_cast<NotesPlugin *>(...);)
  startupSdoPlugin_ = qobject_cast<StartupSdoPlugin *>(pluginRegistry_.findById("startupsdo"));
```

- [ ] **Step 6: Add include for StartupSdoPlugin in MainWindow.cpp**

```cpp
// MainWindow.cpp — add near the top includes
#include "plugins/startupsdo/StartupSdoPlugin.h"
```

- [ ] **Step 7: Build and verify compilation**

Run: `cd build && cmake --build . --target ecat-studio 2>&1 | tail -20`
Expected: No errors

- [ ] **Step 8: Commit**

```bash
git add apps/ecat-studio/CMakeLists.txt apps/ecat-studio/MainWindow.h apps/ecat-studio/MainWindow.cpp
git commit -m "feat: register StartupSdoPlugin in build system and MainWindow"
```

---

## Task 7: Migrate startupSdoTable_ Ownership to StartupSdoPlugin

**Files:**
- Modify: `MainWindow.h:673` — Remove `startupSdoTable_` member
- Modify: `workspaces/MainWindowUiBuild.cpp` — Use plugin's table instead of creating directly
- Modify: `workspaces/MainWindowStartupSdoWorkspace.cpp` — Delegate to plugin's table

- [ ] **Step 1: Update MainWindow.h to remove startupSdoTable_ member**

Remove or comment out line 673:
```cpp
// QTableWidget *startupSdoTable_ = nullptr;  // REMOVED — now owned by StartupSdoPlugin
```

- [ ] **Step 2: Update MainWindowUiBuild.cpp to use plugin's table**

Find where `startupSdoTable_` is created in `MainWindowUiBuild.cpp` and replace with plugin delegation. The startup SDO page should use `startupSdoPlugin_->widget()` as its content.

Search for `startupSdoTable_` creation in MainWindowUiBuild.cpp and replace the page construction to use the plugin's widget.

- [ ] **Step 3: Update MainWindowStartupSdoWorkspace.cpp to use plugin's table**

Replace all occurrences of `startupSdoTable_` with `startupSdoPlugin_->startupSdoTable()` in `MainWindowStartupSdoWorkspace.cpp`.

Key methods to update:
- `ensureStartupSdoTable()` — delegate to `startupSdoPlugin_->ensureStartupSdoTable()`
- `updateStartupSdoWatchEvidence()` — use `startupSdoPlugin_->startupSdoTable()`
- `filterStartupSdoTable()` — use plugin's table
- `updateStartupSdoRowDetail()` — use plugin's table
- `focusStartupSdoWatchDiffs()` — use plugin's table
- `addStartupSdo()` — use plugin's table
- `removeStartupSdo()` — use plugin's table
- `moveStartupSdoRow()` — use plugin's table
- `verifyStartupSdoList()` — use plugin's table
- `verifyStartupSdoRow()` — use plugin's table
- `verifySelectedStartupSdoRows()` — use plugin's table
- `addStartupSdoRowToWatch()` — use plugin's table
- `preflightStartupSdoList()` — use plugin's table
- `updateStartupSdoControls()` — delegate to plugin
- `applyStartupSdoRow()` — use plugin's table
- `applySelectedStartupSdoRows()` — use plugin's table
- `applyStartupSdoRows()` — use plugin's table
- `startupSdoRowsWithWatchDiffs()` — use plugin's table
- `applyStartupSdoWatchDiffRows()` — use plugin's table
- `applyStartupSdoList()` — use plugin's table

- [ ] **Step 4: Update watch_ widget references**

The `watch_->startupWatchDiffsOnly`, `watch_->startupWatchSummaryLabel`, and `watch_->startupSdoDetailLabel` should now be accessed via `startupSdoPlugin_->startupWatchDiffsOnly()`, `startupSdoPlugin_->startupWatchSummaryLabel()`, and `startupSdoPlugin_->startupSdoDetailLabel()`.

- [ ] **Step 5: Build and verify compilation**

Run: `cd build && cmake --build . --target ecat-studio 2>&1 | tail -30`
Expected: No errors

- [ ] **Step 6: Commit**

```bash
git add apps/ecat-studio/MainWindow.h apps/ecat-studio/workspaces/MainWindowUiBuild.cpp apps/ecat-studio/workspaces/MainWindowStartupSdoWorkspace.cpp
git commit -m "refactor: migrate startupSdoTable_ ownership to StartupSdoPlugin"
```

---

## Task 8: Final Verification

- [ ] **Step 1: Full build verification**

Run: `cd build && cmake --build . --target ecat-studio 2>&1`
Expected: Clean build with no errors or warnings related to our changes

- [ ] **Step 2: Verify all plugin constructors use ServiceContainer***

Run: `grep -n "ServiceContainer \*container" apps/ecat-studio/plugins/*/DcSyncPlugin.h apps/ecat-studio/plugins/*/AlEventPlugin.h apps/ecat-studio/plugins/*/SignalPlugin.h`
Expected: All three files show `ServiceContainer *container` parameter

- [ ] **Step 3: Verify StartupSdoPlugin is registered**

Run: `grep -n "StartupSdoPlugin" apps/ecat-studio/MainWindow.cpp`
Expected: Registration and pointer storage lines present

- [ ] **Step 4: Verify no direct startupSdoTable_ references in MainWindow.h**

Run: `grep -n "startupSdoTable_" apps/ecat-studio/MainWindow.h`
Expected: No matches (removed from MainWindow, now in plugin)

- [ ] **Step 5: Final commit if needed**

```bash
git add -A
git commit -m "refactor: complete StartupSdoPlugin extraction and constructor unification"
```

---

## Notes

### Design Decisions

1. **StartupSdoPlugin owns UI only** — The plugin owns the table widget and toolbar, but business logic (SDO writes, verification, preflight) stays in MainWindow. This follows the OdPlugin pattern where the plugin provides UI and MainWindow orchestrates data.

2. **ServiceContainer* pattern** — All plugins now receive `ServiceContainer*` instead of individual service pointers. This decouples plugins from the specific service instantiation order and allows the container to provide additional services in the future without changing plugin constructors.

3. **Table accessor pattern** — MainWindow accesses the startup SDO table via `startupSdoPlugin_->startupSdoTable()`, matching how `odPlugin_->sdoTable()` works.

4. **Watch workspace widgets** — The `startupWatchDiffsOnly`, `startupWatchSummaryLabel`, and `startupSdoDetailLabel` move from `WatchWorkspaceWidgets` to `StartupSdoPlugin`, since they logically belong to the Startup SDO workspace.

### Risks

- **UI layout changes** — The startup SDO page construction in `MainWindowUiBuild.cpp` needs careful migration to use the plugin's widget. Test that the tab still appears and functions correctly.
- **Signal connections** — Ensure all signal/slot connections that reference `startupSdoTable_` are updated to use the plugin's accessor.
