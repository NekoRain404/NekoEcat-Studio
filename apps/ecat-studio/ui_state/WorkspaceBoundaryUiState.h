#pragma once

// Workspace boundary banner text: title, detail, severity, and actions.


#include <QString>
#include <QStringList>

// Identifies which workspace tab the boundary banner applies to.
enum class WorkspaceBoundaryKind {
  Overview,
  ObjectDictionary,
  PdoMap,
  Watch,
  StartupSdo,
  FreeRun,
  IoVariables,
  Consistency,
  StateMachine,
  Diagnostics,
  Esi,
  Notes,
  RawEvidence,
};

// Matrix priority counts used by the overview boundary to adjust severity.
struct WorkspaceBoundaryCounts {
  int matrixP0 = 0;
  int matrixP1 = 0;
  int matrixP2 = 0;
  int matrixP3 = 0;
};

// All localized strings for every workspace boundary banner.
struct WorkspaceBoundaryTexts {
  QString workspacePattern;
  QString overviewLabel;
  QString overviewMixedActions;
  QString overviewLocalEvidence;
  QString overviewMatrixPattern;
  QString objectDictionaryLabel;
  QString objectDictionaryLocalFill;
  QString objectDictionaryOnlineAccess;
  QString pdoMapLabel;
  QString pdoMapLoadedLocal;
  QString pdoMapLocalFill;
  QString watchLabel;
  QString watchReadsOnline;
  QString watchStartupLocal;
  QString startupSdoLabel;
  QString startupSdoLocalEditing;
  QString startupSdoOnlineApply;
  QString freeRunLabel;
  QString freeRunProcessData;
  QString freeRunLocalFiltering;
  QString ioVariablesLabel;
  QString ioVariablesMergedEvidence;
  QString ioVariablesLocalEditing;
  QString consistencyLabel;
  QString consistencyLoadedEvidence;
  QString consistencyLocalNavigation;
  QString stateMachineLabel;
  QString stateMachineOnlineRequests;
  QString stateMachineConfirmation;
  QString diagnosticsLabel;
  QString diagnosticsHostOnly;
  QString diagnosticsHostCheck;
  QString esiLabel;
  QString esiFileEvidence;
  QString esiImportAction;
  QString notesLabel;
  QString notesLocalRecords;
  QString rawEvidenceLabel;
  QString rawEvidenceCachedOutput;
};

// Resolved boundary banner state with label, severity, detail lines, and tooltip.
struct WorkspaceBoundaryUiState {
  QString label;
  QString severityKey;
  QStringList details;
  QString tooltip;
};

// Maps a workspace kind to its boundary banner state.
WorkspaceBoundaryUiState buildWorkspaceBoundaryUiState(
    WorkspaceBoundaryKind kind, const QString &workspaceName,
    const WorkspaceBoundaryCounts &counts, const WorkspaceBoundaryTexts &texts);
