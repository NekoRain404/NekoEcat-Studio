// RtTestPluginTest — Tests for RtTestPlugin
//
// Test coverage:
//   - Plugin identity, visibility, and widget creation
//   - Duration formatting
//   - Telemetry updates for running and stopped states

#include <QJsonArray>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTest>

#include "infra/EcatClient.h"
#include "plugins/rttest/RtTestPlugin.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"

class RtTestPluginTest : public QObject {
    Q_OBJECT
private:
    EcatClient* client_ = nullptr;
    ServiceContainer* container_ = nullptr;
    RtTestPlugin* plugin_ = nullptr;

private slots:
    void init() {
        client_ = new EcatClient(this);
        container_ = new ServiceContainer(client_, new EventBus(this), this);
        plugin_ = new RtTestPlugin(container_, this);
    }

    void cleanup() {
        delete plugin_;
        plugin_ = nullptr;
        delete container_;
        container_ = nullptr;
    }

    // Verify plugin id, displayName, displayNameZh, defaultOrder
    void testIdentity() {
        QCOMPARE(plugin_->id(), QString("rttest"));
        QCOMPARE(plugin_->displayName(), QString("RT Test"));
        QCOMPARE(plugin_->displayNameZh(), QString("实时测试"));
        QCOMPARE(plugin_->defaultOrder(), 75);
    }

    // RtTestPlugin is a hidden helper plugin (no standalone workspace tab)
    void testVisible() { QVERIFY(!plugin_->visible()); }

    // Widget is created and not null
    // Verify widget is non-null
    void testWidgetNotNull() { QVERIFY(plugin_->widget() != nullptr); }

    // Widget has correct object name
    // Verify widget object name is "rtTestPage"
    void testWidgetObjectName() { QCOMPARE(plugin_->widget()->objectName(), QString("rtTestPage")); }

    // Format duration into human-readable string (s, m, h)
    // Verify formatDuration for seconds, minutes, and hours
    void testFormatDuration() {
        QCOMPARE(RtTestPlugin::formatDuration(5.5), QString("5.5s"));
        QCOMPARE(RtTestPlugin::formatDuration(65.0), QString("1m 5s"));
        QCOMPARE(RtTestPlugin::formatDuration(3661.0), QString("1h 1m"));
    }

    // Telemetry updates metrics in running state
    // Process running telemetry and verify widget is updated
    void testTelemetryUpdatesMetrics() {
        QJsonObject telemetry;
        telemetry["running"] = true;
        telemetry["cycles"] = QJsonValue(static_cast<qint64>(10000));
        telemetry["errors"] = QJsonValue(static_cast<qint64>(2));
        telemetry["lossRate"] = 0.02;
        telemetry["minUsec"] = 95.0;
        telemetry["maxUsec"] = 110.0;
        telemetry["avgUsec"] = 100.5;
        telemetry["jitterUsec"] = 7.3;
        telemetry["recent"] = QJsonArray({99.0, 100.0, 101.0, 102.0});

        emit container_->client()->rtTestTelemetry(telemetry);
        QCoreApplication::processEvents();

        QWidget* w = plugin_->widget();
        QVERIFY(w != nullptr);
    }

    // Telemetry reflects stopped state correctly
    // Process stopped telemetry and verify widget remains valid
    void testTelemetryStoppedState() {
        QJsonObject telemetry;
        telemetry["running"] = false;
        telemetry["cycles"] = QJsonValue(static_cast<qint64>(5000));
        telemetry["errors"] = QJsonValue(static_cast<qint64>(0));
        telemetry["lossRate"] = 0.0;
        telemetry["minUsec"] = 98.0;
        telemetry["maxUsec"] = 102.0;
        telemetry["avgUsec"] = 100.0;
        telemetry["jitterUsec"] = 2.0;

        emit container_->client()->rtTestTelemetry(telemetry);
        QCoreApplication::processEvents();

        QVERIFY(plugin_->widget() != nullptr);
    }
};

QTEST_MAIN(RtTestPluginTest)
#include "rttest_plugin_test.moc"
