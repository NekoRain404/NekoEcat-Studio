# Architecture

## Overview

NekoEcat Studio is a modern EtherCAT commissioning and diagnostics workstation for Linux, built on the IgH EtherCAT Master stack. The architecture follows a two-process model with a plugin-based Qt6 GUI, JSON-RPC over TCP for inter-process communication, and a service container for dependency injection.

The codebase uses C++20, Qt6, and CMake 3.20+. It is organized into four logical layers:

| Layer | Location | Purpose |
|-------|----------|---------|
| Core Types | `src/core` | Shared domain types and JSON framing protocol |
| IgH Adapter | `src/igh` | Parses IgH `ethercat` CLI stdout into structured objects |
| Daemon | `apps/ecatd` | Local TCP server that wraps IgH CLI tools and ecrt API |
| GUI | `apps/ecat-studio` | Qt6 desktop application with plugin-based workspace tabs |

## System Architecture

```
 Engineer
     |
     v
+---------------------+
|   ecat-studio       |  Qt6 GUI, 28 visible workspace tabs
|   (plugins,         |  PluginRegistry, EventBus, ServiceContainer
|    services)         |
+----------+----------+
           |
           | JSON-RPC / TCP : 5877
           | (newline-delimited JSON)
           v
+---------------------+
|   ecatd             |  Runtime daemon
|   (CommandDispatcher, |  Accepts multiple TCP clients
|    handler chain)    |
+----------+----------+
           |
           | IgH ethercat CLI / ecrt API
           v
+---------------------+
| IgH EtherCAT Master |  Linux kernel module
+----------+----------+
           |
           v
   EtherCAT Bus
   (slaves, drives, I/O)
```

### Dependency Direction

```
src/core  (no external deps beyond Qt6::Core)
    ^
    |
src/igh   (depends on core)
    ^
    |
apps/ecatd  (depends on core + igh + IgH system library)
    ^
    |
apps/ecat-studio  (depends on core only; talks to daemon via TCP)
```

- **src/core** is the foundation -- domain types (`SlaveInfo`, `JsonProtocol`) with zero external dependencies beyond Qt6::Core.
- **src/igh** depends on `ecat_core` and wraps the IgH `ethercat` CLI, parsing stdout into `QVector<SlaveInfo>` and structured results.
- **apps/ecatd** links `ecat_core`, `ecat_igh`, and the `ethercat` system library. It runs a local TCP server and dispatches JSON commands.
- **apps/ecat-studio** links only `ecat_core`. It communicates with the daemon exclusively over TCP using `EcatClient`.

## Process Model

### ecatd (Daemon)

The daemon is a local TCP server listening on `127.0.0.1:5877`. It accepts newline-delimited JSON requests and returns JSON responses. Each request carries an `id` (for correlation), a `method` (string command name), and `params` (object).

**Request format:**
```json
{"id": "1", "method": "scan", "params": {"master": "0"}}
```

**Response format:**
```json
{"id": "1", "ok": true, "result": {"slaves": [...]}}
{"id": "1", "ok": false, "error": {"message": "...", "code": -1}}
```

**Daemon internals:**

```
EcatDaemon
  +-- CommandDispatcher        (routes method -> handler lambda via O(1) hash table)
  +-- EthercatCliBackend       (EcatService impl; wraps IgH CLI)
  +-- FreeRunController        (ecrt-based process image I/O at ~1kHz, SCHED_FIFO + TIMER_ABSTIME)
  +-- RtTestController         (real-time latency testing)
  +-- handlers/
  |     +-- DcSyncHandler      (DC sync status, configuration, activation, deactivation)
  |     +-- AlEventHandler     (AL event log, polled every 1s)
  |     +-- AdapterHandler     (network adapter discovery/selection)
  |     +-- FoEHandler         (File over EtherCAT firmware read/write)
  |     +-- SoEHandler         (Servo over EtherCAT IDN read/write)
  |     +-- EoEHandler         (Ethernet over EtherCAT configuration)
  |     +-- SignalHandler      (signal subscription/polling)
  |     +-- RedundancyHandler  (cable redundancy management)
  |     +-- OnlineChangeHandler (runtime reconfiguration)
```

The daemon accepts multiple TCP clients simultaneously with per-socket line buffers for reassembly across TCP fragmentation.

### ecat-studio (GUI)

