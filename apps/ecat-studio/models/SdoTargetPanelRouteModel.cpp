// SDO target panel source classification (dictionary, watch, startup, etc.).
#include "SdoTargetPanelRouteModel.h"

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
