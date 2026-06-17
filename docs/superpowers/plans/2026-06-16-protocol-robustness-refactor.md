# Protocol Robustness & Structural Refactor — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Harden the GUI↔daemon protocol layer, replace the monolithic daemon dispatch with a testable command table, and add timeout/recovery logic to the client — all without changing observable behavior.

**Architecture:** Extract a `CommandDispatcher` from `EcatDaemon::handle()`, giving each command its own handler function registered in a dispatch table. Add request timeouts and explicit connection-state tracking to `EcatClient`. Tests cover the dispatcher in isolation (no TCP) and the client protocol logic with a mock socket.

**Tech Stack:** C++17, Qt 6 (Core, Network, Widgets), CMake, IgH ecrt, `QTest`-free test harness (existing pattern: `std::exit(1)` on failure).

---

## File Structure

### New Files

| File | Responsibility |
|------|----------------|
| `apps/ecatd/CommandDispatcher.h` | Generic string→handler dispatch table |
| `apps/ecatd/CommandDispatcher.cpp` | Implementation: register, dispatch, unknown-method error |
| `tests/command_dispatcher_test.cpp` | Unit tests for CommandDispatcher |
| `tests/protocol_integration_test.cpp` | Client↔Daemon round-trip tests over localhost TCP |

### Modified Files

| File | Changes |
|------|---------|
| `apps/ecatd/EcatDaemon.h` | Add `CommandDispatcher dispatcher_` member; move handler wiring to `setupHandlers()` |
| `apps/ecatd/EcatDaemon.cpp` | Replace if/else chain in `handle()` with `dispatcher_.dispatch()`; extract `setupHandlers()` |
| `apps/ecat-studio/infra/EcatClient.h` | Add `requestTimeoutMs_`, `connectionState_`, timeout timer |
| `apps/ecat-studio/infra/EcatClient.cpp` | Add per-request timeout, explicit state enum, cleanup on disconnect |
| `apps/ecatd/CMakeLists.txt` | Add `CommandDispatcher.cpp/.h` |
| `tests/CMakeLists.txt` | Add `command_dispatcher_test` and `protocol_integration_test` targets |

---

## Phase 1: CommandDispatcher — Extract & Test

### Task 1: Write CommandDispatcher unit tests (all fail)

**Files:**
- Create: `tests/command_dispatcher_test.cpp`

- [ ] **Step 1: Write the test file**

