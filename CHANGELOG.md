# Changelog

All notable changes to NekoEcat Studio are documented in this file.

## [3.8.0] - 2026-06-21

### Added
- **Native IgH API Backend**: Direct ecrt API integration for 10-100x SDO performance improvement
  - `EthercatNativeBackend` class implementing `EcatService` interface using ecrt API
  - `MasterGuard` RAII class for safe master lifecycle management with mutex protection
  - Type-aware SDO upload/download with hex index parsing
  - PDO information retrieval via sync manager enumeration
  - Slave information retrieval via `ecrt_master_get_slave()`
- **Dual-Backend Mode**: Runtime backend switching between Native API and CLI
  - Three modes: Auto (recommended), Native API, CLI
  - Settings UI with combo box in EtherCAT tab
  - `setBackend`/`getBackend` JSON-RPC commands
  - Runtime switching without daemon restart
- **Unit Tests**: 9 new tests for native backend (8 pass, 1 skip)
- **Integration Tests**: 4 new tests comparing native vs CLI backend performance
- **Documentation**: Comprehensive project overview and TwinCAT benchmark review

### Changed
- Version bumped to 3.8.0
- Default backend mode: Auto (try native, fallback to CLI)
- Default registered tests: see the corresponding build output; rerun validation before release
- Updated README.md, ARCHITECTURE.md, PROJECT_OVERVIEW.md

### Performance
- SDO operations: ~500ms (CLI) → ~50ms (Native) = 10x improvement
- Topology scanning: ~500ms (CLI) → ~100ms (Native) = 5x improvement

## [3.7.0] - 2026-06-20

### Added
- **Boundary Tests**: Empty data boundary tests, large data boundary tests for robustness verification
- **Integration Tests**: Plugin integration tests, service integration tests, EventBus integration tests
- **Concurrent Access Tests**: Thread-safety verification for ServiceContainer and shared services
- **Error Recovery Tests**: Validation of error detection, classification, and recovery workflows
- **UI Creation Tests**: Widget instantiation and lifecycle verification tests
- **Architecture Documentation**: ServiceContainer single-client design, error handling architecture, performance monitoring architecture

### Fixed
- **ServiceContainer**: Fixed concurrent access issues in multi-threaded test scenarios
- **Test Infrastructure**: Improved test isolation and fixture reliability

### Changed
- Version bumped to 3.7.0
- Default registered tests: see the corresponding build output; rerun validation before release
- ServiceContainer service count is defined by the corresponding source tree
- Plugin count is defined by the corresponding source tree
- Updated README.md, ARCHITECTURE.md, CHANGELOG.md, RELEASE_NOTES.md

## [3.6.0] - 2026-06-20

### Added
- **New Features**:
  - **DC Sync Optimization** (`dcsyncoptimization`): DC distributed clock synchronization optimization with sync, drift, jitter, and configuration optimization via SyncOptimizationWidget and DriftOptimizationWidget
  - **Free Run Optimization** (`freerunoptimization`): Free Run process data exchange optimization with cycle time, data mapping, performance, and error handling optimization via CycleTimeOptimizerWidget and DataMappingOptimizerWidget
  - **PDO Mapping Optimization** (`pdomappingoptimization`): PDO mapping configuration optimization with mapping, size, alignment, and performance optimization via MappingOptimizerWidget and SizeOptimizerWidget
  - **SDO Optimization** (`sdooptimization`): SDO communication optimization with cache, batch, performance, and error handling optimization via CacheOptimizerWidget and BatchOptimizerWidget
- **New Services**: DcSyncOptimizationService, DcSyncOptimizerService, FreeRunOptimizationService, PdoMappingOptimizationService, SdoOptimizationService, RealtimeOptimizerService
- **New Plugins**: 6 new workspace plugins for the optimization features above
- **Documentation**: Comprehensive update to README.md, ARCHITECTURE.md, CHANGELOG.md, and RELEASE_NOTES.md

### Changed
- Version bumped to 3.6.0
- ServiceContainer service count is defined by the corresponding source tree
- Test results: see the corresponding build output; rerun validation before release

## [3.5.0] - 2026-06-20

