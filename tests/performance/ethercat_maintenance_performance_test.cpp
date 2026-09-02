#include "services/EtherCATMaintenanceService.h"
#include <QElapsedTimer>
#include <QTest>

class EtherCATMaintenancePerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testSchedulePerformance() {
        EtherCATMaintenanceService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.scheduleTask("Cleanup", "daily");
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "1000 schedule calls:" << elapsed << "ms";
    }

    void testListPerformance() {
        EtherCATMaintenanceService svc(nullptr, nullptr);
        for (int i = 0; i < 100; i++) {
            svc.scheduleTask("Cleanup", "daily");
        }
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.listTasks();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 100);
        qDebug() << "1000 list calls:" << elapsed << "ms";
    }

    void testOfflineRunRejectionPerformance() {
        EtherCATMaintenanceService svc(nullptr, nullptr);
        QVector<MaintenanceTaskInfo> tasks;
        for (int i = 0; i < 100; i++) {
            tasks.append(svc.scheduleTask("Diagnostic", "weekly"));
        }
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            auto result = svc.runTask(tasks[i % tasks.size()].id);
            QCOMPARE(result.status, QStringLiteral("Rejected"));
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 500);
        qDebug() << "1000 offline run rejections:" << elapsed << "ms";
    }
};

QTEST_MAIN(EtherCATMaintenancePerformanceTest)
#include "ethercat_maintenance_performance_test.moc"
