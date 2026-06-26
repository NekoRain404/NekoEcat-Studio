#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATIntegrationService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class EtherCATIntegrationPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testPLCConnectionOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATIntegrationService svc(&bus, &client);

    PlcConfig cfg;
    cfg.ipAddress = "192.168.1.1";
    cfg.port = 44818;
    cfg.protocol = "EtherNet/IP";
    cfg.timeout = 1000;

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.connectToPLC(cfg));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "PLC connection offline rejection throughput:" << count << "connects in" << elapsed << "ms";
  }

  void testSCADAConnectionOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATIntegrationService svc(&bus, &client);

    ScadaConfig cfg;
    cfg.serverUrl = "opc.tcp://192.168.1.2:4840";
    cfg.username = "admin";
    cfg.password = "admin";

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.connectToSCADA(cfg));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "SCADA connection offline rejection throughput:" << count << "connects in" << elapsed << "ms";
  }

  void testMESConnectionOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATIntegrationService svc(&bus, &client);

    MesConfig cfg;
    cfg.endpoint = "https://mes.example.com/api/v1";
    cfg.apiKey = "test-key-123";
    cfg.version = "v1";

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.connectToMES(cfg));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "MES connection offline rejection throughput:" << count << "connects in" << elapsed << "ms";
  }

  void testERPConnectionOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATIntegrationService svc(&bus, &client);

    ErpConfig cfg;
    cfg.host = "erp.example.com";
    cfg.database = "production";
    cfg.credentials = "user:pass";

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.connectToERP(cfg));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "ERP connection offline rejection throughput:" << count << "connects in" << elapsed << "ms";
  }

  void testSyncDataOfflineRejectionThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATIntegrationService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVERIFY(!svc.syncData("PLC"));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "Integration syncData offline rejection throughput:" << count << "syncs in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATIntegrationPerformanceTest)
#include "ethercat_integration_performance_test.moc"
