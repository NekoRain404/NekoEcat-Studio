// Unit tests for SdoTargetPanelRouteModel.
#include "models/SdoEvidenceModel.h"

#include <cstdlib>
#include <iostream>

namespace {

void fail(const char *message) {
  std::cerr << message << '\n';
  std::exit(1);
}

void expectRoute(SdoTargetPanelRouteKind actual,
                 SdoTargetPanelRouteKind expected, const char *message) {
  if (actual != expected) {
    fail(message);
  }
}

void expectCopyAction(SdoTargetPanelCopyActionKind actual,
                      SdoTargetPanelCopyActionKind expected,
                      const char *message) {
  if (actual != expected) {
    fail(message);
  }
}

void testEvidenceRows() {
  auto decision = sdoTargetPanelRouteDecision("Evidence Set", true);
  expectRoute(decision.routeKind, SdoTargetPanelRouteKind::EvidenceReview,
              "Evidence Set routes to review when available");
  expectCopyAction(decision.copyActionKind,
                   SdoTargetPanelCopyActionKind::ReviewEvidence,
                   "Evidence Set copy action reviews when available");

  decision = sdoTargetPanelRouteDecision("写入差异", false);
  expectRoute(decision.routeKind, SdoTargetPanelRouteKind::EvidenceDigest,
              "Write Delta routes to digest when review is unavailable");
  expectCopyAction(decision.copyActionKind,
                   SdoTargetPanelCopyActionKind::CopyDigestNoDelta,
                   "Write Delta copy action explains no delta");
}

void testEvidenceLinksAndDictionaryRows() {
  expectRoute(sdoTargetPanelRouteDecision("Watch 关联", false).routeKind,
              SdoTargetPanelRouteKind::Watch, "Watch link route");
  expectRoute(sdoTargetPanelRouteDecision("Startup Link", false).routeKind,
              SdoTargetPanelRouteKind::Startup, "Startup link route");
  expectRoute(sdoTargetPanelRouteDecision("书签", false).routeKind,
              SdoTargetPanelRouteKind::Bookmark, "Bookmark route");
  expectRoute(sdoTargetPanelRouteDecision("目标轨迹", false).routeKind,
              SdoTargetPanelRouteKind::TargetTrail, "Target Trail route");
  expectRoute(sdoTargetPanelRouteDecision("OD Evidence", false).routeKind,
              SdoTargetPanelRouteKind::ObjectDictionary, "OD Evidence route");
  expectCopyAction(
      sdoTargetPanelRouteDecision("Read Value", false).copyActionKind,
      SdoTargetPanelCopyActionKind::FocusObjectDictionary,
      "Read Value copy action focuses dictionary");
}

void testFallbackRows() {
  const auto decision = sdoTargetPanelRouteDecision("Safety", true);
  expectRoute(decision.routeKind, SdoTargetPanelRouteKind::CopyDigest,
              "Safety route falls back to digest");
  expectCopyAction(decision.copyActionKind,
                   SdoTargetPanelCopyActionKind::FullDigest,
                   "Safety copy action uses full digest");
}

} // namespace

int main() {
  testEvidenceRows();
  testEvidenceLinksAndDictionaryRows();
  testFallbackRows();
  return 0;
}
