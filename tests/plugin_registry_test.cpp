#include <QTest>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"

class MockPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  MockPlugin(QString id, int order, bool vis = true)
    : id_(id), order_(order), vis_(vis) {}
  QString id() const override { return id_; }
  QString displayName() const override { return id_; }
  QString displayNameZh() const override { return id_; }
  int defaultOrder() const override { return order_; }
  bool visible() const override { return vis_; }
  QWidget *widget() override { return nullptr; }
private:
  QString id_;
  int order_;
  bool vis_;
};

class PluginRegistryTest : public QObject {
  Q_OBJECT
private slots:
  void registerThreePlugins() {
    PluginRegistry reg;
    MockPlugin a("a", 20);
    MockPlugin b("b", 10);
    MockPlugin c("c", 30);
    reg.registerPlugin(&a);
    reg.registerPlugin(&b);
    reg.registerPlugin(&c);
    QCOMPARE(reg.count(), 3);
  }

  void sortedByDefaultOrder() {
    PluginRegistry reg;
    MockPlugin a("a", 20);
    MockPlugin b("b", 10);
    MockPlugin c("c", 30);
    reg.registerPlugin(&a);
    reg.registerPlugin(&b);
    reg.registerPlugin(&c);
    QCOMPARE(reg.pluginAt(0)->id(), "b");
    QCOMPARE(reg.pluginAt(1)->id(), "a");
    QCOMPARE(reg.pluginAt(2)->id(), "c");
  }

  void findByIdReturnsCorrect() {
    PluginRegistry reg;
    MockPlugin a("alpha", 10);
    MockPlugin b("beta", 20);
    reg.registerPlugin(&a);
    reg.registerPlugin(&b);
    QCOMPARE(reg.findById("alpha"), &a);
    QCOMPARE(reg.findById("beta"), &b);
  }

  void findByIdNonexistentReturnsNull() {
    PluginRegistry reg;
    MockPlugin a("alpha", 10);
    reg.registerPlugin(&a);
    QCOMPARE(reg.findById("nope"), nullptr);
  }

  void duplicateRegistrationIgnored() {
    PluginRegistry reg;
    MockPlugin a("only", 10);
    reg.registerPlugin(&a);
    reg.registerPlugin(&a);
    QCOMPARE(reg.count(), 1);
  }

  void visiblePluginsFiltersCorrectly() {
    PluginRegistry reg;
    MockPlugin a("a", 10, true);
    MockPlugin b("b", 20, false);
    MockPlugin c("c", 30, true);
    reg.registerPlugin(&a);
    reg.registerPlugin(&b);
    reg.registerPlugin(&c);
    auto vis = reg.visiblePlugins();
    QCOMPARE(vis.size(), 2);
    QCOMPARE(vis[0]->id(), "a");
    QCOMPARE(vis[1]->id(), "c");
  }

  void nullAndEmptyIdSafe() {
    PluginRegistry reg;
    MockPlugin a("", 10);
    reg.registerPlugin(&a);
    QCOMPARE(reg.count(), 0);
    reg.registerPlugin(nullptr);
    QCOMPARE(reg.count(), 0);
  }
};

QTEST_MAIN(PluginRegistryTest)
#include "plugin_registry_test.moc"