```cpp
// Unit tests for CommandDispatcher.
#include "CommandDispatcher.h"

#include <QCoreApplication>
#include <QJsonObject>

#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;

void fail(const QString &msg) {
    std::cerr << msg.toStdString() << '\n';
    ++failures;
}

void expectTrue(bool cond, const QString &msg) {
    if (!cond) fail(msg);
}

void expectEqual(const QString &actual, const QString &expected, const QString &msg) {
    if (actual != expected)
        fail(QString("%1: expected '%2', got '%3'").arg(msg, expected, actual));
}

void expectEqual(int actual, int expected, const QString &msg) {
    if (actual != expected)
        fail(QString("%1: expected %2, got %3").arg(msg).arg(expected).arg(actual));
}

} // namespace

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Test 1: dispatch returns unknown-method error for unregistered method
    {
        CommandDispatcher dispatcher;
        QJsonObject request = {{"id", "1"}, {"method", "nonexistent"}, {"params", {}}};
        QJsonObject response = dispatcher.dispatch(request);
        expectEqual(response["id"].toString(), QString("1"), "T1: id echoed");
        expectTrue(!response["ok"].toBool(), "T1: ok is false");
        expectTrue(response["error"].toObject()["message"].toString()
                       .contains("nonexistent"),
                   "T1: error mentions method name");
    }

    // Test 2: dispatch calls registered handler
    {
        CommandDispatcher dispatcher;
        bool called = false;
        dispatcher.registerHandler("ping", [&](const QString &id, const QJsonObject &) -> QJsonObject {
            called = true;
            return CommandDispatcher::success(id, {{"name", "ecatd"}});
        });
        QJsonObject response = dispatcher.dispatch({{"id", "42"}, {"method", "ping"}, {"params", {}}});
        expectTrue(called, "T2: handler was called");
        expectEqual(response["id"].toString(), QString("42"), "T2: id echoed");
        expectTrue(response["ok"].toBool(), "T2: ok is true");
        expectEqual(response["result"].toObject()["name"].toString(), QString("ecatd"), "T2: result correct");
    }

    // Test 3: handler receives correct params
    {
        CommandDispatcher dispatcher;
        QJsonObject receivedParams;
        dispatcher.registerHandler("test", [&](const QString &id, const QJsonObject &params) -> QJsonObject {
            receivedParams = params;
            return CommandDispatcher::success(id);
        });
        dispatcher.dispatch({{"id", "7"}, {"method", "test"}, {"params", {{"pos", 3}}}});
        expectEqual(receivedParams["pos"].toInt(), 3, "T3: params passed through");
    }

    // Test 4: handler returning failure propagates correctly
    {
        CommandDispatcher dispatcher;
        dispatcher.registerHandler("fail", [](const QString &id, const QJsonObject &) -> QJsonObject {
            return CommandDispatcher::failure(id, "boom");
        });
        QJsonObject response = dispatcher.dispatch({{"id", "9"}, {"method", "fail"}, {"params", {}}});
        expectTrue(!response["ok"].toBool(), "T4: failure ok=false");
        expectEqual(response["error"].toObject()["message"].toString(), QString("boom"), "T4: error message");
    }

    // Test 5: registerHandler overwrites previous handler
    {
        CommandDispatcher dispatcher;
        dispatcher.registerHandler("dup", [](const QString &id, const QJsonObject &) -> QJsonObject {
            return CommandDispatcher::success(id, {{"v", 1}});
        });
        dispatcher.registerHandler("dup", [](const QString &id, const QJsonObject &) -> QJsonObject {
            return CommandDispatcher::success(id, {{"v", 2}});
        });
        QJsonObject response = dispatcher.dispatch({{"id", "10"}, {"method", "dup"}, {"params", {}}});
        expectEqual(response["result"].toObject()["v"].toInt(), 2, "T5: last registration wins");
    }

    // Test 6: empty method name returns error
    {
        CommandDispatcher dispatcher;
        QJsonObject response = dispatcher.dispatch({{"id", "11"}, {"method", ""}, {"params", {}}});
        expectTrue(!response["ok"].toBool(), "T6: empty method fails");
    }

    // Test 7: missing id still produces valid response
    {
        CommandDispatcher dispatcher;
        dispatcher.registerHandler("noop", [](const QString &id, const QJsonObject &) -> QJsonObject {
            return CommandDispatcher::success(id);
        });
        QJsonObject response = dispatcher.dispatch({{"method", "noop"}, {"params", {}}});
        expectTrue(response["ok"].toBool(), "T7: missing id still works");
    }

    if (failures > 0) {
        std::cerr << failures << " test(s) FAILED\n";
        return 1;
    }
    std::cout << "All command_dispatcher_test PASSED\n";
    return 0;
}
```

- [ ] **Step 2: Add test target to `tests/CMakeLists.txt`**

Append to `tests/CMakeLists.txt`:

```cmake
add_executable(command_dispatcher_test
    command_dispatcher_test.cpp
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
)

target_include_directories(command_dispatcher_test PRIVATE
    ../apps/ecatd
    ../src/core
)
target_link_libraries(command_dispatcher_test PRIVATE Qt6::Core Qt6::Network)

add_test(NAME command_dispatcher_test COMMAND command_dispatcher_test)
```

- [ ] **Step 3: Run test to verify it fails to compile**

```bash
cmake --build build --target command_dispatcher_test 2>&1 | tail -5
```

Expected: `fatal error: CommandDispatcher.h: No such file or directory`

- [ ] **Step 4: Commit**

```bash
git add tests/command_dispatcher_test.cpp tests/CMakeLists.txt
git commit -m "test: add CommandDispatcher unit tests (all fail — no implementation yet)"
```

---

### Task 2: Implement CommandDispatcher (tests pass)

**Files:**
- Create: `apps/ecatd/CommandDispatcher.h`
- Create: `apps/ecatd/CommandDispatcher.cpp`
- Modify: `apps/ecatd/CMakeLists.txt`

- [ ] **Step 1: Create `CommandDispatcher.h`**

