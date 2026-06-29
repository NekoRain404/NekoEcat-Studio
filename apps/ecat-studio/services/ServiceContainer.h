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
//   │   EtherCATBackupService, EtherCATRecoveryService,                 │
//   │   EtherCATSimulationService, EtherCATTestingService,              │
//   │   EtherCATValidationService, EtherCATSecurityService,             │
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
class MasterApiService;
class DomainService;
class SyncManagerService;
class StateMachineService;
class ErrorHandlingService;
class DiagnosticService;
class HotConnectService;
class RedundancyService;
class CableDiagnosticsService;
class EtherCATMonitorService;
class EtherCATAnalyzerService;
class EtherCATOptimizerService;
class EtherCATConfigService;
class EtherCATBackupService;
class EtherCATRecoveryService;
class EtherCATSimulationService;
class EtherCATTestingService;
class EtherCATValidationService;
class EtherCATSecurityService;
class EtherCATComplianceService;
class EtherCATCertificationService;
class EtherCATAnalyticsService;
class EtherCATDeploymentService;
class EtherCATUpdateService;
class EtherCATMaintenanceService;
class EtherCATIntegrationService;
class EtherCATVisualizationService;
class EtherCATReportingService;
class EtherCATDocumentationService;
class ProjectManagementService;
class TaskManagementService;
class ResourceManagementService;
class ProjectPlanningService;
class ProjectTrackingService;
class ProjectReportingService;
class WorkflowAutomationService;
class WorkflowSchedulingService;
class WorkflowMonitoringService;
class WorkflowVisualizationService;
class WorkflowReportingService;
class WorkflowIntegrationService;
class WorkflowSecurityService;
#ifdef ECAT_EXPERIMENTAL_SERVICES
class WorkflowComplianceService;
class WorkflowCertificationService;
class WorkflowDeploymentService;
#endif
class WorkflowUpdateService;
class WorkflowMaintenanceService;
class WorkflowIntegrationHubService;
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
  SdoService *sdo() const { return sdo_; }
  WatchService *watch() const { return watch_; }
  TopologyService *topology() const { return topology_; }
  DcSyncService *dcSync() const { return dcSync_; }
  AlEventService *alEvent() const { return alEvent_; }
  SignalService *signal() const { return signal_; }
  PerformanceMonitorService *perfMonitor() const { return perfMonitor_; }
  EsiService *esi() const { return esi_; }
  BusStatsService *busStats() const { return busStats_; }
  WatchdogService *watchdog() const { return watchdog_; }
  SafetyController *safety() const { return safety_; }
  DiagnosticReportService *diagnosticReport() const { return diagnosticReport_; }
  ProjectManagerService *projectManager() const { return projectManager_; }
  ConfigurationService *configuration() const { return configuration_; }
  AlarmService *alarm() const { return alarm_; }
  LoggingService *logging() const { return logging_; }
  ChartService *chart() const { return chart_; }
#ifdef ECAT_SCRIPTING_ENABLED
  ScriptingService *scripting() const { return scripting_; }
