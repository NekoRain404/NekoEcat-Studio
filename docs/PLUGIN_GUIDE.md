# NekoEcat Studio Plugin Development Guide

## Overview

The plugin system allows adding new workspace tabs to NekoEcat Studio.
Each plugin implements the `WorkspacePlugin` interface and registers with
`PluginRegistry`. Plugins communicate through `EventBus` (pub/sub) and
access domain logic via `ServiceContainer`.

```
MainWindow
  ├── PluginRegistry          (ordered plugin list, lookup by id)
  ├── ServiceContainer         (holds all services + EcatClient)
  │     ├── EventBus           (inter-plugin signal hub)
  │     ├── SdoService
  │     ├── TopologyService
  │     ├── SignalService
  │     └── ... (registered domain services)
  └── WorkspacePlugin[]        (each owns a QWidget tab)
```

**ServiceContainer Details**:
- **Ownership**: All services are QObject children of the container; Qt handles cleanup
- **Lifetime**: Single instance created in MainWindow, lives for entire application
- **Dependency Injection**: Passed to plugin constructors for service access
- **Thread Safety**: Main (GUI) thread only; background I/O services marshal results back
- **Initialization Order**: Infrastructure → Core → Hardware → Monitoring → Composite → Data → Advanced

## Prerequisites

- C++20 compiler (GCC 12+, Clang 15+)
- Qt 6 development packages (`qt6-base-dev`, `qt6-network-dev`)
- CMake 3.22+
- Familiarity with Qt signals/slots and the `Q_OBJECT` macro

## Step 1: Create Plugin Directory

Create a new directory under `apps/ecat-studio/plugins/`:

```
apps/ecat-studio/plugins/myplugin/
  ├── MyPlugin.h
  └── MyPlugin.cpp
```

Follow the naming convention: PascalCase directory and files.

## Step 2: Implement the WorkspacePlugin Interface

### Header — `MyPlugin.h`

```cpp
#pragma once

#include "plugins/WorkspacePlugin.h"

class ServiceContainer;

class MyPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit MyPlugin(ServiceContainer *container, QObject *parent = nullptr);

  // Identity (required)
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;

  // UI (required)
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  // Lifecycle (optional overrides)
  void activate() override;
  void deactivate() override;
  void onConnectionChanged(bool connected) override;

private:
  void buildUi();

  ServiceContainer *container_;
  QWidget *containerWidget_ = nullptr;
};
```

### Implementation — `MyPlugin.cpp`

```cpp
#include "MyPlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"

#include <QLabel>
#include <QVBoxLayout>

MyPlugin::MyPlugin(ServiceContainer *container, QObject *parent)
    : container_(container) {
  if (parent) setParent(parent);
  buildUi();

  // Subscribe to EventBus events
  connect(container_->eventBus(), &EventBus::connectionStateChanged,
          this, &MyPlugin::onConnectionChanged);
}

// ── Identity ──────────────────────────────────────────────────────────
QString MyPlugin::id() const { return "myplugin"; }
QString MyPlugin::displayName() const { return "My Plugin"; }
QString MyPlugin::displayNameZh() const { return QStringLiteral("我的插件"); }

// ── UI ────────────────────────────────────────────────────────────────
QWidget *MyPlugin::widget() { return containerWidget_; }
int MyPlugin::defaultOrder() const { return 50; }
bool MyPlugin::visible() const { return true; }

// ── Lifecycle ─────────────────────────────────────────────────────────
void MyPlugin::activate() {
  // Called when the user switches to this tab
}

void MyPlugin::deactivate() {
  // Called when the user switches away from this tab
}

void MyPlugin::onConnectionChanged(bool connected) {
  // React to EtherCAT connection state changes
}

// ── UI construction ───────────────────────────────────────────────────
void MyPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(new QLabel(tr("My Plugin Content")));
}
```

### Constructor Patterns

Two styles are used in the codebase:

