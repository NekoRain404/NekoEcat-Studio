#pragma once
// =============================================================================
// ServiceContainer — Central dependency injection container for NekoEcat Studio
// =============================================================================
//
// Overview:
//   ServiceContainer holds ALL service instances and the shared EcatClient that
//   the application needs. It acts as the single source of truth for service
//   lifetimes and the primary dependency injection mechanism for plugins.
//
// Ownership Model:
//   All service pointers and the EcatClient are created as QObject children of
//   this container. Qt's parent-child tree handles deletion automatically — no
//   manual cleanup, no smart pointers, no RAII wrappers needed. When the
//   container is destroyed, Qt recursively deletes all children in reverse
//   construction order.
//
// Lifetime:
//   A single ServiceContainer is created in MainWindow's constructor and lives
//   for the entire application lifetime. It is never replaced or rebuilt at
//   runtime. Services are initialized once during construction and remain valid
//   until application exit.
//
// Dependency Injection:
//   The container is passed to each WorkspacePlugin constructor so plugins can
//   access domain services without depending on MainWindow or EcatClient
//   directly. This keeps plugins decoupled from the GUI coordinator and allows
//   services to be mocked or replaced in tests.
//
//   Usage example:
//     // In a plugin constructor:
//     MyPlugin::MyPlugin(ServiceContainer *container, QObject *parent)
//         : container_(container) {
//         connect(container_->eventBus(), &EventBus::slaveChanged,
//                 this, &MyPlugin::onSlavesChanged);
//         container_->sdo()->upload(pos, idx, sub);
//     }
//
// Thread Safety:
//   ALL access to the container and its services is expected from the main
//   (GUI) thread only. Services that perform background I/O (EcatClient,
//   TopologyService, etc.) marshal their results back to the main thread via
//   Qt signals before reaching this container. Do NOT access service pointers
//   from worker threads.
//
// Service Categories (85+ services):
//   ┌─────────────────────────────────────────────────────────────────────┐
//   │ Core Services                                                       │
//   │   EcatClient, EventBus, SdoService, WatchService, TopologyService  │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Hardware Services                                                   │
//   │   DcSyncService, AlEventService, SignalService, BusStatsService    │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Monitoring Services                                                 │
//   │   PerformanceMonitorService, WatchdogService, EcatHealthService,   │
//   │   NetworkDiagnosticsService                                        │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Safety Services                                                     │
//   │   SafetyController, AlarmService                                   │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Data Services                                                       │
//   │   EsiService, ProjectManagerService, ConfigurationService,         │
//   │   DataPipelineService, DeviceManagerService                        │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Reporting Services                                                  │
//   │   DiagnosticReportService, LoggingService, ChartService,           │
//   │   ExportService, ReportGeneratorService                            │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Operations Services                                                 │
//   │   BatchOperationService, AsyncOperationManager, FirmwareUpdateService│
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Optional Services (compile-time guarded)                            │
//   │   ScriptingService (requires ECAT_SCRIPTING_ENABLED)               │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Workflow Services                                                   │
//   │   WorkflowOptimizationService, WorkflowAutomationService,          │
//   │   WorkflowSchedulingService, WorkflowMonitoringService            │
//   ├─────────────────────────────────────────────────────────────────────┤
//   │ Extended EtherCAT Services                                          │
//   │   EtherCATMonitorService, EtherCATAnalyzerService,                │
//   │   EtherCATOptimizerService, EtherCATConfigService,                │
//   │   EtherCATRecoveryService,               │
//   │   EtherCATSimulationService,             │
//   │   EtherCATSecurityService,               │
//   │   EtherCATComplianceService, EtherCATCertificationService         │
//   └─────────────────────────────────────────────────────────────────────┘

#include <QObject>
#include <QStringList>

