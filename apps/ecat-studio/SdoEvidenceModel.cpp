#include "SdoEvidenceModel.h"

#include "StudioTextHelpers.h"

QString sdoEvidenceKey(int position, const QString &index,
                       const QString &subIndex) {
  return QString("%1|%2|%3")
      .arg(position)
      .arg(normalizeHexText(index, 4), normalizeHexText(subIndex, 2));
}

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

bool sdoValuesComparableEqual(const QString &left, const QString &right) {
  const QString normalizedLeft = normalizeComparableValue(left);
  const QString normalizedRight = normalizeComparableValue(right);
  return !normalizedLeft.isEmpty() && normalizedLeft == normalizedRight;
}

bool sdoEvidenceHasConflict(const SdoEvidenceCandidates &candidates) {
  QString firstNormalized;
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

QVector<SdoEvidenceGroup>
groupSdoEvidence(const QVector<SdoEvidenceItem> &items) {
  QVector<SdoEvidenceGroup> groups;
  for (const auto &item : items) {
    const QString value = item.value.trimmed();
    const QString normalized = normalizeComparableValue(value);
    if (item.source.trimmed().isEmpty() || value.isEmpty() ||
        normalized.isEmpty()) {
      continue;
    }

    int groupIndex = -1;
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

SdoWriteDeltaReview reviewSdoWriteDelta(const QVector<SdoEvidenceItem> &items,
                                        const QString &writeValue) {
  SdoWriteDeltaReview review;
  review.state = QStringLiteral("none");

  const auto groups = groupSdoEvidence(items);
  if (groups.isEmpty()) {
    return review;
  }

  const QString normalizedWrite = normalizeComparableValue(writeValue);
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
