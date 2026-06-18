# NekoEcat Studio v2 — 4-Week Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Activate plugin architecture, decompose MainWindow God Object, add graphical topology, DC config, ESI repository, and bus statistics. Target 100+ tests, v2.0.0 release.

**Architecture:** WorkspacePlugin interface + EventBus + ServiceContainer. MainWindow becomes <500 line shell. All workspaces are independent plugins. Daemon gains new handlers for DC config, ESI, and bus stats.

**Tech Stack:** C++20, Qt6 (Core/Network/Widgets/Test), QGraphicsScene, IgH EtherCAT Master API, CMake, .ts/.qm (i18n)

**Spec:** `docs/superpowers/specs/2026-06-18-v2-comprehensive-design.md`

---

# Week 1: Plugin System Activation + Simple Workspace Migrations

## Task 1: Create ServiceContainer

**Files:**
- Create: `apps/ecat-studio/services/ServiceContainer.h`
- Create: `apps/ecat-studio/services/ServiceContainer.cpp`
- Create: `tests/service_container_test.cpp`
- Modify: `apps/ecat-studio/CMakeLists.txt`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write ServiceContainer test**

```cpp
// tests/service_container_test.cpp
#include <QTest>
#include "services/ServiceContainer.h"

class ServiceContainerTest : public QObject {
  Q_OBJECT
private slots:
  void testCreation() {
    ServiceContainer sc;
    QVERIFY(sc.sdo() != nullptr);
    QVERIFY(sc.watch() != nullptr);
    QVERIFY(sc.topology() != nullptr);
    QVERIFY(sc.dcSync() != nullptr);
    QVERIFY(sc.alEvent() != nullptr);
    QVERIFY(sc.signal() != nullptr);
  }
  void testServicesShareClient() {
    ServiceContainer sc;
    // All services should use the same EcatClient
    QCOMPARE(sc.sdo()->client(), sc.watch()->client());
  }
};

QTEST_MAIN(ServiceContainerTest)
#include "service_container_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target service_container_test -j4`
Expected: FAIL — header not found

- [ ] **Step 3: Implement ServiceContainer**

```cpp
// apps/ecat-studio/services/ServiceContainer.h
#pragma once
// ServiceContainer — holds all service instances and shared EcatClient.
// Passed to plugins at construction time so they can access domain services
// without knowing about MainWindow or EcatClient directly.

#include <QObject>

class EcatClient;
class SdoService;
class WatchService;
class TopologyService;
class DcSyncService;
class AlEventService;
class SignalService;
class EventBus;

class ServiceContainer : public QObject {
  Q_OBJECT
public:
  explicit ServiceContainer(QObject *parent = nullptr);

  EcatClient *client() const;
  EventBus *eventBus() const;
  SdoService *sdo() const;
  WatchService *watch() const;
  TopologyService *topology() const;
  DcSyncService *dcSync() const;
  AlEventService *alEvent() const;
  SignalService *signal() const;

private:
  EcatClient *client_ = nullptr;
  EventBus *eventBus_ = nullptr;
  SdoService *sdo_ = nullptr;
  WatchService *watch_ = nullptr;
  TopologyService *topology_ = nullptr;
  DcSyncService *dcSync_ = nullptr;
  AlEventService *alEvent_ = nullptr;
  SignalService *signal_ = nullptr;
};
```

```cpp
// apps/ecat-studio/services/ServiceContainer.cpp
#include "ServiceContainer.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"
#include "services/SdoService.h"
#include "services/WatchService.h"
#include "services/TopologyService.h"
#include "services/DcSyncService.h"
#include "services/AlEventService.h"
#include "services/SignalService.h"

ServiceContainer::ServiceContainer(QObject *parent) : QObject(parent) {
  client_ = new EcatClient(this);
  eventBus_ = new EventBus(this);
  sdo_ = new SdoService(client_, this);
  watch_ = new WatchService(client_, this);
  topology_ = new TopologyService(client_, this);
  dcSync_ = new DcSyncService(client_, this);
  alEvent_ = new AlEventService(client_, this);
  signal_ = new SignalService(client_, this);
}

EcatClient *ServiceContainer::client() const { return client_; }
EventBus *ServiceContainer::eventBus() const { return eventBus_; }
SdoService *ServiceContainer::sdo() const { return sdo_; }
WatchService *ServiceContainer::watch() const { return watch_; }
TopologyService *ServiceContainer::topology() const { return topology_; }
DcSyncService *ServiceContainer::dcSync() const { return dcSync_; }
AlEventService *ServiceContainer::alEvent() const { return alEvent_; }
SignalService *ServiceContainer::signal() const { return signal_; }
```

