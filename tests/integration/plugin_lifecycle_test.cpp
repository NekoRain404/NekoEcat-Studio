#include <QTest>
#include <QSignalSpy>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/topology/TopologyPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/SettingsDialog.h"
#include "infra/EcatClient.h"

class PluginLifecycleTest : public QObject {
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
    bus_ = nullptr;
  }

  void testPluginRegistration() {
    PluginRegistry registry;
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    QCOMPARE(registry.count(), 0);

    registry.registerPlugin(&topo);
    QCOMPARE(registry.count(), 1);
    QVERIFY(registry.findById("topology") == &topo);

    registry.registerPlugin(&sm);
    QCOMPARE(registry.count(), 2);
    QVERIFY(registry.findById("statemachine") == &sm);
  }

  void testPluginActivationDeactivation() {
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    topo.activate();
    topo.deactivate();
    sm.activate();
    sm.deactivate();
  }

  void testPluginSettingsChanges() {
    TopologyPlugin topo(bus_);
    topo.onSettingsChanged(AppSettings{});
  }

  void testPluginConnectionChanges() {
    TopologyPlugin topo(bus_);
    topo.onConnectionChanged(true);
    topo.onConnectionChanged(false);
  }

  void testPluginRegistryOrdering() {
    PluginRegistry registry;
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    registry.registerPlugin(&topo);
    registry.registerPlugin(&sm);

    auto plugins = registry.visiblePlugins();
    QCOMPARE(plugins.size(), 2);
    QVERIFY(plugins[0]->defaultOrder() <= plugins[1]->defaultOrder());
  }

  void testPluginRegistryDuplicateRegistration() {
    PluginRegistry registry;
    TopologyPlugin topo(bus_);

    registry.registerPlugin(&topo);
    QCOMPARE(registry.count(), 1);

    registry.registerPlugin(&topo);
    QCOMPARE(registry.count(), 1);
  }

  void testPluginWidgetCreation() {
    TopologyPlugin topo(bus_);
    StateMachinePlugin sm(container_);

    QVERIFY(topo.widget() != nullptr);
    QVERIFY(sm.widget() != nullptr);
  }

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

  void testTopologyChangePropagation() {
    EventBus bus;
    QSignalSpy spy(&bus, &EventBus::topologyChanged);

    SlaveInfo s1;
    s1.position = 0;
    s1.name = "Slave_0";
    s1.state = "OP";
    SlaveInfo s2;
    s2.position = 1;
    s2.name = "Slave_1";
    s2.state = "OP";

    QVector<SlaveInfo> slaves{s1, s2};
    bus.emitTopologyChanged(slaves);

    QCOMPARE(spy.count(), 1);
    QVector<SlaveInfo> received = spy.at(0).at(0).value<QVector<SlaveInfo>>();
    QCOMPARE(received.size(), 2);
  }

  void testServiceContainerCreation() {
    EcatClient cl;
    EventBus bus;
    ServiceContainer sc(&cl, &bus);
    QVERIFY(sc.client() != nullptr);
    QVERIFY(sc.eventBus() != nullptr);
    QVERIFY(sc.sdo() != nullptr);
    QVERIFY(sc.watch() != nullptr);
    QVERIFY(sc.topology() != nullptr);
    QVERIFY(sc.dcSync() != nullptr);
    QVERIFY(sc.alEvent() != nullptr);
    QVERIFY(sc.signal() != nullptr);
  }
};

QTEST_MAIN(PluginLifecycleTest)
#include "plugin_lifecycle_test.moc"