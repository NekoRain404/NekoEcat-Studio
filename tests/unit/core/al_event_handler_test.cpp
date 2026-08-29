// AlEventHandlerTest — Tests for AlEventHandler
//
// Test coverage:
//   - Empty event list handling
//   - Adding and retrieving AL events
//   - Clearing event history
//   - Limit parameter filtering

#include <QTest>
#include <QJsonObject>
#include <QJsonArray>

#include "../../../apps/ecatd/handlers/AlEventHandler.h"

class AlEventHandlerTest : public QObject {
    Q_OBJECT

private slots:
    // Verify handle returns an ok envelope with empty events array when none added
    void testHandleEmptyEvents() {
        AlEventHandler handler;
        QJsonObject result = handler.handle("1", {});
        QVERIFY(result.value("ok").toBool());
        QJsonObject inner = result.value("result").toObject();
        QVERIFY(inner.contains("events"));
        QVERIFY(inner.contains("total"));
        QCOMPARE(inner["total"].toInt(), 0);
        QCOMPARE(inner["events"].toArray().size(), 0);
    }

    // Verify handle returns correct event data after adding events
    void testHandleWithEvents() {
        AlEventHandler handler;

        AlEventEntry e1;
        e1.timestampMs = 1000;
        e1.slave = 0;
        e1.slaveName = "EL1008";
        e1.alStatusCode = "INIT";
        e1.description = "Slave 0 in INIT state";
        e1.severity = "Error";
        handler.addEvent(e1);

        AlEventEntry e2;
        e2.timestampMs = 2000;
        e2.slave = 1;
        e2.slaveName = "EL2004";
        e2.alStatusCode = "SAFEOP+ERROR";
        e2.description = "Slave 1 in SAFEOP+ERROR state";
        e2.severity = "Warning";
        handler.addEvent(e2);

        QJsonObject result = handler.handle("2", {});
        QVERIFY(result.value("ok").toBool());
        QJsonObject inner = result.value("result").toObject();
        QCOMPARE(inner["total"].toInt(), 2);
        QJsonArray events = inner["events"].toArray();
        QCOMPARE(events.size(), 2);

        QJsonObject first = events[0].toObject();
        QCOMPARE(first["slave"].toInt(), 0);
        QCOMPARE(first["slaveName"].toString(), QString("EL1008"));
        QCOMPARE(first["code"].toString(), QString("INIT"));
        QCOMPARE(first["severity"].toString(), QString("Error"));
    }

    // Verify clear resets event history
    void testClear() {
        AlEventHandler handler;

        AlEventEntry entry;
        entry.timestampMs = 5000;
        entry.slave = 2;
        entry.slaveName = "EL3000";
        entry.alStatusCode = "INIT";
        entry.description = "Slave 2 in INIT state";
        entry.severity = "Error";
        handler.addEvent(entry);

        QCOMPARE(handler.handle("3", {})["result"].toObject()["total"].toInt(), 1);

        handler.clear();

        QJsonObject result = handler.handle("4", {});
        QJsonObject inner = result.value("result").toObject();
        QCOMPARE(inner["total"].toInt(), 0);
        QCOMPARE(inner["events"].toArray().size(), 0);
    }

    // Verify limit parameter restricts returned event count
    void testLimitParam() {
        AlEventHandler handler;

        for (int i = 0; i < 10; ++i) {
            AlEventEntry entry;
            entry.timestampMs = i * 1000;
            entry.slave = i;
            entry.alStatusCode = "INIT";
            entry.description = QStringLiteral("Slave %1 in INIT state").arg(i);
            entry.severity = "Error";
            handler.addEvent(entry);
        }

        QJsonObject result = handler.handle("5", {{"limit", 3}});
        QJsonObject inner = result.value("result").toObject();
        QCOMPARE(inner["total"].toInt(), 10);
        QJsonArray events = inner["events"].toArray();
        QCOMPARE(events.size(), 3);
        // The last 3 events should be returned (slave 7, 8, 9).
        QCOMPARE(events[0].toObject()["slave"].toInt(), 7);
        QCOMPARE(events[2].toObject()["slave"].toInt(), 9);
    }

    void testPollWithoutCliReturnsEmpty() {
        // poll() calls `ethercat slaves` which is not available in a test environment;
        // it should silently produce no events.
        AlEventHandler handler;
        handler.poll();
        QJsonObject result = handler.handle("6", {});
        QCOMPARE(result["result"].toObject()["total"].toInt(), 0);
    }
};

QTEST_MAIN(AlEventHandlerTest)
#include "al_event_handler_test.moc"
