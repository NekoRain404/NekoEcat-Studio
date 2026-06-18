# NekoEcat Studio v2 — Plugin Architecture & Feature Expansion Design

> **Date:** 2026-06-18
> **Status:** Approved
> **Scope:** Architecture refactor + hardware diagnostics + signal analysis (1 month)

---

## 1. Problem Statement

NekoEcat Studio is a Qt6 desktop EtherCAT commissioning tool (46K LOC, 139 files, 55 tests).
It currently works but has critical architectural and functional gaps:

- **God Object:** MainWindow.h has 389 methods, 126 members, 764 lines
- **Tight coupling:** GUI directly calls daemon client, no service abstraction
- **Translation bottleneck:** TranslationRegistry.cpp is 2023 lines, compiles in 90s+
- **Missing hardware features:** No DC Sync, no AL Event log, no adapter selection
- **No signal analysis:** No oscilloscope or multi-channel trending tool
- **Test gap:** 55 tests cover model/adapter only, zero GUI or daemon integration tests

**Goal:** Refactor to plugin architecture, add professional-grade hardware diagnostics and signal analysis, maintain lightweight UX.

---

## 2. Architecture

### 2.1 Plugin System

Each workspace becomes a `WorkspacePlugin` implementing a common interface.
MainWindow becomes a thin shell (<200 methods) that hosts plugins.

```
┌─────────────────────────────────────────────┐
│  MainWindow (thin shell)                     │
│  - MenuBar / ToolBar / StatusBar             │
│  - TabHost (loads plugin UIs)                │
│  - PluginRegistry                            │
└──────────┬──────────────────────────────────┘
           │
    ┌──────┴──────┐
    │  EventBus   │ ← Qt signal/slot central event bus
    └──────┬──────┘
           │
  ┌────────┼────────┬──────────┬───────────┐
  │        │        │          │           │
┌─┴──┐ ┌──┴──┐ ┌───┴──┐ ┌────┴──┐ ┌──────┴──────┐
│OD  │ │Watch│ │FreeRun│ │DcSync │ │SignalAnalyzer│
│    │ │     │ │      │ │       │ │             │
└────┘ └─────┘ └──────┘ └───────┘ └─────────────┘
```

### 2.2 WorkspacePlugin Interface

```cpp
class WorkspacePlugin : public QObject {
    Q_OBJECT
public:
    virtual ~WorkspacePlugin() = default;

    // Identity
    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual QString displayNameZh() const = 0;
    virtual QIcon icon() const = 0;

    // UI
    virtual QWidget *widget() = 0;
    virtual int defaultOrder() const = 0;
    virtual bool visible() const = 0;

    // Lifecycle
    virtual void activate() {}
    virtual void deactivate() {}
    virtual void onSettingsChanged(const AppSettings &s) {}
    virtual void onConnectionChanged(bool connected) {}

signals:
    void requestNavigate(const QString &pluginId);
    void updateDiagnostics(const QString &level, const QString &source, const QString &msg);
};
```

### 2.3 EventBus Events

| Event | Payload | Producers | Consumers |
|---|---|---|---|
| SlaveChanged | QVector<SlaveInfo> | TopologyService | Overview, OD, Watch |
| SdoValueReceived | position, index, subIndex, value | SdoService | OD, Watch, Startup |
| ConnectionStateChanged | bool connected | EcatClient | All plugins |
| FreeRunTelemetry | QJsonObject | FreeRunService | FreeRun, SignalAnalyzer |
| TopologyChanged | QVector<SlaveInfo> | TopologyService | Diagnostics, DC Sync |
| DcSyncUpdate | QJsonObject (new) | DcSyncService | DC Sync plugin |
| AlEvent | QJsonObject (new) | AlEventService | Diagnostics |
| SignalData | ch, values[], timestamps[] (new) | SignalAnalyzer | Signal plugin |

### 2.4 Service Layer

Services sit between plugins and the daemon client. Each service owns a specific domain.

```
Plugin ←→ Service ←→ EcatClient ←→ Daemon (JSON-RPC) ←→ IgH API
```

| Service | Responsibility |
|---|---|
| SdoService | SDO read/write, dictionary caching, evidence tracking |
| WatchService | Watch list management, periodic polling, drift detection |
| TopologyService | Bus scanning, slave info, topology baseline |
| FreeRunService | Process image telemetry, entry management |
| StartupSdoService | Startup SDO list, apply, verify |
| DcSyncService (new) | DC sync status, reference clock, drift monitoring |
| AlEventService (new) | AL error log, event history, severity classification |
| AdapterService (new) | NIC enumeration, adapter selection, IgH config |
| SignalService (new) | Multi-channel acquisition, ring buffer, statistics |

---

## 3. Daemon API Migration

### 3.1 Current State

