// Consistency check models: gate state, issue classification, and evidence routing.
#include "ConsistencyModel.h"

#include "utils/TextHelpers.h"
#include <QRegularExpression>

// ── Gate State ──────────────────────────────────────────────────────

ConsistencyIssueLevel consistencyIssueLevelFromText(const QString& level) {
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

// Tallies one issue into the appropriate severity counter bucket.
void addConsistencyIssueLevel(ConsistencyIssueCounts* counts, ConsistencyIssueLevel level) {
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

// Errors or warnings block further commissioning progress through the gate.
bool consistencyHasBlockingIssues(const ConsistencyIssueCounts& counts) {
    return counts.errors > 0 || counts.warnings > 0;
}

// Derives overall gate status: not run, stale, blocking, or passed.
ConsistencyGateState consistencyGateState(bool available, bool fresh, const ConsistencyIssueCounts& counts) {
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


// ── Evidence Routing ────────────────────────────────────────────────


#include <QRegularExpression>

// Extracts slave position and optional OD address from issue target text like '#3 0x6040:0x00'.
ConsistencyEvidenceAddress parseConsistencyEvidenceAddress(const QString& target) {
    ConsistencyEvidenceAddress address;
    const QRegularExpression addressRe(R"(#\s*(\d+)(?:\s+0x([0-9a-fA-F]+)\s*:\s*0x([0-9a-fA-F]+))?)");
    const auto match = addressRe.match(target);
    if (!match.hasMatch()) {
        return address;
    }

    address.position = match.captured(1).toInt();
    if (!match.captured(2).isEmpty()) {
        address.index = normalizeHexText(QString("0x%1").arg(match.captured(2)), 4);
        address.subIndex = normalizeHexText(QString("0x%1").arg(match.captured(3)), 2);
    }
    return address;
}

// Extracts zero-based startup row index from localized issue text (e.g. 'startup row 5').
int parseConsistencyStartupRow(const QString& target) {
    const QRegularExpression startupRowRe(R"((?:startup\s+row|启动行)\s*(\d+))",
                                          QRegularExpression::CaseInsensitiveOption);
    const auto match = startupRowRe.match(target);
    return match.hasMatch() ? match.captured(1).toInt() - 1 : -1;
}

// Classifies an issue row into an I/O scope category for sidebar filtering.
QString consistencyEvidenceIoScope(const ConsistencyDetailRow& row) {
    const QString combined = QString("%1 %2 %3 %4").arg(row.target, row.evidence, row.actual, row.action).toLower();

    if (combined.contains("plc") || combined.contains("alias") || combined.contains("tag") ||
        combined.contains("交接") || combined.contains("别名") || combined.contains("标签") ||
        combined.contains("符号")) {
        return QString::fromLatin1(kConsistencyIoScopePlcIssues);
    }
    if (combined.contains("startup") || combined.contains("启动")) {
        return QString::fromLatin1(kConsistencyIoScopeStartupDiff);
    }
    if (combined.contains("raw") || combined.contains("watch") || combined.contains("value") ||
        combined.contains("值证据") || combined.contains("缺少 raw") || combined.contains("缺失")) {
        return QString::fromLatin1(kConsistencyIoScopeMissingValue);
    }
    if (combined.contains("map") || combined.contains("pdo") || combined.contains("映射")) {
        return QString::fromLatin1(kConsistencyIoScopePdo);
    }
    return QString::fromLatin1(kConsistencyIoScopeAll);
}

// Combines parsed address, startup row, and scope into a navigation decision.
ConsistencyEvidenceRouteDecision consistencyEvidenceRouteDecision(const ConsistencyDetailRow& row) {
    ConsistencyEvidenceRouteDecision decision;
    decision.address = parseConsistencyEvidenceAddress(row.target);
    decision.startupRow = parseConsistencyStartupRow(row.target);
    decision.ioScope = consistencyEvidenceIoScope(row);

    const QString scope = row.scope.toLower();
    const QString combined = QString("%1 %2 %3").arg(row.scope, row.evidence, row.action).toLower();
    if (scope.contains("topology") || row.scope.contains("拓扑")) {
        decision.kind = ConsistencyEvidenceRouteKind::Topology;
    } else if (scope.contains("startup") || row.scope.contains("启动") || combined.contains("startup") ||
               combined.contains("启动")) {
        decision.kind = ConsistencyEvidenceRouteKind::Startup;
    } else if (combined.contains("watch") || combined.contains("值证据") || combined.contains("缺少 raw") ||
               combined.contains("missing")) {
        decision.kind = ConsistencyEvidenceRouteKind::Watch;
    } else {
        decision.kind = ConsistencyEvidenceRouteKind::IoVariables;
    }
    return decision;
}
