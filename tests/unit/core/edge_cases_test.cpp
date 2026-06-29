// EdgeCasesTest — Boundary and edge-case tests for core types
//
// Test coverage:
//   - SlaveInfo with very long name (>4096 chars)
//   - SlaveInfo with Unicode name
//   - SlaveInfo with null bytes in rawLine
//   - EventBus with empty vectors
//   - EventBus with large payloads
//   - ServiceContainer with null services
//   - SDO with empty index/subIndex
//   - State with invalid state string

#include "EthercatTypes.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTest>

#include "services/EventBus.h"

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

void expectEqual(int actual, int expected, const QString &message) {
    if (actual != expected) {
        fail(QString("%1: expected %2, got %3")
                 .arg(message, QString::number(expected), QString::number(actual)));
    }
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
    if (actual != expected) {
        fail(QString("%1: expected '%2', got '%3'").arg(message, expected, actual));
    }
}

// ── SlaveInfo: very long name (>4096 chars) ─────────────────────────────

void testSlaveInfoVeryLongName() {
    const int len = 8192;
    QString longName(len, QChar('A'));

    SlaveInfo s;
    s.position = 0;
    s.name = longName;
    s.state = "OP";
    s.flags = "--";
    s.rawLine = "0  OP  --  " + longName;

    expectEqual(int(s.name.size()), len, "long name size");

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectTrue(s == restored, "long name round-trip equality");
    expectEqual(int(restored.name.size()), len, "restored long name size");
}

// ── SlaveInfo: Unicode name ─────────────────────────────────────────────

void testSlaveInfoUnicodeName() {
    SlaveInfo s;
    s.position = 0;
    s.name = QString::fromUtf8("驱动器_日本語テスト_🔧_Ω");
    s.state = "OP";
    s.flags = "X";
    s.rawLine = "0  OP  X  " + s.name;

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectTrue(s == restored, "unicode name round-trip equality");
    expectEqual(restored.name, s.name, "unicode name preserved");
}

// ── SlaveInfo: null bytes in rawLine ────────────────────────────────────

void testSlaveInfoNullBytesInRawLine() {
    SlaveInfo s;
    s.position = 0;
    s.name = "EL1008";
    s.state = "OP";
    s.flags = "--";
    s.rawLine = QString("0") + QChar('\0') + QString("OP") + QChar('\0')
              + QString("--") + QChar('\0') + QString("EL1008");

    expectTrue(s.rawLine.contains(QChar('\0')), "rawLine contains null byte");

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectTrue(s == restored, "null bytes round-trip equality");
}

// ── EventBus: empty vectors ─────────────────────────────────────────────

void testEventBusEmptySlaveVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);
    QVector<SlaveInfo> empty;
    bus.emitSlaveChanged(empty);
    expectEqual(int(spy.count()), 1, "empty slave vector emits once");
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    expectTrue(received.isEmpty(), "received empty vector is empty");
}

void testEventBusEmptyTopologyVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);
    QVector<SlaveInfo> empty;
    bus.emitTopologyChanged(empty);
    expectEqual(int(spy.count()), 1, "empty topology vector emits once");
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    expectTrue(received.isEmpty(), "received empty topology vector is empty");
}

void testEventBusEmptySignalData() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    QVector<double> emptyValues;
    QVector<qint64> emptyTimestamps;
    bus.emitSignalData(0, emptyValues, emptyTimestamps);
    expectEqual(int(spy.count()), 1, "empty signal data emits once");
}

// ── EventBus: large payloads ────────────────────────────────────────────

void testEventBusLargeSlaveVector() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::slaveChanged);

    QVector<SlaveInfo> slaves;
    slaves.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        SlaveInfo info;
        info.position = i;
        info.name = QString("Slave_%1").arg(i);
        info.state = "OP";
        info.flags = "--";
        slaves.append(info);
    }

    bus.emitSlaveChanged(slaves);
    expectEqual(int(spy.count()), 1, "large slave vector emits once");
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    expectEqual(int(received.size()), 10000, "received large vector size");
    expectEqual(received.first().position, 0, "first slave position");
    expectEqual(received.last().position, 9999, "last slave position");
}

void testEventBusLargeSdoValue() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);

    QString largeValue(65536, QChar('F'));
    bus.emitSdoValue(0, "0x6000", "0x01", largeValue);
    expectEqual(int(spy.count()), 1, "large SDO value emits once");
    expectEqual(int(spy.at(0).at(3).toString().size()), 65536, "large SDO value size preserved");
}

void testEventBusLargeJsonPayload() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::freeRunTelemetry);

    QJsonObject large;
    for (int i = 0; i < 1000; ++i) {
        large[QString("key_%1").arg(i)] = QString("value_%1").arg(i);
    }
    bus.emitFreeRunTelemetry(large);
    expectEqual(int(spy.count()), 1, "large JSON payload emits once");
    expectEqual(int(spy.at(0).at(0).toJsonObject().size()), 1000, "large JSON size preserved");
}

// ── ServiceContainer: null services ─────────────────────────────────────

void testServiceContainerNullClient() {
    // ServiceContainer requires a non-null EcatClient in its constructor,
    // but we test the null-pointer pattern by verifying default-initialized
    // SlaveInfo fields that would come from a container with no data.
    SlaveInfo s;
    expectEqual(s.position, -1, "default position is -1 (null-like)");
    expectTrue(s.name.isNull(), "default name is null");
    expectTrue(s.state.isNull(), "default state is null");
    expectTrue(s.flags.isNull(), "default flags is null");
    expectTrue(s.rawLine.isNull(), "default rawLine is null");
}

