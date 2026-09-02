#include "services/EtherCATUpdateService.h"
#include <QElapsedTimer>
#include <QTest>

class EtherCATUpdatePerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testOfflineCheckRejectionPerformance() {
        EtherCATUpdateService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            auto result = svc.checkForUpdates(i % 10);
            QCOMPARE(result.status, QStringLiteral("Rejected"));
        }
        qint64 elapsed = timer.elapsed();
        QCOMPARE(svc.getUpdateHistory().size(), 0);
        QVERIFY(elapsed < 500);
        qDebug() << "1000 offline check rejections:" << elapsed << "ms";
    }

    void testOfflineStartUpdateRejectionPerformance() {
        EtherCATUpdateService svc(nullptr, nullptr);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            auto result = svc.startUpdate(i % 10, "2.0.0");
            QCOMPARE(result.status, QStringLiteral("Rejected"));
        }
        qint64 elapsed = timer.elapsed();
        QCOMPARE(svc.getUpdateHistory().size(), 0);
        QVERIFY(elapsed < 500);
        qDebug() << "1000 offline update start rejections:" << elapsed << "ms";
    }

    void testHistoryPerformance() {
        EtherCATUpdateService svc(nullptr, nullptr);
        for (int i = 0; i < 100; i++) {
            svc.checkForUpdates(i);
        }
        QCOMPARE(svc.getUpdateHistory().size(), 0);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.getUpdateHistory();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 100);
        qDebug() << "1000 history calls:" << elapsed << "ms";
    }
};

QTEST_MAIN(EtherCATUpdatePerformanceTest)
#include "ethercat_update_performance_test.moc"
