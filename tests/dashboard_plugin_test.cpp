// Dashboard Plugin Test Suite
//
// This test suite verifies the DashboardPlugin configurable dashboard.
//
// Test Coverage:
//   - Plugin identity: id, displayName, displayNameZh
//   - Default order: 130
//   - Visibility: always visible
//   - Widget creation: non-null container widget
//   - Service accessor: returns correct ChartService pointer
//   - Gauge count: 4 gauge widgets
//   - Counter count: 4 counter labels
//   - Refresh: updates counters without crash
//
// Test Dependencies:
//   - Qt6::Test (QTest framework)
//   - ChartService (data service)
//   - DashboardPlugin (plugin under test)
//
// Test Environment:
//   - Requires QT_QPA_PLATFORM=offscreen for widget-based tests

#include <QTest>
#include "services/ChartService.h"
#include "plugins/dashboard/DashboardPlugin.h"

/// Test suite verifying DashboardPlugin identity, widget creation, and gauge/counter management.
class DashboardPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Test that plugin identity returns correct id, displayName, and displayNameZh.
  void testPluginIdentity() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("dashboard"));
    QCOMPARE(plugin.displayName(), QString("Dashboard"));
    QCOMPARE(plugin.displayNameZh(), QStringLiteral("仪表盘"));
  }

  // Test that default order is 130.
  void testPluginDefaultOrder() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 130);
  }

  // Test that plugin is visible by default.
  void testPluginVisible() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    QVERIFY(plugin.visible());
  }

  // Test that widget() returns a non-null QWidget.
  void testPluginWidgetNotNull() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Test that service() returns the correct ChartService pointer.
  void testPluginServiceAccessor() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  // Test that gaugeCount() returns 4.
  void testGaugeCount() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    QCOMPARE(plugin.gaugeCount(), 4);
  }

  // Test that counterCount() returns 4.
  void testCounterCount() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    QCOMPARE(plugin.counterCount(), 4);
  }

  // Test that refresh() updates counters without crash.
  void testRefreshUpdatesCounters() {
    ChartService svc;
    DashboardPlugin plugin(&svc);
    plugin.refresh();
    QCOMPARE(plugin.counterCount(), 4);
  }
};

QTEST_MAIN(DashboardPluginTest)
#include "dashboard_plugin_test.moc"
