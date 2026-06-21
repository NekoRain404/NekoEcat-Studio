// PluginTestFixtureTest — Tests for PluginTestFixture
//
// Test coverage:
//   - Fixture setup (container, registry, plugin count)
//   - Plugin registration
//   - Plugin lookup by id
//   - Plugin activation and deactivation

#include <QTest>
#include <QSignalSpy>
#include "fixtures/PluginTestFixture.h"
#include "plugins/WorkspacePlugin.h"

namespace {

class StubPlugin : public WorkspacePlugin {
public:
    explicit StubPlugin(const QString &id, int order = 100)
        : id_(id), order_(order) {}

    QString id() const override { return id_; }
    QString displayName() const override { return id_; }
    QString displayNameZh() const override { return id_; }
    QWidget *widget() override { return &widget_; }
    int defaultOrder() const override { return order_; }
    bool visible() const override { return true; }

    bool activated = false;
    void activate() override { activated = true; }
    void deactivate() override { activated = false; }

private:
    QString id_;
    int order_;
    QWidget widget_;
};

}

class PluginTestFixtureTest : public QObject {
    Q_OBJECT
private slots:
    // Verify fixture setup creates container and registry
    void testSetup() {
        PluginTestFixture fixture;
        QVERIFY(fixture.container() != nullptr);
        QVERIFY(fixture.registry() != nullptr);
        QCOMPARE(fixture.pluginCount(), 0);
    }

    // Test registering a plugin increments count
    void testRegisterPlugin() {
        PluginTestFixture fixture;
        StubPlugin p1("test1");
        fixture.registerPlugin(&p1);
        QCOMPARE(fixture.pluginCount(), 1);
    }

    // Test finding plugins by id
    void testFindPlugin() {
        PluginTestFixture fixture;
        StubPlugin p1("alpha");
        StubPlugin p1b("beta");
        fixture.registerPlugin(&p1);
        fixture.registerPlugin(&p1b);
        QCOMPARE(fixture.pluginCount(), 2);
        QVERIFY(fixture.findPlugin("alpha") == &p1);
        QVERIFY(fixture.findPlugin("beta") == &p1b);
        QVERIFY(fixture.findPlugin("missing") == nullptr);
    }

    // Test activating and deactivating a plugin
    void testActivatePlugin() {
        PluginTestFixture fixture;
        StubPlugin p1("test1");
        fixture.registerPlugin(&p1);
        fixture.activatePlugin("test1");
        QVERIFY(p1.activated);
        fixture.deactivatePlugin("test1");
        QVERIFY(!p1.activated);
    }
};

QTEST_MAIN(PluginTestFixtureTest)
#include "plugin_fixture_test.moc"
