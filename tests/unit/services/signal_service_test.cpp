// SignalServiceTest / SignalPluginTest — Tests for Signal Service and Plugin
//
// Test coverage:
//   - Channel add/remove with unique IDs
//   - Channel metadata storage
//   - Signal emissions (channelAdded, channelRemoved, channelDataUpdated)
//   - Data push and statistics computation (min, max, avg, stddev)
//   - EventBus integration for data push
//   - Max points limit enforcement
//   - Polling start/stop
//   - Plugin identity, order, visibility, and widget
#include <QApplication>
#include <QSignalSpy>
#include <QTest>

#include "plugins/signal/SignalPlugin.h"
#include "services/EventBus.h"
#include "services/SignalService.h"

// ── SignalService tests ──────────────────────────────────────────────

class SignalServiceTest : public QObject {
    Q_OBJECT
private:
    EventBus* bus_ = nullptr;
    SignalService* svc_ = nullptr;

private slots:
    // Initialize EventBus and SignalService
    void init() {
        bus_ = new EventBus(this);
        svc_ = new SignalService(bus_, this);
    }

    // Clean up EventBus and SignalService
    void cleanup() {
        delete svc_;
        svc_ = nullptr;
        delete bus_;
        bus_ = nullptr;
    }

    // Verify addChannel returns positive unique IDs
    void testAddChannelReturnsId() {
        const int id1 = svc_->addChannel("ch1", 0, "0x6064", "0");
        const int id2 = svc_->addChannel("ch2", 1, "0x6041", "0");
        QVERIFY(id1 > 0);
        QVERIFY(id2 > 0);
        QVERIFY(id1 != id2);
    }

    // Verify channel metadata (name, slave, index, subIndex) is stored
    void testChannelMetadata() {
        svc_->addChannel("actual_pos", 3, "0x6064", "0x01");
        const auto chs = svc_->channels();
        QCOMPARE(chs.size(), 1);
        QCOMPARE(chs[0].name, QString("actual_pos"));
        QCOMPARE(chs[0].slave, 3);
        QCOMPARE(chs[0].index, QString("0x6064"));
        QCOMPARE(chs[0].subIndex, QString("0x01"));
    }

    // Test removeChannel deletes the correct entry
    void testRemoveChannel() {
        const int id1 = svc_->addChannel("a", 0, "0x6064", "0");
        const int id2 = svc_->addChannel("b", 1, "0x6041", "0");
        svc_->removeChannel(id1);
        const auto chs = svc_->channels();
        QCOMPARE(chs.size(), 1);
        QCOMPARE(chs[0].id, id2);
    }

    // Verify removing a nonexistent channel is a no-op
    void testRemoveNonexistent() {
        svc_->addChannel("a", 0, "0x6064", "0");
        svc_->removeChannel(999);
        QCOMPARE(svc_->channels().size(), 1);
    }

    // Verify channelAdded signal fires on addChannel
    void testChannelAddedSignal() {
        QSignalSpy spy(svc_, &SignalService::channelAdded);
        svc_->addChannel("ch", 0, "0x6064", "0");
        QCOMPARE(spy.count(), 1);
    }

