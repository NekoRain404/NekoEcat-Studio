// SignalHandlerTest — Tests for SignalHandler
//
// Test coverage:
//   - Channel subscription returns unique IDs
//   - Data push stores samples correctly
//   - Ring buffer enforces 10,000-sample limit
//   - Unsubscribe removes channel
//   - Poll returns correct JSON structure
//   - Poll with since parameter filters samples
//   - Push to nonexistent channel is no-op

#include <QTest>
#include <QJsonObject>
#include <QJsonArray>

#include "handlers/SignalHandler.h"

class SignalHandlerTest : public QObject {
  Q_OBJECT

private slots:
  // Verify each subscription returns a unique channel ID
  void testSubscribeReturnsUniqueIds() {
    SignalHandler handler;
    int id1 = handler.subscribe("ch1", 0, "0x6000", "0x01");
    int id2 = handler.subscribe("ch2", 1, "0x6000", "0x02");
    int id3 = handler.subscribe("ch3", 1, "0x6001", "0x01");

    QVERIFY(id1 != id2);
    QVERIFY(id2 != id3);
    QVERIFY(id1 != id3);
    QCOMPARE(handler.channels().size(), 3);
  }

  // Test push stores samples with correct values and timestamps
  void testPushStoresSamples() {
    SignalHandler handler;
    int id = handler.subscribe("test", 0, "0x6000", "0x01");

    handler.push(id, 1.5, 100);
    handler.push(id, 2.5, 200);
    handler.push(id, 3.5, 300);

    auto chs = handler.channels();
    QCOMPARE(chs.size(), 1);
    QCOMPARE(chs[0].samples.size(), 3);
    QCOMPARE(chs[0].samples[0].value, 1.5);
    QCOMPARE(chs[0].samples[0].timestampMs, static_cast<int64_t>(100));
    QCOMPARE(chs[0].samples[2].value, 3.5);
    QCOMPARE(chs[0].samples[2].timestampMs, static_cast<int64_t>(300));
  }

  // Verify ring buffer evicts oldest samples at 10,000 limit
  void testRingBufferLimit() {
    SignalHandler handler;
    int id = handler.subscribe("stress", 0, "0x6000", "0x01");

    // Push more than the 10,000-sample limit.
    for (int i = 0; i < 10050; ++i) {
      handler.push(id, static_cast<double>(i), i);
    }

    auto chs = handler.channels();
    QCOMPARE(chs.size(), 1);
    QCOMPARE(chs[0].samples.size(), 10000);

    // Oldest samples should have been evicted; the first retained sample is #50.
    QCOMPARE(chs[0].samples[0].value, 50.0);
    QCOMPARE(chs[0].samples[0].timestampMs, static_cast<int64_t>(50));
    QCOMPARE(chs[0].samples.last().value, 10049.0);
  }

  // Test unsubscribe removes channel; unsubscribing unknown ID is no-op
  void testUnsubscribeRemovesChannel() {
    SignalHandler handler;
    int id1 = handler.subscribe("keep", 0, "0x6000", "0x01");
    int id2 = handler.subscribe("remove", 1, "0x6001", "0x01");

    QCOMPARE(handler.channels().size(), 2);

    handler.unsubscribe(id2);
    QCOMPARE(handler.channels().size(), 1);
    QCOMPARE(handler.channels()[0].id, id1);

    // Unsubscribing a non-existent ID should be a no-op.
    handler.unsubscribe(999);
    QCOMPARE(handler.channels().size(), 1);
  }

  // Verify handlePoll returns correct JSON with channel and sample data
  void testHandlePollReturnsCorrectJson() {
    SignalHandler handler;
    int id = handler.subscribe("voltage", 0, "0x6000", "0x01");
    handler.push(id, 3.3, 1000);
    handler.push(id, 3.4, 2000);

    QJsonObject result = handler.handlePoll("req-1", {});
    QVERIFY(result.contains("result"));

    QJsonObject resObj = result["result"].toObject();
    QVERIFY(resObj.contains("channels"));

    QJsonArray channels = resObj["channels"].toArray();
    QCOMPARE(channels.size(), 1);

    QJsonObject ch = channels[0].toObject();
    QCOMPARE(ch["id"].toInt(), id);
    QCOMPARE(ch["name"].toString(), QString("voltage"));

    QJsonArray samples = ch["samples"].toArray();
    QCOMPARE(samples.size(), 2);

    QJsonObject s0 = samples[0].toObject();
    QCOMPARE(s0["value"].toDouble(), 3.3);
    QCOMPARE(static_cast<int64_t>(s0["ts"].toDouble()), static_cast<int64_t>(1000));
  }

  // Test poll with since parameter returns only newer samples
  void testPollWithSinceFiltersSamples() {
    SignalHandler handler;
    int id = handler.subscribe("current", 0, "0x6000", "0x02");
    handler.push(id, 1.0, 100);
    handler.push(id, 2.0, 200);
    handler.push(id, 3.0, 300);
    handler.push(id, 4.0, 400);

    // Only samples with ts > 200 should be returned.
    QJsonObject params;
    params["since"] = 200;

    QJsonObject result = handler.handlePoll("req-2", params);
    QJsonArray channels = result["result"].toObject()["channels"].toArray();
    QCOMPARE(channels.size(), 1);

    QJsonArray samples = channels[0].toObject()["samples"].toArray();
    QCOMPARE(samples.size(), 2);
    QCOMPARE(samples[0].toObject()["value"].toDouble(), 3.0);
    QCOMPARE(samples[1].toObject()["value"].toDouble(), 4.0);
  }

  // Verify push to nonexistent channel does not crash or add data
  void testPushToNonexistentChannelIsNoop() {
    SignalHandler handler;
    handler.subscribe("real", 0, "0x6000", "0x01");

    // Should not crash or add data.
    handler.push(999, 1.0, 100);

    QCOMPARE(handler.channels().size(), 1);
    QCOMPARE(handler.channels()[0].samples.size(), 0);
  }
};

QTEST_MAIN(SignalHandlerTest)
#include "signal_handler_test.moc"
