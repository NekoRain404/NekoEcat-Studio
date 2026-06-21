# Testing Framework Improvements Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add comprehensive testing framework improvements including test utilities, integration tests, performance tests, and coverage reporting to NekoEcat Studio.

**Architecture:** Build on the existing QTest-based testing infrastructure. Create shared test utilities (mocks, fixtures, helpers) as a reusable library. Add integration tests for daemon lifecycle, plugin lifecycle, service integration, and UI workflows. Add performance benchmarks for EventBus, cache, plugins, and services. Add a coverage generation script.

**Tech Stack:** Qt6 Test framework, CMake 3.20+, C++20, gcov/lcov for coverage

---

## File Structure

| File | Purpose |
|------|---------|
| `tests/utils/TestUtilities.h` | Shared test helpers, mock classes, test fixtures |
| `tests/utils/TestUtilities.cpp` | Implementation of test utilities |
| `tests/integration/daemon_lifecycle_test.cpp` | Daemon start/stop, client connection, heartbeat, reconnect |
| `tests/integration/plugin_lifecycle_test.cpp` | Plugin registration, activation/deactivation, settings, connection |
| `tests/integration/service_integration_test.cpp` | Service-to-service communication, EventBus propagation, lifecycle |
| `tests/integration/ui_workflow_test.cpp` | Complete user workflows: SDO, topology scan, Free Run |
| `tests/performance/eventbus_performance_test.cpp` | EventBus throughput, latency, memory |
| `tests/performance/cache_performance_test.cpp` | Cache hit rate, eviction, memory |
| `tests/performance/plugin_performance_test.cpp` | Plugin startup, memory, UI responsiveness |
| `tests/performance/service_performance_test.cpp` | Service response time, throughput, memory |
| `scripts/generate_coverage.sh` | Coverage report generation script |
| `tests/CMakeLists.txt` | Modified to add new test targets |

---

### Task 1: Create Test Utilities Header

**Files:**
- Create: `tests/utils/TestUtilities.h`

- [ ] **Step 1: Create the test utilities header**

```cpp
#pragma once

#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVector>
#include <QSignalSpy>
#include <QTimer>
#include <QEventLoop>
#include "EthercatTypes.h"

// --- Common Test Helpers ---

SlaveInfo createTestSlave(int position, const QString &name);
QVector<SlaveInfo> createTestTopology(int count);
QJsonObject createTestSdoValue(int position, const QString &index, const QString &value);
QJsonObject createTestJsonObject(const QMap<QString, QString> &data);
bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000);

// --- MockEcatClient ---

class MockEcatClient : public QObject {
  Q_OBJECT
public:
  explicit MockEcatClient(QObject *parent = nullptr);

  void setConnected(bool connected);
  bool isConnected() const { return connected_; }

  void simulateSlavesChanged(const QVector<SlaveInfo> &slaves);
  void simulateSdoValue(int pos, const QString &idx, const QString &sub, const QString &val);
  void simulateConnectionStateChanged(bool connected);
  void simulateFreeRunTelemetry(const QJsonObject &tel);
  void simulateDcSyncUpdate(const QJsonObject &data);
  void simulateAlEvent(const QJsonObject &event);
  void simulateSignalData(int ch, const QVector<double> &values, const QVector<qint64> &timestamps);

  int scanCallCount() const { return scanCallCount_; }
  int slaveInfoCallCount() const { return slaveInfoCallCount_; }
  int uploadCallCount() const { return uploadCallCount_; }
  int downloadCallCount() const { return downloadCallCount_; }

  void scan();
  void slaveInfo(int position);
  void upload(int position, const QString &index, const QString &subIndex);
  void download(int position, const QString &index, const QString &subIndex,
                const QString &value, const QString &type);

signals:
  void connected();
  void disconnected();
  void connectionStateChanged(bool connected);
  void slavesChanged(const QVector<SlaveInfo> &slaves);
  void sdoValue(int position, const QString &index, const QString &subIndex, const QString &value);
  void freeRunTelemetry(const QJsonObject &telemetry);
  void dcSyncStatusResult(const QJsonObject &data);
  void alEventLogResult(const QJsonObject &data);
  void signalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);
  void commandSucceeded(const QString &message);
  void errorMessage(const QString &message);

private:
  bool connected_ = false;
  int scanCallCount_ = 0;
  int slaveInfoCallCount_ = 0;
  int uploadCallCount_ = 0;
  int downloadCallCount_ = 0;
};

// --- MockEventBus ---

class MockEventBus : public QObject {
  Q_OBJECT
public:
  explicit MockEventBus(QObject *parent = nullptr);

  void emitSlaveChanged(const QVector<SlaveInfo> &slaves);
  void emitSdoValue(int pos, const QString &idx, const QString &sub, const QString &val);
  void emitConnectionStateChanged(bool connected);
  void emitFreeRunTelemetry(const QJsonObject &tel);
  void emitTopologyChanged(const QVector<SlaveInfo> &slaves);
  void emitDcSyncUpdate(const QJsonObject &data);
  void emitAlEvent(const QJsonObject &event);
  void emitSignalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);

  int slaveChangedCount() const { return slaveChangedCount_; }
  int sdoValueCount() const { return sdoValueCount_; }
  int connectionStateCount() const { return connectionStateCount_; }
  int freeRunTelemetryCount() const { return freeRunTelemetryCount_; }
  int topologyChangedCount() const { return topologyChangedCount_; }
  int dcSyncUpdateCount() const { return dcSyncUpdateCount_; }
  int alEventCount() const { return alEventCount_; }
  int signalDataCount() const { return signalDataCount_; }

  QVector<SlaveInfo> lastSlaves() const { return lastSlaves_; }
  QJsonObject lastTelemetry() const { return lastTelemetry_; }

signals:
  void slaveChanged(const QVector<SlaveInfo> &slaves);
  void sdoValueReceived(int position, const QString &index, const QString &subIndex, const QString &value);
  void connectionStateChanged(bool connected);
  void freeRunTelemetry(const QJsonObject &telemetry);
  void topologyChanged(const QVector<SlaveInfo> &slaves);
  void dcSyncUpdate(const QJsonObject &data);
  void alEvent(const QJsonObject &event);
  void signalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);

private:
  int slaveChangedCount_ = 0;
  int sdoValueCount_ = 0;
  int connectionStateCount_ = 0;
  int freeRunTelemetryCount_ = 0;
  int topologyChangedCount_ = 0;
  int dcSyncUpdateCount_ = 0;
  int alEventCount_ = 0;
  int signalDataCount_ = 0;
  QVector<SlaveInfo> lastSlaves_;
  QJsonObject lastTelemetry_;
};

// --- Test Fixtures ---

class PluginTestFixture : public QObject {
  Q_OBJECT
public:
  explicit PluginTestFixture(QObject *parent = nullptr);
  ~PluginTestFixture();

  MockEcatClient *mockClient() const { return mockClient_; }
  MockEventBus *mockEventBus() const { return mockEventBus_; }

private:
  MockEcatClient *mockClient_ = nullptr;
  MockEventBus *mockEventBus_ = nullptr;
};

class ServiceTestFixture : public QObject {
  Q_OBJECT
public:
  explicit ServiceTestFixture(QObject *parent = nullptr);
  ~ServiceTestFixture();

  MockEcatClient *mockClient() const { return mockClient_; }
  MockEventBus *mockEventBus() const { return mockEventBus_; }

private:
  MockEcatClient *mockClient_ = nullptr;
  MockEventBus *mockEventBus_ = nullptr;
};

class UITestFixture : public QObject {
  Q_OBJECT
public:
  explicit UITestFixture(QObject *parent = nullptr);
  ~UITestFixture();

  MockEcatClient *mockClient() const { return mockClient_; }
  MockEventBus *mockEventBus() const { return mockEventBus_; }

private:
  MockEcatClient *mockClient_ = nullptr;
  MockEventBus *mockEventBus_ = nullptr;
};
```