    // Verify channelRemoved signal fires with correct ID
    void testChannelRemovedSignal() {
        const int id = svc_->addChannel("ch", 0, "0x6064", "0");
        QSignalSpy spy(svc_, &SignalService::channelRemoved);
        svc_->removeChannel(id);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), id);
    }

    // Test pushData appends values and emits channelDataUpdated
    void testPushData() {
        const int id = svc_->addChannel("ch", 0, "0x6064", "0");
        QSignalSpy spy(svc_, &SignalService::channelDataUpdated);

        QVector<double> vals = {1.0, 2.0, 3.0};
        QVector<qint64> ts = {100, 200, 300};
        svc_->pushData(id, vals, ts);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(svc_->channels()[0].values.size(), 3);
        QCOMPARE(svc_->channels()[0].timestamps.size(), 3);
    }

    // Verify pushData to nonexistent channel is a no-op
    void testPushDataNonexistent() {
        QSignalSpy spy(svc_, &SignalService::channelDataUpdated);
        svc_->pushData(999, {1.0}, {100});
        QCOMPARE(spy.count(), 0);
    }

    // Test statistics computation (min, max, avg, stddev)
    void testStatsComputation() {
        const int id = svc_->addChannel("ch", 0, "0x6064", "0");
        QVector<double> vals = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
        QVector<qint64> ts(vals.size(), 0);
        svc_->pushData(id, vals, ts);

        const ChannelStats s = svc_->stats(id);
        QCOMPARE(s.min, 2.0);
        QCOMPARE(s.max, 9.0);
        QCOMPARE(s.avg, 5.0);
        // stddev = sqrt(((2-5)^2 + (4-5)^2*3 + (5-5)^2*2 + (7-5)^2 + (9-5)^2) / 8)
        //       = sqrt((9 + 3 + 0 + 4 + 16) / 8) = sqrt(4) = 2.0
        QCOMPARE(s.stddev, 2.0);
    }

    // Verify stats return zeroed for an empty channel
    void testStatsEmpty() {
        const int id = svc_->addChannel("ch", 0, "0x6064", "0");
        const ChannelStats s = svc_->stats(id);
        QCOMPARE(s.min, 0.0);
        QCOMPARE(s.max, 0.0);
        QCOMPARE(s.avg, 0.0);
        QCOMPARE(s.stddev, 0.0);
    }

    // Test data push via EventBus signal works
    void testEventBusIntegration() {
        const int id = svc_->addChannel("ch", 0, "0x6064", "0");
        QSignalSpy spy(svc_, &SignalService::channelDataUpdated);

        QVector<double> vals = {10.0, 20.0};
        QVector<qint64> ts = {1000, 2000};
        emit bus_->signalData(id, vals, ts);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(svc_->channels()[0].values.size(), 2);
        QCOMPARE(svc_->channels()[0].values[0], 10.0);
        QCOMPARE(svc_->channels()[0].values[1], 20.0);
    }

    // Verify data respects kMaxPoints limit (oldest dropped)
    void testMaxPointsLimit() {
        const int id = svc_->addChannel("ch", 0, "0x6064", "0");

        // Push exactly kMaxPoints.
        QVector<double> vals(SignalChannelInfo::kMaxPoints, 1.0);
        QVector<qint64> ts(SignalChannelInfo::kMaxPoints, 0);
        svc_->pushData(id, vals, ts);
        QCOMPARE(svc_->channels()[0].values.size(), SignalChannelInfo::kMaxPoints);

        // Push one more — should still be kMaxPoints (oldest dropped).
        svc_->pushData(id, {99.0}, {50000});
        QCOMPARE(svc_->channels()[0].values.size(), SignalChannelInfo::kMaxPoints);
        QCOMPARE(svc_->channels()[0].values.last(), 99.0);
    }

    // Verify polling start/stop does not crash
    void testPollingStartStop() {
        svc_->startPolling(50);
        QTest::qWait(120);
        svc_->stopPolling();
        // No crash is the assertion here.
        QVERIFY(true);
    }
};

// ── SignalPlugin tests ───────────────────────────────────────────────

class SignalPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify SignalPlugin identity strings
    void testIdentity() {
        EventBus bus;
        SignalService svc(&bus);
        SignalPlugin plugin(&svc);
        QCOMPARE(plugin.id(), QString("signal"));
        QCOMPARE(plugin.displayName(), QString("Signal Analyzer"));
        QCOMPARE(plugin.displayNameZh(), QString("信号分析"));
    }

    // Verify SignalPlugin default order
    void testDefaultOrder() {
        EventBus bus;
        SignalService svc(&bus);
        SignalPlugin plugin(&svc);
        QCOMPARE(plugin.defaultOrder(), 70);
    }

    // Verify SignalPlugin is visible
    void testVisible() {
        EventBus bus;
        SignalService svc(&bus);
        SignalPlugin plugin(&svc);
        QVERIFY(plugin.visible());
    }

    // Verify SignalPlugin widget is created
    void testWidgetNotNull() {
        EventBus bus;
        SignalService svc(&bus);
        SignalPlugin plugin(&svc);
        QVERIFY(plugin.widget() != nullptr);
    }
};

// ── Combined runner ──────────────────────────────────────────────────

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    int status = 0;
    {
        SignalServiceTest t;
        status |= QTest::qExec(&t, argc, argv);
    }
    {
        SignalPluginTest t;
        status |= QTest::qExec(&t, argc, argv);
    }
    return status;
}

#include "signal_service_test.moc"
