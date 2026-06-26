# NekoEcat Studio v3.8.0 Release Notes

**Release Date**: June 21, 2026

## Highlights

Native IgH API backend, dual-backend mode switching, and test coverage that must be revalidated for the target build.

## New Features

### Native IgH API Backend
- Direct ecrt API integration replacing CLI shell-outs
- `EthercatNativeBackend` class with all 12 EcatService methods
- `MasterGuard` RAII class for safe master lifecycle management
- Type-aware SDO upload/download with hex index parsing
- PDO information retrieval via sync manager enumeration

### Dual-Backend Mode
- Three modes: Auto (recommended), Native API, CLI
- Settings UI with combo box in EtherCAT tab
- `setBackend`/`getBackend` JSON-RPC commands
- Runtime switching without daemon restart

### Performance Improvements
- SDO operations: native backend reduces CLI process overhead; measure on target hardware
- Topology scanning: native backend reduces CLI process overhead; measure on target hardware

## New Tests

- **Unit Tests**: 9 new tests for native backend (8 pass, 1 skip)
- **Integration Tests**: 4 new tests comparing native vs CLI backend performance

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Build Warnings**: Recheck the target build log
- **ServiceContainer**: Service count is defined by the corresponding source tree
- **Plugins**: Plugin count is defined by the corresponding source tree

## Documentation

- Added `docs/PROJECT_OVERVIEW.md` - comprehensive project documentation
- Added `docs/TWINCAT_BENCHMARK_REVIEW.md` - TwinCAT comparison analysis
- Updated README.md with native API and dual-backend mode documentation
- Updated ARCHITECTURE.md with backend architecture details

## Release Packages

- Linux x86_64 tar.gz package
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio 3.7 Release Notes

**Release Date**: June 20, 2026

## Highlights

Architecture documentation update, boundary and integration test expansion, ServiceContainer concurrent access fix, and comprehensive test infrastructure improvements.

## New Tests

- **Boundary Tests**: Empty data and large data boundary tests for robustness verification
- **Integration Tests**: Plugin integration, service integration, and EventBus integration tests
- **Concurrent Access Tests**: Thread-safety verification for ServiceContainer and shared services
- **Error Recovery Tests**: Error detection, classification, and recovery workflow validation
- **UI Creation Tests**: Widget instantiation and lifecycle verification

## Bug Fixes

- Fixed ServiceContainer concurrent access issues in multi-threaded test scenarios
- Improved test isolation and fixture reliability

## Architecture Improvements

- Documented ServiceContainer single-EcatClient design pattern
- Documented layered error handling architecture
- Documented performance monitoring architecture with ring buffer strategy
- Documented plugin registration system with validation rules

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **ServiceContainer**: Service count is defined by the corresponding source tree
- **Plugins**: Plugin count is defined by the corresponding source tree

## Release Packages

- Linux x86_64 tar.gz package
- Linux DEB package
- Linux RPM package
- Linux AppImage
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio v3.6.0 Release Notes

**Release Date**: June 20, 2026

## Highlights

New optimization features for DC Sync, Free Run, PDO Mapping, and SDO communication with comprehensive service and plugin implementations.

## New Features

### DC Sync Optimization
- DC distributed clock synchronization optimization
- Sync optimization, drift optimization, jitter optimization, configuration optimization
- SyncOptimizationWidget and DriftOptimizationWidget for visual optimization
- DcSyncOptimizationService and DcSyncOptimizerService

### Free Run Optimization
- Free Run process data exchange optimization
- Cycle time optimization, data mapping optimization, performance optimization, error handling optimization
- CycleTimeOptimizerWidget and DataMappingOptimizerWidget for optimization control
- FreeRunOptimizationService

### PDO Mapping Optimization
- PDO mapping configuration optimization
- Mapping optimization, size optimization, alignment optimization, performance optimization
- MappingOptimizerWidget and SizeOptimizerWidget for mapping optimization analysis
- PdoMappingOptimizationService

