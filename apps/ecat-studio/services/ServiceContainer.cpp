// =============================================================================
// ServiceContainer.cpp — Service initialization and dependency wiring
// =============================================================================
//
// This file implements the ServiceContainer constructor, which creates and
// wires together all application services. The initialization order is
// critical because services have dependencies on each other.
//
// Initialization Order:
//   1. EcatClient      — TCP connection to ecatd daemon (no dependencies)
//   2. EventBus         — Central event hub (no dependencies)
//   3. Core services    — SdoService, WatchService, TopologyService (depend on EcatClient)
//   4. Hardware services — DcSyncService, AlEventService (depend on EcatClient)
//   5. Monitoring       — PerformanceMonitorService, WatchdogService (depend on EventBus + EcatClient)
//   6. Composite services — DiagnosticReportService, EcatHealthService (depend on multiple services)
//   7. Data services    — EsiService, ConfigurationService, ProjectManagerService
//   8. Advanced services — Extended EtherCAT services, workflow services
//
// Dependency Injection Pattern:
//   Services receive their dependencies through constructor parameters:
//     - EcatClient*     — for daemon communication
//     - EventBus*       — for event publishing/subscribing
//     - Other services  — for cross-service coordination
//     - QObject* parent — for Qt ownership management
//
//   Example:
//     sdo_ = new SdoService(client_, this);
//     //     ^^^^^^^^^^^^   ^^^^^^^  ^^^^
//     //     |              |        |
//     //     Service type   Deps     Parent (ownership)
//
// Cleanup Sequence:
//   Qt's parent-child tree handles cleanup automatically. When the container
//   is destroyed, Qt deletes children in reverse construction order. This
//   ensures services that depend on others are cleaned up first.

#include "ServiceContainer.h"
#include "infra/EcatClient.h"
#include "EventBus.h"
#include "SdoService.h"
#include "WatchService.h"
#include "TopologyService.h"
#include "DcSyncService.h"
#include "AlEventService.h"
#include "SignalService.h"
#include "PerformanceMonitorService.h"
#include "EsiService.h"
#include "BusStatsService.h"
#include "WatchdogService.h"
#include "SafetyController.h"
#include "DiagnosticReportService.h"
#include "ProjectManagerService.h"
#include "ConfigurationService.h"
#include "AlarmService.h"
#include "LoggingService.h"
#include "ChartService.h"
#ifdef ECAT_SCRIPTING_ENABLED
#include "ScriptingService.h"
#endif
#include "BatchOperationService.h"
#include "NetworkDiagnosticsService.h"
#include "EcatHealthService.h"
#include "ExportService.h"
#include "DataPipelineService.h"
#include "DeviceManagerService.h"
#include "FirmwareUpdateService.h"
#include "ReportGeneratorService.h"
#include "MasterManagerService.h"
#include "DistributedClockService.h"
#include "DcSyncPrecisionService.h"
#include "DcSyncOptimizerService.h"
#include "SdoCacheService.h"
#include "PdoMappingService.h"
#include "CoEService.h"
#include "FoEService.h"
#include "EoEService.h"
#include "TraceService.h"
#include "DomainService.h"
#include "SyncManagerService.h"
#include "StateMachineService.h"
#include "ErrorHandlingService.h"
#include "HotConnectService.h"
#include "RedundancyService.h"
#include "EtherCATMonitorService.h"
#include "EtherCATAnalyzerService.h"
#include "EtherCATOptimizerService.h"
#include "EtherCATConfigService.h"
#include "EtherCATBackupService.h"
#include "EtherCATRecoveryService.h"
#include "EtherCATSimulationService.h"
#include "EtherCATTestingService.h"
#include "EtherCATValidationService.h"
#include "EtherCATSecurityService.h"
#include "EtherCATComplianceService.h"
#include "EtherCATCertificationService.h"
#include "EtherCATAnalyticsService.h"
#include "EtherCATDeploymentService.h"
#include "EtherCATUpdateService.h"
#include "EtherCATMaintenanceService.h"
#include "EtherCATIntegrationService.h"
#include "EtherCATVisualizationService.h"
#include "EtherCATReportingService.h"
#include "EtherCATDocumentationService.h"
#include "TaskManagementService.h"
#include "ResourceManagementService.h"
#include "WorkflowAutomationService.h"
#include "WorkflowSchedulingService.h"
#include "WorkflowMonitoringService.h"
#include "WorkflowVisualizationService.h"
#include "WorkflowReportingService.h"
#include "WorkflowSecurityService.h"
#ifdef ECAT_EXPERIMENTAL_SERVICES
#include "WorkflowComplianceService.h"
#include "WorkflowCertificationService.h"
#include "WorkflowDeploymentService.h"
#endif
#include "WorkflowUpdateService.h"
#include "WorkflowMaintenanceService.h"
#include "WorkflowVisualizationStudioService.h"
#include "WorkflowReportDesignerService.h"
#include "WorkflowDocumentationBrowserService.h"
#include "WorkflowSecurityManagerService.h"
#include "WorkflowComplianceManagerService.h"
#include "WorkflowCertificationManagerService.h"
#include "OnlineDiagnosticsService.h"
#include "MultiMasterService.h"
#include "RealtimePerformanceService.h"
#include "RealtimeOptimizerService.h"
#include "HardwareVerificationService.h"
#include "FreeRunOptimizationService.h"
#include "SdoOptimizationService.h"
#include "FreeRunConfigurationService.h"
#include "FreeRunMonitoringService.h"
#include "PdoConfigurationService.h"
#include "PdoMappingOptimizationService.h"
#include "OpStateService.h"
#include "plugins/oscilloscope/OscilloscopeService.h"
#include "plugins/protocol/ProtocolAnalyzerService.h"
#include "WorkflowAnalyticsService.h"