- [ ] **Step 4: Add to CMakeLists.txt and run test**

Run: `cmake --build build --target service_container_test -j4 && ctest --test-dir build -R service_container_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add apps/ecat-studio/services/ServiceContainer.h apps/ecat-studio/services/ServiceContainer.cpp tests/service_container_test.cpp
git commit -m "feat: ServiceContainer — centralized service ownership"
```

---

## Task 2: Wire EventBus to All Services

**Files:**
- Modify: `apps/ecat-studio/MainWindow.cpp` — replace direct EcatClient connections with EventBus
- Modify: `apps/ecat-studio/services/ServiceContainer.cpp` — wire EventBus connections

- [ ] **Step 1: Wire EcatClient signals → EventBus in ServiceContainer**

Add to ServiceContainer constructor:
```cpp
  // EcatClient → EventBus forwarding
  connect(client_, &EcatClient::slavesChanged, eventBus_, &EventBus::emitSlaveChanged);
  connect(client_, &EcatClient::sdoValue, eventBus_,
    [this](int p, const QString &i, const QString &si, const QString &v) {
      eventBus_->emitSdoValue(p, i, si, v);
    });
  connect(client_, &EcatClient::connectionStateChanged, eventBus_,
    [this](ConnectionState s) { eventBus_->emitConnectionStateChanged(s == ConnectionState::Connected); });
  connect(client_, &EcatClient::freeRunTelemetry, eventBus_, &EventBus::emitFreeRunTelemetry);
  connect(client_, &EcatClient::dcSyncStatusResult, eventBus_, &EventBus::emitDcSyncUpdate);
  connect(client_, &EcatClient::alEventLogResult, eventBus_, &EventBus::emitAlEvent);
```

- [ ] **Step 2: Remove duplicate wiring from MainWindow.cpp**

Remove the `wire()` method's EventBus connections from MainWindow.cpp (lines ~2934-2944).

- [ ] **Step 3: Build and test**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4`
Expected: 66+ tests PASS

- [ ] **Step 4: Commit**

```bash
git commit -m "refactor: move EventBus wiring from MainWindow to ServiceContainer"
```

---

## Task 3: Migrate ConsistencyPlugin (Low Coupling)

**Files:**
- Create: `apps/ecat-studio/plugins/consistency/ConsistencyPlugin.h`
- Create: `apps/ecat-studio/plugins/consistency/ConsistencyPlugin.cpp`
- Create: `tests/consistency_plugin_test.cpp`
- Modify: `apps/ecat-studio/CMakeLists.txt`
- Modify: `tests/CMakeLists.txt`
- Modify: `apps/ecat-studio/workspaces/MainWindowUiBuild.cpp` — remove consistency tab creation
- Modify: `apps/ecat-studio/MainWindow.cpp` — remove consistency methods

- [ ] **Step 1: Write ConsistencyPlugin test**

Test identity: id="consistency", displayName="Consistency", displayNameZh="一致性", defaultOrder=67, visible=true.

- [ ] **Step 2: Create ConsistencyPlugin**

Move code from `MainWindowConsistency.cpp` (597 lines) into the plugin. The plugin takes `ServiceContainer*` and accesses `TopologyService` for slave data.

Key methods to move:
- `MainWindow::openConsistencyView()` → `ConsistencyPlugin::activate()`
- `MainWindow::updateConsistencyView()` → `ConsistencyPlugin::updateView()`
- `MainWindow::consistencyIssueCounts()` → `ConsistencyPlugin::issueCounts()`

- [ ] **Step 3: Remove from MainWindow**

Remove `consistencyTabIndex_` member, remove consistency tab from `MainWindowUiBuild.cpp`.

- [ ] **Step 4: Register in PluginRegistry**

Add to MainWindow constructor:
```cpp
  auto *consistencyPlugin = new ConsistencyPlugin(services_, this);
  pluginRegistry_->registerPlugin(consistencyPlugin);
