// PluginIntegrationTest — Tests for plugin registry integration
//
// Test coverage:
//   - Plugin registration and lookup by id
//   - Plugin ordering (sorted by defaultOrder)
//   - Plugin visibility filtering
//   - Plugin widget creation
//   - Plugin identity (id, display names)

#include <QTest>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/topology/TopologyPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class PluginIntegrationTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  EventBus *bus_ = nullptr;
  ServiceContainer *container_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    bus_ = new EventBus(this);
    container_ = new ServiceContainer(client_, bus_, this);
  }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  // Register plugins and verify lookup by id
  void testPluginRegistration() {
    PluginRegistry registry;
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    registry.registerPlugin(&topo);
    registry.registerPlugin(&sm);

    QCOMPARE(registry.count(), 2);
    QVERIFY(registry.findById("topology") == &topo);
    QVERIFY(registry.findById("statemachine") == &sm);
  }

  // Visible plugins are sorted by default order
  void testPluginOrdering() {
    PluginRegistry registry;
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    registry.registerPlugin(&topo);
    registry.registerPlugin(&sm);

    auto plugins = registry.visiblePlugins();
    QCOMPARE(plugins.size(), 2);
    QVERIFY(plugins[0]->defaultOrder() <= plugins[1]->defaultOrder());
  }

  // All registered plugins report visible
  void testPluginVisibility() {
    PluginRegistry registry;
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    registry.registerPlugin(&topo);
    registry.registerPlugin(&sm);

    auto visible = registry.visiblePlugins();
    QCOMPARE(visible.size(), 2);
    for (auto *p : visible) {
      QVERIFY(p->visible());
    }
  }

  // Both plugins create non-null widgets
  void testPluginWidgetCreation() {
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    QVERIFY(topo.widget() != nullptr);
    QVERIFY(sm.widget() != nullptr);
  }

  // Plugins have correct id and display names
  void testPluginIdentity() {
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    QCOMPARE(topo.id(), QString("topology"));
    QCOMPARE(sm.id(), QString("statemachine"));

    QVERIFY(!topo.displayName().isEmpty());
    QVERIFY(!sm.displayName().isEmpty());

    QVERIFY(!topo.displayNameZh().isEmpty());
    QVERIFY(!sm.displayNameZh().isEmpty());
  }
};

QTEST_MAIN(PluginIntegrationTest)
#include "plugin_integration_test.moc"