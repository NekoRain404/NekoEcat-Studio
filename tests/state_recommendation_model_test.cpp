#include "StateRecommendationModel.h"

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
    fail(QString("%1: expected '%2', got '%3'").arg(message, expected, actual));
  }
}

EthercatStateEvidence evidence(QString state) {
  EthercatStateEvidence item;
  item.currentState = state;
  return item;
}

void testInitRecommendsPreop() {
  expectEqual(recommendedEthercatState(evidence("INIT")), "PREOP",
              "INIT recommends PREOP");
  expectEqual(recommendedEthercatState(evidence(" Init ")), "PREOP",
              "state matching is trimmed and case-insensitive");
}

void testPreopRequiresPdoAndWatchEvidence() {
  EthercatStateEvidence item = evidence("PREOP");
  expectEqual(recommendedEthercatState(item), QString(),
              "PREOP without evidence has no recommendation");

  item.pdoLoaded = true;
  expectEqual(recommendedEthercatState(item), QString(),
              "PREOP with PDO but no Watch values has no recommendation");

  item.watchValueRows = 1;
  expectEqual(recommendedEthercatState(item), "SAFEOP",
              "PREOP with PDO and Watch values recommends SAFEOP");
}

void testSafeopRequiresCleanProcessEvidenceAndConsistency() {
  EthercatStateEvidence item = evidence("SAFEOP");
  item.pdoLoaded = true;
  item.watchValueRows = 1;
  item.freeRunRows = 1;
  item.consistencyOk = true;
  expectEqual(recommendedEthercatState(item), "OP",
              "SAFEOP with clean process evidence recommends OP");

  item.startupDiffs = 1;
  expectEqual(recommendedEthercatState(item), QString(),
              "SAFEOP with Startup diffs does not recommend OP");

  item.startupDiffs = 0;
  item.mapIssues = 1;
  expectEqual(recommendedEthercatState(item), QString(),
              "SAFEOP with map issues does not recommend OP");

  item.mapIssues = 0;
  item.consistencyOk = false;
  expectEqual(recommendedEthercatState(item), QString(),
              "SAFEOP with stale or failed consistency does not recommend OP");

  item.consistencyOk = true;
  item.freeRunRows = 0;
  expectEqual(recommendedEthercatState(item), QString(),
              "SAFEOP without process evidence does not recommend OP");
}

void testOpAndUnknownHaveNoRecommendation() {
  expectEqual(recommendedEthercatState(evidence("OP")), QString(),
              "OP has no next-state recommendation");
  expectEqual(recommendedEthercatState(evidence("BOOT")), QString(),
              "unknown state has no recommendation");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testInitRecommendsPreop();
  testPreopRequiresPdoAndWatchEvidence();
  testSafeopRequiresCleanProcessEvidenceAndConsistency();
  testOpAndUnknownHaveNoRecommendation();
  return 0;
}