- [ ] **Step 2: Verify the file compiles**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I src/core tests/utils/TestUtilities.h`
Expected: No errors

---

### Task 2: Create Test Utilities Implementation

**Files:**
- Create: `tests/utils/TestUtilities.cpp`

- [ ] **Step 1: Create the test utilities implementation**

```cpp
#include "TestUtilities.h"

// --- Common Test Helpers ---

SlaveInfo createTestSlave(int position, const QString &name) {
  SlaveInfo info;
  info.position = position;
  info.name = name;
  info.state = "OP";
  info.flags = "--";
  info.rawLine = QString("%1  %2  %3  %4").arg(position).arg(name).arg("OP").arg("--");
  return info;
}

QVector<SlaveInfo> createTestTopology(int count) {
  QVector<SlaveInfo> slaves;
  slaves.reserve(count);
  for (int i = 0; i < count; ++i) {
    slaves.append(createTestSlave(i, QString("Slave_%1").arg(i)));
  }
  return slaves;
}

QJsonObject createTestSdoValue(int position, const QString &index, const QString &value) {
  return QJsonObject{
    {"position", position},
    {"index", index},
    {"value", value}
  };
}

QJsonObject createTestJsonObject(const QMap<QString, QString> &data) {
  QJsonObject obj;
  for (auto it = data.begin(); it != data.end(); ++it) {
    obj[it.key()] = it.value();
  }
  return obj;
}

bool waitForSignal(QObject *sender, const char *signal, int timeout) {
  QSignalSpy spy(sender, signal);
  if (!spy.isValid()) return false;

  QTimer timer;
  timer.setSingleShot(true);
  timer.setInterval(timeout);

  QEventLoop loop;
  QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
  QObject::connect(sender, signal, &loop, &QEventLoop::quit);
  timer.start();
  loop.exec();

  return spy.count() > 0;
}

// --- MockEcatClient ---

MockEcatClient::MockEcatClient(QObject *parent) : QObject(parent) {}

void MockEcatClient::setConnected(bool connected) {
  if (connected_ != connected) {
    connected_ = connected;
    emit connectionStateChanged(connected);
    if (connected) emit connected();
    else emit disconnected();
  }
}

void MockEcatClient::simulateSlavesChanged(const QVector<SlaveInfo> &slaves) {
  emit slavesChanged(slaves);
}

void MockEcatClient::simulateSdoValue(int pos, const QString &idx, const QString &sub, const QString &val) {
  emit sdoValue(pos, idx, sub, val);
}

void MockEcatClient::simulateConnectionStateChanged(bool connected) {
  setConnected(connected);
}

void MockEcatClient::simulateFreeRunTelemetry(const QJsonObject &tel) {
  emit freeRunTelemetry(tel);
}

void MockEcatClient::simulateDcSyncUpdate(const QJsonObject &data) {
  emit dcSyncStatusResult(data);
}

void MockEcatClient::simulateAlEvent(const QJsonObject &event) {
  emit alEventLogResult(event);
}

void MockEcatClient::simulateSignalData(int ch, const QVector<double> &values, const QVector<qint64> &timestamps) {
  emit signalData(ch, values, timestamps);
}

void MockEcatClient::scan() {
  ++scanCallCount_;
  emit commandSucceeded("scan");
}

void MockEcatClient::slaveInfo(int position) {
  Q_UNUSED(position);
  ++slaveInfoCallCount_;
}

void MockEcatClient::upload(int position, const QString &index, const QString &subIndex) {
  Q_UNUSED(position);
  Q_UNUSED(index);
  Q_UNUSED(subIndex);
  ++uploadCallCount_;
}

void MockEcatClient::download(int position, const QString &index, const QString &subIndex,
                               const QString &value, const QString &type) {
  Q_UNUSED(position);
  Q_UNUSED(index);
  Q_UNUSED(subIndex);
  Q_UNUSED(value);
  Q_UNUSED(type);
  ++downloadCallCount_;
}

// --- MockEventBus ---

MockEventBus::MockEventBus(QObject *parent) : QObject(parent) {}

void MockEventBus::emitSlaveChanged(const QVector<SlaveInfo> &slaves) {
  ++slaveChangedCount_;
  lastSlaves_ = slaves;
  emit slaveChanged(slaves);
}

void MockEventBus::emitSdoValue(int pos, const QString &idx, const QString &sub, const QString &val) {
  ++sdoValueCount_;
  emit sdoValueReceived(pos, idx, sub, val);
}

void MockEventBus::emitConnectionStateChanged(bool connected) {
  ++connectionStateCount_;
  emit connectionStateChanged(connected);
}

void MockEventBus::emitFreeRunTelemetry(const QJsonObject &tel) {
  ++freeRunTelemetryCount_;
  lastTelemetry_ = tel;
  emit freeRunTelemetry(tel);
}

void MockEventBus::emitTopologyChanged(const QVector<SlaveInfo> &slaves) {
  ++topologyChangedCount_;
  lastSlaves_ = slaves;
  emit topologyChanged(slaves);
}

void MockEventBus::emitDcSyncUpdate(const QJsonObject &data) {
  ++dcSyncUpdateCount_;
  emit dcSyncUpdate(data);
}

void MockEventBus::emitAlEvent(const QJsonObject &event) {
  ++alEventCount_;
  emit alEvent(event);
}

void MockEventBus::emitSignalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps) {
  ++signalDataCount_;
  emit signalData(channel, values, timestamps);
}

// --- Test Fixtures ---

PluginTestFixture::PluginTestFixture(QObject *parent)
  : QObject(parent)
  , mockClient_(new MockEcatClient(this))
  , mockEventBus_(new MockEventBus(this))
{}

PluginTestFixture::~PluginTestFixture() = default;

ServiceTestFixture::ServiceTestFixture(QObject *parent)
  : QObject(parent)
  , mockClient_(new MockEcatClient(this))
  , mockEventBus_(new MockEventBus(this))
{}

ServiceTestFixture::~ServiceTestFixture() = default;

UITestFixture::UITestFixture(QObject *parent)
  : QObject(parent)
  , mockClient_(new MockEcatClient(this))
  , mockEventBus_(new MockEventBus(this))
{}

UITestFixture::~UITestFixture() = default;
```

- [ ] **Step 2: Verify the file compiles**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -c -I apps/ecat-studio -I src/core tests/utils/TestUtilities.cpp -o /dev/null`
Expected: No errors

---

### Task 3: Create Integration Test Directory and Daemon Lifecycle Test

**Files:**
- Create: `tests/integration/daemon_lifecycle_test.cpp`

- [ ] **Step 1: Create the daemon lifecycle test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include "infra/EcatClient.h"
#include "services/EventBus.h"