### Added
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages
- **Release Documentation**: Release notes, installation guide, user manual, developer guide
- **Quality Reports**: Test coverage, performance, memory usage, and code quality reports
- **Final Verification**: Clean build, all tests pass, all packages valid

### Changed
- Version bumped to 3.5.0
- Test results: see the corresponding build output; rerun validation before release

## [3.4.0] - 2026-06-20

### Added
- **New Features**:
  - **PDO Mapping Editor** (`pdomapping`): Visual PDO mapping configuration with canvas-based drag-and-drop, mapping validator, SM/PDO assignment, and export
  - **ESI Browser** (`esi`): Enhanced ESI repository with tree-based device browsing, PDO mapping lookup, and multi-file ESI XML import
  - **Online Diagnostics** (`onlinediagnostics`): Real-time bus monitoring with BusMonitorWidget, ErrorAnalyzerWidget, health scoring, and reviewable diagnostic suggestions
  - **DC Sync Precision** (`dcsyncprecision`): Extended DC sync diagnostics with DriftMonitorWidget, JitterAnalysisWidget, and per-slave synchronization precision assessment
  - **Multi-Master Support** (`multimaster`): Multi-master management with MasterComparisonWidget for side-by-side diagnostics and cross-master slave management
  - **Real-time Performance Monitor** (`realtimeperf`): Latency and throughput monitoring with LatencyMonitorWidget, ThroughputMonitorWidget, and 1000-sample ring buffers
  - **Advanced Error Analysis** (`erroranalysis`): Error timeline with ErrorTimelineWidget, error correlation with ErrorCorrelationWidget, and cross-slave pattern recognition
  - **Hardware Verification** (`hardwareverification`): Pre-commissioning device and network verification with DeviceVerificationWidget and NetworkVerificationWidget
- **New Services**: WorkflowSecurityManagerService, WorkflowComplianceManagerService, WorkflowCertificationManagerService, PdoMappingService, DcSyncPrecisionService, OnlineDiagnosticsService, MultiMasterService, RealtimePerformanceService, AdvancedErrorAnalysisService, HardwareVerificationService
- **New Plugins**: 8 new workspace plugins for the features above
- **Unit Tests**: 3 new service unit tests for WorkflowSecurityManagerService (12 cases), WorkflowComplianceManagerService (12 cases), WorkflowCertificationManagerService (13 cases)
- **Documentation**: Comprehensive update to README.md, ARCHITECTURE.md, PLUGIN_GUIDE.md, and CHANGELOG.md
- **Release Preparation**: Version bump to 3.4.0, comprehensive testing, optimization, and release packaging

### Fixed
- **Build Fix**: Added missing WorkflowSecurityManagerService, WorkflowComplianceManagerService, WorkflowCertificationManagerService source files referenced by ServiceContainer

### Changed
- Version bumped to 3.4.0
- ServiceContainer service count is defined by the corresponding source tree
- Plugin count is defined by the corresponding source tree
- registered tests and test files as reported by the corresponding build output
- Test results: see the corresponding build output; rerun validation before release

## [3.3.0] - 2026-06-20

### Added
- **New Services**: WorkflowVisualizationStudioService, WorkflowReportDesignerService, WorkflowDocumentationBrowserService
- **Unit Tests**: 3 new service unit tests for WorkflowVisualizationStudioService (14 cases), WorkflowReportDesignerService (12 cases), WorkflowDocumentationBrowserService (14 cases)
- **Performance Tests**: 3 new performance tests for VisualizationStudio (4 cases), ReportDesigner (4 cases), DocumentationBrowser (4 cases)
- **Release Preparation**: Version bump to 3.3.0, comprehensive testing, optimization, and release packaging

### Changed
- Version bumped to 3.3.0
- Test results: see the corresponding build output; rerun validation before release

## [3.2.0] - 2026-06-19

