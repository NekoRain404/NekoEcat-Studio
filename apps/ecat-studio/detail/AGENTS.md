# detail/ — Detail Panel UI State Builders

Plain data structs and builder functions for populating the detail/info
strip below each workspace table. These are pure text builders — they
return localized strings and severity keys, never touch widgets directly.

## Files

| File | Purpose |
|------|---------|
| `CommissioningWorkflowDetail` | Commissioning workflow step detail texts |
| `CommissioningWorkflowStepDetail` | Workflow step detail panel state |
| `ConsistencyDetail` | Consistency check row detail texts |
| `DiagnosticsEventDetail` | Diagnostics event severity mapping |
| `FreeRunEntryDetail` | Free Run entry detail texts |
| `HostHealthDetail` | Host health check result texts |
| `IoVariableDetail` | I/O variable row detail texts |
| `NextBestActionDetail` | Next-best-action suggestion texts |
| `ObjectBookmarkDetail` | Object bookmark row detail texts |
| `PdoMapDetail` | PDO map row detail texts |
| `SdoHistoryRowDetail` | SDO history row detail texts |
| `SdoTargetTrailDetail` | SDO target trail row detail texts |
| `SelectedDriveSummaryDetail` | Selected drive summary label texts |
| `SelectedSlaveEvidenceSummaryDetail` | Slave evidence summary texts |
| `SessionBriefDetail` | Session brief row detail texts |
| `SlaveEvidenceDetail` | Slave evidence matrix cell texts |
| `StartupSdoRowDetail` | Startup SDO row detail texts |
| `StateMachineRowDetail` | CiA 402 state machine row detail texts |
| `WatchRowDetail` | Watch row detail texts |
| `WatchStartupDetail` | Watch-Startup comparison detail texts |
| `WorkspaceBoundaryDetail` | Workspace boundary label texts |
| `WorkspaceTabBadgeDetail` | Tab badge count/tooltip texts |