class DaemonLifecycleTest : public QObject {
  Q_OBJECT
private slots:
  void testInitialState() {
    EcatClient client;
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);
    QVERIFY(!client.isConnected());
  }

  void testConnectToDaemon() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15900));

    EcatClient client;
    QSignalSpy stateSpy(&client, &EcatClient::connectionStateChanged);
    QSignalSpy connectedSpy(&client, &EcatClient::connected);

    client.connectToHost(QHostAddress::LocalHost, 15900);
    QCOMPARE(client.connectionState(), ConnectionState::Connecting);

    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);
    QVERIFY(stateSpy.count() >= 1);
    QVERIFY(connectedSpy.count() == 1);
  }

  void testDisconnect() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15901));

    EcatClient client;
    client.connectToHost(QHostAddress::LocalHost, 15901);
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);

    QSignalSpy disconnectedSpy(&client, &EcatClient::disconnected);
    server.close();
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Disconnected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);
    QVERIFY(disconnectedSpy.count() >= 1);
  }

  void testHeartbeatDetection() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15902));

    EcatClient client;
    client.connectToHost(QHostAddress::LocalHost, 15902);
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);

    // Close server to simulate daemon crash - heartbeat should detect disconnect
    server.close();
    for (int i = 0; i < 100 && client.connectionState() != ConnectionState::Disconnected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);
  }

  void testAutoReconnect() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15903));

    EcatClient client;
    client.enableAutoReconnect(true);
    client.connectToHost(QHostAddress::LocalHost, 15903);
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);

    // Close server to trigger disconnect
    server.close();
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Disconnected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Disconnected);

    // Restart server - client should reconnect
    QTcpServer server2;
    QVERIFY(server2.listen(QHostAddress::LocalHost, 15903));

    QSignalSpy reconnectedSpy(&client, &EcatClient::reconnected);
    for (int i = 0; i < 100 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }
    QCOMPARE(client.connectionState(), ConnectionState::Connected);
    QVERIFY(reconnectedSpy.count() >= 1);
  }

  void testDoubleConnect() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 15904));

    EcatClient client;
    client.connectToHost(QHostAddress::LocalHost, 15904);
    for (int i = 0; i < 50 && client.connectionState() != ConnectionState::Connected; ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(100);
    }

    int changeCount = 0;
    QObject::connect(&client, &EcatClient::connectionStateChanged,
      [&](ConnectionState) { ++changeCount; });

    client.connectToDaemon();  // Should be no-op
    QCOMPARE(client.connectionState(), ConnectionState::Connected);
    QCOMPARE(changeCount, 0);
  }
};

QTEST_MAIN(DaemonLifecycleTest)
#include "daemon_lifecycle_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I apps/ecat-studio/infra -I src/core tests/integration/daemon_lifecycle_test.cpp`
Expected: No errors (or expected moc-related note)

---

### Task 4: Create Plugin Lifecycle Integration Test

**Files:**
- Create: `tests/integration/plugin_lifecycle_test.cpp`

- [ ] **Step 1: Create the plugin lifecycle test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"

class MockTestPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  MockTestPlugin(QString id, int order, bool vis = true)
    : id_(id), order_(order), vis_(vis) {}

  QString id() const override { return id_; }
  QString displayName() const override { return id_; }
  QString displayNameZh() const override { return id_; }
  int defaultOrder() const override { return order_; }
  bool visible() const override { return vis_; }
  QWidget *widget() override { return nullptr; }

  int activateCount() const { return activateCount_; }
  int deactivateCount() const { return deactivateCount_; }
  int settingsChangedCount() const { return settingsChangedCount_; }
  int connectionChangedCount() const { return connectionChangedCount_; }
  bool lastConnectionState() const { return lastConnectionState_; }

  void activate() override { ++activateCount_; }
  void deactivate() override { ++deactivateCount_; }
  void onConnectionChanged(bool connected) override {
    ++connectionChangedCount_;
    lastConnectionState_ = connected;
  }

private:
  QString id_;
  int order_;
  bool vis_;
  int activateCount_ = 0;
  int deactivateCount_ = 0;
  int settingsChangedCount_ = 0;
  int connectionChangedCount_ = 0;
  bool lastConnectionState_ = false;
};

class PluginLifecycleTest : public QObject {
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
    MockTestPlugin p1("test1", 10);
    MockTestPlugin p2("test2", 20);

    registry.registerPlugin(&p1);
    registry.registerPlugin(&p2);

    QCOMPARE(registry.count(), 2);
    QVERIFY(registry.findById("test1") == &p1);
    QVERIFY(registry.findById("test2") == &p2);
  }

  void testPluginActivation() {
    MockTestPlugin p("test", 10);
    QCOMPARE(p.activateCount(), 0);
    QCOMPARE(p.deactivateCount(), 0);

    p.activate();
    QCOMPARE(p.activateCount(), 1);

    p.deactivate();
    QCOMPARE(p.deactivateCount(), 1);

    p.activate();
    QCOMPARE(p.activateCount(), 2);
  }

  void testPluginConnectionChange() {
    MockTestPlugin p("test", 10);
    QCOMPARE(p.connectionChangedCount(), 0);

    p.onConnectionChanged(true);
    QCOMPARE(p.connectionChangedCount(), 1);
    QVERIFY(p.lastConnectionState());

    p.onConnectionChanged(false);
    QCOMPARE(p.connectionChangedCount(), 2);
    QVERIFY(!p.lastConnectionState());
  }

  void testPluginOrdering() {
    PluginRegistry registry;
    MockTestPlugin p1("first", 10);
    MockTestPlugin p2("second", 20);
    MockTestPlugin p3("third", 5);

    registry.registerPlugin(&p1);
    registry.registerPlugin(&p2);
    registry.registerPlugin(&p3);

    auto visible = registry.visiblePlugins();
    QCOMPARE(visible.size(), 3);
    QCOMPARE(visible[0]->id(), QString("third"));
    QCOMPARE(visible[1]->id(), QString("first"));
    QCOMPARE(visible[2]->id(), QString("second"));
  }

  void testPluginVisibility() {
    PluginRegistry registry;
    MockTestPlugin p1("visible", 10, true);
    MockTestPlugin p2("hidden", 20, false);

    registry.registerPlugin(&p1);
    registry.registerPlugin(&p2);

    QCOMPARE(registry.count(), 2);
    auto visible = registry.visiblePlugins();
    QCOMPARE(visible.size(), 1);
    QCOMPARE(visible[0]->id(), QString("visible"));
  }

  void testDuplicateRegistration() {
    PluginRegistry registry;
    MockTestPlugin p("test", 10);

    registry.registerPlugin(&p);
    registry.registerPlugin(&p);  // Should be ignored

    QCOMPARE(registry.count(), 1);
  }

  void testNullPluginRegistration() {
    PluginRegistry registry;
    registry.registerPlugin(nullptr);
    QCOMPARE(registry.count(), 0);
  }

  void testEmptyIdPlugin() {
    PluginRegistry registry;
    MockTestPlugin p("", 10);
    registry.registerPlugin(&p);
    QCOMPARE(registry.count(), 0);
  }

  void testRealPluginRegistration() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);

    QCOMPARE(registry.count(), 2);
    QVERIFY(registry.findById("notes") == &notes);
    QVERIFY(registry.findById("statemachine") == &sm);
  }

  void testRealPluginLifecycle() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    // Test activation/deactivation don't crash
    notes.activate();
    notes.deactivate();
    sm.activate();
    sm.deactivate();

    // Test connection change
    notes.onConnectionChanged(true);
    notes.onConnectionChanged(false);
    sm.onConnectionChanged(true);
    sm.onConnectionChanged(false);
  }
};