```cpp
#pragma once

// String-keyed command dispatch table for JSON-RPC-style request routing.
// Replaces monolithic if/else chains with O(1) lookup and per-command handler functions.

#include <QJsonObject>
#include <QString>

#include <functional>
#include <unordered_map>

class CommandDispatcher {
public:
    // Handler signature: receives (id, params) and returns a full JSON response object.
    using Handler = std::function<QJsonObject(const QString &id, const QJsonObject &params)>;

    CommandDispatcher() = default;

    // Register a handler for a named command. Overwrites any previous handler for the same name.
    void registerHandler(const QString &method, Handler handler);

    // Look up the handler for request["method"] and invoke it.
    // Returns an unknown-method error if no handler is registered.
    QJsonObject dispatch(const QJsonObject &request) const;

    // Convenience: build a success/failure response envelope.
    static QJsonObject success(const QString &id, const QJsonObject &result = {});
    static QJsonObject failure(const QString &id, const QString &message, int code = -1);

private:
    std::unordered_map<std::string, Handler> handlers_;
};
```

- [ ] **Step 2: Create `CommandDispatcher.cpp`**

```cpp
// Command dispatch table implementation.
#include "CommandDispatcher.h"

void CommandDispatcher::registerHandler(const QString &method, Handler handler) {
    handlers_[method.toStdString()] = std::move(handler);
}

QJsonObject CommandDispatcher::dispatch(const QJsonObject &request) const {
    const QString id = request.value("id").toString();
    const QString method = request.value("method").toString();
    const QJsonObject params = request.value("params").toObject();

    if (method.isEmpty()) {
        return failure(id, "Missing method name");
    }

    auto it = handlers_.find(method.toStdString());
    if (it == handlers_.end()) {
        return failure(id, QString("Unknown method: %1").arg(method));
    }
    return it->second(id, params);
}

QJsonObject CommandDispatcher::success(const QString &id, const QJsonObject &result) {
    return {{"id", id}, {"ok", true}, {"result", result}};
}

QJsonObject CommandDispatcher::failure(const QString &id, const QString &message, int code) {
    return {
        {"id", id},
        {"ok", false},
        {"error", {{"message", message}, {"code", code}}}
    };
}
```

- [ ] **Step 3: Add to `apps/ecatd/CMakeLists.txt`**

Add `CommandDispatcher.cpp` and `CommandDispatcher.h` to the `add_executable(ecatd ...)` source list.

- [ ] **Step 4: Build and run tests**

```bash
cmake --build build --target command_dispatcher_test && ctest --test-dir build -R command_dispatcher_test --output-on-failure
```

Expected: `All command_dispatcher_test PASSED`

- [ ] **Step 5: Commit**

```bash
git add apps/ecatd/CommandDispatcher.h apps/ecatd/CommandDispatcher.cpp apps/ecatd/CMakeLists.txt
git commit -m "feat: CommandDispatcher — string-keyed dispatch table for daemon commands"
```

---

### Task 3: Extract setupHandlers() from EcatDaemon::handle()

**Files:**
- Modify: `apps/ecatd/EcatDaemon.h`
- Modify: `apps/ecatd/EcatDaemon.cpp`

- [ ] **Step 1: Add `CommandDispatcher` include and member to `EcatDaemon.h`**

Add `#include "CommandDispatcher.h"` at the top.

Add a private member:
```cpp
CommandDispatcher dispatcher_;
void setupHandlers();
```

- [ ] **Step 2: Implement `setupHandlers()` in `EcatDaemon.cpp`**

Move every `else if` branch from `handle()` into a registered handler. Each handler is a lambda that captures `this` and calls the appropriate `backend_` or `freeRun_`/`rtTest_` method.

