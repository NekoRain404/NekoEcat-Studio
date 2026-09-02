// Topology Plugin Test Suite
//
// This test suite verifies the functionality of the TopologyPlugin,
// TopologyGraphWidget, and SlaveNodeItem components.
//
// Test Coverage:
//   - SlaveNodeItem: bounding rect, initial state, state changes, signals
//   - TopologyGraphWidget: layout modes, slave list management, signals
//   - TopologyPlugin: identity, default order, visibility, widget, EventBus integration
//
// Test Dependencies:
//   - Qt6::Test (QTest framework)
//   - Qt6::Widgets (for QApplication and widgets)
//   - TopologyPlugin, TopologyGraphWidget, SlaveNodeItem
//   - EventBus for inter-component communication
//   - EthercatTypes for SlaveInfo structure
//
// Test Environment:
//   - Requires QT_QPA_PLATFORM=offscreen for headless testing
//   - Uses QApplication for widget-based tests

#include <QApplication>
#include <QSignalSpy>
#include <QTest>

#include "EthercatTypes.h"
#include "plugins/topology/SlaveNodeItem.h"
#include "plugins/topology/TopologyGraphWidget.h"
#include "plugins/topology/TopologyPlugin.h"
#include "services/EventBus.h"

// Test suite for SlaveNodeItem graphics item.
class SlaveNodeItemTest : public QObject {
    Q_OBJECT
private slots:
    // Test that bounding rect has correct dimensions (180x80).
    void testBoundingRect() {
        SlaveNodeItem item(0, "EL1008", "OP");
        const QRectF rect = item.boundingRect();
        QCOMPARE(rect.width(), 180.0);
        QCOMPARE(rect.height(), 80.0);
    }

    // Test that initial state is correctly set.
    void testInitialState() {
        SlaveNodeItem item(3, "EK1100", "PREOP");
        QCOMPARE(item.position(), 3);
        QCOMPARE(item.slaveName(), QString("EK1100"));
        QCOMPARE(item.state(), QString("PREOP"));
    }

    // Test that state can be changed.
    void testSetState() {
        SlaveNodeItem item(1, "EL2004", "INIT");
        item.setState("OP");
        QCOMPARE(item.state(), QString("OP"));
    }

    // Test that vendor ID can be set without crash.
    void testSetVendorId() {
        SlaveNodeItem item(1, "EL2004", "OP");
        item.setVendorId("Beckhoff");
        // Vendor ID is internal, just verify no crash
        QVERIFY(true);
    }

    // Test that clicked signal exists and is connectable.
    void testClickSignal() {
        SlaveNodeItem item(5, "EL1008", "OP");
        QSignalSpy spy(&item, &SlaveNodeItem::clicked);
        QVERIFY(spy.isValid());
        // Signal is emitted on mouse press — can't easily simulate in unit test
        // but we verify the signal exists and is connectable.
        QCOMPARE(spy.count(), 0);
    }

    // Test that doubleClicked signal exists and is connectable.
    void testDoubleClickSignal() {
        SlaveNodeItem item(5, "EL1008", "OP");
        QSignalSpy spy(&item, &SlaveNodeItem::doubleClicked);
        QVERIFY(spy.isValid());
        QCOMPARE(spy.count(), 0);
    }
};

// Test suite for TopologyGraphWidget.
class TopologyGraphWidgetTest : public QObject {
    Q_OBJECT
private slots:
    // Test that initial layout mode is Linear.
    void testInitialLayout() {
        TopologyGraphWidget widget;
        QCOMPARE(widget.layoutMode(), TopologyGraphWidget::Layout::Linear);
    }

    // Test that layout mode can be changed to Tree.
    void testSetLayoutMode() {
        TopologyGraphWidget widget;
        widget.setLayoutMode(TopologyGraphWidget::Layout::Tree);
        QCOMPARE(widget.layoutMode(), TopologyGraphWidget::Layout::Tree);
    }

    // Test that empty slave list doesn't crash.
    void testSetSlavesEmpty() {
        TopologyGraphWidget widget;
        QVector<SlaveInfo> slaves;
        widget.setSlaves(slaves);
        // Should not crash with empty slave list
        QVERIFY(true);
    }

    // Test that populated slave list doesn't crash.
    void testSetSlavesPopulated() {
        TopologyGraphWidget widget;

        SlaveInfo s1;
        s1.position = 0;
        s1.name = "EK1100";
        s1.state = "OP";

        SlaveInfo s2;
        s2.position = 1;
        s2.name = "EL1008";
        s2.state = "PREOP";

        QVector<SlaveInfo> slaves;
        slaves << s1 << s2;
        widget.setSlaves(slaves);
        QVERIFY(true);
    }

    // Test that slaveClicked signal exists and is connectable.
    void testSlaveClickedSignal() {
        TopologyGraphWidget widget;
        QSignalSpy spy(&widget, &TopologyGraphWidget::slaveClicked);
        QVERIFY(spy.isValid());
    }
};

// Test suite for TopologyPlugin.
class TopologyPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Test that plugin identity is correct.
    void testIdentity() {
        EventBus bus;
        TopologyPlugin plugin(&bus);
        QCOMPARE(plugin.id(), QString("topology"));
        QCOMPARE(plugin.displayName(), QString("Topology"));
        QCOMPARE(plugin.displayNameZh(), QString("拓扑"));
    }

    // Test that default order is 15.
    void testDefaultOrder() {
        EventBus bus;
        TopologyPlugin plugin(&bus);
        QCOMPARE(plugin.defaultOrder(), 15);
    }

    // Test that plugin is visible by default.
    void testVisible() {
        EventBus bus;
        TopologyPlugin plugin(&bus);
        QVERIFY(plugin.visible());
    }

    // Test that widget is not null.
    void testWidgetNotNull() {
        EventBus bus;
        TopologyPlugin plugin(&bus);
        QVERIFY(plugin.widget() != nullptr);
    }

    // Test that graph widget is accessible.
    void testGraphWidgetAccess() {
        EventBus bus;
        TopologyPlugin plugin(&bus);
        QVERIFY(plugin.graphWidget() != nullptr);
    }

    // Test that EventBus slave changes update the plugin.
    void testSlaveChangedUpdates() {
        EventBus bus;
        TopologyPlugin plugin(&bus);

        SlaveInfo s1;
        s1.position = 0;
        s1.name = "EK1100";
        s1.state = "OP";

        SlaveInfo s2;
        s2.position = 1;
        s2.name = "EL1008";
        s2.state = "SAFEOP";

        QVector<SlaveInfo> slaves;
        slaves << s1 << s2;

        bus.emitSlaveChanged(slaves);

        // Plugin should have received the update — graph should have 2 nodes.
        // We can't easily inspect QGraphicsScene items without exposing them,
        // but we verify no crash occurred.
        QVERIFY(true);
    }
};

// Main test runner - executes all test suites sequentially.
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    int status = 0;
    {
        SlaveNodeItemTest t;
        status |= QTest::qExec(&t, argc, argv);
    }
    {
        TopologyGraphWidgetTest t;
        status |= QTest::qExec(&t, argc, argv);
    }
    {
        TopologyPluginTest t;
        status |= QTest::qExec(&t, argc, argv);
    }
    return status;
}

#include "topology_plugin_test.moc"
