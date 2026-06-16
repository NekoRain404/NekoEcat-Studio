# NekoEcat Studio — One-Month Comprehensive Development Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Harden protocol layer, decouple the MainWindow god object, add missing test coverage, and improve i18n — producing a production-quality EtherCAT commissioning tool.

**Architecture:** Four-phase plan across 4 weeks. Each phase produces working, testable software independently. Phase 1 (protocol) and Phase 2 (architecture) are foundational. Phase 3 (features) and Phase 4 (polish) build on top.

**Tech Stack:** C++17, Qt 6 (Core, Network, Widgets), IgH ecrt, CMake, `std::exit(1)` test harness.

**Worktree:** `.worktrees/dev` on branch `dev/comprehensive-refactor`

---

## File Structure

### New Files (Phase 1)
| File | Responsibility |
|------|----------------|
| `apps/ecatd/CommandDispatcher.h` | Generic string→handler dispatch table |
| `apps/ecatd/CommandDispatcher.cpp` | Implementation |
| `apps/ecatd/EcatService.h` | Abstract backend interface (decouples CLI from daemon) |
| `apps/ecatd/EcatService.cpp` | Service registry + lifecycle |
| `tests/command_dispatcher_test.cpp` | Unit tests for CommandDispatcher |
| `tests/protocol_integration_test.cpp` | Client↔Server round-trip test |
| `tests/connection_lifecycle_test.cpp` | ConnectionState + timeout tests |

### New Files (Phase 2)
| File | Responsibility |
|------|----------------|
| `apps/ecat-studio/infra/ServiceLocator.h` | Central service registry for GUI |
| `apps/ecat-studio/infra/ServiceLocator.cpp` | Implementation |
| `apps/ecat-studio/workspaces/WorkspaceRegistry.h` | Tab→page mapping |
| `apps/ecat-studio/workspaces/WorkspaceRegistry.cpp` | Registration + lookup |

### Modified Files (Phase 1)
| File | Changes |
|------|---------|
| `apps/ecatd/EcatDaemon.h` | Add CommandDispatcher member, setupHandlers() |
| `apps/ecatd/EcatDaemon.cpp` | Replace if/else chain with dispatch |
| `apps/ecat-studio/infra/EcatClient.h` | Add timeout, ConnectionState, connectToHost overload |
| `apps/ecat-studio/infra/EcatClient.cpp` | Implement timeout sweep, state tracking |
| `apps/ecatd/CMakeLists.txt` | Add new sources |
| `tests/CMakeLists.txt` | Add new test targets |

### Modified Files (Phase 2)
| File | Changes |
|------|---------|
| `apps/ecat-studio/MainWindow.h` | Remove per-workspace members, use ServiceLocator |
| `apps/ecat-studio/MainWindow.cpp` | Reduce connect() calls, delegate to services |
| `apps/ecat-studio/infra/EcatClient.h` | Add ServiceLocator dependency |

---

## Week 1: Protocol Robustness + Testing Foundation

### Task 1: CommandDispatcher unit tests (all fail)

**Files:** Create `tests/command_dispatcher_test.cpp`, Modify `tests/CMakeLists.txt`