### SDO Optimization
- SDO communication optimization
- Cache optimization, batch optimization, performance optimization, error handling optimization
- CacheOptimizerWidget and BatchOptimizerWidget for optimization management
- SdoOptimizationService

### Real-time Performance Optimization
- Real-time performance optimization with latency and throughput optimization
- LatencyOptimizerWidget and ThroughputOptimizerWidget
- RealtimeOptimizerService

## New Services

- **DcSyncOptimizationService**: DC sync optimization with sync, drift, jitter, and config optimization
- **DcSyncOptimizerService**: DC synchronization optimizer for distributed clock
- **FreeRunOptimizationService**: Free Run process data exchange optimization
- **PdoMappingOptimizationService**: PDO mapping configuration optimization
- **SdoOptimizationService**: SDO communication optimization with cache and batch optimization
- **RealtimeOptimizerService**: Real-time performance optimization with latency and throughput optimization

## New Plugins

- **DcSyncOptimizerPlugin**: DC synchronization optimization workspace
- **DcSyncOptimizationPlugin**: DC sync precision optimization workspace
- **FreeRunOptimizationPlugin**: Free Run optimization workspace
- **PdoMappingOptimizationPlugin**: PDO mapping optimization workspace
- **SdoOptimizationPlugin**: SDO optimization workspace
- **RealtimeOptimizerPlugin**: Real-time performance optimization workspace

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **ServiceContainer**: Service count is defined by the corresponding source tree
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages

## Release Packages

- Linux x86_64 tar.gz package
- Linux DEB package
- Linux RPM package
- Linux AppImage
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio v3.5.0 Release Notes

**Release Date**: June 20, 2026

## Highlights

Final release preparation with comprehensive release packages, quality assurance reports, and documentation.

## Release Packages

- **Linux tar.gz**: Binary package with launcher script
- **Linux DEB**: Debian/Ubuntu package with desktop integration
- **Linux RPM**: Fedora/RHEL package with spec file
- **Linux AppImage**: Portable single-file application
- **Source Code**: Complete source archive (tar.gz + zip)

## Documentation

- **Release Notes**: Detailed release notes for each version
- **Installation Guide**: Step-by-step installation instructions
- **User Manual**: Complete user guide for NekoEcat Studio
- **Developer Guide**: Architecture, build, and contribution guide

## Quality Assurance

- **Test Coverage**: Re-run coverage reporting for the target build
- **Performance**: Startup, memory, and I/O benchmarks
- **Memory Analysis**: Valgrind leak detection
- **Code Quality**: cppcheck, clang-tidy, formatting checks
- **Total Tests**: See the corresponding build output

## Verification

- Re-run a clean build and inspect warnings for the target build
- Rerun the full test suite for the target build
- Recreate and validate packages for the target release
- All documentation generated

---

# NekoEcat Studio v3.4.0 Release Notes

**Release Date**: June 20, 2026

## Highlights

New security, compliance, and certification manager services with comprehensive unit tests. Build fix for missing Manager service source files.

## New Services

- **WorkflowSecurityManagerService**: Security policy management with CRUD, enable/disable, and enforcement
- **WorkflowComplianceManagerService**: Compliance rule management with CRUD, activation, auditing, and category filtering
- **WorkflowCertificationManagerService**: Certification requirement management with CRUD, status tracking, and renewal

## New Tests

- **Unit Tests**: 3 new service unit tests (37 test cases total)
  - WorkflowSecurityManagerService: 12 test cases
  - WorkflowComplianceManagerService: 12 test cases
  - WorkflowCertificationManagerService: 13 test cases

## Bug Fixes

- Fixed build failure caused by missing WorkflowSecurityManagerService, WorkflowComplianceManagerService, and WorkflowCertificationManagerService source files

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages

---

# NekoEcat Studio v3.3.0 Release Notes

**Release Date**: June 20, 2026

## Highlights

Final testing, optimization, and release preparation with three new workflow services, comprehensive unit and performance tests, and release packaging.