| Style | Constructor | When to use |
|-------|------------|-------------|
| **Container injection** | `(ServiceContainer *container, QObject *parent)` | New plugins — access any service via `container->sdo()`, `container->eventBus()`, etc. |
| **Fine-grained injection** | `(EventBus *bus, FooService *svc, QObject *parent)` | Older plugins that only need specific services |

Prefer container injection for new plugins — it decouples your plugin
from service count changes.

### Pure Virtual Methods (must override)

| Method | Purpose |
|--------|---------|
| `id()` | Unique lowercase identifier (e.g. `"myplugin"`) |
| `displayName()` | English tab name |
| `displayNameZh()` | Chinese tab name (8 languages supported) |
| `widget()` | Root QWidget for the tab content |
| `defaultOrder()` | Tab ordering (lower = left). See existing values below. |
| `visible()` | Whether the tab appears (can be dynamic) |

### Lifecycle Hooks (optional overrides)

| Method | Trigger |
|--------|---------|
| `activate()` | User switches to this tab |
| `deactivate()` | User switches away |
| `onSettingsChanged(settings)` | User changes app settings |
| `onConnectionChanged(connected)` | EtherCAT link goes up/down |

### Signals (inherited)

| Signal | Purpose |
|--------|---------|
| `requestNavigate(pluginId)` | Ask MainWindow to switch to another plugin tab |
| `updateDiagnostics(level, source, msg)` | Emit a diagnostic message |

## Step 3: Register the Plugin

In `MainWindow.cpp`, add registration after `PluginRegistry` creation:

```cpp
pluginRegistry_ = new PluginRegistry();
pluginRegistry_->registerPlugin(new MyPlugin(serviceContainer, this));
```

`PluginRegistry` guards against null, empty id, and duplicate id.
After registration, plugins are sorted by `defaultOrder()` ascending.

**Registration Process Details**:
1. Plugin is validated (not null, non-empty id, no duplicate id)
2. Plugin is added to the internal vector and map
3. Vector is sorted by defaultOrder() to maintain consistent tab order
4. MainWindow uses the registry to create tabs in the correct order

**Plugin Ordering**: Lower `defaultOrder()` values appear first (leftmost tabs). See the existing order values at the bottom of this guide.

## Step 4: Connect to EventBus

`EventBus` is the central pub/sub hub. Connect to signals in your
constructor or `activate()` method.

**Architecture**: EventBus implements a publish/subscribe pattern. Producers
(services) emit events via convenience wrapper methods, and consumers (plugins)
subscribe via Qt signal-slot connections. All subscribers receive all events
(no filtering or routing).

### Listening to Events

```cpp
// In constructor or buildUi()
connect(container_->eventBus(), &EventBus::slaveChanged,
        this, [this](const QVector<SlaveInfo> &slaves) {
          // Update UI with new slave list
        });

connect(container_->eventBus(), &EventBus::sdoValueReceived,
        this, [this](int pos, const QString &idx,
                      const QString &sub, const QString &val) {
          // Handle SDO value update
        });
```

### Emitting Events

Call the corresponding `emit` method on EventBus:

```cpp
container_->eventBus()->emitConnectionStateChanged(true);
```

### Available Event Channels

| Signal | Emitter Method | Payload | Description |
|--------|---------------|---------|-------------|
| `slaveChanged` | `emitSlaveChanged()` | `QVector<SlaveInfo>` | Topology scan results available |
| `sdoValueReceived` | `emitSdoValue()` | `int, QString, QString, QString` | SDO value read from slave |
| `connectionStateChanged` | `emitConnectionStateChanged()` | `bool` | Daemon connection state changed |
| `freeRunTelemetry` | `emitFreeRunTelemetry()` | `QJsonObject` | Free Run process data snapshot |
| `topologyChanged` | `emitTopologyChanged()` | `QVector<SlaveInfo>` | Topology change detected |
| `dcSyncUpdate` | `emitDcSyncUpdate()` | `QJsonObject` | DC sync status per slave |
| `alEvent` | `emitAlEvent()` | `QJsonObject` | AL event log entry |
| `signalData` | `emitSignalData()` | `int, QVector<double>, QVector<qint64>` | Multi-channel signal data |

