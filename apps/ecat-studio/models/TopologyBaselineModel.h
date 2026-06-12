#pragma once

// Topology baseline capture and drift comparison against live slave list.


#include "EthercatTypes.h"

#include <QString>
#include <QVector>

enum class TopologyBaselineIssueKind {
  MissingSlave,
  UnexpectedSlave,
  NameChanged,
  StateChanged,
};

struct TopologyBaselineIssue {
  TopologyBaselineIssueKind kind = TopologyBaselineIssueKind::MissingSlave;
  int position = -1;
  SlaveInfo baseline;
  SlaveInfo current;
};

QVector<TopologyBaselineIssue>
compareTopologyBaseline(const QVector<SlaveInfo> &baseline,
                        const QVector<SlaveInfo> &current);

QString topologySlaveDisplayName(const SlaveInfo &slave);