class EcatClient;
class EventBus;
class SdoService;
class WatchService;
class TopologyService;
class DcSyncService;
class AlEventService;
class SignalService;
class PerformanceMonitorService;
class EsiService;
class BusStatsService;
class WatchdogService;
class SafetyController;
class DiagnosticReportService;
class ProjectManagerService;
class ConfigurationService;
class AlarmService;
class LoggingService;
class ChartService;
#ifdef ECAT_SCRIPTING_ENABLED
class ScriptingService;
#endif
class BatchOperationService;
class NetworkDiagnosticsService;
class EcatHealthService;
class ExportService;
class DataPipelineService;
class DeviceManagerService;
class FirmwareUpdateService;
class ReportGeneratorService;
class MasterManagerService;
class DistributedClockService;
class DcSyncPrecisionService;
class DcSyncOptimizerService;
class SdoCacheService;
class PdoMappingService;
class CoEService;
class FoEService;
class EoEService;
class TraceService;
class DomainService;
class SyncManagerService;
class StateMachineService;
class ErrorHandlingService;
class HotConnectService;
class RedundancyService;
class EtherCATMonitorService;
class EtherCATAnalyzerService;
class EtherCATOptimizerService;
class EtherCATConfigService;
class EtherCATRecoveryService;
class EtherCATSimulationService;
class EtherCATSecurityService;
class EtherCATComplianceService;
class EtherCATCertificationService;
class EtherCATAnalyticsService;
class EtherCATUpdateService;
class EtherCATMaintenanceService;
class EtherCATVisualizationService;
class EtherCATReportingService;
class EtherCATDocumentationService;
class TaskManagementService;
class ResourceManagementService;
class WorkflowAutomationService;
class WorkflowSchedulingService;
class WorkflowMonitoringService;
class WorkflowVisualizationService;
class WorkflowReportingService;
class WorkflowSecurityService;
#ifdef ECAT_EXPERIMENTAL_SERVICES
class WorkflowComplianceService;
class WorkflowDeploymentService;
#endif
class WorkflowMaintenanceService;
class WorkflowVisualizationStudioService;
class WorkflowReportDesignerService;
class WorkflowDocumentationBrowserService;
class WorkflowSecurityManagerService;
class WorkflowComplianceManagerService;
class WorkflowCertificationManagerService;
class OnlineDiagnosticsService;
class MultiMasterService;
class RealtimePerformanceService;
class RealtimeOptimizerService;
class HardwareVerificationService;
class FreeRunOptimizationService;
class SdoOptimizationService;
class FreeRunConfigurationService;
class FreeRunMonitoringService;
class PdoConfigurationService;
class PdoMappingOptimizationService;
class OpStateService;
class OscilloscopeService;
class ProtocolAnalyzerService;
class WorkflowAnalyticsService;

/// @brief Central dependency injection container for NekoEcat Studio services.
///
/// @details Owns all service instances as QObject children, providing a single
/// source of truth for service lifetimes and the primary dependency injection
/// mechanism for plugins. All services are created during construction in
/// dependency order. Qt's parent-child tree handles automatic deletion.
/// The container is passed to WorkspacePlugin constructors, keeping plugins
/// decoupled from MainWindow. All access is expected from the main (GUI) thread.
class ServiceContainer : public QObject {
  Q_OBJECT
public:
  // ── Construction ─────────────────────────────────────────────────────
  // Creates all services in dependency order. Services that depend on other
  // services receive their dependencies via constructor injection.
  // The parent parameter enables Qt's parent-child ownership model.
  explicit ServiceContainer(EcatClient *client, EventBus *eventBus, QObject *parent = nullptr);

  // ── Initialization Validation ────────────────────────────────────────
  // Returns true if all critical services were created successfully.
  // Call after construction to verify the container is healthy.
  bool isInitialized() const { return initialized_; }

  // Returns a list of services that failed to initialize.
  QStringList initializationErrors() const { return initErrors_; }

  // ── Core Service Accessors ───────────────────────────────────────────
  // These provide access to the fundamental services that most plugins need.
  // Each accessor returns a non-owning pointer; the container owns all instances.

  /// TCP client to ecatd daemon. All bus communication goes through this.
  EcatClient *client() const { return client_; }

