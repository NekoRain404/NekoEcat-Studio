#pragma once

#include "ConsistencyDetailUiState.h"

#include <QString>

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

inline constexpr const char *kConsistencyIoScopeAll = "all";
inline constexpr const char *kConsistencyIoScopePdo = "pdo";
inline constexpr const char *kConsistencyIoScopeStartupDiff = "startupDiff";
inline constexpr const char *kConsistencyIoScopeMissingValue = "missingValue";
inline constexpr const char *kConsistencyIoScopePlcIssues = "plcIssues";

ConsistencyEvidenceAddress
parseConsistencyEvidenceAddress(const QString &target);
int parseConsistencyStartupRow(const QString &target);
QString consistencyEvidenceIoScope(const ConsistencyDetailRow &row);
ConsistencyEvidenceRouteDecision
consistencyEvidenceRouteDecision(const ConsistencyDetailRow &row);