The GUI process is a Qt6 desktop application that provides the engineering workstation interface. It connects to the daemon at startup and maintains the connection with automatic reconnection and exponential backoff (2s->4s->8s->16s->30s).

Components:

```
ecat-studio
  +-- MainWindow              (top-level window, tab bar coordinator)
  |     +-- workspaces/       (partial implementations for MainWindow)
  +-- plugins/                (28 visible WorkspacePlugin implementations)
  |     +-- PluginRegistry    (central plugin management, ordering, visibility)
  |     +-- WorkspacePlugin.h (base interface for all plugins)
  +-- services/               (domain service layer)
  |     +-- ServiceContainer  (dependency injection container)
  |     +-- EventBus          (pub/sub signal hub)
  +-- models/                 (pure data/logic types)
  +-- adapters/               (model-to-QTableWidget bridge)
  +-- detail/                 (detail panel text builders)
  +-- utils/                  (reusable utilities)
  +-- infra/                  (EcatClient, settings, i18n)
  |     +-- EcatClient.h/cpp  (TCP client, connection state machine)
  |     +-- LanguageManager   (i18n/l10n registry)
  +-- themes/                 (12 .qss theme files + ThemeManager)
```

## IPC Protocol

The IPC protocol is a simple JSON-RPC-like protocol over TCP.

- **Transport**: TCP on localhost port 5877
- **Framing**: Newline-delimited JSON (`\n`-terminated frames)
- **Correlation**: Each request has a unique numeric `id`, echoed in the response
- **Timeouts**: Configurable per-request timeout (default 10s), swept periodically
- **Reconnection**: Automatic with exponential backoff, heartbeat pings every 5s

Key daemon methods:

| Method | Description |
|--------|-------------|
| `ping` | Daemon health check, returns version and metrics |
| `scan` | Bus scan, returns all slaves |
| `upload` / `download` | SDO read/write |
| `setState` / `setAllStates` | Slave state control |
| `freeRunStart` / `freeRunStop` | Free Run mode control |
| `dcSyncStatus` / `dcActivate` | Distributed clock management |
| `foeRead` / `foeWrite` | File over EtherCAT |
| `soeRead` / `soeWrite` | Servo over EtherCAT |
| `eoeStatus` / `eoeConfigureIp` | Ethernet over EtherCAT |
| `redundancyStatus` / `redundancyEnable` | Cable redundancy |
| `onlineChangePreview` / `onlineChangeApply` | Runtime reconfiguration |
| `alEventLog` | AL event log retrieval |

## Plugin System

The GUI uses a plugin architecture where each workspace tab is a self-contained `WorkspacePlugin` implementation. Plugins register with a central `PluginRegistry` and communicate through an `EventBus`.

### WorkspacePlugin Interface

**File**: `apps/ecat-studio/plugins/WorkspacePlugin.h`

All workspace tabs inherit from `WorkspacePlugin` (extends `QObject`) and implement:

- **Identity**: `id()`, `displayName()`, `displayNameZh()`, `icon()`
- **UI**: `widget()`, `defaultOrder()`, `visible()`
- **Lifecycle**: `activate()`, `deactivate()`, `onSettingsChanged()`, `onConnectionChanged()`
- **Signals**: `requestNavigate()`, `updateDiagnostics()`

### PluginRegistry

**File**: `apps/ecat-studio/plugins/PluginRegistry.h`

Central registry managing all workspace plugins. Provides:
- Ordered access (sorted by `defaultOrder()` ascending)
- ID-based lookup (O(log n) via `QMap`)
- Visibility filtering (`visiblePlugins()`)
- Validation (no null, no empty id, no duplicate id)

### Registered Plugins (28 visible)

