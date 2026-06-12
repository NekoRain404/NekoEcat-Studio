// Populates and queries the commissioning workflow QTableWidget.
#include "CommissioningWorkflowTableAdapter.h"

#include "helpers/StudioTableHelpers.h"

#include <QTableWidget>

namespace {

constexpr int kCommissioningWorkflowStatusRole = Qt::UserRole + 32;

// Normalizes text and checks if it represents an absent or "none" value.
bool isNoneText(const QString &text, const QString &noneText) {
  const QString normalized = text.trimmed().toLower();
  return normalized.isEmpty() || normalized == QStringLiteral("none") ||
         normalized == noneText.trimmed().toLower();
}

// Detects localized risk/evidence keywords that indicate missing commissioning data.
bool hasEvidenceGap(const QString &riskText, const QString &evidence) {
  const QString riskLower = riskText.toLower();
  const QString evidenceLower = evidence.toLower();
  return riskLower.contains(QStringLiteral("missing")) ||
         riskLower.contains(QStringLiteral("缺失")) ||
         riskLower.contains(QStringLiteral("offline")) ||
         riskLower.contains(QStringLiteral("离线")) ||
         evidenceLower.contains(QStringLiteral("not loaded")) ||
         evidenceLower.contains(QStringLiteral("not run")) ||
         evidenceLower.contains(QStringLiteral("missing")) ||
         evidenceLower.contains(QStringLiteral("no active")) ||
         evidenceLower.contains(QStringLiteral("stopped")) ||
         evidenceLower.contains(QStringLiteral("尚未")) ||
         evidenceLower.contains(QStringLiteral("缺")) ||
         evidenceLower.contains(QStringLiteral("未运行")) ||
         evidenceLower.contains(QStringLiteral("已停止"));
}

} // namespace

// Stores the status key as custom role data on every cell in the row for later retrieval.
void setCommissioningWorkflowStatusKey(QTableWidget *table, int row,
                                       const QString &statusKey) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return;
  }
  for (int column = 0; column < table->columnCount(); ++column) {
    if (auto *item = table->item(row, column)) {
      item->setData(kCommissioningWorkflowStatusRole, statusKey);
    }
  }
}

// Retrieves the stored status key used by scope filters and routing logic.
QString commissioningWorkflowStatusKeyForRow(QTableWidget *table, int row) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return QString();
  }
  if (auto *item = table->item(row, kCommissioningWorkflowStatusColumn)) {
    return item->data(kCommissioningWorkflowStatusRole).toString();
  }
  return QString();
}

// Extracts all visible cell values into a structured row for model binding.
CommissioningWorkflowTableRow
commissioningWorkflowTableRowFromTable(QTableWidget *table, int row) {
  CommissioningWorkflowTableRow result;
  result.row = row;
  if (!table || row < 0 || row >= table->rowCount()) {
    return result;
  }

  result.phase = tableText(table, row, kCommissioningWorkflowPhaseColumn);
  result.status = tableText(table, row, kCommissioningWorkflowStatusColumn);
  result.statusKey = commissioningWorkflowStatusKeyForRow(table, row);
  result.step = tableText(table, row, kCommissioningWorkflowStepColumn);
  result.risk = tableText(table, row, kCommissioningWorkflowRiskColumn);
  result.evidence = tableText(table, row, kCommissioningWorkflowEvidenceColumn);
  result.nextAction = tableText(table, row, kCommissioningWorkflowActionColumn);
  return result;
}

// Derives boolean state flags that drive filtering, badges, and issue navigation.
CommissioningWorkflowRowState
commissioningWorkflowRowState(QTableWidget *table, int row,
                              const QString &noneText) {
  CommissioningWorkflowRowState state;
  if (!table || row < 0 || row >= table->rowCount()) {
    return state;
  }

  const QString statusKey = commissioningWorkflowStatusKeyForRow(table, row);
  const QString riskText =
      tableText(table, row, kCommissioningWorkflowRiskColumn).trimmed();
  const QString evidence =
      tableText(table, row, kCommissioningWorkflowEvidenceColumn).trimmed();

  state.isReady = statusKey == QStringLiteral("ready");
  state.isAction = statusKey == QStringLiteral("action");
  state.isBlocked = statusKey == QStringLiteral("blocked");
  state.hasRisk = !isNoneText(riskText, noneText);
  state.hasGap = hasEvidenceGap(riskText, evidence);
  state.reviewIssue = !state.isReady;
  return state;
}

