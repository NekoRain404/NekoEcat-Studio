// Unit tests for DcSyncHandler — DC sync status parsing and JSON output.
#include "handlers/DcSyncHandler.h"
#include "CommandDispatcher.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;

void fail(const QString &msg) {
  std::cerr << msg.toStdString() << '\n';
  ++failures;
}

void expectTrue(bool cond, const QString &msg) {
  if (!cond) fail(msg);
}

void expectEqual(int actual, int expected, const QString &msg) {
  if (actual != expected)
    fail(QString("%1: expected %2, got %3").arg(msg).arg(expected).arg(actual));
}

void expectEqual(int64_t actual, int64_t expected, const QString &msg) {
  if (actual != expected)
    fail(QString("%1: expected %2, got %3").arg(msg).arg(expected).arg(actual));
}

void expectEqual(const QString &actual, const QString &expected, const QString &msg) {
  if (actual != expected)
    fail(QString("%1: expected '%2', got '%3'").arg(msg, expected, actual));
}

// ─── Sample CLI outputs for testing ────────────────────────────────────────

const char *kSampleMasterOutput = R"(Master0
  Phase: Idle
  Active: no
  Slaves: 3
  DC reference clock: Slave 0
)";

const char *kSampleSlavesVerboseOutput = R"(=== Master 0, Slave 0 ===
Alias 0x0000, Position 0x0000, Vendor 0x00000002, Product 0x0c1e3052
  Distributed Clocks:
    Jitter:          12 ns
    Drift:           -5 ns
    Reference Clock
    System Time:     123456789 ns

=== Master 0, Slave 1 ===
Alias 0x0000, Position 0x0001, Vendor 0x00000002, Product 0x14000401
  Distributed Clocks:
    Jitter:          8 ns
    Drift:           3 ns
    System Time:     123456790 ns

=== Master 0, Slave 2 ===
Alias 0x0000, Position 0x0002, Vendor 0x00000002, Product 0x04000401
)";

} // namespace

// ─── T1: empty input returns no slaves ─────────────────────────────────────

void testEmptyInput() {
  DcSyncHandler handler;
  const auto slaves = handler.queryDcStatus(QString());
  expectTrue(slaves.isEmpty(), "T1: empty input yields no slaves");
}

// ─── T2: short-form slave output (no DC blocks) ───────────────────────────

void testShortFormNoDC() {
  DcSyncHandler handler;
  const QString input =
    "0  0:0   OP  +  EL1008\n"
    "1  0:1   OP  +  EL2008\n";
  const auto slaves = handler.queryDcStatus(input);
  expectEqual((int)slaves.size(), 2, "T2: two slaves parsed");
  expectEqual(slaves[0].position, 0, "T2: slave 0 position");
  expectTrue(!slaves[0].dcCapable, "T2: slave 0 not DC capable");
  expectEqual(slaves[1].position, 1, "T2: slave 1 position");
  expectTrue(!slaves[1].dcCapable, "T2: slave 1 not DC capable");
}

// ─── T3: verbose output with DC blocks ─────────────────────────────────────

void testVerboseWithDC() {
  DcSyncHandler handler;
  const auto slaves = handler.queryDcStatus(kSampleSlavesVerboseOutput);
  expectEqual((int)slaves.size(), 3, "T3: three slaves parsed");

  expectEqual(slaves[0].position, 0, "T3: slave 0 position");
  expectTrue(slaves[0].dcCapable, "T3: slave 0 DC capable");
  expectTrue(slaves[0].syncing, "T3: slave 0 syncing");
  expectEqual(slaves[0].jitterMinNs, (int64_t)12, "T3: slave 0 jitter min");
  expectEqual(slaves[0].jitterMaxNs, (int64_t)12, "T3: slave 0 jitter max");
  expectEqual(slaves[0].driftNs, (int64_t)-5, "T3: slave 0 drift");

  expectEqual(slaves[1].position, 1, "T3: slave 1 position");
  expectTrue(slaves[1].dcCapable, "T3: slave 1 DC capable");
  expectTrue(slaves[1].syncing, "T3: slave 1 syncing");
  expectEqual(slaves[1].jitterMinNs, (int64_t)8, "T3: slave 1 jitter min");

  expectEqual(slaves[2].position, 2, "T3: slave 2 position");
  expectTrue(!slaves[2].dcCapable, "T3: slave 2 not DC capable");
}

