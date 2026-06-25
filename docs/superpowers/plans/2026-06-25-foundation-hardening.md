# Foundation Hardening Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make release metadata, CI gates, daemon validation, command execution boundaries, and experimental feature defaults trustworthy without changing normal user workflows.

**Architecture:** Keep the existing Qt/CMake structure and add small focused seams: a generated version header, daemon request validation helpers, and argument-based process execution for adapter probing. CI remains GitHub Actions based but soft quality checks become real gates.

**Tech Stack:** CMake 3.20, C++20, Qt6 Core/Network/Widgets/Test, Bash packaging scripts, GitHub Actions.

---

## File Structure

- Modify `CMakeLists.txt`: configure version header and keep project version as source of truth.
- Create `src/core/Version.h.in`: template for generated `NEKOECAT_VERSION`.
- Modify `src/core/CMakeLists.txt`: expose generated include directory through `ecat_core`.
- Modify `apps/ecat-studio/main.cpp`: use generated version.
- Modify `apps/ecatd/main.cpp`: use generated version.
- Modify `apps/ecatd/EcatDaemon.cpp`: use generated version and validation helpers before backend calls.
- Create `apps/ecatd/RequestValidation.h`: field validation API.
- Create `apps/ecatd/RequestValidation.cpp`: validation implementation.
- Modify `apps/ecatd/CMakeLists.txt`: add validation files.
- Create `tests/request_validation_test.cpp`: unit coverage for validation.
- Modify `tests/daemon_handler_test.cpp`: stop expecting hardcoded `0.1.0`.
- Modify `tests/CMakeLists.txt`: register validation/version tests.
- Modify `apps/ecatd/handlers/AdapterHandler.cpp`: remove `sh -c`, capture stderr, keep timeout.
- Modify `tests/adapter_handler_test.cpp`: add a stable test for command argument execution behavior if helper visibility allows; otherwise cover through list structure and no shell dependency with static source test.
- Modify `.github/workflows/ci.yml`: make smoke/static-analysis/valgrind checks meaningful.
- Modify `scripts/package-source.sh`, `scripts/package-linux.sh`, `scripts/package-appimage.sh`, `scripts/package-deb.sh`, `scripts/package-rpm.sh`: derive default version from CMake.
- Modify `apps/ecat-studio/CMakeLists.txt`: default `ECAT_EXPERIMENTAL_SERVICES` off.
- Modify `README.md`: mark experimental AI/Blockchain/Quantum-style surfaces as experimental when disabled by default.

---

## Task 1: Single-Source Version

**Files:**
- Create: `src/core/Version.h.in`
- Modify: `CMakeLists.txt`
- Modify: `src/core/CMakeLists.txt`
- Modify: `apps/ecat-studio/main.cpp`
- Modify: `apps/ecatd/main.cpp`
- Modify: `apps/ecatd/EcatDaemon.cpp`
- Modify: `tests/daemon_handler_test.cpp`

- [ ] **Step 1: Add generated version template**

Create `src/core/Version.h.in`:

```cpp
#pragma once

#define NEKOECAT_VERSION "@PROJECT_VERSION@"
```

- [ ] **Step 2: Configure the generated header**

In top-level `CMakeLists.txt`, after the `project(...)` line, add:

```cmake
configure_file(
    ${CMAKE_SOURCE_DIR}/src/core/Version.h.in
    ${CMAKE_BINARY_DIR}/generated/Version.h
    @ONLY
)
```

- [ ] **Step 3: Expose the generated include directory**

In `src/core/CMakeLists.txt`, add the generated include directory to `ecat_core`. If the file already has `target_include_directories(ecat_core ...)`, extend it:

```cmake
target_include_directories(ecat_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/generated
)
```

- [ ] **Step 4: Use the generated version in GUI**

In `apps/ecat-studio/main.cpp`, include `Version.h` and replace the hardcoded version:

```cpp
#include "Version.h"
```

```cpp
QApplication::setApplicationVersion(NEKOECAT_VERSION);
```

