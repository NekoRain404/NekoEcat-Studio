// Topology change event classification: appeared, disappeared, reordered.
#include "TopologyChangeModel.h"

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