| Plugin | ID | Purpose |
|--------|-----|---------|
| OverviewPlugin | `overview` | Bus summary and slave overview |
| TopologyPlugin | `topology` | Topology graph visualization |
| OdPlugin | `od` | Object Dictionary browser |
| WatchPlugin | `watch` | SDO watch list with polling |
| FreeRunPlugin | `freerun` | Process data Free Run mode |
| RtTestPlugin | `rttest` | Real-time performance testing |
| DcSyncPlugin | `dcsync` | DC sync diagnostics |
| AlEventPlugin | `alevent` | AL event log viewer |
| StateMachinePlugin | `statemachine` | Slave state machine control |
| StartupSdoPlugin | `startupsdo` | Startup SDO configuration |
| IoVariablePlugin | `iovariable` | I/O variable monitoring |
| ConsistencyPlugin | `consistency` | Bus consistency checks |
| DiagnosticsPlugin | `diagnostics` | Host and bus diagnostics |
| SessionPlugin | `session` | Session management |
| SignalPlugin | `signal` | Multi-channel signal analyzer |
| ExportPlugin | `export` | Data export |
| NotesPlugin | `notes` | User notes |
| EsiBrowserPlugin | `esibrowser` | ESI XML browser and parser |
| BusStatsPlugin | `busstats` | Bus statistics monitoring |
| PdoMappingEditorPlugin | `pdomapping` | Visual PDO mapping editor |
| DcSyncPrecisionPlugin | `dcsyncprecision` | DC sync precision analysis |
| OnlineDiagnosticsPlugin | `onlinediagnostics` | Real-time bus monitoring |
| MultiMasterPlugin | `multimaster` | Multi-master management |
| RealtimePerformancePlugin | `realtimeperf` | Latency and throughput monitoring |
| AdvancedErrorAnalysisPlugin | `erroranalysis` | Error timeline and correlation |
| HardwareVerificationPlugin | `hardwareverification` | Pre-commissioning checks |
| DcSyncOptimizerPlugin | `dcsyncoptimizer` | DC synchronization optimization |
| FreeRunOptimizationPlugin | `freerunoptimization` | Free Run process data optimization |
| PdoMappingOptimizationPlugin | `pdomappingoptimization` | PDO mapping configuration optimization |
| SdoOptimizationPlugin | `sdooptimization` | SDO communication optimization |
| RealtimeOptimizerPlugin | `realtimeoptimizer` | Real-time performance optimization |

## Service Container

**File**: `apps/ecat-studio/services/ServiceContainer.h`

Dependency injection container holding all service instances. Passed to plugins at construction time so they can access domain services without knowing about `MainWindow` or `EcatClient` directly.

### Design

- **Single EcatClient**: All services share one TCP connection to ecatd
- **Qt parent-child tree**: All services are QObject children of the container; destruction is automatic
- **Lifetime**: Created once in MainWindow's constructor, lives for entire application lifetime
- **Thread safety**: All access from main (GUI) thread only; services marshal background I/O results via Qt signals

### Initialization Order

Services are created in dependency order:
1. Infrastructure (EcatClient, EventBus) -- no dependencies
2. Core services (Sdo, Watch, Topology) -- depend on EcatClient
3. Hardware services (DcSync, AlEvent) -- depend on EcatClient
4. Monitoring services -- depend on EventBus + EcatClient
5. Composite services -- depend on multiple services
6. Data services -- mostly standalone
7. Advanced services -- depend on EventBus + EcatClient

```
ServiceContainer
  +-- EcatClient (single instance)
  |     +-- SdoService
  |     +-- WatchService
  |     +-- TopologyService
  |     +-- DcSyncService
  |     +-- AlEventService
  |     +-- ... 40+ services all sharing the same client
  +-- EventBus (single instance, pub/sub hub)
```

## Event System

**File**: `apps/ecat-studio/services/EventBus.h`

Central signal hub for inter-plugin communication implementing a publish/subscribe pattern. Producers (services) emit events via typed convenience methods; consumers (plugins) subscribe via Qt signal-slot connections.

### Event Types

| Signal | Parameters | Purpose |
|--------|------------|---------|
| `slaveChanged` | `QVector<SlaveInfo>` | Topology scan results |
| `sdoValueReceived` | `pos, idx, sub, val` | SDO read result |
| `connectionStateChanged` | `bool connected` | Daemon connect/disconnect |
| `freeRunTelemetry` | `QJsonObject` | Free Run process data snapshot |
| `topologyChanged` | `QVector<SlaveInfo>` | Topology change detected |
| `dcSyncUpdate` | `QJsonObject` | DC sync status per slave |
| `alEvent` | `QJsonObject` | AL event log entry |
| `signalData` | `channel, values, timestamps` | Multi-channel signal data |

All services that do background I/O marshal results to the main thread before emitting through EventBus. Signal parameters use const references to minimize copying.

## Translation and i18n

The application supports 8 languages through a dual system:

### TranslationRegistry (runtime dictionary)

A `QMap<QString, QString>`-based dictionary supporting:
- Language switching without application restart
- Fallback to English for untranslated strings
- Lazy initialization with language code index