#endif
  BatchOperationService *batch() const { return batch_; }
  NetworkDiagnosticsService *networkDiagnostics() const { return networkDiagnostics_; }
  EcatHealthService *ecatHealth() const { return ecatHealth_; }
  ExportService *exportService() const { return exportService_; }
  DataPipelineService *dataPipeline() const { return dataPipeline_; }
  DeviceManagerService *deviceManager() const { return deviceManager_; }
  FirmwareUpdateService *firmwareUpdate() const { return firmwareUpdate_; }
  ReportGeneratorService *reportGenerator() const { return reportGenerator_; }
  MasterManagerService *masterManager() const { return masterManager_; }
  DistributedClockService *distributedClock() const { return distributedClock_; }
  DcSyncPrecisionService *dcSyncPrecision() const { return dcSyncPrecision_; }
  DcSyncOptimizerService *dcSyncOptimizer() const { return dcSyncOptimizer_; }
  SdoCacheService *sdoCache() const { return sdoCache_; }
  PdoMappingService *pdoMapping() const { return pdoMapping_; }
  CoEService *coe() const { return coe_; }
  FoEService *foe() const { return foe_; }
  EoEService *eoe() const { return eoe_; }
  TraceService *trace() const { return trace_; }
  MasterApiService *masterApi() const { return masterApi_; }
  DomainService *domain() const { return domain_; }
  SyncManagerService *syncManager() const { return syncManager_; }
  StateMachineService *stateMachine() const { return stateMachine_; }
  ErrorHandlingService *errorHandling() const { return errorHandling_; }
  DiagnosticService *diagnostic() const { return diagnostic_; }
  HotConnectService *hotConnect() const { return hotConnect_; }
  RedundancyService *redundancy() const { return redundancy_; }
  CableDiagnosticsService *cableDiagnostics() const { return cableDiagnostics_; }
  EtherCATMonitorService *ecatMonitor() const { return ecatMonitor_; }
  EtherCATAnalyzerService *ecatAnalyzer() const { return ecatAnalyzer_; }
  EtherCATOptimizerService *ecatOptimizer() const { return ecatOptimizer_; }
  EtherCATConfigService *ecatConfig() const { return ecatConfig_; }
  EtherCATBackupService *ecatBackup() const { return ecatBackup_; }
  EtherCATRecoveryService *ecatRecovery() const { return ecatRecovery_; }
  EtherCATSimulationService *ecatSimulation() const { return ecatSimulation_; }
  EtherCATTestingService *ecatTesting() const { return ecatTesting_; }
  EtherCATValidationService *ecatValidation() const { return ecatValidation_; }
  EtherCATSecurityService *ecatSecurity() const { return ecatSecurity_; }
  EtherCATComplianceService *ecatCompliance() const { return ecatCompliance_; }
  EtherCATCertificationService *ecatCertification() const { return ecatCertification_; }
  EtherCATAnalyticsService *ecatAnalytics() const { return ecatAnalytics_; }
  EtherCATDeploymentService *ecatDeployment() const { return ecatDeployment_; }
  EtherCATUpdateService *ecatUpdate() const { return ecatUpdate_; }
  EtherCATMaintenanceService *ecatMaintenance() const { return ecatMaintenance_; }
  EtherCATIntegrationService *ecatIntegration() const { return ecatIntegration_; }
  EtherCATVisualizationService *ecatVisualization() const { return ecatVisualization_; }
  EtherCATReportingService *ecatReporting() const { return ecatReporting_; }
  EtherCATDocumentationService *ecatDocumentation() const { return ecatDocumentation_; }
  ProjectManagementService *projectManagement() const { return projectManagement_; }
  TaskManagementService *taskManagement() const { return taskManagement_; }
  ResourceManagementService *resourceManagement() const { return resourceManagement_; }
  ProjectPlanningService *projectPlanning() const { return projectPlanning_; }
  ProjectTrackingService *projectTracking() const { return projectTracking_; }
  ProjectReportingService *projectReporting() const { return projectReporting_; }
  WorkflowAutomationService *workflowAutomation() const { return workflowAutomation_; }
  WorkflowSchedulingService *workflowScheduling() const { return workflowScheduling_; }
  WorkflowMonitoringService *workflowMonitoring() const { return workflowMonitoring_; }
  WorkflowVisualizationService *workflowVisualization() const { return workflowVisualization_; }
  WorkflowReportingService *workflowReporting() const { return workflowReporting_; }
  WorkflowIntegrationService *workflowIntegration() const { return workflowIntegration_; }
  WorkflowSecurityService *workflowSecurity() const { return workflowSecurity_; }
#ifdef ECAT_EXPERIMENTAL_SERVICES
  WorkflowComplianceService *workflowCompliance() const { return workflowCompliance_; }
  WorkflowCertificationService *workflowCertification() const { return workflowCertification_; }
  WorkflowDeploymentService *workflowDeployment() const { return workflowDeployment_; }
#endif
  WorkflowUpdateService *workflowUpdate() const { return workflowUpdate_; }
  WorkflowMaintenanceService *workflowMaintenance() const { return workflowMaintenance_; }
  WorkflowIntegrationHubService *workflowIntegrationHub() const { return workflowIntegrationHub_; }
  WorkflowVisualizationStudioService *workflowVisualizationStudio() const { return workflowVisualizationStudio_; }
  WorkflowReportDesignerService *workflowReportDesigner() const { return workflowReportDesigner_; }
  WorkflowDocumentationBrowserService *workflowDocumentationBrowser() const { return workflowDocumentationBrowser_; }
  WorkflowSecurityManagerService *workflowSecurityManager() const { return workflowSecurityManager_; }
  WorkflowComplianceManagerService *workflowComplianceManager() const { return workflowComplianceManager_; }
  WorkflowCertificationManagerService *workflowCertificationManager() const { return workflowCertificationManager_; }
  OnlineDiagnosticsService *onlineDiagnostics() const { return onlineDiagnostics_; }
  MultiMasterService *multiMaster() const { return multiMaster_; }
  RealtimePerformanceService *realtimePerformance() const { return realtimePerformance_; }
  RealtimeOptimizerService *realtimeOptimizer() const { return realtimeOptimizer_; }
  HardwareVerificationService *hardwareVerification() const { return hardwareVerification_; }
  FreeRunOptimizationService *freeRunOptimization() const { return freeRunOptimization_; }
  SdoOptimizationService *sdoOptimization() const { return sdoOptimization_; }
  FreeRunConfigurationService *freeRunConfig() const { return freeRunConfig_; }
  FreeRunMonitoringService *freeRunMonitor() const { return freeRunMonitor_; }
  PdoConfigurationService *pdoConfiguration() const { return pdoConfiguration_; }
  PdoMappingOptimizationService *pdoMappingOptimization() const { return pdoMappingOptimization_; }
  OpStateService *opState() const { return opState_; }
  OscilloscopeService *oscilloscope() const { return oscilloscope_; }
  ProtocolAnalyzerService *protocolAnalyzer() const { return protocolAnalyzer_; }
  WorkflowAnalyticsService *workflowAnalytics() const { return workflowAnalytics_; }

