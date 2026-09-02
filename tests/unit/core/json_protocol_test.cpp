// JsonProtocolTest — Tests for the wire protocol used in GUI-daemon communication.
//
// Test coverage:
//   - encode() produces valid compact JSON with trailing newline
//   - request() creates correct request envelope
//   - success() creates correct success response envelope
//   - failure() creates correct failure response envelope
//   - Parsing round-trip for each message type
//   - Edge cases: empty method, null/empty result, special characters

#include "JsonProtocol.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QtTest/QtTest>

class JsonProtocolTest : public QObject {
    Q_OBJECT

private slots:
    void encodeProducesValidJson();
    void encodeEndsWithNewline();
    void encodeIsCompact();
    void requestHasIdMethodParams();
    void requestWithEmptyMethod();
    void requestWithParams();
    void successHasIdOkResult();
    void successWithEmptyResult();
    void successOkIsTrue();
    void failureHasIdOkError();
    void failureOkIsFalse();
    void failureDefaultCode();
    void failureCustomCode();
    void parseEncodedRequest();
    void parseEncodedSuccess();
    void parseEncodedFailure();
    void roundTripRequest();
    void roundTripSuccess();
    void roundTripFailure();
    void requestWithSpecialChars();
    void failureWithSpecialChars();
};

// ── encode() ─────────────────────────────────────────────────────────

void JsonProtocolTest::encodeProducesValidJson() {
    auto bytes = JsonProtocol::encode({{"key", "value"}});
    auto doc = QJsonDocument::fromJson(bytes.trimmed());
    QVERIFY(!doc.isNull());
    QVERIFY(doc.isObject());
}

void JsonProtocolTest::encodeEndsWithNewline() {
    auto bytes = JsonProtocol::encode({{"a", 1}});
    QVERIFY(bytes.endsWith('\n'));
}

void JsonProtocolTest::encodeIsCompact() {
    auto bytes = JsonProtocol::encode({{"a", 1}});
    QCOMPARE(bytes.count('\n'), 1);
    QVERIFY(!bytes.contains(" \n"));
}

// ── request() ────────────────────────────────────────────────────────

void JsonProtocolTest::requestHasIdMethodParams() {
    auto obj = JsonProtocol::request("1", "getStatus");
    QCOMPARE(obj["id"].toString(), QString("1"));
    QCOMPARE(obj["method"].toString(), QString("getStatus"));
    QVERIFY(obj.contains("params"));
}

void JsonProtocolTest::requestWithEmptyMethod() {
    auto obj = JsonProtocol::request("42", "");
    QCOMPARE(obj["id"].toString(), QString("42"));
    QCOMPARE(obj["method"].toString(), QString(""));
}

void JsonProtocolTest::requestWithParams() {
    QJsonObject params{{"index", 0x1000}, {"subindex", 1}};
    auto obj = JsonProtocol::request("7", "sdoRead", params);
    QCOMPARE(obj["params"].toObject()["index"].toInt(), 0x1000);
    QCOMPARE(obj["params"].toObject()["subindex"].toInt(), 1);
}

// ── success() ────────────────────────────────────────────────────────

void JsonProtocolTest::successHasIdOkResult() {
    auto obj = JsonProtocol::success("5", {{"value", 42}});
    QCOMPARE(obj["id"].toString(), QString("5"));
    QVERIFY(obj.contains("ok"));
    QVERIFY(obj.contains("result"));
}

void JsonProtocolTest::successWithEmptyResult() {
    auto obj = JsonProtocol::success("10");
    QCOMPARE(obj["id"].toString(), QString("10"));
    QVERIFY(obj["result"].isObject());
    QVERIFY(obj["result"].toObject().isEmpty());
}

void JsonProtocolTest::successOkIsTrue() {
    auto obj = JsonProtocol::success("1");
    QCOMPARE(obj["ok"].toBool(), true);
}

// ── failure() ────────────────────────────────────────────────────────

void JsonProtocolTest::failureHasIdOkError() {
    auto obj = JsonProtocol::failure("3", "not found");
    QCOMPARE(obj["id"].toString(), QString("3"));
    QVERIFY(obj.contains("ok"));
    QVERIFY(obj.contains("error"));
}

void JsonProtocolTest::failureOkIsFalse() {
    auto obj = JsonProtocol::failure("1", "err");
    QCOMPARE(obj["ok"].toBool(), false);
}