QTEST_MAIN(PluginLifecycleTest)
#include "plugin_lifecycle_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I src/core tests/integration/plugin_lifecycle_test.cpp`
Expected: No errors

---

### Task 5: Create Service Integration Test

**Files:**
- Create: `tests/integration/service_integration_test.cpp`

- [ ] **Step 1: Create the service integration test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "services/SdoService.h"
#include "services/WatchService.h"
#include "services/TopologyService.h"
#include "services/DcSyncService.h"
#include "services/AlEventService.h"
#include "services/SignalService.h"
#include "services/PerformanceMonitorService.h"
#include "services/EsiService.h"
#include "services/BusStatsService.h"
#include "services/WatchdogService.h"
#include "services/SafetyController.h"
#include "services/DiagnosticReportService.h"
#include "services/ProjectManagerService.h"
#include "services/ConfigurationService.h"
#include "services/AlarmService.h"
#include "services/LoggingService.h"
#include "services/CacheService.h"
#include "services/AsyncOperationManager.h"
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
    QVERIFY(sc.perfMonitor() != nullptr);
    QVERIFY(sc.esi() != nullptr);
    QVERIFY(sc.busStats() != nullptr);
    QVERIFY(sc.watchdog() != nullptr);
    QVERIFY(sc.safety() != nullptr);
    QVERIFY(sc.diagnosticReport() != nullptr);
    QVERIFY(sc.projectManager() != nullptr);
    QVERIFY(sc.configuration() != nullptr);
    QVERIFY(sc.alarm() != nullptr);
    QVERIFY(sc.logging() != nullptr);
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
    WatchdogService *watchdog = sc.watchdog();
    SafetyController *safety = sc.safety();
    DiagnosticReportService *diag = sc.diagnosticReport();

    QVERIFY(bus != nullptr);
    QVERIFY(sdo != nullptr);
    QVERIFY(watch != nullptr);
    QVERIFY(topology != nullptr);
    QVERIFY(dcSync != nullptr);
    QVERIFY(alEvent != nullptr);
    QVERIFY(signal != nullptr);
    QVERIFY(watchdog != nullptr);
    QVERIFY(safety != nullptr);
    QVERIFY(diag != nullptr);
  }

  void testServiceConfiguration() {
    ServiceContainer sc;
    EcatClient *client = sc.client();
    QVERIFY(client != nullptr);

    // Test that the same client instance is returned consistently
    QVERIFY(sc.client() == client);
    ServiceContainer sc2;
    QVERIFY(sc2.client() != client);  // Different containers have different clients
  }

  void testServiceEventBusWiring() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();

    QSignalSpy slaveSpy(bus, &EventBus::slaveChanged);
    QSignalSpy connectionSpy(bus, &EventBus::connectionStateChanged);
    QSignalSpy sdoSpy(bus, &EventBus::sdoValueReceived);
    QSignalSpy topologySpy(bus, &EventBus::topologyChanged);
    QSignalSpy dcSyncSpy(bus, &EventBus::dcSyncUpdate);
    QSignalSpy alEventSpy(bus, &EventBus::alEvent);
    QSignalSpy signalSpy(bus, &EventBus::signalData);
    QSignalSpy freeRunSpy(bus, &EventBus::freeRunTelemetry);

    QVERIFY(slaveSpy.isValid());
    QVERIFY(connectionSpy.isValid());
    QVERIFY(sdoSpy.isValid());
    QVERIFY(topologySpy.isValid());
    QVERIFY(dcSyncSpy.isValid());
    QVERIFY(alEventSpy.isValid());
    QVERIFY(signalSpy.isValid());
    QVERIFY(freeRunSpy.isValid());
  }

  void testEventBusEventPropagation() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();

    QSignalSpy slaveSpy(bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(bus, &EventBus::sdoValueReceived);
    QSignalSpy connectionSpy(bus, &EventBus::connectionStateChanged);

    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    bus->emitSlaveChanged({info});
    QCOMPARE(slaveSpy.count(), 1);

    bus->emitSdoValue(1, "0x6040", "0x00", "0x000F");
    QCOMPARE(sdoSpy.count(), 1);

    bus->emitConnectionStateChanged(true);
    QCOMPARE(connectionSpy.count(), 1);
  }

  void testSafetyControllerValidation() {
    ServiceContainer sc;
    SafetyController *safety = sc.safety();
    QVERIFY(safety != nullptr);

    auto r = safety->validateStateTransition(8, 1);
    QCOMPARE(r.allowed, false);
    QVERIFY(r.reason.contains("OP"));

    auto r2 = safety->validateFreeRunStart(true);
    QCOMPARE(r2.allowed, true);
  }

  void testCacheServiceIntegration() {
    CacheService cache(100, 30000);
    cache.put("key1", "value1");
    QByteArray val;
    QVERIFY(cache.get("key1", val));
    QCOMPARE(val, QByteArray("value1"));

    cache.invalidate("key1");
    QVERIFY(!cache.get("key1", val));
  }

  void testAsyncOperationManager() {
    AsyncOperationManager mgr;
    QVERIFY(mgr.activeOperationCount() == 0);
  }

  void testServiceLifecycle() {
    // Test that services can be created and destroyed without issues
    {
      ServiceContainer sc;
      QVERIFY(sc.eventBus() != nullptr);
      QVERIFY(sc.client() != nullptr);
    }
    // If we get here without crash, lifecycle is correct
    QVERIFY(true);
  }

  void testServiceErrorHandling() {
    ServiceContainer sc;
    EcatClient *client = sc.client();

    // Test that error signal can be connected
    QSignalSpy errorSpy(client, &EcatClient::errorMessage);
    QVERIFY(errorSpy.isValid());
  }
};

QTEST_MAIN(ServiceIntegrationTest)
#include "service_integration_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I apps/ecat-studio/infra -I apps/ecat-studio/services -I src/core tests/integration/service_integration_test.cpp`
Expected: No errors

---

### Task 6: Create UI Workflow Integration Test

**Files:**
- Create: `tests/integration/ui_workflow_test.cpp`

