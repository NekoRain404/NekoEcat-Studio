// MasterApiServiceTest — Tests for MasterApiService
//
// Test coverage:
//   - Initial master state (no slaves, no link)
//   - Master creation fails closed without backend confirmation
//   - Repeated create/activate attempts do not synthesize lifecycle success
//   - Activate before create failure
//   - Activation remains closed after rejected create
//   - Deactivate without active master is a no-op
//   - Idempotent deactivate
//   - Slave config query (valid, invalid, negative position)
//   - Null client error handling
//   - Rejected lifecycle does not emit success signals

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

  // Master creation must not be synthesized without backend confirmation.
  void testCreateMasterFailsClosedWithoutBackend() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterCreated);
    QVERIFY(!svc.createMaster());
    QCOMPARE(spy.count(), 0);
    QVERIFY(!svc.masterState().linkUp);
  }

  // Verify repeated create attempts do not create a local-only master.
  void testRepeatedCreateDoesNotSynthesizeMaster() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterCreated);
    QVERIFY(!svc.createMaster());
    QVERIFY(!svc.createMaster());
    QCOMPARE(spy.count(), 0);
  }

  // Verify activate fails before create
  void testActivateBeforeCreate() {
    EcatClient client;
    MasterApiService svc(&client);
    QVERIFY(!svc.activateMaster());
  }

  // Activation must remain closed after backend creation is rejected.
  void testActivateAfterRejectedCreateFailsClosed() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterActivated);
    QVERIFY(!svc.createMaster());
    QVERIFY(!svc.activateMaster());
    QCOMPARE(spy.count(), 0);
    MasterApiState st = svc.masterState();
    QVERIFY(!st.linkUp);
  }

  // Verify repeated activate attempts do not report success offline.
  void testRepeatedActivateFailsClosedWithoutBackend() {
    EcatClient client;
    MasterApiService svc(&client);
    QVERIFY(!svc.createMaster());
    QVERIFY(!svc.activateMaster());
    QVERIFY(!svc.activateMaster());
  }

  // Deactivation without an active master is an idempotent no-op.
  void testDeactivateWithoutActiveMasterIsNoOp() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterDeactivated);
    QVERIFY(svc.deactivateMaster());
    QCOMPARE(spy.count(), 0);
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

  // Rejected create must not unlock local-only slave configuration.
  void testSlaveConfigAfterRejectedCreateIsInvalid() {
    EcatClient client;
    MasterApiService svc(&client);
    QVERIFY(!svc.createMaster());
    SlaveApiConfig cfg = svc.slaveConfig(0);
    QVERIFY(!cfg.valid);
  }

  // Verify slave config invalid for negative position
  void testSlaveConfigNegativePosition() {
    EcatClient client;
    MasterApiService svc(&client);
    QVERIFY(!svc.createMaster());
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

  // Rejected lifecycle must not emit success signals or set linkUp locally.
  void testRejectedLifecycleDoesNotEmitSuccessSignals() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy createdSpy(&svc, &MasterApiService::masterCreated);
    QSignalSpy activatedSpy(&svc, &MasterApiService::masterActivated);
    QSignalSpy deactivatedSpy(&svc, &MasterApiService::masterDeactivated);

    QVERIFY(!svc.createMaster());
    QVERIFY(!svc.activateMaster());
    QCOMPARE(svc.masterState().linkUp, false);
    QVERIFY(svc.deactivateMaster());
    QCOMPARE(svc.masterState().linkUp, false);

    QCOMPARE(createdSpy.count(), 0);
    QCOMPARE(activatedSpy.count(), 0);
    QCOMPARE(deactivatedSpy.count(), 0);
  }
};

QTEST_MAIN(MasterApiServiceTest)
#include "master_api_service_test.moc"
