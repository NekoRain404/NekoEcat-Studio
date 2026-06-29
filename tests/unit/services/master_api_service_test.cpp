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
#include <QFile>
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

  // Deactivation without an active backend-confirmed master must fail closed.
  void testDeactivateWithoutActiveMasterFailsClosed() {
    EcatClient client;
    MasterApiService svc(&client);
    QSignalSpy spy(&svc, &MasterApiService::masterDeactivated);
    QVERIFY(!svc.deactivateMaster());
    QCOMPARE(spy.count(), 0);
    MasterApiState st = svc.masterState();
    QVERIFY(!st.linkUp);
  }

  // Verify repeated deactivate attempts do not report local-only success.
  void testRepeatedDeactivateFailsClosed() {
    EcatClient client;
    MasterApiService svc(&client);
    QVERIFY(!svc.deactivateMaster());
    QVERIFY(!svc.deactivateMaster());
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
    QVERIFY(!svc.deactivateMaster());
    QCOMPARE(svc.masterState().linkUp, false);

    QCOMPARE(createdSpy.count(), 0);
    QCOMPARE(activatedSpy.count(), 0);
    QCOMPARE(deactivatedSpy.count(), 0);
  }

  // Source-level guard against reintroducing local lifecycle success.
  void testSourceDoesNotContainSyntheticLifecycleSuccess() {
    QFile file(QStringLiteral(SOURCE_ROOT
                              "/apps/ecat-studio/services/MasterApiService.cpp"));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("if (created_) return true")),
             "createMaster must not treat local created_ state as backend success.");
    QVERIFY2(!source.contains(QStringLiteral("if (active_) return true")),
             "activateMaster must not treat local active_ state as backend success.");
    QVERIFY2(!source.contains(QStringLiteral("if (!active_) return true")),
             "deactivateMaster must not treat inactive state as backend success.");
    QVERIFY2(!source.contains(QStringLiteral("cfg.valid = true")),
             "slaveConfig must not synthesize valid configs without backend data.");
  }
};

QTEST_MAIN(MasterApiServiceTest)
#include "master_api_service_test.moc"