- FreeRun: direct `ecrt_*` API (ecrt_request_master, ecrt_domain_*, etc.)
- Everything else: IgH CLI wrapper (`ethercat slaves`, `ethercat master`, etc.)
- 20 JSON-RPC handlers

### 3.2 Migration Plan

**Phase 1 — New direct-API handlers (Week 2):**

| Handler | API | Purpose |
|---|---|---|
| `dcSyncStatus` | `ecrt_master_reference_clock()`, `ecrt_slave_config_dc()` | DC sync query |
| `alEventLog` | `ecrt_master_ALerror()` + periodic poll | AL error log |
| `listAdapters` | `/sys/class/net/` + ethtool | NIC enumeration |
| `setAdapter` | IgH config + daemon restart | Switch adapter |

**Phase 2 — Migrate existing CLI handlers (Week 2-3):**

| Handler | From | To |
|---|---|---|
| `upload` | `ethercat upload` | `ecrt_master_sdo_upload()` |
| `download` | `ethercat download` | `ecrt_master_sdo_download()` |
| `sdos` | `ethercat sdos` | `ecrt_master_sdo_upload()` batch |
| `setState` | `ethercat state` | `ecrt_master_state()` |

**Phase 3 — Signal analysis service (Week 3):**

Daemon-side ring buffer + JSON-RPC push for multi-channel signal data.

### 3.3 Auto-Reconnect

- Daemon sends heartbeat ping every 5s
- GUI detects missed heartbeat → enters "reconnecting" state
- GUI retries connection every 2s with exponential backoff (max 30s)
- On reconnect: re-scan topology, restore watch list, resume FreeRun if active

---

## 4. New Features

### 4.1 DC Sync Diagnostics Plugin

**UI:**
- Reference clock slave indicator
- Per-slave DC sync status (syncing / not syncing / error)
- Cycle time vs configured time comparison
- Drift histogram (last 1000 cycles)
- Jitter statistics (min/max/avg/stddev)

**Data flow:**
```
Daemon: ecrt_master_reference_clock() + ecrt_slave_config_dc()
  → dcSyncStatus handler → JSON response
    → DcSyncService → EventBus::DcSyncUpdate
      → DcSyncPlugin UI update
```

### 4.2 AL Event Log Plugin

**UI:**
- Chronological event table (timestamp, slave, error code, description)
- Severity filter (Error / Warning / Info)
- Auto-scroll with pause-on-hover
- Export to CSV

**Data flow:**
```
Daemon: periodic ecrt_master_ALerror() poll (1s interval)
  → alEventLog handler → JSON array
    → AlEventService → EventBus::AlEvent
      → AlEventPlugin table update
```

### 4.3 Network Adapter Selection

**UI:**
- Settings tab: dropdown of detected NICs
- Shows: interface name, MAC, driver, link status
- Selecting a NIC updates daemon config

### 4.4 Signal Analyzer Plugin

**UI:**
- Multi-channel real-time line chart (QPainter, no QtCharts)
- Channel list panel (add/remove/rename)
- X-axis: time (seconds) or sample index
- Y-axis: auto-range or manual min/max
- Configurable poll interval (1ms - 1s)
- Statistics overlay: min, max, avg, stddev
- Up to 4 independent windows
- CSV export

**Data model:**
```cpp
struct SignalChannel {
    QString name;
    int slavePosition;
    QString index;
    QString subIndex;
    QVector<double> values;     // ring buffer, max 10000
    QVector<qint64> timestamps; // ms since start
    double yMin, yMax;
};
```

### 4.5 Enhanced Auto-Reconnect

- Status bar indicator: Connected / Reconnecting / Disconnected
- Toast notification on disconnect/reconnect
- Graceful degradation: read-only mode during reconnect
- Configurable retry policy in Settings

---

## 5. Testing Strategy

### 5.1 Test Pyramid

| Layer | Coverage Target | Tools |
|---|---|---|
| Model/Adapter unit tests | 100% of existing + new | Qt Test, 55+ tests |
| Service layer tests | 100% of new services | Qt Test, Mock daemon |
| Daemon integration tests | All handlers | Qt Test, real daemon + mock IgH |
| GUI smoke tests | Critical workflows | offscreen QPA, screenshot compare |

### 5.2 Daemon Integration Test Architecture

```cpp
class DaemonIntegrationTest : public QObject {
    Q_OBJECT
    // Starts real ecatd process on localhost
    // Sends JSON-RPC requests via QTcpSocket
    // Validates responses against expected schema
};
```

### 5.3 TDD Workflow

For each new feature:
1. Write failing test (service layer)
2. Implement minimal service code
3. Write failing test (daemon handler)
4. Implement daemon handler
5. Write GUI smoke test
6. Implement plugin UI
7. Integration test end-to-end
8. Commit

---

## 6. CI/CD Pipeline