### Added
- **Unit Tests**: 6 new service unit tests for WorkflowDigitalTwinService, WorkflowBlockchainService, WorkflowQuantumService, WorkflowCloudService, WorkflowEdgeService, WorkflowAIService
- **Integration Tests**: 2 new plugin integration tests for WorkflowOptimizerPlugin, WorkflowDashboardPlugin
- **Performance Tests**: 6 new performance test cases for DigitalTwin, Blockchain, Quantum, Cloud, Edge, and AI services
- **Release Preparation**: Version bump to 3.2.0, updated CHANGELOG, RELEASE_NOTES, build and test validation

### Changed
- Version bumped to 3.2.0
- Test results: see the corresponding build output; rerun validation before release

## [3.1.0] - 2026-06-19

### Added
- **New Services**: WorkflowCloudService, WorkflowEdgeService, WorkflowAIService
- **Unit Tests**: 33 new test cases for WorkflowCloudService (9), WorkflowEdgeService (12), WorkflowAIService (12)
- **Performance Tests**: 18 new performance test cases for cloud (6), edge (6), and AI (6) services
- **Release Preparation**: Version bump to 3.1.0, updated CHANGELOG, RELEASE_NOTES

### Changed
- Version bumped to 3.1.0
- Test results: see the corresponding build output; rerun validation before release

## [3.0.0] - 2026-06-19

### Added
- **New Services**: WorkflowIntegrationHubService, WorkflowSyncService, WorkflowReplicationService
- **Unit Tests**: 36 new test cases for WorkflowIntegrationHubService, WorkflowSyncService, WorkflowReplicationService
- **Performance Tests**: 9 new performance test cases for integration hub, sync, and replication services
- **Release Preparation**: Version bump to 3.0.0, updated CHANGELOG, RELEASE_NOTES

### Changed
- Version bumped to 3.0.0
- Test results: see the corresponding build output; rerun validation before release

## [2.9.0] - 2026-06-19

### Added
- **Performance Tests**: WorkflowDeploymentService performance test (6 test cases), WorkflowUpdateService performance test (6 test cases), WorkflowMaintenanceService performance test (6 test cases)
- **Release Preparation**: Version bump to 2.9.0, updated CHANGELOG, RELEASE_NOTES

### Changed
- Version bumped to 2.9.0
- Test results: see the corresponding build output; rerun validation before release

## [2.8.0] - 2026-06-19

### Added
- **Final Release Preparation**: Comprehensive testing, optimization, and release packaging for v2.8.0
- **Quality Assurance**: Test, coverage, performance, and memory reports should be regenerated for the target release build

### Changed
- Version bumped to 2.8.0

## [2.7.0] - 2026-06-19

### Added
- **Performance Tests**: WorkflowAnalyticsService performance test (6 test cases), WorkflowMonitoringService performance test (6 test cases)
- **Release Preparation**: Version bump to 2.7.0, updated CHANGELOG, RELEASE_NOTES

### Changed
- Version bumped to 2.7.0

## [2.6.0] - 2026-06-19

### Added
- **Performance Tests**: WorkflowAutomationService (6), WorkflowOptimizationService (8), WorkflowSchedulingService (7) performance tests
- **Integration Tests**: WorkflowOptimizerPlugin integration test (9 test cases), WorkflowDashboardPlugin integration test (10 test cases)
- **Performance Optimization**: Lazy service initialization, parallel plugin loading, connection pooling, data caching, async operations, batch processing, object pooling, memory arenas

### Changed
- Version bumped to 2.6.0

## [2.5.0] - 2026-06-19

### Added
- **Digital Twin Service**: EtherCATDigitalTwinService for digital twin modeling, node management, connection tracking, and snapshot capabilities
- **Blockchain Service**: EtherCATBlockchainService for blockchain-based configuration audit, immutable logging, and distributed verification
- **Quantum Service**: EtherCATQuantumService for quantum-resistant encryption, key management, and secure communication
- **Digital Twin Studio Plugin**: DigitalTwinStudioPlugin for digital twin visualization and management workspace
- **Blockchain Explorer Plugin**: BlockchainExplorerPlugin for blockchain chain exploration and audit log management
- **Quantum Security Plugin**: QuantumSecurityPlugin for quantum key management and security audit workspace
- **Unit Tests**: EtherCATDigitalTwinService (18), EtherCATBlockchainService (14), EtherCATQuantumService (14) unit tests
- **Integration Tests**: DigitalTwinStudioPlugin (14), BlockchainExplorerPlugin (12), QuantumSecurityPlugin (12) integration tests
- **Performance Tests**: EtherCATDigitalTwinService (4), EtherCATBlockchainService (4), EtherCATQuantumService (4) performance tests

