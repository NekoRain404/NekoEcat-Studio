#include "ui_state/WorkspaceBoundaryUiState.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3").arg(message, expected, actual));
  }
}

void expectContains(const QStringList &actual, const QString &expected,
                    const QString &message) {
  if (!actual.contains(expected)) {
    fail(QString("%1: missing %2").arg(message, expected));
  }
}

WorkspaceBoundaryTexts englishTexts() {
  return {
      .workspacePattern = "Workspace: %1",
      .overviewLabel = "Boundary: Mixed",
      .overviewMixedActions = "Overview mixes local and online actions.",
      .overviewLocalEvidence = "Overview review remains local.",
      .overviewMatrixPattern = "Matrix queue: P0 %1 | P1 %2 | P2 %3 | P3 %4",
      .objectDictionaryLabel = "Boundary: SDO",
      .objectDictionaryLocalFill = "Selecting rows only fills local SDO.",
      .objectDictionaryOnlineAccess = "Read and Write explicitly use SDO.",
      .pdoMapLabel = "Boundary: Online PDO",
      .pdoMapLoadedLocal = "PDO Map review is local once loaded.",
      .pdoMapLocalFill = "Alt+Enter fills the SDO target locally.",
      .watchLabel = "Boundary: Watch Reads",
      .watchReadsOnline = "Refresh and Auto poll SDO objects.",
      .watchStartupLocal = "Startup sync is local until Apply.",
      .startupSdoLabel = "Boundary: Startup Danger",
      .startupSdoLocalEditing = "Editing and preflight are local.",
      .startupSdoOnlineApply = "Apply and Verify use online SDO access.",
      .freeRunLabel = "Boundary: Process Data",
      .freeRunProcessData = "Free Run can exchange process data.",
      .freeRunLocalFiltering = "Filtering rows is local.",
      .ioVariablesLabel = "Boundary: Engineering",
      .ioVariablesMergedEvidence = "I/O Variables merges loaded evidence.",
      .ioVariablesLocalEditing = "Alias and exports are local.",
      .consistencyLabel = "Boundary: Local Gate",
      .consistencyLoadedEvidence = "Consistency uses loaded evidence.",
      .consistencyLocalNavigation = "Open Evidence navigates locally.",
      .stateMachineLabel = "Boundary: State Danger",
      .stateMachineOnlineRequests = "State buttons request state changes.",
      .stateMachineConfirmation = "State requests keep confirmations.",
      .diagnosticsLabel = "Boundary: Host",
      .diagnosticsHostOnly = "Diagnostics is the only Host Health workspace.",
      .diagnosticsHostCheck = "Running Host Check inspects the host.",
      .esiLabel = "Boundary: File/ESI",
      .esiFileEvidence = "ESI review is file evidence.",
      .esiImportAction = "Importing ESI XML is a file action.",
      .notesLabel = "Boundary: Project",
      .notesLocalRecords = "Notes are project-local records.",
      .rawEvidenceLabel = "Boundary: Raw Evidence",
      .rawEvidenceCachedOutput = "Raw output pages show cached output.",
  };
}

void testOverviewActionAndWarningStates() {
  WorkspaceBoundaryCounts counts;
  counts.matrixP0 = 0;
  counts.matrixP1 = 0;
  counts.matrixP2 = 3;
  counts.matrixP3 = 4;

  WorkspaceBoundaryUiState state = buildWorkspaceBoundaryUiState(
      WorkspaceBoundaryKind::Overview, "Overview 3", counts, englishTexts());
  expectEqual(state.label, "Boundary: Mixed", "overview label");
  expectEqual(state.severityKey, "action", "overview action severity");
  expectContains(state.details, "Workspace: Overview 3", "workspace detail");
  expectContains(state.details, "Matrix queue: P0 0 | P1 0 | P2 3 | P3 4",
                 "overview matrix detail");
  expectEqual(state.tooltip, state.details.join('\n'), "overview tooltip");

  counts.matrixP1 = 1;
  state = buildWorkspaceBoundaryUiState(WorkspaceBoundaryKind::Overview,
                                        "Overview", counts, englishTexts());
  expectEqual(state.severityKey, "warning", "overview warning severity");
}

void testHighRiskWorkspaceKinds() {
  WorkspaceBoundaryUiState state = buildWorkspaceBoundaryUiState(
      WorkspaceBoundaryKind::StartupSdo, "Startup SDO", {}, englishTexts());
  expectEqual(state.label, "Boundary: Startup Danger", "startup label");
  expectEqual(state.severityKey, "error", "startup severity");
  expectContains(state.details, "Apply and Verify use online SDO access.",
                 "startup online detail");

  state = buildWorkspaceBoundaryUiState(WorkspaceBoundaryKind::FreeRun,
                                        "Free Run", {}, englishTexts());
  expectEqual(state.label, "Boundary: Process Data", "free run label");
  expectEqual(state.severityKey, "error", "free run severity");

  state = buildWorkspaceBoundaryUiState(WorkspaceBoundaryKind::StateMachine,
                                        "State Machine", {}, englishTexts());
  expectEqual(state.label, "Boundary: State Danger", "state machine label");
  expectEqual(state.severityKey, "error", "state machine severity");
}

void testDiagnosticsAndLocalWorkspaceKinds() {
  WorkspaceBoundaryUiState state = buildWorkspaceBoundaryUiState(
      WorkspaceBoundaryKind::Diagnostics, "Diagnostics", {}, englishTexts());
  expectEqual(state.label, "Boundary: Host", "diagnostics label");
  expectEqual(state.severityKey, "warning", "diagnostics severity");
  expectContains(state.details,
                 "Diagnostics is the only Host Health workspace.",
                 "diagnostics detail");

  state = buildWorkspaceBoundaryUiState(WorkspaceBoundaryKind::Consistency,
                                        "Consistency", {}, englishTexts());
  expectEqual(state.label, "Boundary: Local Gate", "consistency label");
  expectEqual(state.severityKey, "ok", "consistency severity");

  state = buildWorkspaceBoundaryUiState(WorkspaceBoundaryKind::RawEvidence,
                                        "Raw", {}, englishTexts());
  expectEqual(state.label, "Boundary: Raw Evidence", "raw label");
  expectEqual(state.severityKey, "neutral", "raw severity");
  expectContains(state.details, "Raw output pages show cached output.",
                 "raw detail");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testOverviewActionAndWarningStates();
  testHighRiskWorkspaceKinds();
  testDiagnosticsAndLocalWorkspaceKinds();
  return 0;
}
