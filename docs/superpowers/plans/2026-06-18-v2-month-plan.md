# NekoEcat Studio v2 — 4-Week Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Refactor NekoEcat Studio from God Object to plugin architecture, add DC Sync/AL Event/Adapter Selection/Signal Analyzer, achieve 100+ tests, CI/CD, v1.0.0 release.

**Architecture:** WorkspacePlugin interface + EventBus + Service layer. Each workspace becomes an independent plugin. MainWindow becomes a thin shell. Daemon migrates from CLI wrapper to direct IgH API.

**Tech Stack:** C++20, Qt6 (Core/Network/Widgets/Test), IgH EtherCAT Master API, CMake, QPainter (charts), .ts/.qm (i18n)

**Spec:** `docs/superpowers/specs/2026-06-18-v2-plugin-architecture-design.md`

---

# Week 1: Architecture Refactor

## Task 1: Create WorkspacePlugin Interface

**Files:**
- Create: `apps/ecat-studio/plugins/WorkspacePlugin.h`
- Create: `tests/workspace_plugin_interface_test.cpp`
- Modify: `apps/ecat-studio/CMakeLists.txt`

- [ ] **Step 1: Write the interface test**

```cpp
// tests/workspace_plugin_interface_test.cpp
#include <QTest>
#include "plugins/WorkspacePlugin.h"
#include <QSignalSpy>

class MockPlugin : public WorkspacePlugin {
public:
    QString id() const override { return "mock"; }
    QString displayName() const override { return "Mock"; }
    QString displayNameZh() const override { return "模拟"; }
    int defaultOrder() const override { return 0; }
    bool visible() const override { return true; }
};

class WorkspacePluginInterfaceTest : public QObject {
    Q_OBJECT
private slots:
    void testIdentity() {
        MockPlugin p;
        QCOMPARE(p.id(), "mock");
        QCOMPARE(p.displayName(), "Mock");
        QCOMPARE(p.displayNameZh(), "模拟");
        QCOMPARE(p.defaultOrder(), 0);
        QVERIFY(p.visible());
    }
    void testSignalsExist() {
        MockPlugin p;
        QSignalSpy navSpy(&p, SIGNAL(requestNavigate(QString)));
        QSignalSpy diagSpy(&p, SIGNAL(updateDiagnostics(QString,QString,QString)));
        QVERIFY(navSpy.isValid());
        QVERIFY(diagSpy.isValid());
    }
};

QTEST_MAIN(WorkspacePluginInterfaceTest)
#include "workspace_plugin_interface_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target workspace_plugin_interface_test -j4`
Expected: FAIL — header not found

- [ ] **Step 3: Create the WorkspacePlugin interface**

```cpp
// apps/ecat-studio/plugins/WorkspacePlugin.h
#pragma once
#include <QObject>
#include <QString>
#include <QWidget>
#include <QIcon>

class AppSettings;

class WorkspacePlugin : public QObject {
    Q_OBJECT
public:
    virtual ~WorkspacePlugin() = default;

    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual QString displayNameZh() const = 0;
    virtual QIcon icon() const { return QIcon(); }
    virtual QWidget *widget() = 0;
    virtual int defaultOrder() const = 0;
    virtual bool visible() const = 0;

    virtual void activate() {}
    virtual void deactivate() {}
    virtual void onSettingsChanged(const AppSettings &) {}
    virtual void onConnectionChanged(bool) {}

signals:
    void requestNavigate(const QString &pluginId);
    void updateDiagnostics(const QString &level, const QString &source, const QString &msg);
};
```

- [ ] **Step 4: Add to CMakeLists.txt**

