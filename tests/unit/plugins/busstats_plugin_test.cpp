/// @brief BusStatsPlugin unit tests.
///
/// @details Tests the Bus Statistics workspace plugin's service, identity,
/// UI construction, and accessor methods. Verifies that the plugin correctly
/// implements the WorkspacePlugin interface and provides the expected
/// widget hierarchy.
///
/// @par Test Coverage
///   - Service default state with zero counters
///   - Service offline start does not synthesize active monitoring
///   - Stats JSON structure validation
///   - Plugin identity (id, displayName, displayNameZh, defaultOrder, visible)
///   - Widget construction and non-null checks
///   - Stats table accessor
///   - Service accessor
///
/// @par Test Dependencies
///   - Qt6::Test (QTest framework)
///   - Qt6::Widgets (for QTableWidget)
///   - BusStatsPlugin, BusStatsService, EcatClient
///
/// @par Test Environment
///   - Requires QT_QPA_PLATFORM=offscreen for headless execution
///   - Creates an EcatClient and BusStatsService per test (no daemon required)

#include "infra/EcatClient.h"
#include "plugins/busstats/BusStatsPlugin.h"
#include "services/BusStatsService.h"
#include <QTableWidget>
#include <QTest>

/// @brief Test suite for BusStatsPlugin and BusStatsService.
class BusStatsPluginTest : public QObject {
    Q_OBJECT
private slots:
    /// @brief Verifies service default state with zero counters.
    /// @details Checks that all statistics counters are initialized to zero
    /// and monitoring is disabled by default.
    void testServiceDefaultState() {
        EcatClient client;
        BusStatsService svc(&client);
        QVERIFY(!svc.isMonitoring());
        auto stats = svc.currentStats();
        QCOMPARE(stats.txFrames, quint64(0));
        QCOMPARE(stats.rxFrames, quint64(0));
        QCOMPARE(stats.txErrors, quint64(0));
        QCOMPARE(stats.rxErrors, quint64(0));
        QCOMPARE(stats.crcErrors, quint64(0));
        QCOMPARE(stats.lostFrames, quint64(0));
    }

    /// @brief Verifies offline start does not create a monitoring session.
    void testServiceStartMonitoringOfflineDoesNotActivate() {
        EcatClient client;
        BusStatsService svc(&client);
        svc.startMonitoring(100);
        QVERIFY(!svc.isMonitoring());
        svc.stopMonitoring();
        QVERIFY(!svc.isMonitoring());
    }

    // Verify stats JSON contains all required fields
    void testServiceStatsJson() {
        EcatClient client;
        BusStatsService svc(&client);
        QJsonObject json = svc.currentStatsJson();
        QVERIFY(json.contains("txFrames"));
        QVERIFY(json.contains("rxFrames"));
        QVERIFY(json.contains("txErrors"));
        QVERIFY(json.contains("rxErrors"));
        QVERIFY(json.contains("crcErrors"));
        QVERIFY(json.contains("lostFrames"));
        QVERIFY(json.contains("bandwidthMbps"));
        QVERIFY(json.contains("frameRate"));
        QVERIFY(json.contains("timestampMs"));
    }

    // Verify plugin identity metadata
    void testPluginIdentity() {
        EcatClient client;
        BusStatsService svc(&client);
        BusStatsPlugin plugin(&svc);
        QCOMPARE(plugin.id(), QString("busstats"));
        QCOMPARE(plugin.displayName(), QString("Bus Statistics"));
        QCOMPARE(plugin.displayNameZh(), QStringLiteral("总线统计"));
    }

    // Verify plugin default order
    void testPluginDefaultOrder() {
        EcatClient client;
        BusStatsService svc(&client);
        BusStatsPlugin plugin(&svc);
        QCOMPARE(plugin.defaultOrder(), 95);
    }

    /// @brief Verifies plugin is visible.
    void testPluginVisible() {
        EcatClient client;
        BusStatsService svc(&client);
        BusStatsPlugin plugin(&svc);
        QVERIFY(plugin.visible());
    }

    /// @brief Verifies widget is not null.
    void testPluginWidgetNotNull() {
        EcatClient client;
        BusStatsService svc(&client);
        BusStatsPlugin plugin(&svc);
        QVERIFY(plugin.widget() != nullptr);
    }

    /// @brief Verifies service accessor returns correct pointer.
    void testPluginServiceAccessor() {
        EcatClient client;
        BusStatsService svc(&client);
        BusStatsPlugin plugin(&svc);
        QCOMPARE(plugin.service(), &svc);
    }

    /// @brief Verifies stats table widget exists.
    void testPluginStatsTableNotNull() {
        EcatClient client;
        BusStatsService svc(&client);
        BusStatsPlugin plugin(&svc);
        QVERIFY(plugin.statsTable() != nullptr);
    }

    /// @brief Verifies stats table has correct dimensions.
    /// @details Checks that the table has 8 rows (for each metric) and 2 columns
    /// (metric name and value).
    void testPluginStatsTableMetrics() {
        EcatClient client;
        BusStatsService svc(&client);
        BusStatsPlugin plugin(&svc);
        auto* table = plugin.statsTable();
        QCOMPARE(table->rowCount(), 8);
        QCOMPARE(table->columnCount(), 2);
    }
};

QTEST_MAIN(BusStatsPluginTest)
#include "busstats_plugin_test.moc"