## New Services

- **WorkflowVisualizationStudioService**: Multi-scene visualization studio with project management, scene creation, and rendering
- **WorkflowReportDesignerService**: Custom report template designer with template CRUD, report generation, and multi-format support
- **WorkflowDocumentationBrowserService**: Documentation browser with tree navigation, full-text search, and bookmark management

## New Tests

- **Unit Tests**: 3 new service unit tests (40 test cases total)
  - WorkflowVisualizationStudioService: 14 test cases
  - WorkflowReportDesignerService: 12 test cases
  - WorkflowDocumentationBrowserService: 14 test cases
- **Performance Tests**: 3 new performance tests (12 test cases total)
  - WorkflowVisualizationStudioService performance: 4 test cases
  - WorkflowReportDesignerService performance: 4 test cases
  - WorkflowDocumentationBrowserService performance: 4 test cases

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **New Unit Tests**: 3 test files
- **New Performance Tests**: 3 test files
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages

## Release Packages

- Linux x86_64 tar.gz package
- Linux DEB package
- Linux RPM package
- Linux AppImage
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio v3.2.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

Final testing, optimization, and release preparation with coverage to be revalidated for the target build.

## New Tests

- **Unit Tests**: 6 service unit tests for WorkflowDigitalTwinService, WorkflowBlockchainService, WorkflowQuantumService, WorkflowCloudService, WorkflowEdgeService, WorkflowAIService
- **Integration Tests**: 2 plugin integration tests for WorkflowOptimizerPlugin, WorkflowDashboardPlugin
- **Performance Tests**: 6 performance test cases for DigitalTwin, Blockchain, Quantum, Cloud, Edge, and AI services

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **New Unit Tests**: 6 test files
- **New Integration Tests**: 2 test files
- **New Performance Tests**: 6 test files
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages

## Release Packages

- Linux x86_64 tar.gz package
- Linux DEB package
- Linux RPM package
- Linux AppImage
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio v3.1.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

New workflow services for cloud, edge computing, and AI-powered optimization with tests and performance checks to be revalidated for the target build.

## New Services

- **WorkflowCloudService**: Cloud connectivity, synchronization, backup, and monitoring for workflow systems
- **WorkflowEdgeService**: Edge computing, data processing, analysis, and local storage for workflow systems
- **WorkflowAIService**: AI-powered predictive maintenance, anomaly detection, optimization, and pattern recognition

## New Tests

- **WorkflowCloudService Tests**: 9 unit test cases covering cloud connection, sync, backup, and monitoring
- **WorkflowEdgeService Tests**: 12 unit test cases covering node registration, data processing, analysis, and storage
- **WorkflowAIService Tests**: 12 unit test cases covering prediction, anomaly detection, optimization, and pattern recognition
- **Performance Tests**: 18 new performance test cases for cloud, edge, and AI services

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **New Unit Tests**: 33 test cases
- **New Performance Tests**: 18 test cases
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages

## Release Packages

- Linux x86_64 tar.gz package
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio v3.0.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

Major release with new workflow integration services, test coverage to revalidate for the target build, and performance optimizations.

## New Services

- **WorkflowIntegrationHubService**: Central hub for managing all workflow integrations including CI/CD, issue trackers, communication, and documentation systems
- **WorkflowSyncService**: Synchronizes workflow state, configurations, and data between local and remote systems
- **WorkflowReplicationService**: Replicates workflow configurations and state across multiple EtherCAT network instances

## New Tests

- **WorkflowIntegrationHubService Tests**: 12 unit test cases covering endpoint registration, connection management, and signal emissions
- **WorkflowSyncService Tests**: 9 unit test cases covering sync configuration, operations, and history tracking
- **WorkflowReplicationService Tests**: 12 unit test cases covering target management, replication jobs, and mode handling
- **Performance Tests**: 9 new performance test cases for integration hub, sync, and replication services

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **New Unit Tests**: 33 test cases
- **New Performance Tests**: 9 test cases
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages

