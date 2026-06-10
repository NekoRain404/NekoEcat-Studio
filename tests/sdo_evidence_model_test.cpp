#include "SdoEvidenceModel.h"

#include <QCoreApplication>
#include <QStringList>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected '%2', got '%3'").arg(message, expected, actual));
  }
}

void testSdoEvidenceKey() {
  expectEqual(sdoEvidenceKey(2, "6040", "0"), "2|0x6040|0x00",
              "normalizes bare hexadecimal SDO addresses");
  expectEqual(sdoEvidenceKey(7, "0X6060", "01"), "7|0x6060|0x01",
              "normalizes prefixed hexadecimal SDO addresses");
  expectEqual(sdoEvidenceKey(3, "device-mode", "x"), "3|device-mode|x",
              "keeps non-hex address text comparable");
}

void testHistoryStartupSource() {
  expectTrue(isSdoHistoryStartupSource("Complete", "42"),
             "completed history values can seed Startup SDO rows");
  expectTrue(isSdoHistoryStartupSource("成功", "0x0006"),
             "localized successful history values can seed Startup SDO rows");
  expectTrue(!isSdoHistoryStartupSource("Failed", "42"),
             "failed history values are not Startup SDO sources");
  expectTrue(!isSdoHistoryStartupSource("已请求", "42"),
             "requested history values are not Startup SDO sources");
  expectTrue(!isSdoHistoryStartupSource("Complete", "  "),
             "empty history values are not Startup SDO sources");
}

void testPreferredEvidenceValue() {
  SdoEvidenceCandidates candidates = {{"Read", "0x0006"}, {"Watch", "0x0007"}};
  QString source;
  expectEqual(preferredSdoEvidenceValue(candidates, &source), "0x0006",
              "preferred evidence uses first non-empty candidate");
  expectEqual(source, "Read", "preferred evidence reports first source");

  source = "unchanged";
  expectEqual(preferredSdoEvidenceValue({}, &source), QString(),
              "empty evidence has no preferred value");
  expectEqual(source, QString(), "empty evidence clears source");
}

void testComparableValues() {
  expectTrue(sdoValuesComparableEqual("0x 00 06", " 0x0006 "),
             "comparable values ignore whitespace and case");
  expectTrue(sdoValuesComparableEqual("TRUE", "true"),
             "comparable values ignore case");
  expectTrue(!sdoValuesComparableEqual("0x0006", "0x0007"),
             "different comparable values are not equal");
  expectTrue(!sdoValuesComparableEqual("", " "),
             "empty comparable values are not treated as equal evidence");
}

void testEvidenceConflict() {
  expectTrue(
      !sdoEvidenceHasConflict(
          {{"Read", "0x0006"}, {"Watch", "0x0006"}, {"Startup", "0x 00 06"}}),
      "matching comparable values are not a conflict");
  expectTrue(sdoEvidenceHasConflict({{"Read", "0x0006"}, {"Watch", "0x0007"}}),
             "different comparable values are a conflict");
  expectTrue(!sdoEvidenceHasConflict({{"Read", " "}, {"Watch", ""}}),
             "empty evidence values do not create false conflicts");
}

void testEvidenceGrouping() {
  const auto groups = groupSdoEvidence({{"Read", " 0x0006 "},
                                        {"Watch", "0x0006"},
                                        {"Startup", "0x0007"},
                                        {"Empty", "  "}});
  expectTrue(groups.size() == 2, "groups comparable non-empty evidence values");
  expectEqual(groups.at(0).sources.join("/"), "Read/Watch",
              "matching evidence values merge sources");
  expectEqual(groups.at(0).value, "0x0006", "group keeps first display value");
  expectEqual(groups.at(1).sources.join("/"), "Startup",
              "different values stay in separate groups");
}

void testWriteDeltaReview() {
  const QVector<SdoEvidenceItem> items = {{"Read", "0x0006"},
                                          {"Watch", "0x0007"}};
  const SdoWriteDeltaReview conflict = reviewSdoWriteDelta(items, "0x0006");
  expectEqual(conflict.state, "conflict",
              "mixed local evidence reports conflict state");
  expectTrue(conflict.hasConflict, "mixed local evidence sets conflict flag");
  expectTrue(conflict.hasDiff, "write value differs from at least one group");
  expectEqual(conflict.matchingSources.join("/"), "Read",
              "matching sources are preserved");
  expectTrue(conflict.differingGroups.size() == 1,
             "differing groups are preserved for localized UI formatting");
  expectEqual(conflict.differingGroups.first().sources.join("/"), "Watch",
              "differing group source is preserved");

  const SdoWriteDeltaReview diff =
      reviewSdoWriteDelta({{"OD", "0x0005"}}, "0x0006");
  expectEqual(diff.state, "diff", "single differing evidence reports diff");
  expectTrue(diff.hasDiff && !diff.hasConflict,
             "single differing evidence does not report conflict");

  const SdoWriteDeltaReview match =
      reviewSdoWriteDelta({{"OD", "0x0006"}}, "0x0006");
  expectEqual(match.state, "match", "matching evidence reports match");
  expectTrue(match.matchesEvidence && !match.hasDiff,
             "matching evidence sets match flags");

  const SdoWriteDeltaReview none = reviewSdoWriteDelta({}, "0x0006");
  expectEqual(none.state, "none", "empty evidence reports none");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testSdoEvidenceKey();
  testHistoryStartupSource();
  testPreferredEvidenceValue();
  testComparableValues();
  testEvidenceConflict();
  testEvidenceGrouping();
  testWriteDeltaReview();
  return 0;
}