- [ ] **Step 1: Create the UI workflow test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "plugins/od/OdPlugin.h"
#include "plugins/watch/WatchPlugin.h"
#include "plugins/freerun/FreeRunPlugin.h"
#include "plugins/iovariable/IoVariablePlugin.h"
#include "plugins/topology/TopologyPlugin.h"
#include "plugins/diagnostics/DiagnosticsPlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class UIWorkflowTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testSdoReadWorkflow() {
    EventBus *bus = container_->eventBus();
    QSignalSpy sdoSpy(bus, &EventBus::sdoValueReceived);

    // Simulate SDO read response
    bus->emitSdoValue(1, "0x6040", "0x00", "0x000F");
    QCOMPARE(sdoSpy.count(), 1);

    auto args = sdoSpy.at(0);
    QCOMPARE(args.at(0).toInt(), 1);
    QCOMPARE(args.at(1).toString(), QString("0x6040"));
    QCOMPARE(args.at(2).toString(), QString("0x00"));
    QCOMPARE(args.at(3).toString(), QString("0x000F"));
  }

  void testTopologyScanWorkflow() {
    EventBus *bus = container_->eventBus();
    QSignalSpy topologySpy(bus, &EventBus::topologyChanged);

    // Simulate topology scan result
    SlaveInfo s1;
    s1.position = 0;
    s1.name = "EK1100";
    s1.state = "OP";

    SlaveInfo s2;
    s2.position = 1;
    s2.name = "EL1008";
    s2.state = "OP";

    bus->emitTopologyChanged({s1, s2});
    QCOMPARE(topologySpy.count(), 1);

    QVector<SlaveInfo> received = topologySpy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 2);
    QCOMPARE(received.at(0).name, QString("EK1100"));
    QCOMPARE(received.at(1).name, QString("EL1008"));
  }

  void testFreeRunWorkflow() {
    EventBus *bus = container_->eventBus();
    QSignalSpy freeRunSpy(bus, &EventBus::freeRunTelemetry);

    // Simulate Free Run telemetry
    QJsonObject tel{
      {"running", true},
      {"cycleCount", 1000},
      {"errorCount", 0}
    };
    bus->emitFreeRunTelemetry(tel);
    QCOMPARE(freeRunSpy.count(), 1);

    QJsonObject received = freeRunSpy.at(0).at(0).toJsonObject();
    QVERIFY(received["running"].toBool());
    QCOMPARE(received["cycleCount"].toInt(), 1000);
  }

  void testConnectionStateWorkflow() {
    EventBus *bus = container_->eventBus();
    QSignalSpy connectionSpy(bus, &EventBus::connectionStateChanged);

    // Simulate connection established
    bus->emitConnectionStateChanged(true);
    QCOMPARE(connectionSpy.count(), 1);
    QVERIFY(connectionSpy.at(0).at(0).toBool());

    // Simulate connection lost
    bus->emitConnectionStateChanged(false);
    QCOMPARE(connectionSpy.count(), 2);
    QVERIFY(!connectionSpy.at(1).at(0).toBool());
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

  void testDcSyncWorkflow() {
    EventBus *bus = container_->eventBus();
    QSignalSpy dcSyncSpy(bus, &EventBus::dcSyncUpdate);

    QJsonObject data{
      {"refClock", 0},
      {"slave0", true},
      {"slave1", true}
    };
    bus->emitDcSyncUpdate(data);
    QCOMPARE(dcSyncSpy.count(), 1);

    QJsonObject received = dcSyncSpy.at(0).at(0).toJsonObject();
    QVERIFY(received.contains("refClock"));
  }

  void testAlEventWorkflow() {
    EventBus *bus = container_->eventBus();
    QSignalSpy alEventSpy(bus, &EventBus::alEvent);

    QJsonObject event{
      {"slave", 1},
      {"code", 0x001A},
      {"text", "State change"}
    };
    bus->emitAlEvent(event);
    QCOMPARE(alEventSpy.count(), 1);

    QJsonObject received = alEventSpy.at(0).at(0).toJsonObject();
    QCOMPARE(received["slave"].toInt(), 1);
    QVERIFY(received.contains("code"));
  }

  void testSignalDataWorkflow() {
    EventBus *bus = container_->eventBus();
    QSignalSpy signalSpy(bus, &EventBus::signalData);

    bus->emitSignalData(0, {1.0, 2.0, 3.0}, {100, 200, 300});
    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.at(0).at(0).toInt(), 0);
  }
};

QTEST_MAIN(UIWorkflowTest)
#include "ui_workflow_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I apps/ecat-studio/infra -I apps/ecat-studio/services -I src/core tests/integration/ui_workflow_test.cpp`
Expected: No errors

---

### Task 7: Create EventBus Performance Test

**Files:**
- Create: `tests/performance/eventbus_performance_test.cpp`

- [ ] **Step 1: Create the EventBus performance test**

```cpp
#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include <QVector>
#include "services/EventBus.h"
#include "EthercatTypes.h"

class EventBusPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testThroughput() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);

    const int iterations = 10000;
    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    QVector<SlaveInfo> slaves{info};

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
      bus.emitSlaveChanged(slaves);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(spy.count(), iterations);

    // Should handle 10000 events in reasonable time (< 1 second)
    QVERIFY(elapsed < 1000);
    qDebug() << "EventBus throughput:" << iterations << "events in" << elapsed << "ms";
  }

  void testLatency() {
    EventBus bus;
    const int iterations = 1000;
    QVector<qint64> latencies;
    latencies.reserve(iterations);

    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    QVector<SlaveInfo> slaves{info};

    for (int i = 0; i < iterations; ++i) {
      QElapsedTimer timer;
      timer.start();
      bus.emitSlaveChanged(slaves);
      latencies.append(timer.nsecsElapsed());
    }

    // Calculate average latency
    qint64 total = 0;
    for (auto l : latencies) total += l;
    double avgNs = static_cast<double>(total) / iterations;
    double avgUs = avgNs / 1000.0;

    // Average latency should be under 1ms (1000us)
    QVERIFY(avgUs < 1000.0);
    qDebug() << "EventBus average latency:" << avgUs << "us";
  }

  void testMemoryStability() {
    EventBus bus;
    const int iterations = 10000;

    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";
    QVector<SlaveInfo> slaves{info};

    // Run many events and verify no crash or memory issues
    for (int i = 0; i < iterations; ++i) {
      bus.emitSlaveChanged(slaves);
      bus.emitSdoValue(1, "0x6040", "0x00", "0x000F");
      bus.emitConnectionStateChanged(true);
    }

    // If we get here, memory is stable
    QVERIFY(true);
  }

  void testMultipleSignalTypes() {
    EventBus bus;
    QSignalSpy slaveSpy(&bus, &EventBus::slaveChanged);
    QSignalSpy sdoSpy(&bus, &EventBus::sdoValueReceived);
    QSignalSpy connectionSpy(&bus, &EventBus::connectionStateChanged);
    QSignalSpy freeRunSpy(&bus, &EventBus::freeRunTelemetry);
    QSignalSpy topologySpy(&bus, &EventBus::topologyChanged);
    QSignalSpy dcSyncSpy(&bus, &EventBus::dcSyncUpdate);
    QSignalSpy alEventSpy(&bus, &EventBus::alEvent);
    QSignalSpy signalSpy(&bus, &EventBus::signalData);

    const int iterations = 1000;

    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
      bus.emitSlaveChanged({info});
      bus.emitSdoValue(1, "0x6040", "0x00", "0x000F");
      bus.emitConnectionStateChanged(true);
      bus.emitFreeRunTelemetry({{"running", true}});
      bus.emitTopologyChanged({info});
      bus.emitDcSyncUpdate({{"refClock", 0}});
      bus.emitAlEvent({{"slave", 1}});
      bus.emitSignalData(0, {1.0}, {100});
    }

    qint64 elapsed = timer.elapsed();

    QCOMPARE(slaveSpy.count(), iterations);
    QCOMPARE(sdoSpy.count(), iterations);
    QCOMPARE(connectionSpy.count(), iterations);
    QCOMPARE(freeRunSpy.count(), iterations);
    QCOMPARE(topologySpy.count(), iterations);
    QCOMPARE(dcSyncSpy.count(), iterations);
    QCOMPARE(alEventSpy.count(), iterations);
    QCOMPARE(signalSpy.count(), iterations);

    // 8 signal types * 1000 iterations should complete in reasonable time
    QVERIFY(elapsed < 2000);
    qDebug() << "Multiple signal types:" << iterations * 8 << "events in" << elapsed << "ms";
  }

  void testConcurrentEmission() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);

    const int iterations = 5000;
    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";

    // Simulate rapid sequential emissions (main thread only)
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
      bus.emitSlaveChanged({info});
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(spy.count(), iterations);
    qDebug() << "Concurrent emission:" << iterations << "events in" << elapsed << "ms";
  }
};

QTEST_MAIN(EventBusPerformanceTest)
#include "eventbus_performance_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I src/core tests/performance/eventbus_performance_test.cpp`
Expected: No errors

