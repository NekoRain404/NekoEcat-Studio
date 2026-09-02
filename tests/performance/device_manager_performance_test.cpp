#include "infra/EcatClient.h"
#include "services/DeviceManagerService.h"
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QTest>

class DeviceManagerPerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testDiscoveryPerformance() {
        EcatClient client;
        DeviceManagerService svc(&client);

        QElapsedTimer timer;
        timer.start();
        svc.discoverDevices();
        qint64 elapsed = timer.elapsed();

        QVERIFY(elapsed < 10000);
        qDebug() << "Device discovery:" << elapsed << "ms";
    }

    void testDeviceListPerformance() {
        EcatClient client;
        DeviceManagerService svc(&client);

        QElapsedTimer timer;
        timer.start();
        auto list = svc.deviceList();
        qint64 elapsed = timer.elapsed();

        QVERIFY(elapsed < 100);
        qDebug() << "Device list query:" << elapsed << "ms for" << list.size() << "devices";
    }

    void testDeviceInfoPerformance() {
        EcatClient client;
        DeviceManagerService svc(&client);

        QElapsedTimer timer;
        timer.start();
        auto info = svc.deviceInfo(0);
        qint64 elapsed = timer.elapsed();

        QVERIFY(elapsed < 100);
        qDebug() << "Device info query:" << elapsed << "ms";
    }

    void testSignalEmission() {
        EcatClient client;
        DeviceManagerService svc(&client);
        QSignalSpy discoveredSpy(&svc, &DeviceManagerService::deviceDiscovered);
        QSignalSpy removedSpy(&svc, &DeviceManagerService::deviceRemoved);
        QSignalSpy stateSpy(&svc, &DeviceManagerService::deviceStateChanged);
        QVERIFY(discoveredSpy.isValid());
        QVERIFY(removedSpy.isValid());
        QVERIFY(stateSpy.isValid());
    }

    void testDefaultDeviceCount() {
        EcatClient client;
        DeviceManagerService svc(&client);
        QCOMPARE(svc.deviceCount(), 0);
    }
};

QTEST_MAIN(DeviceManagerPerformanceTest)
#include "device_manager_performance_test.moc"
