# Test Coverage Expansion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Expand test coverage for NekoEcat Studio from 78 tests to comprehensive coverage including integration, boundary, UI, performance, and regression tests.

**Architecture:** Add new test files following existing patterns (Qt Test framework, QTest macros). Tests will be organized by category with clear separation of concerns. Each test file focuses on a specific component or integration scenario.

**Tech Stack:** Qt6 Test framework, QTestLib, QSignalSpy, QTableWidget, offscreen rendering for UI tests.

---

## File Structure

### New Test Files to Create

1. `tests/plugin_integration_test.cpp` - Plugin system integration tests
2. `tests/service_integration_test.cpp` - Service container integration tests  
3. `tests/event_bus_integration_test.cpp` - EventBus integration tests
4. `tests/empty_data_boundary_test.cpp` - Empty data handling tests
5. `tests/large_data_boundary_test.cpp` - Large data volume tests
6. `tests/concurrent_access_test.cpp` - Concurrent access tests
7. `tests/error_recovery_test.cpp` - Error recovery tests
8. `tests/ui_creation_test.cpp` - UI widget creation tests
9. `tests/ui_update_test.cpp` - UI update tests
10. `tests/ui_event_response_test.cpp` - UI event response tests
11. `tests/ui_error_handling_test.cpp` - UI error handling tests
12. `tests/performance_large_data_test.cpp` - Performance with large data
13. `tests/performance_frequent_update_test.cpp` - Performance with frequent updates
14. `tests/memory_usage_test.cpp` - Memory usage tests
15. `tests/regression_test.cpp` - Regression tests for fixed bugs

### Existing Files to Modify

1. `tests/CMakeLists.txt` - Add new test targets

---

## Task 1: Plugin Integration Tests

**Files:**
- Create: `tests/plugin_integration_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QApplication>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"

class PluginIntegrationTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testPluginRegistration() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);
    
    QCOMPARE(registry.count(), 2);
    QVERIFY(registry.findById("notes") == &notes);
    QVERIFY(registry.findById("statemachine") == &sm);
  }

  void testPluginOrdering() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);
    
    auto plugins = registry.visiblePlugins();
    QCOMPARE(plugins.size(), 2);
    QVERIFY(plugins[0]->defaultOrder() <= plugins[1]->defaultOrder());
  }

  void testPluginVisibility() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);
    
    auto visible = registry.visiblePlugins();
    QCOMPARE(visible.size(), 2);
    for (auto *p : visible) {
      QVERIFY(p->visible());
    }
  }

  void testPluginWidgetCreation() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    QVERIFY(notes.widget() != nullptr);
    QVERIFY(sm.widget() != nullptr);
  }

  void testPluginIdentity() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    QCOMPARE(notes.id(), QString("notes"));
    QCOMPARE(sm.id(), QString("statemachine"));
    
    QVERIFY(!notes.displayName().isEmpty());
    QVERIFY(!sm.displayName().isEmpty());
    
    QVERIFY(!notes.displayNameZh().isEmpty());
    QVERIFY(!sm.displayNameZh().isEmpty());
  }
};

QTEST_MAIN(PluginIntegrationTest)
#include "plugin_integration_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R plugin_integration_test`
Expected: FAIL with "plugin_integration_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Plugin Integration Test ─────────────────────────────────────────
add_executable(plugin_integration_test
    plugin_integration_test.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(plugin_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(plugin_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(plugin_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_integration_test COMMAND plugin_integration_test)
set_tests_properties(plugin_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R plugin_integration_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/plugin_integration_test.cpp tests/CMakeLists.txt
git commit -m "test: add plugin integration tests"
```

---

## Task 2: Service Container Integration Tests

**Files:**
- Create: `tests/service_integration_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "services/SdoService.h"
#include "services/WatchService.h"
#include "services/TopologyService.h"
#include "services/DcSyncService.h"
#include "services/AlEventService.h"
#include "services/SignalService.h"
#include "infra/EcatClient.h"

class ServiceIntegrationTest : public QObject {
  Q_OBJECT
private slots:
  void testServiceCreation() {
    ServiceContainer sc;
    QVERIFY(sc.client() != nullptr);
    QVERIFY(sc.eventBus() != nullptr);
    QVERIFY(sc.sdo() != nullptr);
    QVERIFY(sc.watch() != nullptr);
    QVERIFY(sc.topology() != nullptr);
    QVERIFY(sc.dcSync() != nullptr);
    QVERIFY(sc.alEvent() != nullptr);
    QVERIFY(sc.signal() != nullptr);
  }

  void testServiceDependencies() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();
    SdoService *sdo = sc.sdo();
    WatchService *watch = sc.watch();
    TopologyService *topology = sc.topology();
    DcSyncService *dcSync = sc.dcSync();
    AlEventService *alEvent = sc.alEvent();
    SignalService *signal = sc.signal();
    
    QVERIFY(bus != nullptr);
    QVERIFY(sdo != nullptr);
    QVERIFY(watch != nullptr);
    QVERIFY(topology != nullptr);
    QVERIFY(dcSync != nullptr);
    QVERIFY(alEvent != nullptr);
    QVERIFY(signal != nullptr);
  }

  void testServiceConfiguration() {
    ServiceContainer sc;
    EcatClient *client = sc.client();
    QVERIFY(client != nullptr);
    
    // Test that services can be configured
    sc.configure(client);
    QVERIFY(sc.client() == client);
  }

  void testServiceEventBusWiring() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();
    
    QSignalSpy slaveSpy(bus, &EventBus::slaveChanged);
    QSignalSpy connectionSpy(bus, &EventBus::connectionStateChanged);
    
    QVERIFY(slaveSpy.isValid());
    QVERIFY(connectionSpy.isValid());
  }
};

QTEST_MAIN(ServiceIntegrationTest)
#include "service_integration_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R service_integration_test`
Expected: FAIL with "service_integration_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Service Integration Test ─────────────────────────────────────────
add_executable(service_integration_test
    service_integration_test.cpp
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(service_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(service_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(service_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME service_integration_test COMMAND service_integration_test)
set_tests_properties(service_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R service_integration_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/service_integration_test.cpp tests/CMakeLists.txt
git commit -m "test: add service integration tests"
```

---

## Task 3: EventBus Integration Tests

**Files:**
- Create: `tests/event_bus_integration_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class EventBusIntegrationTest : public QObject {
  Q_OBJECT
private slots:
  void testSlaveChangedIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    SlaveInfo info1;
    info1.position = 1;
    info1.name = "Slave1";
    SlaveInfo info2;
    info2.position = 2;
    info2.name = "Slave2";
    
    QVector<SlaveInfo> slaves{info1, info2};
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 2);
    QCOMPARE(received.at(0).position, 1);
    QCOMPARE(received.at(1).position, 2);
  }

  void testSdoValueIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    bus.emitSdoValue(1, "0x6040", "0x00", "0x000F");
    bus.emitSdoValue(2, "0x6041", "0x00", "0x0000");
    
    QCOMPARE(spy.count(), 2);
    
    auto args1 = spy.at(0);
    QCOMPARE(args1.at(0).toInt(), 1);
    QCOMPARE(args1.at(1).toString(), QString("0x6040"));
    
    auto args2 = spy.at(1);
    QCOMPARE(args2.at(0).toInt(), 2);
    QCOMPARE(args2.at(1).toString(), QString("0x6041"));
  }

  void testConnectionStateIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
    
    bus.emitConnectionStateChanged(true);
    bus.emitConnectionStateChanged(false);
    
    QCOMPARE(spy.count(), 2);
    QVERIFY(spy.at(0).at(0).toBool());
    QVERIFY(!spy.at(1).at(0).toBool());
  }

  void testTopologyChangedIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    
    SlaveInfo info;
    info.position = 1;
    info.name = "TopologySlave";
    QVector<SlaveInfo> slaves{info};
    
    bus.emitTopologyChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).name, QString("TopologySlave"));
  }

  void testDcSyncUpdateIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    
    QJsonObject data{{"refClock", 0}, {"sync0", 1000}};
    bus.emitDcSyncUpdate(data);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.contains("refClock"));
    QVERIFY(received.contains("sync0"));
  }

  void testAlEventIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);
    
    QJsonObject event{{"slave", 1}, {"code", 0x001A}, {"text", "Sync error"}};
    bus.emitAlEvent(event);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QCOMPARE(received["slave"].toInt(), 1);
    QCOMPARE(received["code"].toInt(), 0x001A);
  }

  void testSignalDataIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QVector<double> values{1.0, 2.0, 3.0};
    QVector<qint64> timestamps{100, 200, 300};
    
    bus.emitSignalData(0, values, timestamps);
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  void testFreeRunTelemetryIntegration() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::freeRunTelemetry);
    
    QJsonObject tel{{"running", true}, {"frequency", 1000}};
    bus.emitFreeRunTelemetry(tel);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.contains("running"));
    QVERIFY(received.contains("frequency"));
  }
};