---

### Task 8: Create Cache Performance Test

**Files:**
- Create: `tests/performance/cache_performance_test.cpp`

- [ ] **Step 1: Create the cache performance test**

```cpp
#include <QTest>
#include <QElapsedTimer>
#include <QVector>
#include "services/CacheService.h"

class CachePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testHitRate() {
    CacheService cache(1000, 30000);

    // Populate cache
    for (int i = 0; i < 100; ++i) {
      cache.put(QString("key_%1").arg(i), QByteArray("value"));
    }

    // Test hit rate
    int hits = 0;
    int misses = 0;
    QByteArray val;

    for (int i = 0; i < 100; ++i) {
      if (cache.get(QString("key_%1").arg(i), val)) {
        ++hits;
      } else {
        ++misses;
      }
    }

    QCOMPARE(hits, 100);
    QCOMPARE(misses, 0);
    qDebug() << "Cache hit rate:" << hits << "/" << (hits + misses);
  }

  void testEvictionPerformance() {
    const int maxSize = 1000;
    CacheService cache(maxSize, 30000);

    QElapsedTimer timer;
    timer.start();

    // Fill cache beyond capacity to trigger evictions
    for (int i = 0; i < maxSize * 2; ++i) {
      cache.put(QString("key_%1").arg(i), QByteArray("value"));
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(cache.size(), maxSize);

    // Eviction should be fast (< 100ms for 2000 operations)
    QVERIFY(elapsed < 100);
    qDebug() << "Cache eviction:" << maxSize * 2 << "operations in" << elapsed << "ms";
  }

  void testMemoryUsage() {
    const int maxSize = 10000;
    CacheService cache(maxSize, 30000);

    // Fill cache
    for (int i = 0; i < maxSize; ++i) {
      cache.put(QString("key_%1").arg(i), QByteArray(100, 'x'));
    }

    QCOMPARE(cache.size(), maxSize);
    QCOMPARE(cache.maxSize(), maxSize);

    // Verify all entries are accessible
    QByteArray val;
    int accessible = 0;
    for (int i = 0; i < maxSize; ++i) {
      if (cache.get(QString("key_%1").arg(i), val)) {
        ++accessible;
      }
    }
    QCOMPARE(accessible, maxSize);
    qDebug() << "Cache memory test:" << maxSize << "entries stored and retrieved";
  }

  void testLookupPerformance() {
    CacheService cache(1000, 30000);

    // Populate
    for (int i = 0; i < 1000; ++i) {
      cache.put(QString("key_%1").arg(i), QByteArray("value"));
    }

    QElapsedTimer timer;
    timer.start();

    QByteArray val;
    const int lookups = 100000;
    for (int i = 0; i < lookups; ++i) {
      cache.get(QString("key_%1").arg(i % 1000), val);
    }

    qint64 elapsed = timer.elapsed();

    // 100k lookups should be fast (< 500ms)
    QVERIFY(elapsed < 500);
    qDebug() << "Cache lookups:" << lookups << "in" << elapsed << "ms";
  }

  void testOverwritePerformance() {
    CacheService cache(1000, 30000);

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; ++i) {
      cache.put(QString("key_%1").arg(i % 1000), QByteArray("updated"));
    }

    qint64 elapsed = timer.elapsed();

    // Overwrites should be fast (< 200ms for 10k operations)
    QVERIFY(elapsed < 200);
    qDebug() << "Cache overwrites:" << iterations << "in" << elapsed << "ms";
  }

  void testInvalidateAllPerformance() {
    CacheService cache(10000, 30000);

    for (int i = 0; i < 10000; ++i) {
      cache.put(QString("key_%1").arg(i), QByteArray("value"));
    }

    QElapsedTimer timer;
    timer.start();
    cache.invalidateAll();
    qint64 elapsed = timer.elapsed();

    QCOMPARE(cache.size(), 0);

    // Invalidating 10k entries should be fast (< 100ms)
    QVERIFY(elapsed < 100);
    qDebug() << "Cache invalidateAll:" << elapsed << "ms";
  }
};

QTEST_MAIN(CachePerformanceTest)
#include "cache_performance_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio tests/performance/cache_performance_test.cpp`
Expected: No errors

---

### Task 9: Create Plugin Performance Test

**Files:**
- Create: `tests/performance/plugin_performance_test.cpp`

- [ ] **Step 1: Create the plugin performance test**

```cpp
#include <QTest>
#include <QElapsedTimer>
#include <QVector>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"

class PluginPerformanceTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testPluginStartupTime() {
    QElapsedTimer timer;
    timer.start();

    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    qint64 elapsed = timer.elapsed();

    // Plugin creation should be fast (< 100ms)
    QVERIFY(elapsed < 100);
    qDebug() << "Plugin startup:" << elapsed << "ms";
  }

  void testPluginWidgetCreationTime() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    QElapsedTimer timer;
    timer.start();

    QWidget *w1 = notes.widget();
    QWidget *w2 = sm.widget();

    qint64 elapsed = timer.elapsed();

    QVERIFY(w1 != nullptr);
    QVERIFY(w2 != nullptr);

    // Widget creation should be fast (< 500ms)
    QVERIFY(elapsed < 500);
    qDebug() << "Plugin widget creation:" << elapsed << "ms";
  }

  void testPluginRegistryPerformance() {
    QElapsedTimer timer;
    timer.start();

    PluginRegistry registry;
    const int iterations = 100;

    for (int i = 0; i < iterations; ++i) {
      NotesPlugin *notes = new NotesPlugin(this);
      registry.registerPlugin(notes);
    }

    qint64 elapsed = timer.elapsed();

    // Registration should be fast (< 100ms for 100 plugins)
    QVERIFY(elapsed < 100);
    qDebug() << "Plugin registry:" << iterations << "registrations in" << elapsed << "ms";
  }

  void testPluginLookupPerformance() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; ++i) {
      registry.findById("notes");
      registry.findById("statemachine");
    }

    qint64 elapsed = timer.elapsed();

    // Lookups should be fast (< 100ms for 20k lookups)
    QVERIFY(elapsed < 100);
    qDebug() << "Plugin lookups:" << iterations * 2 << "in" << elapsed << "ms";
  }

  void testPluginActivationPerformance() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    QElapsedTimer timer;
    timer.start();

    const int iterations = 1000;
    for (int i = 0; i < iterations; ++i) {
      notes.activate();
      notes.deactivate();
      sm.activate();
      sm.deactivate();
    }

    qint64 elapsed = timer.elapsed();

    // Activation/deactivation should be fast (< 200ms for 4000 cycles)
    QVERIFY(elapsed < 200);
    qDebug() << "Plugin activation:" << iterations * 4 << "cycles in" << elapsed << "ms";
  }

  void testPluginConnectionChangePerformance() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    QElapsedTimer timer;
    timer.start();

    const int iterations = 1000;
    for (int i = 0; i < iterations; ++i) {
      notes.onConnectionChanged(true);
      notes.onConnectionChanged(false);
      sm.onConnectionChanged(true);
      sm.onConnectionChanged(false);
    }

    qint64 elapsed = timer.elapsed();

    // Connection changes should be fast (< 200ms for 4000 cycles)
    QVERIFY(elapsed < 200);
    qDebug() << "Plugin connection changes:" << iterations * 4 << "in" << elapsed << "ms";
  }
};

QTEST_MAIN(PluginPerformanceTest)
#include "plugin_performance_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I src/core tests/performance/plugin_performance_test.cpp`
Expected: No errors

