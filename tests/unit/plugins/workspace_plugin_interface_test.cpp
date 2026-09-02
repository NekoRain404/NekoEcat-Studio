// WorkspacePluginInterfaceTest — Tests for Workspace Plugin Interface
//
// Test coverage:
//   - Plugin identity contract (id, display names, order, visibility)
//   - Signal interface validity (requestNavigate, updateDiagnostics)
//   - Default icon behavior
//   - Lifecycle methods (activate, deactivate, onConnectionChanged)

#include "plugins/WorkspacePlugin.h"
#include <QSignalSpy>
#include <QTest>

class MockPlugin : public WorkspacePlugin {
public:
    QString id() const override { return "mock"; }
    QString displayName() const override { return "Mock Plugin"; }
    QString displayNameZh() const override { return "模拟插件"; }
    int defaultOrder() const override { return 42; }
    bool visible() const override { return true; }
    QWidget* widget() override { return nullptr; }
};

class WorkspacePluginInterfaceTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin identity contract (id, names, order, visibility)
    // Plugin identity contract returns configured values
    void testIdentity() {
        MockPlugin p;
        QCOMPARE(p.id(), "mock");
        QCOMPARE(p.displayName(), "Mock Plugin");
        QCOMPARE(p.displayNameZh(), "模拟插件");
        QCOMPARE(p.defaultOrder(), 42);
        QVERIFY(p.visible());
    }
    // Verify requestNavigate and updateDiagnostics signals are valid
    // Signal interfaces are valid and connectable
    void testSignalsExist() {
        MockPlugin p;
        QSignalSpy navSpy(&p, SIGNAL(requestNavigate(QString)));
        QSignalSpy diagSpy(&p, SIGNAL(updateDiagnostics(QString, QString, QString)));
        QVERIFY(navSpy.isValid());
        QVERIFY(diagSpy.isValid());
    }
    // Default icon is null for base plugin
    // Default icon is null when not overridden
    void testDefaultIcon() {
        MockPlugin p;
        QVERIFY(p.icon().isNull());
    }
    // Lifecycle methods (activate, deactivate, onConnectionChanged) do not crash
    // Lifecycle methods execute without crash
    void testLifecycleDefaults() {
        MockPlugin p;
        // Should not crash
        p.activate();
        p.deactivate();
        p.onConnectionChanged(true);
        p.onConnectionChanged(false);
    }
};

QTEST_MAIN(WorkspacePluginInterfaceTest)
#include "workspace_plugin_interface_test.moc"
