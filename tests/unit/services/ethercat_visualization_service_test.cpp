// EtherCATVisualizationServiceTest — Tests for EtherCATVisualizationService
//
// Test coverage:
//   - Default and custom visualization config
//   - Topology view creation (empty + with slaves)
//   - Data view creation (empty + with data points)
//   - Performance and error view creation
//   - Signal emission and color configuration

#include "EthercatTypes.h"
#include "services/EtherCATAnalyticsService.h"
#include "services/EtherCATMonitorService.h"
#include "services/EtherCATVisualizationService.h"
#include "services/NetworkDiagnosticsService.h"
#include <QGraphicsScene>
#include <QSignalSpy>
#include <QTest>

class EtherCATVisualizationServiceTest : public QObject {
    Q_OBJECT
private slots:
    // Verify default visualization config values
    // Default config has default view type
    void testDefaultConfig() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        auto cfg = svc.config();
        QCOMPARE(cfg.viewType, QStringLiteral("default"));
        QVERIFY(!cfg.animations);
        QVERIFY(cfg.interactions);
    }

    // Set custom config and verify all fields
    // Set custom visualization config
    void testSetConfig() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        VisualizationConfig cfg;
        cfg.viewType = QStringLiteral("custom");
        cfg.layout = QStringLiteral("vertical");
        cfg.animations = true;
        svc.setConfig(cfg);
        QCOMPARE(svc.config().viewType, QStringLiteral("custom"));
        QCOMPARE(svc.config().layout, QStringLiteral("vertical"));
        QVERIFY(svc.config().animations);
    }

    // Create topology view with no slaves
    // Create topology view with no slaves
    void testCreateTopologyViewEmpty() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        QVector<SlaveInfo> slaves;
        auto* scene = svc.createTopologyView(slaves);
        QVERIFY(scene != nullptr);
        QVERIFY(scene->items().size() >= 2);
        delete scene;
    }

    // Create topology view with slaves and verify items
    // Create topology view with slave nodes
    void testCreateTopologyViewWithSlaves() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        QVector<SlaveInfo> slaves;
        SlaveInfo s1;
        s1.position = 0;
        s1.name = QStringLiteral("Slave 0");
        slaves << s1;
        SlaveInfo s2;
        s2.position = 1;
        s2.name = QStringLiteral("Slave 1");
        slaves << s2;
        auto* scene = svc.createTopologyView(slaves);
        QVERIFY(scene != nullptr);
        QVERIFY(scene->items().size() >= 6);
        delete scene;
    }

    // Create data view with no data points
    // Create data view with no data points
    void testCreateDataViewEmpty() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        QVector<DataPoint> data;
        auto* scene = svc.createDataView(data);
        QVERIFY(scene != nullptr);
        delete scene;
    }

    // Create data view with data points and verify items
    // Create data view with data points
    void testCreateDataViewWithData() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        QVector<DataPoint> data;
        for (int i = 0; i < 5; ++i) {
            DataPoint dp;
            dp.value = 10.0 + i * 5;
            data << dp;
        }
        auto* scene = svc.createDataView(data);
        QVERIFY(scene != nullptr);
        QVERIFY(scene->items().size() >= 5);
        delete scene;
    }

    // Create performance view with metrics
    // Create performance metrics view
    void testCreatePerformanceView() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        PerformanceMetrics metrics;
        metrics.cycleTimeUs = 500.0;
        metrics.jitterUs = 0.5;
        metrics.frameLossRate = 0.01;
        metrics.sdoResponseMs = 2.0;
        metrics.pdoUpdateRate = 1000.0;
        auto* scene = svc.createPerformanceView(metrics);
        QVERIFY(scene != nullptr);
        QVERIFY(scene->items().size() >= 10);
        delete scene;
    }

    // Create error view with no errors
    // Create error view with no errors
    void testCreateErrorViewEmpty() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        QVector<ErrorInfo> errors;
        auto* scene = svc.createErrorView(errors);
        QVERIFY(scene != nullptr);
        delete scene;
    }

    // Create error view with errors and verify items
    // Create error view with error entries
    void testCreateErrorViewWithErrors() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        QVector<ErrorInfo> errors;
        ErrorInfo e1;
        e1.timestampMs = 1000;
        e1.port = 0;
        e1.type = QStringLiteral("CRC");
        e1.description = QStringLiteral("CRC error detected");
        errors << e1;
        auto* scene = svc.createErrorView(errors);
        QVERIFY(scene != nullptr);
        QVERIFY(scene->items().size() >= 2);
        delete scene;
    }

    // Verify viewCreated signal with correct view type
    // viewCreated signal carries view type
    void testViewCreatedSignal() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        QSignalSpy spy(&svc, &EtherCATVisualizationService::viewCreated);
        QVector<SlaveInfo> slaves;
        svc.createTopologyView(slaves);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("topology"));
    }

    // Set and verify custom color configuration
    // Custom color configuration persists
    void testColorsConfig() {
        EtherCATVisualizationService svc(nullptr, nullptr);
        VisualizationConfig cfg;
        cfg.colors[QStringLiteral("master")] = Qt::red;
        cfg.colors[QStringLiteral("slave")] = Qt::blue;
        svc.setConfig(cfg);
        QCOMPARE(svc.config().colors.value(QStringLiteral("master")), Qt::red);
    }
};

QTEST_MAIN(EtherCATVisualizationServiceTest)
#include "ethercat_visualization_service_test.moc"