```

- [ ] **Step 5: Build, test, commit**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4`

```bash
git commit -m "refactor: extract ConsistencyPlugin from MainWindow"
```

---

## Task 4: Migrate StateMachinePlugin

**Files:**
- Create: `apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h`
- Create: `apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp`
- Create: `tests/statemachine_plugin_test.cpp`

- [ ] **Step 1: Write test, create plugin, move 675 lines from MainWindowStateMachine.cpp**
- [ ] **Step 2: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 3: Build, test, commit**

```bash
git commit -m "refactor: extract StateMachinePlugin from MainWindow"
```

---

## Task 5: Migrate DiagnosticsPlugin

**Files:**
- Create: `apps/ecat-studio/plugins/diagnostics/DiagnosticsPlugin.h`
- Create: `apps/ecat-studio/plugins/diagnostics/DiagnosticsPlugin.cpp`
- Create: `tests/diagnostics_plugin_test.cpp`

- [ ] **Step 1: Write test, create plugin, move ~400 lines from MainWindowDiagnosticsTopology.cpp**
- [ ] **Step 2: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 3: Build, test, commit**

```bash
git commit -m "refactor: extract DiagnosticsPlugin from MainWindow"
```

---

## Task 6: Migrate RtTestPlugin

**Files:**
- Create: `apps/ecat-studio/plugins/rttest/RtTestPlugin.h`
- Create: `apps/ecat-studio/plugins/rttest/RtTestPlugin.cpp`
- Create: `tests/rttest_plugin_test.cpp`

- [ ] **Step 1: Write test, create plugin, move 532 lines from MainWindowRtTestWorkspace.cpp**
- [ ] **Step 2: Include RtTestLatencyChart and RtTestJitterSpark as plugin-private widgets**
- [ ] **Step 3: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 4: Build, test, commit**

```bash
git commit -m "refactor: extract RtTestPlugin from MainWindow"
```

---

## Task 7: Week 1 Integration

- [ ] **Step 1: Build all, run all tests**

Expected: 80+ tests PASS

- [ ] **Step 2: Verify GUI smoke**

Run: `QT_QPA_PLATFORM=offscreen timeout 3 build/apps/ecat-studio/ecat-studio`

- [ ] **Step 3: Commit**

```bash
git commit -m "chore: week 1 integration — 4 plugins migrated, ServiceContainer active"
```

---

# Week 2: Remaining Workspace Migrations

## Task 8: Migrate WatchPlugin

**Files:**
- Create: `apps/ecat-studio/plugins/watch/WatchPlugin.h`
- Create: `apps/ecat-studio/plugins/watch/WatchPlugin.cpp`
- Create: `tests/watch_plugin_test.cpp`

- [ ] **Step 1: Write test, create plugin, move ~600 lines from MainWindowWatchWorkspace.cpp + MainWindowWatchSync.cpp**
- [ ] **Step: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 3: Build, test, commit**

---

## Task 9: Migrate FreeRunPlugin

**Files:**
- Create: `apps/ecat-studio/plugins/freerun/FreeRunPlugin.h`
- Create: `apps/ecat-studio/plugins/freerun/FreeRunPlugin.cpp`
- Create: `tests/freerun_plugin_test.cpp`

- [ ] **Step 1: Write test, create plugin, move ~500 lines from MainWindowFreeRunChart.cpp**
- [ ] **Step 2: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 3: Build, test, commit**

---

## Task 10: Migrate IoVariablePlugin

**Files:**
- Create: `apps/ecat-studio/plugins/iovariable/IoVariablePlugin.h`
- Create: `apps/ecat-studio/plugins/iovariable/IoVariablePlugin.cpp`
- Create: `tests/iovariable_plugin_test.cpp`

- [ ] **Step 1: Write test, create plugin, move 835 lines from MainWindowIoVariableWorkspace.cpp**
- [ ] **Step 2: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 3: Build, test, commit**

---

## Task 11: Migrate StartupSdoPlugin (High Coupling)