- [ ] **Step 5: Use the generated version in daemon main**

In `apps/ecatd/main.cpp`, include `Version.h` and replace the hardcoded version:

```cpp
#include "Version.h"
```

```cpp
QCoreApplication::setApplicationVersion(NEKOECAT_VERSION);
```

- [ ] **Step 6: Use the generated version in daemon ping**

In `apps/ecatd/EcatDaemon.cpp`, include `Version.h` and replace:

```cpp
{"version", "0.1.0"},
```

with:

```cpp
{"version", NEKOECAT_VERSION},
```

- [ ] **Step 7: Update daemon version tests**

In `tests/daemon_handler_test.cpp`, include `Version.h` and change local ping fixtures from `"0.1.0"` to `NEKOECAT_VERSION`. Example:

```cpp
#include "Version.h"
```

```cpp
{"version", NEKOECAT_VERSION},
```

```cpp
QCOMPARE(result["version"].toString(), QString(NEKOECAT_VERSION));
```

- [ ] **Step 8: Reconfigure and build**

Run:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Expected: build completes with generated `build/generated/Version.h`.

- [ ] **Step 9: Run focused tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R "daemon_handler_test|command_dispatcher_test"
```

Expected: all selected tests pass.

- [ ] **Step 10: Commit**

```bash
git add CMakeLists.txt src/core/Version.h.in src/core/CMakeLists.txt apps/ecat-studio/main.cpp apps/ecatd/main.cpp apps/ecatd/EcatDaemon.cpp tests/daemon_handler_test.cpp
git commit --no-gpg-sign -m "chore: use project version across binaries"
```

---

## Task 2: Packaging Version Defaults

**Files:**
- Modify: `scripts/package-source.sh`
- Modify: `scripts/package-linux.sh`
- Modify: `scripts/package-appimage.sh`
- Modify: `scripts/package-deb.sh`
- Modify: `scripts/package-rpm.sh`

- [ ] **Step 1: Add shared inline version extraction to each packaging script**

In each listed script, replace hardcoded default assignments such as `VERSION="${1:-1.2.0}"` or `VERSION="${1:-0.1.0}"` with:

```bash
cmake_project_version() {
  sed -nE 's/^project\(NekoEcatStudio VERSION ([^ ]+) LANGUAGES CXX\)$/\1/p' "${ROOT_DIR}/CMakeLists.txt"
}

VERSION="${1:-$(cmake_project_version)}"
if [[ -z "${VERSION}" ]]; then
  echo "Unable to determine project version from CMakeLists.txt" >&2
  exit 1
fi
```

Place the function after `ROOT_DIR=...` so `ROOT_DIR` is available.

- [ ] **Step 2: Fix generated binary package release note literals**

In `scripts/package-linux.sh`, replace hardcoded `v0.1.0` text in generated release notes with interpolated shell output by changing the heredoc delimiter from quoted to unquoted:

```bash
cat >"${PACKAGE_DIR}/RELEASE_NOTES.md" <<NOTES
# NekoEcat Studio v${VERSION}
...
tar -xzf NekoEcat-Studio-v${VERSION}-linux-x86_64.tar.gz
cd NekoEcat-Studio-v${VERSION}-linux-x86_64
...
NOTES
```

Keep the rest of the release note content unchanged unless it claims first release status that conflicts with the current version.

- [ ] **Step 3: Run shell syntax checks**

Run:

```bash
bash -n scripts/package-source.sh scripts/package-linux.sh scripts/package-appimage.sh scripts/package-deb.sh scripts/package-rpm.sh
```

Expected: no output and exit code 0.

- [ ] **Step 4: Verify default version extraction**

Run:

```bash
for s in scripts/package-source.sh scripts/package-linux.sh scripts/package-appimage.sh scripts/package-deb.sh scripts/package-rpm.sh; do grep -n 'VERSION="${1:-$(cmake_project_version)}"' "$s"; done
```

Expected: one matching line per script.

- [ ] **Step 5: Commit**

```bash
git add scripts/package-source.sh scripts/package-linux.sh scripts/package-appimage.sh scripts/package-deb.sh scripts/package-rpm.sh
git commit --no-gpg-sign -m "fix: derive package versions from cmake"
```

---

## Task 3: Daemon Request Validation Helper

**Files:**
- Create: `apps/ecatd/RequestValidation.h`
- Create: `apps/ecatd/RequestValidation.cpp`
- Modify: `apps/ecatd/CMakeLists.txt`
- Create: `tests/request_validation_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Add validation header**

