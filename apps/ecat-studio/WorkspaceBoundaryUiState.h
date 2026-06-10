#pragma once

#include <QString>
#include <QStringList>

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

struct WorkspaceBoundaryCounts {
  int matrixP0 = 0;
  int matrixP1 = 0;
  int matrixP2 = 0;
  int matrixP3 = 0;
};

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

struct WorkspaceBoundaryUiState {
  QString label;
  QString severityKey;
  QStringList details;
  QString tooltip;
};

WorkspaceBoundaryUiState buildWorkspaceBoundaryUiState(
    WorkspaceBoundaryKind kind, const QString &workspaceName,
    const WorkspaceBoundaryCounts &counts, const WorkspaceBoundaryTexts &texts);
