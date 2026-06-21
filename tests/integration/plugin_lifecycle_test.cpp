#include <QTest>
#include <QSignalSpy>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/SettingsDialog.h"
#include "infra/EcatClient.h"

class PluginLifecycleTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  ServiceContainer *container_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
  }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  void testPluginRegistration() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    QCOMPARE(registry.count(), 0);

    registry.registerPlugin(&notes);
    QCOMPARE(registry.count(), 1);
    QVERIFY(registry.findById("notes") == &notes);

    registry.registerPlugin(&sm);
    QCOMPARE(registry.count(), 2);
    QVERIFY(registry.findById("statemachine") == &sm);
  }

  void testPluginActivationDeactivation() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    notes.activate();
    notes.deactivate();
    sm.activate();
    sm.deactivate();
  }

  void testPluginSettingsChanges() {
    NotesPlugin notes;
    notes.onSettingsChanged(AppSettings{});
  }

  void testPluginConnectionChanges() {
    NotesPlugin notes;
    notes.onConnectionChanged(true);
    notes.onConnectionChanged(false);
  }

  void testPluginRegistryOrdering() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);

    auto plugins = registry.visiblePlugins();
    QCOMPARE(plugins.size(), 2);
    QVERIFY(plugins[0]->defaultOrder() <= plugins[1]->defaultOrder());
  }

  void testPluginRegistryDuplicateRegistration() {
    PluginRegistry registry;
    NotesPlugin notes;

    registry.registerPlugin(&notes);
    QCOMPARE(registry.count(), 1);

    registry.registerPlugin(&notes);
    QCOMPARE(registry.count(), 1);
  }

  void testPluginWidgetCreation() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    QVERIFY(notes.widget() != nullptr);
    QVERIFY(sm.widget() != nullptr);
  }

  void testPluginIdentity() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);

    QCOMPARE(notes.id(), QString("notes"));
    QCOMPARE(sm.id(), QString("statemachine"));

    QVERIFY(!notes.displayName().isEmpty());
    QVERIFY(!sm.displayName().isEmpty());

    QVERIFY(!notes.displayNameZh().isEmpty());
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
