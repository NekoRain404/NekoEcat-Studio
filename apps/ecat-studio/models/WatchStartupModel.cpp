#include "WatchStartupModel.h"

#include "SdoEvidenceModel.h"
#include "helpers/StudioTextHelpers.h"

#include <QHash>

bool watchStartupHasTarget(int position, const QString &index,
                           const QString &subIndex) {
  return position >= 0 && !index.trimmed().isEmpty() &&
         !subIndex.trimmed().isEmpty();
}

QString watchStartupTargetKey(int position, const QString &index,
                              const QString &subIndex) {
  return sdoEvidenceKey(position, normalizeHexText(index, 4),
                        normalizeHexText(subIndex, 2));
}

WatchStartupWatchMatch
watchStartupMatchForWatchRow(const QVector<WatchStartupStartupRow> &startupRows,
                             const WatchStartupWatchRow &watchRow) {
  WatchStartupWatchMatch match;
  if (!watchStartupHasTarget(watchRow.position, watchRow.index,
                             watchRow.subIndex)) {
    match.state = WatchStartupDeltaState::NoTarget;
    return match;
  }

  const QString key = watchStartupTargetKey(watchRow.position, watchRow.index,
                                            watchRow.subIndex);
  for (const auto &startupRow : startupRows) {
    if (!watchStartupHasTarget(startupRow.position, startupRow.index,
                               startupRow.subIndex) ||
        watchStartupTargetKey(startupRow.position, startupRow.index,
                              startupRow.subIndex) != key) {
      continue;
    }
    ++match.matchingStartupRows;
    match.expectedValue = startupRow.value.trimmed();
    match.expectedType = startupRow.type.trimmed();
  }

  if (match.matchingStartupRows <= 0) {
    match.state = WatchStartupDeltaState::NoStartup;
  } else if (watchRow.value.trimmed().isEmpty() ||
             match.expectedValue.trimmed().isEmpty()) {
    match.state = WatchStartupDeltaState::Pending;
  } else {
    match.state = sdoValuesComparableEqual(watchRow.value, match.expectedValue)
                      ? WatchStartupDeltaState::Match
                      : WatchStartupDeltaState::Diff;
  }
  return match;
}

QVector<WatchStartupStartupDelta>
evaluateStartupWatchDeltas(const QVector<WatchStartupStartupRow> &startupRows,
                           const QVector<WatchStartupWatchRow> &watchRows) {
  QHash<QString, QString> watchValues;
  for (const auto &watchRow : watchRows) {
    if (!watchStartupHasTarget(watchRow.position, watchRow.index,
                               watchRow.subIndex)) {
      continue;
    }
    watchValues.insert(watchStartupTargetKey(watchRow.position, watchRow.index,
                                             watchRow.subIndex),
                       watchRow.value.trimmed());
  }

  QVector<WatchStartupStartupDelta> deltas;
  deltas.reserve(startupRows.size());
  for (const auto &startupRow : startupRows) {
    WatchStartupStartupDelta delta;
    delta.startupRow = startupRow.row;
    if (!watchStartupHasTarget(startupRow.position, startupRow.index,
                               startupRow.subIndex)) {
      delta.state = WatchStartupDeltaState::NoTarget;
      deltas.append(delta);
      continue;
    }

    const QString key = watchStartupTargetKey(
        startupRow.position, startupRow.index, startupRow.subIndex);
    if (!watchValues.contains(key)) {
      delta.state = WatchStartupDeltaState::MissingWatch;
      deltas.append(delta);
      continue;
    }

    delta.watchValue = watchValues.value(key);
    if (delta.watchValue.trimmed().isEmpty() ||
        startupRow.value.trimmed().isEmpty()) {
      delta.state = WatchStartupDeltaState::Pending;
    } else {
      delta.state = sdoValuesComparableEqual(delta.watchValue, startupRow.value)
                        ? WatchStartupDeltaState::Match
                        : WatchStartupDeltaState::Diff;
    }
    deltas.append(delta);
  }
  return deltas;
}

WatchStartupSummary
summarizeStartupWatchDeltas(const QVector<WatchStartupStartupDelta> &deltas) {
  WatchStartupSummary summary;
  for (const auto &delta : deltas) {
    switch (delta.state) {
    case WatchStartupDeltaState::Match:
      ++summary.matched;
      break;
    case WatchStartupDeltaState::Diff:
      ++summary.diff;
      break;
    case WatchStartupDeltaState::MissingWatch:
      ++summary.missingWatch;
      break;
    case WatchStartupDeltaState::NoTarget:
    case WatchStartupDeltaState::NoStartup:
    case WatchStartupDeltaState::Pending:
      ++summary.pending;
      break;
    }
  }
  return summary;
}

QVector<int>
startupRowsWithWatchDiffs(const QVector<WatchStartupStartupDelta> &deltas) {
  QVector<int> rows;
  for (const auto &delta : deltas) {
    if (delta.state == WatchStartupDeltaState::Diff && delta.startupRow >= 0) {
      rows.append(delta.startupRow);
    }
  }
  return rows;
}
