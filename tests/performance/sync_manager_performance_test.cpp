#include "services/SyncManagerService.h"
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QTest>

class SyncManagerPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testConfigureThroughput() {
        SyncManagerService svc;
        QElapsedTimer timer;
        timer.start();

        const int totalConfigs = 10000;
        const int positions = 2500;
        SyncManagerConfig cfg;
        for (int i = 0; i < totalConfigs; i++) {
            int pos = i / 4;
            int sm = i % 4;
            cfg.smIndex = sm;
            cfg.direction = (sm < 2) ? SmDirection::Input : SmDirection::Output;
            svc.configureSyncManager(pos, sm, cfg);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "Configure throughput:" << totalConfigs << "SM configs (" << positions << "positions) in" << elapsed
                 << "ms";
    }

    void testAssignPdoThroughput() {
        SyncManagerService svc;

        for (int pos = 0; pos < 2500; pos++) {
            for (int sm = 0; sm < 4; sm++) {
                SyncManagerConfig cfg;
                cfg.smIndex = sm;
                svc.configureSyncManager(pos, sm, cfg);
            }
        }

        QElapsedTimer timer;
        timer.start();

        const int count = 10000;
        for (int i = 0; i < count; i++) {
            svc.assignPdo(i / 4, i % 4, 0x1600 + i);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "Assign PDO throughput:" << count << "assignments in" << elapsed << "ms";
    }

    void testConfigQueryLatency() {
        SyncManagerService svc;

        for (int pos = 0; pos < 100; pos++) {
            for (int sm = 0; sm < 4; sm++) {
                SyncManagerConfig cfg;
                cfg.smIndex = sm;
                svc.configureSyncManager(pos, sm, cfg);
            }
        }

        QElapsedTimer timer;
        timer.start();

        const int count = 100000;
        volatile int sink = 0;
        for (int i = 0; i < count; i++) {
            SyncManagerConfig cfg = svc.syncManagerConfig(i % 100, i % 4);
            sink = cfg.smIndex;
        }

        qint64 elapsed = timer.elapsed();
        Q_UNUSED(sink);
        QVERIFY(elapsed < 500);
        qDebug() << "Config query latency:" << count << "syncManagerConfig() in" << elapsed << "ms";
    }

    void testSyncManagersListThroughput() {
        SyncManagerService svc;

        for (int pos = 0; pos < 100; pos++) {
            for (int sm = 0; sm < 4; sm++) {
                SyncManagerConfig cfg;
                cfg.smIndex = sm;
                svc.configureSyncManager(pos, sm, cfg);
            }
        }

        QElapsedTimer timer;
        timer.start();

        const int count = 10000;
        volatile int sink = 0;
        for (int i = 0; i < count; i++) {
            QVector<int> list = svc.syncManagers(i % 100);
            sink = list.size();
        }

        qint64 elapsed = timer.elapsed();
        Q_UNUSED(sink);
        QVERIFY(elapsed < 500);
        qDebug() << "SyncManagers list throughput:" << count << "syncManagers() in" << elapsed << "ms";
    }
};

QTEST_MAIN(SyncManagerPerformanceTest)
#include "sync_manager_performance_test.moc"
