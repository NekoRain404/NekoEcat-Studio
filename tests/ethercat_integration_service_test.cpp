// EtherCATIntegrationServiceTest — Tests for EtherCATIntegrationService
//
// Test coverage:
//   - PLC connection fails closed offline (valid, empty IP, zero port)
//   - SCADA connection fails closed offline (valid, empty URL)
//   - MES connection fails closed offline (valid, empty endpoint)
//   - ERP connection fails closed offline (valid, empty host)
//   - Data synchronization and success signals are not synthesized offline

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATIntegrationService.h"

class EtherCATIntegrationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify valid PLC config fails closed without a live integration backend.
  void testConnectToPLCFailsClosedWithoutBackend() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    PlcConfig cfg;
    cfg.ipAddress = QStringLiteral("192.168.1.1");
    cfg.port = 502;
    QVERIFY(!svc.connectToPLC(cfg));
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

  // Verify valid SCADA config fails closed without a live integration backend.
  void testConnectToSCADAFailsClosedWithoutBackend() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ScadaConfig cfg;
    cfg.serverUrl = QStringLiteral("http://scada.local");
    cfg.username = QStringLiteral("admin");
    QVERIFY(!svc.connectToSCADA(cfg));
  }

  // Verify SCADA connection fails with empty URL
  void testConnectToSCADAEmptyUrl() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ScadaConfig cfg;
    cfg.username = QStringLiteral("admin");
    QVERIFY(!svc.connectToSCADA(cfg));
  }

  // Verify valid MES config fails closed without a live integration backend.
  void testConnectToMESFailsClosedWithoutBackend() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    MesConfig cfg;
    cfg.endpoint = QStringLiteral("http://mes.local/api");
    cfg.apiKey = QStringLiteral("key-123");
    QVERIFY(!svc.connectToMES(cfg));
  }

  // Verify MES connection fails with empty endpoint
  void testConnectToMESEmptyEndpoint() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    MesConfig cfg;
    cfg.apiKey = QStringLiteral("key-123");
    QVERIFY(!svc.connectToMES(cfg));
  }

  // Verify valid ERP config fails closed without a live integration backend.
  void testConnectToERPFailsClosedWithoutBackend() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ErpConfig cfg;
    cfg.host = QStringLiteral("erp.local");
    cfg.database = QStringLiteral("production");
    QVERIFY(!svc.connectToERP(cfg));
  }

  // Verify ERP connection fails with empty host
  void testConnectToERPEmptyHost() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    ErpConfig cfg;
    cfg.database = QStringLiteral("production");
    QVERIFY(!svc.connectToERP(cfg));
  }

  // Verify sync with non-empty ID fails closed without a live integration backend.
  void testSyncDataFailsClosedWithoutBackend() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QVERIFY(!svc.syncData(QStringLiteral("plc-01")));
  }

  // Verify sync with empty ID fails
  void testSyncDataEmpty() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QVERIFY(!svc.syncData(QString()));
  }

  // Verify connectedToSystem is not emitted for offline rejection.
  void testConnectedToSystemSignalNotEmittedWithoutBackend() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATIntegrationService::connectedToSystem);
    PlcConfig cfg;
    cfg.ipAddress = QStringLiteral("192.168.1.1");
    cfg.port = 502;
    svc.connectToPLC(cfg);
    QCOMPARE(spy.count(), 0);
  }

  // Verify dataSynced is not emitted for offline rejection.
  void testDataSyncedSignalNotEmittedWithoutBackend() {
    EtherCATIntegrationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATIntegrationService::dataSynced);
    svc.syncData(QStringLiteral("plc-01"));
    QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(EtherCATIntegrationServiceTest)
#include "ethercat_integration_service_test.moc"
