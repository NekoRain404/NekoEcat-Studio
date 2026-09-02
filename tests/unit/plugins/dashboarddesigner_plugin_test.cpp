// DashboardDesignerPluginTest — Tests for DashboardDesignerPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget palette and property editor
//   - Add/remove/select widgets
//   - Preview mode toggling
//   - Export/import configuration

#include "plugins/dashboarddesigner/DashboardDesignerPlugin.h"
#include <QLabel>
#include <QListWidget>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTabWidget>
#include <QTest>

class DashboardDesignerPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin id, display names, order, and visibility
    void testPluginIdentity() {
        DashboardDesignerPlugin plugin;

        QCOMPARE(plugin.id(), QString("dashboarddesigner"));
        QCOMPARE(plugin.displayName(), QString("Dashboard Designer"));
        QCOMPARE(plugin.displayNameZh(), QString("仪表盘设计器"));
        QCOMPARE(plugin.defaultOrder(), 235);
        QCOMPARE(plugin.visible(), false);
    }

    // Verify main widget is created
    void testWidgetCreation() {
        DashboardDesignerPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify initial state has no widgets and no selection
    void testInitialState() {
        DashboardDesignerPlugin plugin;

        QCOMPARE(plugin.widgetCount(), 0);
        QCOMPARE(plugin.selectedWidget(), -1);
        QCOMPARE(plugin.isPreviewMode(), false);
    }

    // Verify adding a widget increments count
    void testAddWidget() {
        DashboardDesignerPlugin plugin;

        plugin.addWidget("gauge");
        QCOMPARE(plugin.widgetCount(), 1);
    }

    // Verify removing a widget decrements count
    void testRemoveWidget() {
        DashboardDesignerPlugin plugin;

        plugin.addWidget("gauge");
        QCOMPARE(plugin.widgetCount(), 1);

        plugin.removeWidget(0);
        QCOMPARE(plugin.widgetCount(), 0);
    }

    // Verify widget selection emits widgetSelected signal
    void testSelectWidget() {
        DashboardDesignerPlugin plugin;
        QSignalSpy selectSpy(&plugin, &DashboardDesignerPlugin::widgetSelected);

        plugin.addWidget("gauge");
        plugin.selectWidget(0);
        QCOMPARE(plugin.selectedWidget(), 0);
        QCOMPARE(selectSpy.count(), 1);
    }

    // Verify property update emits widgetModified signal
    void testUpdateWidgetProperty() {
        DashboardDesignerPlugin plugin;
        QSignalSpy propSpy(&plugin, &DashboardDesignerPlugin::widgetModified);

        plugin.addWidget("gauge");
        plugin.updateWidgetProperty(0, "label", "Updated Label");
        QCOMPARE(propSpy.count(), 1);
    }

    // Verify widget palette widget exists
    void testWidgetPalette() {
        DashboardDesignerPlugin plugin;
        QVERIFY(plugin.widgetPalette() != nullptr);
    }

    // Verify property editor widget exists
    void testPropertyEditor() {
        DashboardDesignerPlugin plugin;
        QVERIFY(plugin.propertyEditor() != nullptr);
    }

    // Verify mode tabs widget exists
    void testModeTabs() {
        DashboardDesignerPlugin plugin;
        QVERIFY(plugin.modeTabs() != nullptr);
    }

    // Verify preview mode toggling
    void testPreviewMode() {
        DashboardDesignerPlugin plugin;

        QCOMPARE(plugin.isPreviewMode(), false);

        plugin.setPreviewMode(true);
        QCOMPARE(plugin.isPreviewMode(), true);

        plugin.setPreviewMode(false);
        QCOMPARE(plugin.isPreviewMode(), false);
    }

    // Verify export produces non-empty JSON
    void testExportConfiguration() {
        DashboardDesignerPlugin plugin;

        plugin.addWidget("gauge");
        QString json = plugin.exportConfiguration();
        QVERIFY(!json.isEmpty());
    }

    // Verify import creates widgets from JSON
    void testImportConfiguration() {
        DashboardDesignerPlugin plugin;

        QString json =
            R"([{"id":"w1","type":"gauge","label":"Test","x":0,"y":0,"width":100,"height":100,"properties":{}}])";
        plugin.importConfiguration(json);
        QVERIFY(plugin.widgetCount() > 0);
    }

    // Verify status label widget exists
    void testStatusLabel() {
        DashboardDesignerPlugin plugin;
        QVERIFY(plugin.statusLabel() != nullptr);
    }
};

QTEST_MAIN(DashboardDesignerPluginTest)
#include "dashboarddesigner_plugin_test.moc"