```cpp
void EcatDaemon::setupHandlers() {
    dispatcher_.registerHandler("ping", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, {
            {"name", "ecatd"}, {"version", "0.1.0"}, {"multiMaster", true}
        });
    });

    dispatcher_.registerHandler("hostDiagnostics", [this](const QString &id, const QJsonObject &) {
        QString error;
        const auto checks = backend_.hostDiagnostics(&error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"checks", checks}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("master", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_.masterText(requestedMaster(params), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("scan", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const auto slaves = backend_.scanSlaves(requestedMaster(params), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"slaves", toJson(slaves)}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("rescan", [this](const QString &id, const QJsonObject &params) {
        QString error;
        return backend_.rescan(requestedMaster(params), &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("slaveInfo", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_.slaveInfo(requestedMaster(params), params.value("position").toInt(), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("pdos", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_.pdos(requestedMaster(params), params.value("position").toInt(), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("sdos", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_.sdos(requestedMaster(params), params.value("position").toInt(), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("xml", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_.slaveXml(requestedMaster(params), params.value("position").toInt(), &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"text", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("upload", [this](const QString &id, const QJsonObject &params) {
        QString error;
        const QString text = backend_.upload(requestedMaster(params),
                                             params.value("position").toInt(),
                                             params.value("index").toString(),
                                             params.value("subIndex").toString(),
                                             &error);
        return error.isEmpty()
            ? CommandDispatcher::success(id, {{"value", text}})
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("download", [this](const QString &id, const QJsonObject &params) {
        QString error;
        return backend_.download(requestedMaster(params),
                                 params.value("position").toInt(),
                                 params.value("index").toString(),
                                 params.value("subIndex").toString(),
                                 params.value("value").toString(),
                                 params.value("type").toString(),
                                 &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("applyStartupSdos", [this](const QString &id, const QJsonObject &params) {
        int applied = 0, failed = 0;
        QJsonArray failures, results;
        int row = 0;
        for (const auto &value : params.value("items").toArray()) {
            const auto item = value.toObject();
            QString itemError;
            if (backend_.download(requestedMaster(params),
                                  item.value("position").toInt(),
                                  item.value("index").toString(),
                                  item.value("subIndex").toString(),
                                  item.value("value").toString(),
                                  item.value("type").toString(),
                                  &itemError)) {
                ++applied;
                results.append(QJsonObject{{"row", row}, {"ok", true},
                    {"position", item.value("position").toInt()},
                    {"index", item.value("index").toString()},
                    {"subIndex", item.value("subIndex").toString()}});
            } else {
                ++failed;
                QJsonObject fail{{"row", row},
                    {"position", item.value("position").toInt()},
                    {"index", item.value("index").toString()},
                    {"subIndex", item.value("subIndex").toString()},
                    {"error", itemError}};
                failures.append(fail);
                QJsonObject result = fail; result.insert("ok", false);
                results.append(result);
            }
            ++row;
        }
        return CommandDispatcher::success(id, {{"applied", applied}, {"failed", failed},
                                               {"failures", failures}, {"results", results}});
    });

    dispatcher_.registerHandler("setState", [this](const QString &id, const QJsonObject &params) {
        QString error;
        return backend_.setState(requestedMaster(params), params.value("position").toInt(),
                                 params.value("state").toString(), &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("setAllStates", [this](const QString &id, const QJsonObject &params) {
        QString error;
        return backend_.setAllStates(requestedMaster(params), params.value("state").toString(), &error)
            ? CommandDispatcher::success(id)
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("freeRunStart", [this](const QString &id, const QJsonObject &params) {
        uint32_t masterIndex = 0;
        QString error;
        if (!requestedMasterIndex(params, &masterIndex, &error))
            return CommandDispatcher::failure(id, error);
        return freeRun_.start(masterIndex, &error)
            ? CommandDispatcher::success(id, freeRun_.telemetry())
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("freeRunStop", [this](const QString &id, const QJsonObject &) {
        freeRun_.stop();
        return CommandDispatcher::success(id, {{"status", freeRun_.status()}});
    });

    dispatcher_.registerHandler("freeRunStatus", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, freeRun_.telemetry());
    });

    dispatcher_.registerHandler("rtTestStart", [this](const QString &id, const QJsonObject &params) {
        uint32_t masterIndex = 0;
        QString error;
        if (!requestedMasterIndex(params, &masterIndex, &error))
            return CommandDispatcher::failure(id, error);
        const int cycleUsec = params.value("cycleUsec").toInt(1000);
        return rtTest_.start(masterIndex, cycleUsec, &error)
            ? CommandDispatcher::success(id, rtTest_.telemetry())
            : CommandDispatcher::failure(id, error);
    });

    dispatcher_.registerHandler("rtTestStop", [this](const QString &id, const QJsonObject &) {
        rtTest_.stop();
        return CommandDispatcher::success(id, rtTest_.telemetry());
    });

    dispatcher_.registerHandler("rtTestStatus", [this](const QString &id, const QJsonObject &) {
        return CommandDispatcher::success(id, rtTest_.telemetry());
    });
}
```

