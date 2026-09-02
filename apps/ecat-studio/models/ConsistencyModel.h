#pragma once

// Consistency check models: gate state, issue classification, and evidence routing.
//
// This file consolidates:
// - ConsistencyGateModel: issue severity, gate state, blocking rules
// - ConsistencyEvidenceRouteModel: navigation from consistency issues to source evidence

#include "detail/ConsistencyDetail.h"
#include "EthercatTypes.h"

#include <QString>
#include <QVector>

// ── Gate State ──────────────────────────────────────────────────────

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

ConsistencyIssueLevel consistencyIssueLevelFromText(const QString& level);
void addConsistencyIssueLevel(ConsistencyIssueCounts* counts, ConsistencyIssueLevel level);
bool consistencyHasBlockingIssues(const ConsistencyIssueCounts& counts);
ConsistencyGateState consistencyGateState(bool available, bool fresh, const ConsistencyIssueCounts& counts);

// ── Evidence Routing ────────────────────────────────────────────────

enum class ConsistencyEvidenceRouteKind {
    Topology,
    Startup,
    Watch,
    IoVariables,
};

struct ConsistencyEvidenceAddress {
    int position = -1;
    QString index;
    QString subIndex;
};

struct ConsistencyEvidenceRouteDecision {
    ConsistencyEvidenceRouteKind kind = ConsistencyEvidenceRouteKind::IoVariables;
    ConsistencyEvidenceAddress address;
    int startupRow = -1;
    QString ioScope;
};

inline constexpr const char* kConsistencyIoScopeAll = "all";
inline constexpr const char* kConsistencyIoScopePdo = "pdo";
inline constexpr const char* kConsistencyIoScopeStartupDiff = "startupDiff";
inline constexpr const char* kConsistencyIoScopeMissingValue = "missingValue";
inline constexpr const char* kConsistencyIoScopePlcIssues = "plcIssues";

ConsistencyEvidenceAddress parseConsistencyEvidenceAddress(const QString& target);
int parseConsistencyStartupRow(const QString& target);
QString consistencyEvidenceIoScope(const ConsistencyDetailRow& row);
ConsistencyEvidenceRouteDecision consistencyEvidenceRouteDecision(const ConsistencyDetailRow& row);
