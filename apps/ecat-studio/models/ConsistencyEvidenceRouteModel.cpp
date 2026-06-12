#include "ConsistencyEvidenceRouteModel.h"

#include "helpers/StudioTextHelpers.h"

#include <QRegularExpression>

ConsistencyEvidenceAddress
parseConsistencyEvidenceAddress(const QString &target) {
  ConsistencyEvidenceAddress address;
  const QRegularExpression addressRe(
      R"(#\s*(\d+)(?:\s+0x([0-9a-fA-F]+)\s*:\s*0x([0-9a-fA-F]+))?)");
  const auto match = addressRe.match(target);
  if (!match.hasMatch()) {
    return address;
  }

  address.position = match.captured(1).toInt();
  if (!match.captured(2).isEmpty()) {
    address.index = normalizeHexText(QString("0x%1").arg(match.captured(2)), 4);
    address.subIndex =
        normalizeHexText(QString("0x%1").arg(match.captured(3)), 2);
  }
  return address;
}

int parseConsistencyStartupRow(const QString &target) {
  const QRegularExpression startupRowRe(
      R"((?:startup\s+row|启动行)\s*(\d+))",
      QRegularExpression::CaseInsensitiveOption);
  const auto match = startupRowRe.match(target);
  return match.hasMatch() ? match.captured(1).toInt() - 1 : -1;
}

QString consistencyEvidenceIoScope(const ConsistencyDetailRow &row) {
  const QString combined =
      QString("%1 %2 %3 %4")
          .arg(row.target, row.evidence, row.actual, row.action)
          .toLower();

  if (combined.contains("plc") || combined.contains("alias") ||
      combined.contains("tag") || combined.contains("交接") ||
      combined.contains("别名") || combined.contains("标签") ||
      combined.contains("符号")) {
    return QString::fromLatin1(kConsistencyIoScopePlcIssues);
  }
  if (combined.contains("startup") || combined.contains("启动")) {
    return QString::fromLatin1(kConsistencyIoScopeStartupDiff);
  }
  if (combined.contains("raw") || combined.contains("watch") ||
      combined.contains("value") || combined.contains("值证据") ||
      combined.contains("缺少 raw") || combined.contains("缺失")) {
    return QString::fromLatin1(kConsistencyIoScopeMissingValue);
  }
  if (combined.contains("map") || combined.contains("pdo") ||
      combined.contains("映射")) {
    return QString::fromLatin1(kConsistencyIoScopePdo);
  }
  return QString::fromLatin1(kConsistencyIoScopeAll);
}

ConsistencyEvidenceRouteDecision
consistencyEvidenceRouteDecision(const ConsistencyDetailRow &row) {
  ConsistencyEvidenceRouteDecision decision;
  decision.address = parseConsistencyEvidenceAddress(row.target);
  decision.startupRow = parseConsistencyStartupRow(row.target);
  decision.ioScope = consistencyEvidenceIoScope(row);

  const QString scope = row.scope.toLower();
  const QString combined =
      QString("%1 %2 %3").arg(row.scope, row.evidence, row.action).toLower();
  if (scope.contains("topology") || row.scope.contains("拓扑")) {
    decision.kind = ConsistencyEvidenceRouteKind::Topology;
  } else if (scope.contains("startup") || row.scope.contains("启动") ||
             combined.contains("startup") || combined.contains("启动")) {
    decision.kind = ConsistencyEvidenceRouteKind::Startup;
  } else if (combined.contains("watch") || combined.contains("值证据") ||
             combined.contains("缺少 raw") || combined.contains("missing")) {
    decision.kind = ConsistencyEvidenceRouteKind::Watch;
  } else {
    decision.kind = ConsistencyEvidenceRouteKind::IoVariables;
  }
  return decision;
}
