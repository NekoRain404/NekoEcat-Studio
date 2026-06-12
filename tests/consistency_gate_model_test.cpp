// Unit tests for ConsistencyGateModel.
#include "models/ConsistencyGateModel.h"

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

void expectLevel(ConsistencyIssueLevel actual, ConsistencyIssueLevel expected,
                 const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

void expectGateState(ConsistencyGateState actual, ConsistencyGateState expected,
                     const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

void testIssueLevelParsing() {
  expectLevel(consistencyIssueLevelFromText("Error"),
              ConsistencyIssueLevel::Error, "English error is recognized");
  expectLevel(consistencyIssueLevelFromText("错误"),
              ConsistencyIssueLevel::Error, "Chinese error is recognized");
  expectLevel(consistencyIssueLevelFromText("Warning"),
              ConsistencyIssueLevel::Warning, "English warning is recognized");
  expectLevel(consistencyIssueLevelFromText("警告"),
              ConsistencyIssueLevel::Warning, "Chinese warning is recognized");
  expectLevel(consistencyIssueLevelFromText("Ready"),
              ConsistencyIssueLevel::Ready, "English ready is recognized");
  expectLevel(consistencyIssueLevelFromText("就绪"),
              ConsistencyIssueLevel::Ready, "Chinese ready is recognized");
  expectLevel(consistencyIssueLevelFromText("Info"),
              ConsistencyIssueLevel::Info, "non-empty text is info");
  expectLevel(consistencyIssueLevelFromText("  "), ConsistencyIssueLevel::Empty,
              "blank text is empty");
}

void testIssueCountsAndBlocking() {
  ConsistencyIssueCounts counts;
  addConsistencyIssueLevel(&counts, ConsistencyIssueLevel::Error);
  addConsistencyIssueLevel(&counts, ConsistencyIssueLevel::Warning);
  addConsistencyIssueLevel(&counts, ConsistencyIssueLevel::Ready);
  addConsistencyIssueLevel(&counts, ConsistencyIssueLevel::Info);
  addConsistencyIssueLevel(&counts, ConsistencyIssueLevel::Empty);

  expectEqual(counts.errors, 1, "counts errors");
  expectEqual(counts.warnings, 1, "counts warnings");
  expectEqual(counts.ready, 1, "counts ready rows");
  expectEqual(counts.infos, 1, "counts info rows");
  expectTrue(consistencyHasBlockingIssues(counts),
             "error or warning rows are blocking");
  expectTrue(!consistencyHasBlockingIssues({0, 0, 2, 3}),
             "info and ready rows are not blocking");
}

void testGateState() {
  expectGateState(consistencyGateState(false, false, {}),
                  ConsistencyGateState::NotRun,
                  "unavailable consistency check is not run");
  expectGateState(consistencyGateState(true, false, {}),
                  ConsistencyGateState::Stale,
                  "available but stale consistency check is stale");
  expectGateState(consistencyGateState(true, true, {1, 0, 0, 0}),
                  ConsistencyGateState::Blocking,
                  "fresh check with errors is blocking");
  expectGateState(consistencyGateState(true, true, {0, 1, 0, 0}),
                  ConsistencyGateState::Blocking,
                  "fresh check with warnings is blocking");
  expectGateState(consistencyGateState(true, true, {0, 0, 2, 3}),
                  ConsistencyGateState::Passed,
                  "fresh check without blockers passes");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testIssueLevelParsing();
  testIssueCountsAndBlocking();
  testGateState();
  return 0;
}