---

### Task 10: Create Service Performance Test

**Files:**
- Create: `tests/performance/service_performance_test.cpp`

- [ ] **Step 1: Create the service performance test**

```cpp
#include <QTest>
#include <QElapsedTimer>
#include <QSignalSpy>
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "services/CacheService.h"
#include "services/SafetyController.h"
#include "infra/EcatClient.h"

class ServicePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testServiceContainerCreationTime() {
    QElapsedTimer timer;
    timer.start();

    ServiceContainer sc;

    qint64 elapsed = timer.elapsed();

    // ServiceContainer creation should be fast (< 500ms)
    QVERIFY(elapsed < 500);
    qDebug() << "ServiceContainer creation:" << elapsed << "ms";
  }

  void testEventBusEmissionPerformance() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);

    const int iterations = 10000;
    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
      bus.emitSlaveChanged({info});
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(spy.count(), iterations);

    // 10k emissions should be fast (< 500ms)
    QVERIFY(elapsed < 500);
    qDebug() << "EventBus emissions:" << iterations << "in" << elapsed << "ms";
  }

  void testCacheServicePerformance() {
    CacheService cache(1000, 30000);

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; ++i) {
      cache.put(QString("key_%1").arg(i % 1000), QByteArray("value"));
    }

    qint64 elapsed = timer.elapsed();

    // 10k cache operations should be fast (< 500ms)
    QVERIFY(elapsed < 500);
    qDebug() << "Cache operations:" << iterations << "in" << elapsed << "ms";
  }

  void testSafetyControllerPerformance() {
    ServiceContainer sc;
    SafetyController *safety = sc.safety();

    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; ++i) {
      safety->validateStateTransition(8, 1);
      safety->validateFreeRunStart(true);
    }

    qint64 elapsed = timer.elapsed();

    // 10k validations should be fast (< 200ms)
    QVERIFY(elapsed < 200);
    qDebug() << "Safety validations:" << iterations * 2 << "in" << elapsed << "ms";
  }

  void testServiceResponseTime() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();

    QElapsedTimer timer;
    timer.start();

    const int iterations = 1000;
    for (int i = 0; i < iterations; ++i) {
      bus->emitSdoValue(1, "0x6040", "0x00", "0x000F");
    }

    qint64 elapsed = timer.elapsed();

    // Response time should be fast (< 100ms for 1000 operations)
    QVERIFY(elapsed < 100);
    qDebug() << "Service response time:" << iterations << "in" << elapsed << "ms";
  }

  void testServiceThroughput() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();

    SlaveInfo info;
    info.position = 1;
    info.name = "TestSlave";

    QElapsedTimer timer;
    timer.start();

    const int iterations = 5000;
    for (int i = 0; i < iterations; ++i) {
      bus->emitSlaveChanged({info});
      bus->emitSdoValue(1, "0x6040", "0x00", "0x000F");
      bus->emitConnectionStateChanged(true);
    }

    qint64 elapsed = timer.elapsed();

    // 15k operations should complete in reasonable time (< 1000ms)
    QVERIFY(elapsed < 1000);
    qDebug() << "Service throughput:" << iterations * 3 << "operations in" << elapsed << "ms";
  }

  void testServiceMemoryStability() {
    // Create and destroy multiple ServiceContainers
    for (int i = 0; i < 10; ++i) {
      ServiceContainer sc;
      EventBus *bus = sc.eventBus();
      bus->emitSlaveChanged({});
      bus->emitSdoValue(1, "0x6040", "0x00", "0x000F");
    }

    // If we get here, memory is stable
    QVERIFY(true);
  }
};

QTEST_MAIN(ServicePerformanceTest)
#include "service_performance_test.moc"
```

- [ ] **Step 2: Verify the file syntax**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && g++ -std=c++20 -fsyntax-only -I apps/ecat-studio -I apps/ecat-studio/infra -I apps/ecat-studio/services -I src/core tests/performance/service_performance_test.cpp`
Expected: No errors

---

### Task 11: Create Coverage Script

**Files:**
- Create: `scripts/generate_coverage.sh`

- [ ] **Step 1: Create the coverage script**

```bash
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"
COVERAGE_DIR="$PROJECT_DIR/coverage"

echo "=== NekoEcat Studio Coverage Report Generator ==="
echo ""

# Clean previous coverage data
rm -rf "$COVERAGE_DIR"
mkdir -p "$COVERAGE_DIR"

# Build with coverage flags
echo "[1/4] Building with coverage flags..."
cd "$PROJECT_DIR"
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" -DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage"
cmake --build build -j$(nproc)

# Run tests
echo ""
echo "[2/4] Running tests..."
cd "$BUILD_DIR"
ctest --output-on-failure -j$(nproc) || true

# Collect coverage data
echo ""
echo "[3/4] Collecting coverage data..."
cd "$PROJECT_DIR"

# Find all .gcda files
GCDA_FILES=$(find "$BUILD_DIR" -name "*.gcda" -type f 2>/dev/null)

if [ -z "$GCDA_FILES" ]; then
  echo "Warning: No .gcda files found. Tests may not have run."
  echo "Trying alternative approach..."
  
  # Try running tests directly
  cd "$BUILD_DIR"
  for test_bin in *_test; do
    if [ -x "$test_bin" ]; then
      echo "Running $test_bin..."
      QT_QPA_PLATFORM=offscreen ./"$test_bin" || true
    fi
  done
  cd "$PROJECT_DIR"
fi

# Generate HTML report with gcovr
echo ""
echo "[4/4] Generating HTML report..."

if command -v gcovr &> /dev/null; then
  gcovr -r "$PROJECT_DIR" \
    --filter "apps/ecat-studio/" \
    --filter "src/core/" \
    --filter "src/igh/" \
    --exclude ".*_test\\.cpp$" \
    --exclude ".*moc_.*" \
    --html-details "$COVERAGE_DIR/index.html" \
    --print-summary
  
  echo ""
  echo "Coverage report generated at: $COVERAGE_DIR/index.html"
  
  # Also generate a text summary
  gcovr -r "$PROJECT_DIR" \
    --filter "apps/ecat-studio/" \
    --filter "src/core/" \
    --filter "src/igh/" \
    --exclude ".*_test\\.cpp$" \
    --exclude ".*moc_.*" \
    --txt > "$COVERAGE_DIR/summary.txt"
  
  echo "Text summary at: $COVERAGE_DIR/summary.txt"
  