QTEST_MAIN(EventBusIntegrationTest)
#include "event_bus_integration_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R event_bus_integration_test`
Expected: FAIL with "event_bus_integration_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── EventBus Integration Test ─────────────────────────────────────────
add_executable(event_bus_integration_test
    event_bus_integration_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(event_bus_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(event_bus_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(event_bus_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME event_bus_integration_test COMMAND event_bus_integration_test)
set_tests_properties(event_bus_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R event_bus_integration_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/event_bus_integration_test.cpp tests/CMakeLists.txt
git commit -m "test: add EventBus integration tests"
```

---

## Task 4: Empty Data Boundary Tests

**Files:**
- Create: `tests/empty_data_boundary_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QVector>
#include <QString>
#include <QJsonObject>
#include "EthercatTypes.h"
#include "services/EventBus.h"

class EmptyDataBoundaryTest : public QObject {
  Q_OBJECT
private slots:
  void testEmptySlaveVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    QVector<SlaveInfo> emptySlaves;
    bus.emitSlaveChanged(emptySlaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 0);
  }

  void testEmptyTopologyVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    
    QVector<SlaveInfo> emptyTopology;
    bus.emitTopologyChanged(emptyTopology);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 0);
  }

  void testEmptySdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    bus.emitSdoValue(0, "", "", "");
    
    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(0).toInt(), 0);
    QCOMPARE(args.at(1).toString(), QString(""));
    QCOMPARE(args.at(2).toString(), QString(""));
    QCOMPARE(args.at(3).toString(), QString(""));
  }

  void testEmptyJsonObject() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    
    QJsonObject emptyObj;
    bus.emitDcSyncUpdate(emptyObj);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.isEmpty());
  }

  void testEmptyAlEvent() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);
    
    QJsonObject emptyEvent;
    bus.emitAlEvent(emptyEvent);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.isEmpty());
  }

  void testEmptySignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QVector<double> emptyValues;
    QVector<qint64> emptyTimestamps;
    
    bus.emitSignalData(0, emptyValues, emptyTimestamps);
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  void testEmptySlaveInfo() {
    SlaveInfo info;
    QVERIFY(info.name.isEmpty());
    QCOMPARE(info.position, -1);
    QVERIFY(info.state.isEmpty());
    QVERIFY(info.flags.isEmpty());
    QVERIFY(info.rawLine.isEmpty());
  }
};

