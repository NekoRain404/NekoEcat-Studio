#include "models/Cia402DriveModel.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected '%2', got '%3'").arg(message, expected, actual));
  }
}

void expectRecommendation(const QString &status, const QString &label,
                          const QString &value, const QString &message) {
  const Cia402ControlwordRecommendation recommendation =
      recommendedCia402ControlwordFromStatus(status);
  expectEqual(recommendation.label, label, message + " label");
  expectEqual(recommendation.value, value, message + " value");
  expectEqual(recommendation.reason, status, message + " reason");
}

void testRecommendations() {
  expectRecommendation("Fault (0x0008)", "Fault Reset", "0x0080",
                       "fault recommends reset");
  expectRecommendation("Switch on disabled (0x0040)", "Shutdown", "0x0006",
                       "switch-on-disabled recommends shutdown");
  expectRecommendation("Ready to switch on (0x0021)", "Switch On", "0x0007",
                       "ready-to-switch-on recommends switch on");
  expectRecommendation("Switched on (0x0023)", "Enable Operation", "0x000f",
                       "switched-on recommends enable operation");
  expectRecommendation("Quick stop active (0x0007)", "Shutdown", "0x0006",
                       "quick-stop-active recommends shutdown");
}

void testNoRecommendation() {
  expectRecommendation("Operation enabled (0x0027)", QString(), QString(),
                       "operation-enabled has no next controlword");
  expectRecommendation(QString(), QString(), QString(),
                       "empty status has no recommendation");
}

void testCia402ObjectDetection() {
  if (!isCia402Object("0x6041")) {
    fail("statusword index is CiA 402");
  }
  if (!isCia402Object("0x6064")) {
    fail("actual position index is CiA 402");
  }
  if (!isCia402Object("0x2000", "CiA 402 vendor object")) {
    fail("CiA 402 mode marks vendor object as CiA 402");
  }
  if (isCia402Object("0x2000")) {
    fail("unmarked vendor object is not CiA 402");
  }
}

void testCia402Decode() {
  expectEqual(decodeCia402Value("0x6041", "0x0037"),
              "Operation enabled (0x0037, voltage)",
              "statusword decodes state and flags");
  expectEqual(decodeCia402Value("0x6040", "0x000f"),
              "enable operation (0x000f)", "controlword decodes command");
  expectEqual(decodeCia402Value("0x6060", "8"), "Cyclic sync position",
              "mode decodes known mode");
  expectEqual(decodeCia402Value("0x6061", "123"), "Mode 123",
              "mode decodes unknown mode");
  expectEqual(decodeCia402Value("0x603f", "0"), "No error",
              "zero error code decodes as no error");
  expectEqual(decodeCia402Value("0x603f", "0x2310"), "Error code 0x2310",
              "non-zero error code is shown in hex");
  expectEqual(decodeCia402Value("0x6064", "42"), "Actual position",
              "actual position object decodes by name");
  expectEqual(decodeCia402Value("0x60ff", "42"), "Target velocity",
              "target velocity object decodes by name");
  expectEqual(decodeCia402Value("0x6041", "not-number"), QString(),
              "invalid numeric value has no decode");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testRecommendations();
  testNoRecommendation();
  testCia402ObjectDetection();
  testCia402Decode();
  return 0;
}