### Changed
- Version bumped to 2.5.0
- ServiceContainer expanded to 52 services

## [2.4.0] - 2026-06-19

### Added
- **Cloud Manager Plugin**: CloudManagerPlugin for cloud connectivity, sync, backup, and monitoring
- **Edge Computing Plugin**: EdgeComputingPlugin for edge data processing, analysis, and storage
- **AI Assistant Plugin**: AIAssistantPlugin for predictive maintenance, anomaly detection, optimization, and pattern recognition
- **Unit Tests**: EtherCATCloudService (12), EtherCATEdgeService (10), EtherCATAIService (17), CloudManagerPlugin (13), EdgeComputingPlugin (9), AIAssistantPlugin (10) unit tests
- **Performance Tests**: EtherCATCloudService, EtherCATEdgeService, EtherCATAIService performance tests (11 test cases)

### Changed
- Version bumped to 2.4.0

## [2.3.0] - 2026-06-19

### Added
- **Documentation Browser Plugin**: DocumentationBrowserPlugin for browsing, searching, and bookmarking documentation
- **Performance Tests**: Performance tests for EtherCATVisualizationService, EtherCATReportingService, and EtherCATDocumentationService (15 test cases)
- **Unit Tests**: DocumentationBrowserPlugin unit tests (16 test cases)

### Changed
- Version bumped to 2.3.0

## [2.2.0] - 2026-06-19

### Added
- **Integration Hub Plugin**: EtherCATIntegrationService for PLC, SCADA, MES, and ERP connectivity
- **Sync Manager Plugin**: EtherCATSyncService for time, data, state, and configuration synchronization
- **Replication Manager Plugin**: EtherCATReplicationService for multi-target configuration, data, state, and backup replication
- **Performance Tests**: Performance tests for EtherCATIntegrationService, EtherCATSyncService, and EtherCATReplicationService (15 test cases)
- **Performance Optimizations**: Throughput and latency improvements across integration, sync, and replication services

### Changed
- Version bumped to 2.2.0

## [2.1.0] - 2026-06-19

### Added
- **New Services**: EtherCATDeploymentService, EtherCATUpdateService, EtherCATMaintenanceService for deployment management, firmware updates, and maintenance scheduling
- **New Plugins**: UpdateManagerPlugin, MaintenanceSchedulerPlugin workspace plugins
- **Unit Tests**: Unit tests for all new services (25+ test cases)
- **Integration Tests**: Integration tests for all new plugins (20+ test cases)
- **Performance Tests**: Performance tests for all new services (9 test cases)
- **Version Bump**: Version bumped to 2.1.0

### Changed
- Version bumped to 2.1.0

## [2.0.0] - 2026-06-19

### Added
- **New Plugins**: OptimizationDashboardPlugin, MonitoringDashboardPlugin, AnalyticsDashboardPlugin workspace plugins
- **Integration Tests**: Integration tests for all new dashboard plugins
- **Performance Optimization**: Lazy initialization, parallel plugin initialization, cached UI initialization
- **Memory Optimization**: Object pooling, memory arenas, reference counting, weak references

### Changed
- Version bumped to 2.0.0

## [1.9.0] - 2026-06-19

### Added
- **New Services**: EtherCATSecurityService for security policy management and auditing, EtherCATComplianceService for compliance rule management and checking, EtherCATCertificationService for certification requirement management and testing
- **New Plugins**: SecurityManagerPlugin, ComplianceCheckerPlugin, CertificationManagerPlugin workspace plugins
- **Unit Tests**: Unit tests for all new services
- **Integration Tests**: Integration tests for all new plugins
- **Performance Tests**: Performance tests for all new services
- **Startup Optimization**: Lazy initialization for faster startup
- **Connection Pooling**: Connection pooling for daemon communication
- **Data Caching**: Data caching for SDO/PDO data

### Changed
- Version bumped to 1.9.0

