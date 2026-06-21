// MasterApiServiceTest — Tests for MasterApiService
//
// Test coverage:
//   - Initial master state (no slaves, no link)
//   - Master creation with signal
//   - Idempotent create and activate
//   - Activate before create failure
//   - Activate after create with state verification
//   - Deactivate with signal and state change
//   - Idempotent deactivate
//   - Slave config query (valid, invalid, negative position)
//   - Null client error handling
//   - Full lifecycle (create -> activate -> deactivate)

#include <QTest>
#include <QSignalSpy>
#include "services/MasterApiService.h"
#include "infra/EcatClient.h"

class MasterApiServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify default master state values
  void testInitialState() {
    EcatClient client;
    MasterApiService svc(&client);
    MasterApiState st = svc.masterState();
    QCOMPARE(st.slavesResponding, 0);
    QCOMPARE(st.alStates, 0);
    QVERIFY(!st.linkUp);
  }

  // Test creating master with signal verification
  void testCreateMaster() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterCreated);
    QVERIFY(svc.createMaster());
    QCOMPARE(spy.count(), 1);
  }

  // Verify create is idempotent
  void testCreateIdempotent() {
    EcatClient client;
    MasterApiService svc(&client);
    QVERIFY(svc.createMaster());
    QVERIFY(svc.createMaster());
  }

  // Verify activate fails before create
  void testActivateBeforeCreate() {
    EcatClient client;
    MasterApiService svc(&client);
    QVERIFY(!svc.activateMaster());
  }

  // Test activate after create with link state check
  void testActivateAfterCreate() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterActivated);
    svc.createMaster();
    QVERIFY(svc.activateMaster());
    QCOMPARE(spy.count(), 1);
    MasterApiState st = svc.masterState();
    QVERIFY(st.linkUp);
  }

  // Verify activate is idempotent
  void testActivateIdempotent() {
    EcatClient client;
    MasterApiService svc(&client);
    svc.createMaster();
    svc.activateMaster();
    QVERIFY(svc.activateMaster());
  }

  // Test deactivation with signal and state change
  void testDeactivate() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterDeactivated);
    svc.createMaster();
    svc.activateMaster();
    QVERIFY(svc.deactivateMaster());
    QCOMPARE(spy.count(), 1);
    MasterApiState st = svc.masterState();
    QVERIFY(!st.linkUp);
  }

  // Verify deactivate is idempotent
  void testDeactivateIdempotent() {
    EcatClient client;
    MasterApiService svc(&client);
    svc.deactivateMaster();
    QVERIFY(svc.deactivateMaster());
  }

  // Verify slave config invalid before create
  void testSlaveConfigBeforeCreate() {
    EcatClient client;
    MasterApiService svc(&client);
    SlaveApiConfig cfg = svc.slaveConfig(0);
    QVERIFY(!cfg.valid);
  }

  // Verify slave config valid after create
  void testSlaveConfigAfterCreate() {
    EcatClient client;
    MasterApiService svc(&client);
    svc.createMaster();
    SlaveApiConfig cfg = svc.slaveConfig(0);
    QVERIFY(cfg.valid);
    QCOMPARE(cfg.position, 0);
  }

  // Verify slave config invalid for negative position
  void testSlaveConfigNegativePosition() {
    EcatClient client;
    MasterApiService svc(&client);
    svc.createMaster();
    SlaveApiConfig cfg = svc.slaveConfig(-1);
    QVERIFY(!cfg.valid);
  }

  // Test error handling with null client
  void testCreateWithNullClient() {
    MasterApiService svc(nullptr);
    QSignalSpy spy(&svc, &MasterApiService::error);
    QVERIFY(!svc.createMaster());
    QCOMPARE(spy.count(), 1);
  }

  // Test full create/activate/deactivate lifecycle
  void testFullLifecycle() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy createdSpy(&svc, &MasterApiService::masterCreated);
    QSignalSpy activatedSpy(&svc, &MasterApiService::masterActivated);
    QSignalSpy deactivatedSpy(&svc, &MasterApiService::masterDeactivated);

    svc.createMaster();
    svc.activateMaster();
    QCOMPARE(svc.masterState().linkUp, true);
    svc.deactivateMaster();
    QCOMPARE(svc.masterState().linkUp, false);

    QCOMPARE(createdSpy.count(), 1);
    QCOMPARE(activatedSpy.count(), 1);
    QCOMPARE(deactivatedSpy.count(), 1);
  }
};

QTEST_MAIN(MasterApiServiceTest)
#include "master_api_service_test.moc"
