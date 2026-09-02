// EthercatTypesTest — Tests for EthercatTypes domain types
//
// Test coverage:
//   - SlaveInfo default construction
//   - toJson() serialization
//   - slaveFromJson() deserialization
//   - Round-trip (toJson -> slaveFromJson)
//   - Batch (QVector) serialization / deserialization
//   - Edge cases (empty name, negative position, special characters)
#include "EthercatTypes.h"

#include <QJsonArray>
#include <QJsonObject>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString& message) {
    std::cerr << message.toStdString() << '\n';
    std::exit(1);
}

void expectTrue(bool condition, const QString& message) {
    if (!condition) {
        fail(message);
    }
}

void expectEqual(int actual, int expected, const QString& message) {
    if (actual != expected) {
        fail(QString("%1: expected %2, got %3").arg(message, QString::number(expected), QString::number(actual)));
    }
}

void expectEqual(const QString& actual, const QString& expected, const QString& message) {
    if (actual != expected) {
        fail(QString("%1: expected '%2', got '%3'").arg(message, expected, actual));
    }
}

void testDefaultConstruction() {
    SlaveInfo s;
    expectEqual(s.position, -1, "default position");
    expectEqual(s.state, QString(), "default state");
    expectEqual(s.flags, QString(), "default flags");
    expectEqual(s.name, QString(), "default name");
    expectEqual(s.rawLine, QString(), "default rawLine");
}

void testToJson() {
    SlaveInfo s;
    s.position = 2;
    s.state = "OP";
    s.flags = "-I-";
    s.name = "EL1008";
    s.rawLine = "2  OP  -I-  EL1008";

    QJsonObject obj = toJson(s);
    expectEqual(obj["position"].toInt(), 2, "toJson position");
    expectEqual(obj["state"].toString(), QString("OP"), "toJson state");
    expectEqual(obj["flags"].toString(), QString("-I-"), "toJson flags");
    expectEqual(obj["name"].toString(), QString("EL1008"), "toJson name");
    expectEqual(obj["rawLine"].toString(), QString("2  OP  -I-  EL1008"), "toJson rawLine");
}

void testSlaveFromJson() {
    QJsonObject obj;
    obj["position"] = 3;
    obj["state"] = "SAFEOP";
    obj["flags"] = "-I-";
    obj["name"] = "EK1100";
    obj["rawLine"] = "3  SAFEOP  -I-  EK1100";

    SlaveInfo s = slaveFromJson(obj);
    expectEqual(s.position, 3, "fromJson position");
    expectEqual(s.state, QString("SAFEOP"), "fromJson state");
    expectEqual(s.flags, QString("-I-"), "fromJson flags");
    expectEqual(s.name, QString("EK1100"), "fromJson name");
    expectEqual(s.rawLine, QString("3  SAFEOP  -I-  EK1100"), "fromJson rawLine");
}

void testRoundTrip() {
    SlaveInfo original;
    original.position = 5;
    original.state = "PREOP";
    original.flags = "I--";
    original.name = "AX5206";
    original.rawLine = "5  PREOP  I--  AX5206";

    QJsonObject obj = toJson(original);
    SlaveInfo restored = slaveFromJson(obj);

    expectTrue(original == restored, "round-trip equality");
}

void testRoundTripDefaultValues() {
    SlaveInfo original;
    QJsonObject obj = toJson(original);
    SlaveInfo restored = slaveFromJson(obj);
    expectTrue(original == restored, "round-trip default equality");
}

void testBatchSerialization() {
    QVector<SlaveInfo> slaves;
    SlaveInfo s1;
    s1.position = 0;
    s1.name = "EK1100";
    slaves.append(s1);

    SlaveInfo s2;
    s2.position = 1;
    s2.name = "EL1008";
    slaves.append(s2);

    QJsonArray arr = toJson(slaves);
    expectEqual(arr.size(), 2, "batch toJson size");
    expectEqual(arr[0].toObject()["name"].toString(), QString("EK1100"), "batch toJson [0] name");
    expectEqual(arr[1].toObject()["name"].toString(), QString("EL1008"), "batch toJson [1] name");

    QVector<SlaveInfo> restored = slavesFromJson(arr);
    expectEqual(restored.size(), 2, "batch fromJson size");
    expectTrue(slaves[0] == restored[0], "batch [0] equality");
    expectTrue(slaves[1] == restored[1], "batch [1] equality");
}