**Thread Safety**: All event emission and subscription should happen on the main
(GUI) thread. Services that do background I/O must marshal results to the main
thread before emitting through EventBus.

**Performance Notes**: Signal emission is O(n) where n is the number of connected
slots. For high-frequency events (e.g., signalData at 1kHz), producers should
consider batching or throttling. Signal parameters use const references to
minimize data copying.

## Step 5: Add to CMakeLists.txt

In `apps/ecat-studio/CMakeLists.txt`, add your source file to the
`add_executable(ecat-studio ...)` sources list under the plugins section:

```cmake
# Plugins
plugins/PluginRegistry.cpp
plugins/notes/NotesPlugin.cpp
plugins/topology/TopologyPlugin.cpp
plugins/myplugin/MyPlugin.cpp          # <-- add here
```

If your plugin lives in a subdirectory, also add the include path:

```cmake
target_include_directories(ecat-studio PRIVATE
    plugins/myplugin
)
```

## Step 6: Write Tests

Create `tests/myplugin_test.cpp`:

```cpp
#include <QTest>
#include "plugins/myplugin/MyPlugin.h"
#include "services/ServiceContainer.h"

class MyPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testIdentity() {
    ServiceContainer svc;
    MyPlugin p(&svc);
    QCOMPARE(p.id(), QString("myplugin"));
    QCOMPARE(p.displayName(), QString("My Plugin"));
    QCOMPARE(p.displayNameZh(), QString("我的插件"));
  }

  void testDefaultOrder() {
    ServiceContainer svc;
    MyPlugin p(&svc);
    QCOMPARE(p.defaultOrder(), 50);
  }

  void testVisible() {
    ServiceContainer svc;
    MyPlugin p(&svc);
    QVERIFY(p.visible());
  }

  void testWidgetNotNull() {
    ServiceContainer svc;
    MyPlugin p(&svc);
    QVERIFY(p.widget() != nullptr);
  }
};

QTEST_MAIN(MyPluginTest)
#include "myplugin_test.moc"
```

Add the test to `tests/CMakeLists.txt`:

```cmake
add_executable(myplugin_test myplugin_test.cpp
    ../apps/ecat-studio/plugins/myplugin/MyPlugin.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    # ... service sources as needed
)
target_link_libraries(myplugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(myplugin_test PRIVATE
    ../apps/ecat-studio
    ../apps/ecat-studio/plugins/myplugin
)
set_target_properties(myplugin_test PROPERTIES AUTOMOC ON)
add_test(NAME myplugin_test COMMAND myplugin_test)
```

Run with `QT_QPA_PLATFORM=offscreen` for headless environments.

## Plugin Lifecycle

```
Construction      →  buildUi(), connect to EventBus
       ↓
Registration      →  PluginRegistry::registerPlugin()
       ↓
Tab creation      →  MainWindow calls widget() to embed in QTabWidget
       ↓
Activation        →  User selects this tab → activate()
       ↓
Event handling    →  React to EventBus signals (slaveChanged, etc.)
       ↓
Deactivation      →  User switches away → deactivate()
       ↓
Settings change   →  onSettingsChanged() if user modifies AppSettings
       ↓
Connection change →  onConnectionChanged(true/false) on link state
```

## Best Practices

- **Lazy UI construction**: Build the widget in the constructor (not on
  first `widget()` call) — the registry and MainWindow expect immediate
  availability.
- **Service access via ServiceContainer**: Never reach into MainWindow
  internals. Use `container_->sdo()`, `container_->topology()`, etc.
- **Event-driven communication**: Use EventBus signals instead of direct
  pointer coupling between plugins.