// ─── T4: detectRefClock from master output ─────────────────────────────────

void testDetectRefClock() {
  DcSyncHandler handler;
  const int ref = handler.detectRefClock(kSampleMasterOutput);
  expectEqual(ref, 0, "T4: reference clock is slave 0");
}

// ─── T5: detectRefClock returns -1 on empty ────────────────────────────────

void testDetectRefClockEmpty() {
  DcSyncHandler handler;
  expectEqual(handler.detectRefClock(QString()), -1, "T5: empty input returns -1");
  expectEqual(handler.detectRefClock("Master0\n  Phase: Idle\n"), -1,
              "T5: no ref clock returns -1");
}

// ─── T6: detectRefClock with alternate format ──────────────────────────────

void testDetectRefClockAlternate() {
  DcSyncHandler handler;
  const QString alt = "Slave 1 is the reference clock for DC";
  expectEqual(handler.detectRefClock(alt), 1, "T6: alternate format ref clock");
}

// ─── T7: handle() returns valid JSON envelope ──────────────────────────────

void testHandleEnvelope() {
  DcSyncHandler handler;
  QJsonObject params;
  params["master"] = "0";
  const QJsonObject response = handler.handle("test-1", params);

  // The response must always be a valid daemon envelope with id + ok fields.
  expectTrue(response.contains("ok"), "T7: response has ok field");
  expectTrue(response.contains("id"), "T7: response has id field");
  expectEqual(response.value("id").toString(), QString("test-1"), "T7: response id matches");

  // If the CLI is available, ok should be true and result should contain refClock.
  if (response.value("ok").toBool()) {
    const QJsonObject result = response.value("result").toObject();
    expectTrue(result.contains("refClock"), "T7: result has refClock");
    expectTrue(result.contains("slaves"), "T7: result has slaves");
  }
}

// ─── T8: multiple jitter values produce correct min/max/avg ────────────────

void testMultipleJitterValues() {
  DcSyncHandler handler;
  const QString input =
    "=== Master 0, Slave 0 ===\n"
    "Alias 0x0000, Position 0x0000, Vendor 0x00000002, Product 0x00000001\n"
    "  Distributed Clocks:\n"
    "    Jitter:          10 ns\n"
    "    Jitter:          25 ns\n"
    "    Jitter:          5 ns\n"
    "    System Time:     100 ns\n";

  const auto slaves = handler.queryDcStatus(input);
  expectEqual((int)slaves.size(), 1, "T8: one slave parsed");
  expectEqual(slaves[0].jitterMinNs, (int64_t)5, "T8: jitter min");
  expectEqual(slaves[0].jitterMaxNs, (int64_t)25, "T8: jitter max");
  expectEqual(slaves[0].jitterAvgNs, (int64_t)15, "T8: jitter avg");
}

// ─── T9: handle() response includes data fields ────────────────────────────

void testHandleData() {
  DcSyncHandler handler;
  QJsonObject params;
  params["master"] = "0";
  const QJsonObject response = handler.handle("test-2", params);

  // On a system with ethercat CLI, the response should be wrapped in success envelope.
  if (response.value("ok").toBool()) {
    const QJsonObject result = response.value("result").toObject();
    expectTrue(result.contains("refClock"), "T9: result has refClock");
    expectTrue(result.contains("slaves"), "T9: result has slaves");
  }
}

// ─── Main ──────────────────────────────────────────────────────────────────

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  testEmptyInput();
  testShortFormNoDC();
  testVerboseWithDC();
  testDetectRefClock();
  testDetectRefClockEmpty();
  testDetectRefClockAlternate();
  testHandleEnvelope();
  testMultipleJitterValues();
  testHandleData();

  if (failures > 0) {
    std::cerr << failures << " test(s) failed.\n";
    return 1;
  }
  std::cout << "All DC sync handler tests passed.\n";
  return 0;
}