## Release Packages

- Linux x86_64 tar.gz package
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio v2.9.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

Final testing, optimization, and release preparation with quality checks and release packaging to revalidate for the target build.

## Quality Assurance

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Unit Tests**: All service tests passing (security, compliance, certification, visualization, reporting, integration)
- **Integration Tests**: WorkflowOptimizerPlugin and WorkflowDashboardPlugin integration tests passing
- **Performance Tests**: Security, compliance, and certification performance tests passing
- **Release Packages**: Linux tar.gz, DEB, RPM, AppImage, and source code packages

## Release Packages

- Linux x86_64 tar.gz package
- Source code package
- SHA256 checksums included

---

# NekoEcat Studio v2.7.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

Final release preparation with additional performance tests for workflow analytics and monitoring services, version bump, and coverage to revalidate for the target build.

## New Features

### Performance Tests
- WorkflowAnalyticsService performance tests (6 test cases: recording throughput, analysis latency, signal throughput, memory stability)
- WorkflowMonitoringService performance tests (6 test cases: recording throughput, query latency, signal throughput, clear history throughput)

## Quality Statistics

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Performance Test Coverage**: All major services
- **Integration Test Coverage**: All workflow plugins

---

# NekoEcat Studio v2.6.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

Comprehensive performance testing for workflow services, plugin integration tests, and performance optimizations across startup, runtime, and memory usage.

## New Features

### Performance Tests
- WorkflowAutomationService performance tests (6 test cases: throughput, latency, memory stability)
- WorkflowOptimizationService performance tests (8 test cases: schedule/resource/parallel/dependency optimization)
- WorkflowSchedulingService performance tests (7 test cases: schedule/trigger/query throughput)

### Integration Tests
- WorkflowOptimizerPlugin integration test (9 test cases: designer-to-optimization pipeline, full lifecycle)
- WorkflowDashboardPlugin integration test (10 test cases: dashboard with optimization and monitoring services)

### Performance Optimizations
- Lazy initialization of all services for faster startup
- Parallel initialization of all plugins
- Connection pooling for all daemon communication
- Data caching for all SDO/PDO data
- Async operations for all slow tasks
- Batch operations for all multiple requests
- Object pooling for all frequent allocations
- Memory arenas for all temporary data

## Quality Statistics

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Performance Test Coverage**: All major services
- **Integration Test Coverage**: All workflow plugins

---

# NekoEcat Studio v2.5.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

Digital Twin, Blockchain, and Quantum Security services with unit, integration, and performance tests to revalidate for the target build.

## New Features

### Digital Twin Service (EtherCATDigitalTwinService)
- Digital twin modeling with node and connection management
- Snapshot creation and restoration
- Real-time twin state tracking

### Blockchain Service (EtherCATBlockchainService)
- Blockchain-based configuration audit trail
- Immutable logging with SHA-256 hashing
- Chain validation and integrity verification

### Quantum Security Service (EtherCATQuantumService)
- Quantum-resistant key pair generation
- Encryption, decryption, and integrity verification
- Security audit logging

### New Plugins
- **DigitalTwinStudioPlugin** — Digital twin visualization and management workspace
- **BlockchainExplorerPlugin** — Blockchain chain exploration and audit log management
- **QuantumSecurityPlugin** — Quantum key management and security audit workspace

### New Unit Tests
- EtherCATDigitalTwinService unit tests (18 test cases)
- EtherCATBlockchainService unit tests (14 test cases)
- EtherCATQuantumService unit tests (14 test cases)

### New Integration Tests
- DigitalTwinStudioPlugin integration tests (14 test cases)
- BlockchainExplorerPlugin integration tests (12 test cases)
- QuantumSecurityPlugin integration tests (12 test cases)

### New Performance Tests
- EtherCATDigitalTwinService performance tests (4 test cases)
- EtherCATBlockchainService performance tests (4 test cases)
- EtherCATQuantumService performance tests (4 test cases)