- **Thread safety**: All UI work happens on the main thread. Services
  that touch the network (`EcatClient`) marshal callbacks to the main
  thread internally.
- **Testing without hardware**: `ServiceContainer` can be instantiated
  in tests without a running `ecatd`. Connect to EventBus signals and
  emit test data directly.
- **Member naming**: Use trailing underscore for private members
  (`container_`, `bus_`, `graph_`).
- **Include convention**: Use bare header names — CMake adds
  subdirectories to the include path.

## Existing Plugin Examples

| Plugin | Complexity | Constructor | Key Features |
|--------|-----------|-------------|--------------|
| `NotesPlugin` | Simple | `(QObject*)` | Self-contained, no services, plain QPlainTextEdit |
| `EsiBrowserPlugin` | Medium | `(EsiService*, QObject*)` | Fine-grained service injection, ESI parser and matcher |
| `BusStatsPlugin` | Medium | `(BusStatsService*, QObject*)` | Fine-grained injection, live stats via timer |
| `TopologyPlugin` | Complex | `(EventBus*, QObject*)` | EventBus subscription, custom graph widget, layout modes |
| `SignalPlugin` | Complex | `(SignalService*, QObject*)` | Real-time chart, channel management, data streaming |
| `DcSyncPlugin` | Complex | `(EventBus*, DcSyncService*, QObject*)` | EventBus + service, JSON data display |
| `DashboardPlugin` | Complex | `(ChartService*, QObject*)` | Configurable gauges, counters, sparklines in grid layout |
| `ChartPlugin` | Medium | `(ChartService*, QObject*)` | Line/bar/pie/scatter/gauge charts, data source selection, export |
| `AutomationPlugin` | Complex | `(ScriptingService*, QObject*)` | Script editor, console output, template insertion, run/stop controls |
| `ProtocolAnalyzerPlugin` | Complex | `(ProtocolAnalyzerService*, QObject*)` | Frame capture, protocol decode, filtering, PCAP export |
| `ProjectPlugin` | Medium | `(ProjectManagerService*, ConfigurationService*, QObject*)` | Project tree, config pages, import/export |
| `AlarmPlugin` | Medium | `(AlarmService*, LoggingService*, QObject*)` | Alarm table, filtering, acknowledge/clear actions, history export |
| `OscilloscopePlugin` | Complex | `(OscilloscopeService*, QObject*)` | Multi-channel scope, timebase, trigger modes, cursor, FFT |
| `DataPipelinePlugin` | Medium | `(DataPipelineService*, QObject*)` | Pipeline configuration, stage management, monitoring |
| `DeviceManagerPlugin` | Medium | `(DeviceManagerService*, QObject*)` | Device discovery, configuration, status monitoring |
| `MasterManagerPlugin` | Complex | `(MasterManagerService*, DistributedClockService*, QObject*)` | Master info, diagnostics, restart, log viewer |
| `SdoCachePlugin` | Medium | `(SdoCacheService*, QObject*)` | SDO cache management and statistics |
| `ThemeCustomizerPlugin` | Simple | `(QObject*)` | UI theme customization |
| `KeyboardShortcutsPlugin` | Simple | `(QObject*)` | Keyboard shortcut configuration |
| `UserPreferencesPlugin` | Simple | `(QObject*)` | User preference management |
| `TracePlugin` | Medium | `(TraceService*, QObject*)` | EtherCAT frame trace capture |
| `LogicAnalyzerPlugin` | Medium | `(TraceService*, QObject*)` | Logic analyzer for digital signals |
| `DiagramPlugin` | Medium | `(QObject*)` | Network topology diagram editor |
| `FormulaPlugin` | Simple | `(QObject*)` | Formula calculator for process data |
| `ScriptLibraryPlugin` | Simple | `(QObject*)` | Automation script library |
| `SimulationPlugin` | Medium | `(QObject*)` | EtherCAT bus simulation |
| `CalibrationPlugin` | Medium | `(QObject*)` | Device calibration tools |
| `DocumentationPlugin` | Simple | `(QObject*)` | Integrated documentation browser |
| `WizardPlugin` | Medium | `(QObject*)` | Setup wizard for common tasks |
| `TemplatePlugin` | Simple | `(QObject*)` | Project template management |
| `ReportPlugin` | Medium | `(QObject*)` | Report generation and export |
| `DashboardDesignerPlugin` | Complex | `(QObject*)` | Custom dashboard layout designer |
| `AlarmManagerPlugin` | Medium | `(QObject*)` | Advanced alarm management and rules |
| `DataLoggerPlugin` | Medium | `(QObject*)` | High-speed data logging to file |
| `WorkflowDesignerPlugin` | Complex | `(QObject*)` | Commissioning workflow designer |
| `TestSuitePlugin` | Medium | `(QObject*)` | Automated test suite runner |
| `DeploymentPlugin` | Medium | `(QObject*)` | Device deployment and provisioning |
| `ConfigurationEditorPlugin` | Medium | `(QObject*)` | Advanced configuration editor |
| `NetworkAnalyzerPlugin` | Medium | `(QObject*)` | Network traffic analyzer |
| `SystemMonitorPlugin` | Medium | `(QObject*)` | System resource monitoring |
| `SecurityManagerPlugin` | Medium | `(QObject*)` | Security policy management |
| `ComplianceCheckerPlugin` | Medium | `(QObject*)` | EtherCAT compliance checking |
| `CertificationManagerPlugin` | Medium | `(QObject*)` | Device certification management |
| `OptimizationDashboardPlugin` | Complex | `(QObject*)` | Performance optimization dashboard |
| `MonitoringDashboardPlugin` | Complex | `(QObject*)` | Real-time monitoring dashboard |
| `AnalyticsDashboardPlugin` | Complex | `(QObject*)` | Data analytics dashboard |
| `DeploymentManagerPlugin` | Complex | `(QObject*)` | Multi-device deployment management |
| `UpdateManagerPlugin` | Medium | `(QObject*)` | Firmware update management |
| `MaintenanceSchedulerPlugin` | Medium | `(QObject*)` | Maintenance scheduling |
| `IntegrationHubPlugin` | Complex | `(QObject*)` | Third-party integration hub |
| `SyncManagerPlugin` | Medium | `(QObject*)` | Sync manager configuration |
| `ReplicationManagerPlugin` | Medium | `(QObject*)` | Configuration replication |
| `VisualizationStudioPlugin` | Complex | `(QObject*)` | Advanced data visualization |
| `ReportDesignerPlugin` | Complex | `(QObject*)` | Custom report designer |
| `DocumentationBrowserPlugin` | Medium | `(QObject*)` | Documentation browser |
| `CloudManagerPlugin` | Complex | `(QObject*)` | Experimental opt-in: Cloud connectivity management |
| `EdgeComputingPlugin` | Complex | `(QObject*)` | Experimental opt-in: Edge computing management |
| `AIAssistantPlugin` | Complex | `(QObject*)` | Experimental opt-in: AI-powered diagnostic assistant |
| `DigitalTwinStudioPlugin` | Complex | `(QObject*)` | Experimental opt-in: Digital twin modeling and management |
| `BlockchainExplorerPlugin` | Complex | `(QObject*)` | Experimental opt-in: Blockchain audit log explorer |
| `QuantumSecurityPlugin` | Complex | `(QObject*)` | Experimental opt-in: Quantum-resistant security management |
| `WorkflowOptimizerPlugin` | Complex | `(WorkflowAnalyticsService*, QObject*)` | Workflow optimization recommendations |
| `WorkflowDashboardPlugin` | Complex | `(WorkflowMonitoringService*, QObject*)` | Workflow monitoring dashboard |
| `PdoMappingEditorPlugin` | Complex | `(PdoMappingService*, QObject*)` | Visual PDO mapping with canvas, validator, and export |
| `DcSyncPrecisionPlugin` | Complex | `(DcSyncPrecisionService*, QObject*)` | DC sync drift monitoring and jitter analysis |
| `OnlineDiagnosticsPlugin` | Complex | `(OnlineDiagnosticsService*, QObject*)` | Real-time bus monitoring and error analysis |
| `MultiMasterPlugin` | Complex | `(MultiMasterService*, QObject*)` | Multi-master management and comparison |
| `RealtimePerformancePlugin` | Complex | `(RealtimePerformanceService*, QObject*)` | Latency and throughput monitoring |
| `AdvancedErrorAnalysisPlugin` | Complex | `(AdvancedErrorAnalysisService*, QObject*)` | Error timeline and correlation analysis |
| `HardwareVerificationPlugin` | Complex | `(HardwareVerificationService*, QObject*)` | Device and network verification |

