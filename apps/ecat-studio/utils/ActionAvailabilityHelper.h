#pragma once

// Computes enabled/disabled state for all toolbar and context-menu actions.

#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>

namespace ActionAvailabilityHelper {

using ActionState = QPair<QString, bool>;

struct ActionFlags {
  bool connected = false;
  bool hasSlave = false;
  bool hasAnySlave = false;
  bool hasCurrentSdoTarget = false;
  bool sdoLoadedForSelected = false;
  bool pdoLoadedForSelected = false;
  bool hasDictionarySelection = false;
  bool hasDictionaryValueSelection = false;
  bool hasObjectBookmarks = false;
  bool hasObjectBookmarkSelection = false;
  bool hasSdoTargetTrailSelection = false;
  bool hasSdoTargetTrailRows = false;
  bool canCreateStartupFromSdoTargetTrail = false;
  bool hasVisibleDictionaryRows = false;
  bool hasPdoSelection = false;
  bool hasIoVariableSelection = false;
  bool hasVisibleIoVariables = false;
  bool hasIoVariableValueSelection = false;
  bool hasVisibleIoVariableValues = false;
  bool hasHistorySelection = false;
  bool hasHistoryValueSelection = false;
  bool hasWatchValueSelection = false;
  bool hasHostCommand = false;
  bool hasSelectedObjectRow = false;
  bool selectedSdoWritable = false;
  bool sdoValueNonEmpty = false;
  bool sdoEvidenceNonEmpty = false;
  bool hasSdoEvidenceCandidates = false;
  bool sdoWriteDeltaReviewAvailable = false;
  bool hasTopologyBaseline = false;
  bool hasFailedSdoEvidence = false;
  int currentSdoWatchRow = -1;
  int currentSdoStartupRow = -1;
  int currentSdoBookmarkRow = -1;
  int currentSdoTargetTrailRow = -1;
  int nextCommissioningStep = -1;
  bool watchTableHasRows = false;
  bool ioVariableTableExists = false;
  bool ioVariableTableHasRows = false;
  bool consistencyTableExists = false;
  int ioVariableTabIndex = -1;
};

// Returns the full list of (actionName, enabled) pairs for the given flags.
QVector<ActionState> computeActionStates(const ActionFlags &flags);

}  // namespace ActionAvailabilityHelper