**Files:**
- Create: `apps/ecat-studio/plugins/startupsdo/StartupSdoPlugin.h`
- Create: `apps/ecat-studio/plugins/startupsdo/StartupSdoPlugin.cpp`
- Create: `tests/startupsdo_plugin_test.cpp`

- [ ] **Step 1: Write test, create plugin, move 1098 lines from MainWindowStartupSdoWorkspace.cpp**
- [ ] **Step 2: Extract StartupSdoService for startup SDO list management**
- [ ] **Step 3: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 4: Build, test, commit**

---

## Task 12: Migrate OdPlugin (Highest Coupling — 4500 lines)

**Files:**
- Create: `apps/ecat-studio/plugins/od/OdPlugin.h`
- Create: `apps/ecat-studio/plugins/od/OdPlugin.cpp`
- Create: `tests/od_plugin_test.cpp`

- [ ] **Step 1: Write test**
- [ ] **Step 2: Move code from 4 files:**
  - MainWindowSdoWorkspace.cpp
  - MainWindowSdoSelection.cpp
  - MainWindowSdoWrite.cpp
  - MainWindowSdoTargetPanel.cpp
  - MainWindowSdoBookmarks.cpp
  - MainWindowSdoHistory (if exists)
- [ ] **Step 3: OdPlugin becomes the largest plugin (~4500 lines). Internal structure:**
  - OdPlugin.cpp — plugin shell, tab management
  - OdDictionaryTab.cpp — OD browsing/filtering
  - OdSelectionTab.cpp — selected objects management
  - OdWritePanel.cpp — SDO write interface
  - OdTargetTrail.cpp — target trail tracking
  - OdBookmarks.cpp — bookmark management
- [ ] **Step 4: Remove from MainWindow, register in PluginRegistry**
- [ ] **Step 5: Build, test, commit**

---

## Task 13: Migrate SessionPlugin + ExportPlugin

**Files:**
- Create: `apps/ecat-studio/plugins/session/SessionPlugin.h/.cpp`
- Create: `apps/ecat-studio/plugins/export/ExportPlugin.h/.cpp`

- [ ] **Step 1: Move MainWindowSessionWorkspace.cpp (1275 lines) → SessionPlugin**
- [ ] **Step 2: Move MainWindowExport.cpp (~300 lines) → ExportPlugin**
- [ ] **Step 3: Build, test, commit**

---

## Task 14: Migrate OverviewPlugin + ContextMenus

**Files:**
- Create: `apps/ecat-studio/plugins/overview/OverviewPlugin.h/.cpp`

- [ ] **Step 1: Move ~300 lines from MainWindowUiBuild.cpp (overview section)**
- [ ] **Step 2: Move context menu code (1555 lines) to respective plugins**
- [ ] **Step 3: Move CommandPalette to MainWindow (keep as-is, 1922 lines)**
- [ ] **Step 4: Build, test, commit**

---

## Task 15: MainWindow Final Cleanup

**Files:**
- Modify: `apps/ecat-studio/MainWindow.h`
- Modify: `apps/ecat-studio/MainWindow.cpp`

- [ ] **Step 1: Remove all migrated members and methods from MainWindow.h**
- [ ] **Step 2: MainWindow.h should be <200 lines**
- [ ] **Step 3: MainWindow.cpp should be <500 lines**
- [ ] **Step 4: Build, test, commit**

---

## Task 16: Week 2 Integration

- [ ] **Step 1: Build all, run all tests**

Expected: 100+ tests PASS

- [ ] **Step 2: Verify all workspace tabs functional**

- [ ] **Step 3: Commit**

```bash
git commit -m "chore: week 2 integration — all workspaces migrated, MainWindow <500 lines"
```

---

# Week 3: New Features — Topology + DC + ESI

## Task 17: SlaveNodeItem + TopologyGraphWidget

**Files:**
- Create: `apps/ecat-studio/plugins/topology/SlaveNodeItem.h`
- Create: `apps/ecat-studio/plugins/topology/SlaveNodeItem.cpp`
- Create: `apps/ecat-studio/plugins/topology/TopologyGraphWidget.h`
- Create: `apps/ecat-studio/plugins/topology/TopologyGraphWidget.cpp`
- Create: `tests/topology_graph_test.cpp`

