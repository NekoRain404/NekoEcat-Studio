// PerformanceDashboardPluginTest — Tests for PerformanceDashboardPlugin
//
// Test coverage:
//   - Plugin identity (id, displayName, displayNameZh)
//   - Plugin visibility and ordering
//   - Widget creation
//   - Refresh functionality
//   - Report generation

#include <QApplication>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTabWidget>
#include <QTest>

#include "infra/EcatClient.h"
#include "plugins/performancedashboard/PerformanceDashboardPlugin.h"
#include "services/EventBus.h"
#include "services/PerformanceMonitorService.h"

class PerformanceDashboardPluginTest : public QObject {
    Q_OBJECT
private:
    EcatClient* client_ = nullptr;
    EventBus* bus_ = nullptr;
    PerformanceMonitorService* svc_ = nullptr;
    PerformanceDashboardPlugin* plugin_ = nullptr;

private slots:
    void init() {
        client_ = new EcatClient(this);
        bus_ = new EventBus(this);
        svc_ = new PerformanceMonitorService(bus_, client_, this);
        plugin_ = new PerformanceDashboardPlugin(svc_, this);
    }

    void cleanup() {
        delete plugin_;
        plugin_ = nullptr;
        delete svc_;
        svc_ = nullptr;
        delete bus_;
        bus_ = nullptr;
        delete client_;
        client_ = nullptr;
    }

    // ── Identity ─────────────────────────────────────────────────────

    void testPluginId() { QCOMPARE(plugin_->id(), QString("performancedashboard")); }

    void testDisplayName() { QCOMPARE(plugin_->displayName(), QString("Performance")); }

    void testDisplayNameZh() { QCOMPARE(plugin_->displayNameZh(), QString("性能监控")); }

    void testDefaultOrder() { QCOMPARE(plugin_->defaultOrder(), 135); }

    void testVisible() { QVERIFY(!plugin_->visible()); }

    // ── Widget ───────────────────────────────────────────────────────

    void testWidgetNotNull() { QVERIFY(plugin_->widget() != nullptr); }

    void testWidgetHasTabs() {
        QWidget* w = plugin_->widget();
        QTabWidget* tabs = w->findChild<QTabWidget*>();
        QVERIFY(tabs != nullptr);
        QCOMPARE(tabs->count(), 6); // Overview, Startup, Runtime, Memory, History, Reports
    }

    // ── Lifecycle ────────────────────────────────────────────────────

    void testActivateDeactivate() {
        // Should not crash
        plugin_->activate();
        plugin_->deactivate();
    }

    // ── Refresh ──────────────────────────────────────────────────────

    void testRefresh() {
        // Should not crash
        plugin_->refresh();
    }

    void testRefreshWithStartupData() {
        svc_->beginStartup();
        svc_->recordStartupPhase("test", 10.0);
        svc_->endStartup();

        plugin_->refresh();
        // Should not crash
    }

    void testRefreshWithRuntimeData() {
        svc_->recordSdoReadLatency(15.0);
        svc_->recordSdoWriteLatency(20.0);
        svc_->recordStateTransition(100.0);
        svc_->recordFreeRunCycleTime(1000.0);
        svc_->recordUiUpdateTime(8.0);

        plugin_->refresh();
        // Should not crash
    }

    void testRefreshWithMemoryData() {
        svc_->recordServiceMemory("TestService", 1024);
        svc_->recordPluginMemory("TestPlugin", 512);
        svc_->recordCacheMemory("TestCache", 2048);

        plugin_->refresh();
        // Should not crash
    }

    // ── Report Generation ────────────────────────────────────────────

    void testGenerateReport() {
        svc_->beginStartup();
        svc_->recordStartupPhase("test", 10.0);
        svc_->endStartup();

        svc_->recordSdoReadLatency(15.0);

        plugin_->generateReport();
        // Should not crash
    }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    PerformanceDashboardPluginTest t;
    return QTest::qExec(&t, argc, argv);
}

#include "performance_dashboard_plugin_test.moc"
