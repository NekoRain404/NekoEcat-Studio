#include "services/EtherCATConfigService.h"
#include <QElapsedTimer>
#include <QTest>

class EtherCATConfigPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testValidatePerformance() {
        EtherCATConfigService svc;
        ConfigProfile p;
        p.name = "perf";
        ConfigParameter param;
        param.name = "cycle_time";
        param.value = "1000";
        p.parameters.append(param);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.validateProfile(p);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 validateProfile() calls:" << elapsed << "ms";
    }

    void testSaveLoadPerformance() {
        EtherCATConfigService svc;
        ConfigProfile p;
        p.name = "perf";
        svc.setCurrentProfile(p);
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 1000; i++) {
            svc.saveProfile(QStringLiteral("p%1").arg(i));
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "1000 saveProfile() calls:" << elapsed << "ms";
    }

    void testAddParameterPerformance() {
        EtherCATConfigService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            ConfigParameter param;
            param.name = QStringLiteral("param_%1").arg(i);
            param.value = QStringLiteral("%1").arg(i);
            svc.addParameter(param);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 addParameter() calls:" << elapsed << "ms";
    }
};

QTEST_MAIN(EtherCATConfigPerformanceTest)
#include "ethercat_config_performance_test.moc"