// =============================================================================
// ServiceContainer Constructor — Initialize all services in dependency order
// =============================================================================
//
// Services are created in a specific order to satisfy dependencies:
//   1. Infrastructure (EcatClient, EventBus) — no dependencies
//   2. Core services (Sdo, Watch, Topology) — depend on EcatClient
//   3. Hardware services (DcSync, AlEvent) — depend on EcatClient
//   4. Monitoring services — depend on EventBus + EcatClient
//   5. Composite services — depend on multiple services
//   6. Data services — mostly standalone
//   7. Advanced services — depend on EventBus + EcatClient
//
// All services use Qt's parent-child ownership model via `this` as parent.
// This ensures automatic cleanup when the container is destroyed.

ServiceContainer::ServiceContainer(EcatClient *client, EventBus *eventBus, QObject *parent)
    : QObject(parent)
    , client_(client)
    , eventBus_(eventBus)
{
    // ── Phase 1: Infrastructure (injected) ─────────────────────────────

    // ── Phase 2: Core services (depend on EcatClient) ─────────────────
    sdo_ = new SdoService(client_, this);     // SDO read/write operations
    watch_ = new WatchService(client_, this);
    topology_ = new TopologyService(client_, this);
    dcSync_ = new DcSyncService(client_, this);
    alEvent_ = new AlEventService(client_, this);
    signal_ = new SignalService(eventBus_, this);
    perfMonitor_ = new PerformanceMonitorService(eventBus_, client_, this);
    esi_ = new EsiService(this);
    busStats_ = new BusStatsService(client_, this);
    watchdog_ = new WatchdogService(eventBus_, client_, this);
    safety_ = new SafetyController(this);
    diagnosticReport_ = new DiagnosticReportService(
        eventBus_, client_, topology_, dcSync_, perfMonitor_, watchdog_, this);
    projectManager_ = new ProjectManagerService(this);
    configuration_ = new ConfigurationService(this);
    alarm_ = new AlarmService(this);
    logging_ = new LoggingService(this);
    chart_ = new ChartService(this);
#ifdef ECAT_SCRIPTING_ENABLED
    scripting_ = new ScriptingService(client_, sdo_, topology_, this);
#endif
    batch_ = new BatchOperationService(client_, sdo_, topology_, this);
    networkDiagnostics_ = new NetworkDiagnosticsService(client_, this);
    ecatHealth_ = new EcatHealthService(client_, eventBus_, topology_,
                                        dcSync_, alEvent_, watchdog_, this);
    exportService_ = new ExportService(this);
    dataPipeline_ = new DataPipelineService(this);
    deviceManager_ = new DeviceManagerService(client_, this);
    firmwareUpdate_ = new FirmwareUpdateService(client_, this);
    reportGenerator_ = new ReportGeneratorService(this);
    masterManager_ = new MasterManagerService(client_, this);
    distributedClock_ = new DistributedClockService(client_, this);
    dcSyncPrecision_ = new DcSyncPrecisionService(client_, eventBus_, this);
    dcSyncOptimizer_ = new DcSyncOptimizerService(client_, eventBus_, this);
    sdoCache_ = new SdoCacheService(this);
    pdoMapping_ = new PdoMappingService(this);
    coe_ = new CoEService(client_, this);
    foe_ = new FoEService(client_, this);
    eoe_ = new EoEService(client_, this);
    trace_ = new TraceService(this);
    domain_ = new DomainService(this);
    syncManager_ = new SyncManagerService(this);
    stateMachine_ = new StateMachineService(this);
    errorHandling_ = new ErrorHandlingService(this);
    hotConnect_ = new HotConnectService(this);
    redundancy_ = new RedundancyService(this);
    ecatMonitor_ = new EtherCATMonitorService(eventBus_, client_, this);
    ecatAnalyzer_ = new EtherCATAnalyzerService(eventBus_, client_, this);
    ecatOptimizer_ = new EtherCATOptimizerService(eventBus_, client_, this);
    ecatConfig_ = new EtherCATConfigService(this);
    ecatBackup_ = new EtherCATBackupService(this);
    ecatRecovery_ = new EtherCATRecoveryService(this);
    ecatSimulation_ = new EtherCATSimulationService(this);
    ecatTesting_ = new EtherCATTestingService(this);
    ecatValidation_ = new EtherCATValidationService(this);
    ecatSecurity_ = new EtherCATSecurityService(this);
    ecatCompliance_ = new EtherCATComplianceService(this);
    ecatCertification_ = new EtherCATCertificationService(this);
    ecatAnalytics_ = new EtherCATAnalyticsService(eventBus_, client_, this);
    ecatDeployment_ = new EtherCATDeploymentService(eventBus_, client_, this);
    ecatUpdate_ = new EtherCATUpdateService(eventBus_, client_, this);
    ecatMaintenance_ = new EtherCATMaintenanceService(eventBus_, client_, this);
    ecatIntegration_ = new EtherCATIntegrationService(eventBus_, client_, this);
    ecatVisualization_ = new EtherCATVisualizationService(eventBus_, client_, this);
    ecatReporting_ = new EtherCATReportingService(eventBus_, client_, this);
    ecatDocumentation_ = new EtherCATDocumentationService(eventBus_, client_, this);
    taskManagement_ = new TaskManagementService(this);
    resourceManagement_ = new ResourceManagementService(this);
    workflowAutomation_ = new WorkflowAutomationService(this);
    workflowScheduling_ = new WorkflowSchedulingService(this);
    workflowMonitoring_ = new WorkflowMonitoringService(this);
    workflowVisualization_ = new WorkflowVisualizationService(this);
    workflowReporting_ = new WorkflowReportingService(this);
    workflowSecurity_ = new WorkflowSecurityService(this);
#ifdef ECAT_EXPERIMENTAL_SERVICES
    workflowCompliance_ = new WorkflowComplianceService(this);
    workflowCertification_ = new WorkflowCertificationService(this);
    workflowDeployment_ = new WorkflowDeploymentService(this);
#endif
    workflowUpdate_ = new WorkflowUpdateService(this);
    workflowMaintenance_ = new WorkflowMaintenanceService(this);
    workflowVisualizationStudio_ = new WorkflowVisualizationStudioService(this);
    workflowReportDesigner_ = new WorkflowReportDesignerService(this);
    workflowDocumentationBrowser_ = new WorkflowDocumentationBrowserService(this);
    workflowSecurityManager_ = new WorkflowSecurityManagerService(this);
    workflowComplianceManager_ = new WorkflowComplianceManagerService(this);
    workflowCertificationManager_ = new WorkflowCertificationManagerService(this);
    onlineDiagnostics_ = new OnlineDiagnosticsService(client_, this);
    multiMaster_ = new MultiMasterService(client_, eventBus_, this);
    realtimePerformance_ = new RealtimePerformanceService(client_, this);
    realtimeOptimizer_ = new RealtimeOptimizerService(this);
    hardwareVerification_ = new HardwareVerificationService(client_, this);
    freeRunOptimization_ = new FreeRunOptimizationService(client_, eventBus_, this);
    sdoOptimization_ = new SdoOptimizationService(client_, eventBus_, this);
    freeRunConfig_ = new FreeRunConfigurationService(client_, eventBus_, this);
    freeRunMonitor_ = new FreeRunMonitoringService(client_, eventBus_, this);
    pdoConfiguration_ = new PdoConfigurationService(this);
    pdoMappingOptimization_ = new PdoMappingOptimizationService(this);
    opState_ = new OpStateService(this);
    oscilloscope_ = new OscilloscopeService(this);
    protocolAnalyzer_ = new ProtocolAnalyzerService(this);
    workflowAnalytics_ = new WorkflowAnalyticsService(this);

    // ── Phase 3: Validate critical services ────────────────────────────
    validateService(client_, "EcatClient");
    validateService(eventBus_, "EventBus");
    validateService(sdo_, "SdoService");
    validateService(watch_, "WatchService");
    validateService(topology_, "TopologyService");
    validateService(errorHandling_, "ErrorHandlingService");
    validateService(safety_, "SafetyController");

    initialized_ = initErrors_.isEmpty();
}

void ServiceContainer::validateService(QObject *service, const QString &name) {
    if (!service) {
        initErrors_.append(name);
        emit serviceInitFailed(name, "Service creation returned null");
    }
}
