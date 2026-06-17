# NekoEcat Studio Project Management

This document keeps the project maintainable while the GUI grows into a full
EtherCAT engineering workstation.

## Directory Map

- `apps/ecat-studio/`: Qt desktop GUI. It owns presentation, local project
  state, command palette, user confirmations, and local evidence navigation.
- `apps/ecatd/`: local runtime daemon. It owns online EtherCAT operations and
  process-image Free Run execution.
- `src/core/`: shared data types and newline-delimited JSON protocol helpers.
- `src/igh/`: IgH EtherCAT integration. CLI calls and future native ecrt
  adapters belong here, not in the GUI.
- `tests/`: small executable regression tests for extracted pure logic. Tests
  must not require EtherCAT hardware, host services, or GUI display access.
- `docs/`: product, development, operation, and project-management notes.
- `build/`: generated CMake/build output. Do not treat it as source.

## Dependency Direction

Keep dependencies one-way:

1. `src/core` has no dependency on app code.
2. `src/igh` may depend on `src/core`.
3. `apps/ecatd` may depend on `src/core` and `src/igh`.
4. `apps/ecat-studio` may depend on `src/core` and its own GUI helpers.
5. The GUI must talk to online EtherCAT behavior through `EcatClient` and
   `ecatd`; it should not shell out to IgH or modify host services directly.

## Current Risk Register

- `apps/ecat-studio/MainWindow.cpp` is still the main growth risk. It contains
  UI construction, project persistence, local evidence routing, command palette,
  and many table workflows.
- Host-environment operations are high risk. Keep Host Health in Diagnostics,
  keep confirmations for dangerous online actions, and avoid running host
  repair commands during normal UI refactors.
- Build output currently lives in the workspace. Avoid editing generated files
  under `build/`.

## Gradual Split Plan

Use small, behavior-preserving steps. Each split must compile and pass the
offscreen smoke test before the next one.

1. `StudioUiHelpers`: pure widget helpers and presentation utilities with no
   EtherCAT state. This has started with section titles, metric cards, toolbar
   labels, status-summary labels, common widget repolishing, and generic
   tab activation for an already-created child widget.
2. `StudioTableHelpers`: table extraction, row-copy, CSV/Markdown export, and
   generic table selection helpers. This has started with table text access,
   selected/visible row helpers, row copying, CSV/Markdown cell escaping, and
   Markdown table writing; shared row selection, centering, and focus behavior
   also belongs here so workspace navigation does not duplicate widget
   mechanics. Shared lookup for position rows, object index/subindex rows,
   object-address rows, and the first visible row belongs here when the source
   is already-loaded `QTableWidget` evidence.
3. `StudioTextHelpers`: pure text normalization and capture helpers that do not
   depend on widgets, MainWindow state, or EtherCAT online behavior. This has
   started with hex normalization, comparable-value normalization, PLC
   identifier generation, and regex capture.
4. `StudioDocumentation`: Help/About documentation dialog helpers, including
   documentation browser construction, manual search controls, and light/dark
   HTML theming. This keeps long-form product documentation presentation out of
   the MainWindow workflow logic while the HTML content is gradually moved.
5. `SdoEvidenceModel`: SDO evidence keys, reusable history checks, preferred
   evidence selection, evidence grouping, conflict detection, write delta, and
   digest generation. This has started with key generation, SDO history reuse
   checks, preferred candidate selection, evidence grouping, evidence conflict
   detection, write-delta state review, and shared comparable-value checks for
   Watch/Startup SDO deltas.
6. `ObjectDictionaryWorkspace`: Object Dictionary, Selected Object, Target
   Trail, Object Bookmarks, and SDO History UI wiring.
7. `WatchStartupWorkspace`: Watch, Startup SDO, and Watch/Startup delta logic.
   This has started with `WatchStartupModel`, a pure model for Watch/Startup
   address matching, delta state evaluation, summary counts, diff-row
   filtering; `WatchStartupTableAdapter`, the Qt table-to-model bridge; and
   `WatchStartupUiState`, the local cell text/color renderer for Watch/Startup
   evidence review. `WatchRowDetailUiState` owns Watch row detail-strip
   severity, value/evidence labels, CiA 402 detection, summary text, and
   tooltip lines from an already extracted `WatchStartupWatchRow`; MainWindow
   keeps current row extraction, changed-key injection, QLabel application,
   filtering, refresh/auto-polling actions, Startup mutations, and
   localization. `StartupSdoRowDetailUiState` owns Startup SDO row
   detail-strip severity, Watch comparison evidence labels, summary text, and
   tooltip lines from an already extracted `WatchStartupStartupRow`; MainWindow
   keeps current row extraction, QLabel application, filtering, Verify/Apply
   execution, table mutations, and localization.
