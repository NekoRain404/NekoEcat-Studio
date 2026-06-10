#include "SessionBriefModel.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3").arg(message, expected, actual));
  }
}

void expectStatus(SessionBriefStatus actual, SessionBriefStatus expected,
                  const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

SessionBriefRow rowByKind(const QVector<SessionBriefRow> &rows,
                          SessionBriefRowKind kind) {
  for (const auto &row : rows) {
    if (row.kind == kind) {
      return row;
    }
  }
  fail("missing Session Brief row kind");
  return {};
}

SessionBriefInput readyInput() {
  SessionBriefInput input;
  input.connected = true;
  input.hasSlaves = true;
  input.hasSelectedSlave = true;
  input.hasSdoRows = true;
  input.hasPdoRows = true;
  input.hasConsistencyCheck = true;
  input.currentSdoComplete = true;
  input.currentSdoEvidenceGroups = 1;
  input.currentSdoHasEvidence = true;
  input.hasWatchRows = true;
  input.freeRunEnabled = true;
  input.hasFreeRunRows = true;
  input.nextWorkflowStep = -1;
  return input;
}

void testReadyRowsAndActionKeys() {
  const QVector<SessionBriefRow> rows = buildSessionBriefRows(readyInput());
  expectEqual(rows.size(), 6, "brief row count");
  expectEqual(rowByKind(rows, SessionBriefRowKind::Target).actionKey, "target",
              "target action key");
  expectEqual(rowByKind(rows, SessionBriefRowKind::Gate).actionKey, "gate",
              "gate action key");
  expectEqual(rowByKind(rows, SessionBriefRowKind::Map).actionKey, "map",
              "map action key");
  expectEqual(rowByKind(rows, SessionBriefRowKind::CurrentSdo).actionKey,
              "currentSdo", "current SDO action key");
  expectEqual(rowByKind(rows, SessionBriefRowKind::RuntimeEvidence).actionKey,
              "runtime", "runtime action key");
  expectEqual(rowByKind(rows, SessionBriefRowKind::Next).actionKey, "next",
              "next action key");

  for (const auto &row : rows) {
    expectStatus(row.status, SessionBriefStatus::Ready,
                 "ready input produces ready rows");
  }
}

void testStatusRules() {
  SessionBriefInput input = readyInput();
  input.connected = false;
  expectStatus(
      rowByKind(buildSessionBriefRows(input), SessionBriefRowKind::Target)
          .status,
      SessionBriefStatus::Action, "offline target needs action");

  input = readyInput();
  input.consistencyErrors = 1;
  expectStatus(
      rowByKind(buildSessionBriefRows(input), SessionBriefRowKind::Gate).status,
      SessionBriefStatus::Error, "consistency errors are errors");

  input = readyInput();
  input.hasFailedOdEvidence = true;
  expectStatus(
      rowByKind(buildSessionBriefRows(input), SessionBriefRowKind::Map).status,
      SessionBriefStatus::Warning, "failed OD evidence warns");

  input = readyInput();
  input.currentSdoEvidenceGroups = 2;
  expectStatus(
      rowByKind(buildSessionBriefRows(input), SessionBriefRowKind::CurrentSdo)
          .status,
      SessionBriefStatus::Warning, "conflicting SDO evidence warns");

  input = readyInput();
  input.startupDiffs = 1;
  expectStatus(rowByKind(buildSessionBriefRows(input),
                         SessionBriefRowKind::RuntimeEvidence)
                   .status,
               SessionBriefStatus::Warning, "startup diffs warn");

  input = readyInput();
  input.nextWorkflowStep = 3;
  expectStatus(
      rowByKind(buildSessionBriefRows(input), SessionBriefRowKind::Next).status,
      SessionBriefStatus::Action, "open workflow row needs action");
}

void testStatusKeys() {
  expectEqual(sessionBriefStatusKey(SessionBriefStatus::Ready), "ready",
              "ready status key");
  expectEqual(sessionBriefStatusKey(SessionBriefStatus::Action), "action",
              "action status key");
  expectEqual(sessionBriefStatusKey(SessionBriefStatus::Warning), "warning",
              "warning status key");
  expectEqual(sessionBriefStatusKey(SessionBriefStatus::Error), "error",
              "error status key");
  expectEqual(sessionBriefStatusKey(SessionBriefStatus::Info), "info",
              "info status key");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testReadyRowsAndActionKeys();
  testStatusRules();
  testStatusKeys();
  return 0;
}