```yaml
# .github/workflows/ci.yml
name: CI
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - checkout
      - install Qt6 + IgH headers
      - cmake build (all targets)
      - ctest (all tests)
      - offscreen GUI smoke test
  lint:
    - clang-tidy
    - include-what-you-use
```

---

## 7. Migration Strategy

### 7.1 Incremental Plugin Extraction

Each existing workspace is converted to a plugin one at a time:

1. Define WorkspacePlugin interface (Day 1)
2. Extract simplest workspace first (Notes → NotesPlugin) as proof of concept
3. Extract progressively: State Machine → Consistency → Diagnostics → Watch → OD → FreeRun → Overview
4. Each extraction: write service tests first, then convert UI, then remove old code

### 7.2 Risk Mitigation

- Each plugin extraction is a separate commit/PR
- All 55 existing tests must pass after each extraction
- GUI smoke test after each extraction
- Rollback: revert single commit if issues found

---

## 8. File Structure (Target)

```
apps/ecat-studio/
├── main.cpp
├── MainWindow.cpp/.h              # Thin shell (<200 methods)
├── plugins/                       # NEW
│   ├── WorkspacePlugin.h          # Interface
│   ├── PluginRegistry.h/.cpp      # Plugin loader
│   ├── overview/OverviewPlugin.h/.cpp
│   ├── od/OdPlugin.h/.cpp
│   ├── watch/WatchPlugin.h/.cpp
│   ├── freerun/FreeRunPlugin.h/.cpp
│   ├── dcsync/DcSyncPlugin.h/.cpp      # NEW
│   ├── alevent/AlEventPlugin.h/.cpp     # NEW
│   ├── signal/SignalPlugin.h/.cpp       # NEW
│   └── ...
├── services/                      # NEW
│   ├── EventBus.h/.cpp
│   ├── SdoService.h/.cpp
│   ├── WatchService.h/.cpp
│   ├── TopologyService.h/.cpp
│   ├── DcSyncService.h/.cpp       # NEW
│   ├── AlEventService.h/.cpp      # NEW
│   ├── AdapterService.h/.cpp      # NEW
│   └── SignalService.h/.cpp       # NEW
├── infra/                         # Existing
│   ├── EcatClient.h/.cpp
│   ├── SettingsDialog.h/.cpp
│   └── TranslationRegistry.h/.cpp # TO BE REFACTORED → .ts files
├── models/                        # Existing, unchanged
├── adapters/                      # Existing, unchanged
├── detail/                        # Existing, will be absorbed into plugins
├── utils/                         # Existing, unchanged
└── themes/                        # NEW — extracted from MainWindowTheme.cpp
    ├── ThemeManager.h/.cpp
    ├── themes.qss
    ├── dark.qss
    └── ...

apps/ecatd/
├── handlers/
│   ├── DcSyncHandler.h/.cpp       # NEW
│   ├── AlEventHandler.h/.cpp      # NEW
│   ├── AdapterHandler.h/.cpp      # NEW
│   └── SignalHandler.h/.cpp       # NEW
└── ...existing...
```

---

## 9. Translation Refactor

**Problem:** TranslationRegistry.cpp is 2023 lines, compiles in 90s+.

**Solution:** Move to Qt's standard .ts/.qm translation system.

- Extract all map_ entries to `translations/en.ts`, `translations/zh.ts`, etc.
- Use `lrelease` to compile .ts → .qm at build time
- Load via `QTranslator` at runtime
- Remove TranslationRegistry.cpp entirely
- `uiText()` becomes `tr()` with proper context

---

## 10. Theme Refactor

**Problem:** MainWindowTheme.cpp is 3104 lines of inline QSS strings.

**Solution:** Extract to external .qss files.

- Each theme → one `.qss` file in `themes/` directory
- `ThemeManager` loads QSS from file at runtime
- Themes can be edited without recompilation
- Support user-custom themes via `~/.config/NekoEcatStudio/themes/`

---

## 11. Milestones

| Week | Focus | Deliverables |
|---|---|---|
| W1 | Architecture refactor | Plugin interface, EventBus, first plugin (Notes), MainWindow slimmed, TranslationRegistry → .ts, Theme extraction |
| W2 | Daemon migration + DC Sync | Direct API handlers, DcSyncService + Plugin, AL Event handler, Adapter selection, 20+ new tests |
| W3 | Signal analysis + remaining plugins | SignalAnalyzer plugin, remaining workspace plugins converted, auto-reconnect, 30+ new tests |
| W4 | Integration + CI/CD + release | Full test suite, GitHub Actions, documentation, v1.0.0 release |

---

## 12. Success Criteria

- MainWindow.h < 200 methods
- All 22 workspace tabs converted to plugins
- DC Sync, AL Event, Adapter Selection, Signal Analyzer fully functional
- 100+ tests passing (55 existing + 45+ new)
- CI/CD pipeline green
- v1.0.0 release published on GitHub