8. `TopologyBaselineModel`: pure topology-baseline comparison shared by
   summaries, diagnostics reports, and the Consistency gate. It returns
   structured issue types while UI text and localization stay in the
   presentation layer.
9. `TopologyChangeModel`: pure scan-to-scan topology change detection for
   added, removed, identity, state, and flag changes. Diagnostics rendering
   stays in the GUI, while change ordering and payload selection stay reusable.
10. `StateRecommendationModel`: pure EtherCAT state recommendation rules.
    The GUI collects loaded evidence from tables and handles confirmation;
    INIT/PREOP/SAFEOP progression rules stay testable and UI-independent.
    `StateMachineTableAdapter` owns state-machine table column constants,
    selected-row extraction, slave-position parsing, and recommendation
    availability checks. `StateMachineRowDetailUiState` owns selected
    state-machine row detail-strip severity, transition-boundary wording,
    summary text, and tooltip boundary lines from an already extracted
    `StateMachineTableRow`. MainWindow keeps evidence collection, table
    rendering, QLabel application, current-row selection, and all explicit
    confirmed state request paths.
11. `ConsistencyGateModel`: pure Consistency level parsing, issue counting,
    blocking detection, and NotRun/Stale/Blocking/Passed gate state. The GUI
    keeps table extraction and localized action text.
12. `ConsistencyTableAdapter`: read-only Qt table adapter for the Consistency
    workspace. It owns column constants, local scope/search filtering,
    issue-count extraction, first blocking row lookup, and first I/O issue row
    lookup. It must not rebuild checks, navigate evidence, call the runtime
    client, or run host diagnostics.
13. `ConsistencyDetailUiState`: local presentation-state helpers for the
    selected Consistency row detail strip. It owns severity keys, route labels,
    fallback text, summary text, and tooltip boundary lines from already
    extracted row text. MainWindow keeps table row extraction, QLabel
    application, and actual evidence navigation.
14. `ConsistencyEvidenceRouteModel`: pure local evidence routing rules for
    Consistency rows. It owns target address parsing, Startup row parsing,
    route-kind selection, and I/O Variables scope selection from already
    extracted row text. MainWindow keeps route orchestration, tab activation,
    diagnostics logging, and all actual navigation side effects; shared widget
    row lookup and row focusing stay in `StudioTableHelpers`.
15. `SdoEvidenceTableAdapter`: read-only Qt table adapter for current SDO
    evidence candidates and local SDO evidence items. It owns table-column
    extraction for Read/OD/Watch/Startup/Bookmark/Target Trail candidate
    values, current target row lookup, multi-row local evidence collection,
    write-evidence item assembly, Target Trail row parsing/startup-value
    fallback/deduplication keys, Object Bookmark row parsing, SDO History row
    parsing, and read-only write-delta evidence availability checks after
    MainWindow has provided the current target and localized source labels. It
    must not classify write risk, open tabs, issue SDO commands, call the
    runtime client, or run host diagnostics. `SdoHistoryRowDetailUiState` owns
    SDO History detail-strip severity, reusable-value wording, summary text,
    and tooltip boundary lines from an already extracted `SdoHistoryRow`.
    `ObjectBookmarkDetailUiState` owns Object Bookmark detail-strip severity,
    read-only reuse wording, summary text, and tooltip boundary lines from an
    already extracted `SdoObjectBookmarkRow`. `SdoTargetTrailDetailUiState`
    owns Target Trail detail-strip severity, saved-value/startup reuse wording,
    summary text, and tooltip boundary lines from an already extracted
    `SdoTargetTrailRow` plus MainWindow-provided startup-readiness context.
    MainWindow keeps QLabel application, table row extraction, local
    navigation, target filling, Watch and Startup table mutations, and all
    explicit online read/write paths.
