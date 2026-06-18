# models/ — Pure Data & Logic Types

Domain models with no QWidget dependencies. Safe to include in tests
without a QApplication.

## Files

| File | Purpose |
|------|---------|
| `Cia402DriveModel` | CiA 402 controlword/statusword recommendation |
| `CommissioningWorkflowModel` | Workflow step status, scope, severity |
| `ConsistencyModel` | Consistency issue classification and routing |
| `EvidenceModel` | Evidence severity classification and state recommendations |
| `IoVariableModel` | I/O variable: bulk naming, filtering, and PLC handoff |
| `NextBestActionModel` | Next-best-action recommendation engine |
| `ProcessDataRowModel` | PDO row formatting and evidence lookup |
| `SdoEvidenceModel` | SDO evidence tracking, comparison, and target panel routing |
| `SessionBriefModel` | Session summary rows |
| `SlaveEvidenceModel` | Per-slave evidence row model |
| `TopologyModel` | Topology baseline capture and change detection |
| `WatchStartupModel` | Watch/Startup SDO delta comparison |
