# Changelog

## [Real-Time SHM Hardening + Fix Round] — 2026-08-29

Covers the 11 commits after `4fcba7a`: SHM data-plane hardening, GUI fixes, build/test/CI/script
fixes, three CI-compatibility rounds, new tests, residual fixes, and the final data-race fix.

### Added

- **Real-time SHM data plane (408fd06)**: `ecatd` now mirrors the Free Run process image into
  POSIX shared memory via a lock-free double-buffer protocol. The canonical, Qt-free ABI
  (`ShmHeader`, mirror/layout entry structs, atomic helpers, compile-time layout assertions) lives
  in `src/core/nekoecat_shm.h`, shared verbatim between the C++20 daemon and the pure-C client.
  The daemon-side mirror (`apps/ecatd/freerun_shm_mirror.cpp`) publishes with a strict atomic
  order (timestamp/status/cycle_count relaxed → `active_buffer` release → `version` release), and
  overlays validated client RxPDO writes back into the domain data (last-write-wins); out-of-bounds
  writes are ignored and counted.
- **Pure-C client library (408fd06, 2fae860)**: `client/nekoecat_client.{c,h}` + `shm_layout.h`
  provide a C99 client over the SHM data plane and a JSON-RPC control plane. Adds
  `nekoecat_client_attach()`, cycle sync (`wait_next_cycle`/`try_wait_next_cycle`), typed
  read/write by index and raw-offset helpers with bounds validation, and the `DATA_STALE` state.
  A standalone `client/CMakeLists.txt` builds `libnekoecat_client.a`; install rules place the
  library in `lib/` and the headers in `include/nekoecat`.
- **Python example (408fd06)**: `examples/realtime/nekoecat_client_example.py` demonstrates raw
  `mmap`+`ctypes` access plus a ctypes wrapper of the C library.
- **Daemon RPC (408fd06)**: `apps/ecatd/freerun_rpc_handlers.{cpp,h}` add the `freeRunShmInfo`
  JSON-RPC method (layout/shm_name/data_size) alongside `freeRunStart`/`freeRunStop`/`freeRunStatus`.