Create `apps/ecatd/RequestValidation.h`:

```cpp
#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>

namespace RequestValidation {

struct Result {
    bool ok = false;
    QString error;
    QJsonObject normalized;
};

Result master(const QJsonObject &params);
Result position(const QJsonObject &params);
Result sdoTarget(const QJsonObject &params, bool requireValue);
Result alState(const QJsonObject &params);
Result startupSdoItems(const QJsonObject &params);

bool isSupportedType(const QString &type);
QString normalizeIntegerString(const QString &value, int min, int max, const QString &field, bool *ok, QString *error);

} // namespace RequestValidation
```

- [ ] **Step 2: Add validation implementation**

Create `apps/ecatd/RequestValidation.cpp`:

```cpp
#include "RequestValidation.h"

#include <QJsonValue>
#include <QRegularExpression>

namespace {

QString valueToString(const QJsonValue &value)
{
    if (value.isString()) return value.toString().trimmed();
    if (value.isDouble()) return QString::number(value.toInt());
    return {};
}

bool parseIntLike(const QJsonValue &value, int min, int max, const QString &field, int *out, QString *error)
{
    bool ok = false;
    int parsed = 0;
    if (value.isDouble()) {
        const double raw = value.toDouble();
        parsed = value.toInt();
        ok = (raw == parsed);
    } else if (value.isString()) {
        QString text = value.toString().trimmed();
        int base = 10;
        if (text.startsWith("0x", Qt::CaseInsensitive)) {
            text = text.mid(2);
            base = 16;
        }
        parsed = text.toInt(&ok, base);
    }
    if (!ok || parsed < min || parsed > max) {
        if (error) {
            *error = QString("Invalid %1: expected integer in range %2..%3").arg(field).arg(min).arg(max);
        }
        return false;
    }
    if (out) *out = parsed;
    return true;
}

QString canonicalHex(int value, int width)
{
    return QString("0x%1").arg(value, width, 16, QChar('0')).toUpper();
}

} // namespace

namespace RequestValidation {

bool isSupportedType(const QString &type)
{
    if (type.trimmed().isEmpty()) return true;
    static const QStringList types = {
        "bool", "bit1", "bit2", "bit3", "bit4", "bit5", "bit6", "bit7", "bit8",
        "int8", "int16", "int32", "int64", "s8", "s16", "s32", "s64",
        "uint8", "uint16", "uint32", "uint64", "u8", "u16", "u32", "u64",
        "float", "double", "string", "visible_string", "octet_string"
    };
    return types.contains(type.trimmed().toLower());
}

QString normalizeIntegerString(const QString &value, int min, int max, const QString &field, bool *ok, QString *error)
{
    int parsed = 0;
    const bool valid = parseIntLike(QJsonValue(value), min, max, field, &parsed, error);
    if (ok) *ok = valid;
    return valid ? QString::number(parsed) : QString();
}

Result master(const QJsonObject &params)
{
    Result result;
    QString masterValue = params.value("master").toString("0").trimmed();
    if (masterValue.isEmpty()) masterValue = "0";
    bool ok = false;
    QString error;
    normalizeIntegerString(masterValue, 0, 65535, "master", &ok, &error);
    if (!ok) {
        result.error = error;
        return result;
    }
    result.ok = true;
    result.normalized.insert("master", masterValue);
    return result;
}

Result position(const QJsonObject &params)
{
    Result result = master(params);
    if (!result.ok) return result;
    if (!params.contains("position")) {
        result.ok = false;
        result.error = "Missing required field: position";
        return result;
    }
    int parsed = 0;
    if (!parseIntLike(params.value("position"), 0, 65535, "position", &parsed, &result.error)) {
        result.ok = false;
        return result;
    }
    result.normalized.insert("position", parsed);
    return result;
}

Result sdoTarget(const QJsonObject &params, bool requireValue)
{
    Result result = position(params);
    if (!result.ok) return result;
    if (!params.contains("index")) {
        result.ok = false;
        result.error = "Missing required field: index";
        return result;
    }
    if (!params.contains("subIndex")) {
        result.ok = false;
        result.error = "Missing required field: subIndex";
        return result;
    }
    int index = 0;
    int subIndex = 0;
    if (!parseIntLike(params.value("index"), 0, 0xFFFF, "index", &index, &result.error)) {
        result.ok = false;
        return result;
    }
    if (!parseIntLike(params.value("subIndex"), 0, 0xFF, "subIndex", &subIndex, &result.error)) {
        result.ok = false;
        return result;
    }
    const QString type = params.value("type").toString().trimmed();
    if (!isSupportedType(type)) {
        result.ok = false;
        result.error = QString("Invalid type: %1").arg(type);
        return result;
    }
    if (requireValue && !params.contains("value")) {
        result.ok = false;
        result.error = "Missing required field: value";
        return result;
    }
    result.ok = true;
    result.normalized.insert("index", canonicalHex(index, 4));
    result.normalized.insert("subIndex", canonicalHex(subIndex, 2));
    result.normalized.insert("type", type);
    if (params.contains("value")) result.normalized.insert("value", valueToString(params.value("value")));
    return result;
}

Result alState(const QJsonObject &params)
{
    Result result = position(params);
    if (!result.ok) return result;
    const QString state = params.value("state").toString().trimmed().toUpper();
    static const QStringList states = {"INIT", "PREOP", "SAFEOP", "OP"};
    if (!states.contains(state)) {
        result.ok = false;
        result.error = "Invalid state: expected one of INIT, PREOP, SAFEOP, OP";
        return result;
    }
    result.ok = true;
    result.normalized.insert("state", state);
    return result;
}

Result startupSdoItems(const QJsonObject &params)
{
    Result result = master(params);
    if (!result.ok) return result;
    if (!params.value("items").isArray()) {
        result.ok = false;
        result.error = "Missing required field: items";
        return result;
    }
    QJsonArray normalizedItems;
    QJsonArray failures;
    int row = 0;
    for (const auto &entry : params.value("items").toArray()) {
        Result item = sdoTarget(entry.toObject(), true);
        if (item.ok) {
            QJsonObject normalized = item.normalized;
            normalized.insert("row", row);
            normalizedItems.append(normalized);
        } else {
            failures.append(QJsonObject{{"row", row}, {"error", item.error}});
        }
        ++row;
    }
    result.ok = true;
    result.normalized.insert("items", normalizedItems);
    result.normalized.insert("validationFailures", failures);
    return result;
}

} // namespace RequestValidation
```