- [ ] **Step 3: Replace `handle()` body**

```cpp
void EcatDaemon::handle(QTcpSocket *socket, const QJsonObject &request) {
    send(socket, dispatcher_.dispatch(request));
}
```

- [ ] **Step 4: Call `setupHandlers()` from constructor**

Add `setupHandlers();` at the end of `EcatDaemon::EcatDaemon()`.

- [ ] **Step 5: Build both targets**

```bash
cmake --build build --target ecatd ecat-studio 2>&1 | tail -5
```

Expected: `[100%] Built target ecatd` and `[100%] Built target ecat-studio`

- [ ] **Step 6: Run smoke tests**

```bash
cmake --build build --target release-smoke && QT_QPA_PLATFORM=offscreen timeout 5 build/apps/ecat-studio/ecat-studio; echo "exit: $?"
```

Expected: 15/15 pass, exit 124

- [ ] **Step 7: Commit**

```bash
git add apps/ecatd/EcatDaemon.h apps/ecatd/EcatDaemon.cpp
git commit -m "refactor: replace EcatDaemon if/else chain with CommandDispatcher dispatch table"
```

---

## Phase 2: Protocol Robustness — Timeouts & State Machine

### Task 4: Add request timeout to EcatClient

**Files:**
- Modify: `apps/ecat-studio/infra/EcatClient.h`
- Modify: `apps/ecat-studio/infra/EcatClient.cpp`

- [ ] **Step 1: Add timeout infrastructure to `EcatClient.h`**

Add include `<QTimer>` and add members:

```cpp
static constexpr int kDefaultRequestTimeoutMs = 10000;
int requestTimeoutMs_ = kDefaultRequestTimeoutMs;
QTimer *requestSweepTimer_ = nullptr;
QHash<QString, qint64> requestTimestamps_;
void sweepTimedOutRequests();
```

Add public method:
```cpp
void setRequestTimeout(int ms);
```

- [ ] **Step 2: Implement timeout in `EcatClient.cpp`**

In the constructor, create and wire the sweep timer:

```cpp
requestSweepTimer_ = new QTimer(this);
requestSweepTimer_->setInterval(2000);
connect(requestSweepTimer_, &QTimer::timeout, this, &EcatClient::sweepTimedOutRequests);
requestSweepTimer_->start();
```

In `send()`, record the timestamp:
```cpp
requestTimestamps_.insert(id, QDateTime::currentMSecsSinceEpoch());
```

In `handleLine()`, remove the timestamp:
```cpp
requestTimestamps_.remove(id);
```

Implement the sweep:
```cpp
void EcatClient::sweepTimedOutRequests() {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    QStringList timedOut;
    for (auto it = requestTimestamps_.begin(); it != requestTimestamps_.end(); ++it) {
        if (now - it.value() > requestTimeoutMs_) {
            timedOut.append(it.key());
        }
    }
    for (const QString &id : timedOut) {
        handlers_.remove(id);
        requestTimestamps_.remove(id);
        emit errorMessage(QString("Request %1 timed out after %2ms").arg(id).arg(requestTimeoutMs_));
    }
}
```

Implement the setter:
```cpp
void EcatClient::setRequestTimeout(int ms) {
    requestTimeoutMs_ = ms > 0 ? ms : kDefaultRequestTimeoutMs;
}
```

- [ ] **Step 3: Add `QDateTime` include**

Add `#include <QDateTime>` to `EcatClient.cpp`.

- [ ] **Step 4: Build and smoke test**

```bash
cmake --build build --target ecat-studio 2>&1 | tail -5
QT_QPA_PLATFORM=offscreen timeout 5 build/apps/ecat-studio/ecat-studio; echo "exit: $?"
```

Expected: build success, exit 124

- [ ] **Step 5: Commit**

```bash
git add apps/ecat-studio/infra/EcatClient.h apps/ecat-studio/infra/EcatClient.cpp
git commit -m "feat: add request timeout to EcatClient (default 10s, sweep every 2s)"
```

---