Add `plugins/WorkspacePlugin.h` to the SOURCES list in `apps/ecat-studio/CMakeLists.txt`.
Add `tests/workspace_plugin_interface_test.cpp` to `tests/CMakeLists.txt`.

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target workspace_plugin_interface_test -j4 && ctest --test-dir build -R workspace_plugin_interface_test`
Expected: PASS

- [ ] **Step 6: Commit**

```bash
git add apps/ecat-studio/plugins/WorkspacePlugin.h tests/workspace_plugin_interface_test.cpp
git commit -m "feat: WorkspacePlugin interface + unit test"
```

---

## Task 2: Create EventBus

**Files:**
- Create: `apps/ecat-studio/services/EventBus.h`
- Create: `apps/ecat-studio/services/EventBus.cpp`
- Create: `tests/event_bus_test.cpp`
- Modify: `apps/ecat-studio/CMakeLists.txt`

- [ ] **Step 1: Write the EventBus test**

```cpp
// tests/event_bus_test.cpp
#include <QTest>
#include <QSignalSpy>
#include "services/EventBus.h"

class EventBusTest : public QObject {
    Q_OBJECT
private slots:
    void testSlaveChanged() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::slaveChanged);
        QVector<SlaveInfo> slaves;
        SlaveInfo s; s.position = 0; s.name = "Test";
        slaves.append(s);
        bus.emitSlaveChanged(slaves);
        QCOMPARE(spy.count(), 1);
    }
    void testSdoValueReceived() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
        bus.emitSdoValue(1, "0x6040", "0x00", "0x000F");
        QCOMPARE(spy.count(), 1);
        auto args = spy.at(0);
        QCOMPARE(args.at(0).toInt(), 1);
    }
    void testConnectionStateChanged() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
        bus.emitConnectionStateChanged(true);
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toBool());
    }
    void testDcSyncUpdate() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
        QJsonObject data{{"refClock", 0}, {"sync", true}};
        bus.emitDcSyncUpdate(data);
        QCOMPARE(spy.count(), 1);
    }
    void testAlEvent() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::alEvent);
        QJsonObject evt{{"slave", 1}, {"code", 0x001A}};
        bus.emitAlEvent(evt);
        QCOMPARE(spy.count(), 1);
    }
    void testSignalData() {
        EventBus bus;
        QSignalSpy spy(&bus, &EventBus::signalData);
        bus.emitSignalData(0, {1.0, 2.0, 3.0}, {100, 200, 300});
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(EventBusTest)
#include "event_bus_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target event_bus_test -j4`
Expected: FAIL

- [ ] **Step 3: Implement EventBus**

```cpp
// apps/ecat-studio/services/EventBus.h
#pragma once
#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QString>
#include "EthercatTypes.h"

class EventBus : public QObject {
    Q_OBJECT
public:
    explicit EventBus(QObject *parent = nullptr) : QObject(parent) {}

    void emitSlaveChanged(const QVector<SlaveInfo> &slaves) { emit slaveChanged(slaves); }
    void emitSdoValue(int pos, const QString &idx, const QString &sub, const QString &val) {
        emit sdoValueReceived(pos, idx, sub, val);
    }
    void emitConnectionStateChanged(bool connected) { emit connectionStateChanged(connected); }
    void emitFreeRunTelemetry(const QJsonObject &tel) { emit freeRunTelemetry(tel); }
    void emitTopologyChanged(const QVector<SlaveInfo> &slaves) { emit topologyChanged(slaves); }
    void emitDcSyncUpdate(const QJsonObject &data) { emit dcSyncUpdate(data); }
    void emitAlEvent(const QJsonObject &event) { emit alEvent(event); }
    void emitSignalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps) {
        emit signalData(channel, values, timestamps);
    }

signals:
    void slaveChanged(const QVector<SlaveInfo> &slaves);
    void sdoValueReceived(int position, const QString &index, const QString &subIndex, const QString &value);
    void connectionStateChanged(bool connected);
    void freeRunTelemetry(const QJsonObject &telemetry);
    void topologyChanged(const QVector<SlaveInfo> &slaves);
    void dcSyncUpdate(const QJsonObject &data);
    void alEvent(const QJsonObject &event);
    void signalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);
};
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build --target event_bus_test -j4 && ctest --test-dir build -R event_bus_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add apps/ecat-studio/services/EventBus.h apps/ecat-studio/services/EventBus.cpp tests/event_bus_test.cpp
git commit -m "feat: EventBus with 6 event types + unit test"
```

---

## Task 3: Create PluginRegistry

**Files:**
- Create: `apps/ecat-studio/plugins/PluginRegistry.h`
- Create: `apps/ecat-studio/plugins/PluginRegistry.cpp`
- Create: `tests/plugin_registry_test.cpp`

- [ ] **Step 1: Write the registry test**

Test: register 3 plugins, verify count/order/lookup by id, verify visibility filtering.

- [ ] **Step 2: Run test to verify it fails**

- [ ] **Step 3: Implement PluginRegistry**

```cpp
// apps/ecat-studio/plugins/PluginRegistry.h
#pragma once
#include <QVector>
#include <QMap>
#include "WorkspacePlugin.h"

class PluginRegistry {
public:
    void registerPlugin(WorkspacePlugin *plugin);
    int count() const;
    WorkspacePlugin *pluginAt(int index) const;
    WorkspacePlugin *findById(const QString &id) const;
    QVector<WorkspacePlugin *> visiblePlugins() const;

private:
    QVector<WorkspacePlugin *> plugins_;
    QMap<QString, WorkspacePlugin *> idMap_;
};
```

- [ ] **Step 4: Run test, verify PASS**

- [ ] **Step 5: Commit**

```bash
git commit -m "feat: PluginRegistry with ordering/lookup/visibility + test"
```

---

## Task 4: Create NotesPlugin (Proof of Concept)

**Files:**
- Create: `apps/ecat-studio/plugins/notes/NotesPlugin.h`
- Create: `apps/ecat-studio/plugins/notes/NotesPlugin.cpp`
- Create: `tests/notes_plugin_test.cpp`
- Modify: `apps/ecat-studio/CMakeLists.txt`

- [ ] **Step 1: Write NotesPlugin test**

Test: id="notes", displayName, widget not null, defaultOrder=100, visible=true.

- [ ] **Step 2: Run test, verify FAIL**

- [ ] **Step 3: Implement NotesPlugin**

Extract the Notes workspace from MainWindowUiBuild.cpp (the notesPage QPlainTextEdit) into a self-contained plugin. The plugin owns its QPlainTextEdit and provides load/save via `projectData()` / `loadProjectData()`.

- [ ] **Step 4: Run test, verify PASS**

- [ ] **Step 5: Integrate into MainWindow**

In MainWindow constructor: register NotesPlugin with PluginRegistry, replace direct notesPage creation with plugin->widget().

- [ ] **Step 6: Run all existing tests**

Run: `ctest --test-dir build --output-on-failure -j4`
Expected: 55/55 PASS + new tests PASS

- [ ] **Step 7: Commit**

```bash
git commit -m "feat: NotesPlugin — first plugin proof of concept"
```

---

## Task 5: Extract EventBus Wiring from MainWindow

**Files:**
- Modify: `apps/ecat-studio/MainWindow.cpp`
- Modify: `apps/ecat-studio/MainWindow.h`

- [ ] **Step 1: Add EventBus member to MainWindow**

```cpp
// MainWindow.h — add member
EventBus *eventBus_ = nullptr;
```

- [ ] **Step 2: Initialize EventBus in constructor**

In MainWindow.cpp constructor, before buildUi():
```cpp
eventBus_ = new EventBus(this);
```

- [ ] **Step 3: Wire existing EcatClient signals to EventBus**

In wire(), connect EcatClient signals to EventBus emitters:
```cpp
connect(&client_, &EcatClient::slavesChanged, eventBus_, [this](const QVector<SlaveInfo> &s) {
    eventBus_->emitSlaveChanged(s);
});
connect(&client_, &EcatClient::sdoValue, eventBus_, [this](int p, const QString &i, const QString &si, const QString &v) {
    eventBus_->emitSdoValue(p, i, si, v);
});
// ... etc for all EcatClient signals
```

- [ ] **Step 4: Run all tests, verify 55/55 PASS**

- [ ] **Step 5: Commit**

```bash
git commit -m "feat: EventBus wired to EcatClient signals"
```

---

## Task 6: Extract TranslationRegistry to .ts Files

**Files:**
- Create: `translations/nekoecat_zh.ts`
- Create: `translations/nekoecat_ja.ts`
- Create: `translations/nekoecat_de.ts`
- Create: `translations/nekoecat_ko.ts`
- Create: `translations/nekoecat_fr.ts`
- Create: `translations/nekoecat_es.ts`
- Modify: `apps/ecat-studio/CMakeLists.txt`
- Modify: `apps/ecat-studio/main.cpp`

- [ ] **Step 1: Write a Python script to convert TranslationRegistry.cpp entries to .ts XML format**

The script reads all `map_[QStringLiteral("key")] = {zh, ja, de, ko, zh-tw, fr, es}` entries and generates standard Qt .ts XML files.

- [ ] **Step 2: Run the script, generate .ts files**

- [ ] **Step 3: Add Qt6 linguist tools to CMake**

```cmake
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
qt_add_translations(ecat-studio
    TS_FILES translations/nekoecat_zh.ts translations/nekoecat_ja.ts ...
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)
```

- [ ] **Step 4: Update main.cpp to load QTranslator**

```cpp
QTranslator translator;
if (translator.load(QLocale(), "nekoecat", "_", ":/translations"))
    app.installTranslator(&translator);
```

- [ ] **Step 5: Replace uiText() calls with tr() throughout codebase**

Use sed/script to convert `uiText("English", "中文")` to `tr("English")`.

- [ ] **Step 6: Remove TranslationRegistry.cpp**

- [ ] **Step 7: Build and run all tests**

Expected: compile time drops significantly (from 90s to <10s for translation changes).

- [ ] **Step 8: Commit**

```bash
git commit -m "refactor: TranslationRegistry → Qt .ts/.qm system, remove 2023-line monolith"
```

---

## Task 7: Extract Themes to .qss Files

**Files:**
- Create: `apps/ecat-studio/themes/ThemeManager.h`
- Create: `apps/ecat-studio/themes/ThemeManager.cpp`
- Create: `apps/ecat-studio/themes/dark.qss`
- Create: `apps/ecat-studio/themes/light.qss`
- Create: `apps/ecat-studio/themes/nord.qss`
- (one .qss per theme, 12 total)
- Modify: `apps/ecat-studio/CMakeLists.txt`

- [ ] **Step 1: Write ThemeManager test**

Test: loadTheme("dark") returns non-empty QSS, availableThemes() returns 12 names.

- [ ] **Step 2: Implement ThemeManager**

```cpp
class ThemeManager {
public:
    static QStringList availableThemes();
    static QString loadTheme(const QString &name);
    static void applyTheme(QWidget *root, const QString &name);
};
```

- [ ] **Step 3: Extract each theme string from MainWindowTheme.cpp to a .qss file**

Use a script to extract each `if (theme == "Dark") { ... }` block to `dark.qss`.

- [ ] **Step 4: Register .qss as Qt resources**

Add `themes/themes.qrc` to CMakeLists.txt.

- [ ] **Step 5: Replace MainWindowTheme.cpp with ThemeManager calls**

- [ ] **Step 6: Delete MainWindowTheme.cpp**

- [ ] **Step 7: Build and run all tests**

- [ ] **Step 8: Commit**

```bash
git commit -m "refactor: themes → external .qss files, ThemeManager, delete 3104-line monolith"
```

---

## Task 8: Slim Down MainWindow

**Files:**
- Modify: `apps/ecat-studio/MainWindow.h`
- Modify: `apps/ecat-studio/MainWindow.cpp`

- [ ] **Step 1: Audit MainWindow.h methods**

For each method, determine: stays in MainWindow (UI coordination only), moves to a Service, or moves to a Plugin.

Rule of thumb:
- `buildUi()`, `wire()`, `applySettings()` → stays
- `updateTopologyView()`, `refreshSlaveInfo()` → moves to TopologyService
- `addSelectedDictionaryRowsToWatch()` → moves to WatchService
- `writeCurrentSdo()` → moves to SdoService
- All `filter*Table()` → stays (UI-only)

- [ ] **Step 2: Move SDO-related methods to SdoService**

Create `apps/ecat-studio/services/SdoService.h/.cpp`:
- `upload()`, `download()`, `readDictionary()`, `writeSdo()`
- Evidence tracking, SDO history

- [ ] **Step 3: Move Watch-related methods to WatchService**

Create `apps/ecat-studio/services/WatchService.h/.cpp`:
- `refreshWatchList()`, `addWatch()`, `removeWatch()`, `captureBaseline()`

- [ ] **Step 4: Move Topology-related methods to TopologyService**

Create `apps/ecat-studio/services/TopologyService.h/.cpp`:
- `scan()`, `rescan()`, `slaveInfo()`, `captureBaseline()`

- [ ] **Step 5: Verify MainWindow.h < 300 methods after W1**

- [ ] **Step 6: Run all tests, commit**

```bash
git commit -m "refactor: extract SdoService, WatchService, TopologyService from MainWindow"
```

---

## Task 9: Week 1 Integration Test

- [ ] **Step 1: Build all targets**

Run: `cmake --build build -j4`

- [ ] **Step 2: Run all tests**

Run: `ctest --test-dir build --output-on-failure -j4`
Expected: 55 existing + 10+ new = 65+ PASS

- [ ] **Step 3: GUI smoke test**

Run: `QT_QPA_PLATFORM=offscreen timeout 3 build/apps/ecat-studio/ecat-studio`
Expected: exit 124

- [ ] **Step 4: Verify MainWindow.h method count**

Run: `grep -c "void \|bool \|QString " apps/ecat-studio/MainWindow.h`
Expected: < 300

- [ ] **Step 5: Verify TranslationRegistry.cpp deleted**

Run: `test ! -f apps/ecat-studio/infra/TranslationRegistry.cpp && echo "OK"`
Expected: OK

- [ ] **Step 6: Verify MainWindowTheme.cpp deleted**

Run: `test ! -f apps/ecat-studio/workspaces/MainWindowTheme.cpp && echo "OK"`
Expected: OK

- [ ] **Step 7: Final Week 1 commit**

```bash
git commit -m "chore: Week 1 integration — plugin arch, EventBus, services, .ts, .qss"
```

---

# Week 2: Daemon Migration + DC Sync + AL Event + Adapter

## Task 10: Daemon — DC Sync Handler

**Files:**
- Create: `apps/ecatd/handlers/DcSyncHandler.h`
- Create: `apps/ecatd/handlers/DcSyncHandler.cpp`
- Create: `tests/dc_sync_handler_test.cpp`
- Modify: `apps/ecatd/EcatDaemon.cpp` (register handler)

- [ ] **Step 1: Write DC Sync handler test**

Test: Mock ecrt calls, verify JSON response contains refClock, sync status, drift per slave.

- [ ] **Step 2: Implement DcSyncHandler**

Uses `ecrt_master_reference_clock()` and `ecrt_slave_config_dc()` to query DC status.
Returns JSON: `{refClock: 0, slaves: [{pos: 0, dcCapable: true, syncing: true, driftNs: 120}]}`

- [ ] **Step 3: Register handler in EcatDaemon**

```cpp
dispatcher_.registerHandler("dcSyncStatus", [this](const QString &id, const QJsonObject &p) {
    return dcSyncHandler_.handle(id, p);
});
```

- [ ] **Step 4: Run tests, commit**

---

## Task 11: Daemon — AL Event Handler

**Files:**
- Create: `apps/ecatd/handlers/AlEventHandler.h/.cpp`
- Create: `tests/al_event_handler_test.cpp`

- [ ] **Step 1: Write AL Event handler test**

Test: Mock ecrt_master_ALerror(), verify JSON response with event history.

- [ ] **Step 2: Implement AlEventHandler**

Polls `ecrt_master_ALerror()` every 1s, maintains event history ring buffer (1000 entries).
Returns JSON: `{events: [{timestamp, slave, code, description}]}`

- [ ] **Step 3: Register handler, test, commit**

---

## Task 12: Daemon — Adapter Handler

**Files:**
- Create: `apps/ecatd/handlers/AdapterHandler.h/.cpp`
- Create: `tests/adapter_handler_test.cpp`

- [ ] **Step 1: Write adapter handler test**

Test: Mock /sys/class/net/ reads, verify JSON list of NICs.

- [ ] **Step 2: Implement AdapterHandler**

Reads `/sys/class/net/*/device/` for PCI devices, checks ethtool for link status.
Returns JSON: `{adapters: [{name: "eth0", mac: "...", driver: "r8169", linkUp: true}]}`

- [ ] **Step 3: Implement setAdapter handler**

Modifies IgH `/etc/ethercat.conf` master0_device, restarts ethercat service.

- [ ] **Step 4: Test, commit**

---

## Task 13: GUI — DcSyncPlugin + DcSyncService

**Files:**
- Create: `apps/ecat-studio/plugins/dcsync/DcSyncPlugin.h/.cpp`
- Create: `apps/ecat-studio/services/DcSyncService.h/.cpp`
- Create: `tests/dc_sync_service_test.cpp`

- [ ] **Step 1: Write DcSyncService test**

Test: parse JSON from daemon, emit dcSyncUpdate on EventBus, poll timer.

- [ ] **Step 2: Implement DcSyncService**

- [ ] **Step 3: Implement DcSyncPlugin UI**

Table: slave position, DC capable, syncing, drift, jitter stats.
Drift histogram widget (QPainter, last 1000 samples).

- [ ] **Step 4: Register plugin, test, commit**

---

## Task 14: GUI — AlEventPlugin + AlEventService

**Files:**
- Create: `apps/ecat-studio/plugins/alevent/AlEventPlugin.h/.cpp`
- Create: `apps/ecat-studio/services/AlEventService.h/.cpp`
- Create: `tests/al_event_service_test.cpp`

- [ ] **Step 1: Write test, implement service, implement plugin UI**

Table: timestamp, slave, error code, description. Severity filter. Auto-scroll.

- [ ] **Step 2: Test, commit**

---

## Task 15: GUI — Adapter Selection in Settings

**Files:**
- Modify: `apps/ecat-studio/infra/SettingsDialog.cpp`
- Modify: `apps/ecat-studio/infra/SettingsDialog.h`
- Create: `apps/ecat-studio/services/AdapterService.h/.cpp`

- [ ] **Step 1: Implement AdapterService**

Queries daemon `listAdapters`, caches result, provides `availableAdapters()` and `selectAdapter()`.

- [ ] **Step 2: Add adapter dropdown to Settings → EtherCAT tab**

QComboBox showing detected NICs with name, driver, link status.

- [ ] **Step 3: Test, commit**

---

## Task 16: Auto-Reconnect

**Files:**
- Modify: `apps/ecat-studio/infra/EcatClient.h/.cpp`
- Modify: `apps/ecat-studio/MainWindow.cpp`

- [ ] **Step 1: Write reconnection test**

Test: simulate disconnect, verify retry with exponential backoff, verify reconnect restores state.

- [ ] **Step 2: Add heartbeat mechanism to EcatClient**

Send ping every 5s, track consecutive failures. After 3 failures → enter "reconnecting" state.

- [ ] **Step 3: Add reconnection logic**

Exponential backoff: 2s → 4s → 8s → 16s → 30s (max). On success: rescan, restore watch.

- [ ] **Step 4: Add status bar indicator**

`Connected` (green) / `Reconnecting` (yellow, pulsing) / `Disconnected` (red).

- [ ] **Step 5: Test, commit**

---

## Task 17: Week 2 Integration

- [ ] **Step 1: Build all, run all tests**

Expected: 80+ tests PASS

- [ ] **Step 2: GUI smoke test**

- [ ] **Step 3: Commit**

---

# Week 3: Signal Analyzer + Remaining Plugins

## Task 18: Daemon — Signal Handler

**Files:**
- Create: `apps/ecatd/handlers/SignalHandler.h/.cpp`
- Create: `tests/signal_handler_test.cpp`

- [ ] **Step 1: Write signal handler test**

- [ ] **Step 2: Implement SignalHandler**

Multi-channel ring buffer (10,000 points per channel). Reads from FreeRun domain data.
JSON-RPC: `signalSubscribe` / `signalUnsubscribe` / `signalPoll`

- [ ] **Step 3: Test, commit**

---

## Task 19: GUI — SignalAnalyzerPlugin + SignalService

**Files:**
- Create: `apps/ecat-studio/plugins/signal/SignalPlugin.h/.cpp`
- Create: `apps/ecat-studio/plugins/signal/SignalChartWidget.h/.cpp`
- Create: `apps/ecat-studio/services/SignalService.h/.cpp`
- Create: `tests/signal_service_test.cpp`

- [ ] **Step 1: Write SignalService test**

Test: channel management, data buffering, statistics calculation.

- [ ] **Step 2: Implement SignalService**

```cpp
class SignalService : public QObject {
    Q_OBJECT
public:
    void addChannel(const QString &name, int slave, const QString &idx, const QString &sub);
    void removeChannel(int channelId);
    QVector<SignalChannel> channels() const;
    ChannelStats stats(int channelId) const;
};
```

- [ ] **Step 3: Implement SignalChartWidget (QPainter)**

Multi-channel scrolling line chart. Each channel gets a distinct color.
X-axis: time (seconds) or sample index. Y-axis: auto or manual range.
Max 10,000 points per channel. Configurable visible window (20-10000).

- [ ] **Step 4: Implement SignalPlugin UI**

Left panel: channel list with add/remove buttons.
Center: SignalChartWidget.
Bottom: stats overlay (min/max/avg/stddev per channel).

- [ ] **Step 5: Support multiple independent windows**

Each window is a separate SignalPlugin instance with its own channel set.

- [ ] **Step 6: Test, commit**

---

## Task 20: Convert Remaining Workspaces to Plugins

Convert each workspace to a plugin, one at a time. Order (simplest first):

1. StateMachinePlugin
2. ConsistencyPlugin
3. DiagnosticsPlugin
4. EsiRepositoryPlugin
5. RtTestPlugin
6. PdoMapPlugin
7. StartupSdoPlugin
8. IoVariablePlugin
9. WatchPlugin
10. OdPlugin (Object Dictionary)
11. FreeRunPlugin
12. OverviewPlugin

For each:
- [ ] Create plugin class implementing WorkspacePlugin
- [ ] Move UI code from MainWindow*Workspace.cpp to plugin
- [ ] Move service code to corresponding Service class
- [ ] Register plugin in PluginRegistry
- [ ] Remove old code from MainWindow
- [ ] Run all tests, verify PASS
- [ ] Commit

---

## Task 21: Week 3 Integration

- [ ] **Step 1: Build all, run all tests**

Expected: 100+ tests PASS

- [ ] **Step 2: Verify all 22 workspace tabs still functional**

- [ ] **Step 3: Commit**

---

# Week 4: Testing + CI/CD + Documentation + Release

## Task 22: Daemon Integration Tests

**Files:**
- Create: `tests/integration/daemon_integration_test.cpp`

- [ ] **Step 1: Write daemon lifecycle test**

Start daemon, connect, send ping, verify pong, disconnect, stop daemon.

- [ ] **Step 2: Write SDO round-trip test**

Connect, send upload request, verify response schema.

- [ ] **Step 3: Write DC Sync query test**

Connect, send dcSyncStatus, verify response contains expected fields.

- [ ] **Step 4: Write AL Event query test**

- [ ] **Step 5: Write adapter list test**

- [ ] **Step 6: Commit**

---

## Task 23: GUI Workflow Smoke Tests

**Files:**
- Create: `tests/integration/gui_smoke_test.cpp`

- [ ] **Step 1: Write startup smoke test**

Launch app, verify MainWindow appears, verify all tabs load, verify no crashes.

- [ ] **Step 2: Write settings dialog smoke test**

Open settings, change theme, change language, verify apply.

- [ ] **Step 3: Write command palette smoke test**

Open palette (Ctrl+P), type search, verify results appear.

- [ ] **Step 4: Commit**

---

## Task 24: CI/CD Pipeline

**Files:**
- Create: `.github/workflows/ci.yml`

- [ ] **Step 1: Write GitHub Actions workflow**

```yaml
name: CI
on: [push, pull_request]
jobs:
  build-and-test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install Qt6
        run: sudo apt-get install -y qt6-base-dev qt6-tools-dev qt6-l10n-tools
      - name: Install IgH headers
        run: sudo apt-get install -y ethercat-master-dev || true
      - name: Configure
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      - name: Build
        run: cmake --build build -j$(nproc)
      - name: Test
        run: ctest --test-dir build --output-on-failure
      - name: GUI Smoke
        run: QT_QPA_PLATFORM=offscreen timeout 10 build/apps/ecat-studio/ecat-studio || true
```

- [ ] **Step 2: Push and verify CI passes**

- [ ] **Step 3: Commit**

---

## Task 25: Documentation

- [ ] **Step 1: Update README.md with v2 features**

Add DC Sync, AL Event, Adapter Selection, Signal Analyzer to feature list.

- [ ] **Step 2: Update User Manual (MainWindowManual.cpp)**

Add sections for DC Sync, AL Event, Signal Analyzer, Adapter Selection.

- [ ] **Step 3: Write ARCHITECTURE.md**

Document plugin system, EventBus, Service layer, daemon protocol.

- [ ] **Step 4: Write CONTRIBUTING.md**

Plugin development guide: how to create a new workspace plugin.

- [ ] **Step 5: Commit**

---

## Task 26: Release v1.0.0

- [ ] **Step 1: Update version in CMakeLists.txt**

```cmake
project(NekoEcatStudio VERSION 1.0.0 LANGUAGES CXX)
```

- [ ] **Step 2: Build release binary**

```bash
cmake --build build --config Release -j4
```

- [ ] **Step 3: Create release package**

```bash
tar czf NekoEcat-Studio-1.0.0-linux-x86_64.tar.gz -C build/apps/ecat-studio ecat-studio -C ../../apps/ecatd ecatd
```

- [ ] **Step 4: Create GitHub release**

```bash
export all_proxy=http://127.0.0.1:7890
gh release create v1.0.0 NekoEcat-Studio-1.0.0-linux-x86_64.tar.gz \
  --title "NekoEcat Studio v1.0.0" \
  --notes-file RELEASE_NOTES.md
```

- [ ] **Step 5: Final verification**

Run all tests, GUI smoke, manual check of all 22 tabs.

---

## Task 27: Final Cleanup

- [ ] **Step 1: Remove all dead code**

Run: `grep -rn "TODO\|FIXME\|HACK" apps/ecat-studio/` — should be empty.

- [ ] **Step 2: Verify MainWindow.h < 200 methods**

- [ ] **Step 3: Verify no file > 1000 lines (except tests)**

- [ ] **Step 4: Final commit and push**

```bash
git add -A && git commit -m "chore: v1.0.0 release cleanup"
git push origin main
```
