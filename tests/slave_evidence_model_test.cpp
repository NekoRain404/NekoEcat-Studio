#include "models/SlaveEvidenceModel.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
}

void expectPriority(SlaveEvidencePriority actual,
                    SlaveEvidencePriority expected, const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

void expectNext(SlaveEvidenceNextAction actual,
                SlaveEvidenceNextAction expected, const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

void expectRoute(SlaveEvidenceRouteTarget actual,
                 SlaveEvidenceRouteTarget expected, const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

bool hasRisk(const SlaveEvidenceRow &row, SlaveEvidenceRiskKind kind) {
  for (const auto &risk : row.risks) {
    if (risk.kind == kind) {
      return true;
    }
  }
  return false;
}

void testPriorityOrderingAndCounts() {
  SlaveEvidenceInput ready;
  ready.position = 3;
  ready.name = "Ready Drive";
  ready.state = "OP";
  ready.identityRows = 4;
  ready.odRows = 20;
  ready.pdoRows = 8;
  ready.watchRows = 3;
  ready.watchValueRows = 3;
  ready.processRows = 6;

  SlaveEvidenceInput action = ready;
  action.position = 2;
  action.processRows = 0;
  action.state = "INIT";

  SlaveEvidenceInput risk = ready;
  risk.position = 1;
  risk.watchValueRows = 0;
  risk.state = "PREOP";

  SlaveEvidenceInput fault = ready;
  fault.position = 4;
  fault.startupRows = 2;
  fault.startupDiffs = 1;

  const SlaveEvidenceMatrix matrix =
      buildSlaveEvidenceMatrix({ready, action, risk, fault});

  expectEqual(matrix.rows.size(), 4, "matrix keeps all rows");
  expectEqual(matrix.readyRows, 1, "ready count");
  expectEqual(matrix.actionRows, 1, "action count");
  expectEqual(matrix.riskRows, 2, "risk count");
  expectEqual(matrix.evidenceGaps, 1, "evidence gap count");

  expectEqual(matrix.rows.at(0).position, 4, "P0 row sorts first");
  expectPriority(matrix.rows.at(0).priority, SlaveEvidencePriority::Fault,
                 "startup diff is P0");
  expectEqual(matrix.rows.at(1).position, 1, "P1 row sorts before action");
  expectPriority(matrix.rows.at(1).priority, SlaveEvidencePriority::Risk,
                 "missing PREOP watch value is P1");
  expectEqual(matrix.rows.at(2).position, 2, "P2 row sorts before ready");
  expectPriority(matrix.rows.at(2).priority, SlaveEvidencePriority::Action,
                 "partial INIT evidence is P2");
  expectEqual(matrix.rows.at(3).position, 3, "ready row sorts last");
  expectPriority(matrix.rows.at(3).priority, SlaveEvidencePriority::Ready,
                 "complete row is P3");
}

void testNextActionOrder() {
  SlaveEvidenceInput input;
  input.position = 1;
  input.name = "Drive";
  input.state = "SAFEOP";
  input.identityRows = 1;

  SlaveEvidenceMatrix matrix = buildSlaveEvidenceMatrix({input});
  expectNext(matrix.rows.first().nextAction, SlaveEvidenceNextAction::ReviewOd,
             "missing OD is reviewed before PDO");

  input.odRows = 10;
  matrix = buildSlaveEvidenceMatrix({input});
  expectNext(matrix.rows.first().nextAction, SlaveEvidenceNextAction::LoadPdo,
             "missing PDO is next after OD");

  input.pdoRows = 4;
  matrix = buildSlaveEvidenceMatrix({input});
  expectNext(matrix.rows.first().nextAction, SlaveEvidenceNextAction::AddWatch,
             "missing Watch follows PDO");

  input.watchRows = 2;
  input.watchValueRows = 2;
  input.startupRows = 1;
  input.startupDiffs = 1;
  matrix = buildSlaveEvidenceMatrix({input});
  expectNext(matrix.rows.first().nextAction,
             SlaveEvidenceNextAction::ReviewStartup,
             "startup diff follows Watch");

  input.startupDiffs = 0;
  input.processRows = 0;
  matrix = buildSlaveEvidenceMatrix({input});
  expectNext(matrix.rows.first().nextAction,
             SlaveEvidenceNextAction::ValidateProcess,
             "SAFEOP still needs process evidence");
}

void testRiskKinds() {
  SlaveEvidenceInput input;
  input.position = 1;
  input.state = "OP";
  input.startupDiffs = 2;
  input.mapIssues = 1;
  input.topologyIssue = true;
  input.driveFault = true;

  const SlaveEvidenceRow row = buildSlaveEvidenceMatrix({input}).rows.first();
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::IdentityMissing),
             "missing identity risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::OdMissing), "missing OD risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::PdoMissing),
             "missing PDO risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::WatchMissing),
             "missing Watch risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::ProcessMissing),
             "missing process risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::StartupDiff),
             "startup diff risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::PdoMapIssue), "PDO map risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::TopologyBaselineIssue),
             "topology risk");
  expectTrue(hasRisk(row, SlaveEvidenceRiskKind::DriveFault),
             "drive fault risk");
}

void testRouteTargets() {
  SlaveEvidenceInput input;
  input.position = 1;
  input.name = "Drive";
  input.state = "OP";
  input.identityRows = 1;

  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::ObjectDictionary,
      "missing OD routes to Object Dictionary");

  input.odRows = 10;
  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::PdoMap, "missing PDO routes to PDO Map");

  input.pdoRows = 4;
  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::Watch, "missing Watch routes to Watch");

  input.watchRows = 2;
  input.watchValueRows = 2;
  input.startupRows = 1;
  input.startupDiffs = 1;
  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::Startup, "startup diff routes to Startup SDO");

  input.startupDiffs = 0;
  input.processRows = 0;
  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::Process,
      "missing OP process evidence routes to Free Run process");

  input.processRows = 3;
  input.mapIssues = 1;
  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::Process,
      "PDO map issue routes to Free Run process");

  input.mapIssues = 0;
  input.topologyIssue = true;
  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::StateMachine,
      "topology risk routes to State Machine");

  input.topologyIssue = false;
  expectRoute(
      slaveEvidenceRouteTarget(buildSlaveEvidenceMatrix({input}).rows.first()),
      SlaveEvidenceRouteTarget::Overview, "ready row stays on Overview");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testPriorityOrderingAndCounts();
  testNextActionOrder();
  testRiskKinds();
  testRouteTargets();
  return 0;
}
