// Unit tests for EvidenceStatusModel.
#include "models/EvidenceModel.h"

#include <QCoreApplication>

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

void expectSeverity(DriveEvidenceSeverity actual,
                    DriveEvidenceSeverity expected, const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

void testStartupDiffEvidence() {
  expectTrue(hasStartupDiffEvidence("diff"), "diff is Startup diff evidence");
  expectTrue(hasStartupDiffEvidence("Watch diff: 0x0006 -> 0x000F"),
             "diff detail text is Startup diff evidence");
  expectTrue(hasStartupDiffEvidence("不一致"),
             "Chinese mismatch is Startup diff evidence");
  expectTrue(hasStartupDiffEvidence("Watch mismatch"),
             "mismatch text is Startup diff evidence");
  expectTrue(hasStartupDiffEvidence("启动偏差"),
             "Chinese diff text is Startup diff evidence");
  expectTrue(!hasStartupDiffEvidence("match"),
             "match is not Startup diff evidence");
  expectTrue(!hasStartupDiffEvidence("pending"),
             "pending is not Startup diff evidence");
}

void testPdoMapIssueEvidence() {
  expectTrue(hasPdoMapIssueEvidence("warning"),
             "warning is PDO map issue evidence");
  expectTrue(hasPdoMapIssueEvidence("Missing process map"),
             "missing is PDO map issue evidence");
  expectTrue(hasPdoMapIssueEvidence("No PDO Map"),
             "No PDO Map is PDO map issue evidence");
  expectTrue(hasPdoMapIssueEvidence("警告"),
             "Chinese warning is PDO map issue evidence");
  expectTrue(hasPdoMapIssueEvidence("缺失"),
             "Chinese missing is PDO map issue evidence");
  expectTrue(hasPdoMapIssueEvidence("无 PDO 映射"),
             "Chinese no PDO map is PDO map issue evidence");
  expectTrue(!hasPdoMapIssueEvidence("Mapped"),
             "mapped status is not PDO map issue evidence");
  expectTrue(!hasPdoMapIssueEvidence("OK"), "OK is not PDO map issue evidence");
}

void testDriveEvidenceSeverity() {
  expectSeverity(driveEvidenceSeverity(""), DriveEvidenceSeverity::Neutral,
                 "empty drive evidence is neutral");
  expectSeverity(driveEvidenceSeverity("Fault reaction active"),
                 DriveEvidenceSeverity::Error, "fault evidence is error");
  expectSeverity(driveEvidenceSeverity("error code 0x2310"),
                 DriveEvidenceSeverity::Error, "error code evidence is error");
  expectSeverity(driveEvidenceSeverity("错误 0x2310"),
                 DriveEvidenceSeverity::Error, "Chinese error is error");
  expectSeverity(driveEvidenceSeverity("quick stop active"),
                 DriveEvidenceSeverity::Warning, "quick stop is warning");
  expectSeverity(driveEvidenceSeverity("warning | internal limit"),
                 DriveEvidenceSeverity::Warning,
                 "warning or internal limit is warning");
  expectSeverity(driveEvidenceSeverity("operation enabled"),
                 DriveEvidenceSeverity::Ok, "operation enabled is ok");
  expectSeverity(driveEvidenceSeverity("ready to switch on"),
                 DriveEvidenceSeverity::Action,
                 "other non-empty evidence is action");
  expectTrue(hasDriveFaultEvidence("fault"),
             "fault drive evidence reports fault risk");
  expectTrue(!hasDriveFaultEvidence("operation enabled"),
             "healthy drive evidence does not report fault risk");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testStartupDiffEvidence();
  testPdoMapIssueEvidence();
  testDriveEvidenceSeverity();
  return 0;
}
