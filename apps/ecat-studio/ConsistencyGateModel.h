#pragma once

#include <QString>

enum class ConsistencyIssueLevel {
  Error,
  Warning,
  Ready,
  Info,
  Empty,
};

enum class ConsistencyGateState {
  NotRun,
  Stale,
  Blocking,
  Passed,
};

struct ConsistencyIssueCounts {
  int errors = 0;
  int warnings = 0;
  int infos = 0;
  int ready = 0;
};

ConsistencyIssueLevel consistencyIssueLevelFromText(const QString &level);
void addConsistencyIssueLevel(ConsistencyIssueCounts *counts,
                              ConsistencyIssueLevel level);
bool consistencyHasBlockingIssues(const ConsistencyIssueCounts &counts);
ConsistencyGateState consistencyGateState(bool available, bool fresh,
                                          const ConsistencyIssueCounts &counts);