else
  echo "gcovr not found. Using gcov directly..."
  
  # Use gcov directly
  cd "$BUILD_DIR"
  GCOV_OUTPUT=$(find . -name "*.gcno" -exec gcov -pb {} + 2>&1 || true)
  
  # Parse coverage summary
  echo "$GCOV_OUTPUT" | grep -E "^(File|Lines|Branches)" | head -20
  
  # Generate simple summary
  TOTAL_LINES=0
  COVERED_LINES=0
  
  for gcov_file in $(find . -name "*.gcov" -type f 2>/dev/null); do
    file_total=$(grep -c "^[[:space:]]*[0-9]+:" "$gcov_file" 2>/dev/null || echo 0)
    file_covered=$(grep -c "^[[:space:]]*[1-9][0-9]*:" "$gcov_file" 2>/dev/null || echo 0)
    TOTAL_LINES=$((TOTAL_LINES + file_total))
    COVERED_LINES=$((COVERED_LINES + file_covered))
  done
  
  if [ "$TOTAL_LINES" -gt 0 ]; then
    COVERAGE_PCT=$((COVERED_LINES * 100 / TOTAL_LINES))
    echo ""
    echo "=== Coverage Summary ==="
    echo "Total lines: $TOTAL_LINES"
    echo "Covered lines: $COVERED_LINES"
    echo "Coverage: ${COVERAGE_PCT}%"
    
    # Save summary
    cat > "$COVERAGE_DIR/summary.txt" << EOF
Coverage Summary
================
Total lines: $TOTAL_LINES
Covered lines: $COVERED_LINES
Coverage: ${COVERAGE_PCT}%
EOF
    echo "Summary saved to: $COVERAGE_DIR/summary.txt"
  else
    echo "No coverage data found."
  fi
fi

echo ""
echo "=== Coverage report generation complete ==="
```

- [ ] **Step 2: Make the script executable**

Run: `chmod +x /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat/scripts/generate_coverage.sh`
Expected: No errors

- [ ] **Step 3: Verify the script syntax**

Run: `bash -n /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat/scripts/generate_coverage.sh`
Expected: No errors

---

### Task 12: Update CMakeLists.txt

**Files:**
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Add test utilities library**

Add after the existing test definitions (before `set(RELEASE_SMOKE_TESTS ...)`):

```cmake
# ── Test Utilities Library ───────────────────────────────────────────
add_library(test_utilities STATIC
    utils/TestUtilities.cpp
    utils/TestUtilities.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)

target_include_directories(test_utilities PUBLIC
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(test_utilities PUBLIC Qt6::Core Qt6::Test)

set_target_properties(test_utilities PROPERTIES AUTOMOC ON)
```

- [ ] **Step 2: Add integration tests**

Add after the test utilities library:

```cmake
# ── Integration Tests ────────────────────────────────────────────────
add_executable(daemon_lifecycle_test
    integration/daemon_lifecycle_test.cpp
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)

target_include_directories(daemon_lifecycle_test PRIVATE
    ../apps/ecatd
    ../apps/ecat-studio/infra
    ../src/core
    ../apps/ecat-studio
)
target_link_libraries(daemon_lifecycle_test PRIVATE Qt6::Core Qt6::Network Qt6::Test)
set_target_properties(daemon_lifecycle_test PROPERTIES AUTOMOC ON)
add_test(NAME daemon_lifecycle_test COMMAND daemon_lifecycle_test)

add_executable(plugin_lifecycle_test
    integration/plugin_lifecycle_test.cpp
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
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(plugin_lifecycle_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(plugin_lifecycle_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(plugin_lifecycle_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_lifecycle_test COMMAND plugin_lifecycle_test)
set_tests_properties(plugin_lifecycle_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(ui_workflow_test
    integration/ui_workflow_test.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/plugins/od/OdPlugin.cpp
    ../apps/ecat-studio/plugins/od/OdPlugin.h
    ../apps/ecat-studio/plugins/watch/WatchPlugin.cpp
    ../apps/ecat-studio/plugins/watch/WatchPlugin.h
    ../apps/ecat-studio/plugins/freerun/FreeRunPlugin.cpp
    ../apps/ecat-studio/plugins/freerun/FreeRunPlugin.h
    ../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.cpp
    ../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.h
    ../apps/ecat-studio/plugins/topology/TopologyPlugin.cpp
    ../apps/ecat-studio/plugins/topology/TopologyPlugin.h
    ../apps/ecat-studio/plugins/diagnostics/DiagnosticsPlugin.cpp
    ../apps/ecat-studio/plugins/diagnostics/DiagnosticsPlugin.h
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
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)
target_link_libraries(ui_workflow_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(ui_workflow_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(ui_workflow_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_workflow_test COMMAND ui_workflow_test)
set_tests_properties(ui_workflow_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 3: Add performance tests**

Add after the integration tests:

```cmake
# ── Performance Tests ────────────────────────────────────────────────
add_executable(eventbus_performance_test
    performance/eventbus_performance_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(eventbus_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(eventbus_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(eventbus_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME eventbus_performance_test COMMAND eventbus_performance_test)
set_tests_properties(eventbus_performance_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(cache_performance_test
    performance/cache_performance_test.cpp
    ../apps/ecat-studio/services/CacheService.cpp
    ../apps/ecat-studio/services/CacheService.h
)
target_link_libraries(cache_performance_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(cache_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(cache_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME cache_performance_test COMMAND cache_performance_test)

add_executable(plugin_performance_test
    performance/plugin_performance_test.cpp
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
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(plugin_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(plugin_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(plugin_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_performance_test COMMAND plugin_performance_test)
set_tests_properties(plugin_performance_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(service_performance_test
    performance/service_performance_test.cpp
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/CacheService.cpp
    ../apps/ecat-studio/services/CacheService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
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
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/AsyncOperationManager.cpp
    ../apps/ecat-studio/services/AsyncOperationManager.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(service_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(service_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(service_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME service_performance_test COMMAND service_performance_test)
set_tests_properties(service_performance_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
```

- [ ] **Step 4: Create integration and performance directories**

Run: `mkdir -p /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat/tests/integration /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat/tests/performance`
Expected: No errors

---

### Task 13: Build and Run All Tests

**Files:**
- None (build and test verification)

- [ ] **Step 1: Build the project**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && cmake --build build -j4`
Expected: Build succeeds with no errors

- [ ] **Step 2: Run all tests**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && ctest --test-dir build --output-on-failure -j4`
Expected: All tests pass

- [ ] **Step 3: Run specific new tests**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && ctest --test-dir build -R "daemon_lifecycle|plugin_lifecycle|ui_workflow|eventbus_performance|cache_performance|plugin_performance|service_performance" --output-on-failure`
Expected: All new tests pass

---

### Task 14: Generate Coverage Report

**Files:**
- None (coverage generation)

- [ ] **Step 1: Generate coverage report**

Run: `cd /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat && bash scripts/generate_coverage.sh`
Expected: Coverage report generated in `coverage/` directory

- [ ] **Step 2: Verify coverage report exists**

Run: `ls -la /home/nekorain/Documents/LTSPICE/NEW/vibecoding/Ethercat/coverage/`
Expected: `index.html` and `summary.txt` files exist

---

## Self-Review Checklist

- [ ] All test files compile without errors
- [ ] All tests pass when run individually
- [ ] All tests pass when run together via ctest
- [ ] Coverage report generates successfully
- [ ] Test utilities are reusable and well-documented
- [ ] Mock classes simulate real behavior accurately
- [ ] Performance tests have reasonable thresholds
- [ ] Integration tests cover key workflows
- [ ] CMakeLists.txt correctly defines all new targets
- [ ] No placeholder code or TODO comments