## Existing defaultOrder Values

| Order | Plugin |
|-------|--------|
| 5 | Overview |
| 10 | Topology |
| 15 | (available) |
| 20 | Object Dictionary |
| 25 | (available) |
| 30 | Watch |
| 35 | FreeRun, StartupSdo |
| 40 | IoVariable |
| 50 | (available — use for new plugins) |
| 60 | DcSync, StateMachine |
| 65 | AlEvent |
| 67 | Consistency, Signal |
| 70 | Diagnostics |
| 75 | RtTest |
| 80 | Session |
| 85 | Export |
| 90 | DataPipeline, Esi |
| 95 | BusStats, DeviceManager |
| 100 | Notes, Oscilloscope |
| 105 | Protocol Analyzer |
| 110 | Alarm |
| 115 | (available) |
| 120 | Automation |
| 125 | Chart |
| 130 | Dashboard |
| 135 | Network |
| 140 | SdoCache, Trace, LogicAnalyzer |
| 145 | Diagram, Formula, ScriptLibrary |
| 150 | Simulation, Calibration, Documentation |
| 155 | MasterManager |
| 160 | Wizard, Template, Report |
| 165 | DashboardDesigner, AlarmManager, DataLogger |
| 170 | WorkflowDesigner, TestSuite, Deployment |
| 175 | ConfigurationEditor, NetworkAnalyzer, SystemMonitor |
| 180 | SecurityManager, ComplianceChecker, CertificationManager |
| 185 | OptimizationDashboard, MonitoringDashboard, AnalyticsDashboard |
| 190 | DeploymentManager, UpdateManager, MaintenanceScheduler |
| 195 | IntegrationHub, SyncManager, ReplicationManager |
| 200 | VisualizationStudio, ReportDesigner, DocumentationBrowser |
| 205 | Experimental opt-in: CloudManager, EdgeComputing, AIAssistant |
| 210 | Experimental opt-in: DigitalTwinStudio, BlockchainExplorer, QuantumSecurity |
| 215 | ThemeCustomizer, KeyboardShortcuts, UserPreferences |
| 220 | WorkflowOptimizer, WorkflowDashboard |
| 225 | PdoMappingEditor, DcSyncPrecision |
| 230 | OnlineDiagnostics, MultiMaster |
| 235 | RealtimePerformance, AdvancedErrorAnalysis |
| 240 | HardwareVerification |