- [ ] Write 7 unit tests covering: unknown method, handler invocation, param passthrough, failure propagation, handler overwrite, empty method, missing id
- [ ] Add CMake target (links Qt6::Core, Qt6::Network)
- [ ] Build — confirm fails (CommandDispatcher.h doesn't exist)
- [ ] Commit: "test: add CommandDispatcher unit tests"

### Task 2: Implement CommandDispatcher (tests pass)

**Files:** Create `apps/ecatd/CommandDispatcher.h`, `apps/ecatd/CommandDispatcher.cpp`, Modify `apps/ecatd/CMakeLists.txt`

- [ ] Create header with Handler typedef, registerHandler(), dispatch(), static success()/failure()
- [ ] Implement using `std::unordered_map<std::string, Handler>` for O(1) lookup
- [ ] Build and run tests — all 7 must pass
- [ ] Commit: "feat: CommandDispatcher — string-keyed dispatch table"

### Task 3: Extract setupHandlers() from EcatDaemon

**Files:** Modify `apps/ecatd/EcatDaemon.h`, `apps/ecatd/EcatDaemon.cpp`

- [ ] Add CommandDispatcher member and setupHandlers() declaration
- [ ] Implement setupHandlers() — register all 19 commands as lambdas
- [ ] Replace handle() body with `send(socket, dispatcher_.dispatch(request))`
- [ ] Call setupHandlers() from constructor
- [ ] Build both targets, run release-smoke (15/15)
- [ ] Commit: "refactor: replace if/else chain with CommandDispatcher"

### Task 4: Add request timeout to EcatClient

**Files:** Modify `apps/ecat-studio/infra/EcatClient.h`, `apps/ecat-studio/infra/EcatClient.cpp`

- [ ] Add QTimer-based sweep (2s interval), timestamp hash, timeout constant (10s)
- [ ] Record timestamp in send(), remove in handleLine()
- [ ] Implement sweepTimedOutRequests() — evict stale handlers, emit errorMessage
- [ ] Add setRequestTimeout(int ms) public method
- [ ] Build and smoke test
- [ ] Commit: "feat: add request timeout to EcatClient"

### Task 5: Add explicit ConnectionState to EcatClient

**Files:** Modify `apps/ecat-studio/infra/EcatClient.h`, `apps/ecat-studio/infra/EcatClient.cpp`

- [ ] Define ConnectionState enum (Disconnected, Connecting, Connected, Reconnecting)
- [ ] Add connectionState() getter, connectionStateChanged signal, setConnectionState() setter
- [ ] Update connectToDaemon() and isConnected() to use explicit state
- [ ] Wire socket connected/disconnected signals to update state + clear pending handlers
- [ ] Build and smoke test
- [ ] Commit: "feat: explicit ConnectionState enum in EcatClient"

### Task 6: Client↔Server integration test

**Files:** Create `tests/protocol_integration_test.cpp`, Modify `tests/CMakeLists.txt`, Modify `apps/ecat-studio/infra/EcatClient.h`

- [ ] Add connectToHost(QHostAddress, quint16) overload to EcatClient
- [ ] Create TestServer (QTcpServer + CommandDispatcher) handling ping/echo/fail
- [ ] Test connection, ping response, daemon info format
- [ ] Add CMake target with AUTOMOC (no ethercat deps)
- [ ] Build and run — all assertions pass
- [ ] Commit: "test: add Client↔Server integration test"

### Task 7: Connection lifecycle test

**Files:** Create `tests/connection_lifecycle_test.cpp`, Modify `tests/CMakeLists.txt`

- [ ] Test: connect → state=Connected, disconnect → state=Disconnected, timeout → error emitted
- [ ] Test: double connect is no-op, connect during connecting is no-op
- [ ] Test: pending handlers cleared on disconnect
- [ ] Build and run
- [ ] Commit: "test: add connection lifecycle tests"

### Task 8: Week 1 verification

- [ ] `cmake --build build` — no warnings
- [ ] `ctest --test-dir build --output-on-failure` — all pass (should be 51+ tests)
- [ ] `QT_QPA_PLATFORM=offscreen timeout 5 build/apps/ecat-studio/ecat-studio` — exit 124
- [ ] `cmake --build build --target release-smoke` — 15/15
- [ ] Commit: "chore: Week 1 verification checkpoint"

---

## Week 2: Architecture Decoupling

### Task 9: Extract EcatService interface

**Files:** Create `apps/ecatd/EcatService.h`, `apps/ecatd/EcatService.cpp`

- [ ] Define abstract EcatService interface with pure virtual methods matching all backend operations
- [ ] Make EthercatCliBackend inherit from EcatService
- [ ] Update EcatDaemon to use EcatService* instead of EthercatCliBackend directly
- [ ] Build and test
- [ ] Commit: "refactor: extract EcatService interface from backend"

### Task 10: Add ServiceLocator to GUI

**Files:** Create `apps/ecat-studio/infra/ServiceLocator.h`, `apps/ecat-studio/infra/ServiceLocator.cpp`

- [ ] Create ServiceLocator with typed get/set for named services
- [ ] Register EcatClient as a service in MainWindow constructor
- [ ] Update workspaces to get EcatClient from ServiceLocator instead of direct member access
- [ ] Build and test
- [ ] Commit: "refactor: add ServiceLocator for GUI service registry"

### Task 11: Reduce MainWindow header size

**Files:** Modify `apps/ecat-studio/MainWindow.h`

- [ ] Move per-workspace member variables to workspace-specific structs
- [ ] Remove 50+ QLabel/QPushButton/QComboBox pointers that can be found via findChild
- [ ] Extract workspace-specific logic into workspace classes (not just partial .cpp files)
- [ ] Build and test
- [ ] Commit: "refactor: reduce MainWindow header from 815 to ~500 lines"

### Task 12: Week 2 verification

- [ ] Full build + all tests pass
- [ ] Smoke test
- [ ] Commit: "chore: Week 2 verification checkpoint"

---

## Week 3: Feature Gaps + i18n

### Task 13: SDO auto-fill on object index selection

**Files:** Modify `apps/ecat-studio/workspaces/MainWindowSdoWorkspace.cpp`

- [ ] When user selects an object index in the dictionary table, auto-fill sub-index, type, and value fields
- [ ] Add unit test for the auto-fill logic
- [ ] Build and test
- [ ] Commit: "feat: auto-fill SDO fields on object index selection"

### Task 14: Project schema versioning

**Files:** Modify `apps/ecat-studio/workspaces/MainWindowProjectIo.cpp`

- [ ] Add schema version field to project file format
- [ ] Handle migration for old project files (version < current)
- [ ] Add unit test for version detection and migration
- [ ] Build and test
- [ ] Commit: "feat: project schema versioning with migration"

### Task 15: Extract i18n strings to .ts file

**Files:** Create `i18n/nekoecat_zh_CN.ts`

- [ ] Run `lupdate` to extract all uiText() calls
- [ ] Create Chinese translation file with initial translations
- [ ] Add QTranslator loading to main.cpp
- [ ] Build and test
- [ ] Commit: "feat: extract i18n strings to Qt .ts translation file"

### Task 16: Week 3 verification

- [ ] Full build + all tests pass
- [ ] Smoke test
- [ ] Commit: "chore: Week 3 verification checkpoint"

---

## Week 4: Polish + Final Testing + Code Review

### Task 17: Chart Y-axis snap + right-align

**Files:** Modify `apps/ecat-studio/workspaces/MainWindowRtTestWorkspace.cpp`

- [ ] Snap Y-axis to data range on first update (don't blend from [0,1000])
- [ ] Right-align chart data so it always looks populated
- [ ] Build and test
- [ ] Commit: "fix: chart Y-axis snap + right-align data"

### Task 18: Add comprehensive inline comments

**Files:** All `.cpp` and `.h` files

- [ ] Add file-level purpose comments to every source file
- [ ] Add function-level comments to public APIs
- [ ] Add inline comments for non-obvious logic
- [ ] Commit: "docs: add comprehensive inline comments"

### Task 19: Final test suite run

- [ ] Build all targets clean
- [ ] Run full test suite — all must pass
- [ ] Run release-smoke — 15/15
- [ ] Run GUI smoke test — exit 124
- [ ] Commit: "chore: final verification checkpoint"

### Task 20: Code review preparation

- [ ] Generate diff stats for the entire branch
- [ ] Verify no regressions in existing tests
- [ ] Check for TODO/FIXME/HACK comments
- [ ] Commit: "chore: code review preparation"

---

## Verification Checklist

After all tasks:

- [ ] `cmake --build build` — clean, no warnings
- [ ] `ctest --test-dir build --output-on-failure` — all pass (51+ tests)
- [ ] `QT_QPA_PLATFORM=offscreen timeout 5 build/apps/ecat-studio/ecat-studio` — exit 124
- [ ] `cmake --build build --target release-smoke` — 15/15
- [ ] `handle()` in EcatDaemon.cpp is 1 line (dispatch + send)
- [ ] EcatClient has timeout sweep running
- [ ] EcatClient has explicit ConnectionState
- [ ] Integration test passes over localhost TCP
- [ ] MainWindow.h reduced from 815 lines
- [ ] All files have inline comments
