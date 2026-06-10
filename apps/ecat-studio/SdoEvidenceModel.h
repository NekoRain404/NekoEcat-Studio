#pragma once

#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>

using SdoEvidenceCandidates = QVector<QPair<QString, QString>>;

struct SdoEvidenceItem {
  QString source;
  QString value;
};

struct SdoEvidenceGroup {
  QStringList sources;
  QString value;
  QString normalized;
};

struct SdoWriteDeltaReview {
  QString state;
  bool hasDiff = false;
  bool hasConflict = false;
  bool matchesEvidence = false;
  QStringList matchingSources;
  QStringList conflictFacts;
  QVector<SdoEvidenceGroup> differingGroups;
};

QString sdoEvidenceKey(int position, const QString &index,
                       const QString &subIndex);
bool isSdoHistoryStartupSource(const QString &status, const QString &value);
QString preferredSdoEvidenceValue(const SdoEvidenceCandidates &candidates,
                                  QString *source = nullptr);
bool sdoValuesComparableEqual(const QString &left, const QString &right);
bool sdoEvidenceHasConflict(const SdoEvidenceCandidates &candidates);
QVector<SdoEvidenceGroup>
groupSdoEvidence(const QVector<SdoEvidenceItem> &items);
SdoWriteDeltaReview reviewSdoWriteDelta(const QVector<SdoEvidenceItem> &items,
                                        const QString &writeValue);
