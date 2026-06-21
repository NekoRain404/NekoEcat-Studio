// Computes enabled/disabled state for all toolbar and context-menu actions.
#include "utils/ActionAvailabilityHelper.h"

namespace ActionAvailabilityHelper {

QVector<ActionState> computeActionStates(const ActionFlags &f) {
  QVector<ActionState> states;
  states.reserve(90);

  // ── Connection / Topology ──────────────────────────────────────
  states << ActionState{"menuConnectAction", true};
  states << ActionState{"connectAction", true};
  states << ActionState{"overviewConnect", true};
  states << ActionState{"menuRefreshAction", f.connected};
  states << ActionState{"refreshAction", f.connected};
  states << ActionState{"overviewRefresh", f.connected};
  states << ActionState{"overviewRunNext", f.nextCommissioningStep >= 0};
  states << ActionState{"menuRescanAction", f.connected};
  states << ActionState{"rescanAction", f.connected};
  states << ActionState{"captureTopologyBaseline", f.connected && f.hasAnySlave};
  states << ActionState{"clearTopologyBaseline", f.hasTopologyBaseline};

  // ── SDO Read/Write ─────────────────────────────────────────────
  states << ActionState{"readSdo", f.connected && f.hasSlave};
  states << ActionState{"readTargetSdo", f.connected && f.hasCurrentSdoTarget};
  states << ActionState{"readSelectedDictionary",
                        f.connected && f.hasSlave && f.hasDictionarySelection};
  states << ActionState{"readVisibleDictionary",
                        f.connected && f.hasSlave && f.hasVisibleDictionaryRows};
  states << ActionState{"readFailedDictionary",
                        f.connected && f.hasSlave && f.hasFailedSdoEvidence};
  states << ActionState{"writeSdo",
                        f.connected && f.hasSlave && f.selectedSdoWritable};
  states << ActionState{"writeTargetSdo", f.connected && f.hasCurrentSdoTarget &&
                                             f.selectedSdoWritable};
  states << ActionState{"useSdoValue",
                        f.selectedSdoWritable && f.sdoValueNonEmpty};
  states << ActionState{"useSdoEvidence",
                        f.selectedSdoWritable && f.sdoEvidenceNonEmpty};
  states << ActionState{"pickSdoEvidence",
                        f.selectedSdoWritable && f.hasSdoEvidenceCandidates};

  // ── Context Menu ───────────────────────────────────────────────
  states << ActionState{"contextObjectDictionary", f.hasSlave};
  states << ActionState{"contextPdoMap", f.hasSlave};
  states << ActionState{"contextWatch", f.hasSlave};
  states << ActionState{"contextFreeRun", f.connected && f.hasSlave};
  states << ActionState{"contextDiagnostics", true};
  states << ActionState{"contextRefreshSlave", f.connected && f.hasSlave};
  states << ActionState{"contextPrepareSnapshot", f.connected && f.hasSlave};

  // ── Startup SDO ────────────────────────────────────────────────
  states << ActionState{"addStartupSdo", f.connected && f.hasSlave};
  states << ActionState{"startupTargetSdo",
                        f.connected && f.hasCurrentSdoTarget};

  // ── Watch SDO ──────────────────────────────────────────────────
  states << ActionState{"addWatchSdo", f.hasSlave};
  states << ActionState{"watchTargetSdo", f.hasCurrentSdoTarget};

  // ── SDO Target Panel ───────────────────────────────────────────
  states << ActionState{"reviewSdoWriteDelta", f.sdoWriteDeltaReviewAvailable};
  states << ActionState{"runSdoTargetRowAction", f.hasSelectedObjectRow};
  states << ActionState{"copySdoTargetRowEvidence", f.hasSelectedObjectRow};
  states << ActionState{"copySdoEvidenceDigest", f.hasCurrentSdoTarget};
  states << ActionState{"openSdoWatchLink", f.currentSdoWatchRow >= 0};
  states << ActionState{"openSdoStartupLink", f.currentSdoStartupRow >= 0};
  states << ActionState{"openSdoBookmarkLink", f.currentSdoBookmarkRow >= 0};
  states << ActionState{"openSdoTargetTrailLink",
                        f.currentSdoTargetTrailRow >= 0};

  // ── Dictionary / Bookmarks / Trail ─────────────────────────────
  states << ActionState{"watchSelectedDictionary",
                        f.hasSlave && f.hasDictionarySelection};
  states << ActionState{"watchVisibleDictionary",
                        f.hasSlave && f.hasVisibleDictionaryRows};
  states << ActionState{"startupSelectedEvidence",
                        f.hasSlave && f.hasDictionaryValueSelection};
  states << ActionState{"bookmarkTargetSdo",
                        f.hasSlave && f.hasCurrentSdoTarget};
  states << ActionState{"bookmarkSelectedDictionary",
                        f.hasSlave && f.hasDictionarySelection};
  states << ActionState{"fillBookmarkSdo", f.hasObjectBookmarkSelection};
  states << ActionState{"watchBookmarkSdo", f.hasObjectBookmarkSelection};
  states << ActionState{"startupBookmarkSdo", f.hasObjectBookmarkSelection};
  states << ActionState{"removeBookmarkSdo", f.hasObjectBookmarkSelection};
  states << ActionState{"restoreSdoTargetTrail", f.hasSdoTargetTrailSelection};
  states << ActionState{"watchSdoTargetTrail", f.hasSdoTargetTrailSelection};
  states << ActionState{"bookmarkSdoTargetTrail",
                        f.hasSdoTargetTrailSelection};
  states << ActionState{"startupSdoTargetTrail",
                        f.canCreateStartupFromSdoTargetTrail};
  states << ActionState{"removeSdoTargetTrail", f.hasSdoTargetTrailSelection};
  states << ActionState{"clearSdoTargetTrail", f.hasSdoTargetTrailRows};

  // ── PDO ────────────────────────────────────────────────────────
  states << ActionState{"addSelectedPdoWatch", f.hasSlave && f.hasPdoSelection};

  // ── IO Variables ───────────────────────────────────────────────
  states << ActionState{"refreshIoVariables", f.ioVariableTableExists};
  states << ActionState{"fillIoVariableSdo", f.hasIoVariableSelection};
  states << ActionState{"readIoVariableSdo",
                        f.connected && f.hasIoVariableSelection};
  states << ActionState{"watchSelectedIoVariables", f.hasIoVariableSelection};
  states << ActionState{"watchVisibleIoVariables", f.hasVisibleIoVariables};
  states << ActionState{"startupSelectedIoVariables",
                        f.hasIoVariableValueSelection};
  states << ActionState{"startupVisibleIoVariables",
                        f.hasVisibleIoVariableValues};
  states << ActionState{"editIoVariableMetadata", f.hasIoVariableSelection};
  states << ActionState{"bulkNameIoVariables",
                        f.hasIoVariableSelection || f.hasVisibleIoVariables};
  states << ActionState{"reviewPlcHandoffAction", f.hasVisibleIoVariables};
  states << ActionState{"reviewPlcHandoff", f.hasVisibleIoVariables};
  states << ActionState{"copySelectedPlcDeclarations",
                        f.hasIoVariableSelection};
  states << ActionState{"copyVisiblePlcDeclarations", f.hasVisibleIoVariables};
  states << ActionState{"exportIoVariablesAction",
                        f.ioVariableTableHasRows};
  states << ActionState{"exportIoVariablesCsv", f.ioVariableTableHasRows};
  states << ActionState{"exportIoPlcSymbolsAction",
                        f.ioVariableTableHasRows};
  states << ActionState{"exportIoPlcSymbolsCsv", f.ioVariableTableHasRows};
  states << ActionState{"exportPlcDeclarationsAction", f.hasVisibleIoVariables};

  // ── Consistency ────────────────────────────────────────────────
  states << ActionState{"refreshConsistency", f.consistencyTableExists};
  states << ActionState{"openIoVariablesFromConsistency",
                        f.ioVariableTabIndex >= 0};

  // ── History ────────────────────────────────────────────────────
  states << ActionState{"watchSelectedHistory", f.hasHistorySelection};
  states << ActionState{"startupFromSelectedHistory",
                        f.hasHistoryValueSelection};

  // ── Watch ──────────────────────────────────────────────────────
  states << ActionState{"addCia402WatchPreset", f.hasSlave};
  states << ActionState{"refreshWatch",
                        f.connected && f.watchTableHasRows};
  states << ActionState{"captureWatchBaseline", f.watchTableHasRows};
  states << ActionState{"clearWatchBaseline", f.watchTableHasRows};
  states << ActionState{"startupFromSelectedWatch", f.hasWatchValueSelection};
  states << ActionState{"syncStartupFromWatch", f.hasWatchValueSelection};
  states << ActionState{"clearWatch", f.watchTableHasRows};

  // ── Host ───────────────────────────────────────────────────────
  states << ActionState{"runHostCheck", f.connected};
  states << ActionState{"copyHostCommand", f.hasHostCommand};

  // ── State Machine ──────────────────────────────────────────────
  const bool stateOk = f.connected && f.hasSlave;
  states << ActionState{"initAction", stateOk};
  states << ActionState{"preOpAction", stateOk};
  states << ActionState{"safeOpAction", stateOk};
  states << ActionState{"opAction", stateOk};
  states << ActionState{"contextInit", stateOk};
  states << ActionState{"contextPreOp", stateOk};
  states << ActionState{"contextSafeOp", stateOk};
  states << ActionState{"contextOp", stateOk};
  states << ActionState{"allInitAction", f.connected && f.hasAnySlave};
  states << ActionState{"allPreOpAction", f.connected && f.hasAnySlave};
  states << ActionState{"allSafeOpAction", f.connected && f.hasAnySlave};
  states << ActionState{"allOpAction", f.connected && f.hasAnySlave};

  return states;
}

}  // namespace ActionAvailabilityHelper