## File Reference

| File | Purpose |
|------|---------|
| `plugins/WorkspacePlugin.h` | Base interface all plugins implement |
| `plugins/PluginRegistry.h/.cpp` | Plugin manager — registration, lookup, ordering |
| `services/ServiceContainer.h/.cpp` | Holds all service instances |
| `services/EventBus.h/.cpp` | Central pub/sub signal hub |
| `services/*.h/.cpp` | Domain services (Sdo, Topology, Signal, etc.) |
| `infra/EcatClient.h` | TCP client to `ecatd` daemon |
| `EthercatTypes.h` | Shared POD types (`SlaveInfo`, etc.) |

## Error Handling Best Practices

### Null Pointer Checks

Always check service pointers before use, especially in lifecycle methods:

```cpp
void MyPlugin::onConnectionChanged(bool connected) {
  if (!container_->topology()) return;
  // ... proceed with topology operations
}
```

### Graceful Degradation

Plugins should remain functional even when optional services are unavailable:

```cpp
QWidget *MyPlugin::widget() {
  if (!containerWidget_) {
    containerWidget_ = new QWidget;
    auto *layout = new QVBoxLayout(containerWidget_);
    layout->addWidget(new QLabel(tr("Service unavailable")));
  }
  return containerWidget_;
}
```

