// Topology baseline capture and drift comparison against live slave list.
#include "TopologyBaselineModel.h"

#include <QHash>

// Falls back to raw scan line when the slave has no parsed name.
QString topologySlaveDisplayName(const SlaveInfo &slave) {
  return slave.name.isEmpty() ? slave.rawLine : slave.name;
}

// Diffs baseline against live topology: detects missing, unexpected, renamed, and state-changed slaves.
QVector<TopologyBaselineIssue>
compareTopologyBaseline(const QVector<SlaveInfo> &baseline,
                        const QVector<SlaveInfo> &current) {
  QVector<TopologyBaselineIssue> issues;
  if (baseline.isEmpty()) {
    return issues;
  }

  QHash<int, SlaveInfo> expected;
  QHash<int, SlaveInfo> actual;
  for (const auto &slave : baseline) {
    expected.insert(slave.position, slave);
  }
  for (const auto &slave : current) {
    actual.insert(slave.position, slave);
  }

  for (const auto &expectedSlave : baseline) {
    if (!actual.contains(expectedSlave.position)) {
      TopologyBaselineIssue issue;
      issue.kind = TopologyBaselineIssueKind::MissingSlave;
      issue.position = expectedSlave.position;
      issue.baseline = expectedSlave;
      issues.append(issue);
      continue;
    }

    const SlaveInfo currentSlave = actual.value(expectedSlave.position);
    if (currentSlave.name != expectedSlave.name) {
      TopologyBaselineIssue issue;
      issue.kind = TopologyBaselineIssueKind::NameChanged;
      issue.position = expectedSlave.position;
      issue.baseline = expectedSlave;
      issue.current = currentSlave;
      issues.append(issue);
    }
    if (!expectedSlave.state.isEmpty() && !currentSlave.state.isEmpty() &&
        currentSlave.state != expectedSlave.state) {
      TopologyBaselineIssue issue;
      issue.kind = TopologyBaselineIssueKind::StateChanged;
      issue.position = expectedSlave.position;
      issue.baseline = expectedSlave;
      issue.current = currentSlave;
      issues.append(issue);
    }
  }

  for (const auto &currentSlave : current) {
    if (!expected.contains(currentSlave.position)) {
      TopologyBaselineIssue issue;
      issue.kind = TopologyBaselineIssueKind::UnexpectedSlave;
      issue.position = currentSlave.position;
      issue.current = currentSlave;
      issues.append(issue);
    }
  }

  return issues;
}
