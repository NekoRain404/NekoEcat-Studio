#pragma once

// Watch/Startup SDO delta comparison: baseline, startup, and live value diffs.


#include <QString>
#include <QVector>

struct WatchStartupWatchRow {
  int row = -1;
  int position = -1;
  QString index;
  QString subIndex;
  QString value;
  QString time;
  QString decoded;
  QString type;
  QString mode;
  QString baseline;
  QString baselineDelta;
  QString startup;
  QString startupDelta;
  bool changed = false;
};

struct WatchStartupStartupRow {
  int row = -1;
  int position = -1;
  QString index;
  QString subIndex;
  QString value;
  QString type;
  QString positionText;
  QString status;
  QString detail;
  QString watchValue;
  QString watchDelta;
};

enum class WatchStartupDeltaState {
  NoTarget,
  NoStartup,
  MissingWatch,
  Pending,
  Match,
  Diff,
};

struct WatchStartupWatchMatch {
  int matchingStartupRows = 0;
  QString expectedValue;
  QString expectedType;
  WatchStartupDeltaState state = WatchStartupDeltaState::NoStartup;
};

struct WatchStartupStartupDelta {
  int startupRow = -1;
  QString watchValue;
  WatchStartupDeltaState state = WatchStartupDeltaState::NoTarget;
};

struct WatchStartupSummary {
  int matched = 0;
  int diff = 0;
  int pending = 0;
  int missingWatch = 0;
};

bool watchStartupHasTarget(int position, const QString &index,
                           const QString &subIndex);
QString watchStartupTargetKey(int position, const QString &index,
                              const QString &subIndex);
WatchStartupWatchMatch
watchStartupMatchForWatchRow(const QVector<WatchStartupStartupRow> &startupRows,
                             const WatchStartupWatchRow &watchRow);
QVector<WatchStartupStartupDelta>
evaluateStartupWatchDeltas(const QVector<WatchStartupStartupRow> &startupRows,
                           const QVector<WatchStartupWatchRow> &watchRows);
WatchStartupSummary
summarizeStartupWatchDeltas(const QVector<WatchStartupStartupDelta> &deltas);
QVector<int>
startupRowsWithWatchDiffs(const QVector<WatchStartupStartupDelta> &deltas);
