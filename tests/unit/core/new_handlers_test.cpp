// Tests for EoE, Redundancy, and OnlineChange daemon handlers.
//
// These tests exercise handler logic that does not require live EtherCAT
// hardware: parameter validation, error envelopes for missing params, and
// the online-change preview computation (affected slaves, operation count).
// Operations that require a backend return failure envelopes, which we assert.

#include <QJsonArray>
#include <QJsonObject>
#include <QTest>

#include "handlers/EoEHandler.h"
#include "handlers/OnlineChangeHandler.h"
#include "handlers/RedundancyHandler.h"

class NewHandlersTest : public QObject {
    Q_OBJECT
private slots:
    // ── EoE ──────────────────────────────────────────────────────────────

    // Missing position must yield a failure envelope.
    void testEoeStatusMissingPosition() {
        EoEHandler h(nullptr);
        QJsonObject resp = h.handleEoeStatus("1", QJsonObject{});
        QVERIFY(!resp.value("ok").toBool());
    }

    // ConfigureIp with invalid IP must fail before touching the backend.
    void testEoeConfigureInvalidIp() {
        EoEHandler h(nullptr);
        QJsonObject params{{"position", 0}, {"ip", "999.1.1.1"}, {"subnet", "255.255.255.0"}};
        QJsonObject resp = h.handleEoeConfigureIp("1", params);
        QVERIFY(!resp.value("ok").toBool());
    }

    // ConfigureIp with missing subnet must fail.
    void testEoeConfigureMissingSubnet() {
        EoEHandler h(nullptr);
        QJsonObject params{{"position", 0}, {"ip", "192.168.1.10"}};
        QJsonObject resp = h.handleEoeConfigureIp("1", params);
        QVERIFY(!resp.value("ok").toBool());
    }

    // ── Redundancy ───────────────────────────────────────────────────────

    // History query returns a valid envelope even with no backend.
    void testRedundancyHistoryEmpty() {
        RedundancyHandler h(nullptr);
        QJsonObject resp = h.handleHistory("1", QJsonObject{});
        QVERIFY(resp.value("ok").toBool());
        QVERIFY(resp.value("result").toObject().contains("events"));
    }

    // Disable is unsupported at runtime; must report failure.
    void testRedundancyDisableUnsupported() {
        RedundancyHandler h(nullptr);
        QJsonObject resp = h.handleDisable("1", QJsonObject{});
        QVERIFY(!resp.value("ok").toBool());
    }

    // ── Online Change ────────────────────────────────────────────────────

    // Preview with no changes must fail.
    void testOnlineChangePreviewEmpty() {
        OnlineChangeHandler h(nullptr);
        QJsonObject resp = h.handlePreview("1", QJsonObject{});
        QVERIFY(!resp.value("ok").toBool());
    }

    // Preview computes affected slaves and operation count correctly.
    void testOnlineChangePreviewAffectedSlaves() {
        OnlineChangeHandler h(nullptr);
        QJsonArray changes;
        changes.append(
            QJsonObject{{"position", 2}, {"index", "0x6000"}, {"subIndex", "0x01"}, {"value", "1"}, {"type", "uint8"}});
        changes.append(
            QJsonObject{{"position", 0}, {"index", "0x6001"}, {"subIndex", "0x00"}, {"value", "2"}, {"type", "uint8"}});
        changes.append(
            QJsonObject{{"position", 2}, {"index", "0x6002"}, {"subIndex", "0x00"}, {"value", "3"}, {"type", "uint8"}});
        QJsonObject resp = h.handlePreview("1", QJsonObject{{"changes", changes}});
        QVERIFY(resp.value("ok").toBool());
        const QJsonObject result = resp.value("result").toObject();
        // Two unique slaves (0 and 2), sorted ascending.
        const QJsonArray affected = result.value("affectedSlaves").toArray();
        QCOMPARE(affected.size(), 2);
        QCOMPARE(affected.at(0).toInt(), 0);
        QCOMPARE(affected.at(1).toInt(), 2);
        // Three operations total.
        QCOMPARE(result.value("operationCount").toInt(), 3);
        // Downtime estimate scales with affected slave count.
        QCOMPARE(result.value("estimatedDowntimeMs").toInt(), 2 * 300);
    }

    // Apply with no backend must fail gracefully.
    void testOnlineChangeApplyNoBackend() {
        OnlineChangeHandler h(nullptr);
        QJsonArray changes;
        changes.append(
            QJsonObject{{"position", 0}, {"index", "0x6000"}, {"subIndex", "0x00"}, {"value", "1"}, {"type", "uint8"}});
        QJsonObject resp = h.handleApply("1", QJsonObject{{"changes", changes}});
        QVERIFY(!resp.value("ok").toBool());
    }

    // Status query returns idle when no change is running.
    void testOnlineChangeStatusIdle() {
        OnlineChangeHandler h(nullptr);
        QJsonObject resp = h.handleStatus("1", QJsonObject{});
        QVERIFY(resp.value("ok").toBool());
        const QJsonObject result = resp.value("result").toObject();
        QCOMPARE(result.value("inProgress").toBool(), false);
        QCOMPARE(result.value("phase").toString(), QString("idle"));
    }
};

QTEST_MAIN(NewHandlersTest)
#include "new_handlers_test.moc"
