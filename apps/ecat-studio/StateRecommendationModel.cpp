#include "StateRecommendationModel.h"

QString recommendedEthercatState(const EthercatStateEvidence &evidence) {
  const QString state = evidence.currentState.trimmed().toUpper();
  if (state.contains("INIT")) {
    return "PREOP";
  }
  if (state.contains("PREOP")) {
    return evidence.pdoLoaded && evidence.watchValueRows > 0 ? QString("SAFEOP")
                                                             : QString();
  }
  if (state.contains("SAFEOP")) {
    return evidence.freeRunRows > 0 && evidence.startupDiffs <= 0 &&
                   evidence.mapIssues <= 0 && evidence.consistencyOk
               ? QString("OP")
               : QString();
  }
  return QString();
}