### Error Logging

Use `updateDiagnostics` to report errors to the Diagnostics panel:

```cpp
emit updateDiagnostics("error", "MyPlugin",
                       tr("Failed to load data: %1").arg(errorString));
```

### Safety Validation

For operations that modify bus state, use `SafetyController`:

```cpp
auto result = container_->safety()->validateStateTransition(currentState, targetState);
if (!result.allowed) {
  emit updateDiagnostics("warning", "MyPlugin", result.reason);
  return;
}
```

## Troubleshooting

### Plugin tab not appearing

1. Verify `visible()` returns `true`
2. Check that `registerPlugin()` is called in MainWindow
3. Ensure no duplicate `id()` values exist
4. Check `defaultOrder()` doesn't conflict with existing plugins

### Widget not rendering

1. Ensure `widget()` returns a non-null QWidget
2. Check that `buildUi()` is called in the constructor
3. Verify Qt parent-child ownership is set correctly
4. For headless testing, set `QT_QPA_PLATFORM=offscreen`

### EventBus events not received

1. Verify `connect()` is called before the event is emitted
2. Check signal signature matches exactly (parameter types and order)
3. Ensure the emitting service is initialized before subscribers connect
4. For cross-thread events, verify marshaling to main thread

### Service access returning null

1. Verify the service is registered in `ServiceContainer`
2. Check that `ServiceContainer` is constructed before plugins
3. For optional services (e.g., `ScriptingService`), always null-check
4. Verify `ECAT_SCRIPTING_ENABLED` is defined if using scripting features

### Test failures with offscreen platform

1. Ensure `QT_QPA_PLATFORM=offscreen` is set in test environment
2. Widget-based tests require `Qt6::Widgets` linkage
3. AUTOMOC must be enabled for tests using Q_OBJECT
4. Verify all source files are listed in the test's `add_executable`

## FAQ

### Q: How do I access a service from my plugin?

Use the ServiceContainer passed to your constructor:
```cpp
// Access SDO service:
container_->sdo()->upload(pos, idx, sub);

// Access EventBus:
container_->eventBus()->emitSlaveChanged(slaves);

// Access TopologyService:
auto slaves = container_->topology()->currentSlaves();
```

### Q: How do I communicate with other plugins?

Use EventBus for loose-coupled communication:
```cpp
// Subscribe to events:
connect(container_->eventBus(), &EventBus::slaveChanged,
        this, &MyPlugin::onSlavesChanged);

// Emit events:
container_->eventBus()->emitConnectionStateChanged(true);
```

### Q: How do I add a new service?

1. Create the service class in `apps/ecat-studio/services/`
2. Add it to ServiceContainer.h (forward declaration + accessor + member)
3. Add it to ServiceContainer.cpp (include + initialization)
4. Use EventBus for cross-service communication

### Q: What's the difference between emitXxx() and the signal?

`emitXxx()` is a convenience wrapper around the Qt signal. It provides a clean API
boundary and allows future enhancements (logging, metrics) without changing the
signal interface. Always use the emit methods instead of emitting signals directly.

### Q: How do I handle thread safety?

All UI work happens on the main thread. Services that touch the network (EcatClient)
marshal callbacks to the main thread internally. If you need to access services from
a worker thread, use Qt's queued connections or QTimer::singleShot to marshal back
to the main thread.

