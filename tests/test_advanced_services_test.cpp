// EtherCATAdvancedServicesTest — Tests for advanced EtherCAT services
//
// Test coverage:
//   - Integration service: PLC, SCADA, MES, ERP connections
//   - Integration service: data synchronization
//   - Sync service: time, data, state, and configuration sync
//   - Replication service: configuration, data, state, and backup replication

#include <QTest>
#include <QSignalSpy>
#include "EtherCATIntegrationService.h"
#include "EtherCATSyncService.h"
#include "EtherCATReplicationService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

class EtherCATAdvancedServicesTest : public QObject {
  Q_OBJECT
private slots:
  // Connect to PLC with valid config
  void testIntegrationConnectPLC() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    QSignalSpy spy(&svc, &EtherCATIntegrationService::connectedToSystem);
    PlcConfig cfg;
    cfg.ipAddress = QStringLiteral("192.168.1.1");
    cfg.port = 502;
    cfg.protocol = QStringLiteral("ModbusTCP");
    cfg.timeout = 3000;
    QVERIFY(svc.connectToPLC(cfg));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("PLC"));
  }

  // Connect to PLC with empty config fails
  void testIntegrationConnectPLCInvalid() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    PlcConfig cfg;
    QVERIFY(!svc.connectToPLC(cfg));
  }

  // Connect to SCADA with valid config
  void testIntegrationConnectSCADA() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    QSignalSpy spy(&svc, &EtherCATIntegrationService::connectedToSystem);
    ScadaConfig cfg;
    cfg.serverUrl = QStringLiteral("http://scada.local");
    cfg.username = QStringLiteral("admin");
    cfg.password = QStringLiteral("pass");
    QVERIFY(svc.connectToSCADA(cfg));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("SCADA"));
  }

  // Connect to SCADA with empty config fails
  void testIntegrationConnectSCADAInvalid() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    ScadaConfig cfg;
    QVERIFY(!svc.connectToSCADA(cfg));
  }

  // Connect to MES with valid config
  void testIntegrationConnectMES() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    QSignalSpy spy(&svc, &EtherCATIntegrationService::connectedToSystem);
    MesConfig cfg;
    cfg.endpoint = QStringLiteral("http://mes.local/api");
    cfg.apiKey = QStringLiteral("key123");
    cfg.version = QStringLiteral("v2");
    QVERIFY(svc.connectToMES(cfg));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("MES"));
  }

  // Connect to MES with empty config fails
  void testIntegrationConnectMESInvalid() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    MesConfig cfg;
    QVERIFY(!svc.connectToMES(cfg));
  }

  // Connect to ERP with valid config
  void testIntegrationConnectERP() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    QSignalSpy spy(&svc, &EtherCATIntegrationService::connectedToSystem);
    ErpConfig cfg;
    cfg.host = QStringLiteral("erp.local");
    cfg.database = QStringLiteral("production");
    cfg.credentials = QStringLiteral("token");
    QVERIFY(svc.connectToERP(cfg));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("ERP"));
  }

  // Connect to ERP with empty config fails
  void testIntegrationConnectERPInvalid() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    ErpConfig cfg;
    QVERIFY(!svc.connectToERP(cfg));
  }

  // Sync data to connected system
  void testIntegrationSyncData() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    QSignalSpy spy(&svc, &EtherCATIntegrationService::dataSynced);
    QVERIFY(svc.syncData(QStringLiteral("PLC")));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("PLC"));
  }

  // Sync data with empty system name fails
  void testIntegrationSyncDataEmpty() {
    EcatClient client;
    EventBus bus;
    EtherCATIntegrationService svc(&bus, &client);

    QVERIFY(!svc.syncData(QString()));
  }

  // Sync time fails closed without a live backend.
  void testSyncTime() {
    EcatClient client;
    EventBus bus;
    EtherCATSyncService svc(&bus, &client);

    QSignalSpy spy(&svc, &EtherCATSyncService::timeSynced);
    QVERIFY(!svc.syncTime());
    QCOMPARE(spy.count(), 0);
    SyncStatus st = svc.syncStatus();
    QCOMPARE(st.syncCount, 0);
  }

  // Sync data fails closed without a live backend.
  void testSyncData() {
    EcatClient client;
    EventBus bus;
    EtherCATSyncService svc(&bus, &client);

    QSignalSpy spy(&svc, &EtherCATSyncService::dataSynced);
    QVERIFY(!svc.syncData());
    QCOMPARE(spy.count(), 0);
  }

  // Sync state fails closed without a live backend.
  void testSyncState() {
    EcatClient client;
    EventBus bus;
    EtherCATSyncService svc(&bus, &client);

    QVERIFY(!svc.syncState());
    SyncStatus st = svc.syncStatus();
    QCOMPARE(st.syncCount, 0);
  }

  // Sync configuration fails closed without a live backend.
  void testSyncConfiguration() {
    EcatClient client;
    EventBus bus;
    EtherCATSyncService svc(&bus, &client);

    QVERIFY(!svc.syncConfiguration());
    SyncStatus st = svc.syncStatus();
    QCOMPARE(st.syncCount, 0);
  }

  // Sync status does not accumulate rejected offline syncs.
  void testSyncStatusAccumulates() {
    EcatClient client;
    EventBus bus;
    EtherCATSyncService svc(&bus, &client);

    svc.syncTime();
    svc.syncData();
    svc.syncState();
    SyncStatus st = svc.syncStatus();
    QCOMPARE(st.syncCount, 0);
    QVERIFY(!st.lastSync.isValid());
  }

  // Replicate configuration fails closed without a live backend.
  void testReplicateConfiguration() {
    EcatClient client;
    EventBus bus;
    EtherCATReplicationService svc(&bus, &client);

    QSignalSpy startSpy(&svc, &EtherCATReplicationService::replicationStarted);
    QSignalSpy doneSpy(&svc, &EtherCATReplicationService::replicationCompleted);
    QStringList targets = {QStringLiteral("node1"), QStringLiteral("node2")};
    QVERIFY(!svc.replicateConfiguration(targets));
    QCOMPARE(startSpy.count(), 0);
    QCOMPARE(doneSpy.count(), 0);
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Replicate data fails closed without a live backend.
  void testReplicateData() {
    EcatClient client;
    EventBus bus;
    EtherCATReplicationService svc(&bus, &client);

    QSignalSpy doneSpy(&svc, &EtherCATReplicationService::replicationCompleted);
    QStringList targets = {QStringLiteral("node1")};
    QVERIFY(!svc.replicateData(targets));
    QCOMPARE(doneSpy.count(), 0);
  }

  // Replicate state fails closed without a live backend.
  void testReplicateState() {
    EcatClient client;
    EventBus bus;
    EtherCATReplicationService svc(&bus, &client);

    QStringList targets = {QStringLiteral("node1")};
    QVERIFY(!svc.replicateState(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Replicate backup fails closed without a live backend.
  void testReplicateBackup() {
    EcatClient client;
    EventBus bus;
    EtherCATReplicationService svc(&bus, &client);

    QStringList targets = {QStringLiteral("backup1")};
    QVERIFY(!svc.replicateBackup(targets));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }

  // Empty target list produces no replications
  void testReplicateEmptyTargets() {
    EcatClient client;
    EventBus bus;
    EtherCATReplicationService svc(&bus, &client);

    QVERIFY(svc.replicateConfiguration(QStringList()));
    QCOMPARE(svc.replicationHistory().size(), 0);
  }
};

QTEST_MAIN(EtherCATAdvancedServicesTest)
#include "test_advanced_services_test.moc"