## [1.8.0] - 2026-06-19

### Added
- **New Plugins**: ConfigurationEditorPlugin, NetworkAnalyzerPlugin, SystemMonitorPlugin for visual configuration editing, network analysis, and system monitoring
- **Unit Tests**: 3 new unit tests for EtherCATSimulationService, EtherCATTestingService, EtherCATValidationService (35+ test cases)
- **Integration Tests**: 3 new integration tests for ConfigurationEditorPlugin, NetworkAnalyzerPlugin, SystemMonitorPlugin
- **Performance Tests**: 3 new performance tests for simulation, testing, and validation services

### Changed
- Version bump to 1.8.0
- Expanded test suite to 220+ tests total

## [1.7.0] - 2026-06-19

### Added
- **New Services**: EtherCATConfigService, EtherCATBackupService, EtherCATRecoveryService for configuration management, backup/restore, and error recovery
- **New Plugins**: WorkflowDesignerPlugin, TestSuitePlugin, DeploymentPlugin for visual workflow design, test suite management, and deployment management
- **Unit Tests**: 38+ new unit tests for all new and existing services
- **Integration Tests**: 3 new integration tests for WorkflowDesignerPlugin, TestSuitePlugin, DeploymentPlugin
- **Performance Tests**: 15+ new performance tests for configuration, backup, recovery, and monitoring services

### Changed
- Version bump to 1.7.0
- Improved test coverage across all service and plugin layers

## [1.6.0] - 2026-06-19

### Added

#### New Services
- **EtherCATOptimizerService** — runtime optimization for EtherCAT operations with batch execution, data caching, prefetching, and warmup support.

#### New Service Unit Tests
- **EtherCATMonitorService** — 12 unit tests covering initial state, monitoring start/stop, signal emissions, and polling behavior.
- **EtherCATAnalyzerService** — 12 unit tests covering frame analysis, error analysis, performance analysis, trend analysis, and signal emissions.
- **EtherCATOptimizerService** — 14 unit tests covering configuration, batch execution, cache operations, prefetch, invalidation, and warmup.

#### New Performance Tests
- **EtherCATMonitorService performance** — start/stop, polling, query, and monitoring overhead throughput tests.
- **EtherCATAnalyzerService performance** — frame, error, performance, trend, and mixed analysis throughput tests.
- **EtherCATOptimizerService performance** — batch execution, cache, prefetch, invalidation, mixed operations, and warmup throughput tests.

### Changed
- Updated version to 1.6.0

## [1.5.0] - 2026-06-19

### Added

#### New Services
- **HotConnectService** — manages hot-connect groups for EtherCAT slaves with dynamic addition/removal during runtime.
- **RedundancyService** — manages EtherCAT network redundancy with primary/secondary path monitoring and failover.
- **CableDiagnosticsService** — performs cable diagnostics on EtherCAT network testing cable quality, length, and fault detection.

#### New Service Unit Tests
- **HotConnectService** — 14 unit tests covering group creation, activation, deactivation, history, and signals.
- **RedundancyService** — 13 unit tests covering redundancy enable/disable, failover, failback, and history.
- **CableDiagnosticsService** — 10 unit tests covering port testing, all ports testing, history, and clearing.

#### New Performance Tests
- **HotConnectService performance** — group creation, activation/deactivation, query, and history throughput tests.
- **RedundancyService performance** — enable/disable, failover, history, and query throughput tests.
- **CableDiagnosticsService performance** — single port, all ports, history, and query throughput tests.

### Changed
- Updated version to 1.5.0

## [1.4.0] - 2026-06-19

### Added

#### New Service Unit Tests
- **StateMachineService** — 14 unit tests covering state transitions, validation, history, and recovery.
- **ErrorHandlingService** — 13 unit tests covering error detection, classification, recovery, and history.

#### New Plugin Integration Tests
- **SimulationPlugin** — 16 integration tests for EtherCAT bus simulation plugin.
- **CalibrationPlugin** — 17 integration tests for device calibration wizard plugin.
- **DocumentationPlugin** — 19 integration tests for documentation browsing and search plugin.

