#include <QTest>
#include <QSignalSpy>
#include "plugins/WorkspacePlugin.h"

class MockPlugin : public WorkspacePlugin {
public:
    QString id() const override { return "mock"; }
    QString displayName() const override { return "Mock Plugin"; }
    QString displayNameZh() const override { return "模拟插件"; }
    int defaultOrder() const override { return 42; }
    bool visible() const override { return true; }
    QWidget *widget() override { return nullptr; }
};

class WorkspacePluginInterfaceTest : public QObject {
    Q_OBJECT
private slots:
    void testIdentity() {
        MockPlugin p;
        QCOMPARE(p.id(), "mock");
        QCOMPARE(p.displayName(), "Mock Plugin");
        QCOMPARE(p.displayNameZh(), "模拟插件");
        QCOMPARE(p.defaultOrder(), 42);
        QVERIFY(p.visible());
    }
    void testSignalsExist() {
        MockPlugin p;
        QSignalSpy navSpy(&p, SIGNAL(requestNavigate(QString)));
        QSignalSpy diagSpy(&p, SIGNAL(updateDiagnostics(QString,QString,QString)));
        QVERIFY(navSpy.isValid());
        QVERIFY(diagSpy.isValid());
    }
    void testDefaultIcon() {
        MockPlugin p;
        QVERIFY(p.icon().isNull());
    }
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
