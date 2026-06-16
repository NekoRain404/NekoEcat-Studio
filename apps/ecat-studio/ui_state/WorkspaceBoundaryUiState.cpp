// Workspace boundary banner text: title, detail, severity, and actions.
#include "WorkspaceBoundaryUiState.h"

namespace {

// Helper to set label, severity, and details in one call to reduce repetition.
void setBoundary(WorkspaceBoundaryUiState *state, const QString &label,
                 const QString &severity, const QStringList &details) {
  state->label = label;
  state->severityKey = severity;
  state->details << details;
}

} // namespace

// Maps each workspace kind to its boundary banner: label, severity, detail lines, and tooltip.
WorkspaceBoundaryUiState
buildWorkspaceBoundaryUiState(WorkspaceBoundaryKind kind,
                              const QString &workspaceName,
                              const WorkspaceBoundaryCounts &counts,
                              const WorkspaceBoundaryTexts &texts) {
  WorkspaceBoundaryUiState state;
  state.label = texts.rawEvidenceLabel;
  state.severityKey = QStringLiteral("neutral");
  state.details << texts.workspacePattern.arg(workspaceName);

  switch (kind) {
  case WorkspaceBoundaryKind::Overview:
    setBoundary(&state, texts.overviewLabel, QStringLiteral("action"),
                {texts.overviewMixedActions, texts.overviewLocalEvidence,
                 texts.overviewMatrixPattern.arg(counts.matrixP0)
                     .arg(counts.matrixP1)
                     .arg(counts.matrixP2)
                     .arg(counts.matrixP3)});
    if (counts.matrixP0 + counts.matrixP1 > 0) {
      state.severityKey = QStringLiteral("warning");
    }
    break;
  case WorkspaceBoundaryKind::ObjectDictionary:
    setBoundary(
        &state, texts.objectDictionaryLabel, QStringLiteral("warning"),
        {texts.objectDictionaryLocalFill, texts.objectDictionaryOnlineAccess});
    break;
  case WorkspaceBoundaryKind::PdoMap:
    setBoundary(&state, texts.pdoMapLabel, QStringLiteral("action"),
                {texts.pdoMapLoadedLocal, texts.pdoMapLocalFill});
    break;
  case WorkspaceBoundaryKind::Watch:
    setBoundary(&state, texts.watchLabel, QStringLiteral("warning"),
                {texts.watchReadsOnline, texts.watchStartupLocal});
    break;
  case WorkspaceBoundaryKind::StartupSdo:
    setBoundary(&state, texts.startupSdoLabel, QStringLiteral("error"),
                {texts.startupSdoLocalEditing, texts.startupSdoOnlineApply});
    break;
  case WorkspaceBoundaryKind::FreeRun:
    setBoundary(&state, texts.freeRunLabel, QStringLiteral("error"),
                {texts.freeRunProcessData, texts.freeRunLocalFiltering});
    break;
  case WorkspaceBoundaryKind::IoVariables:
    setBoundary(
        &state, texts.ioVariablesLabel, QStringLiteral("action"),
        {texts.ioVariablesMergedEvidence, texts.ioVariablesLocalEditing});
    break;
  case WorkspaceBoundaryKind::Consistency:
    setBoundary(
        &state, texts.consistencyLabel, QStringLiteral("ok"),
        {texts.consistencyLoadedEvidence, texts.consistencyLocalNavigation});
    break;
  case WorkspaceBoundaryKind::StateMachine:
    setBoundary(
        &state, texts.stateMachineLabel, QStringLiteral("error"),
        {texts.stateMachineOnlineRequests, texts.stateMachineConfirmation});
    break;
  case WorkspaceBoundaryKind::Diagnostics:
    setBoundary(&state, texts.diagnosticsLabel, QStringLiteral("warning"),
                {texts.diagnosticsHostOnly, texts.diagnosticsHostCheck});
    break;
  case WorkspaceBoundaryKind::RtTest:
    setBoundary(&state, texts.rtTestLabel, QStringLiteral("action"),
                {texts.rtTestOnlineCycle, texts.rtTestLocalStats});
    break;
  case WorkspaceBoundaryKind::Esi:
    setBoundary(&state, texts.esiLabel, QStringLiteral("neutral"),
                {texts.esiFileEvidence, texts.esiImportAction});
    break;
  case WorkspaceBoundaryKind::Notes:
    setBoundary(&state, texts.notesLabel, QStringLiteral("neutral"),
                {texts.notesLocalRecords});
    break;
  case WorkspaceBoundaryKind::RawEvidence:
    setBoundary(&state, texts.rawEvidenceLabel, QStringLiteral("neutral"),
                {texts.rawEvidenceCachedOutput});
    break;
  }

  state.tooltip = state.details.join('\n');
  return state;
}
