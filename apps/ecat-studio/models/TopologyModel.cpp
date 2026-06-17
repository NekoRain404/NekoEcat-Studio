// Topology comparison models: baseline drift detection and change classification.
#include "TopologyModel.h"

#include <QRegularExpression>

// ── Baseline Comparison ─────────────────────────────────────────────

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
    // Iterate over collection
  for (const auto &slave : baseline) {
    expected.insert(slave.position, slave);
  }
    // Iterate over collection
  for (const auto &slave : current) {
    actual.insert(slave.position, slave);
  }

    // Iterate over collection
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

    // Iterate over collection
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


// ── Change Detection ────────────────────────────────────────────────

#include <QHash>

// Detects added/removed/name/state/flags changes between two topology snapshots.
QVector<TopologyChange>
detectTopologyChanges(const QVector<SlaveInfo> &previous,
                      const QVector<SlaveInfo> &current) {
  QVector<TopologyChange> changes;
  if (previous.isEmpty()) {
    return changes;
  }

  QHash<int, SlaveInfo> before;
  QHash<int, SlaveInfo> after;
    // Iterate over collection
  for (const auto &slave : previous) {
    before.insert(slave.position, slave);
  }
    // Iterate over collection
  for (const auto &slave : current) {
    after.insert(slave.position, slave);
  }

    // Iterate over collection
  for (const auto &currentSlave : current) {
    if (!before.contains(currentSlave.position)) {
      TopologyChange change;
      change.kind = TopologyChangeKind::Added;
      change.position = currentSlave.position;
      change.current = currentSlave;
      changes.append(change);
      continue;
    }

    const SlaveInfo previousSlave = before.value(currentSlave.position);
    if (previousSlave.name != currentSlave.name) {
      TopologyChange change;
      change.kind = TopologyChangeKind::NameChanged;
      change.position = currentSlave.position;
      change.previous = previousSlave;
      change.current = currentSlave;
      changes.append(change);
    }
    if (previousSlave.state != currentSlave.state) {
      TopologyChange change;
      change.kind = TopologyChangeKind::StateChanged;
      change.position = currentSlave.position;
      change.previous = previousSlave;
      change.current = currentSlave;
      changes.append(change);
    }
    if (previousSlave.flags != currentSlave.flags) {
      TopologyChange change;
      change.kind = TopologyChangeKind::FlagsChanged;
      change.position = currentSlave.position;
      change.previous = previousSlave;
      change.current = currentSlave;
      changes.append(change);
    }
  }

    // Iterate over collection
  for (const auto &previousSlave : previous) {
    if (!after.contains(previousSlave.position)) {
      TopologyChange change;
      change.kind = TopologyChangeKind::Removed;
      change.position = previousSlave.position;
      change.previous = previousSlave;
      changes.append(change);
    }
  }

  return changes;
}