## Quality Statistics

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Performance Test Coverage**: All major services

---

# NekoEcat Studio v2.4.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

Cloud Manager, Edge Computing, and AI Assistant plugins with unit and performance tests to revalidate for the target build.

## New Features

### Cloud Manager Plugin
- Cloud connection management with table view
- Sync progress tracking
- Backup history with tree view
- Real-time monitoring dashboard
- Cloud report export

### Edge Computing Plugin
- Edge data management and processing
- Statistical analysis (mean, variance, min, max)
- Local storage and synchronization
- Edge report export

### AI Assistant Plugin
- Predictive maintenance with probability and timeframe
- Anomaly detection with severity classification
- Performance optimization suggestions
- Pattern recognition (monotonic, stable trends)
- AI report export

### New Unit Tests
- EtherCATCloudService unit tests (12 test cases)
- EtherCATEdgeService unit tests (10 test cases)
- EtherCATAIService unit tests (17 test cases)
- CloudManagerPlugin unit tests (13 test cases)
- EdgeComputingPlugin unit tests (9 test cases)
- AIAssistantPlugin unit tests (10 test cases)

### New Performance Tests
- EtherCATCloudService performance tests (4 test cases)
- EtherCATEdgeService performance tests (3 test cases)
- EtherCATAIService performance tests (4 test cases)

## Quality Statistics

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Performance Test Coverage**: All major services

---

# NekoEcat Studio v2.3.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

New Documentation Browser plugin, comprehensive performance tests for visualization, reporting, and documentation services, and full test coverage for all new components.

## New Features

### Documentation Browser Plugin
- Documentation tree navigation
- Content viewer with search
- Bookmark management
- Export capabilities

### New Performance Tests
- EtherCATVisualizationService performance tests (5 test cases)
- EtherCATReportingService performance tests (5 test cases)
- EtherCATDocumentationService performance tests (5 test cases)

### New Unit Tests
- DocumentationBrowserPlugin unit tests (16 test cases)

## Quality Statistics

- **Total Tests**: See the corresponding build output
- **Test Pass Rate**: Revalidate for the target build
- **Performance Test Coverage**: All major services

