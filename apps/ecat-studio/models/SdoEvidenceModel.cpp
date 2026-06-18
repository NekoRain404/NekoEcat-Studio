// SDO read evidence tracking: value, status, source, and comparison.
#include "SdoEvidenceModel.h"

#include "utils/TextHelpers.h"

// Builds a normalized composite key for SDO evidence lookup and deduplication.
QString sdoEvidenceKey(int position, const QString &index,
                       const QString &subIndex) {
  return QString("%1|%2|%3")
      .arg(position)
      .arg(normalizeHexText(index, 4), normalizeHexText(subIndex, 2));
}

// Filters out failed/pending reads when checking if startup evidence is usable.
bool isSdoHistoryStartupSource(const QString &status, const QString &value) {
  if (value.trimmed().isEmpty()) {
    return false;
  }
  const QString normalizedStatus = status.trimmed().toLower();
  if (normalizedStatus == "failed" || normalizedStatus == "requested" ||
      normalizedStatus == "失败" || normalizedStatus == "已请求") {
    return false;
  }
  return true;
}

// Returns the first (highest-priority) evidence candidate's value and its source.
QString preferredSdoEvidenceValue(const SdoEvidenceCandidates &candidates,
                                  QString *source) {
  if (source) {
    source->clear();
  }
  if (candidates.isEmpty()) {
    return QString();
  }
  if (source) {
    *source = candidates.first().first;
  }
  return candidates.first().second;
}

// Normalized comparison that ignores whitespace and formatting differences.
bool sdoValuesComparableEqual(const QString &left, const QString &right) {
  const QString normalizedLeft = normalizeComparableValue(left);
  const QString normalizedRight = normalizeComparableValue(right);
  return !normalizedLeft.isEmpty() && normalizedLeft == normalizedRight;
}

// Detects when multiple evidence sources report different values for the same object.
bool sdoEvidenceHasConflict(const SdoEvidenceCandidates &candidates) {
  QString firstNormalized;
    // Iterate over collection
  for (const auto &candidate : candidates) {
    const QString normalized = normalizeComparableValue(candidate.second);
    if (normalized.isEmpty()) {
      continue;
    }
    if (firstNormalized.isEmpty()) {
      firstNormalized = normalized;
      continue;
    }
    if (normalized != firstNormalized) {
      return true;
    }
  }
  return false;
}

// Clusters evidence items by normalized value for side-by-side comparison display.
QVector<SdoEvidenceGroup>
groupSdoEvidence(const QVector<SdoEvidenceItem> &items) {
  QVector<SdoEvidenceGroup> groups;
    // Iterate over collection
  for (const auto &item : items) {
    const QString value = item.value.trimmed();
    const QString normalized = normalizeComparableValue(value);
    if (item.source.trimmed().isEmpty() || value.isEmpty() ||
        normalized.isEmpty()) {
      continue;
    }

    int groupIndex = -1;
    // Iterate over collection
    for (int i = 0; i < groups.size(); ++i) {
      if (groups.at(i).normalized == normalized) {
        groupIndex = i;
        break;
      }
    }

    if (groupIndex < 0) {
      groups.append({QStringList{item.source}, value, normalized});
    } else {
      groups[groupIndex].sources << item.source;
    }
  }
  return groups;
}

// Compares a proposed write value against all known evidence to surface diffs and conflicts.
SdoWriteDeltaReview reviewSdoWriteDelta(const QVector<SdoEvidenceItem> &items,
                                        const QString &writeValue) {
  SdoWriteDeltaReview review;
  review.state = QStringLiteral("none");

  const auto groups = groupSdoEvidence(items);
  if (groups.isEmpty()) {
    return review;
  }

  const QString normalizedWrite = normalizeComparableValue(writeValue);
    // Iterate over collection
  for (const auto &group : groups) {
    const QString sources = group.sources.join("/");
    review.conflictFacts << QString("%1=%2").arg(sources, group.value);
    if (group.normalized == normalizedWrite) {
      review.matchingSources << sources;
    } else {
      review.differingGroups.append(group);
    }
  }

  review.hasDiff = !review.differingGroups.isEmpty();
  review.hasConflict = groups.size() > 1;
  review.matchesEvidence = !review.hasDiff;
  review.state =
      review.hasConflict
          ? QStringLiteral("conflict")
          : (review.hasDiff ? QStringLiteral("diff") : QStringLiteral("match"));
  return review;
}

// ─────────────────────────────────────────────────────────────────────────────
// SDO target panel route classification
// ─────────────────────────────────────────────────────────────────────────────

namespace {

// Bilingual key comparison: matches against both English and Chinese labels.
bool keyMatches(const QString &key, const QString &english, const QString &zh) {
  return key == english || key == zh;
}

} // namespace

// Maps a target panel row key to the appropriate navigation route and copy action.
SdoTargetPanelRouteDecision
sdoTargetPanelRouteDecision(const QString &rowKey,
                            bool writeDeltaReviewAvailable) {
  SdoTargetPanelRouteDecision decision;

  if (keyMatches(rowKey, "Watch Link", "Watch 关联")) {
    decision.routeKind = SdoTargetPanelRouteKind::Watch;
    decision.copyActionKind = SdoTargetPanelCopyActionKind::OpenWatch;
  } else if (keyMatches(rowKey, "Startup Link", "Startup 关联")) {
    decision.routeKind = SdoTargetPanelRouteKind::Startup;
    decision.copyActionKind = SdoTargetPanelCopyActionKind::OpenStartup;
  } else if (keyMatches(rowKey, "Bookmark", "书签")) {
    decision.routeKind = SdoTargetPanelRouteKind::Bookmark;
    decision.copyActionKind = SdoTargetPanelCopyActionKind::OpenBookmark;
  } else if (keyMatches(rowKey, "Target Trail", "目标轨迹")) {
    decision.routeKind = SdoTargetPanelRouteKind::TargetTrail;
    decision.copyActionKind = SdoTargetPanelCopyActionKind::OpenTargetTrail;
  } else if (keyMatches(rowKey, "Evidence Set", "证据集") ||
             keyMatches(rowKey, "Write Delta", "写入差异")) {
    decision.routeKind = writeDeltaReviewAvailable
                             ? SdoTargetPanelRouteKind::EvidenceReview
                             : SdoTargetPanelRouteKind::EvidenceDigest;
    decision.copyActionKind =
        writeDeltaReviewAvailable
            ? SdoTargetPanelCopyActionKind::ReviewEvidence
            : SdoTargetPanelCopyActionKind::CopyDigestNoDelta;
  } else if (keyMatches(rowKey, "Target", "目标") ||
             keyMatches(rowKey, "Read Value", "读回值") ||
             keyMatches(rowKey, "OD Evidence", "OD 证据")) {
    decision.routeKind = SdoTargetPanelRouteKind::ObjectDictionary;
    decision.copyActionKind =
        SdoTargetPanelCopyActionKind::FocusObjectDictionary;
  } else {
    decision.routeKind = SdoTargetPanelRouteKind::CopyDigest;
    decision.copyActionKind = SdoTargetPanelCopyActionKind::FullDigest;
  }

  return decision;
}
