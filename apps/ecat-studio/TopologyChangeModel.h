#pragma once

#include "EthercatTypes.h"

#include <QVector>

enum class TopologyChangeKind {
  Added,
  Removed,
  NameChanged,
  StateChanged,
  FlagsChanged,
};

struct TopologyChange {
  TopologyChangeKind kind = TopologyChangeKind::Added;
  int position = -1;
  SlaveInfo previous;
  SlaveInfo current;
};

QVector<TopologyChange>
detectTopologyChanges(const QVector<SlaveInfo> &previous,
                      const QVector<SlaveInfo> &current);