- [ ] **Step 3: Add validation files to ecatd target**

In `apps/ecatd/CMakeLists.txt`, add:

```cmake
    RequestValidation.cpp
    RequestValidation.h
```

inside `add_executable(ecatd ...)`.

- [ ] **Step 4: Add validation tests**

Create `tests/request_validation_test.cpp`:

```cpp
#include "../apps/ecatd/RequestValidation.h"

#include <QTest>

class RequestValidationTest : public QObject {
    Q_OBJECT

private slots:
    void rejectsMissingPositionForSdo() {
        auto result = RequestValidation::sdoTarget({{"index", "0x6000"}, {"subIndex", "0x01"}}, false);
        QVERIFY(!result.ok);
        QVERIFY(result.error.contains("position"));
    }

    void normalizesSdoTarget() {
        auto result = RequestValidation::sdoTarget({{"position", 2}, {"index", "6000"}, {"subIndex", "1"}, {"type", "UINT16"}}, false);
        QVERIFY(result.ok);
        QCOMPARE(result.normalized.value("position").toInt(), 2);
        QCOMPARE(result.normalized.value("index").toString(), QString("0X6000"));
        QCOMPARE(result.normalized.value("subIndex").toString(), QString("0X01"));
    }

    void rejectsUnsupportedType() {
        auto result = RequestValidation::sdoTarget({{"position", 2}, {"index", "0x6000"}, {"subIndex", "0x01"}, {"type", "badtype"}}, false);
        QVERIFY(!result.ok);
        QVERIFY(result.error.contains("Invalid type"));
    }

    void validatesAlState() {
        auto result = RequestValidation::alState({{"position", 0}, {"state", "op"}});
        QVERIFY(result.ok);
        QCOMPARE(result.normalized.value("state").toString(), QString("OP"));
    }

    void reportsStartupRowFailures() {
        QJsonArray items;
        items.append(QJsonObject{{"position", 0}, {"index", "0x6000"}, {"subIndex", "0x01"}, {"value", "1"}, {"type", "uint8"}});
        items.append(QJsonObject{{"position", -1}, {"index", "0x6000"}, {"subIndex", "0x01"}, {"value", "1"}});
        auto result = RequestValidation::startupSdoItems({{"items", items}});
        QVERIFY(result.ok);
        QCOMPARE(result.normalized.value("items").toArray().size(), 1);
        QCOMPARE(result.normalized.value("validationFailures").toArray().size(), 1);
    }
};

QTEST_MAIN(RequestValidationTest)
#include "request_validation_test.moc"
```

