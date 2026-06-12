// Unit tests for NextBestActionModel.
#include "models/NextBestActionModel.h"

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

void expectDecision(const NextBestActionDecision &actual,
                    NextBestActionKind kind, NextBestActionSeverity severity,
                    const QString &message) {
  if (actual.kind != kind || actual.severity != severity) {
    fail(message);
  }
}

NextBestActionInput readyInput() {
  NextBestActionInput input;
  input.workflow.connected = true;
  input.workflow.hasSlaves = true;
  input.workflow.hasSelectedSlave = true;
  input.workflow.hasSdoRows = true;
  input.workflow.hasPdoRows = true;
  input.workflow.hasWatchRows = true;
  input.workflow.hasConsistencyCheck = true;
  input.workflow.freeRunEnabled = true;
  return input;
}

void testPrimaryOrdering() {
  NextBestActionInput input;
  input.hasDiagnosticError = true;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::Connect,
                 NextBestActionSeverity::Action,
                 "offline connect outranks diagnostics");

  input = readyInput();
  input.hasDiagnosticError = true;
  input.workflow.hasSlaves = false;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::Diagnostics,
                 NextBestActionSeverity::Error,
                 "diagnostics outranks rescan after connected");

  input = readyInput();
  input.workflow.hasSdoRows = false;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::LoadOd,
                 NextBestActionSeverity::Action, "missing OD loads first");

  input = readyInput();
  input.workflow.hasFailedOdEvidence = true;
  input.workflow.hasPdoRows = false;
  expectDecision(chooseNextBestAction(input),
                 NextBestActionKind::FailedOdEvidence,
                 NextBestActionSeverity::Warning,
                 "failed OD evidence outranks missing PDO");

  input = readyInput();
  input.workflow.hasStartupWatchDiffs = true;
  input.workflow.hasConsistencyCheck = false;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::StartupDiffs,
                 NextBestActionSeverity::Warning,
                 "Startup diffs outrank consistency");
}

void testConsistencyFreeRunAndMatrixOrdering() {
  NextBestActionInput input = readyInput();
  input.workflow.hasConsistencyCheck = false;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::Consistency,
                 NextBestActionSeverity::Action,
                 "missing consistency check is actionable");

  input = readyInput();
  input.workflow.hasConsistencyBlockingIssues = true;
  input.consistencyBlockingIssueRow = 4;
  expectDecision(chooseNextBestAction(input),
                 NextBestActionKind::ConsistencyEvidenceIssue,
                 NextBestActionSeverity::Warning,
                 "blocking consistency evidence routes to evidence");

  input = readyInput();
  input.workflow.freeRunEnabled = false;
  input.workflow.hasFreeRunRows = false;
  input.matrixP0 = 1;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::FreeRun,
                 NextBestActionSeverity::Action,
                 "Free Run evidence outranks matrix review");

  input = readyInput();
  input.matrixP1 = 1;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::MatrixReview,
                 NextBestActionSeverity::Warning, "P1 matrix review warns");

  input = readyInput();
  input.matrixP2 = 1;
  expectDecision(chooseNextBestAction(input), NextBestActionKind::MatrixReview,
                 NextBestActionSeverity::Action, "P2 matrix review acts");

  input = readyInput();
  expectDecision(
      chooseNextBestAction(input), NextBestActionKind::CommandPalette,
      NextBestActionSeverity::Neutral, "ready session falls back to commands");
}

void testKeys() {
  expectEqual(nextBestActionKey(NextBestActionKind::Connect), "connect",
              "connect action key");
  expectEqual(nextBestActionKey(NextBestActionKind::ConsistencyEvidenceIssue),
              "consistencyEvidenceIssue", "consistency evidence action key");
  expectEqual(nextBestActionKey(NextBestActionKind::CommandPalette),
              "commandPalette", "command palette action key");
  expectEqual(nextBestActionSeverityKey(NextBestActionSeverity::Action),
              "action", "action severity key");
  expectEqual(nextBestActionSeverityKey(NextBestActionSeverity::Warning),
              "warning", "warning severity key");
  expectEqual(nextBestActionSeverityKey(NextBestActionSeverity::Neutral),
              "neutral", "neutral severity key");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testPrimaryOrdering();
  testConsistencyFreeRunAndMatrixOrdering();
  testKeys();
  return 0;
}