void testBatchEmpty() {
    QVector<SlaveInfo> empty;
    QJsonArray arr = toJson(empty);
    expectTrue(arr.isEmpty(), "empty vector toJson");

    QVector<SlaveInfo> restored = slavesFromJson(arr);
    expectTrue(restored.isEmpty(), "empty array fromJson");
}

void testSlavesFromJsonSkipsNonObjects() {
    QJsonArray arr;
    arr.append(42);
    arr.append("not an object");
    QJsonObject valid;
    valid["position"] = 0;
    valid["name"] = "valid";
    arr.append(valid);

    QVector<SlaveInfo> slaves = slavesFromJson(arr);
    expectEqual(slaves.size(), 1, "skips non-objects count");
    expectEqual(slaves[0].name, QString("valid"), "skips non-objects kept");
}

void testEdgeEmptyName() {
    SlaveInfo s;
    s.position = 0;
    s.name = "";

    QJsonObject obj = toJson(s);
    expectEqual(obj["name"].toString(), QString(), "empty name serialized");

    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(restored.name, QString(), "empty name deserialized");
}

void testEdgeNegativePosition() {
    SlaveInfo s;
    s.position = -1;
    s.name = "unknown";

    QJsonObject obj = toJson(s);
    expectEqual(obj["position"].toInt(), -1, "negative position serialized");

    SlaveInfo restored = slaveFromJson(obj);
    expectEqual(restored.position, -1, "negative position deserialized");
}

void testEdgeSpecialCharacters() {
    SlaveInfo s;
    s.position = 0;
    s.name = "Über-Slave (v2.0) #1";
    s.state = "OP";
    s.flags = "X";
    s.rawLine = "0  OP  X  Über-Slave (v2.0) #1";

    QJsonObject obj = toJson(s);
    SlaveInfo restored = slaveFromJson(obj);
    expectTrue(s == restored, "special characters round-trip");
}

void testFromJsonMissingFields() {
    QJsonObject obj;
    obj["position"] = 4;
    SlaveInfo s = slaveFromJson(obj);
    expectEqual(s.position, 4, "missing fields position");
    expectEqual(s.state, QString(), "missing fields state defaults empty");
    expectEqual(s.flags, QString(), "missing fields flags defaults empty");
    expectEqual(s.name, QString(), "missing fields name defaults empty");
    expectEqual(s.rawLine, QString(), "missing fields rawLine defaults empty");
}

void testFromJsonWrongTypes() {
    QJsonObject obj;
    obj["position"] = "not a number";
    obj["state"] = 42;

    SlaveInfo s = slaveFromJson(obj);
    expectEqual(s.position, -1, "wrong type position defaults -1");
    expectEqual(s.state, QString(), "wrong type state defaults empty");
}

void testEqualityOperator() {
    SlaveInfo a;
    a.position = 1;
    a.name = "test";
    a.state = "OP";
    a.flags = "-";
    a.rawLine = "line";

    SlaveInfo b = a;
    expectTrue(a == b, "copied equality");

    b.position = 2;
    expectTrue(!(a == b), "different position inequality");

    b = a;
    b.name = "other";
    expectTrue(!(a == b), "different name inequality");
}

} // namespace

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    testDefaultConstruction();
    testToJson();
    testSlaveFromJson();
    testRoundTrip();
    testRoundTripDefaultValues();
    testBatchSerialization();
    testBatchEmpty();
    testSlavesFromJsonSkipsNonObjects();
    testEdgeEmptyName();
    testEdgeNegativePosition();
    testEdgeSpecialCharacters();
    testFromJsonMissingFields();
    testFromJsonWrongTypes();
    testEqualityOperator();

    std::cout << "All ethercat_types_test tests passed.\n";
    return 0;
}
