#pragma once

// Topology comparison models: baseline drift detection and change classification.
//
// This file consolidates:
// - TopologyBaselineModel: compare live topology against saved baseline
// - TopologyChangeModel: classify topology changes (added, removed, renamed)

#include "EthercatTypes.h"

#include <QString>
#include <QVector>

// ── Baseline Comparison ─────────────────────────────────────────────

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

// ── Change Detection ────────────────────────────────────────────────

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
