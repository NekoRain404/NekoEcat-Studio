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
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class PluginIntegrationTest : public QObject {
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

  // Register plugins and verify lookup by id
  // Test registering plugins and lookup by id
  void testPluginRegistration() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);
    
    QCOMPARE(registry.count(), 2);
    QVERIFY(registry.findById("notes") == &notes);
    QVERIFY(registry.findById("statemachine") == &sm);
  }

  // Visible plugins are sorted by default order
  // Test plugins are sorted by defaultOrder
  void testPluginOrdering() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);
    
    auto plugins = registry.visiblePlugins();
    QCOMPARE(plugins.size(), 2);
    QVERIFY(plugins[0]->defaultOrder() <= plugins[1]->defaultOrder());
  }

  // All registered plugins report visible
  // Test only visible plugins are returned
  void testPluginVisibility() {
    PluginRegistry registry;
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    registry.registerPlugin(&notes);
    registry.registerPlugin(&sm);
    
    auto visible = registry.visiblePlugins();
    QCOMPARE(visible.size(), 2);
    for (auto *p : visible) {
      QVERIFY(p->visible());
    }
  }

  // Both plugins create non-null widgets
  // Test widget creation for each plugin
  void testPluginWidgetCreation() {
    NotesPlugin notes;
    StateMachinePlugin sm(container_);
    
    QVERIFY(notes.widget() != nullptr);
    QVERIFY(sm.widget() != nullptr);
  }

  // Plugins have correct id and display names
  // Test plugin id and display names
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
};

QTEST_MAIN(PluginIntegrationTest)
#include "plugin_integration_test.moc"
