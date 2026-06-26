// DcSyncOptimizerPluginTest — Tests for DcSyncOptimizerService and
// DcSyncOptimizerPlugin
//
// Test coverage:
//   - Offline optimization fails closed without synthetic DC measurements
//   - Offline apply fails closed without marking results applied
//   - Plugin identity, ordering, visibility
//   - Widget creation and tab count
//   - Optimization result display

#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QJsonObject>
#include <QTabWidget>
#include <QTcpServer>
#include <QFile>

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

    static bool waitForConnected(EcatClient &client) {
        for (int i = 0; i < 50 && !client.isConnected(); ++i) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
            QTest::qWait(10);
        }
        return client.isConnected();
    }

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
        QCOMPARE(result.improvement, 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QVERIFY(result.before.isEmpty());
        QVERIFY(result.after.isEmpty());
        QCOMPARE(spy.count(), 0);
    }

    void testOptimizeDrift() {
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc_->optimizeDrift();
        QCOMPARE(result.category, QString("Drift"));
        QCOMPARE(result.improvement, 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QVERIFY(result.before.isEmpty());
        QVERIFY(result.after.isEmpty());
        QCOMPARE(spy.count(), 0);
    }

    void testOptimizeJitter() {
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc_->optimizeJitter();
        QCOMPARE(result.category, QString("Jitter"));
        QCOMPARE(result.improvement, 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QVERIFY(result.before.isEmpty());
        QVERIFY(result.after.isEmpty());
        QCOMPARE(spy.count(), 0);
    }

    void testOptimizeConfiguration() {
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc_->optimizeConfiguration();
        QCOMPARE(result.category, QString("Configuration"));
        QCOMPARE(result.improvement, 0.0);
        QVERIFY(!result.recommendations.isEmpty());
        QVERIFY(result.before.isEmpty());
        QVERIFY(result.after.isEmpty());
        QCOMPARE(spy.count(), 0);
    }

    void testApplyOptimizationOfflineFailsClosed() {
        auto result = svc_->optimizeSync();
        QSignalSpy spy(svc_, &DcSyncOptimizerService::optimizationApplied);
        bool ok = svc_->applyOptimization(result);
        QVERIFY(!ok);
        QCOMPARE(spy.count(), 0);
        QVERIFY(svc_->pendingResults().isEmpty());
    }

    void testOfflineOptimizationDoesNotCreatePendingResults() {
        svc_->optimizeSync();
        svc_->optimizeDrift();
        QVERIFY(svc_->pendingResults().isEmpty());
    }

    void testConnectedDaemonWithoutTelemetryFailsClosed() {
        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost, 0));

        EcatClient client;
        client.connectToHost(QHostAddress::LocalHost, server.serverPort());
        QVERIFY(waitForConnected(client));

        EventBus bus;
        DcSyncOptimizerService svc(&client, &bus);
        QSignalSpy spy(&svc, &DcSyncOptimizerService::optimizationCompleted);
        auto result = svc.optimizeSync();

        QCOMPARE(result.category, QString("Sync"));
        QCOMPARE(result.improvement, 0.0);
        QVERIFY(result.before.isEmpty());
        QVERIFY(result.after.isEmpty());
        QCOMPARE(spy.count(), 0);
        QVERIFY(svc.pendingResults().isEmpty());
    }

    void testConnectedApplyWithoutBackendAckFailsClosed() {
        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost, 0));

        EcatClient client;
        client.connectToHost(QHostAddress::LocalHost, server.serverPort());
        QVERIFY(waitForConnected(client));

        EventBus bus;
        DcSyncOptimizerService svc(&client, &bus);
        DcSyncOptimizationResult result;
        result.category = QStringLiteral("Sync");
        result.description = QStringLiteral("Distributed Clock synchronization optimization");
        result.after.insert(QStringLiteral("syncIntervalNs"), 500000);
        result.timestamp = QDateTime::currentDateTime();

        QSignalSpy appliedSpy(&svc, &DcSyncOptimizerService::optimizationApplied);
        QSignalSpy errorSpy(&svc, &DcSyncOptimizerService::error);
        QVERIFY(!svc.applyOptimization(result));
        QCOMPARE(appliedSpy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
        QVERIFY(svc.pendingResults().isEmpty());
    }

    void testImplementationDoesNotKeepSyntheticOptimizationSuccessPath() {
        QFile source(QStringLiteral(SOURCE_ROOT
                                    "/apps/ecat-studio/services/DcSyncOptimizerService.cpp"));
        QVERIFY2(source.open(QIODevice::ReadOnly | QIODevice::Text),
                 qPrintable(source.errorString()));
        const QString text = QString::fromUtf8(source.readAll());

        QVERIFY2(!text.contains(QStringLiteral("result.improvement = 50.0")),
                 "DC sync optimizer must not retain synthetic sync improvement");
        QVERIFY2(!text.contains(QStringLiteral("result.improvement = 40.0")),
                 "DC sync optimizer must not retain synthetic drift improvement");
        QVERIFY2(!text.contains(QStringLiteral("result.improvement = 35.0")),
                 "DC sync optimizer must not retain synthetic jitter improvement");
        QVERIFY2(!text.contains(QStringLiteral("result.improvement = 45.0")),
                 "DC sync optimizer must not retain synthetic configuration improvement");
        QVERIFY2(!text.contains(QStringLiteral("emit optimizationCompleted(result)")),
                 "DC sync optimizer must not announce optimization completion without a backend result");
        QVERIFY2(!text.contains(QStringLiteral("applied.applied = true")),
                 "DC sync optimizer must not mark optimizations applied without backend acknowledgement");
        QVERIFY2(!text.contains(QStringLiteral("emit optimizationApplied(applied)")),
                 "DC sync optimizer must not emit applied success without backend acknowledgement");
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
