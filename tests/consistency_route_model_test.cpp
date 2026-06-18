// Unit tests for ConsistencyEvidenceRouteModel.
#include "models/ConsistencyModel.h"

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

void expectKind(ConsistencyEvidenceRouteKind actual,
                ConsistencyEvidenceRouteKind expected, const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

void testAddressAndStartupParsing() {
  ConsistencyEvidenceAddress address =
      parseConsistencyEvidenceAddress("#12 0x6040:0x0");
  expectEqual(address.position, 12, "address position");
  expectEqual(address.index, "0x6040", "address index");
  expectEqual(address.subIndex, "0x00", "address subindex");

  address = parseConsistencyEvidenceAddress("#3 state");
  expectEqual(address.position, 3, "position-only address");
  expectEqual(address.index, QString(), "position-only index");
  expectEqual(parseConsistencyStartupRow("Startup row 4"), 3,
              "English startup row");
  expectEqual(parseConsistencyStartupRow("启动行 2"), 1, "Chinese startup row");
}

void testRouteKinds() {
  expectKind(consistencyEvidenceRouteDecision({.scope = "Topology"}).kind,
             ConsistencyEvidenceRouteKind::Topology, "topology route");
  expectKind(consistencyEvidenceRouteDecision({.scope = "启动"}).kind,
             ConsistencyEvidenceRouteKind::Startup, "startup route");
  expectKind(
      consistencyEvidenceRouteDecision({.evidence = "watch missing"}).kind,
      ConsistencyEvidenceRouteKind::Watch, "watch route");
  expectKind(consistencyEvidenceRouteDecision({.scope = "I/O Variables"}).kind,
             ConsistencyEvidenceRouteKind::IoVariables, "default I/O route");
}

void testIoScopes() {
  expectEqual(consistencyEvidenceIoScope({.action = "PLC alias missing"}),
              QString::fromLatin1(kConsistencyIoScopePlcIssues), "PLC scope");
  expectEqual(consistencyEvidenceIoScope({.evidence = "Startup diff"}),
              QString::fromLatin1(kConsistencyIoScopeStartupDiff),
              "startup diff scope");
  expectEqual(consistencyEvidenceIoScope({.actual = "Missing Raw value"}),
              QString::fromLatin1(kConsistencyIoScopeMissingValue),
              "missing value scope");
  expectEqual(consistencyEvidenceIoScope({.evidence = "PDO map warning"}),
              QString::fromLatin1(kConsistencyIoScopePdo), "PDO scope");
  expectEqual(consistencyEvidenceIoScope({}),
              QString::fromLatin1(kConsistencyIoScopeAll), "default scope");
}

void testDecisionPayload() {
  const ConsistencyEvidenceRouteDecision route =
      consistencyEvidenceRouteDecision(
          {.scope = "Startup",
           .target = "#7 0x6060:0x00 Startup row 3",
           .evidence = "diff"});
  expectKind(route.kind, ConsistencyEvidenceRouteKind::Startup,
             "decision kind");
  expectEqual(route.address.position, 7, "decision position");
  expectEqual(route.address.index, "0x6060", "decision index");
  expectEqual(route.address.subIndex, "0x00", "decision subindex");
  expectEqual(route.startupRow, 2, "decision startup row");
  expectEqual(route.ioScope,
              QString::fromLatin1(kConsistencyIoScopeStartupDiff),
              "decision I/O scope");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testAddressAndStartupParsing();
  testRouteKinds();
  testIoScopes();
  testDecisionPayload();
  return 0;
}
