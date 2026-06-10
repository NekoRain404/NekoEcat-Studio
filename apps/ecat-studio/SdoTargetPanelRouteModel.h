#pragma once

#include <QString>

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

struct SdoTargetPanelRouteDecision {
  SdoTargetPanelRouteKind routeKind = SdoTargetPanelRouteKind::CopyDigest;
  SdoTargetPanelCopyActionKind copyActionKind =
      SdoTargetPanelCopyActionKind::FullDigest;
};

SdoTargetPanelRouteDecision
sdoTargetPanelRouteDecision(const QString &rowKey,
                            bool writeDeltaReviewAvailable);
