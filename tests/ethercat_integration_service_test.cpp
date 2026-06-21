// EtherCATIntegrationServiceTest — Tests for EtherCATIntegrationService
//
// Test coverage:
//   - PLC connection (valid, empty IP, zero port)
//   - SCADA connection (valid, empty URL)
//   - MES connection (valid, empty endpoint)
//   - ERP connection (valid, empty host)
//   - Data synchronization and signal emission

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATIntegrationService.h"

class EtherCATIntegrationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify valid PLC config connects successfully
  void testConnectToPLCValid() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    PlcConfig cfg;
    cfg.ipAddress = QStringLiteral("192.168.1.1");
    cfg.port = 502;
    QVERIFY(svc.connectToPLC(cfg));
  }

  // Verify PLC connection fails with empty IP
  void testConnectToPLCEmptyIp() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    PlcConfig cfg;
    cfg.port = 502;
    QVERIFY(!svc.connectToPLC(cfg));
  }

  // Verify PLC connection fails with zero port
  void testConnectToPLCZeroPort() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    PlcConfig cfg;
    cfg.ipAddress = QStringLiteral("192.168.1.1");
    cfg.port = 0;
    QVERIFY(!svc.connectToPLC(cfg));
  }

  // Verify valid SCADA config connects successfully
  void testConnectToSCADAValid() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ScadaConfig cfg;
    cfg.serverUrl = QStringLiteral("http://scada.local");
    cfg.username = QStringLiteral("admin");
    QVERIFY(svc.connectToSCADA(cfg));
  }

  // Verify SCADA connection fails with empty URL
  void testConnectToSCADAEmptyUrl() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ScadaConfig cfg;
    cfg.username = QStringLiteral("admin");
    QVERIFY(!svc.connectToSCADA(cfg));
  }

  // Verify valid MES config connects successfully
  void testConnectToMESValid() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    MesConfig cfg;
    cfg.endpoint = QStringLiteral("http://mes.local/api");
    cfg.apiKey = QStringLiteral("key-123");
    QVERIFY(svc.connectToMES(cfg));
  }

  // Verify MES connection fails with empty endpoint
  void testConnectToMESEmptyEndpoint() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    MesConfig cfg;
    cfg.apiKey = QStringLiteral("key-123");
    QVERIFY(!svc.connectToMES(cfg));
  }

  // Verify valid ERP config connects successfully
  void testConnectToERPValid() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ErpConfig cfg;
    cfg.host = QStringLiteral("erp.local");
    cfg.database = QStringLiteral("production");
    QVERIFY(svc.connectToERP(cfg));
  }

  // Verify ERP connection fails with empty host
  void testConnectToERPEmptyHost() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ErpConfig cfg;
    cfg.database = QStringLiteral("production");
    QVERIFY(!svc.connectToERP(cfg));
  }

  // Verify sync with non-empty ID succeeds
  void testSyncDataNonEmpty() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QVERIFY(svc.syncData(QStringLiteral("plc-01")));
  }

  // Verify sync with empty ID fails
  void testSyncDataEmpty() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QVERIFY(!svc.syncData(QString()));
  }

  // Verify connectedToSystem signal is emitted on PLC connect
  void testConnectedToSystemSignal() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATIntegrationService::connectedToSystem);
    PlcConfig cfg;
    cfg.ipAddress = QStringLiteral("192.168.1.1");
    cfg.port = 502;
    svc.connectToPLC(cfg);
    QCOMPARE(spy.count(), 1);
  }

  // Verify dataSynced signal is emitted on sync
  void testDataSyncedSignal() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATIntegrationService::dataSynced);
    svc.syncData(QStringLiteral("plc-01"));
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(EtherCATIntegrationServiceTest)
#include "ethercat_integration_service_test.moc"