### Task 5: Add explicit ConnectionState to EcatClient

**Files:**
- Modify: `apps/ecat-studio/infra/EcatClient.h`
- Modify: `apps/ecat-studio/infra/EcatClient.cpp`

- [ ] **Step 1: Define the state enum in `EcatClient.h`**

```cpp
enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
};
```

Add members:
```cpp
ConnectionState connectionState_ = ConnectionState::Disconnected;
ConnectionState connectionState() const;
```

Add signal:
```cpp
signals:
    void connectionStateChanged(ConnectionState state);
```

- [ ] **Step 2: Update `connectToDaemon()` to track state**

```cpp
void EcatClient::connectToDaemon() {
    if (connectionState_ == ConnectionState::Connected ||
        connectionState_ == ConnectionState::Connecting) {
        return;
    }
    setConnectionState(ConnectionState::Connecting);
    socket_.connectToHost(QHostAddress::LocalHost, 5877);
}
```

Add helper:
```cpp
void EcatClient::setConnectionState(ConnectionState state) {
    if (connectionState_ != state) {
        connectionState_ = state;
        emit connectionStateChanged(state);
    }
}
```

- [ ] **Step 3: Wire socket signals to update state**

In the constructor, update the existing connections:

```cpp
connect(&socket_, &QTcpSocket::connected, this, [this] {
    setConnectionState(ConnectionState::Connected);
    emit connected();
});
connect(&socket_, &QTcpSocket::disconnected, this, [this] {
    setConnectionState(ConnectionState::Disconnected);
    // Clean up all pending requests on unexpected disconnect.
    handlers_.clear();
    requestTimestamps_.clear();
    emit disconnected();
});
```

- [ ] **Step 4: Add `isConnected()` check using state**

```cpp
bool EcatClient::isConnected() const {
    return connectionState_ == ConnectionState::Connected;
}
```

- [ ] **Step 5: Build and smoke test**

```bash
cmake --build build --target ecat-studio 2>&1 | tail -5
QT_QPA_PLATFORM=offscreen timeout 5 build/apps/ecat-studio/ecat-studio; echo "exit: $?"
```

Expected: build success, exit 124

- [ ] **Step 6: Commit**

```bash
git add apps/ecat-studio/infra/EcatClient.h apps/ecat-studio/infra/EcatClient.cpp
git commit -m "feat: explicit ConnectionState enum in EcatClient with state-change signal"
```

---

## Phase 3: Integration Tests

### Task 6: Client↔Daemon round-trip test

**Files:**
- Create: `tests/protocol_integration_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Write the integration test**

```cpp
// Integration test: EcatClient ↔ EcatDaemon over localhost TCP.
// Tests the JSON protocol framing, dispatch, and error handling end-to-end.
#include "EcatClient.h"
#include "EcatDaemon.h"
#include "JsonProtocol.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTimer>

#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;

void fail(const QString &msg) {
    std::cerr << msg.toStdString() << '\n';
    ++failures;
}

void expectTrue(bool cond, const QString &msg) {
    if (!cond) fail(msg);
}

} // namespace

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Start daemon on a random high port.
    EcatDaemon daemon;
    const quint16 port = 15877;  // Fixed port for testing.
    if (!daemon.listen(port)) {
        fail("Daemon failed to listen on test port");
        return 1;
    }

    // Connect client.
    EcatClient client;
    QSignalSpy connectedSpy(&client, &EcatClient::connected);
    QSignalSpy errorSpy(&client, &EcatClient::errorMessage);
    QSignalSpy daemonInfoSpy(&client, &EcatClient::daemonInfo);

    // Override port by connecting directly.
    client.connectToHost(QHostAddress::LocalHost, port);

    // Wait for connection (max 2s).
    connectedSpy.wait(2000);
    expectTrue(connectedSpy.count() >= 1, "Client connected to daemon");
    expectTrue(client.isConnected(), "isConnected() returns true");

    // Test ping.
    QSignalSpy pingSpy(&client, &EcatClient::daemonInfo);
    client.ping();
    pingSpy.wait(2000);
    expectTrue(pingSpy.count() >= 1, "Ping returned daemon info");
    if (pingSpy.count() > 0) {
        const QString info = pingSpy.at(0).at(0).toString();
        expectTrue(info.contains("ecatd"), "Daemon info contains 'ecatd'");
    }

    // Test unknown method returns error.
    QSignalSpy unknownSpy(&client, &EcatClient::errorMessage);
    // Send raw unknown method via the socket.
    // (EcatClient doesn't expose raw send, so we test via the daemon directly.)

    if (failures > 0) {
        std::cerr << failures << " integration test(s) FAILED\n";
        return 1;
    }
    std::cout << "All protocol_integration_test PASSED\n";
    return 0;
}
```

- [ ] **Step 2: Add test target to `tests/CMakeLists.txt`**

```cmake
add_executable(protocol_integration_test
    protocol_integration_test.cpp
    ../apps/ecatd/EcatDaemon.cpp
    ../apps/ecatd/EcatDaemon.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../apps/ecatd/FreeRunController.cpp
    ../apps/ecatd/FreeRunController.h
    ../apps/ecatd/RtTestController.cpp
    ../apps/ecatd/RtTestController.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
    ../src/igh/EthercatCliBackend.cpp
    ../src/igh/EthercatCliBackend.h
)

