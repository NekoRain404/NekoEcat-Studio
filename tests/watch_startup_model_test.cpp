#include "models/WatchStartupModel.h"

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

void expectState(WatchStartupDeltaState actual, WatchStartupDeltaState expected,
                 const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

WatchStartupWatchRow watch(int row, int position, QString index,
                           QString subIndex, QString value) {
  return {row, position, index, subIndex, value};
}

WatchStartupStartupRow startup(int row, int position, QString index,
                               QString subIndex, QString value,
                               QString type = "uint16") {
  return {row, position, index, subIndex, value, type};
}

void testWatchRowMatch() {
  expectTrue(watchStartupHasTarget(2, "6040", "0"),
             "watch/startup target accepts complete addresses");
  expectTrue(!watchStartupHasTarget(-1, "6040", "0"),
             "watch/startup target rejects invalid positions");
  expectTrue(watchStartupTargetKey(2, "6040", "0") == "2|0x6040|0x00",
             "watch/startup target key normalizes addresses");

  const QVector<WatchStartupStartupRow> startupRows = {
      startup(0, 2, "6040", "0", "0x0006"),
      startup(1, 2, "0x6040", "00", "0x0006"),
  };
  const WatchStartupWatchMatch match = watchStartupMatchForWatchRow(
      startupRows, watch(4, 2, "0x6040", "0x00", "0x0006"));
  expectEqual(match.matchingStartupRows, 2,
              "watch target finds all matching startup rows");
  expectState(match.state, WatchStartupDeltaState::Match,
              "matching watch value reports match");
  expectTrue(match.expectedValue == "0x0006", "expected value is retained");
}

void testWatchRowPendingAndDiff() {
  const QVector<WatchStartupStartupRow> startupRows = {
      startup(0, 2, "6040", "0", "0x0006"),
  };
  expectState(watchStartupMatchForWatchRow(startupRows,
                                           watch(4, 2, "0x6040", "0x00", ""))
                  .state,
              WatchStartupDeltaState::Pending, "empty watch value is pending");
  expectState(watchStartupMatchForWatchRow(
                  startupRows, watch(4, 2, "0x6040", "0x00", "0x0007"))
                  .state,
              WatchStartupDeltaState::Diff,
              "different watch value reports diff");
}

void testStartupDeltasAndSummary() {
  const QVector<WatchStartupWatchRow> watchRows = {
      watch(0, 1, "6040", "0", "0x0006"),
      watch(1, 1, "6060", "0", "8"),
      watch(2, 1, "607a", "0", ""),
  };
  const QVector<WatchStartupStartupRow> startupRows = {
      startup(0, 1, "0x6040", "0x00", "0x0006"),
      startup(1, 1, "0x6060", "0x00", "9"),
      startup(2, 1, "0x607a", "0x00", "100"),
      startup(3, 1, "0x6081", "0x00", "1000"),
      startup(4, -1, "0x6040", "0x00", "0x0006"),
  };

  const auto deltas = evaluateStartupWatchDeltas(startupRows, watchRows);
  expectEqual(deltas.size(), 5, "one delta per startup row");
  expectState(deltas.at(0).state, WatchStartupDeltaState::Match,
              "same value reports match");
  expectState(deltas.at(1).state, WatchStartupDeltaState::Diff,
              "different value reports diff");
  expectState(deltas.at(2).state, WatchStartupDeltaState::Pending,
              "empty watch value reports pending");
  expectState(deltas.at(3).state, WatchStartupDeltaState::MissingWatch,
              "missing watch row reports missing watch");
  expectState(deltas.at(4).state, WatchStartupDeltaState::NoTarget,
              "invalid startup target reports no target");

  const WatchStartupSummary summary = summarizeStartupWatchDeltas(deltas);
  expectEqual(summary.matched, 1, "summary counts matches");
  expectEqual(summary.diff, 1, "summary counts diffs");
  expectEqual(summary.pending, 2, "summary counts pending/no-target rows");
  expectEqual(summary.missingWatch, 1, "summary counts missing watch rows");

  const QVector<int> diffRows = startupRowsWithWatchDiffs(deltas);
  expectEqual(diffRows.size(), 1, "diff row filter keeps only diff rows");
  expectEqual(diffRows.first(), 1, "diff row filter preserves startup row id");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testWatchRowMatch();
  testWatchRowPendingAndDiff();
  testStartupDeltasAndSummary();
  return 0;
}