  /// Central event bus for inter-plugin communication (pub/sub).
  EventBus *eventBus() const { return eventBus_; }
  /// CoE SDO upload/download operations.
  SdoService *sdo() const { return sdo_; }
  /// CoE watchdog monitoring and configuration.
  WatchService *watch() const { return watch_; }
  /// EtherCAT bus topology discovery and management.
  TopologyService *topology() const { return topology_; }
  /// Distributed clock synchronization configuration and monitoring.
  DcSyncService *dcSync() const { return dcSync_; }
  /// AL event monitoring and notification.
  AlEventService *alEvent() const { return alEvent_; }
  /// Signal management and routing.
  SignalService *signal() const { return signal_; }
  /// Performance monitoring and metrics collection.
  PerformanceMonitorService *perfMonitor() const { return perfMonitor_; }
  /// ESI file parsing, validation, and device database management.
  EsiService *esi() const { return esi_; }
  /// EtherCAT bus statistics collection.
  BusStatsService *busStats() const { return busStats_; }
  /// Watchdog timer configuration and monitoring.
  WatchdogService *watchdog() const { return watchdog_; }
  /// Functional safety (FSoE) controller interface.
  SafetyController *safety() const { return safety_; }
  /// Diagnostic report generation and management.
  DiagnosticReportService *diagnosticReport() const { return diagnosticReport_; }
  /// Project file save/load and lifecycle management.
  ProjectManagerService *projectManager() const { return projectManager_; }
  /// Application-wide configuration storage and retrieval.
  ConfigurationService *configuration() const { return configuration_; }
  /// Alarm management and notification.
  AlarmService *alarm() const { return alarm_; }
  /// Application logging service.
  LoggingService *logging() const { return logging_; }
  /// Chart and plot generation service.
  ChartService *chart() const { return chart_; }
#ifdef ECAT_SCRIPTING_ENABLED
  /// Lua/Python scripting engine (compile-time optional).
  ScriptingService *scripting() const { return scripting_; }
#endif
  /// Batch operation execution and queuing.
  BatchOperationService *batch() const { return batch_; }
  /// Network diagnostics and link quality monitoring.
  NetworkDiagnosticsService *networkDiagnostics() const { return networkDiagnostics_; }
  /// EtherCAT network health monitoring.
  EcatHealthService *ecatHealth() const { return ecatHealth_; }
  /// Data export to file or external formats.
  ExportService *exportService() const { return exportService_; }
  /// Data pipeline processing and transformation.
  DataPipelineService *dataPipeline() const { return dataPipeline_; }
  /// Device manager for slave configuration.
  DeviceManagerService *deviceManager() const { return deviceManager_; }
  /// Firmware update via FoE or vendor protocol.
  FirmwareUpdateService *firmwareUpdate() const { return firmwareUpdate_; }
  /// Report generation service.
  ReportGeneratorService *reportGenerator() const { return reportGenerator_; }
  /// Master manager for multi-master configurations.
  MasterManagerService *masterManager() const { return masterManager_; }
  /// Distributed clock configuration and status.
  DistributedClockService *distributedClock() const { return distributedClock_; }
  /// DC synchronization precision monitoring.
  DcSyncPrecisionService *dcSyncPrecision() const { return dcSyncPrecision_; }
  /// DC synchronization optimization.
  DcSyncOptimizerService *dcSyncOptimizer() const { return dcSyncOptimizer_; }
  /// SDO cache service for optimized repeated access.
  SdoCacheService *sdoCache() const { return sdoCache_; }
  /// PDO mapping configuration and validation.
  PdoMappingService *pdoMapping() const { return pdoMapping_; }
  /// CoE (CANopen over EtherCAT) protocol service.
  CoEService *coe() const { return coe_; }
  /// FoE (File over EtherCAT) protocol service.
  FoEService *foe() const { return foe_; }
  /// EoE (Ethernet over EtherCAT) protocol service.
  EoEService *eoe() const { return eoe_; }
  /// Trace service for diagnostic tracing.
  TraceService *trace() const { return trace_; }
  /// Domain management for process data domains.
  DomainService *domain() const { return domain_; }
  /// Sync manager configuration service.
  SyncManagerService *syncManager() const { return syncManager_; }
  /// State machine management (bus state transitions).
  StateMachineService *stateMachine() const { return stateMachine_; }
  /// Error handling and recovery service.
  ErrorHandlingService *errorHandling() const { return errorHandling_; }
  /// Hot-connect (plug-and-play) support.
  HotConnectService *hotConnect() const { return hotConnect_; }
  /// Redundancy (cable/redundant master) management.
  RedundancyService *redundancy() const { return redundancy_; }
  /// EtherCAT bus monitoring service.
  EtherCATMonitorService *ecatMonitor() const { return ecatMonitor_; }
  /// EtherCAT protocol analyzer.
  EtherCATAnalyzerService *ecatAnalyzer() const { return ecatAnalyzer_; }
  /// EtherCAT network optimization.
  EtherCATOptimizerService *ecatOptimizer() const { return ecatOptimizer_; }
  /// EtherCAT configuration management.
  EtherCATConfigService *ecatConfig() const { return ecatConfig_; }
  /// EtherCAT error recovery service.
  EtherCATRecoveryService *ecatRecovery() const { return ecatRecovery_; }
  /// EtherCAT network simulation.
  EtherCATSimulationService *ecatSimulation() const { return ecatSimulation_; }
  /// EtherCAT security service.
  EtherCATSecurityService *ecatSecurity() const { return ecatSecurity_; }
  /// EtherCAT compliance checking.
  EtherCATComplianceService *ecatCompliance() const { return ecatCompliance_; }
  /// EtherCAT certification support.
  EtherCATCertificationService *ecatCertification() const { return ecatCertification_; }
  /// EtherCAT analytics and telemetry.
  EtherCATAnalyticsService *ecatAnalytics() const { return ecatAnalytics_; }
  /// EtherCAT update/firmware management.
  EtherCATUpdateService *ecatUpdate() const { return ecatUpdate_; }
  /// EtherCAT maintenance service.
  EtherCATMaintenanceService *ecatMaintenance() const { return ecatMaintenance_; }
  /// EtherCAT visualization service.
  EtherCATVisualizationService *ecatVisualization() const { return ecatVisualization_; }
  /// EtherCAT reporting service.
  EtherCATReportingService *ecatReporting() const { return ecatReporting_; }
  /// EtherCAT documentation service.
  EtherCATDocumentationService *ecatDocumentation() const { return ecatDocumentation_; }
  /// Task management for scheduled operations.
  TaskManagementService *taskManagement() const { return taskManagement_; }
  /// Resource management for bus resources.
  ResourceManagementService *resourceManagement() const { return resourceManagement_; }
  /// Workflow automation service.
  WorkflowAutomationService *workflowAutomation() const { return workflowAutomation_; }
  /// Workflow scheduling service.
  WorkflowSchedulingService *workflowScheduling() const { return workflowScheduling_; }
  /// Workflow monitoring service.
  WorkflowMonitoringService *workflowMonitoring() const { return workflowMonitoring_; }
  /// Workflow visualization service.
  WorkflowVisualizationService *workflowVisualization() const { return workflowVisualization_; }
  /// Workflow reporting service.
  WorkflowReportingService *workflowReporting() const { return workflowReporting_; }
  /// Workflow security service.
  WorkflowSecurityService *workflowSecurity() const { return workflowSecurity_; }
#ifdef ECAT_EXPERIMENTAL_SERVICES
  #ifdef ECAT_EXPERIMENTAL_SERVICES
  /// Workflow compliance checking (experimental).
  WorkflowComplianceService *workflowCompliance() const { return workflowCompliance_; }
  /// Workflow deployment service (experimental).
  WorkflowDeploymentService *workflowDeployment() const { return workflowDeployment_; }
#endif
#endif
  /// Workflow maintenance service.
  WorkflowMaintenanceService *workflowMaintenance() const { return workflowMaintenance_; }
  /// Workflow visualization studio service.
  WorkflowVisualizationStudioService *workflowVisualizationStudio() const { return workflowVisualizationStudio_; }
  /// Workflow report designer service.
  WorkflowReportDesignerService *workflowReportDesigner() const { return workflowReportDesigner_; }
  /// Workflow documentation browser service.
  WorkflowDocumentationBrowserService *workflowDocumentationBrowser() const { return workflowDocumentationBrowser_; }
  /// Workflow security manager service.
  WorkflowSecurityManagerService *workflowSecurityManager() const { return workflowSecurityManager_; }
  /// Workflow compliance manager service.
  WorkflowComplianceManagerService *workflowComplianceManager() const { return workflowComplianceManager_; }
  /// Workflow certification manager service.
  WorkflowCertificationManagerService *workflowCertificationManager() const { return workflowCertificationManager_; }
  /// Online diagnostics service.
  OnlineDiagnosticsService *onlineDiagnostics() const { return onlineDiagnostics_; }
  /// Multi-master coordination service.
  MultiMasterService *multiMaster() const { return multiMaster_; }
  /// Realtime performance monitoring.
  RealtimePerformanceService *realtimePerformance() const { return realtimePerformance_; }
  /// Realtime performance optimization.
  RealtimeOptimizerService *realtimeOptimizer() const { return realtimeOptimizer_; }
  /// Hardware verification service.
  HardwareVerificationService *hardwareVerification() const { return hardwareVerification_; }
  /// FreeRun optimization service.
  FreeRunOptimizationService *freeRunOptimization() const { return freeRunOptimization_; }
  /// SDO optimization service.
  SdoOptimizationService *sdoOptimization() const { return sdoOptimization_; }
  /// FreeRun configuration service.
  FreeRunConfigurationService *freeRunConfig() const { return freeRunConfig_; }
  /// FreeRun monitoring service.
  FreeRunMonitoringService *freeRunMonitor() const { return freeRunMonitor_; }
  /// PDO configuration service.
  PdoConfigurationService *pdoConfiguration() const { return pdoConfiguration_; }
  /// PDO mapping optimization service.
  PdoMappingOptimizationService *pdoMappingOptimization() const { return pdoMappingOptimization_; }
  /// Operational state service.
  OpStateService *opState() const { return opState_; }
  /// Oscilloscope service for signal capture.
  OscilloscopeService *oscilloscope() const { return oscilloscope_; }
  /// Protocol analyzer service.
  ProtocolAnalyzerService *protocolAnalyzer() const { return protocolAnalyzer_; }
  /// Workflow analytics service.
  WorkflowAnalyticsService *workflowAnalytics() const { return workflowAnalytics_; }

signals:
  /// Emitted when a service fails to initialize.
  /// @param serviceName  Name of the service that failed
  /// @param reason       Description of the failure
  void serviceInitFailed(const QString &serviceName, const QString &reason);

private:
  /// @name Service Instance Pointers
  /// Non-owning pointers to all created service instances.
  /// Owned as QObject children; Qt handles their lifetime via the parent-child
  /// tree. Each pointer corresponds to the public accessor above it.
  ///@{
  EcatClient *client_ = nullptr;
  EventBus *eventBus_ = nullptr;
  SdoService *sdo_ = nullptr;
  WatchService *watch_ = nullptr;
  TopologyService *topology_ = nullptr;
  DcSyncService *dcSync_ = nullptr;
  AlEventService *alEvent_ = nullptr;
  SignalService *signal_ = nullptr;
  PerformanceMonitorService *perfMonitor_ = nullptr;
  EsiService *esi_ = nullptr;
  BusStatsService *busStats_ = nullptr;
  WatchdogService *watchdog_ = nullptr;
  SafetyController *safety_ = nullptr;
  DiagnosticReportService *diagnosticReport_ = nullptr;
  ProjectManagerService *projectManager_ = nullptr;
  ConfigurationService *configuration_ = nullptr;
  AlarmService *alarm_ = nullptr;
  LoggingService *logging_ = nullptr;
  ChartService *chart_ = nullptr;
#ifdef ECAT_SCRIPTING_ENABLED
  ScriptingService *scripting_ = nullptr;
#endif
  BatchOperationService *batch_ = nullptr;
  NetworkDiagnosticsService *networkDiagnostics_ = nullptr;
  EcatHealthService *ecatHealth_ = nullptr;
  ExportService *exportService_ = nullptr;
  DataPipelineService *dataPipeline_ = nullptr;
  DeviceManagerService *deviceManager_ = nullptr;
  FirmwareUpdateService *firmwareUpdate_ = nullptr;
  ReportGeneratorService *reportGenerator_ = nullptr;
  MasterManagerService *masterManager_ = nullptr;
  DistributedClockService *distributedClock_ = nullptr;
  DcSyncPrecisionService *dcSyncPrecision_ = nullptr;
  DcSyncOptimizerService *dcSyncOptimizer_ = nullptr;
  SdoCacheService *sdoCache_ = nullptr;
  PdoMappingService *pdoMapping_ = nullptr;
  CoEService *coe_ = nullptr;
  FoEService *foe_ = nullptr;
  EoEService *eoe_ = nullptr;
  TraceService *trace_ = nullptr;
  DomainService *domain_ = nullptr;
  SyncManagerService *syncManager_ = nullptr;
  StateMachineService *stateMachine_ = nullptr;
  ErrorHandlingService *errorHandling_ = nullptr;
  HotConnectService *hotConnect_ = nullptr;
  RedundancyService *redundancy_ = nullptr;
  EtherCATMonitorService *ecatMonitor_ = nullptr;
  EtherCATAnalyzerService *ecatAnalyzer_ = nullptr;
  EtherCATOptimizerService *ecatOptimizer_ = nullptr;
  EtherCATConfigService *ecatConfig_ = nullptr;
  EtherCATRecoveryService *ecatRecovery_ = nullptr;
  EtherCATSimulationService *ecatSimulation_ = nullptr;
  EtherCATSecurityService *ecatSecurity_ = nullptr;
  EtherCATComplianceService *ecatCompliance_ = nullptr;
  EtherCATCertificationService *ecatCertification_ = nullptr;
  EtherCATAnalyticsService *ecatAnalytics_ = nullptr;
  EtherCATUpdateService *ecatUpdate_ = nullptr;
  EtherCATMaintenanceService *ecatMaintenance_ = nullptr;
  EtherCATVisualizationService *ecatVisualization_ = nullptr;
  EtherCATReportingService *ecatReporting_ = nullptr;
  EtherCATDocumentationService *ecatDocumentation_ = nullptr;
  TaskManagementService *taskManagement_ = nullptr;
  ResourceManagementService *resourceManagement_ = nullptr;
  WorkflowAutomationService *workflowAutomation_ = nullptr;
  WorkflowSchedulingService *workflowScheduling_ = nullptr;
  WorkflowMonitoringService *workflowMonitoring_ = nullptr;
  WorkflowVisualizationService *workflowVisualization_ = nullptr;
  WorkflowReportingService *workflowReporting_ = nullptr;
  WorkflowSecurityService *workflowSecurity_ = nullptr;
#ifdef ECAT_EXPERIMENTAL_SERVICES
  #ifdef ECAT_EXPERIMENTAL_SERVICES
  WorkflowComplianceService *workflowCompliance_ = nullptr;
  WorkflowDeploymentService *workflowDeployment_ = nullptr;
#endif
#endif
  WorkflowMaintenanceService *workflowMaintenance_ = nullptr;
  WorkflowVisualizationStudioService *workflowVisualizationStudio_ = nullptr;
  WorkflowReportDesignerService *workflowReportDesigner_ = nullptr;
  WorkflowDocumentationBrowserService *workflowDocumentationBrowser_ = nullptr;
  WorkflowSecurityManagerService *workflowSecurityManager_ = nullptr;
  WorkflowComplianceManagerService *workflowComplianceManager_ = nullptr;
  WorkflowCertificationManagerService *workflowCertificationManager_ = nullptr;
  OnlineDiagnosticsService *onlineDiagnostics_ = nullptr;
  MultiMasterService *multiMaster_ = nullptr;
  RealtimePerformanceService *realtimePerformance_ = nullptr;
  RealtimeOptimizerService *realtimeOptimizer_ = nullptr;
  HardwareVerificationService *hardwareVerification_ = nullptr;
  FreeRunOptimizationService *freeRunOptimization_ = nullptr;
  SdoOptimizationService *sdoOptimization_ = nullptr;
  FreeRunConfigurationService *freeRunConfig_ = nullptr;
  FreeRunMonitoringService *freeRunMonitor_ = nullptr;
  PdoConfigurationService *pdoConfiguration_ = nullptr;
  PdoMappingOptimizationService *pdoMappingOptimization_ = nullptr;
  OpStateService *opState_ = nullptr;
  OscilloscopeService *oscilloscope_ = nullptr;
  ProtocolAnalyzerService *protocolAnalyzer_ = nullptr;
  WorkflowAnalyticsService *workflowAnalytics_ = nullptr;
  ///@}

  // ── Initialization Tracking ──────────────────────────────────────
  /// Flag indicating whether all critical services initialized successfully.
  bool initialized_ = false;
  /// List of human-readable error messages from services that failed to init.
  QStringList initErrors_;
  /// Checks whether @p service was created non-null; records errors if not.
  /// @param service  The newly-constructed service pointer to validate.
  /// @param name     Human-readable name of the service (for error messages).
  void validateService(QObject *service, const QString &name);
};