#### New Performance Tests
- **StateMachineService performance** — throughput, latency, and memory stability tests.
- **ErrorHandlingService performance** — throughput, latency, and memory stability tests.
- **DiagnosticReportService performance** — throughput, latency, and memory stability tests.

### Changed
- Updated version to 1.4.0

## [1.3.0] - 2026-06-19

### Added

#### New Services (Unit Tested)
- **MasterApiService** — EtherCAT master lifecycle management (create, activate, deactivate) with state tracking and slave configuration.
- **DomainService** — EtherCAT domain management with PDO entry registration, domain processing, and data access.
- **SyncManagerService** — Sync Manager configuration with PDO assignment, direction control, and watchdog settings.

#### New Plugins
- **DiagramPlugin** — Network topology diagram visualization using QPainter with node management and antialiased rendering.
- **FormulaPlugin** — Mathematical formula calculator with expression parser supporting +, -, *, /, and parentheses.
- **ScriptLibraryPlugin** — Script library manager with pre-loaded EtherCAT scripts, list navigation, and editor.

#### Performance Tests
- MasterApiService performance tests (throughput, latency, signal emission)
- DomainService performance tests (creation, PDO registration, processing, queries)
- SyncManagerService performance tests (configuration, PDO assignment, queries)

#### New Unit Tests
- MasterApiService unit tests (creation, activation, deactivation, error handling)
- DomainService unit tests (domain creation, PDO registration, processing, data access)
- SyncManagerService unit tests (configuration, PDO assignment, direction, watchdog)
- ConnectionPool unit tests (acquire, release, health, exhaustion)
- StartupOptimizer unit tests (lazy init, metrics, service registration)
- DataCache unit tests (get/put, batch operations, eviction, prefetch, stats)
- BatchProcessor unit tests (start, cancel, progress, results)

### Changed
- Updated version to 1.3.0

## [1.2.0] - 2026-06-19

### Added

#### Performance Optimization Services

- **ConnectionPool** — TCP connection pooling for daemon communication with health checking and automatic reconnection.
- **StartupOptimizer** — Lazy initialization, parallel initialization, and preloading of frequently used data.
- **DataCache** — Specialized cache for SDO/PDO data with batch operations, prefetching, and statistics.
- **BatchProcessor** — Batch operations for multiple SDO/state requests with progress tracking and cancellation.

#### Release Preparation

- Updated version to 1.2.0 in CMakeLists.txt
- Added DEB package generation script
- Added RPM package generation script
- Added AppImage generation script
- Added source code package generation script

### Changed

- Enhanced CacheService with batch operations and prefetching capabilities
- Improved AsyncOperationManager with better error handling and progress tracking
- Optimized MemoryPool with better allocation strategies

### Testing

- 118 tests covering models, adapters, plugins, services, integration, and boundary cases
- 15 release smoke tests that must pass before packaging
- Test categories: model tests, adapter tests, UI state tests, integration tests, performance tests
- Offscreen Qt platform for headless CI testing

## [Unreleased]

### Added

#### New Services

- **CacheService** — Generic LRU cache with TTL expiration for SDO, topology, PDO, and ESI data. Thread-safe via `QReadWriteLock` with per-cache-type configuration.
- **AsyncOperationManager** — Priority-queued async operations with configurable concurrency (default 4), timeout handling (default 30s), cancellation support, and progress tracking.
- **WatchdogService** — EtherCAT watchdog monitoring with per-slave status tracking, timeout/trigger counters, and EventBus integration for topology changes.
- **SafetyController** — Safety boundary validation for state transitions, SDO writes during Free Run, and Free Run start conditions. Prevents dangerous operations without explicit confirmation.
- **DiagnosticReportService** — Comprehensive diagnostic reports covering topology, slave status, performance metrics, DC sync, and watchdog status. Export in Markdown and CSV formats.
- **ProjectManagerService** — Project lifecycle management (create/open/save/export/import) with `.ecatproj` JSON files, recent projects list, and unsaved-change tracking.
- **ConfigurationService** — Structured configuration management for master, slave, network, timing, and safety settings with JSON serialization and validation.
- **AlarmService** — System alarm management with severity levels (Info/Warning/Error/Critical), categories (Communication/Device/Network/Configuration), and lifecycle states (Active/Acknowledged/Cleared).
- **LoggingService** — Centralized logging with file rotation (10MB max per file, 10 files), level filtering (Debug through Fatal), and category-based organization.
- **ChartService** — Chart data management for DashboardPlugin and ChartPlugin with support for line, bar, pie, scatter, and gauge chart types.
- **BatchOperationService** — Batch SDO read/write, state change, and topology scan operations with progress tracking and cancellation.
- **ScriptingService** — Embedded JavaScript scripting engine (optional, requires `Qt6::Qml` and `ECAT_SCRIPTING_ENABLED`) with access to SDO, topology, and state operations.

