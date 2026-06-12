# adapters/ — Model-to-Table Bridge

Each adapter populates a `QTableWidget` from model data and provides
row-lookup helpers. Adapters depend on models and Qt Widgets only.

## Files

| File | Purpose |
|------|---------|
| `CommissioningWorkflowTableAdapter` | Commissioning workflow table |
| `ConsistencyTableAdapter` | Consistency issue table |
| `ProcessDataTableAdapter` | PDO map / process data table |
| `SdoDictionaryTableAdapter` | Object Dictionary (SDO) table |
| `SdoEvidenceTableAdapter` | SDO evidence / history table |
| `SessionBriefTableAdapter` | Session brief table |
| `SlaveEvidenceTableAdapter` | Slave evidence matrix table |
| `StateMachineTableAdapter` | State machine recommendation table |
| `WatchStartupTableAdapter` | Watch and Startup SDO tables |
| `WorkspaceTabBadgeTableAdapter` | Tab badge text and severity |
