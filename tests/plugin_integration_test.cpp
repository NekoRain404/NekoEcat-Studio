#include <QTest>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"
#include "plugins/notes/NotesPlugin.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/ServiceContainer.h"

class PluginIntegrationTest : public QObject {
  Q_OBJECT
private:
  ServiceContainer *container_ = nullptr;

private slots:
  void init() { container_ = new ServiceContainer(this); }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

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
};

QTEST_MAIN(PluginIntegrationTest)
#include "plugin_integration_test.moc"