- [ ] **Step 5: Register validation test**

In `tests/CMakeLists.txt`, add a target near other daemon tests:

```cmake
add_executable(request_validation_test
    request_validation_test.cpp
    ../apps/ecatd/RequestValidation.cpp
    ../apps/ecatd/RequestValidation.h
)
target_include_directories(request_validation_test PRIVATE
    ../apps/ecatd
)
target_link_libraries(request_validation_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(request_validation_test PROPERTIES AUTOMOC ON)
add_test(NAME request_validation_test COMMAND request_validation_test)
```

- [ ] **Step 6: Build and run validation tests**

Run:

```bash
cmake --build build --target request_validation_test -j$(nproc)
ctest --test-dir build --output-on-failure -R request_validation_test
```

Expected: `request_validation_test` passes.

- [ ] **Step 7: Commit**

```bash
git add apps/ecatd/RequestValidation.h apps/ecatd/RequestValidation.cpp apps/ecatd/CMakeLists.txt tests/request_validation_test.cpp tests/CMakeLists.txt
git commit --no-gpg-sign -m "feat: add daemon request validation helpers"
```

---

## Task 4: Apply Validation to High-Risk Daemon Commands

**Files:**
- Modify: `apps/ecatd/EcatDaemon.cpp`
- Modify: `tests/daemon_handler_test.cpp` or create focused dispatcher tests if easier

- [ ] **Step 1: Include validation helper**

In `apps/ecatd/EcatDaemon.cpp`, add:

```cpp
#include "RequestValidation.h"
```

- [ ] **Step 2: Validate `slaveInfo`, `pdos`, `sdos`, and `xml`**

For each handler, replace direct `params.value("position").toInt()` use with:

```cpp
const auto validated = RequestValidation::position(params);
if (!validated.ok) {
    return CommandDispatcher::failure(id, validated.error);
}
const auto normalized = validated.normalized;
```

Then use:

```cpp
normalized.value("position").toInt()
```

and:

```cpp
normalized.value("master").toString()
```

- [ ] **Step 3: Validate `upload`**

Replace the body of the `upload` handler with:

```cpp
const auto validated = RequestValidation::sdoTarget(params, false);
if (!validated.ok) {
    return CommandDispatcher::failure(id, validated.error);
}
const auto normalized = validated.normalized;
QString error;
const QString text = backend_->upload(normalized.value("master").toString(),
                                      normalized.value("position").toInt(),
                                      normalized.value("index").toString(),
                                      normalized.value("subIndex").toString(),
                                      normalized.value("type").toString(),
                                      &error);
return error.isEmpty()
    ? CommandDispatcher::success(id, {{"value", text}})
    : CommandDispatcher::failure(id, error);
```