16. `SdoDictionaryTableAdapter`: read-only Qt table adapter for Object
    Dictionary row extraction. It owns OD column constants, index/subindex
    normalization, visible/failed row collection, selected-value detection,
    target-row extraction, and read-object extraction for already selected
    rows. MainWindow keeps online read requests, Watch/Bookmark/Startup table
    mutations, confirmations, localization, and navigation.
17. `SdoTargetPanelRouteModel`: pure route rules for Selected Object panel
    row keys. It maps bilingual row labels plus write-delta availability to
    local navigation/copy action kinds. MainWindow keeps the actual tab
    activation, clipboard writing, status messages, diagnostics logging, and
    all online/confirmed SDO command paths.
18. `EvidenceStatusModel`: pure text-status classification shared by State
    Machine, Consistency, I/O Variables, and Slave Evidence Matrix. Startup
    diff, PDO map issue, and CiA 402 drive evidence severity detection live
    here so bilingual status strings do not drift across workspaces.
19. `Cia402DriveModel`: pure CiA 402 object detection, Watch value decoding,
    decoded-statusword severity support, and recommended controlword mapping.
    The GUI still finds Watch evidence, localizes labels, and uses the existing
    confirmed SDO write path.
20. `SessionBriefModel`: pure Overview Session Brief row-state rules. It turns
    already collected local evidence flags into stable row kinds, action keys,
    and Ready/Action/Warning/Error status; MainWindow keeps local evidence data
    collection, clipboard formatting, and local evidence navigation.
21. `CommissioningWorkflowModel`: pure Overview commissioning workflow rules.
    It turns already collected local evidence flags into 10 stable workflow
    step statuses, the single workflow step order, and the next workflow step
    index. It must not access widgets, project state, the runtime client, or
    host diagnostics.
22. `CommissioningWorkflowUiState`: local presentation-state helpers for the
    Overview commissioning workflow. It owns row status mapping, localized
    table headers, table cells, tooltip detail text, step boundary labels,
    stable status color keys, and Ready/Action/Blocked counts. MainWindow keeps
    evidence collection, clipboard formatting, and online/local action routing.
23. `CommissioningWorkflowTableAdapter`: UI-table adapter for the Overview
    commissioning workflow. It owns table column constants, stable status-key
    storage, structured row extraction for selected workflow rows, local
    filtering statistics, scope/search matching, and first/next visible issue
    row lookup. It must stay local-only and must not run workflow steps, read
    the bus, or call the client.
24. `CommissioningWorkflowStepDetailUiState`: local presentation-state helper
    for the selected Overview commissioning workflow step detail strip. It owns
    empty-selection states, severity-key selection, risk fallback text, summary
    text, tooltip lines, and step boundary display from an already extracted
    `CommissioningWorkflowTableRow`. MainWindow keeps row selection, localized
    text construction, QLabel application, clipboard formatting, navigation,
    and all explicit workflow execution paths.
25. `NextBestActionModel`: pure status-bar Next Best Action rules. It turns the
    current `CommissioningWorkflowInput`, diagnostics-error flag, first
    blocking Consistency row, and Slave Evidence Matrix priority counts into a
    stable action kind and severity. It must not depend on widgets, localized
    text, icons, project state, or the runtime client.
26. `NextBestActionUiState`: local presentation-state helpers for the status-bar
    Next Best Action button. It turns the model decision plus localized text
    into stable action/severity keys, button text, tooltip text, and an icon key.
    MainWindow keeps local evidence collection, Qt icon mapping, button property
    assignment, and explicit online/local execution paths.
27. `SessionBriefTableAdapter`: UI-table adapter for the Overview Session Brief.
    It owns Session Brief column constants, action-key storage, selected-row
    extraction, and tooltip extraction from `QTableWidget`. MainWindow keeps
    Session Brief evidence collection, localized text construction, clipboard
    formatting, and local evidence navigation.
28. `SessionBriefUiState`: local presentation-state helpers for the Overview
    Session Brief. It turns structured Session Brief rows plus localized
    evidence text into table headers, status labels, cells, tooltip text, and
    stable color keys. It must not access widgets, project state, the runtime
    client, or host diagnostics.