// Tests whether a row's state passes the active filter scope (open, blocked, action, etc.).
bool commissioningWorkflowScopeMatches(
    const CommissioningWorkflowRowState &state, const QString &scope) {
  if (scope == QString::fromLatin1(kCommissioningWorkflowScopeOpen)) {
    return !state.isReady;
  }
  if (scope == QString::fromLatin1(kCommissioningWorkflowScopeBlocked)) {
    return state.isBlocked;
  }
  if (scope == QString::fromLatin1(kCommissioningWorkflowScopeAction)) {
    return state.isAction;
  }
  if (scope == QString::fromLatin1(kCommissioningWorkflowScopeReady)) {
    return state.isReady;
  }
  if (scope == QString::fromLatin1(kCommissioningWorkflowScopeRisk)) {
    return state.hasRisk;
  }
  if (scope == QString::fromLatin1(kCommissioningWorkflowScopeGap)) {
    return state.hasGap;
  }
  return true;
}

// Case-insensitive full-row search for user-typed filter text.
bool commissioningWorkflowSearchMatches(QTableWidget *table, int row,
                                        const QString &needle) {
  if (needle.trimmed().isEmpty()) {
    return true;
  }
  if (!table || row < 0 || row >= table->rowCount()) {
    return false;
  }
  for (int column = 0; column < table->columnCount(); ++column) {
    if (tableText(table, row, column).contains(needle, Qt::CaseInsensitive)) {
      return true;
    }
  }
  return false;
}

// Applies scope + text filters to all rows, toggles visibility, and collects aggregate stats.
CommissioningWorkflowFilterStats
filterCommissioningWorkflowTable(QTableWidget *table, const QString &scope,
                                 const QString &needle,
                                 const QString &noneText) {
  CommissioningWorkflowFilterStats stats;
  if (!table) {
    return stats;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    const CommissioningWorkflowRowState state =
        commissioningWorkflowRowState(table, row, noneText);
    if (state.isReady) {
      ++stats.ready;
    } else {
      ++stats.open;
    }
    if (state.isAction) {
      ++stats.action;
    }
    if (state.isBlocked) {
      ++stats.blocked;
    }
    if (state.hasRisk) {
      ++stats.risk;
    }
    if (state.hasGap) {
      ++stats.gaps;
    }

    const bool show = commissioningWorkflowScopeMatches(state, scope) &&
                      commissioningWorkflowSearchMatches(table, row, needle);
    table->setRowHidden(row, !show);
    if (!show) {
      continue;
    }
    ++stats.visible;
    if (stats.firstVisible < 0) {
      stats.firstVisible = row;
    }
    if (state.reviewIssue) {
      stats.hasVisibleIssue = true;
    }
  }
  return stats;
}

// Finds the first visible row that still needs review, for initial auto-scroll.
int firstCommissioningWorkflowIssueRow(QTableWidget *table) {
  if (!table) {
    return -1;
  }
  for (int row = 0; row < table->rowCount(); ++row) {
    if (table->isRowHidden(row) ||
        !commissioningWorkflowRowState(table, row, QStringLiteral("None"))
             .reviewIssue) {
      continue;
    }
    return row;
  }
  return -1;
}

// Wraps around from the current row to locate the next review issue for cycling navigation.
int nextCommissioningWorkflowIssueRow(QTableWidget *table, int currentRow) {
  if (!table || table->rowCount() <= 0) {
    return -1;
  }
  const int start = qMax(0, currentRow);
  for (int offset = 1; offset <= table->rowCount(); ++offset) {
    const int row = (start + offset) % table->rowCount();
    if (table->isRowHidden(row) ||
        !commissioningWorkflowRowState(table, row, QStringLiteral("None"))
             .reviewIssue) {
      continue;
    }
    return row;
  }
  return -1;
}
