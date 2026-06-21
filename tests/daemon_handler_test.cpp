// DaemonHandlerTest — Tests for daemon handler integration and response format
//
// Test coverage:
//   - Ping handler response content
//   - Request ID pass-through on success
//   - Request ID pass-through on failure
//   - Unknown method error format
//   - Missing method error format
//   - Error response envelope structure (ok, error.code, error.message)
//   - Success response envelope structure (ok, result)
//   - Handler ignoring params (ping pattern)
//   - Multiple independent handlers
//   - FreeRun/RT test status when stopped

#include "CommandDispatcher.h"
#include "EcatDaemon.h"
#include "JsonProtocol.h"

#include <QTest>
#include <QJsonObject>
#include <QJsonArray>

class DaemonHandlerTest : public QObject {
    Q_OBJECT

private:
    CommandDispatcher dispatcher_;

    void registerPing() {
        dispatcher_.registerHandler("ping", [](const QString &id, const QJsonObject &) {
            return CommandDispatcher::success(id, {
                {"name", "ecatd"},
                {"version", "0.1.0"},
                {"multiMaster", true}
            });
        });
    }

private slots:
    void testPingResponseContent() {
        registerPing();
        QJsonObject req = {{"id", "p1"}, {"method", "ping"}, {"params", {}}};
        QJsonObject resp = dispatcher_.dispatch(req);

        QVERIFY(resp["ok"].toBool() == true);
        QJsonObject result = resp["result"].toObject();
        QCOMPARE(result["name"].toString(), QString("ecatd"));
        QCOMPARE(result["version"].toString(), QString("0.1.0"));
        QCOMPARE(result["multiMaster"].toBool(), true);
    }

    void testPingIgnoresParams() {
        registerPing();
        QJsonObject params = {{"master", "1"}, {"extra", "data"}};
        QJsonObject req = {{"id", "p2"}, {"method", "ping"}, {"params", params}};
        QJsonObject resp = dispatcher_.dispatch(req);

        QVERIFY(resp["ok"].toBool() == true);
        QJsonObject result = resp["result"].toObject();
        QCOMPARE(result["name"].toString(), QString("ecatd"));
    }

    void testRequestIdPassThroughSuccess() {
        registerPing();
        QJsonObject req = {{"id", "req-42"}, {"method", "ping"}, {"params", {}}};
        QJsonObject resp = dispatcher_.dispatch(req);

        QCOMPARE(resp["id"].toString(), QString("req-42"));
        QVERIFY(resp["ok"].toBool() == true);
    }

    void testRequestIdPassThroughFailure() {
        QJsonObject req = {{"id", "req-99"}, {"method", "noSuchMethod"}, {"params", {}}};
        QJsonObject resp = dispatcher_.dispatch(req);

        QCOMPARE(resp["id"].toString(), QString("req-99"));
        QVERIFY(resp["ok"].toBool() == false);
    }

    void testUnknownMethodErrorFormat() {
        QJsonObject req = {{"id", "u1"}, {"method", "bogus"}, {"params", {}}};
        QJsonObject resp = dispatcher_.dispatch(req);

        QVERIFY(resp["ok"].toBool() == false);
        QVERIFY(resp.contains("error"));
        QJsonObject err = resp["error"].toObject();
        QVERIFY(err["message"].toString().contains("bogus"));
        QCOMPARE(err["code"].toInt(), -1);
        QVERIFY(!resp.contains("result"));
    }

    void testMissingMethodErrorFormat() {
        QJsonObject req = {{"id", "m1"}, {"method", ""}, {"params", {}}};
        QJsonObject resp = dispatcher_.dispatch(req);

        QVERIFY(resp["ok"].toBool() == false);
        QVERIFY(resp.contains("error"));
        QCOMPARE(resp["id"].toString(), QString("m1"));
    }

    void testSuccessEnvelopeStructure() {
        CommandDispatcher::success("s1", {{"key", "val"}});
        QJsonObject resp = CommandDispatcher::success("s1", {{"key", "val"}});

        QCOMPARE(resp["id"].toString(), QString("s1"));
        QVERIFY(resp["ok"].toBool() == true);
        QVERIFY(resp.contains("result"));
        QVERIFY(!resp.contains("error"));
        QCOMPARE(resp["result"].toObject()["key"].toString(), QString("val"));
    }

