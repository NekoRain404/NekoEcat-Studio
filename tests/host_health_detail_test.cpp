// HostHealthDetailTest — Tests for HostHealthDetail
//
// Test coverage:
//   - Empty state (unchecked host health)
//   - Row counts, summary, and color keys
//   - Warning and ready summary generation
//   - Default field values

// Unit tests for HostHealthDetail.
#include "detail/HostHealthDetail.h"

#include <QCoreApplication>
#include <QJsonObject>

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
    fail(QString("%1: expected %2, got %3").arg(message, expected, actual));
  }
}

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
}

HostHealthTexts englishTexts() {
  return {
      .levelHeader = "Level",
      .checkHeader = "Check",
      .resultHeader = "Result",
      .actionHeader = "Action",
      .commandHeader = "Command",
      .detailHeader = "Detail",
      .unchecked = "Host health has not been checked",
      .needsAttention = "Needs attention",
      .usableWithWarnings = "Usable with warnings",
      .ready = "Host environment ready",
      .warningLabel = "Warning",
      .okLabel = "OK",
  };
}

QJsonObject check(const QString &level, const QString &source,
                  const QString &message, const QString &hint,
                  const QString &command, const QString &detail) {
  return {
      {"level", level}, {"source", source},   {"message", message},
      {"hint", hint},   {"command", command}, {"detail", detail},
  };
}

// Verify empty state with no checks
void testEmptyState() {
  const HostHealthDetail state =
      buildHostHealthDetail(QJsonArray(), englishTexts());
  expectEqual(state.headers.size(), 6, "header count");
  expectEqual(state.rows.size(), 0, "empty row count");
  expectEqual(state.summary, "Host health has not been checked",
              "unchecked summary");
}

// Verify row counts, summary, and color keys
void testRowsCountsSummaryAndColors() {
  QJsonArray checks;
  checks.append(check("Error", "Service", "offline", "restart",
                      "sudo systemctl restart ethercat", "service stopped"));
  checks.append(check("Warning", "NIC", "driver", "review", "modinfo r8152",
                      "dkms preferred"));
  checks.append(check("Info", "Device", "ready", "", "", "ok"));

  const HostHealthDetail state =
      buildHostHealthDetail(checks, englishTexts());
  expectEqual(state.rows.size(), 3, "host health row count");
  expectEqual(state.rows.at(0).at(1), "Service", "source cell");
  expectEqual(state.rows.at(1).at(4), "modinfo r8152", "command cell");
  expectEqual(state.errors, 1, "error count");
  expectEqual(state.warnings, 1, "warning count");
  expectEqual(state.infos, 1, "info count");
  expectEqual(state.colorKeys.at(0), "error", "error color key");
  expectEqual(state.colorKeys.at(1), "warning", "warning color key");
  expectEqual(state.colorKeys.at(2), "ok", "ok color key");
  expectEqual(state.summary, "Needs attention: 1   Warning: 1   OK: 1",
              "error summary");
}

// Verify warning and ready summary text
void testWarningAndReadySummaries() {
  QJsonArray warningChecks;
  warningChecks.append(check("Warning", "NIC", "driver", "", "", ""));
  HostHealthDetail state =
      buildHostHealthDetail(warningChecks, englishTexts());
  expectEqual(state.summary, "Usable with warnings   Warning: 1   OK: 0",
              "warning summary");

  QJsonArray readyChecks;
  readyChecks.append(check("Info", "Device", "ready", "", "", ""));
  readyChecks.append(QJsonObject{{"message", "default fields"}});
  state = buildHostHealthDetail(readyChecks, englishTexts());
  expectEqual(state.rows.at(1).at(0), "Info", "default level");
  expectEqual(state.rows.at(1).at(1), "Host", "default source");
  expectEqual(state.summary, "Host environment ready   OK: 2", "ready summary");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testEmptyState();
  testRowsCountsSummaryAndColors();
  testWarningAndReadySummaries();
  return 0;
}