### Q: How do I test my plugin without hardware?

ServiceContainer can be instantiated in tests without a running ecatd:
```cpp
ServiceContainer svc;
MyPlugin p(&svc);
 QVERIFY(p.widget() != nullptr);
```
Connect to EventBus signals and emit test data directly to simulate events.

### Q: How do I add a canvas-based widget to my plugin?

For plugins like PdoMappingEditorPlugin that use custom canvas rendering:
```cpp
// In buildUi():
auto *canvas = new MyCanvasWidget(containerWidget_);
auto *layout = new QVBoxLayout(containerWidget_);
layout->addWidget(canvas);

// Connect mouse events for interaction
connect(canvas, &MyCanvasWidget::itemSelected, this, [this](int id) {
  // Handle canvas item selection
});
```
Use QPainter for custom rendering and handle mouse events for interaction.

### Q: How do I implement multi-widget plugins?

Some plugins (like OnlineDiagnosticsPlugin, DcSyncPrecisionPlugin) contain multiple sub-widgets:
```cpp
void MyPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  
  auto *tabWidget = new QTabWidget;
  tabWidget->addTab(new MonitorWidget(), tr("Monitor"));
  tabWidget->addTab(new AnalysisWidget(), tr("Analysis"));
  
  layout->addWidget(tabWidget);
}
```

## Testing Infrastructure

The test suite provides fixtures, mocks, and utilities for comprehensive plugin testing.

### Test Fixtures

| Fixture | File | Purpose |
|---------|------|---------|
| `PluginTestFixture` | `tests/fixtures/PluginTestFixture.h` | ServiceContainer + PluginRegistry for plugin lifecycle tests |
| `ServiceTestFixture` | `tests/fixtures/ServiceTestFixture.h` | Simulated EtherCAT events for service-layer tests |
| `UITestFixture` | `tests/fixtures/UITestFixture.h` | Widget creation/destruction with offscreen rendering |

### Mock Objects

| Mock | File | Purpose |
|------|------|---------|
| `MockEcatClient` | `tests/mocks/MockEcatClient.h` | Records method calls, configurable responses, signal triggers |
| `MockEventBus` | `tests/mocks/MockEventBus.h` | Records signal emissions with arguments for assertion |
| `MockServiceContainer` | `tests/mocks/MockServiceContainer.h` | ServiceContainer with mock EcatClient and EventBus |

### Test Utilities

| Utility | File | Purpose |
|---------|------|---------|
| `TestDataGenerator` | `tests/utils/TestDataGenerator.h` | Generates SlaveInfo, SDO values, PDO mappings with patterns |
| `TestAutomation` | `tests/utils/TestAutomation.h` | Automated UI test helpers |

### Writing Plugin Tests

```cpp
#include <QTest>
#include "plugins/myplugin/MyPlugin.h"
#include "tests/mocks/MockServiceContainer.h"

class MyPluginTest : public QObject {
  Q_OBJECT
private:
  MockServiceContainer *container_ = nullptr;

private slots:
  void initTestCase() {
    container_ = new MockServiceContainer(this);
  }

  void testIdentity() {
    MyPlugin p(container_);
    QCOMPARE(p.id(), QString("myplugin"));
    QCOMPARE(p.displayName(), QString("My Plugin"));
    QCOMPARE(p.defaultOrder(), 50);
    QVERIFY(p.visible());
  }

  void testWidgetNotNull() {
    MyPlugin p(container_);
    QVERIFY(p.widget() != nullptr);
  }

  void testEventBusIntegration() {
    MyPlugin p(container_);
    container_->mockClient()->setConnected(true);
    container_->mockClient()->triggerSlavesChanged({});
    QCOMPARE(container_->mockEventBus()->signalCount("connectionStateChanged"), 1);
  }
};

QTEST_MAIN(MyPluginTest)
#include "myplugin_test.moc"
```