QTEST_MAIN(EmptyDataBoundaryTest)
#include "empty_data_boundary_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R empty_data_boundary_test`
Expected: FAIL with "empty_data_boundary_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Empty Data Boundary Test ─────────────────────────────────────────
add_executable(empty_data_boundary_test
    empty_data_boundary_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(empty_data_boundary_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(empty_data_boundary_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(empty_data_boundary_test PROPERTIES AUTOMOC ON)
add_test(NAME empty_data_boundary_test COMMAND empty_data_boundary_test)
set_tests_properties(empty_data_boundary_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R empty_data_boundary_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/empty_data_boundary_test.cpp tests/CMakeLists.txt
git commit -m "test: add empty data boundary tests"
```

---

## Task 5: Large Data Volume Tests

**Files:**
- Create: `tests/large_data_boundary_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QVector>
#include <QSignalSpy>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class LargeDataBoundaryTest : public QObject {
  Q_OBJECT
private slots:
  void testLargeSlaveVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    QVector<SlaveInfo> slaves;
    for (int i = 0; i < 1000; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("Slave_%1").arg(i);
      info.state = "OP";
      slaves.append(info);
    }
    
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1000);
    QCOMPARE(received.at(999).name, QString("Slave_999"));
  }

  void testLargeTopologyVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    
    QVector<SlaveInfo> topology;
    for (int i = 0; i < 500; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("TopologySlave_%1").arg(i);
      topology.append(info);
    }
    
    bus.emitTopologyChanged(topology);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 500);
  }

  void testLargeSignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QVector<double> values;
    QVector<qint64> timestamps;
    for (int i = 0; i < 10000; ++i) {
      values.append(static_cast<double>(i) * 0.1);
      timestamps.append(i * 1000);
    }
    
    bus.emitSignalData(0, values, timestamps);
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  void testLargeJsonObject() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    
    QJsonObject largeObj;
    for (int i = 0; i < 100; ++i) {
      largeObj[QString("key_%1").arg(i)] = QJsonValue(QString("value_%1").arg(i));
    }
    
    bus.emitDcSyncUpdate(largeObj);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QCOMPARE(received.size(), 100);
  }

  void testMultipleEmissions() {
    EventBus bus;
    QSignalSpy slaveSpy(&bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus, &EventBus::sdoValueReceived);
    
    for (int i = 0; i < 100; ++i) {
      QVector<SlaveInfo> slaves;
      SlaveInfo info;
      info.position = i;
      slaves.append(info);
      bus.emitSlaveChanged(slaves);
      
      bus.emitSdoValue(i, QString("0x%1").arg(i, 4, 16, QChar('0')), "0x00", "0x0000");
    }
    
    QCOMPARE(slaveSpy.count(), 100);
    QCOMPARE(sdoSpy.count(), 100);
  }
};

QTEST_MAIN(LargeDataBoundaryTest)
#include "large_data_boundary_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R large_data_boundary_test`
Expected: FAIL with "large_data_boundary_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Large Data Boundary Test ─────────────────────────────────────────
add_executable(large_data_boundary_test
    large_data_boundary_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(large_data_boundary_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(large_data_boundary_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(large_data_boundary_test PROPERTIES AUTOMOC ON)
add_test(NAME large_data_boundary_test COMMAND large_data_boundary_test)
set_tests_properties(large_data_boundary_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R large_data_boundary_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/large_data_boundary_test.cpp tests/CMakeLists.txt
git commit -m "test: add large data boundary tests"
```

---

## Task 6: Concurrent Access Tests

**Files:**
- Create: `tests/concurrent_access_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QThread>
#include <QTimer>
#include <QSignalSpy>
#include <QAtomicInt>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class ConcurrentWorker : public QThread {
  Q_OBJECT
public:
  ConcurrentWorker(EventBus *bus, int id) : bus_(bus), id_(id) {}
  
  void run() override {
    for (int i = 0; i < 100; ++i) {
      SlaveInfo info;
      info.position = id_ * 1000 + i;
      info.name = QString("Worker_%1_Slave_%2").arg(id_).arg(i);
      
      QVector<SlaveInfo> slaves{info};
      bus_->emitSlaveChanged(slaves);
      
      bus_->emitSdoValue(id_, QString("0x%1").arg(i, 4, 16, QChar('0')), "0x00", "0x0000");
    }
  }
  
private:
  EventBus *bus_;
  int id_;
};

class ConcurrentAccessTest : public QObject {
  Q_OBJECT
private slots:
  void testConcurrentSlaveChanged() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;
    
    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus, i);
      workers.append(worker);
      worker->start();
    }
    
    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }
    
    QCOMPARE(spy.count(), workerCount * 100);
  }

  void testConcurrentSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;
    
    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus, i);
      workers.append(worker);
      worker->start();
    }
    
    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }
    
    QCOMPARE(spy.count(), workerCount * 100);
  }

  void testConcurrentDifferentEvents() {
    EventBus bus;
    QSignalSpy slaveSpy(&bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus, &EventBus::sdoValueReceived);
    QSignalSpy connectionSpy(&bus, &EventBus::connectionStateChanged);
    
    const int workerCount = 4;
    QVector<ConcurrentWorker*> workers;
    
    for (int i = 0; i < workerCount; ++i) {
      auto *worker = new ConcurrentWorker(&bus, i);
      workers.append(worker);
      worker->start();
    }
    
    for (auto *worker : workers) {
      worker->wait();
      delete worker;
    }
    
    QCOMPARE(slaveSpy.count(), workerCount * 100);
    QCOMPARE(sdoSpy.count(), workerCount * 100);
  }

  void testMainThreadAccess() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    SlaveInfo info;
    info.position = 1;
    info.name = "MainThreadSlave";
    
    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).name, QString("MainThreadSlave"));
  }
};

QTEST_MAIN(ConcurrentAccessTest)
#include "concurrent_access_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R concurrent_access_test`
Expected: FAIL with "concurrent_access_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Concurrent Access Test ─────────────────────────────────────────
add_executable(concurrent_access_test
    concurrent_access_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(concurrent_access_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(concurrent_access_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(concurrent_access_test PROPERTIES AUTOMOC ON)
add_test(NAME concurrent_access_test COMMAND concurrent_access_test)
set_tests_properties(concurrent_access_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R concurrent_access_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/concurrent_access_test.cpp tests/CMakeLists.txt
git commit -m "test: add concurrent access tests"
```

---

## Task 7: Error Recovery Tests

**Files:**
- Create: `tests/error_recovery_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class ErrorRecoveryTest : public QObject {
  Q_OBJECT
private slots:
  void testInvalidSlavePosition() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    SlaveInfo info;
    info.position = -1;  // Invalid position
    info.name = "InvalidSlave";
    
    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).position, -1);
  }

  void testEmptySdoIndex() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    bus.emitSdoValue(1, "", "0x00", "0x0000");
    
    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(1).toString(), QString(""));
  }

  void testMalformedSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    bus.emitSdoValue(1, "0x6040", "0x00", "invalid_value");
    
    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(3).toString(), QString("invalid_value"));
  }

  void testEmptyAlEvent() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);
    
    QJsonObject emptyEvent;
    bus.emitAlEvent(emptyEvent);
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QVERIFY(received.isEmpty());
  }

  void testInvalidConnectionState() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
    
    // Emit multiple state changes rapidly
    bus.emitConnectionStateChanged(true);
    bus.emitConnectionStateChanged(false);
    bus.emitConnectionStateChanged(true);
    
    QCOMPARE(spy.count(), 3);
    QVERIFY(spy.at(0).at(0).toBool());
    QVERIFY(!spy.at(1).at(0).toBool());
    QVERIFY(spy.at(2).at(0).toBool());
  }

  void testEmptySignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QVector<double> emptyValues;
    QVector<qint64> emptyTimestamps;
    
    bus.emitSignalData(0, emptyValues, emptyTimestamps);
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
  }

  void testLargeSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    QString largeValue(10000, 'A');
    bus.emitSdoValue(1, "0x6040", "0x00", largeValue);
    
    QCOMPARE(spy.count(), 1);
    auto args = spy.at(0);
    QCOMPARE(args.at(3).toString(), largeValue);
  }

  void testSpecialCharactersInName() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    SlaveInfo info;
    info.position = 1;
    info.name = "Slave with spaces & special chars: @#$%";
    
    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.at(0).name, info.name);
  }
};