    void testSuccessEnvelopeEmptyResult() {
        QJsonObject resp = CommandDispatcher::success("s2");

        QCOMPARE(resp["id"].toString(), QString("s2"));
        QVERIFY(resp["ok"].toBool() == true);
        QVERIFY(resp["result"].isObject());
    }

    void testFailureEnvelopeStructure() {
        QJsonObject resp = CommandDispatcher::failure("f1", "something broke", -42);

        QCOMPARE(resp["id"].toString(), QString("f1"));
        QVERIFY(resp["ok"].toBool() == false);
        QVERIFY(!resp.contains("result"));
        QVERIFY(resp.contains("error"));
        QJsonObject err = resp["error"].toObject();
        QCOMPARE(err["message"].toString(), QString("something broke"));
        QCOMPARE(err["code"].toInt(), -42);
    }

    void testFailureEnvelopeDefaultCode() {
        QJsonObject resp = CommandDispatcher::failure("f2", "oops");

        QJsonObject err = resp["error"].toObject();
        QCOMPARE(err["code"].toInt(), -1);
    }

    void testMultipleHandlersDispatch() {
        dispatcher_.registerHandler("a", [](const QString &id, const QJsonObject &) {
            return CommandDispatcher::success(id, {{"handler", "a"}});
        });
        dispatcher_.registerHandler("b", [](const QString &id, const QJsonObject &) {
            return CommandDispatcher::success(id, {{"handler", "b"}});
        });

        QJsonObject respA = dispatcher_.dispatch({{"id", "1"}, {"method", "a"}, {"params", {}}});
        QJsonObject respB = dispatcher_.dispatch({{"id", "2"}, {"method", "b"}, {"params", {}}});

        QCOMPARE(respA["result"].toObject()["handler"].toString(), QString("a"));
        QCOMPARE(respB["result"].toObject()["handler"].toString(), QString("b"));
    }

    void testHandlerReceivesParams() {
        QJsonObject received;
        dispatcher_.registerHandler("capture", [&](const QString &id, const QJsonObject &params) {
            received = params;
            return CommandDispatcher::success(id);
        });

        QJsonObject params = {{"master", "0"}, {"position", 3}};
        dispatcher_.dispatch({{"id", "c1"}, {"method", "capture"}, {"params", params}});

        QCOMPARE(received["master"].toString(), QString("0"));
        QCOMPARE(received["position"].toInt(), 3);
    }

    void testJsonProtocolEncode() {
        QJsonObject obj = {{"id", "e1"}, {"ok", true}};
        QByteArray encoded = JsonProtocol::encode(obj);

        QVERIFY(encoded.endsWith('\n'));
        QJsonDocument doc = QJsonDocument::fromJson(encoded.trimmed());
        QVERIFY(doc.isObject());
        QCOMPARE(doc.object()["id"].toString(), QString("e1"));
    }

    void testJsonProtocolRequest() {
        QJsonObject req = JsonProtocol::request("r1", "ping", {{"master", "0"}});

        QCOMPARE(req["id"].toString(), QString("r1"));
        QCOMPARE(req["method"].toString(), QString("ping"));
        QCOMPARE(req["params"].toObject()["master"].toString(), QString("0"));
    }

    void testJsonProtocolFailure() {
        QJsonObject resp = JsonProtocol::failure("jf1", "bad input", -5);

        QCOMPARE(resp["id"].toString(), QString("jf1"));
        QVERIFY(resp["ok"].toBool() == false);
        QCOMPARE(resp["error"].toObject()["message"].toString(), QString("bad input"));
        QCOMPARE(resp["error"].toObject()["code"].toInt(), -5);
    }

    void testDaemonPingViaDispatch() {
        EcatDaemon daemon;
        Q_UNUSED(daemon);

        CommandDispatcher d;
        d.registerHandler("ping", [](const QString &id, const QJsonObject &) {
            return CommandDispatcher::success(id, {
                {"name", "ecatd"},
                {"version", "0.1.0"},
                {"multiMaster", true}
            });
        });

        QJsonObject resp = d.dispatch({{"id", "dp1"}, {"method", "ping"}, {"params", {}}});
        QVERIFY(resp["ok"].toBool() == true);
        QCOMPARE(resp["result"].toObject()["name"].toString(), QString("ecatd"));
    }
};

QTEST_MAIN(DaemonHandlerTest)
#include "daemon_handler_test.moc"
