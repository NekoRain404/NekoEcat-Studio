// PluginRegistry Test Suite
//
// This test suite verifies the PluginRegistry plugin management system.
//
// Test Coverage:
//   - Plugin registration and counting
//   - Sorting by defaultOrder() ascending
//   - Id-based lookup (findById)
//   - Nonexistent id returns nullptr
//   - Duplicate registration is ignored
//   - Visibility filtering (visiblePlugins)
//   - Null and empty id safety guards
//   - Out-of-range pluginAt() returns nullptr
//
// Test Dependencies:
//   - Qt6::Test (QTest framework)
//   - PluginRegistry (plugin manager)
//   - WorkspacePlugin (base interface)
//   - MockPlugin (test double with configurable id/order/visibility)
//
// Test Environment:
//   - No QApplication required (pure logic tests)
//   - Uses MockPlugin as a lightweight test double

#include <QTest>
#include "plugins/PluginRegistry.h"
#include "plugins/WorkspacePlugin.h"

/// Lightweight test double implementing WorkspacePlugin with configurable identity.
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

/// Test suite verifying PluginRegistry registration, ordering, lookup, and filtering.
class PluginRegistryTest : public QObject {
  Q_OBJECT
private slots:
  // Test that registering 3 plugins results in count() == 3.
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

  // Test that plugins are sorted by defaultOrder() ascending after registration.
  // Setup: Register plugins with order 20, 10, 30.
  // Assert: pluginAt(0) has order 10, pluginAt(1) has order 20, pluginAt(2) has order 30.
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

  // Test that findById returns the correct plugin for a known id.
  // Setup: Register plugins "alpha" and "beta".
  // Assert: findById("alpha") returns alpha, findById("beta") returns beta.
  void findByIdReturnsCorrect() {
    PluginRegistry reg;
    MockPlugin a("alpha", 10);
    MockPlugin b("beta", 20);
    reg.registerPlugin(&a);
    reg.registerPlugin(&b);
    QCOMPARE(reg.findById("alpha"), &a);
    QCOMPARE(reg.findById("beta"), &b);
  }

  // Test that findById returns nullptr for a nonexistent id.
  // Setup: Register plugin "alpha", search for "nope".
  // Assert: Result is nullptr.
  void findByIdNonexistentReturnsNull() {
    PluginRegistry reg;
    MockPlugin a("alpha", 10);
    reg.registerPlugin(&a);
    QCOMPARE(reg.findById("nope"), nullptr);
  }

  // Test that registering the same plugin twice is silently ignored.
  // Setup: Register plugin "only" twice.
  // Assert: count() remains 1.
  void duplicateRegistrationIgnored() {
    PluginRegistry reg;
    MockPlugin a("only", 10);
    reg.registerPlugin(&a);
    reg.registerPlugin(&a);
    QCOMPARE(reg.count(), 1);
  }

  // Test that visiblePlugins() filters out plugins where visible() == false.
  // Setup: Register 3 plugins (2 visible, 1 hidden).
  // Assert: visiblePlugins() returns only the 2 visible plugins in order.
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

  // Test that null and empty-id plugins are safely rejected.
  // Setup: Register plugin with empty id, then nullptr.
  // Assert: count() remains 0 for both.
  void nullAndEmptyIdSafe() {
    PluginRegistry reg;
    MockPlugin a("", 10);
    reg.registerPlugin(&a);
    QCOMPARE(reg.count(), 0);
    reg.registerPlugin(nullptr);
    QCOMPARE(reg.count(), 0);
  }

  // Test that pluginAt() returns nullptr for out-of-range indices.
  // Setup: Register 1 plugin, query index -1 and 999.
  // Assert: Both return nullptr.
  void testOutOfRangePluginAt() {
    PluginRegistry reg;
    MockPlugin a("a", 10);
    reg.registerPlugin(&a);
    QCOMPARE(reg.pluginAt(-1), nullptr);
    QCOMPARE(reg.pluginAt(999), nullptr);
  }
};

QTEST_MAIN(PluginRegistryTest)
#include "plugin_registry_test.moc"
