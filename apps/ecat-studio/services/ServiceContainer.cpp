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
#include "ChartService.h"
#ifdef ECAT_EXPERIMENTAL_SERVICES
#include "AlarmService.h"
#include "LoggingService.h"
#endif
#ifdef ECAT_SCRIPTING_ENABLED
#include "ScriptingService.h"
#endif
#include "BatchOperationService.h"
#include "NetworkDiagnosticsService.h"
#include "EcatHealthService.h"
#include "ExportService.h"
#include "FirmwareUpdateService.h"
#include "ReportGeneratorService.h"
#include "DcSyncPrecisionService.h"
#include "SdoCacheService.h"
#include "PdoMappingService.h"
#include "CoEService.h"
#include "FoEService.h"
#include "EoEService.h"
#include "StateMachineService.h"
#include "ErrorHandlingService.h"
#include "HotConnectService.h"
#include "RedundancyService.h"
#include "OnlineDiagnosticsService.h"
#include "RealtimePerformanceService.h"
#include "FreeRunConfigurationService.h"
#include "FreeRunMonitoringService.h"
#include "PdoConfigurationService.h"
#include "OpStateService.h"
#include "plugins/oscilloscope/OscilloscopeService.h"
#include "plugins/protocol/ProtocolAnalyzerService.h"

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
    chart_ = new ChartService(this);
#ifdef ECAT_EXPERIMENTAL_SERVICES
    alarm_ = new AlarmService(this);
    logging_ = new LoggingService(this);
#endif
#ifdef ECAT_SCRIPTING_ENABLED
    scripting_ = new ScriptingService(client_, sdo_, topology_, this);
#endif
    batch_ = new BatchOperationService(client_, sdo_, topology_, this);
    networkDiagnostics_ = new NetworkDiagnosticsService(client_, this);
    ecatHealth_ = new EcatHealthService(client_, eventBus_, topology_,
                                        dcSync_, alEvent_, watchdog_, this);
    exportService_ = new ExportService(this);
    firmwareUpdate_ = new FirmwareUpdateService(client_, this);
    reportGenerator_ = new ReportGeneratorService(this);
    dcSyncPrecision_ = new DcSyncPrecisionService(client_, eventBus_, this);
    sdoCache_ = new SdoCacheService(this);
    pdoMapping_ = new PdoMappingService(this);
    coe_ = new CoEService(client_, sdoCache_, this);
    foe_ = new FoEService(client_, this);
    eoe_ = new EoEService(client_, this);
    stateMachine_ = new StateMachineService(client_, this);
    errorHandling_ = new ErrorHandlingService(this);
    hotConnect_ = new HotConnectService(this);
    redundancy_ = new RedundancyService(client_, this);
    onlineDiagnostics_ = new OnlineDiagnosticsService(client_, this);
    realtimePerformance_ = new RealtimePerformanceService(client_, this);
    freeRunConfig_ = new FreeRunConfigurationService(client_, eventBus_, this);
    freeRunMonitor_ = new FreeRunMonitoringService(client_, eventBus_, this);
    pdoConfiguration_ = new PdoConfigurationService(this);
    opState_ = new OpStateService(this);
    oscilloscope_ = new OscilloscopeService(this);
    protocolAnalyzer_ = new ProtocolAnalyzerService(this);

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