- [ ] **Step 1: Write SlaveNodeItem test**

Test: boundingRect not empty, paint doesn't crash, position text rendered.

- [ ] **Step 2: Implement SlaveNodeItem**

```cpp
class SlaveNodeItem : public QGraphicsItem {
public:
  SlaveNodeItem(int position, const SlaveInfo &info, QGraphicsItem *parent = nullptr);
  QRectF boundingRect() const override { return QRectF(0, 0, 180, 80); }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *w) override;
  int position() const { return position_; }
  void setSlaveInfo(const SlaveInfo &info);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
private:
  int position_;
  SlaveInfo info_;
  QColor stateColor() const;
};
```

- [ ] **Step 3: Implement TopologyGraphWidget**

```cpp
enum class LayoutMode { Linear, Tree };

class TopologyGraphWidget : public QGraphicsView {
  Q_OBJECT
public:
  explicit TopologyGraphWidget(QWidget *parent = nullptr);
  void updateTopology(const QVector<SlaveInfo> &slaves);
  void setLayoutMode(LayoutMode mode);
signals:
  void slaveSelected(int position);
  void slaveDoubleClicked(int position);
private:
  QGraphicsScene *scene_;
  QVector<SlaveNodeItem*> nodes_;
  QVector<QGraphicsLineItem*> links_;
  LayoutMode layoutMode_ = LayoutMode::Linear;
  void layoutLinear();
  void layoutTree();
  void clearItems();
};
```

- [ ] **Step 4: Build, test, commit**

---

## Task 18: TopologyPlugin Integration

**Files:**
- Create: `apps/ecat-studio/plugins/topology/TopologyPlugin.h`
- Create: `apps/ecat-studio/plugins/topology/TopologyPlugin.cpp`
- Modify: `apps/ecat-studio/CMakeLists.txt`

- [ ] **Step 1: Create TopologyPlugin wrapping TopologyGraphWidget**

```cpp
class TopologyPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  QString id() const override { return "topology"; }
  QString displayName() const override { return "Topology"; }
  QString displayNameZh() const override { return "拓扑"; }
  int defaultOrder() const override { return 10; }
  QWidget *widget() override;
private:
  TopologyGraphWidget *graphView_;
  QComboBox *layoutCombo_;
};
```

- [ ] **Step 2: Connect EventBus::slaveChanged to graphView_->updateTopology()**
- [ ] **Step 3: Register in PluginRegistry, replace old topology table tab**
- [ ] **Step 4: Build, test, commit**

---

## Task 19: DC Configuration Daemon Handlers

**Files:**
- Create: `apps/ecatd/handlers/DcConfigHandler.h`
- Create: `apps/ecatd/handlers/DcConfigHandler.cpp`
- Create: `tests/dc_config_handler_test.cpp`
- Modify: `apps/ecatd/CMakeLists.txt`
- Modify: `apps/ecatd/EcatDaemon.cpp`

- [ ] **Step 1: Write test**

Test: handleConfigure returns success JSON, handleRefClockSet validates position.

- [ ] **Step 2: Implement DcConfigHandler**

```cpp
class DcConfigHandler {
public:
  QJsonObject handleConfigure(const QString &id, const QJsonObject &params);
  QJsonObject handleRefClockSet(const QString &id, const QJsonObject &params);
private:
  QString runCommand(const QString &cmd) const;
};
```

- [ ] **Step 3: Register in EcatDaemon: `dcConfigure`, `dcRefClockSet`**
- [ ] **Step 4: Build, test, commit**

---

## Task 20: DcConfigPlugin + DcConfigService

**Files:**
- Create: `apps/ecat-studio/plugins/dcconfig/DcConfigPlugin.h`
- Create: `apps/ecat-studio/plugins/dcconfig/DcConfigPlugin.cpp`
- Create: `apps/ecat-studio/services/DcConfigService.h`
- Create: `apps/ecat-studio/services/DcConfigService.cpp`
- Create: `tests/dc_config_service_test.cpp`