- **FoE path allowlist (2fae860)**: firmware transfers are confined to `NEKOECAT_FIRMWARE_DIR`
  (defaults to `/tmp` and the user's home), with canonical-path resolution that rejects traversal,
  symlink escapes, hard links, and the validate-then-use TOCTOU window.
- **Status flag (408fd06)**: the daemon sets `NEKOECAT_FLAG_RUNNING` in `status_flags` as part of
  each atomic publish.
- **Supplementary tests (7daaf9a)**: `shm_stress_test`, `client_robustness_test`,
  `connection_recovery_test`, `language_switch_test`, plus the earlier SHM/layout/cycle tests
  (`free_run_mirror_test`, `freerun_client_cycle_test`, `nekoecat_client_layout_test`).

### Fixed

- **Red build (1b42117)**: `service_integration_test` no longer calls
  `ServiceContainer::alarm()/logging()` (those accessors exist only under
  `ECAT_EXPERIMENTAL_SERVICES`); the call sites are now guarded so the default build compiles.
- **Test suite reflects what ships (1b42117)**: unit tests for services and plugins that are no
  longer part of the default application build (Workflow/EtherCAT-analytics/optimizer services,
  dashboard and workflow-editor plugins, etc.) are gated behind `ECAT_EXPERIMENTAL_SERVICES`. The
  default run is now 179 tests (100% passing) covering only compiled, registered, and visible
  functionality.
- **SHM client hardening (2fae860)**: bounded receive/line buffers (dynamic growth with a cap
  instead of a fixed `recv_line`), SIGBUS guard that clamps the mmap size to the actual object
  size so a corrupt header cannot drive reads/writes past the mapped region, and stricter stale
  (version/size) detection on attach.
- **Data race (1460820)**: `concurrent_access_test` no longer races; the shared concurrency
  fixture is now synchronized.
- **CI**:
  - Replaced the tiny inline `ecrt.h` stub (which could not compile the daemon/core tests and used
    a broken heredoc) with a tracked, complete stub at `.github/workflows/ci/ecrt_stub.h`
    (1b42117), installed via `setup-ethercat-stub.sh` (2e9038f).
  - Fixed the broken `libethercat.so` creation; removed the dead-code `OpcUaServer.cpp` entries
    from integration/unit test builds; rewrote the ineffective valgrind step into a real
    per-binary loop (1b42117).
  - First CI run blockers (2e9038f): PR-triggered jobs, OpenSSL/Qt-6.2 compatibility, shell
    portability, and static-analysis timing are cleaned up.
  - Second CI run failures (172ecdb): `EsiService`/`EsiParser` string/`QStringView` handling and a
    `project_persistence_test` fix.
  - Third CI round (3ca6073): missing `QJsonDocument` include in `protocol_integration_test`; a
    dangling-lambda test UB in `language_switch_test` is removed.
  - Fourth CI round (b0c672b): `QStringView ==` narrow-literal comparison in the ProjectIo ESI
    parse.
- **GUI layer (aaec6dc)**: language switching (re-trigger translation rebuild, translate the
  SettingsDialog), reconnection (EcatClient reconnect backoff/reset), and memory-leak fixes across
  `MainWindow`, `ServiceContainer`, `AsyncOperationManager`, `ScriptingService`, and the theme
  wiring.
- **Residual low-priority items (c7c9945)**: EcatClient request/response hygiene,
  `AlEventHandler` periodic cleanup, `al_event_handler_test` alignment, and removal of a stale
  `tests/unit/infra/CMakeLists.txt` section.
- **Scripts (1b42117)**: fixed `check_memory.sh` test glob (test binaries live in per-type
  subdirectories); `package-deb.sh`/`package-rpm.sh` derive the version from `CMakeLists.txt`
  instead of a stale 1.2.0 default; `package-linux.sh` no longer ships a fabricated v0.1.0
  release-note block; `capture_goal_evidence.sh` default scratch dir is sane and no longer
  fabricates a duplicate test run; `analyze_coverage.sh` matches modern gcovr `lines:`/`branches:`
  output.
- **Build hygiene (1b42117)**: deduplicated `NotesPlugin` and other duplicated source entries;
  added `unit`/`integration`/`performance` test labels so `ctest -L <label>` is meaningful; wired
  the previously orphaned `export_plugin_test` and `rttest_plugin_test` back into the build.
- **Docs (1b42117 + this round)**: README, ARCHITECTURE, DEVELOPER_GUIDE, PLUGIN_GUIDE and the
  `ServiceContainer.h` header no longer claim stale counts (247 tests, 85+ services, 28 visible
  workspaces); the registered/visible plugin table now matches `MainWindow.cpp`.

### Known CI issues (open, not yet fixed)

- **format-check** runs with `continue-on-error: true`: ~300 pre-existing files predate the
  committed `.clang-format` and fail the dry-run. It stays visible so violations don't regress,
  but it does not block merges until a dedicated formatting commit lands.
- **valgrind** flags ~11 plugin tests for memory leaks (the per-binary loop reports them as
  errors but they are pre-existing Qt-related allocations); the job currently fails on those.
- **QT_QPA_PLATFORM** is now set to `offscreen` at the workflow level so QApplication tests run
  headless in containers; widget tests should still set it per-test for local runs.

## [1.0.0-beta] — 2026-07-03

### Added

- Add Linux release packaging
- Bilingual release notes
- RT stability test workspace for EtherCAT bus timing analysis
- Increase RT test sampling density 4x
- 6x denser chart (3000 bins) with full daemon data transmission
- Custom cycle time input to RT test
- Real-time scrolling chart with fixed 2000-point window
- Replace QtCharts with QPainter for 10-100x faster chart rendering
- CommandDispatcher — string-keyed dispatch table for daemon commands
- Request timeout + explicit ConnectionState to EcatClient
- LanguageManager — centralized language registry for i18n
- 8-language support (en, zh-CN, ja, de, ko, zh-TW, fr, es)
- Expand translations for 6 new languages (ja/de/ko/zh-TW/fr/es)
- Complete zh-TW translations and clean up translation registry
- Complete 8-language translation coverage (100%)
- Expand settings, shared include header, manual with TOC
- 12 themes with proper QSS syntax
- Live theme preview + overview tab reorder
- Data export submenu — PDO, SDO, ESI, Watch, Startup, Topology, Host Health
- Raw text exports — PDO/SDO/Master/Slave Raw
- Free Run real-time chart — independent multi-window
- Real-time Monitor translations
- MRU projects, OD real-time monitor, Watch bulk read, tab shortcuts, 684 i18n entries
- Keyboard shortcut customization in Settings
- WorkspacePlugin interface + 4 unit tests
- EventBus with 8 event types + 8 unit tests
- PluginRegistry with ordering/lookup/visibility + 7 unit tests
- NotesPlugin — first proof-of-concept plugin with 5 tests
- .ts/.qm translation system alongside TranslationRegistry
- Extract SdoService, WatchService, TopologyService from MainWindow
- Daemon AL Event handler with CLI-based polling
- Daemon DC Sync handler with CLI + ecrt API enrichment
- Week 2 — daemon handlers + GUI plugins + auto-reconnect
- Week 3 Tasks 18-19 — Signal Handler + Signal Analyzer Plugin
- v1.0.0 release preparation
- Comprehensive EtherCAT protocol improvements and code quality fixes
- WC error tracking to FreeRunController
- Daemon diagnostic metrics to ping response
- WC error threshold monitoring to FreeRunController
- Cycle time jitter tracking to FreeRunController
- Major cleanup, build fix, and test restoration (81 files)
- Wire live data, add tr() i18n, add populateTable helper, update translations
- Extract 11 themes to .qss files, add ThemeManager
- Comprehensive API documentation for core interfaces, services, and plugins
- Language switching with QTranslator hot-reload

### Fixed

- Restore SDO dictionary filter logic and anti-flicker guards
- RT test chart rendering and layout compactness
- Chart Y-axis snap + right-align data
- Lifecycle test — use connectToHost(port) instead of connectToDaemon()
- Move workspace struct allocations after reset in buildUi()
- Repair broken SdoWorkspace split and complete SDO file decomposition
- QFrame black background in all themes
- Real-time chart now feeds live values from Free Run polling
- Chart now plots data immediately without needing Start
- Axis-linked window control in RealtimeChartDialog
- 2-space indent in WorkspacePlugin.h, add offscreen env to test
- Strengthen EventBus test assertions, add PluginRegistry out-of-range test
- Keep experimental modules opt-in
- Clean Qt build warnings
- Remove duplicate translation messages
- Tighten experimental product boundary
- Keep experimental tests opt-in
- Remove experimental sources from default tests
- Remove duplicate digital twin plugin
- Remove duplicate manager plugins
- Enforce unique plugin ids
- Remove unregistered esi plugin
- Skip hardware verification when offline
- Dynamic language switching with QTranslator hot-reload
- Repair language switching (5 bugs)
- Remove stale test targets and fix CMakeLists paths
- Resolve build errors from deleted services cleanup
- Update plugin tests to match hidden plugin visibility
- Update product_boundary_test to reflect removed experimental plugins
- Remove stale experimental plugin references from docs and tests
- Avoid simulated test pass results across EoE, FoE, CoE, state machine, Free Run, PDO, sync manager, domain, hot connect, redundancy, firmware, deployment, recovery, multi-master, EtherCAT sync/replication/update/maintenance/integration, master API, device manager, monitoring services, optimization services, cable diagnostics, hardware verification, and workflow operations
- Avoid synthetic data for trace capture, DC sync, backup, deployment, certification, compliance, maintenance, validation, master management, PDO/SDO/Free Run optimization, protocol analyzer frames, oscilloscope samples, dashboard metrics, calibration, logic analyzer, blockchain explorer, quantum security, workflow certificates/execution/reports, certification manager, compliance checker, replication manager, batch operations, maintenance scheduler, edge service, and test suite passes
- Require backend ack for optimization apply and Free Run config apply
- Fail batches with failed items
- Avoid unfinished logic analyzer decode text and protocol backend wording
- Persist config profile imports and exports
- Write reporting exports to files
- Confine saved automation scripts
- Export selected script library entries
- Validate all export paths for report generator, export service, diagram JSON, workflow designer, PDO mapping, configuration, ESI, log, diagnostic, automation scripts, maintenance reports, update logs, deployment logs, report designer, workflow optimizer, documentation, test suite, visualization studio, workflow dashboard, script library, simulation results, trace data, alarm history, multi-master reports, error analysis reports, and all optimizer exports
- Validate all import paths for templates, configuration, workflow, report template, visualization, diagram, script, ESI, and project files
- Import hash-prefixed ESI hex ids
- Reject future project versions
- Avoid scripting false bus success
- Avoid synthetic multi-master discovery
- Fail diagnostics without evidence
- Stop synthetic realtime samples
- Avoid empty performance analysis success
- Gate pipeline completion on execution
- Validate workflow report templates, task creation dependencies
- Validate resource allocation, project planning, project tracking, project management inputs
- Validate PDO configuration, PDO mapping, sync manager, domain PDO entries, master manager, distributed clock config, and DC sync polling inputs
- Avoid healthy score without ecat evidence
- Mark network health unknown without evidence
- Avoid monitor healthy defaults without evidence

### Changed

- Restructure project: organize into models/adapters/ui_state/helpers/infra/workspaces subdirectories
- Redesign RT test workspace with QtCharts latency graph and higher cycle rates
- Replace EcatDaemon if/else chain with CommandDispatcher dispatch table
- Extract EcatService interface, daemon uses pointer
- Aggregate RT test widgets into workspace-local struct
- Remove unused Qt6::Charts dependency — charts use QPainter directly
- Extract SessionWorkspaceWidgets from MainWindow
- Extract StateMachineWorkspaceWidgets from MainWindow
- Extract ConsistencyWorkspaceWidgets from MainWindow
- Extract DiagnosticsWorkspaceWidgets from MainWindow
- Extract IoVariableWorkspaceWidgets from MainWindow
- Extract WatchWorkspaceWidgets from MainWindow
- Extract SdoWorkspaceWidgets from MainWindow
- Extract WorkflowWorkspaceWidgets from MainWindow
- Extract SlaveEvidenceWorkspaceWidgets from MainWindow
- Extract FreeRunWorkspaceWidgets from MainWindow
- Extract BookmarkWorkspaceWidgets from MainWindow
- Extract freeRunTable_ to FreeRunWorkspaceWidgets
- Extract SdoInspectorWidgets and RawTextWidgets from MainWindow
- Decompose MainWindowSdoWorkspace.cpp (4877 to 3 files)
- Decompose MainWindow.cpp (4677 to 3 files)
- Decompose WatchWorkspace and ConsistencyCommissioning
- Decompose IoVariableWorkspace (1390 to 2 files)
- Rename ui_state/ to detail/ with cleaner naming
- Rename helpers/ to utils/ and remove Studio prefix
- Consolidate small model files into logical groups
- Include request ID in EcatClient error messages
- EcatClient now invokes handlers on daemon errors
- Add error checks to setState/setAllStates/download handlers
- Track docs/ in git — ignore only docs/build/
- Remove duplicate includes from 15 workspace files
- Remove unused chrono include from FreeRunController
- Ignore local worktrees
- Version bumped to 1.0.0 for official release
- Default package version in scripts updated to 1.0.0
- Clean stale root files and fix .gitignore
- Remove AI development artifacts (AGENTS.md)
- Add .clang-format, .editorconfig, .clang-tidy
- Remove 8 duplicate service pairs, 5 dead services, 6 experimental plugins
- Remove 47 orphaned tests and reorganize test directory
- Split tests/CMakeLists.txt into per-directory files
- Remove helper script used for CMakeLists.txt split
- Remove 9 zero-reference dead services (~2100 lines)
- Hide 37 stub plugins for production release (84 to 47 visible tabs)
- Reduce UI noise (54 to 28 visible tabs), wire up 3 orphaned tests
- Week 1 verification — 52/52 tests pass, GUI smoke test OK

### Removed

- Unused Qt6::Charts dependency
- Duplicate includes from 15 workspace files
- Unused chrono include from FreeRunController
- 8 duplicate service pairs, 5 dead services, 6 experimental plugins
- 47 orphaned tests
- 9 zero-reference dead services (~2100 lines)
- Helper script used for CMakeLists.txt split
- 37 stub plugins from production visibility
- AI development artifacts (AGENTS.md)
- Stale root files and build artifacts from git tracking
- Stale experimental plugin references

### Documentation

- Restructure project with inline comments across all 28 workspace files
- Add modern inline comments across all source files
- Add one-month comprehensive development plan
- Add file-level purpose comments to 15 workspace files
- Add method-level comments to MainWindow, EcatClient, EcatDaemon, EthercatCliBackend
- Add 28 section comments to MainWindowUiBuild.cpp
- Add 24 section comments to MainWindowCommandPalette.cpp
- Add section comments to MainWindowTheme.cpp
- Add section comments to SdoTargetPanel and SdoBookmarks workspace files
- Add section comments to Manual, ContextMenus, and Theme files
- Add method-level comments to MainWindowStateMachine and MainWindowTopologyUi
- Add section markers and inline comments to workspace files
- Massively expand comment density across all source files
- Add QSS and struct field comments to Theme, Manual, Texts
- Add context menu and command palette comments
- Add method-level comments to low-coverage workspace files
- Update AGENTS.md for infra and workspaces directories
- Update AGENTS.md to reflect new directory structure
- Add phase 4 design spec and implementation plan
- Add API documentation to core interfaces (EcatService.h, EthercatTypes.h, JsonProtocol.h, EcatClient.h)
- Add API documentation to 10 key service headers
- Add API documentation to 9 key plugin headers
- Enhance 19 workspace partial file-level documentation
- V2 plugin architecture design — 4-week refactor + feature expansion
- 4-week v2 implementation plan — 27 tasks, plugin arch + daemon migration + features
- V2 comprehensive design — plugin activation + topology + DC + ESI + bus stats
- V2 month plan — 28 tasks, 4 weeks, plugin activation + topology + DC + ESI + bus stats
- Handoff document for mimoCode transition
- Foundation hardening design
- Foundation hardening plan
- Mark experimental surfaces opt-in
- Align public project facts
- Align TwinCAT benchmark facts
- Tighten EtherCAT gap facts
- Clarify native backend coverage
- Align release package version
- Remove stale service count comments
- Remove stale plugin count comments
- Contextualize quality claims
- Remove stale service count claims
- Remove remaining service count claims
- Avoid static plugin and test source counts
- Align plugin guide CMake requirement
- Soften unverifiable TwinCAT scoring
- Remove remaining TwinCAT percentage scores
- Avoid static language coverage counts
- Temper absolute product positioning
- Temper TwinCAT superiority claims
- Avoid static localization and layout counts
- Temper benchmark quality ratings
- Qualify field readiness status
- Temper README commissioning claims
- Qualify release and native backend claims
- Avoid static ctest count claims
- Avoid static test source counts
- Qualify TwinCAT performance comparison
- Qualify TwinCAT parity claims
- Qualify TwinCAT workflow parity
- Temper benchmark advantage claims
- Avoid static release quality claims
- Qualify release performance claims
- Avoid stale release quality claims
- Avoid absolute release coverage claims
- Cover lowercase full coverage claim
- Update architecture documentation with new features
- Remove deleted service references from ARCHITECTURE.md