signals:
  // Emitted when a service fails to initialize.
  // @param serviceName  Name of the service that failed
  // @param reason       Description of the failure
  void serviceInitFailed(const QString &serviceName, const QString &reason);

private:
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
  MasterApiService *masterApi_ = nullptr;
  DomainService *domain_ = nullptr;
  SyncManagerService *syncManager_ = nullptr;
  StateMachineService *stateMachine_ = nullptr;
  ErrorHandlingService *errorHandling_ = nullptr;
  DiagnosticService *diagnostic_ = nullptr;
  HotConnectService *hotConnect_ = nullptr;
  RedundancyService *redundancy_ = nullptr;
  CableDiagnosticsService *cableDiagnostics_ = nullptr;
  EtherCATMonitorService *ecatMonitor_ = nullptr;
  EtherCATAnalyzerService *ecatAnalyzer_ = nullptr;
  EtherCATOptimizerService *ecatOptimizer_ = nullptr;
  EtherCATConfigService *ecatConfig_ = nullptr;
  EtherCATBackupService *ecatBackup_ = nullptr;
  EtherCATRecoveryService *ecatRecovery_ = nullptr;
  EtherCATSimulationService *ecatSimulation_ = nullptr;
  EtherCATTestingService *ecatTesting_ = nullptr;
  EtherCATValidationService *ecatValidation_ = nullptr;
  EtherCATSecurityService *ecatSecurity_ = nullptr;
  EtherCATComplianceService *ecatCompliance_ = nullptr;
  EtherCATCertificationService *ecatCertification_ = nullptr;
  EtherCATAnalyticsService *ecatAnalytics_ = nullptr;
  EtherCATDeploymentService *ecatDeployment_ = nullptr;
  EtherCATUpdateService *ecatUpdate_ = nullptr;
  EtherCATMaintenanceService *ecatMaintenance_ = nullptr;
  EtherCATIntegrationService *ecatIntegration_ = nullptr;
  EtherCATVisualizationService *ecatVisualization_ = nullptr;
  EtherCATReportingService *ecatReporting_ = nullptr;
  EtherCATDocumentationService *ecatDocumentation_ = nullptr;
  ProjectManagementService *projectManagement_ = nullptr;
  TaskManagementService *taskManagement_ = nullptr;
  ResourceManagementService *resourceManagement_ = nullptr;
  ProjectPlanningService *projectPlanning_ = nullptr;
  ProjectTrackingService *projectTracking_ = nullptr;
  ProjectReportingService *projectReporting_ = nullptr;
  WorkflowAutomationService *workflowAutomation_ = nullptr;
  WorkflowSchedulingService *workflowScheduling_ = nullptr;
  WorkflowMonitoringService *workflowMonitoring_ = nullptr;
  WorkflowVisualizationService *workflowVisualization_ = nullptr;
  WorkflowReportingService *workflowReporting_ = nullptr;
  WorkflowIntegrationService *workflowIntegration_ = nullptr;
  WorkflowSecurityService *workflowSecurity_ = nullptr;
#ifdef ECAT_EXPERIMENTAL_SERVICES
  WorkflowComplianceService *workflowCompliance_ = nullptr;
  WorkflowCertificationService *workflowCertification_ = nullptr;
  WorkflowDeploymentService *workflowDeployment_ = nullptr;
#endif
  WorkflowUpdateService *workflowUpdate_ = nullptr;
  WorkflowMaintenanceService *workflowMaintenance_ = nullptr;
  WorkflowIntegrationHubService *workflowIntegrationHub_ = nullptr;
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

  // ── Initialization Tracking ──────────────────────────────────────
  bool initialized_ = false;
  QStringList initErrors_;
  void validateService(QObject *service, const QString &name);
};