29. `SlaveEvidenceModel`: pure Overview slave evidence queue rules. It turns
    already loaded local counts into readiness, risk kinds, next actions, and
    P0/P1/P2/P3 ordering. It also owns the structured local evidence route
    target for a matrix row, so navigation rules do not depend on localized UI
    text. MainWindow keeps table extraction, localized text, colors,
    clipboard, and tab activation.
30. `SlaveEvidenceTableAdapter`: UI-table adapter for the Overview slave
    evidence queue. It owns table column constants and converts already loaded
    Watch, Startup SDO, Free Run, identity, OD, and PDO table state into
    `SlaveEvidenceInput`; it also owns Slave Evidence Matrix column constants,
    local filter statistics, priority counts, matrix route-target row storage,
    and local evidence row lookup for Watch/Startup/Free Run routing. It must
    stay read-only and must not call the client.
31. `SlaveEvidenceUiState`: local presentation-state helpers for the Overview
    slave evidence queue. It turns structured slave evidence rows into
    localized table headers, table cells, and tooltip detail lines. It must not
    access widgets, project state, the runtime client, or host diagnostics.
32. `SelectedSlaveEvidenceSummaryUiState`: local presentation-state helper for
    the selected-slave evidence score strip. It turns one
    `SlaveEvidenceInput`, topology issue count, and localized text into the
    summary label text, tooltip lines, evidence group score, and severity key.
    MainWindow keeps selected-slave lookup, loaded table pointers, topology
    issue collection, and QLabel application.
33. `SelectedDriveSummaryUiState`: local presentation-state helper for the
    selected-slave CiA 402 drive strip. It turns extracted Watch rows,
    selected position, and localized text into the summary label text,
    evidence parts, severity key, and recommended controlword evidence.
    MainWindow keeps Watch table extraction, connection gating, SDO write
    confirmation, and QLabel/button application.
34. `ProcessDataWorkspace`: PDO Map, Free Run process-image rows, and I/O
    Variables.
35. `ProcessDataTypes`: lightweight row structs shared by Process Data models.
    It must stay free of widgets, runtime clients, localization, and project
    metadata mutation so pure rules can compile without pulling in table
    adapters.
36. `ProcessDataRowModel`: pure Process Data row rules. It owns target/value
    checks, normalized I/O object keys, value selection, process/PDO/Watch/
    Startup/Map/Changed/PLC issue classification, basic SDO type inference,
    IEC type mapping, and Rx/Tx/CiA 402 detection from already extracted row
    data.
37. `ProcessDataTableAdapter`: read-only Qt table adapter for Process Data
    row extraction. It owns PDO Map, Free Run Entry, and I/O Variables target
    parsing from `QTableWidget`, including Free Run changed-row state and map
    detail fields; table-backed row key/value availability; and selected/
    visible I/O row collection. MainWindow keeps online reads, local table
    mutations, confirmations, search/scope UI, PLC metadata editing, exports,
    colors, and navigation.
38. `IoVariableHandoffModel`: pure PLC handoff rules for I/O Variables. It owns
    suggested alias generation, PLC symbol names, quality issue classification,
    duplicate symbol detection, handoff comment sanitizing, PLC direction, and
    PLC type selection from `ProcessDataRowModel`; it also owns unique symbol
    suffixing, IEC `VAR_GLOBAL` declaration line/block generation, PLC CSV
    headers, and PLC CSV row values. MainWindow keeps localization, dialogs,
    project metadata mutation, clipboard/file handles, CSV escaping, timestamps,
    and evidence navigation.
39. `IoVariableDetailUiState`: local presentation-state helpers for the I/O
    Variables detail strip. It owns severity-key selection, display fallback
    text, signal-state labels, summary text, and tooltip boundary lines from an
    already extracted `IoVariableTableRow`. MainWindow keeps row extraction,
    localized text construction, QLabel application, and all online/local
    action routing.
40. `IoVariableFilterModel`: pure I/O Variables filtering and summary rules. It
    owns stable scope keys, per-row scope state, local text-search matching,
    visible-row decisions, issue/count accumulation, and summary formatting
    from already extracted row data and cell text. MainWindow keeps
    `QTableWidget` row hiding, current filter widgets, localized scope labels,
    and action refreshes.