- [ ] **Step 4: Validate `download`**

Replace the body of the `download` handler with:

```cpp
const auto validated = RequestValidation::sdoTarget(params, true);
if (!validated.ok) {
    return CommandDispatcher::failure(id, validated.error);
}
const auto normalized = validated.normalized;
QString error;
return backend_->download(normalized.value("master").toString(),
                          normalized.value("position").toInt(),
                          normalized.value("index").toString(),
                          normalized.value("subIndex").toString(),
                          normalized.value("value").toString(),
                          normalized.value("type").toString(),
                          &error)
    ? CommandDispatcher::success(id)
    : CommandDispatcher::failure(id, error);
```

- [ ] **Step 5: Validate `applyStartupSdos` row by row**

At the start of the handler:

```cpp
const auto validated = RequestValidation::startupSdoItems(params);
if (!validated.ok) {
    return CommandDispatcher::failure(id, validated.error);
}
const QString master = validated.normalized.value("master").toString();
const auto items = validated.normalized.value("items").toArray();
QJsonArray failures = validated.normalized.value("validationFailures").toArray();
```

Iterate over `items`, use normalized fields, and initialize `failed` with `failures.size()`.

- [ ] **Step 6: Validate state handlers**

For `setState`, use `RequestValidation::alState(params)`.

For `setAllStates`, validate only the `state` field by passing `position: 0` into a copied object:

```cpp
QJsonObject scoped = params;
scoped.insert("position", 0);
const auto validated = RequestValidation::alState(scoped);
if (!validated.ok) {
    return CommandDispatcher::failure(id, validated.error);
}
```

Then call backend with `validated.normalized.value("state").toString()`.

- [ ] **Step 7: Add daemon validation behavior tests**

In `tests/daemon_handler_test.cpp`, add a test using `CommandDispatcher` and `RequestValidation::sdoTarget` in a registered fake upload handler:

```cpp
void testUploadValidationRejectsMissingPosition() {
    CommandDispatcher d;
    bool backendCalled = false;
    d.registerHandler("upload", [&](const QString &id, const QJsonObject &params) {
        const auto validated = RequestValidation::sdoTarget(params, false);
        if (!validated.ok) return CommandDispatcher::failure(id, validated.error);
        backendCalled = true;
        return CommandDispatcher::success(id);
    });

    QJsonObject resp = d.dispatch({{"id", "v1"}, {"method", "upload"}, {"params", QJsonObject{{"index", "0x6000"}, {"subIndex", "0x01"}}}});
    QVERIFY(!resp.value("ok").toBool());
    QVERIFY(!backendCalled);
}
```

Add `#include "RequestValidation.h"` and update the test target sources in `tests/CMakeLists.txt` if needed.

- [ ] **Step 8: Build and run daemon tests**

Run:

```bash
cmake --build build --target ecatd daemon_handler_test request_validation_test -j$(nproc)
ctest --test-dir build --output-on-failure -R "daemon_handler_test|request_validation_test"
```

Expected: all selected tests pass.

- [ ] **Step 9: Commit**

```bash
git add apps/ecatd/EcatDaemon.cpp tests/daemon_handler_test.cpp tests/CMakeLists.txt
git commit --no-gpg-sign -m "fix: validate high-risk daemon requests"
```

---

## Task 5: Adapter Command Boundary Cleanup

**Files:**
- Modify: `apps/ecatd/handlers/AdapterHandler.cpp`
- Modify: `apps/ecatd/handlers/AdapterHandler.h`
- Modify: `tests/adapter_handler_test.cpp`

- [ ] **Step 1: Change helper signature**

In `AdapterHandler.h`, replace:

```cpp
QString runCommand(const QString &cmd) const;
```

with:

```cpp
QString runCommand(const QString &program, const QStringList &arguments, QString *error = nullptr) const;
```

- [ ] **Step 2: Use argument-based QProcess**

In `AdapterHandler.cpp`, replace:

```cpp
const QString ethtoolOut = runCommand(QStringLiteral("ethtool %1").arg(iface));
```

with:

```cpp
QString ethtoolError;
const QString ethtoolOut = runCommand(QStringLiteral("ethtool"), {iface}, &ethtoolError);
```

Replace the helper implementation with:

```cpp
QString AdapterHandler::runCommand(const QString &program, const QStringList &arguments, QString *error) const
{
  QProcess proc;
  proc.setProgram(program);
  proc.setArguments(arguments);
  proc.start();
  if (!proc.waitForStarted(1000)) {
    if (error) *error = QStringLiteral("Failed to start %1: %2").arg(program, proc.errorString());
    return {};
  }
  if (!proc.waitForFinished(5000)) {
    proc.kill();
    proc.waitForFinished(1000);
    if (error) *error = QStringLiteral("%1 timed out").arg(program);
    return {};
  }
  if (proc.exitCode() != 0 && error) {
    *error = QString::fromUtf8(proc.readAllStandardError()).trimmed();
  }
  return QString::fromUtf8(proc.readAllStandardOutput());
}
```

- [ ] **Step 3: Add a helper test**

If `runCommand` is private, make it protected and add a small test subclass in `tests/adapter_handler_test.cpp`:

```cpp
class TestableAdapterHandler : public AdapterHandler {
public:
  using AdapterHandler::runCommand;
};
```

Add:

```cpp
void testRunCommandUsesArguments() {
  TestableAdapterHandler handler;
  QString error;
  const QString output = handler.runCommand("printf", {"hello"}, &error);
  QCOMPARE(output, QString("hello"));
  QVERIFY(error.isEmpty());
}
```

- [ ] **Step 4: Build and run adapter tests**

Run:

```bash
cmake --build build --target adapter_handler_test -j$(nproc)
ctest --test-dir build --output-on-failure -R adapter_handler_test
```

Expected: adapter tests pass.

- [ ] **Step 5: Commit**

```bash
git add apps/ecatd/handlers/AdapterHandler.h apps/ecatd/handlers/AdapterHandler.cpp tests/adapter_handler_test.cpp
git commit --no-gpg-sign -m "fix: remove shell from adapter probing"
```

---

## Task 6: CI Gates Tightening

**Files:**
- Modify: `.github/workflows/ci.yml`

- [ ] **Step 1: Make GUI smoke test distinguish timeout from crash**

Replace the GUI smoke step with:

```yaml
      - name: GUI Smoke Test
        run: |
          if [ -f build/apps/ecat-studio/ecat-studio ]; then
            set +e
            QT_QPA_PLATFORM=offscreen timeout 10 build/apps/ecat-studio/ecat-studio
            status=$?
            set -e
            if [ "$status" -eq 124 ]; then
              echo "GUI stayed alive for smoke window"
            elif [ "$status" -eq 0 ]; then
              echo "GUI exited cleanly during smoke test"
            else
              echo "::error::GUI smoke test failed with status $status"
              exit "$status"
            fi
          fi
```

- [ ] **Step 2: Make clang-tidy fail on selected files**

Replace the clang-tidy step with:

```yaml
      - name: clang-tidy
        run: |
          find src apps/ecatd apps/ecat-studio/infra apps/ecat-studio/services \
            \( -name '*.cpp' -o -name '*.h' \) -print0 | \
            xargs -0 -r clang-tidy -p build --quiet
```

- [ ] **Step 3: Make cppcheck fail on selected severities**

Change `--error-exitcode=0` to:

```yaml
            --error-exitcode=1 \
```

Keep `--suppress=missingIncludeSystem`.

- [ ] **Step 4: Make Valgrind gate focused tests**

Replace the Valgrind ctest command with a focused regex and no warning swallow:

```yaml
      - name: Valgrind focused tests
        run: |
          ctest --test-dir build --output-on-failure -j1 \
            -R "json_protocol_test|command_dispatcher_test|request_validation_test" \
            --test-action test
```

If the project requires explicit Valgrind wrapping, use:

```yaml
          ctest --test-dir build --output-on-failure -j1 \
            -R "json_protocol_test|command_dispatcher_test|request_validation_test" \
            -T memcheck
```

Use the form that works with the generated build tree.

- [ ] **Step 5: Validate workflow syntax locally as far as possible**

Run:

```bash
python3 - <<'PY'
from pathlib import Path
p = Path('.github/workflows/ci.yml')
text = p.read_text()
assert '|| true' not in text[text.index('GUI Smoke Test'):text.index('static-analysis:')]
assert '--error-exitcode=1' in text
assert 'head -50' not in text
print('ci semantic checks passed')
PY
```

Expected: `ci semantic checks passed`.

- [ ] **Step 6: Commit**

```bash
git add .github/workflows/ci.yml
git commit --no-gpg-sign -m "ci: make quality gates actionable"
```

---

## Task 7: Experimental Feature Default and Documentation

**Files:**
- Modify: `apps/ecat-studio/CMakeLists.txt`
- Modify: `README.md`

- [ ] **Step 1: Default experimental services off**

In `apps/ecat-studio/CMakeLists.txt`, change:

```cmake
option(ECAT_EXPERIMENTAL_SERVICES "Include experimental/stub services (blockchain, quantum, AI, etc.)" ON)
```

to:

```cmake
option(ECAT_EXPERIMENTAL_SERVICES "Include experimental/stub services (blockchain, quantum, AI, etc.)" OFF)
```

- [ ] **Step 2: Add README experimental note**

In `README.md`, near the workspace/product map section that lists AI, Blockchain, Quantum, Cloud, Edge, and Digital Twin features, add:

```markdown
> Experimental note: AI, Blockchain, Quantum Security, Cloud, Edge, and Digital Twin surfaces are experimental/stub-backed unless the project is built with `-DECAT_EXPERIMENTAL_SERVICES=ON`. They are not part of the stable EtherCAT commissioning path.
```

- [ ] **Step 3: Reconfigure default build**

Run:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

Expected output includes `Experimental services: DISABLED`.

- [ ] **Step 4: Build and test**

Run:

```bash
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure -R "plugin_registry_test|workspace_plugin_interface_test|daemon_handler_test|request_validation_test"
```

Expected: build succeeds and selected tests pass.

- [ ] **Step 5: Commit**

```bash
git add apps/ecat-studio/CMakeLists.txt README.md
git commit --no-gpg-sign -m "chore: default experimental services off"
```

---

## Task 8: Full Verification

**Files:**
- No code files expected unless verification exposes failures.

- [ ] **Step 1: Clean configure**

Run:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

Expected: configure completes.

- [ ] **Step 2: Full build**

Run:

```bash
cmake --build build -j$(nproc)
```

Expected: build completes.

- [ ] **Step 3: Full test suite**

Run:

```bash
ctest --test-dir build --output-on-failure -j$(nproc)
```

Expected: 100% tests passed.

- [ ] **Step 4: Git status review**

Run:

```bash
git status --short
git log --oneline -8
```

Expected: only intentional committed changes remain, or working tree is clean.

- [ ] **Step 5: Final commit if verification fixes were needed**

Only if Step 1-4 required fixes:

```bash
git status --short
git add CMakeLists.txt src/core apps/ecatd apps/ecat-studio tests scripts .github README.md
git commit --no-gpg-sign -m "fix: address foundation hardening verification"
```

---

## Self-Review

Spec coverage:

- Version single-source: Tasks 1 and 2.
- CI quality gates: Task 6.
- Daemon request validation: Tasks 3 and 4.
- External command boundary: Task 5.
- Experimental feature default/docs: Task 7.
- Full build/test verification: Task 8.

No placeholders remain. The plan intentionally does not implement async daemon jobs or real experimental services because those are out of scope for this hardening phase.
