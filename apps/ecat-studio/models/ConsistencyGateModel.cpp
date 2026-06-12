#include "ConsistencyGateModel.h"

ConsistencyIssueLevel consistencyIssueLevelFromText(const QString &level) {
  const QString normalized = level.toLower();
  if (normalized.contains("error") || normalized.contains("错误")) {
    return ConsistencyIssueLevel::Error;
  }
  if (normalized.contains("warning") || normalized.contains("警告")) {
    return ConsistencyIssueLevel::Warning;
  }
  if (normalized.contains("ready") || normalized.contains("就绪")) {
    return ConsistencyIssueLevel::Ready;
  }
  if (!normalized.trimmed().isEmpty()) {
    return ConsistencyIssueLevel::Info;
  }
  return ConsistencyIssueLevel::Empty;
}

void addConsistencyIssueLevel(ConsistencyIssueCounts *counts,
                              ConsistencyIssueLevel level) {
  if (!counts) {
    return;
  }
  switch (level) {
  case ConsistencyIssueLevel::Error:
    ++counts->errors;
    break;
  case ConsistencyIssueLevel::Warning:
    ++counts->warnings;
    break;
  case ConsistencyIssueLevel::Ready:
    ++counts->ready;
    break;
  case ConsistencyIssueLevel::Info:
    ++counts->infos;
    break;
  case ConsistencyIssueLevel::Empty:
    break;
  }
}

bool consistencyHasBlockingIssues(const ConsistencyIssueCounts &counts) {
  return counts.errors > 0 || counts.warnings > 0;
}

ConsistencyGateState
consistencyGateState(bool available, bool fresh,
                     const ConsistencyIssueCounts &counts) {
  if (!available) {
    return ConsistencyGateState::NotRun;
  }
  if (!fresh) {
    return ConsistencyGateState::Stale;
  }
  if (consistencyHasBlockingIssues(counts)) {
    return ConsistencyGateState::Blocking;
  }
  return ConsistencyGateState::Passed;
}
