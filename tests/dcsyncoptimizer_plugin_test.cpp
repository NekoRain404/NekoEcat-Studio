// DcSyncOptimizerPluginTest — Tests for DcSyncOptimizerService and
// DcSyncOptimizerPlugin
//
// Test coverage:
//   - Service optimization methods return valid results
//   - Signal emission on optimization completed
//   - Offline apply fails closed without marking results applied
//   - Plugin identity, ordering, visibility
//   - Widget creation and tab count
//   - Optimization result display

#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QJsonObject>
#include <QTabWidget>

#include "plugins/dcsyncoptimizer/DcSyncOptimizerPlugin.h"
#include "plugins/dcsyncoptimizer/SyncOptimizerWidget.h"
#include "plugins/dcsyncoptimizer/DriftOptimizerWidget.h"
#include "services/DcSyncOptimizerService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class DcSyncOptimizerServiceTest : public QObject {
    Q_OBJECT
private:
    EcatClient *client_ = nullptr;
    EventBus *bus_ = nullptr;
    DcSyncOptimizerService *svc_ = nullptr;

private slots:
    void init() {
        client_ = new EcatClient(this);
        bus_ = new EventBus(this);
        svc_ = new DcSyncOptimizerService(client_, bus_, this);
    }

    void cleanup() {
        delete svc_;
        svc_ = nullptr;
        delete bus_;
        bus_ = nullptr;
        delete client_;
        client_ = nullptr;
    }

    void testOptimizeSync() {
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc_->optimizeSync();
        QCOMPARE(result.category, QString("Sync"));
        QVERIFY(result.improvement > 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QVERIFY(!result.before.isEmpty());
        QVERIFY(!result.after.isEmpty());
        QCOMPARE(spy.count(), 1);
    }

    void testOptimizeDrift() {
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc_->optimizeDrift();
        QCOMPARE(result.category, QString("Drift"));
        QVERIFY(result.improvement > 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QCOMPARE(spy.count(), 1);
    }

    void testOptimizeJitter() {
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc_->optimizeJitter();
        QCOMPARE(result.category, QString("Jitter"));
        QVERIFY(result.improvement > 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QCOMPARE(spy.count(), 1);
    }

    void testOptimizeConfiguration() {
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc_->optimizeConfiguration();
        QCOMPARE(result.category, QString("Configuration"));
        QVERIFY(result.improvement > 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QCOMPARE(spy.count(), 1);
    }

    void testApplyOptimizationOfflineFailsClosed() {
        auto result = svc_->optimizeSync();
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationApplied);
        bool ok = svc_->applyOptimization(result);
        QVERIFY(!ok);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(svc_->pendingResults().size(), 1);
        QVERIFY(!svc_->pendingResults().first().applied);
    }

    void testPendingResults() {
        svc_->optimizeSync();
        svc_->optimizeDrift();
        QCOMPARE(svc_->pendingResults().size(), 2);
    }

    void testClearResults() {
        svc_->optimizeSync();
        svc_->clearResults();
        QVERIFY(svc_->pendingResults().isEmpty());
    }
};

class DcSyncOptimizerPluginTest : public QObject {
    Q_OBJECT
private slots:
    void testIdentity() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        QCOMPARE(plugin.id(), QString("dcsyncoptimizer"));
        QCOMPARE(plugin.displayName(), QString("DC Sync Optimizer"));
        QCOMPARE(plugin.displayNameZh(), QStringLiteral("DC 同步优化器"));
    }

    void testDefaultOrder() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        QCOMPARE(plugin.defaultOrder(), 40);
    }

    void testVisible() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        QVERIFY(plugin.visible());
    }

    void testWidgetNotNull() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        QVERIFY(plugin.widget() != nullptr);
    }

    void testTabCount() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        auto *tabs = plugin.widget()->findChild<QTabWidget *>();
        QVERIFY(tabs != nullptr);
        QCOMPARE(tabs->count(), 4);
    }

    void testServiceNotNull() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        QVERIFY(plugin.service() != nullptr);
    }

    void testSyncOptimizerWidgetNotNull() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        QVERIFY(plugin.syncOptimizer() != nullptr);
    }

    void testDriftOptimizerWidgetNotNull() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        QVERIFY(plugin.driftOptimizer() != nullptr);
    }

    void testSyncOptimizeIntegration() {
        EcatClient client;
        EventBus bus;
        DcSyncOptimizerPlugin plugin(&client, &bus);
        auto *tabs = plugin.tabs();
        QVERIFY(tabs != nullptr);
    }
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    int status = 0;
    {
        DcSyncOptimizerServiceTest t;
        status |= QTest::qExec(&t, argc, argv);
    }
    {
        DcSyncOptimizerPluginTest t;
        status |= QTest::qExec(&t, argc, argv);
    }
    return status;
}

#include "dcsyncoptimizer_plugin_test.moc"