## Build Instructions

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
ctest --output-on-failure
```

## System Requirements

- Linux (kernel 4.19+)
- Qt 6.2+
- IgH EtherCAT Master 1.5+ (for hardware features)
- CMake 3.20+
- GCC 11+ or Clang 14+

---

# NekoEcat Studio v2.2.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

New Integration Hub, Sync Manager, and Replication Manager services with workspace plugins, plus comprehensive performance tests and optimizations.

## New Features

### Integration Hub (EtherCATIntegrationService)
- PLC, SCADA, MES, and ERP connectivity management
- Data synchronization across external systems
- Multi-platform integration support

### Sync Manager (EtherCATSyncService)
- Time, data, state, and configuration synchronization
- Sync status monitoring and history
- Network-wide sync coordination

### Replication Manager (EtherCATReplicationService)
- Multi-target configuration, data, state, and backup replication
- Replication history and status tracking
- Parallel replication to multiple targets

### New Performance Tests
- EtherCATIntegrationService performance tests (5 test cases)
- EtherCATSyncService performance tests (5 test cases)
- EtherCATReplicationService performance tests (5 test cases)

## Performance Improvements
- Throughput and latency optimizations across integration, sync, and replication services

## Quality
- 240+ tests covering unit, integration, performance, and UI

## Build

```bash
cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4
```

## System Requirements
- Linux (Ubuntu 22.04+, Fedora 38+, Arch Linux)
- Qt 6.2+ (6.5+ recommended)
- IgH EtherCAT Master 1.5+
- CMake 3.20+

---

# NekoEcat Studio v2.1.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

New Deployment, Update, and Maintenance services with workspace plugins, plus comprehensive unit, integration, and performance tests.

## New Features

### Deployment Management (EtherCATDeploymentService)
- Configuration deployment to remote targets
- Deployment rollback and status tracking
- Deployment history and logging

### Firmware Updates (EtherCATUpdateService)
- Firmware update checking for EtherCAT slaves
- Staged update with progress tracking
- Update cancellation and history

### Maintenance Scheduling (EtherCATMaintenanceService)
- Scheduled maintenance task management
- Task execution, cancellation, and status tracking
- Support for Cleanup, Diagnostic, Backup, and Calibration tasks

### New Plugins
- **UpdateManagerPlugin** — Firmware update management workspace
- **MaintenanceSchedulerPlugin** — Maintenance task scheduling workspace

### New Tests
- Unit tests for all new services (25+ test cases)
- Integration tests for all new plugins (20+ test cases)
- Performance tests for all new services (9 test cases)

## Quality
- 220+ tests covering unit, integration, performance, and UI

## Build

```bash
cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4
```

## System Requirements
- Linux (Ubuntu 22.04+, Fedora 38+, Arch Linux)
- Qt 6.2+ (6.5+ recommended)
- IgH EtherCAT Master 1.5+
- CMake 3.20+

---

# NekoEcat Studio v2.0.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

New Optimization, Monitoring, and Analytics Dashboard plugins with comprehensive integration tests, plus performance and memory optimization improvements.

## New Features

### New Plugins
- **OptimizationDashboardPlugin** — Optimization metrics, history, recommendations, and actions dashboard
- **MonitoringDashboardPlugin** — Monitoring metrics, alerts, events, and dashboards
- **AnalyticsDashboardPlugin** — Analytics metrics, trends, reports, and filters

### New Integration Tests
- Integration tests for all new dashboard plugins

## Performance Improvements
- **Startup Optimization**: Lazy initialization, parallel plugin initialization, cached UI initialization
- **Memory Optimization**: Object pooling, memory arenas, reference counting, weak references

## Quality
- 200+ tests covering unit, integration, performance, and UI

## Build

```bash
cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4
```

## System Requirements
- Linux (Ubuntu 22.04+, Fedora 38+, Arch Linux)
- Qt 6.2+ (6.5+ recommended)
- IgH EtherCAT Master 1.5+
- CMake 3.20+

---

# NekoEcat Studio v1.9.0 Release Notes

**Release Date**: June 19, 2026

## Highlights

New Security, Compliance, and Certification modules with workspace plugins, plus startup optimization, connection pooling, and data caching improvements.

## New Features

### Security Management (EtherCATSecurityService)
- Security policy management and auditing
- Security event tracking and reporting

### Compliance Checking (EtherCATComplianceService)
- Compliance rule management and checking
- Compliance report generation

### Certification Testing (EtherCATCertificationService)
- Certification requirement management and testing
- Certification report generation

### New Plugins
- **SecurityManagerPlugin** — Security policy management workspace
- **ComplianceCheckerPlugin** — Compliance checking workspace
- **CertificationManagerPlugin** — Certification management workspace

### New Unit Tests
- Unit tests for all new services

### New Integration Tests
- Integration tests for all new plugins

### New Performance Tests
- Performance tests for all new services

## Performance Improvements
- **Startup Optimization**: Lazy initialization for faster application startup
- **Connection Pooling**: TCP connection pooling for daemon communication
- **Data Caching**: Specialized SDO/PDO data caching with batch operations and prefetching

## Quality
- 190+ tests covering unit, integration, performance, and UI

## Build

```bash
cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4
```

## System Requirements
- Linux (Ubuntu 22.04+, Fedora 38+, Arch Linux)
- Qt 6.2+ (6.5+ recommended)
- IgH EtherCAT Master 1.5+
- CMake 3.20+

---

# NekoEcat Studio v1.8.0 Release Notes

**Release Date**: June 19, 2026

## New Features

### Configuration Editor (ConfigurationEditorPlugin)
- Visual editor for EtherCAT configuration profiles
- Add, remove, and edit configuration parameters
- Save, load, and validate configurations
- Import/export configuration files

### Network Analyzer (NetworkAnalyzerPlugin)
- Deep analysis of EtherCAT network traffic and topology
- Real-time statistics and analysis results
- Frame counting, error tracking, latency measurement
- Start/stop analysis with live updates

### System Monitor (SystemMonitorPlugin)
- Real-time monitoring of system resources and EtherCAT health
- CPU usage, memory usage, bus state tracking
- Health score calculation with recommendations
- Status labels for quick system overview

### New Unit Tests
- **EtherCATSimulationService** — 13 unit tests covering virtual slave management, simulation lifecycle, and state tracking
- **EtherCATTestingService** — 10 unit tests covering test suite execution and result validation
- **EtherCATValidationService** — 12 unit tests covering configuration, network, timing, and safety validation

### New Integration Tests
- **ConfigurationEditorPlugin** — 14 integration tests covering parameter management, save/load, and validation
- **NetworkAnalyzerPlugin** — 13 integration tests covering analysis lifecycle and statistics
- **SystemMonitorPlugin** — 13 integration tests covering monitoring lifecycle and metrics

### New Performance Tests
- **EtherCATSimulationService** — 4 performance tests for slave creation, state queries, and simulation cycles
- **EtherCATTestingService** — 4 performance tests for test suite execution throughput
- **EtherCATValidationService** — 4 performance tests for validation throughput

## Test Statistics
- **Total Tests**: See the corresponding build output
- **New Tests**: 76+ tests added in this release

## System Requirements
- Linux (Ubuntu 22.04+, Fedora 38+, Arch Linux)
- Qt 6.2+ (6.5+ recommended)
- IgH EtherCAT Master 1.5+
- CMake 3.20+

---

# NekoEcat Studio v1.7.0 Release Notes

**Release Date**: June 19, 2026

## New Features

### Configuration Management (EtherCATConfigService)
- Save, load, validate, and compare EtherCAT configurations
- Import/export configuration profiles
- Configuration validation with error and warning reporting

### Backup and Restore (EtherCATBackupService)
- Create and manage configuration backups
- Restore from any saved backup
- Export/import backups for portability
- Backup verification with checksum validation

### Error Recovery (EtherCATRecoveryService)
- Automated error detection and diagnosis
- Step-by-step recovery execution with progress tracking
- Configurable recovery actions with priority ordering

### Visual Workflow Designer (WorkflowDesignerPlugin)
- Visual canvas for designing EtherCAT commissioning workflows

### Test Suite Management (TestSuitePlugin)
- Create and manage test suites for EtherCAT system validation

### Deployment Management (DeploymentPlugin)
- Push configurations to EtherCAT networks

## Test Statistics
- **Total Tests**: See the corresponding build output
- **New Tests**: 56+ tests added in this release

## System Requirements
- Linux (Ubuntu 22.04+, Fedora 38+, Arch Linux)
- Qt 6.2+ (6.5+ recommended)
- IgH EtherCAT Master 1.5+
- CMake 3.20+

---

# NekoEcat Studio v1.6.0

**Release Date:** 2026-06-19

## 新功能 / New Features

### 新服务 / New Services

#### EtherCATOptimizerService
- 运行时 EtherCAT 操作优化服务
- 支持批量执行、数据缓存、预取和缓存预热
- 14 个单元测试覆盖所有功能

### 新服务单元测试 / New Service Unit Tests

#### EtherCATMonitorService
- EtherCAT 总线实时监控服务
- 流量、错误率、性能指标和健康状态监控
- 12 个单元测试覆盖所有功能

#### EtherCATAnalyzerService
- EtherCAT 协议分析服务
- 帧分析、错误分析、性能分析和趋势分析
- 12 个单元测试覆盖所有功能

### 新性能测试 / New Performance Tests

#### EtherCATMonitorService Performance
- 启动/停止、轮询、查询和监控开销吞吐量测试

#### EtherCATAnalyzerService Performance
- 帧、错误、性能、趋势和混合分析吞吐量测试

#### EtherCATOptimizerService Performance
- 批量执行、缓存、预取、失效、混合操作和预热吞吐量测试

## 测试统计 / Test Statistics

- 单元测试总数: 156
- 性能测试总数: 16+
- 集成测试总数: 30+

---

# NekoEcat Studio v1.5.0

**Release Date:** 2026-06-19

## 新功能 / New Features

### 新服务 / New Services

#### HotConnectService
- 管理 EtherCAT 从站热连接组
- 支持运行时动态添加/移除从站组
- 14 个单元测试覆盖所有功能

#### RedundancyService
- 管理 EtherCAT 网络冗余
- 监控主/备路径并处理故障切换
- 13 个单元测试覆盖所有功能

#### CableDiagnosticsService
- 执行 EtherCAT 网络电缆诊断
- 测试电缆质量、长度和检测故障
- 10 个单元测试覆盖所有功能

### 新服务单元测试 / New Service Unit Tests

#### StateMachineService
- EtherCAT 设备状态机转换管理
- 状态验证、历史跟踪和自动恢复
- 14 个单元测试覆盖所有功能

#### ErrorHandlingService
- 错误检测、分类、恢复和报告
- 错误历史记录和实时监控信号
- 13 个单元测试覆盖所有功能

### 新插件集成测试 / New Plugin Integration Tests

#### SimulationPlugin
- EtherCAT 总线仿真工作区插件
- 实时可视化、统计和结果导出
- 16 个集成测试

#### CalibrationPlugin
- EtherCAT 设备校准工作区插件
- 分步向导、数据收集和结果分析
- 17 个集成测试

#### DocumentationPlugin
- EtherCAT 文档浏览、搜索和注释
- 书签管理和全文搜索
- 19 个集成测试

### 性能测试 / Performance Tests

#### StateMachineService 性能测试
- 状态请求吞吐量测试
- 状态验证延迟测试
- 历史查询延迟测试
- 内存稳定性测试

#### ErrorHandlingService 性能测试
- 错误报告吞吐量测试
- 错误检测吞吐量测试
- 错误分类吞吐量测试
- 内存稳定性测试

#### DiagnosticReportService 性能测试
- 报告生成吞吐量测试
- 报告导出吞吐量测试
- CSV 导出吞吐量测试
- 内存稳定性测试

### 版本更新 / Version Updates
- CMakeLists.txt 版本更新至 1.4.0
- README.md 版本更新
- CHANGELOG.md 版本更新
- RELEASE_NOTES.md 版本更新

### 测试覆盖 / Test Coverage
- 150+ 个单元测试全部通过
- 覆盖：模型层、适配器、插件、服务、集成、性能和边界测试
- 新增 StateMachineService、ErrorHandlingService 单元测试
- 新增 SimulationPlugin、CalibrationPlugin、DocumentationPlugin 集成测试
- 新增 StateMachineService、ErrorHandlingService、DiagnosticReportService 性能测试

## 系统要求 / System Requirements
- Linux (Arch/Ubuntu/Debian)
- Qt6 (Core, Network, Widgets, Test)
- IgH EtherCAT Master
- CMake 3.20+
- C++20 编译器

## 安装 / Installation

```bash
# Arch Linux (AUR)
yay -NekoEcat-Studio

# 从源码构建 / Build from source
git clone https://github.com/NekoRain404/NekoEcat-Studio.git
cd NekoEcat-Studio
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
sudo cmake --install build
```

## 使用 / Usage

```bash
# 启动守护进程 / Start daemon
ecatd &

# 启动 GUI / Start GUI
ecat-studio
```

## 已知限制 / Known Limitations
- 仅支持 Linux 平台
- 需要 IgH EtherCAT Master 内核模块
- 部分功能需要 root 权限

## 下一步 / Next Steps
- 继续优化性能和用户体验
- 添加更多 TwinCAT 对标功能
- 完善文档和示例

---

**Full Changelog**: https://github.com/NekoRain404/NekoEcat-Studio/commits/v1.4.0
