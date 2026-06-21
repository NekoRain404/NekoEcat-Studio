// NetworkDiagnosticsPluginTest — Tests for NetworkDiagnosticsPlugin and service
//
// Test coverage:
//   - Service default state, error counters, and bandwidth
//   - Service error counter reset and port status
//   - Plugin identity, order, visibility, and widget creation
//   - Plugin service accessor and table widgets

#include <QTest>
#include <QTableWidget>
#include "infra/EcatClient.h"
#include "services/NetworkDiagnosticsService.h"
#include "plugins/network/NetworkDiagnosticsPlugin.h"

class NetworkDiagnosticsPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Service has correct default monitoring and health state
  // Verify service default health state
  void testServiceDefaultState() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    QVERIFY(!svc.isMonitoring());
    auto health = svc.currentHealth();
    QCOMPARE(health.portCount, 0);
    QCOMPARE(health.activePorts, 0);
    QCOMPARE(health.errorCount, 0);
  }

  // Error counters are zero by default
  // Verify error counters default to zero
  void testServiceErrorCounters() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    auto errors = svc.errorCounters();
    QCOMPARE(errors.crc, quint64(0));
    QCOMPARE(errors.frame, quint64(0));
    QCOMPARE(errors.lost, quint64(0));
    QCOMPARE(errors.overrun, quint64(0));
  }

  // Bandwidth utilization is zero by default
  // Verify bandwidth utilization defaults to zero
  void testServiceBandwidthUtilization() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    QCOMPARE(svc.bandwidthUtilization(), 0.0);
  }

  // Reset error counters clears all counters
  // Test reset error counters
  void testServiceResetErrorCounters() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    svc.resetErrorCounters();
    auto errors = svc.errorCounters();
    QCOMPARE(errors.crc, quint64(0));
    QCOMPARE(errors.frame, quint64(0));
    QCOMPARE(errors.lost, quint64(0));
    QCOMPARE(errors.overrun, quint64(0));
  }

  // Port status returns invalid for out-of-range port
  // Verify port status for out-of-range index
  void testServicePortStatusOutOfRange() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    auto ps = svc.portStatus(99);
    QCOMPARE(ps.port, -1);
    QVERIFY(!ps.linkUp);
  }

  // Plugin reports correct id, display names
  // Verify plugin id, display names
  void testPluginIdentity() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("network"));
    QCOMPARE(plugin.displayName(), QString("Network Diagnostics"));
    QCOMPARE(plugin.displayNameZh(), QStringLiteral("网络诊断"));
  }

  // Plugin has expected default order
  // Verify default order is 135
  void testPluginDefaultOrder() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 135);
  }

  // Plugin is visible by default
  // Verify plugin is visible
  void testPluginVisible() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    QVERIFY(plugin.visible());
  }

  // Widget is created and not null
  // Check widget is created
  void testPluginWidgetNotNull() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Service accessor returns injected instance
  // Verify service accessor returns correct pointer
  void testPluginServiceAccessor() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  // Port table widget is created and not null
  // Check port table is created
  void testPluginPortTableNotNull() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    QVERIFY(plugin.portTable() != nullptr);
  }

  // Error table widget is created and not null
  // Check error table is created
  void testPluginErrorTableNotNull() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    QVERIFY(plugin.errorTable() != nullptr);
  }

  // Error table has correct row and column count
  // Verify error table has 4 rows and 2 columns
  void testPluginErrorTableCounters() {
    EcatClient client;
    NetworkDiagnosticsService svc(&client);
    NetworkDiagnosticsPlugin plugin(&svc);
    auto *table = plugin.errorTable();
    QCOMPARE(table->rowCount(), 4);
    QCOMPARE(table->columnCount(), 2);
  }
};

QTEST_MAIN(NetworkDiagnosticsPluginTest)
#include "network_diagnostics_plugin_test.moc"