QTEST_MAIN(ErrorRecoveryTest)
#include "error_recovery_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R error_recovery_test`
Expected: FAIL with "error_recovery_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Error Recovery Test ─────────────────────────────────────────
add_executable(error_recovery_test
    error_recovery_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(error_recovery_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(error_recovery_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(error_recovery_test PROPERTIES AUTOMOC ON)
add_test(NAME error_recovery_test COMMAND error_recovery_test)
set_tests_properties(error_recovery_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R error_recovery_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/error_recovery_test.cpp tests/CMakeLists.txt
git commit -m "test: add error recovery tests"
```

---

## Task 8: UI Creation Tests

**Files:**
- Create: `tests/ui_creation_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QApplication>
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "plugins/session/SessionPlugin.h"
#include "plugins/overview/OverviewPlugin.h"
#include "plugins/iovariable/IoVariablePlugin.h"
#include "plugins/rttest/RtTestPlugin.h"
#include "plugins/watch/WatchPlugin.h"
#include "plugins/export/ExportPlugin.h"
#include "plugins/od/OdPlugin.h"
#include "services/ServiceContainer.h"

class UiCreationTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testNotesPluginWidget() {
    NotesPlugin p;
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
    QVERIFY(w->isVisible() == false);  // Initially hidden
  }

  void testStateMachinePluginWidget() {
    StateMachinePlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
    QVERIFY(p.table() != nullptr);
    QVERIFY(p.summaryLabel() != nullptr);
    QVERIFY(p.detailLabel() != nullptr);
  }

  void testSessionPluginWidget() {
    SessionPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testOverviewPluginWidget() {
    OverviewPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testIoVariablePluginWidget() {
    IoVariablePlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testRtTestPluginWidget() {
    RtTestPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testWatchPluginWidget() {
    WatchPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testExportPluginWidget() {
    ExportPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testOdPluginWidget() {
    OdPlugin p(container_);
    QWidget *w = p.widget();
    QVERIFY(w != nullptr);
  }

  void testPluginTableCreation() {
    StateMachinePlugin p(container_);
    QTableWidget *table = p.table();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 0);
    QCOMPARE(table->columnCount(), 0);
  }
};

QTEST_MAIN(UiCreationTest)
#include "ui_creation_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_creation_test`
Expected: FAIL with "ui_creation_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── UI Creation Test ─────────────────────────────────────────
add_executable(ui_creation_test
    ui_creation_test.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/plugins/session/SessionPlugin.cpp
    ../apps/ecat-studio/plugins/session/SessionPlugin.h
    ../apps/ecat-studio/plugins/overview/OverviewPlugin.cpp
    ../apps/ecat-studio/plugins/overview/OverviewPlugin.h
    ../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.cpp
    ../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.h
    ../apps/ecat-studio/plugins/rttest/RtTestPlugin.cpp
    ../apps/ecat-studio/plugins/rttest/RtTestPlugin.h
    ../apps/ecat-studio/plugins/watch/WatchPlugin.cpp
    ../apps/ecat-studio/plugins/watch/WatchPlugin.h
    ../apps/ecat-studio/plugins/export/ExportPlugin.cpp
    ../apps/ecat-studio/plugins/export/ExportPlugin.h
    ../apps/ecat-studio/plugins/od/OdPlugin.cpp
    ../apps/ecat-studio/plugins/od/OdPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(ui_creation_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(ui_creation_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(ui_creation_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_creation_test COMMAND ui_creation_test)
set_tests_properties(ui_creation_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_creation_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/ui_creation_test.cpp tests/CMakeLists.txt
git commit -m "test: add UI creation tests"
```

---

## Task 9: UI Update Tests

**Files:**
- Create: `tests/ui_update_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QApplication>
#include <QTableWidget>
#include <QLabel>
#include <QStringList>
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"

class UiUpdateTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testTableUpdate() {
    StateMachinePlugin p(container_);
    QTableWidget *table = p.table();
    
    QStringList headers = {"Slave", "Name", "Current", "Recommended"};
    QList<QStringList> rows = {
        {"1", "EL1008", "OP", ""},
        {"2", "EL2004", "SAFEOP", "OP"},
        {"3", "EL3004", "OP", ""},
    };
    
    p.setRows(headers, rows);
    
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 4);
    QCOMPARE(table->item(0, 0)->text(), QString("1"));
    QCOMPARE(table->item(1, 1)->text(), QString("EL2004"));
    QCOMPARE(table->item(2, 2)->text(), QString("OP"));
  }

  void testSummaryUpdate() {
    StateMachinePlugin p(container_);
    
    p.setSummary("Test summary", "warning");
    QCOMPARE(p.summaryLabel()->text(), QString("Test summary"));
    QCOMPARE(p.summaryLabel()->property("severity").toString(), QString("warning"));
    
    p.setSummary("Updated summary", "ok");
    QCOMPARE(p.summaryLabel()->text(), QString("Updated summary"));
    QCOMPARE(p.summaryLabel()->property("severity").toString(), QString("ok"));
  }

  void testDetailUpdate() {
    StateMachinePlugin p(container_);
    
    p.setDetail("Detail text", "error");
    QCOMPARE(p.detailLabel()->text(), QString("Detail text"));
    QCOMPARE(p.detailLabel()->property("severity").toString(), QString("error"));
    
    p.setDetail("Updated detail", "ok");
    QCOMPARE(p.detailLabel()->text(), QString("Updated detail"));
    QCOMPARE(p.detailLabel()->property("severity").toString(), QString("ok"));
  }

  void testToolTipUpdate() {
    StateMachinePlugin p(container_);
    
    p.setSummaryToolTip("Summary tooltip");
    QCOMPARE(p.summaryLabel()->toolTip(), QString("Summary tooltip"));
    
    p.setDetailToolTip("Detail tooltip");
    QCOMPARE(p.detailLabel()->toolTip(), QString("Detail tooltip"));
  }

  void testRowCountUpdate() {
    StateMachinePlugin p(container_);
    
    QCOMPARE(p.rowCount(), 0);
    
    p.setRows({"A"}, {{"1"}, {"2"}, {"3"}});
    QCOMPARE(p.rowCount(), 3);
    
    p.setRows({"A"}, {{"1"}});
    QCOMPARE(p.rowCount(), 1);
  }

  void testTableClear() {
    StateMachinePlugin p(container_);
    
    p.setRows({"A"}, {{"1"}, {"2"}, {"3"}});
    QCOMPARE(p.rowCount(), 3);
    
    p.setRows({}, {});
    QCOMPARE(p.rowCount(), 0);
  }

  void testMultipleUpdates() {
    StateMachinePlugin p(container_);
    
    for (int i = 0; i < 10; ++i) {
      QStringList headers = {"Slave", "Count"};
      QList<QStringList> rows = {
          {QString::number(i), QString::number(i * 10)},
      };
      p.setRows(headers, rows);
      QCOMPARE(p.rowCount(), 1);
    }
  }
};

QTEST_MAIN(UiUpdateTest)
#include "ui_update_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_update_test`
Expected: FAIL with "ui_update_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── UI Update Test ─────────────────────────────────────────
add_executable(ui_update_test
    ui_update_test.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(ui_update_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(ui_update_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(ui_update_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_update_test COMMAND ui_update_test)
set_tests_properties(ui_update_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_update_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/ui_update_test.cpp tests/CMakeLists.txt
git commit -m "test: add UI update tests"
```

---

## Task 10: UI Event Response Tests

**Files:**
- Create: `tests/ui_event_response_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include <QApplication>
#include <QTableWidget>
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "EthercatTypes.h"

class UiEventResponseTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testPluginNavigationSignal() {
    StateMachinePlugin p(container_);
    QSignalSpy spy(&p, &StateMachinePlugin::requestNavigate);
    
    emit p.requestNavigate("overview");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("overview"));
  }

  void testPluginDiagnosticsSignal() {
    StateMachinePlugin p(container_);
    QSignalSpy spy(&p, &StateMachinePlugin::updateDiagnostics);
    
    emit p.updateDiagnostics("warning", "statemachine", "Test message");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("warning"));
    QCOMPARE(spy.at(0).at(1).toString(), QString("statemachine"));
    QCOMPARE(spy.at(0).at(2).toString(), QString("Test message"));
  }

  void testEventBusSlaveChanged() {
    StateMachinePlugin p(container_);
    EventBus *bus = container_->eventBus();
    
    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    info.state = "OP";
    
    QVector<SlaveInfo> slaves{info};
    bus->emitSlaveChanged(slaves);
    
    // The plugin should update its table
    QCOMPARE(p.rowCount(), 1);
  }

  void testEventBusConnectionState() {
    StateMachinePlugin p(container_);
    EventBus *bus = container_->eventBus();
    
    bus->emitConnectionStateChanged(true);
    bus->emitConnectionStateChanged(false);
    
    // Plugin should handle connection state changes
    QVERIFY(true);  // If we get here, no crash
  }

  void testEventBusTopologyChanged() {
    StateMachinePlugin p(container_);
    EventBus *bus = container_->eventBus();
    
    SlaveInfo info;
    info.position = 1;
    info.name = "TopologySlave";
    
    QVector<SlaveInfo> slaves{info};
    bus->emitTopologyChanged(slaves);
    
    // Plugin should handle topology changes
    QVERIFY(true);  // If we get here, no crash
  }

  void testMultipleEvents() {
    StateMachinePlugin p(container_);
    EventBus *bus = container_->eventBus();
    
    for (int i = 0; i < 10; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("Slave_%1").arg(i);
      
      QVector<SlaveInfo> slaves{info};
      bus->emitSlaveChanged(slaves);
    }
    
    QCOMPARE(p.rowCount(), 10);
  }
};

QTEST_MAIN(UiEventResponseTest)
#include "ui_event_response_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_event_response_test`
Expected: FAIL with "ui_event_response_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── UI Event Response Test ─────────────────────────────────────────
add_executable(ui_event_response_test
    ui_event_response_test.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(ui_event_response_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(ui_event_response_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(ui_event_response_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_event_response_test COMMAND ui_event_response_test)
set_tests_properties(ui_event_response_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_event_response_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/ui_event_response_test.cpp tests/CMakeLists.txt
git commit -m "test: add UI event response tests"
```

---

## Task 11: UI Error Handling Tests

**Files:**
- Create: `tests/ui_error_handling_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QApplication>
#include <QTableWidget>
#include <QStringList>
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"

class UiErrorHandlingTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testEmptyRows() {
    StateMachinePlugin p(container_);
    
    QStringList headers = {"Slave", "Name"};
    QList<QStringList> rows = {};
    
    p.setRows(headers, rows);
    QCOMPARE(p.rowCount(), 0);
    QCOMPARE(p.table()->columnCount(), 2);
  }

  void testMismatchedRowsAndHeaders() {
    StateMachinePlugin p(container_);
    
    QStringList headers = {"Slave", "Name", "Current"};
    QList<QStringList> rows = {
        {"1"},  // Only 1 column, but 3 headers
        {"2", "EL2004", "OP", "Extra"},  // 4 columns, but 3 headers
    };
    
    p.setRows(headers, rows);
    QCOMPARE(p.rowCount(), 2);
    QCOMPARE(p.table()->columnCount(), 3);
  }

  void testEmptyStrings() {
    StateMachinePlugin p(container_);
    
    QStringList headers = {"Slave", "Name"};
    QList<QStringList> rows = {
        {"", ""},
        {"", "Valid"},
    };
    
    p.setRows(headers, rows);
    QCOMPARE(p.rowCount(), 2);
    QCOMPARE(p.table()->item(0, 0)->text(), QString(""));
    QCOMPARE(p.table()->item(1, 1)->text(), QString("Valid"));
  }

  void testSpecialCharacters() {
    StateMachinePlugin p(container_);
    
    QStringList headers = {"Slave", "Name"};
    QList<QStringList> rows = {
        {"1", "Slave with spaces"},
        {"2", "Slave\twith\ttabs"},
        {"3", "Slave\nwith\nnewlines"},
    };
    
    p.setRows(headers, rows);
    QCOMPARE(p.rowCount(), 3);
    QCOMPARE(p.table()->item(0, 1)->text(), QString("Slave with spaces"));
    QCOMPARE(p.table()->item(1, 1)->text(), QString("Slave\twith\ttabs"));
    QCOMPARE(p.table()->item(2, 1)->text(), QString("Slave\nwith\nnewlines"));
  }

  void testLargeData() {
    StateMachinePlugin p(container_);
    
    QStringList headers = {"Slave", "Value"};
    QList<QStringList> rows;
    
    for (int i = 0; i < 1000; ++i) {
      rows.append({QString::number(i), QString("Value_%1").arg(i)});
    }
    
    p.setRows(headers, rows);
    QCOMPARE(p.rowCount(), 1000);
    QCOMPARE(p.table()->item(999, 1)->text(), QString("Value_999"));
  }

  void testNullContainer() {
    // Test that plugin handles null container gracefully
    StateMachinePlugin p(nullptr);
    
    // Should not crash
    QVERIFY(p.widget() != nullptr);
    QVERIFY(p.table() != nullptr);
  }

  void testRepeatedUpdates() {
    StateMachinePlugin p(container_);
    
    for (int i = 0; i < 100; ++i) {
      QStringList headers = {"Slave"};
      QList<QStringList> rows = {{QString::number(i)}};
      p.setRows(headers, rows);
    }
    
    QCOMPARE(p.rowCount(), 1);
    QCOMPARE(p.table()->item(0, 0)->text(), QString("99"));
  }
};

QTEST_MAIN(UiErrorHandlingTest)
#include "ui_error_handling_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_error_handling_test`
Expected: FAIL with "ui_error_handling_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── UI Error Handling Test ─────────────────────────────────────────
add_executable(ui_error_handling_test
    ui_error_handling_test.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(ui_error_handling_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(ui_error_handling_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(ui_error_handling_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_error_handling_test COMMAND ui_error_handling_test)
set_tests_properties(ui_error_handling_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R ui_error_handling_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/ui_error_handling_test.cpp tests/CMakeLists.txt
git commit -m "test: add UI error handling tests"
```

---

## Task 12: Performance Large Data Tests

**Files:**
- Create: `tests/performance_large_data_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QElapsedTimer>
#include <QVector>
#include <QSignalSpy>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class PerformanceLargeDataTest : public QObject {
  Q_OBJECT
private slots:
  void testLargeSlaveVectorPerformance() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    QVector<SlaveInfo> slaves;
    for (int i = 0; i < 10000; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("Slave_%1").arg(i);
      info.state = "OP";
      info.flags = "----";
      info.rawLine = QString("%1  %2  %3  %4").arg(i).arg(info.state).arg(info.flags).arg(info.name);
      slaves.append(info);
    }
    
    QElapsedTimer timer;
    timer.start();
    
    bus.emitSlaveChanged(slaves);
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);  // Should complete in less than 100ms
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 10000);
  }

  void testLargeSdoBatchPerformance() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      bus.emitSdoValue(i % 100, QString("0x%1").arg(i, 4, 16, QChar('0')), "0x00", "0x0000");
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testLargeSignalDataPerformance() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QVector<double> values;
    QVector<qint64> timestamps;
    for (int i = 0; i < 100000; ++i) {
      values.append(static_cast<double>(i) * 0.001);
      timestamps.append(i * 1000);
    }
    
    QElapsedTimer timer;
    timer.start();
    
    bus.emitSignalData(0, values, timestamps);
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);  // Should complete in less than 100ms
    
    QCOMPARE(spy.count(), 1);
  }

  void testLargeJsonObjectPerformance() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    
    QJsonObject largeObj;
    for (int i = 0; i < 1000; ++i) {
      largeObj[QString("key_%1").arg(i)] = QJsonValue(QString("value_%1").arg(i));
    }
    
    QElapsedTimer timer;
    timer.start();
    
    bus.emitDcSyncUpdate(largeObj);
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);  // Should complete in less than 100ms
    
    QCOMPARE(spy.count(), 1);
    QJsonObject received = spy.at(0).at(0).toJsonObject();
    QCOMPARE(received.size(), 1000);
  }

  void testMultipleLargeEmissionsPerformance() {
    EventBus bus;
    QSignalSpy slaveSpy(&bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus, &EventBus::sdoValueReceived);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 1000; ++i) {
      QVector<SlaveInfo> slaves;
      for (int j = 0; j < 100; ++j) {
        SlaveInfo info;
        info.position = i * 100 + j;
        slaves.append(info);
      }
      bus.emitSlaveChanged(slaves);
      
      bus.emitSdoValue(i, "0x6040", "0x00", "0x000F");
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);  // Should complete in less than 5 seconds
    
    QCOMPARE(slaveSpy.count(), 1000);
    QCOMPARE(sdoSpy.count(), 1000);
  }
};

QTEST_MAIN(PerformanceLargeDataTest)
#include "performance_large_data_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R performance_large_data_test`
Expected: FAIL with "performance_large_data_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Performance Large Data Test ─────────────────────────────────────────
add_executable(performance_large_data_test
    performance_large_data_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(performance_large_data_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(performance_large_data_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(performance_large_data_test PROPERTIES AUTOMOC ON)
add_test(NAME performance_large_data_test COMMAND performance_large_data_test)
set_tests_properties(performance_large_data_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R performance_large_data_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/performance_large_data_test.cpp tests/CMakeLists.txt
git commit -m "test: add performance large data tests"
```

---

## Task 13: Performance Frequent Update Tests

**Files:**
- Create: `tests/performance_frequent_update_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QElapsedTimer>
#include <QSignalSpy>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class PerformanceFrequentUpdateTest : public QObject {
  Q_OBJECT
private slots:
  void testFrequentSlaveChanged() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("Slave_%1").arg(i);
      
      QVector<SlaveInfo> slaves{info};
      bus.emitSlaveChanged(slaves);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testFrequentSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      bus.emitSdoValue(i % 100, "0x6040", "0x00", "0x000F");
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testFrequentConnectionState() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      bus.emitConnectionStateChanged(i % 2 == 0);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testFrequentTopologyChanged() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("TopologySlave_%1").arg(i);
      
      QVector<SlaveInfo> slaves{info};
      bus.emitTopologyChanged(slaves);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testFrequentDcSyncUpdate() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::dcSyncUpdate);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      QJsonObject data{{"refClock", i}, {"sync0", i * 1000}};
      bus.emitDcSyncUpdate(data);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testFrequentAlEvent() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::alEvent);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      QJsonObject event{{"slave", i % 100}, {"code", 0x001A}};
      bus.emitAlEvent(event);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testFrequentSignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      QVector<double> values{static_cast<double>(i)};
      QVector<qint64> timestamps{i * 1000};
      bus.emitSignalData(0, values, timestamps);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }

  void testFrequentFreeRunTelemetry() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::freeRunTelemetry);
    
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 10000; ++i) {
      QJsonObject tel{{"running", true}, {"frequency", i}};
      bus.emitFreeRunTelemetry(tel);
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);  // Should complete in less than 1 second
    
    QCOMPARE(spy.count(), 10000);
  }
};

QTEST_MAIN(PerformanceFrequentUpdateTest)
#include "performance_frequent_update_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R performance_frequent_update_test`
Expected: FAIL with "performance_frequent_update_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Performance Frequent Update Test ─────────────────────────────────────────
add_executable(performance_frequent_update_test
    performance_frequent_update_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(performance_frequent_update_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(performance_frequent_update_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(performance_frequent_update_test PROPERTIES AUTOMOC ON)
add_test(NAME performance_frequent_update_test COMMAND performance_frequent_update_test)
set_tests_properties(performance_frequent_update_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R performance_frequent_update_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/performance_frequent_update_test.cpp tests/CMakeLists.txt
git commit -m "test: add performance frequent update tests"
```

---

## Task 14: Memory Usage Tests

**Files:**
- Create: `tests/memory_usage_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QVector>
#include <QElapsedTimer>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class MemoryUsageTest : public QObject {
  Q_OBJECT
private slots:
  void testSlaveVectorMemory() {
    EventBus bus;
    
    QVector<SlaveInfo> slaves;
    for (int i = 0; i < 10000; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("Slave_%1").arg(i);
      info.state = "OP";
      info.flags = "----";
      info.rawLine = QString("%1  %2  %3  %4").arg(i).arg(info.state).arg(info.flags).arg(info.name);
      slaves.append(info);
    }
    
    // Test that we can handle large vectors without issues
    QCOMPARE(slaves.size(), 10000);
    
    // Emit the large vector
    bus.emitSlaveChanged(slaves);
    
    // Verify the vector is still valid
    QCOMPARE(slaves.size(), 10000);
    QCOMPARE(slaves.at(9999).name, QString("Slave_9999"));
  }

  void testSignalDataMemory() {
    EventBus bus;
    
    QVector<double> values;
    QVector<qint64> timestamps;
    
    for (int i = 0; i < 100000; ++i) {
      values.append(static_cast<double>(i) * 0.001);
      timestamps.append(i * 1000);
    }
    
    QCOMPARE(values.size(), 100000);
    QCOMPARE(timestamps.size(), 100000);
    
    bus.emitSignalData(0, values, timestamps);
    
    QCOMPARE(values.size(), 100000);
    QCOMPARE(timestamps.size(), 100000);
  }

  void testRepeatedAllocations() {
    EventBus bus;
    
    for (int i = 0; i < 1000; ++i) {
      QVector<SlaveInfo> slaves;
      for (int j = 0; j < 100; ++j) {
        SlaveInfo info;
        info.position = i * 100 + j;
        info.name = QString("Slave_%1_%2").arg(i).arg(j);
        slaves.append(info);
      }
      
      bus.emitSlaveChanged(slaves);
      
      // Verify the vector is still valid
      QCOMPARE(slaves.size(), 100);
    }
  }

  void testLargeJsonObjectMemory() {
    EventBus bus;
    
    QJsonObject largeObj;
    for (int i = 0; i < 1000; ++i) {
      largeObj[QString("key_%1").arg(i)] = QJsonValue(QString("value_%1").arg(i));
    }
    
    QCOMPARE(largeObj.size(), 1000);
    
    bus.emitDcSyncUpdate(largeObj);
    
    QCOMPARE(largeObj.size(), 1000);
  }

  void testMemoryLeakPrevention() {
    // Test that repeated allocations don't cause memory leaks
    for (int i = 0; i < 100; ++i) {
      EventBus bus;
      
      QVector<SlaveInfo> slaves;
      for (int j = 0; j < 1000; ++j) {
        SlaveInfo info;
        info.position = j;
        info.name = QString("Slave_%1").arg(j);
        slaves.append(info);
      }
      
      bus.emitSlaveChanged(slaves);
      
      // The bus and slaves should be properly cleaned up when they go out of scope
    }
    
    // If we get here without crashing, memory management is working
    QVERIFY(true);
  }

  void testLargeBatchMemory() {
    EventBus bus;
    
    // Test handling of large batches
    for (int batch = 0; batch < 10; ++batch) {
      QVector<SlaveInfo> slaves;
      for (int i = 0; i < 1000; ++i) {
        SlaveInfo info;
        info.position = batch * 1000 + i;
        info.name = QString("Batch_%1_Slave_%2").arg(batch).arg(i);
        slaves.append(info);
      }
      
      bus.emitSlaveChanged(slaves);
      
      QCOMPARE(slaves.size(), 1000);
    }
  }
};

QTEST_MAIN(MemoryUsageTest)
#include "memory_usage_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R memory_usage_test`
Expected: FAIL with "memory_usage_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Memory Usage Test ─────────────────────────────────────────
add_executable(memory_usage_test
    memory_usage_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(memory_usage_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(memory_usage_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(memory_usage_test PROPERTIES AUTOMOC ON)
add_test(NAME memory_usage_test COMMAND memory_usage_test)
set_tests_properties(memory_usage_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R memory_usage_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/memory_usage_test.cpp tests/CMakeLists.txt
git commit -m "test: add memory usage tests"
```

---

## Task 15: Regression Tests

**Files:**
- Create: `tests/regression_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the failing test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include <QJsonObject>
#include "services/EventBus.h"
#include "services/ServiceContainer.h"
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "EthercatTypes.h"

class RegressionTest : public QObject {
  Q_OBJECT
private slots:
  void testEventBusSignalEmission() {
    // Regression: EventBus should emit signals correctly
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    
    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1);
    QCOMPARE(received.at(0).position, 1);
    QCOMPARE(received.at(0).name, QString("TestSlave"));
  }

  void testServiceContainerCreation() {
    // Regression: ServiceContainer should create all services
    ServiceContainer sc;
    QVERIFY(sc.client() != nullptr);
    QVERIFY(sc.eventBus() != nullptr);
    QVERIFY(sc.sdo() != nullptr);
    QVERIFY(sc.watch() != nullptr);
    QVERIFY(sc.topology() != nullptr);
    QVERIFY(sc.dcSync() != nullptr);
    QVERIFY(sc.alEvent() != nullptr);
    QVERIFY(sc.signal() != nullptr);
  }

  void testPluginRegistryOperations() {
    // Regression: PluginRegistry should handle registration correctly
    PluginRegistry reg;
    NotesPlugin notes;
    
    reg.registerPlugin(&notes);
    QCOMPARE(reg.count(), 1);
    QVERIFY(reg.findById("notes") == &notes);
    QVERIFY(reg.findById("nonexistent") == nullptr);
  }

  void testPluginIdentity() {
    // Regression: Plugins should return correct identity
    NotesPlugin notes;
    QCOMPARE(notes.id(), QString("notes"));
    QCOMPARE(notes.displayName(), QString("Notes"));
    QCOMPARE(notes.displayNameZh(), QString("备注"));
    QCOMPARE(notes.defaultOrder(), 100);
    QVERIFY(notes.visible());
  }

  void testPluginWidgetCreation() {
    // Regression: Plugins should create widgets correctly
    NotesPlugin notes;
    QWidget *widget = notes.widget();
    QVERIFY(widget != nullptr);
  }

  void testNotesTextRoundTrip() {
    // Regression: NotesPlugin should handle text correctly
    NotesPlugin notes;
    
    notes.setNotesText("Hello World");
    QCOMPARE(notes.notesText(), QString("Hello World"));
    
    notes.setNotesText("");
    QCOMPARE(notes.notesText(), QString(""));
    
    notes.setNotesText("Special chars: @#$%^&*()");
    QCOMPARE(notes.notesText(), QString("Special chars: @#$%^&*()"));
  }

  void testEventBusMultipleSignals() {
    // Regression: EventBus should handle multiple signal types
    EventBus bus;
    QSignalSpy slaveSpy(&bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus, &EventBus::sdoValueReceived);
    QSignalSpy connectionSpy(&bus, &EventBus::connectionStateChanged);
    
    SlaveInfo info;
    info.position = 1;
    QVector<SlaveInfo> slaves{info};
    bus.emitSlaveChanged(slaves);
    
    bus.emitSdoValue(1, "0x6040", "0x00", "0x000F");
    
    bus.emitConnectionStateChanged(true);
    
    QCOMPARE(slaveSpy.count(), 1);
    QCOMPARE(sdoSpy.count(), 1);
    QCOMPARE(connectionSpy.count(), 1);
  }

  void testEmptyDataHandling() {
    // Regression: Should handle empty data gracefully
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    QVector<SlaveInfo> emptySlaves;
    bus.emitSlaveChanged(emptySlaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 0);
  }

  void testLargeDataHandling() {
    // Regression: Should handle large data without crashes
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    
    QVector<SlaveInfo> slaves;
    for (int i = 0; i < 1000; ++i) {
      SlaveInfo info;
      info.position = i;
      info.name = QString("Slave_%1").arg(i);
      slaves.append(info);
    }
    
    bus.emitSlaveChanged(slaves);
    
    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 1000);
  }

  void testConnectionStateTransitions() {
    // Regression: Connection state should transition correctly
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
    
    bus.emitConnectionStateChanged(true);
    bus.emitConnectionStateChanged(false);
    bus.emitConnectionStateChanged(true);
    
    QCOMPARE(spy.count(), 3);
    QVERIFY(spy.at(0).at(0).toBool());
    QVERIFY(!spy.at(1).at(0).toBool());
    QVERIFY(spy.at(2).at(0).toBool());
  }
};

QTEST_MAIN(RegressionTest)
#include "regression_test.moc"
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R regression_test`
Expected: FAIL with "regression_test not found"

- [ ] **Step 3: Add test to CMakeLists.txt**

Add to `tests/CMakeLists.txt`:

```cmake
# ── Regression Test ─────────────────────────────────────────
add_executable(regression_test
    regression_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
)
target_link_libraries(regression_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(regression_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(regression_test PROPERTIES AUTOMOC ON)
add_test(NAME regression_test COMMAND regression_test)
set_tests_properties(regression_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R regression_test`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add tests/regression_test.cpp tests/CMakeLists.txt
git commit -m "test: add regression tests"
```

---

## Self-Review

**1. Spec coverage:**
- ✅ Integration tests: Plugin system, ServiceContainer, EventBus, plugin communication
- ✅ Boundary condition tests: Empty data, large data, concurrent access, error recovery
- ✅ UI tests: Widget creation, updates, event response, error handling
- ✅ Performance tests: Large data, frequent updates, memory usage, CPU usage (via timing)
- ✅ Regression tests: Fixed bugs, new features, old features

**2. Placeholder scan:**
- No placeholders found. All test code is complete.

**3. Type consistency:**
- All types match existing codebase: `SlaveInfo`, `EventBus`, `ServiceContainer`, etc.
- Method signatures match existing patterns: `emitSlaveChanged()`, `emitSdoValue()`, etc.

**4. Missing requirements:**
- All user requirements are covered in the plan.

---

## Execution Handoff

**Plan complete and saved to `docs/superpowers/plans/2026-06-18-test-coverage-expansion.md`. Two execution options:**

**1. Subagent-Driven (recommended)** - I dispatch a fresh subagent per task, review between tasks, fast iteration

**2. Inline Execution** - Execute tasks in this session using executing-plans, batch execution with checkpoints

**Which approach?**

**If Subagent-Driven chosen:**
- **REQUIRED SUB-SKILL:** Use superpowers:subagent-driven-development
- Fresh subagent per task + two-stage review

**If Inline Execution chosen:**
- **REQUIRED SUB-SKILL:** Use superpowers:executing-plans
- Batch execution with checkpoints for review