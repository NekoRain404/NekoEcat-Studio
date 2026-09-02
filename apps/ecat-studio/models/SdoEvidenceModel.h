#pragma once

// SDO read evidence tracking: value, status, source, and comparison.


#include <QPair>
#include <QString>
#include <QStringList>
#include <QVector>

// Priority-ordered list of (source, value) pairs for evidence comparison
using SdoEvidenceCandidates = QVector<QPair<QString, QString>>;

// A single evidence entry with its source identifier
struct SdoEvidenceItem {
    QString source;
    QString value;
};

// Cluster of evidence items sharing the same normalized value
struct SdoEvidenceGroup {
    QStringList sources;
    QString value;
    QString normalized;
};

// Result of comparing a write value against all known evidence
struct SdoWriteDeltaReview {
    QString state;
    bool hasDiff = false;
    bool hasConflict = false;
    bool matchesEvidence = false;
    QStringList matchingSources;
    QStringList conflictFacts;
    QVector<SdoEvidenceGroup> differingGroups;
};

QString sdoEvidenceKey(int position, const QString& index, const QString& subIndex);
bool isSdoHistoryStartupSource(const QString& status, const QString& value);
QString preferredSdoEvidenceValue(const SdoEvidenceCandidates& candidates, QString* source = nullptr);
bool sdoValuesComparableEqual(const QString& left, const QString& right);
bool sdoEvidenceHasConflict(const SdoEvidenceCandidates& candidates);
QVector<SdoEvidenceGroup> groupSdoEvidence(const QVector<SdoEvidenceItem>& items);
SdoWriteDeltaReview reviewSdoWriteDelta(const QVector<SdoEvidenceItem>& items, const QString& writeValue);

// ─────────────────────────────────────────────────────────────────────────────
// SDO target panel route classification (dictionary, watch, startup, etc.)
// ─────────────────────────────────────────────────────────────────────────────

// Navigation target when user clicks a target panel row
enum class SdoTargetPanelRouteKind {
    Watch,
    Startup,
    Bookmark,
    TargetTrail,
    EvidenceReview,
    EvidenceDigest,
    ObjectDictionary,
    CopyDigest,
};

// Action to perform when copying target panel content
enum class SdoTargetPanelCopyActionKind {
    FullDigest,
    OpenWatch,
    OpenStartup,
    OpenBookmark,
    OpenTargetTrail,
    ReviewEvidence,
    CopyDigestNoDelta,
    FocusObjectDictionary,
};

// Combined route and copy action for a target panel row
struct SdoTargetPanelRouteDecision {
    SdoTargetPanelRouteKind routeKind = SdoTargetPanelRouteKind::CopyDigest;
    SdoTargetPanelCopyActionKind copyActionKind = SdoTargetPanelCopyActionKind::FullDigest;
};

SdoTargetPanelRouteDecision sdoTargetPanelRouteDecision(const QString& rowKey, bool writeDeltaReviewAvailable);