- [ ] **Step 1: Write test, implement service, implement plugin**
- [ ] **Step 2: UI: left panel (slave list), right panel (config form + clock path + quality chart)**
- [ ] **Step 3: Add dcConfigure/dcRefClockSet to EcatClient**
- [ ] **Step 4: Build, test, commit**

---

## Task 21: ESI Daemon Handler

**Files:**
- Create: `apps/ecatd/handlers/EsiHandler.h`
- Create: `apps/ecatd/handlers/EsiHandler.cpp`
- Create: `tests/esi_handler_test.cpp`

- [ ] **Step 1: Write test with sample ESI XML**
- [ ] **Step 2: Implement XML parsing with QXmlStreamReader**
- [ ] **Step 3: Register handlers: esiImport, esiMatch, esiExport, esiList**
- [ ] **Step 4: Build, test, commit**

---

## Task 22: EsiPlugin + EsiService

**Files:**
- Create: `apps/ecat-studio/plugins/esi/EsiPlugin.h`
- Create: `apps/ecat-studio/plugins/esi/EsiPlugin.cpp`
- Create: `apps/ecat-studio/services/EsiService.h`
- Create: `apps/ecat-studio/services/EsiService.cpp`
- Create: `tests/esi_service_test.cpp`

- [ ] **Step 1: Write test, implement service, implement plugin**
- [ ] **Step 2: UI: file browser + import button + device list + detail panel**
- [ ] **Step 3: Wire auto-match into TopologyService::scanComplete**
- [ ] **Step 4: Build, test, commit**

---

## Task 23: Week 3 Integration

- [ ] **Step 1: Build all, run all tests**

Expected: 120+ tests PASS

- [ ] **Step 2: Verify all new plugins functional**

- [ ] **Step 3: Commit**

---

# Week 4: Bus Stats + Tests + CI/CD + Release

## Task 24: BusStats Daemon Handler + Plugin

**Files:**
- Create: `apps/ecatd/handlers/BusStatsHandler.h/.cpp`
- Create: `apps/ecat-studio/plugins/busstats/BusStatsPlugin.h/.cpp`
- Create: `apps/ecat-studio/services/BusStatsService.h/.cpp`
- Create: `tests/bus_stats_test.cpp`

- [ ] **Step 1: Write test, implement handler, implement service, implement plugin**
- [ ] **Step 2: UI: dashboard with gauges/counters for frame counts, errors, bandwidth**
- [ ] **Step 3: Build, test, commit**

---

## Task 25: Integration Tests

**Files:**
- Create: `tests/integration/daemon_lifecycle_test.cpp`
- Create: `tests/integration/gui_smoke_test.cpp`

- [ ] **Step 1: Daemon lifecycle test — start, connect, ping, disconnect, stop**
- [ ] **Step 2: GUI smoke test — MainWindow opens, all tabs load, no crash**
- [ ] **Step 3: Build, test, commit**

---

## Task 26: CI/CD Pipeline Update

**Files:**
- Modify: `.github/workflows/ci.yml`

- [ ] **Step 1: Add integration test targets**
- [ ] **Step 2: Add release artifact upload step**
- [ ] **Step 3: Commit**

---

## Task 27: Documentation

**Files:**
- Modify: `README.md` — add v2 features
- Create: `docs/ARCHITECTURE.md` — plugin system, EventBus, Service layer
- Create: `docs/PLUGIN_GUIDE.md` — how to create a new workspace plugin
- Modify: `RELEASE_NOTES.md` — v2.0.0 notes

- [ ] **Step 1: Update README with all new features**
- [ ] **Step 2: Write ARCHITECTURE.md**
- [ ] **Step 3: Write PLUGIN_GUIDE.md**
- [ ] **Step 4: Commit**

---

## Task 28: Release v2.0.0

- [ ] **Step 1: Update version to 2.0.0 in CMakeLists.txt**
- [ ] **Step 2: Build release binary**
- [ ] **Step 3: Run all tests**
- [ ] **Step 4: Create release package**
- [ ] **Step 5: Create GitHub release**

```bash
gh release create v2.0.0 NekoEcat-Studio-2.0.0-linux-x86_64.tar.gz \
  --title "NekoEcat Studio v2.0.0" \
  --notes-file RELEASE_NOTES.md
```

