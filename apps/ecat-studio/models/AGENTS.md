# models/ — Pure Data & Logic Types

Domain models with no QWidget dependencies. Safe to include in tests
without a QApplication.

## Files

| File | Purpose |
|------|---------|
| `Cia402DriveModel` | CiA 402 controlword/statusword recommendation |
| `CommissioningWorkflowModel` | Workflow step status, scope, severity |
| `ConsistencyEvidenceRouteModel` | Route from consistency issue to evidence |
| `ConsistencyGateModel` | Issue classification, severity, blocking rules |
| `EvidenceStatusModel` | Drive evidence severity classification |
| `IoVariableBulkNamingModel` | Bulk PLC symbol naming & collision detection |
| `IoVariableFilterModel` | I/O variable table filter predicates |
| `IoVariableHandoffModel` | PLC handoff issue detection |
| `NextBestActionModel` | Next-best-action recommendation engine |
| `ProcessDataRowModel` | PDO row formatting and evidence lookup |
| `SdoEvidenceModel` | SDO read evidence tracking and comparison |
| `SdoTargetPanelRouteModel` | SDO target panel source classification |
| `SessionBriefModel` | Session summary rows |
| `SlaveEvidenceModel` | Per-slave evidence row model |
| `StateRecommendationModel` | EtherCAT state transition recommendations |
| `TopologyBaselineModel` | Topology baseline capture and drift |
| `TopologyChangeModel` | Topology change event classification |
| `WatchStartupModel` | Watch/Startup SDO delta comparison |