void JsonProtocolTest::failureDefaultCode() {
    auto obj = JsonProtocol::failure("2", "generic error");
    QCOMPARE(obj["error"].toObject()["code"].toInt(), -1);
    QCOMPARE(obj["error"].toObject()["message"].toString(), QString("generic error"));
}

void JsonProtocolTest::failureCustomCode() {
    auto obj = JsonProtocol::failure("9", "timeout", 408);
    QCOMPARE(obj["error"].toObject()["code"].toInt(), 408);
    QCOMPARE(obj["error"].toObject()["message"].toString(), QString("timeout"));
}

// ── Parsing ──────────────────────────────────────────────────────────

void JsonProtocolTest::parseEncodedRequest() {
    auto bytes = JsonProtocol::encode(JsonProtocol::request("1", "ping"));
    auto doc = QJsonDocument::fromJson(bytes.trimmed());
    auto obj = doc.object();
    QCOMPARE(obj["id"].toString(), QString("1"));
    QCOMPARE(obj["method"].toString(), QString("ping"));
}

void JsonProtocolTest::parseEncodedSuccess() {
    auto bytes = JsonProtocol::encode(JsonProtocol::success("2", {{"data", "ok"}}));
    auto doc = QJsonDocument::fromJson(bytes.trimmed());
    auto obj = doc.object();
    QCOMPARE(obj["ok"].toBool(), true);
    QCOMPARE(obj["result"].toObject()["data"].toString(), QString("ok"));
}

void JsonProtocolTest::parseEncodedFailure() {
    auto bytes = JsonProtocol::encode(JsonProtocol::failure("3", "bad", 400));
    auto doc = QJsonDocument::fromJson(bytes.trimmed());
    auto obj = doc.object();
    QCOMPARE(obj["ok"].toBool(), false);
    QCOMPARE(obj["error"].toObject()["code"].toInt(), 400);
}

// ── Round-trip ───────────────────────────────────────────────────────

void JsonProtocolTest::roundTripRequest() {
    QJsonObject params{{"slave", 1}};
    auto encoded = JsonProtocol::encode(JsonProtocol::request("r1", "readState", params));
    auto decoded = QJsonDocument::fromJson(encoded.trimmed()).object();
    QCOMPARE(decoded["id"].toString(), QString("r1"));
    QCOMPARE(decoded["method"].toString(), QString("readState"));
    QCOMPARE(decoded["params"].toObject()["slave"].toInt(), 1);
}

void JsonProtocolTest::roundTripSuccess() {
    auto encoded = JsonProtocol::encode(JsonProtocol::success("r2", {{"state", "OP"}}));
    auto decoded = QJsonDocument::fromJson(encoded.trimmed()).object();
    QCOMPARE(decoded["ok"].toBool(), true);
    QCOMPARE(decoded["result"].toObject()["state"].toString(), QString("OP"));
}

void JsonProtocolTest::roundTripFailure() {
    auto encoded = JsonProtocol::encode(JsonProtocol::failure("r3", "not ready", 503));
    auto decoded = QJsonDocument::fromJson(encoded.trimmed()).object();
    QCOMPARE(decoded["ok"].toBool(), false);
    QCOMPARE(decoded["error"].toObject()["code"].toInt(), 503);
    QCOMPARE(decoded["error"].toObject()["message"].toString(), QString("not ready"));
}

// ── Edge cases ───────────────────────────────────────────────────────

void JsonProtocolTest::requestWithSpecialChars() {
    auto obj = JsonProtocol::request("s1", "cmd\"with\nescapes");
    auto encoded = JsonProtocol::encode(obj);
    auto decoded = QJsonDocument::fromJson(encoded.trimmed()).object();
    QCOMPARE(decoded["method"].toString(), QString("cmd\"with\nescapes"));
}

void JsonProtocolTest::failureWithSpecialChars() {
    auto obj = JsonProtocol::failure("s2", "error: \"value\" not found\nretry");
    auto encoded = JsonProtocol::encode(obj);
    auto decoded = QJsonDocument::fromJson(encoded.trimmed()).object();
    QCOMPARE(decoded["error"].toObject()["message"].toString(), QString("error: \"value\" not found\nretry"));
}

QTEST_MAIN(JsonProtocolTest)
#include "json_protocol_test.moc"