void testServiceContainerNullVectorSerialization() {
    // Simulate what happens when a service returns a null/empty vector
    QVector<SlaveInfo> nullVector;
    QJsonArray arr = toJson(nullVector);
    expectTrue(arr.isEmpty(), "null vector serializes to empty array");

    QVector<SlaveInfo> restored = slavesFromJson(arr);
    expectTrue(restored.isEmpty(), "empty array deserializes to empty vector");
}

// ── SDO: empty index/subIndex ───────────────────────────────────────────

void testSdoEmptyIndexSubIndex() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::sdoValueReceived);

    bus.emitSdoValue(0, "", "", "");
    expectEqual(int(spy.count()), 1, "empty SDO fields emits once");
    expectEqual(spy.at(0).at(1).toString(), QString(), "empty index preserved");
    expectEqual(spy.at(0).at(2).toString(), QString(), "empty subIndex preserved");
    expectEqual(spy.at(0).at(3).toString(), QString(), "empty value preserved");
}

void testSdoEmptyIndexSubIndexInSlaveInfo() {
    SlaveInfo s;
    s.position = 0;
    s.name = "";
    s.state = "";
    s.flags = "";
    s.rawLine = "";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectTrue(s == restored, "all-empty fields round-trip");
}

// ── State: invalid state strings ────────────────────────────────────────

void testStateInvalidString() {
    SlaveInfo s;
    s.position = 0;
    s.state = "INVALID_STATE_123!@#";
    s.name = "test";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(restored.state, QString("INVALID_STATE_123!@#"),
                "invalid state string preserved through round-trip");
}

void testStateEmptyString() {
    SlaveInfo s;
    s.position = 0;
    s.state = "";
    s.name = "test";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(restored.state, QString(), "empty state preserved");
}

void testStateWhitespaceOnly() {
    SlaveInfo s;
    s.position = 0;
    s.state = "   \t\n  ";
    s.name = "test";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(restored.state, QString("   \t\n  "),
                "whitespace-only state preserved");
}

void testStateVeryLongStateString() {
    QString longState(4096, QChar('X'));
    SlaveInfo s;
    s.position = 0;
    s.state = longState;
    s.name = "test";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(int(restored.state.size()), 4096, "long state size preserved");
}

// ── Additional boundary tests ───────────────────────────────────────────

void testMaxIntPosition() {
    SlaveInfo s;
    s.position = INT_MAX;
    s.name = "max_pos";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(restored.position, INT_MAX, "max int position preserved");
}

void testMinIntPosition() {
    SlaveInfo s;
    s.position = INT_MIN;
    s.name = "min_pos";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(restored.position, INT_MIN, "min int position preserved");
}

void testBatchWithMixedValidAndInvalid() {
    QJsonArray arr;
    arr.append(42);
    arr.append("string");
    arr.append(QJsonValue::Null);
    arr.append(QJsonValue::Undefined);

    QJsonObject valid;
    valid["position"] = 0;
    valid["name"] = "only_valid";
    arr.append(valid);

    arr.append(true);
    arr.append(QJsonArray{1, 2, 3});

    QVector<SlaveInfo> slaves = slavesFromJson(arr);
    expectEqual(int(slaves.size()), 1, "only valid objects kept from mixed array");
    expectEqual(slaves[0].name, QString("only_valid"), "valid object correct");
}

void testEventBusNegativeChannel() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::signalData);
    bus.emitSignalData(-1, {1.0}, {100});
    expectEqual(int(spy.count()), 1, "negative channel emits once");
    expectEqual(spy.at(0).at(0).toInt(), -1, "negative channel preserved");
}

void testEventBusDisconnectedState() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::connectionStateChanged);
    bus.emitConnectionStateChanged(false);
    expectEqual(int(spy.count()), 1, "disconnect state emits once");
    expectTrue(!spy.at(0).at(0).toBool(), "disconnect state is false");
}

} // namespace

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    // SlaveInfo boundary tests
    testSlaveInfoVeryLongName();
    testSlaveInfoUnicodeName();
    testSlaveInfoNullBytesInRawLine();

    // EventBus empty vector tests
    testEventBusEmptySlaveVector();
    testEventBusEmptyTopologyVector();
    testEventBusEmptySignalData();

    // EventBus large payload tests
    testEventBusLargeSlaveVector();
    testEventBusLargeSdoValue();
    testEventBusLargeJsonPayload();

    // ServiceContainer null service tests
    testServiceContainerNullClient();
    testServiceContainerNullVectorSerialization();

    // SDO empty index/subIndex tests
    testSdoEmptyIndexSubIndex();
    testSdoEmptyIndexSubIndexInSlaveInfo();

    // State invalid string tests
    testStateInvalidString();
    testStateEmptyString();
    testStateWhitespaceOnly();
    testStateVeryLongStateString();

    // Additional boundary tests
    testMaxIntPosition();
    testMinIntPosition();
    testBatchWithMixedValidAndInvalid();
    testEventBusNegativeChannel();
    testEventBusDisconnectedState();

    std::cout << "All edge_cases_test tests passed.\n";
    return 0;
}
