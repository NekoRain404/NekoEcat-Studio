#include "services/WorkflowDeploymentService.h"
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QTest>

class WorkflowDeploymentPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testConfigurationDeploymentThroughput() {
        WorkflowDeploymentService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 1000;
        for (int i = 0; i < count; i++) {
            WfConfigData data;
            data.configuration = QByteArray::number(i);
            data.version = QStringLiteral("1.0.%1").arg(i);
            svc.deployConfiguration(i % 64, data);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Configuration deployment throughput:" << count << "in" << elapsed << "ms";
    }

    void testFirmwareDeploymentThroughput() {
        WorkflowDeploymentService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 500;
        for (int i = 0; i < count; i++) {
            WfFirmwareData data;
            data.firmware = QByteArray(1024, static_cast<char>(i % 256));
            data.version = QStringLiteral("2.0.%1").arg(i);
            svc.deployFirmware(i % 64, data);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Firmware deployment throughput:" << count << "in" << elapsed << "ms";
    }

    void testSoftwareDeploymentThroughput() {
        WorkflowDeploymentService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 200;
        for (int i = 0; i < count; i++) {
            WfSoftwareData data;
            data.software = QByteArray(4096, static_cast<char>(i % 256));
            data.version = QStringLiteral("3.0.%1").arg(i);
            svc.deploySoftware(i % 64, data);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Software deployment throughput:" << count << "in" << elapsed << "ms";
    }

    void testSignalThroughput() {
        WorkflowDeploymentService svc;
        QSignalSpy startedSpy(&svc, &WorkflowDeploymentService::deploymentStarted);
        QSignalSpy completedSpy(&svc, &WorkflowDeploymentService::deploymentCompleted);

        QElapsedTimer timer;
        timer.start();

        const int count = 1000;
        for (int i = 0; i < count; i++) {
            WfConfigData data;
            data.configuration = QByteArray::number(i);
            data.version = QStringLiteral("1.0.%1").arg(i);
            svc.deployConfiguration(i % 64, data);
        }

        qint64 elapsed = timer.elapsed();
        QCOMPARE(startedSpy.count(), count);
        QCOMPARE(completedSpy.count(), count);
        QVERIFY(elapsed < 5000);
        qDebug() << "Signal throughput:" << count << "deployments in" << elapsed << "ms";
    }

    void testMixedDeploymentThroughput() {
        WorkflowDeploymentService svc;
        QElapsedTimer timer;
        timer.start();

        const int count = 250;
        for (int i = 0; i < count; i++) {
            WfConfigData cfg;
            cfg.configuration = QByteArray::number(i);
            cfg.version = QStringLiteral("1.0.%1").arg(i);
            svc.deployConfiguration(i % 64, cfg);

            WfFirmwareData fw;
            fw.firmware = QByteArray(512, static_cast<char>(i % 256));
            fw.version = QStringLiteral("2.0.%1").arg(i);
            svc.deployFirmware(i % 64, fw);

            WfSoftwareData sw;
            sw.software = QByteArray(1024, static_cast<char>(i % 256));
            sw.version = QStringLiteral("3.0.%1").arg(i);
            svc.deploySoftware(i % 64, sw);

            WfSystemData sys;
            sys.system = QByteArray(2048, static_cast<char>(i % 256));
            sys.version = QStringLiteral("4.0.%1").arg(i);
            svc.deploySystem(i % 64, sys);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "Mixed deployment throughput:" << count * 4 << "deployments in" << elapsed << "ms";
    }

    void testMemoryStability() {
        WorkflowDeploymentService svc;

        for (int round = 0; round < 10; round++) {
            for (int i = 0; i < 100; i++) {
                WfConfigData cfg;
                cfg.configuration = QByteArray(256, static_cast<char>(i % 256));
                cfg.version = QStringLiteral("1.%1.%2").arg(round).arg(i);
                svc.deployConfiguration(i % 64, cfg);
            }
        }

        qDebug() << "Memory stability: 1000 deployments across 10 rounds";
    }
};

QTEST_MAIN(WorkflowDeploymentPerformanceTest)
#include "workflow_deployment_performance_test.moc"