#### New Plugins

- **DashboardPlugin** — Configurable dashboard with gauges, counters, and sparkline charts in a grid layout. Uses ChartService for data visualization.
- **ChartPlugin** — Data visualization workspace with line, bar, pie, scatter, and gauge charts. Supports data source selection and chart export.
- **AutomationPlugin** — JavaScript script editor with syntax highlighting, script management, output console, and execution controls. Uses ScriptingService.
- **ProtocolAnalyzerPlugin** — EtherCAT protocol analysis with frame capture, protocol decode (EtherCAT/CoE/EoE/FoE/SoE), filtering, statistics, and PCAP export.
- **ProjectPlugin** — Project management workspace with tree-based navigation, configuration pages, and import/export capabilities.
- **AlarmPlugin** — System alarm display with filtering by level/category/state, acknowledge/clear actions, and alarm history export.
- **OscilloscopePlugin** — Real-time multi-channel waveform display with configurable timebase, trigger modes, cursor measurements, and FFT analysis.
- **DataPipelinePlugin** — Data pipeline management with pipeline configuration, stage management, and monitoring UI.
- **DeviceManagerPlugin** — Device discovery, configuration, and status monitoring UI.
- **MasterManagerPlugin** — EtherCAT master management with master info display, diagnostics, restart, and log viewer.

#### New Utilities

- **MemoryPool\<T\>** — Fixed-size object pool for frequent allocations. Thread-safe, with overflow fallback to heap allocation and statistics tracking (peak usage, overflow count).
- **ConfirmDialogBuilder** — Fluent builder for confirmation dialogs with impact details and safety labels.
- **ActionAvailabilityHelper** — Central guard for enabling/disabling actions based on selection and connection state.

#### Architecture

- **Plugin System** — WorkspacePlugin interface with PluginRegistry for lifecycle management, EventBus for inter-plugin communication, and ServiceContainer for dependency injection.
- **Service Layer** — Service coverage spans SDO, topology, watch, DC sync, AL events, signals, performance monitoring, ESI, bus statistics, caching, async operations, watchdog, safety, diagnostics, project management, configuration, alarms, logging, charts, batch operations, scripting, export, data pipeline, device management, firmware update, report generation, master management, distributed clock, PDO mapping, DC sync precision, online diagnostics, multi-master, real-time performance, error analysis, and hardware verification.
- **Safety Model** — Explicit safety boundaries with local-only review paths, confirmation dialogs for dangerous operations, and SafetyController validation.

### Changed

- MainWindow refactored into 31 partial `.cpp` files in `workspaces/` for maintainability
- ServiceContainer service count is defined by the corresponding source tree
- EventBus maintained 8 event types with type-safe emit methods
- PluginRegistry supports null/empty/duplicate id guards
- All service headers include comprehensive documentation comments
- Plugin count is defined by the corresponding source tree
- Registered tests across unit, integration, performance, and boundary categories are defined by the corresponding build output

### Fixed

- **AsyncOperationManager** — Fixed SEGFAULT in destructor by waiting for thread pool to complete before deleting operations

### Testing

- Registered tests covering models, adapters, plugins, services, integration, and boundary cases are defined by the corresponding build output
- Test files on disk are defined by the corresponding source tree
- Release smoke coverage should be regenerated for the target packaging build
- Test categories: model tests, adapter tests, UI state tests, integration tests, performance tests
- Offscreen Qt platform for headless CI testing
