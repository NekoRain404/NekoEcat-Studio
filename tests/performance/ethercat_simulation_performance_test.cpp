#include "services/EtherCATSimulationService.h"
#include <QElapsedTimer>
#include <QTest>

class EtherCATSimulationPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testCreateSlavePerformance() {
        EtherCATSimulationService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            SimulationSlaveConfig config;
            config.position = i;
            config.name = QStringLiteral("slave_%1").arg(i);
            config.vendorId = "0x00000001";
            config.productCode = "0x00000001";
            config.inputSize = 4;
            config.outputSize = 4;
            config.cycleTimeUs = 1000.0;
            svc.createVirtualSlave(config);
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 createVirtualSlave() calls:" << elapsed << "ms";
    }

    void testSimulationStatePerformance() {
        EtherCATSimulationService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.simulationState();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 simulationState() calls:" << elapsed << "ms";
    }

    void testVirtualSlavesPerformance() {
        EtherCATSimulationService svc;
        for (int i = 0; i < 100; i++) {
            SimulationSlaveConfig config;
            config.position = i;
            config.name = QStringLiteral("slave_%1").arg(i);
            config.vendorId = "0x00000001";
            config.productCode = "0x00000001";
            config.inputSize = 4;
            config.outputSize = 4;
            config.cycleTimeUs = 1000.0;
            svc.createVirtualSlave(config);
        }
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.virtualSlaves();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 1000);
        qDebug() << "10000 virtualSlaves() calls:" << elapsed << "ms";
    }

    void testStartStopPerformance() {
        EtherCATSimulationService svc;
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < 10000; i++) {
            svc.startSimulation();
            svc.stopSimulation();
        }
        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 2000);
        qDebug() << "10000 start/stop cycles:" << elapsed << "ms";
    }
};

QTEST_MAIN(EtherCATSimulationPerformanceTest)
#include "ethercat_simulation_performance_test.moc"
