# NekoEcat Studio — Qt6 Desktop GUI
#
# Directory layout:
#   models/      Pure data/logic types (no widget dependencies)
#   adapters/    Bridge between models and QTableWidget
#   detail/    Detail panel text builders for each workspace
#   utils/     Reusable utilities (table, text, UI, docs)
#   infra/       Client, settings, shared types
#   workspaces/  MainWindow partial implementations per workspace
#   (root)       MainWindow core, main entry point

add_executable(ecat-studio
    # ── Plugins ───────────────────────────────────────────────────────
    plugins/PluginRegistry.h                 plugins/PluginRegistry.cpp
    plugins/notes/NotesPlugin.h              plugins/notes/NotesPlugin.cpp
    plugins/dcsync/DcSyncPlugin.h                plugins/dcsync/DcSyncPlugin.cpp
    plugins/alevent/AlEventPlugin.h            plugins/alevent/AlEventPlugin.cpp
    plugins/signal/SignalPlugin.h              plugins/signal/SignalPlugin.cpp
    plugins/signal/SignalChartWidget.h         plugins/signal/SignalChartWidget.cpp
    plugins/WorkspacePlugin.h
    plugins/esi/EsiPlugin.h                     plugins/esi/EsiPlugin.cpp
    plugins/esi/EsiBrowserPlugin.h              plugins/esi/EsiBrowserPlugin.cpp
    plugins/esi/EsiParser.h                     plugins/esi/EsiParser.cpp
    plugins/esi/EsiDeviceMatcher.h              plugins/esi/EsiDeviceMatcher.cpp
    plugins/busstats/BusStatsPlugin.h           plugins/busstats/BusStatsPlugin.cpp
    plugins/oscilloscope/OscilloscopePlugin.h   plugins/oscilloscope/OscilloscopePlugin.cpp
    plugins/oscilloscope/OscilloscopeWidget.h   plugins/oscilloscope/OscilloscopeWidget.cpp
    plugins/oscilloscope/OscilloscopeService.h  plugins/oscilloscope/OscilloscopeService.cpp
    plugins/protocol/ProtocolAnalyzerPlugin.h   plugins/protocol/ProtocolAnalyzerPlugin.cpp
    plugins/protocol/ProtocolAnalyzerService.h  plugins/protocol/ProtocolAnalyzerService.cpp
    plugins/project/ProjectPlugin.h             plugins/project/ProjectPlugin.cpp
    plugins/alarm/AlarmPlugin.h                 plugins/alarm/AlarmPlugin.cpp
    plugins/automation/AutomationPlugin.h       plugins/automation/AutomationPlugin.cpp
    plugins/chart/ChartPlugin.h                  plugins/chart/ChartPlugin.cpp
    plugins/chart/EcatChartWidget.h              plugins/chart/EcatChartWidget.cpp
    plugins/dashboard/DashboardPlugin.h          plugins/dashboard/DashboardPlugin.cpp
    plugins/network/NetworkDiagnosticsPlugin.h   plugins/network/NetworkDiagnosticsPlugin.cpp
    plugins/master/MasterManagerPlugin.h         plugins/master/MasterManagerPlugin.cpp
    plugins/sdocache/SdoCachePlugin.h           plugins/sdocache/SdoCachePlugin.cpp
    plugins/themecustomizer/ThemeCustomizerPlugin.h plugins/themecustomizer/ThemeCustomizerPlugin.cpp
    plugins/shortcuts/KeyboardShortcutsPlugin.h plugins/shortcuts/KeyboardShortcutsPlugin.cpp
    plugins/preferences/UserPreferencesPlugin.h plugins/preferences/UserPreferencesPlugin.cpp
    plugins/trace/TracePlugin.h                plugins/trace/TracePlugin.cpp
    plugins/logicanalyzer/LogicAnalyzerPlugin.h plugins/logicanalyzer/LogicAnalyzerPlugin.cpp
    plugins/diagram/DiagramPlugin.h          plugins/diagram/DiagramPlugin.cpp
    plugins/diagram/DiagramWidget.h          plugins/diagram/DiagramWidget.cpp
    plugins/formula/FormulaPlugin.h           plugins/formula/FormulaPlugin.cpp
    plugins/scriptlibrary/ScriptLibraryPlugin.h plugins/scriptlibrary/ScriptLibraryPlugin.cpp
    plugins/simulation/SimulationPlugin.h       plugins/simulation/SimulationPlugin.cpp
    plugins/calibration/CalibrationPlugin.h     plugins/calibration/CalibrationPlugin.cpp
    plugins/documentation/DocumentationPlugin.h plugins/documentation/DocumentationPlugin.cpp
    plugins/wizard/WizardPlugin.h              plugins/wizard/WizardPlugin.cpp
    plugins/template/TemplatePlugin.h          plugins/template/TemplatePlugin.cpp
    plugins/report/ReportPlugin.h              plugins/report/ReportPlugin.cpp
    plugins/dashboarddesigner/DashboardDesignerPlugin.h plugins/dashboarddesigner/DashboardDesignerPlugin.cpp
    plugins/alarmmanager/AlarmManagerPlugin.h    plugins/alarmmanager/AlarmManagerPlugin.cpp
    plugins/datalogger/DataLoggerPlugin.h        plugins/datalogger/DataLoggerPlugin.cpp
    plugins/workflowdesigner/WorkflowDesignerPlugin.h plugins/workflowdesigner/WorkflowDesignerPlugin.cpp
    plugins/testsuite/TestSuitePlugin.h          plugins/testsuite/TestSuitePlugin.cpp
    plugins/deployment/DeploymentPlugin.h        plugins/deployment/DeploymentPlugin.cpp
    plugins/configeditor/ConfigurationEditorPlugin.h plugins/configeditor/ConfigurationEditorPlugin.cpp
    plugins/networkanalyzer/NetworkAnalyzerPlugin.h plugins/networkanalyzer/NetworkAnalyzerPlugin.cpp
    plugins/systemmonitor/SystemMonitorPlugin.h    plugins/systemmonitor/SystemMonitorPlugin.cpp
    plugins/securitymanager/SecurityManagerPlugin.h plugins/securitymanager/SecurityManagerPlugin.cpp
    plugins/compliancechecker/ComplianceCheckerPlugin.h plugins/compliancechecker/ComplianceCheckerPlugin.cpp
    plugins/certificationmanager/CertificationManagerPlugin.h plugins/certificationmanager/CertificationManagerPlugin.cpp
    plugins/optimizationdashboard/OptimizationDashboardPlugin.h plugins/optimizationdashboard/OptimizationDashboardPlugin.cpp
    plugins/monitoringdashboard/MonitoringDashboardPlugin.h plugins/monitoringdashboard/MonitoringDashboardPlugin.cpp
    plugins/analyticsdashboard/AnalyticsDashboardPlugin.h plugins/analyticsdashboard/AnalyticsDashboardPlugin.cpp
    plugins/updatemanager/UpdateManagerPlugin.h plugins/updatemanager/UpdateManagerPlugin.cpp
    plugins/maintenancescheduler/MaintenanceSchedulerPlugin.h plugins/maintenancescheduler/MaintenanceSchedulerPlugin.cpp
    plugins/deploymentmanager/DeploymentManagerPlugin.h plugins/deploymentmanager/DeploymentManagerPlugin.cpp
    plugins/integrationhub/IntegrationHubPlugin.h plugins/integrationhub/IntegrationHubPlugin.cpp
    plugins/syncmanager/SyncManagerPlugin.h plugins/syncmanager/SyncManagerPlugin.cpp
    plugins/replicationmanager/ReplicationManagerPlugin.h plugins/replicationmanager/ReplicationManagerPlugin.cpp
    plugins/visualizationstudio/VisualizationStudioPlugin.h plugins/visualizationstudio/VisualizationStudioPlugin.cpp
    plugins/reportdesigner/ReportDesignerPlugin.h plugins/reportdesigner/ReportDesignerPlugin.cpp
    plugins/documentationbrowser/DocumentationBrowserPlugin.h plugins/documentationbrowser/DocumentationBrowserPlugin.cpp
    plugins/cloudmanager/CloudManagerPlugin.h plugins/cloudmanager/CloudManagerPlugin.cpp
    plugins/edgecomputing/EdgeComputingPlugin.h plugins/edgecomputing/EdgeComputingPlugin.cpp
    plugins/aiassistant/AIAssistantPlugin.h plugins/aiassistant/AIAssistantPlugin.cpp
    plugins/digitaltwinstudio/DigitalTwinStudioPlugin.h plugins/digitaltwinstudio/DigitalTwinStudioPlugin.cpp
    plugins/blockchainexplorer/BlockchainExplorerPlugin.h plugins/blockchainexplorer/BlockchainExplorerPlugin.cpp
    plugins/quantumsecurity/QuantumSecurityPlugin.h plugins/quantumsecurity/QuantumSecurityPlugin.cpp
    plugins/pdomapping/PdoMappingEditorPlugin.h plugins/pdomapping/PdoMappingEditorPlugin.cpp
    plugins/pdomapping/PdoMappingCanvas.h plugins/pdomapping/PdoMappingCanvas.cpp
    plugins/pdomapping/PdoMappingValidator.h plugins/pdomapping/PdoMappingValidator.cpp
    plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.h plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.cpp
    plugins/pdomappingoptimization/MappingOptimizerWidget.h plugins/pdomappingoptimization/MappingOptimizerWidget.cpp
    plugins/pdomappingoptimization/SizeOptimizerWidget.h plugins/pdomappingoptimization/SizeOptimizerWidget.cpp
    plugins/dcsyncprecision/DcSyncPrecisionPlugin.h plugins/dcsyncprecision/DcSyncPrecisionPlugin.cpp
    plugins/dcsyncprecision/DriftMonitorWidget.h plugins/dcsyncprecision/DriftMonitorWidget.cpp
    plugins/dcsyncprecision/JitterAnalysisWidget.h plugins/dcsyncprecision/JitterAnalysisWidget.cpp
    plugins/dcsyncoptimizer/DcSyncOptimizerPlugin.h plugins/dcsyncoptimizer/DcSyncOptimizerPlugin.cpp
    plugins/dcsyncoptimizer/SyncOptimizerWidget.h plugins/dcsyncoptimizer/SyncOptimizerWidget.cpp
    plugins/dcsyncoptimizer/DriftOptimizerWidget.h plugins/dcsyncoptimizer/DriftOptimizerWidget.cpp
    plugins/onlinediagnostics/OnlineDiagnosticsPlugin.h plugins/onlinediagnostics/OnlineDiagnosticsPlugin.cpp
    plugins/onlinediagnostics/BusMonitorWidget.h plugins/onlinediagnostics/BusMonitorWidget.cpp
    plugins/onlinediagnostics/ErrorAnalyzerWidget.h plugins/onlinediagnostics/ErrorAnalyzerWidget.cpp
    plugins/realtimeperf/RealtimePerformancePlugin.h plugins/realtimeperf/RealtimePerformancePlugin.cpp
    plugins/realtimeperf/LatencyMonitorWidget.h plugins/realtimeperf/LatencyMonitorWidget.cpp
    plugins/realtimeperf/ThroughputMonitorWidget.h plugins/realtimeperf/ThroughputMonitorWidget.cpp
    plugins/realtimeoptimizer/RealtimeOptimizerPlugin.h plugins/realtimeoptimizer/RealtimeOptimizerPlugin.cpp
    plugins/realtimeoptimizer/LatencyOptimizerWidget.h plugins/realtimeoptimizer/LatencyOptimizerWidget.cpp
    plugins/realtimeoptimizer/ThroughputOptimizerWidget.h plugins/realtimeoptimizer/ThroughputOptimizerWidget.cpp
    plugins/multimaster/MultiMasterPlugin.h plugins/multimaster/MultiMasterPlugin.cpp
    plugins/multimaster/MasterComparisonWidget.h plugins/multimaster/MasterComparisonWidget.cpp
    plugins/hardwareverification/HardwareVerificationPlugin.h plugins/hardwareverification/HardwareVerificationPlugin.cpp
    plugins/hardwareverification/DeviceVerificationWidget.h plugins/hardwareverification/DeviceVerificationWidget.cpp
    plugins/hardwareverification/NetworkVerificationWidget.h plugins/hardwareverification/NetworkVerificationWidget.cpp
    plugins/erroranalysis/AdvancedErrorAnalysisPlugin.h plugins/erroranalysis/AdvancedErrorAnalysisPlugin.cpp
    plugins/erroranalysis/ErrorTimelineWidget.h plugins/erroranalysis/ErrorTimelineWidget.cpp
    plugins/erroranalysis/ErrorCorrelationWidget.h plugins/erroranalysis/ErrorCorrelationWidget.cpp
    plugins/freerunoptimization/FreeRunOptimizationPlugin.h plugins/freerunoptimization/FreeRunOptimizationPlugin.cpp
    plugins/freerunoptimization/CycleTimeOptimizerWidget.h plugins/freerunoptimization/CycleTimeOptimizerWidget.cpp
    plugins/freerunoptimization/DataMappingOptimizerWidget.h plugins/freerunoptimization/DataMappingOptimizerWidget.cpp
    plugins/sdooptimization/SdoOptimizationPlugin.h plugins/sdooptimization/SdoOptimizationPlugin.cpp
    plugins/sdooptimization/CacheOptimizerWidget.h plugins/sdooptimization/CacheOptimizerWidget.cpp
    plugins/sdooptimization/BatchOptimizerWidget.h plugins/sdooptimization/BatchOptimizerWidget.cpp
    plugins/overview/OverviewPlugin.h              plugins/overview/OverviewPlugin.cpp
    plugins/od/OdPlugin.h                          plugins/od/OdPlugin.cpp
    plugins/watch/WatchPlugin.h                    plugins/watch/WatchPlugin.cpp
    plugins/freerun/FreeRunPlugin.h                plugins/freerun/FreeRunPlugin.cpp
    plugins/iovariable/IoVariablePlugin.h          plugins/iovariable/IoVariablePlugin.cpp
    plugins/startupsdo/StartupSdoPlugin.h          plugins/startupsdo/StartupSdoPlugin.cpp
    plugins/session/SessionPlugin.h                plugins/session/SessionPlugin.cpp
    plugins/consistency/ConsistencyPlugin.h        plugins/consistency/ConsistencyPlugin.cpp
    plugins/statemachine/StateMachinePlugin.h      plugins/statemachine/StateMachinePlugin.cpp
    plugins/topology/TopologyPlugin.h              plugins/topology/TopologyPlugin.cpp
    plugins/topology/SlaveNodeItem.h               plugins/topology/SlaveNodeItem.cpp
    plugins/topology/TopologyGraphWidget.h         plugins/topology/TopologyGraphWidget.cpp
    plugins/diagnostics/DiagnosticsPlugin.h        plugins/diagnostics/DiagnosticsPlugin.cpp
    plugins/rttest/RtTestPlugin.h                  plugins/rttest/RtTestPlugin.cpp
    plugins/export/ExportPlugin.h                  plugins/export/ExportPlugin.cpp
    plugins/datapipeline/DataPipelinePlugin.h      plugins/datapipeline/DataPipelinePlugin.cpp
    plugins/devicemanager/DeviceManagerPlugin.h    plugins/devicemanager/DeviceManagerPlugin.cpp
    plugins/performancedashboard/PerformanceDashboardPlugin.h plugins/performancedashboard/PerformanceDashboardPlugin.cpp

    # ── Services ──────────────────────────────────────────────────────
    services/EventBus.h                      services/EventBus.cpp
    services/SdoService.h                      services/SdoService.cpp
    services/WatchService.h                    services/WatchService.cpp
    services/TopologyService.h                 services/TopologyService.cpp
    services/DcSyncService.h                      services/DcSyncService.cpp
    services/AlEventService.h                  services/AlEventService.cpp
    services/SignalService.h                   services/SignalService.cpp
    services/ServiceContainer.h                services/ServiceContainer.cpp
    services/EsiService.h                      services/EsiService.cpp
    services/BusStatsService.h                 services/BusStatsService.cpp
    services/OnlineDiagnosticsService.h          services/OnlineDiagnosticsService.cpp
    services/RealtimePerformanceService.h        services/RealtimePerformanceService.cpp
    services/RealtimeOptimizerService.h          services/RealtimeOptimizerService.cpp
    services/HardwareVerificationService.h       services/HardwareVerificationService.cpp
    services/AdvancedErrorAnalysisService.h      services/AdvancedErrorAnalysisService.cpp
    services/FreeRunOptimizationService.h       services/FreeRunOptimizationService.cpp
    services/SdoOptimizationService.h           services/SdoOptimizationService.cpp
    services/FreeRunConfigurationService.h      services/FreeRunConfigurationService.cpp
    services/FreeRunMonitoringService.h         services/FreeRunMonitoringService.cpp
    services/PdoConfigurationService.h         services/PdoConfigurationService.cpp
    services/PdoMappingOptimizationService.h services/PdoMappingOptimizationService.cpp
    services/OpStateService.h                  services/OpStateService.cpp
    services/MultiMasterService.h              services/MultiMasterService.cpp
    services/ImpactAnalysisService.h           services/ImpactAnalysisService.cpp
    services/CacheService.h                    services/CacheService.cpp
    services/AsyncOperationManager.h           services/AsyncOperationManager.cpp
    services/PerformanceMonitorService.h       services/PerformanceMonitorService.cpp
    services/WatchdogService.h                 services/WatchdogService.cpp
    services/SafetyController.h                services/SafetyController.cpp
    services/DiagnosticReportService.h         services/DiagnosticReportService.cpp
    services/ProjectManagerService.h           services/ProjectManagerService.cpp
    services/ConfigurationService.h            services/ConfigurationService.cpp
    services/AlarmService.h                    services/AlarmService.cpp
    services/LoggingService.h                  services/LoggingService.cpp
    services/ChartService.h                    services/ChartService.cpp
    services/ScriptingService.h                services/ScriptingService.cpp
    services/BatchOperationService.h           services/BatchOperationService.cpp
    services/NetworkDiagnosticsService.h       services/NetworkDiagnosticsService.cpp
    services/EcatHealthService.h               services/EcatHealthService.cpp
    services/MasterManagerService.h            services/MasterManagerService.cpp
    services/DistributedClockService.h         services/DistributedClockService.cpp
    services/DcSyncPrecisionService.h         services/DcSyncPrecisionService.cpp
    services/DcSyncOptimizerService.h        services/DcSyncOptimizerService.cpp
    services/SdoCacheService.h                 services/SdoCacheService.cpp
    services/PdoMappingService.h               services/PdoMappingService.cpp
    services/CoEService.h                      services/CoEService.cpp
    services/FoEService.h                      services/FoEService.cpp
    services/EoEService.h                      services/EoEService.cpp
    services/ConnectionPool.h                  services/ConnectionPool.cpp
    services/StartupOptimizer.h                services/StartupOptimizer.cpp
    services/DataCache.h                       services/DataCache.cpp
    services/BatchProcessor.h                  services/BatchProcessor.cpp
    services/TraceService.h                    services/TraceService.cpp
    services/MasterApiService.h                services/MasterApiService.cpp
    services/DomainService.h                   services/DomainService.cpp
    services/SyncManagerService.h              services/SyncManagerService.cpp
    services/StateMachineService.h             services/StateMachineService.cpp
    services/ErrorHandlingService.h            services/ErrorHandlingService.cpp
    services/DiagnosticService.h               services/DiagnosticService.cpp
    services/HotConnectService.h               services/HotConnectService.cpp
    services/RedundancyService.h               services/RedundancyService.cpp
    services/CableDiagnosticsService.h               services/CableDiagnosticsService.cpp
    services/EtherCATMonitorService.h                services/EtherCATMonitorService.cpp
    services/EtherCATAnalyzerService.h               services/EtherCATAnalyzerService.cpp
    services/EtherCATOptimizerService.h              services/EtherCATOptimizerService.cpp
    services/EtherCATConfigService.h                 services/EtherCATConfigService.cpp
    services/EtherCATBackupService.h                 services/EtherCATBackupService.cpp
    services/EtherCATRecoveryService.h               services/EtherCATRecoveryService.cpp
    services/EtherCATSimulationService.h             services/EtherCATSimulationService.cpp
    services/EtherCATTestingService.h                services/EtherCATTestingService.cpp
    services/EtherCATValidationService.h             services/EtherCATValidationService.cpp
    services/EtherCATSecurityService.h               services/EtherCATSecurityService.cpp
    services/EtherCATComplianceService.h             services/EtherCATComplianceService.cpp
    services/EtherCATCertificationService.h          services/EtherCATCertificationService.cpp
    services/EtherCATAnalyticsService.h              services/EtherCATAnalyticsService.cpp
    services/EtherCATDeploymentService.h             services/EtherCATDeploymentService.cpp
    services/EtherCATUpdateService.h                 services/EtherCATUpdateService.cpp
    services/EtherCATMaintenanceService.h            services/EtherCATMaintenanceService.cpp
    services/EtherCATIntegrationService.h            services/EtherCATIntegrationService.cpp
    services/EtherCATSyncService.h                   services/EtherCATSyncService.cpp
    services/EtherCATReplicationService.h            services/EtherCATReplicationService.cpp
    services/EtherCATVisualizationService.h          services/EtherCATVisualizationService.cpp
    services/EtherCATReportingService.h              services/EtherCATReportingService.cpp
    services/EtherCATDocumentationService.h          services/EtherCATDocumentationService.cpp
    services/EtherCATCloudService.h                services/EtherCATCloudService.cpp
    services/EtherCATEdgeService.h                 services/EtherCATEdgeService.cpp
    services/EtherCATAIService.h                   services/EtherCATAIService.cpp
    services/EtherCATDigitalTwinService.h          services/EtherCATDigitalTwinService.cpp
    services/EtherCATBlockchainService.h           services/EtherCATBlockchainService.cpp
    services/EtherCATQuantumService.h              services/EtherCATQuantumService.cpp
    services/WorkflowOptimizationService.h         services/WorkflowOptimizationService.cpp
    services/WorkflowAutomationService.h           services/WorkflowAutomationService.cpp
    services/WorkflowSchedulingService.h           services/WorkflowSchedulingService.cpp
    services/WorkflowMonitoringService.h           services/WorkflowMonitoringService.cpp
    services/WorkflowVisualizationService.h        services/WorkflowVisualizationService.cpp
    services/WorkflowReportingService.h            services/WorkflowReportingService.cpp
    services/WorkflowIntegrationService.h          services/WorkflowIntegrationService.cpp
    services/WorkflowSecurityService.h             services/WorkflowSecurityService.cpp
    services/WorkflowComplianceService.h           services/WorkflowComplianceService.cpp
    services/WorkflowCertificationService.h        services/WorkflowCertificationService.cpp
    services/WorkflowDeploymentService.h           services/WorkflowDeploymentService.cpp
    services/WorkflowUpdateService.h               services/WorkflowUpdateService.cpp
    services/WorkflowMaintenanceService.h          services/WorkflowMaintenanceService.cpp
    services/WorkflowIntegrationHubService.h       services/WorkflowIntegrationHubService.cpp
    services/WorkflowSyncService.h                 services/WorkflowSyncService.cpp
    services/WorkflowReplicationService.h          services/WorkflowReplicationService.cpp
    services/WorkflowAnalyticsService.h            services/WorkflowAnalyticsService.cpp
    services/WorkflowCloudService.h                services/WorkflowCloudService.cpp
    services/WorkflowEdgeService.h                 services/WorkflowEdgeService.cpp
    services/WorkflowAIService.h                   services/WorkflowAIService.cpp
    services/WorkflowDigitalTwinService.h          services/WorkflowDigitalTwinService.cpp
    services/WorkflowBlockchainService.h           services/WorkflowBlockchainService.cpp
    services/WorkflowQuantumService.h              services/WorkflowQuantumService.cpp
    services/WorkflowVisualizationStudioService.h  services/WorkflowVisualizationStudioService.cpp
    services/WorkflowReportDesignerService.h       services/WorkflowReportDesignerService.cpp
    services/WorkflowDocumentationBrowserService.h services/WorkflowDocumentationBrowserService.cpp
    services/WorkflowSecurityManagerService.h         services/WorkflowSecurityManagerService.cpp
    services/WorkflowComplianceManagerService.h       services/WorkflowComplianceManagerService.cpp
    services/WorkflowCertificationManagerService.h    services/WorkflowCertificationManagerService.cpp
    plugins/workflowoptimizer/WorkflowOptimizerPlugin.h plugins/workflowoptimizer/WorkflowOptimizerPlugin.cpp
    plugins/workflowdashboard/WorkflowDashboardPlugin.h plugins/workflowdashboard/WorkflowDashboardPlugin.cpp
    services/ProjectManagementService.h              services/ProjectManagementService.cpp
    services/TaskManagementService.h                 services/TaskManagementService.cpp
    services/ResourceManagementService.h             services/ResourceManagementService.cpp
    services/ProjectPlanningService.h                services/ProjectPlanningService.cpp
    services/ProjectTrackingService.h                services/ProjectTrackingService.cpp
    services/ProjectReportingService.h               services/ProjectReportingService.cpp
    services/ExportService.h                         services/ExportService.cpp
    services/DataPipelineService.h                   services/DataPipelineService.cpp
    services/DeviceManagerService.h                  services/DeviceManagerService.cpp
    services/FirmwareUpdateService.h                 services/FirmwareUpdateService.cpp
    services/ReportGeneratorService.h                services/ReportGeneratorService.cpp

    # ── Models ──────────────────────────────────────────────────────────
    models/Cia402DriveModel.cpp                        models/Cia402DriveModel.h
    models/CommissioningWorkflowModel.cpp              models/CommissioningWorkflowModel.h
    models/EvidenceModel.cpp                     models/EvidenceModel.h
    models/NextBestActionModel.cpp                     models/NextBestActionModel.h
    models/ProcessDataRowModel.cpp                     models/ProcessDataRowModel.h
    models/SdoEvidenceModel.cpp                        models/SdoEvidenceModel.h
    models/SessionBriefModel.cpp                       models/SessionBriefModel.h
    models/SlaveEvidenceModel.cpp                      models/SlaveEvidenceModel.h
    models/TopologyModel.cpp                   models/TopologyModel.h
    models/ConsistencyModel.cpp                  models/ConsistencyModel.h
    models/IoVariableModel.cpp                         models/IoVariableModel.h
    models/WatchStartupModel.cpp                    models/WatchStartupModel.h
    adapters/WorkflowTableAdapter.cpp     adapters/WorkflowTableAdapter.h
    adapters/ConsistencyTableAdapter.cpp               adapters/ConsistencyTableAdapter.h
    adapters/ProcessDataTableAdapter.cpp               adapters/ProcessDataTableAdapter.h
    adapters/SdoDictionaryTableAdapter.cpp             adapters/SdoDictionaryTableAdapter.h
    adapters/SdoEvidenceTableAdapter.cpp               adapters/SdoEvidenceTableAdapter.h
    adapters/SessionBriefTableAdapter.cpp              adapters/SessionBriefTableAdapter.h
    adapters/SlaveEvidenceTableAdapter.cpp             adapters/SlaveEvidenceTableAdapter.h
    adapters/StateMachineTableAdapter.cpp              adapters/StateMachineTableAdapter.h
    adapters/WatchStartupTableAdapter.cpp              adapters/WatchStartupTableAdapter.h
    adapters/WorkspaceTabBadgeTableAdapter.cpp         adapters/WorkspaceTabBadgeTableAdapter.h
    # ── UI State (detail panel text builders) ───────────────────────────
    detail/WorkflowStepDetail.cpp  detail/WorkflowStepDetail.h
    detail/CommissioningWorkflowDetail.cpp            detail/CommissioningWorkflowDetail.h
    detail/ConsistencyDetail.cpp                detail/ConsistencyDetail.h
    detail/DiagnosticsEventDetail.cpp                 detail/DiagnosticsEventDetail.h
    detail/FreeRunEntryDetail.cpp               detail/FreeRunEntryDetail.h
    detail/HostHealthDetail.cpp                       detail/HostHealthDetail.h
    detail/IoVariableDetail.cpp                 detail/IoVariableDetail.h
    detail/NextBestActionDetail.cpp                   detail/NextBestActionDetail.h
    detail/ObjectBookmarkDetail.cpp             detail/ObjectBookmarkDetail.h
    detail/PdoMapDetail.cpp                     detail/PdoMapDetail.h
    detail/SdoHistoryRowDetail.cpp              detail/SdoHistoryRowDetail.h
    detail/SdoTargetTrailDetail.cpp             detail/SdoTargetTrailDetail.h
    detail/SelectedDriveSummaryDetail.cpp             detail/SelectedDriveSummaryDetail.h
    detail/SlaveEvidenceSummaryDetail.cpp     detail/SlaveEvidenceSummaryDetail.h
    detail/SessionBriefDetail.cpp                     detail/SessionBriefDetail.h
    detail/SlaveEvidenceDetail.cpp                    detail/SlaveEvidenceDetail.h
    detail/StartupSdoRowDetail.cpp              detail/StartupSdoRowDetail.h
    detail/StateMachineRowDetail.cpp            detail/StateMachineRowDetail.h
    detail/WatchRowDetail.cpp                   detail/WatchRowDetail.h
    detail/WatchStartupDetail.cpp                     detail/WatchStartupDetail.h
    detail/WorkspaceBoundaryDetail.cpp                detail/WorkspaceBoundaryDetail.h
    detail/WorkspaceTabBadgeDetail.cpp                detail/WorkspaceTabBadgeDetail.h
    detail/RealtimeChartDialog.cpp              detail/RealtimeChartDialog.h
    # ── Helpers ─────────────────────────────────────────────────────────
    utils/ConfirmDialogBuilder.h               utils/ConfirmDialogBuilder.cpp
    utils/Documentation.cpp                    utils/Documentation.h
    utils/TableHelpers.cpp                     utils/TableHelpers.h
    utils/TextHelpers.cpp                      utils/TextHelpers.h
    utils/UiHelpers.cpp                        utils/UiHelpers.h
    utils/MemoryPool.h
    # ── Infrastructure ──────────────────────────────────────────────────
    infra/EcatClient.cpp                               infra/EcatClient.h
    infra/LanguageManager.cpp                       infra/LanguageManager.h
    infra/TranslationRegistry.cpp                   infra/TranslationRegistry.h
    infra/ProcessDataTypes.h
    infra/SettingsDialog.cpp                           infra/SettingsDialog.h
    # ── MainWindow (core + partial per workspace) ───────────────────────
    MainWindow.h
    workspaces/MainWindowIncludes.h
    MainWindow.cpp
    workspaces/MainWindowStateMachine.cpp                workspaces/MainWindowTopologyUi.cpp
    workspaces/MainWindowCommandPalette.cpp
    workspaces/MainWindowConsistency.cpp
    workspaces/MainWindowCommissioning.cpp
    workspaces/MainWindowSlaveEvidence.cpp
    workspaces/MainWindowContextMenus.cpp
    workspaces/MainWindowExport.cpp
    workspaces/MainWindowFreeRunChart.cpp
    workspaces/MainWindowFreeRunWorkspace.cpp
    workspaces/MainWindowDiagnosticsTopology.cpp
    workspaces/MainWindowIoVariableWorkspace.cpp
    workspaces/MainWindowIoVariableTable.cpp
    workspaces/MainWindowManual.cpp
    workspaces/MainWindowProjectIo.cpp
    workspaces/MainWindowRtTestWorkspace.cpp
    workspaces/MainWindowSdoWorkspace.cpp
    workspaces/MainWindowSdoWrite.cpp
    workspaces/MainWindowSdoSelection.cpp
    workspaces/MainWindowSdoTargetPanel.cpp
    workspaces/MainWindowSdoBookmarks.cpp
    workspaces/MainWindowSessionWorkspace.cpp
    workspaces/MainWindowStartupSdoWorkspace.cpp
    workspaces/MainWindowTexts.cpp
    workspaces/MainWindowTheme.cpp
    themes/ThemeManager.h             themes/ThemeManager.cpp
    workspaces/MainWindowUiBuild.cpp
    workspaces/MainWindowWatchWorkspace.cpp
    workspaces/MainWindowWatchSync.cpp
    workspaces/MainWindowWiring.cpp
    workspaces/MainWindowWorkspaceNav.cpp
    workspaces/MainWindowSettings.cpp
    themes/themes.qrc
    main.cpp
)
target_include_directories(ecat-studio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters
    ${CMAKE_CURRENT_SOURCE_DIR}/detail
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/infra
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/esi
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/busstats
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/alarm
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/project
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/oscilloscope
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/chart
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/dashboard
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/automation
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/network
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/sdocache
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/themecustomizer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/shortcuts
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/preferences
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/trace
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/logicanalyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/diagram
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/formula
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/scriptlibrary
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/calibration
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/documentation
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/wizard
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/template
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/report
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/dashboarddesigner
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/alarmmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/datalogger
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/workflowdesigner
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/workflowoptimizer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/workflowdashboard
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/testsuite
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/deployment
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/configeditor
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/networkanalyzer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/systemmonitor
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/securitymanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/compliancechecker
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/certificationmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/optimizationdashboard
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/monitoringdashboard
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/analyticsdashboard
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/updatemanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/maintenancescheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/deploymentmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/integrationhub
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/syncmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/replicationmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/visualizationstudio
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/reportdesigner
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/documentationbrowser
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/cloudmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/edgecomputing
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/aiassistant
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/digitaltwinstudio
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/blockchainexplorer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/quantumsecurity
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/pdomapping
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/pdomappingoptimization
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/onlinediagnostics
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/multimaster
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/dcsyncprecision
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/dcsyncoptimizer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/realtimeperf
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/realtimeoptimizer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/erroranalysis
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/freerunoptimization
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/sdooptimization
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/hardwareverification
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/overview
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/od
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/watch
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/freerun
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/iovariable
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/startupsdo
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/session
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/consistency
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/statemachine
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/topology
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/diagnostics
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/alevent
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/dcsync
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/signal
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/rttest
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/export
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/notes
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/datapipeline
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/devicemanager
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/performancedashboard
)
# ── Experimental Services ─────────────────────────────────────────────
option(ECAT_EXPERIMENTAL_SERVICES "Include experimental/stub services (blockchain, quantum, AI, etc.)" ON)
if(ECAT_EXPERIMENTAL_SERVICES)
    target_compile_definitions(ecat-studio PRIVATE ECAT_EXPERIMENTAL_SERVICES=1)
    message(STATUS "Experimental services: ENABLED")
else()
    message(STATUS "Experimental services: DISABLED")
endif()

find_package(Qt6 COMPONENTS Qml QUIET)
if(Qt6Qml_FOUND)
    target_link_libraries(ecat-studio PRIVATE Qt6::Qml)
    target_compile_definitions(ecat-studio PRIVATE ECAT_SCRIPTING_ENABLED)
endif()

target_link_libraries(ecat-studio PRIVATE Qt6::Core Qt6::Network Qt6::Widgets ecat_core)

# ── Precompiled Headers ──────────────────────────────────────────────
target_precompile_headers(ecat-studio PRIVATE pch.h)

# ── Build Optimization Flags ─────────────────────────────────────────
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(ecat-studio PRIVATE -O2 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(ecat-studio PRIVATE -g -O0 -DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
    target_compile_options(ecat-studio PRIVATE -O2 -pg -fprofile-generate)
    target_link_options(ecat-studio PRIVATE -pg -fprofile-generate)
endif()

# ── Qt Translation Files ─────────────────────────────────────────────
qt_add_translations(ecat-studio
    TS_FILES
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_zh.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_ja.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_de.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_ko.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_zh_TW.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_fr.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_es.ts
)