### Qt .ts/.qm System

Standard Qt linguafiles for static strings, complementing the runtime dictionary. Language files are stored in `translations/`:

| File | Language |
|------|----------|
| `nekoecat_zh.ts` | Chinese (Simplified) |
| `nekoecat_ja.ts` | Japanese |
| `nekoecat_de.ts` | German |
| `nekoecat_ko.ts` | Korean |
| `nekoecat_zh_TW.ts` | Chinese (Traditional) |
| `nekoecat_fr.ts` | French |
| `nekoecat_es.ts` | Spanish |

English is the source language and is always available without a `.ts` file.

### LanguageManager

Centralized registry providing:
- `addLanguage(name, file)` -- register a language by BCP 47 code
- `switchToLanguage(name)` -- change active language
- `currentLanguage()` / `availableLanguages()` -- query state
- Emits `languageChanged()` signal for UI updates

## Build System

### Build Targets

| Target | Type | Location | Description |
|--------|------|----------|-------------|
| `ecat_core` | Static library | `src/core/` | Domain types (SlaveInfo, JsonProtocol) |
| `ecat_igh` | Static library | `src/igh/` | IgH CLI adapter (EthercatCliBackend) |
| `ecatd` | Executable | `apps/ecatd/` | Runtime daemon (TCP server, command dispatch) |
| `ecat-studio` | Executable | `apps/ecat-studio/` | Qt6 desktop GUI |
| `release-smoke` | CTest target | `tests/` | Core tests required before release |

### Build Commands

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --build build --target release-smoke   # run release smoke tests
ctest --test-dir build --output-on-failure   # all tests
```

## Test Strategy

### Test Organization

```
tests/
  +-- *_test.cpp              # Unit tests per plugin/service/model
  +-- integration/            # Integration tests
  +-- performance/            # Performance benchmarks
  +-- fixtures/               # Test fixtures (PluginTestFixture, ServiceTestFixture, UITestFixture)
  +-- mocks/                  # Mock objects (MockEcatClient, MockEventBus, MockServiceContainer)
  +-- utils/                  # Test utilities (TestDataGenerator, TestAutomation)
```

### Test Fixtures

- **PluginTestFixture**: Creates ServiceContainer + PluginRegistry for plugin lifecycle tests
- **ServiceTestFixture**: Provides simulateConnection(), simulateSlaveChange(), simulateSdoValue()
- **UITestFixture**: Manages QWidget creation/destruction with offscreen rendering

### Mock Strategy

- **MockEcatClient**: Records all method calls, configurable canned responses, manual signal triggering
- **MockEventBus**: Records signal emissions with counting for assertions
- **MockServiceContainer**: Replaces real EcatClient and EventBus with mocks

### Test Environment

- Headless execution via `QT_QPA_PLATFORM=offscreen`
- No daemon required -- MockEcatClient eliminates need for running ecatd
- Per-test isolation with separate ServiceContainer/PluginRegistry
- AUTOMOC required for tests using Q_OBJECT macro

## Key Directories

```
apps/
  +-- ecatd/                       # Daemon
  |     +-- EcatDaemon.cpp/.h      # TCP server + dispatch logic
  |     +-- CommandDispatcher.h    # String-keyed routing table
  |     +-- FreeRunController      # ecrt process image I/O
  |     +-- RtTestController       # Real-time latency testing
  |     +-- handlers/              # Domain-specific handlers
  +-- ecat-studio/                 # GUI application
        +-- plugins/               # 80+ workspace plugin implementations
        +-- services/              # 40+ domain services
        +-- models/                # Pure data/logic types
        +-- adapters/              # Model-to-QTableWidget bridges
        +-- detail/                # Detail panel text builders
        +-- utils/                 # Reusable utilities
        +-- infra/                 # EcatClient, settings, i18n
        +-- themes/                # Theme manager and 12 .qss themes
        +-- workspaces/            # MainWindow partial implementations
src/
  +-- core/                        # Shared domain types (SlaveInfo, JsonProtocol, EcatService)
  +-- igh/                         # IgH CLI backend adapter
tests/
  +-- unit/                        # Unit tests
  +-- performance/                 # Performance benchmarks
translations/                      # Qt .ts files for 8 languages
scripts/                           # Build and utility scripts
tools/                             # Development tools
docs/                              # Project documentation