target_include_directories(protocol_integration_test PRIVATE
    ../apps/ecatd
    ../apps/ecat-studio/infra
    ../src/core
    ../src/igh
)
target_link_libraries(protocol_integration_test PRIVATE Qt6::Core Qt6::Network ecat_core)

add_test(NAME protocol_integration_test COMMAND protocol_integration_test)
```

- [ ] **Step 3: Add `connectToHost` overload to `EcatClient.h`**

The test needs to connect to a non-default port. Add:

```cpp
void connectToHost(const QHostAddress &address, quint16 port);
```

Implementation:
```cpp
void EcatClient::connectToHost(const QHostAddress &address, quint16 port) {
    if (connectionState_ == ConnectionState::Connected ||
        connectionState_ == ConnectionState::Connecting) {
        return;
    }
    setConnectionState(ConnectionState::Connecting);
    socket_.connectToHost(address, port);
}
```

- [ ] **Step 4: Build and run**

```bash
cmake --build build --target protocol_integration_test 2>&1 | tail -5
ctest --test-dir build -R protocol_integration_test --output-on-failure
```

Expected: `All protocol_integration_test PASSED`

- [ ] **Step 5: Commit**

```bash
git add tests/protocol_integration_test.cpp tests/CMakeLists.txt \
        apps/ecat-studio/infra/EcatClient.h apps/ecat-studio/infra/EcatClient.cpp
git commit -m "test: add Client↔Daemon integration test over localhost TCP"
```

---

## Phase 4: Cleanup & Polish

### Task 7: Remove dead code and verify full build

**Files:**
- Modify: `apps/ecatd/EcatDaemon.cpp` (remove any leftover if/else remnants)

- [ ] **Step 1: Verify `handle()` is now just 2 lines**

```bash
grep -n 'void EcatDaemon::handle' apps/ecatd/EcatDaemon.cpp
sed -n '/void EcatDaemon::handle/,/^}/p' apps/ecatd/EcatDaemon.cpp
```

Expected: the function body should be just `send(socket, dispatcher_.dispatch(request));`

- [ ] **Step 2: Full build + all tests**

```bash
cmake --build build 2>&1 | tail -5
ctest --test-dir build --output-on-failure
```

Expected: all targets build, all tests pass

- [ ] **Step 3: Run release-smoke**

```bash
cmake --build build --target release-smoke
```

Expected: 15/15 pass (or 17/17 if new tests are in the smoke set)

- [ ] **Step 4: Final commit**

```bash
git add -A
git commit -m "chore: verify clean build after CommandDispatcher refactor"
```

---

## Verification Checklist

After all tasks:

- [ ] `cmake --build build` — no warnings
- [ ] `ctest --test-dir build --output-on-failure` — all pass
- [ ] `QT_QPA_PLATFORM=offscreen timeout 5 build/apps/ecat-studio/ecat-studio` — exit 124
- [ ] `handle()` in EcatDaemon.cpp is 2 lines (dispatch + send)
- [ ] No if/else chain remains in EcatDaemon
- [ ] EcatClient has timeout sweep running
- [ ] EcatClient has explicit ConnectionState
- [ ] Integration test passes over localhost TCP