41. `IoVariableBulkNamingModel`: pure I/O Variables bulk-naming rules. It owns
    target-row planning, existing-alias skip counts, project-local alias
    generation, duplicate alias suffixing, requested tag merging, and inferred
    PLC direction/type tags from already extracted row data. MainWindow keeps
    dialogs, confirmations, current visible/selected row collection,
    project-metadata writes, localization, and refresh/navigation side effects.
42. `FreeRunEntryDetailUiState`: local presentation-state helpers for the Free
    Run process-image detail strip. It owns output/input boundary wording,
    name-source parsing from map detail text, severity-key selection, summary
    text, and tooltip lines from an already extracted `FreeRunEntryTableRow`.
    MainWindow keeps table row extraction, QLabel application, filtering,
    online Free Run toggling, SDO target fill/read actions, and localization.
43. `PdoMapDetailUiState`: local presentation-state helpers for the PDO Map
    detail strip. It owns Rx/Tx direction classification, process-role wording,
    bit-width type inference through `ProcessDataRowModel`, CiA 402 candidate
    detection, severity-key selection, summary text, and tooltip lines from an
    already extracted `PdoMapTableRow`. MainWindow keeps table row extraction,
    current selected slave, QLabel application, filtering, online PDO loading,
    SDO target fill/read actions, Watch mutations, and localization.
44. `HostHealthUiState`: local presentation-state helpers for Diagnostics Host
    Health. It turns already returned host-check JSON into stable table
    headers, table rows, summary text, counts, and color keys. It must not run
    Host Check, inspect the host, call the runtime client, or suggest executing
    commands.
45. `DiagnosticsEventUiState`: local presentation-state helpers for the
    Diagnostics Event Stream. It owns event table headers, level color keys, and
    visible/total/error/warning/info summary text. MainWindow keeps event
    insertion, filtering, and tab-badge updates.
46. `WorkspaceTabBadgeUiState`: local presentation-state helpers for
    high-frequency workspace tab badges. It owns badge text, issue marker, and
    tooltip rules from already collected counts. MainWindow keeps table count
    collection and actual tab assignment.
47. `WorkspaceTabBadgeTableAdapter`: read-only Qt table adapter for
    high-frequency workspace tab badge counts. It owns column constants and
    count extraction from Watch, Startup SDO, Free Run, I/O Variables,
    Consistency, State Machine, Diagnostics, and the Slave Evidence Matrix. I/O
    issue counting must reuse `ProcessDataTableAdapter` row extraction and
    `ProcessDataRowModel` issue rules instead of duplicating status heuristics.
    It must not set tab text, localize badge labels, call the runtime client,
    or run host diagnostics.
48. `WorkspaceBoundaryUiState`: local presentation-state helpers for the
    status-bar workspace boundary marker. It owns boundary labels, severity
    keys, and tooltip detail rules from the current workspace kind and already
    collected Overview matrix counts. MainWindow keeps QWidget-to-workspace
    mapping and QLabel application.
49. `DiagnosticsWorkspace`: Host Health and Event Stream presentation.
## Commenting Standard

- Prefer names and small functions over comments.
- Add a short comment only when it protects a boundary or explains a non-obvious
  workflow constraint.
- Good comments explain why a block must stay local-only, why a confirmation is
  required, or why a fallback order matters.
- Avoid comments that restate one line of code.

## Change Discipline

- Keep UI-only review actions local: they may update labels, selection, filters,
  clipboard, and project tables, but must not read SDOs, write SDOs, change
  EtherCAT state, toggle Free Run, rescan/connect, or run Host Health.
- Keep online actions explicit and visible through buttons, menu commands, or
  command-palette entries with existing confirmations.
- Update Help/About/project docs when adding user-visible workflows.
- For release收口 and UI-only edits, run `clang-format` on touched C++ files,
  `cmake --build build`, `cmake --build build --target release-smoke`, and the
  offscreen GUI startup smoke test.
- Run full `ctest --test-dir build --output-on-failure` before tagged releases,
  shared model refactors, CMake changes, or risky workflow changes.
- Limit `clang-format` to C++ source/header files. Do not apply it to
  `CMakeLists.txt`; CMake files require manual edits or a CMake-aware formatter.
