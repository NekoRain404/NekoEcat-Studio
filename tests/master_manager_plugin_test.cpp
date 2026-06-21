// MasterManagerPluginTest — Tests for MasterManagerPlugin and services
//
// Test coverage:
//   - Plugin identity, order, visibility, and widget creation
//   - Master state, info, and diagnostic defaults
//   - Configure and restart when disconnected
//   - DC sync service defaults and configuration
//   - Master state changed signal behavior

#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include "infra/EcatClient.h"
#include "services/MasterManagerService.h"
#include "services/DistributedClockService.h"
#include "plugins/master/MasterManagerPlugin.h"

class MasterManagerPluginTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  MasterManagerService *masterSvc_ = nullptr;
  DistributedClockService *dcSvc_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    masterSvc_ = new MasterManagerService(client_, this);
    dcSvc_ = new DistributedClockService(client_, this);
  }
  void cleanup() {
    delete dcSvc_;
    dcSvc_ = nullptr;
    delete masterSvc_;
    masterSvc_ = nullptr;
    delete client_;
    client_ = nullptr;
  }

  // Verify plugin id, display names
  void testIdentity() {
    MasterManagerPlugin p(masterSvc_, dcSvc_);
    QCOMPARE(p.id(), QString("master"));
    QCOMPARE(p.displayName(), QString("Master Manager"));
    QCOMPARE(p.displayNameZh(), QString::fromUtf8("主站管理"));
  }

  // Plugin has expected default order
  // Verify default order is 155
  void testDefaultOrder() {
    MasterManagerPlugin p(masterSvc_, dcSvc_);
    QCOMPARE(p.defaultOrder(), 155);
  }

  // Plugin is visible by default
  // Verify plugin is visible
  void testVisible() {
    MasterManagerPlugin p(masterSvc_, dcSvc_);
    QVERIFY(p.visible());
  }

  // Widget is created and not null
  // Check widget is created
  void testWidgetNotNull() {
    MasterManagerPlugin p(masterSvc_, dcSvc_);
    QVERIFY(p.widget() != nullptr);
  }

  // Activate and deactivate lifecycle completes without error
  // Test activate and deactivate cycle
  void testActivateDeactivate() {
    MasterManagerPlugin p(masterSvc_, dcSvc_);
    p.activate();
    p.deactivate();
  }

  // Service accessors return injected instances
  // Verify service accessors return correct pointers
  void testServiceAccessors() {
    MasterManagerPlugin p(masterSvc_, dcSvc_);
    QCOMPARE(p.masterService(), masterSvc_);
    QCOMPARE(p.dcService(), dcSvc_);
  }

  // Master state defaults to Unknown
  // Verify default master state is Unknown
  void testMasterStateDefault() {
    QCOMPARE(masterSvc_->masterState(), MasterMgrState::Unknown);
  }

  // Master info has zero slave count by default
  // Verify default master info values
  void testMasterInfoDefault() {
    MasterMgrInfo info = masterSvc_->masterInfo();
    QCOMPARE(info.slaveCount, 0);
    QCOMPARE(info.masterState, MasterMgrState::Unknown);
  }

  // Diagnostic fails when disconnected
  // Test diagnostic fails when disconnected
  void testDiagnosticWhenDisconnected() {
    MasterMgrDiagnosticResult result = masterSvc_->diagnoseMaster();
    QVERIFY(!result.success);
    QVERIFY(!result.summary.isEmpty());
  }

  // Configure fails when disconnected
  // Test configure fails when disconnected
  void testConfigureWhenDisconnected() {
    MasterMgrConfig config;
    config.adapterName = "eth0";
    QVERIFY(!masterSvc_->configureMaster(config));
  }

  // Restart fails when disconnected
  // Test restart fails when disconnected
  void testRestartWhenDisconnected() {
    QVERIFY(!masterSvc_->restartMaster());
  }

  // DC service has correct default values
  // Verify DC service default values
  void testDcServiceDefaults() {
    QCOMPARE(dcSvc_->referenceClock(), -1);
    DriftStatus drift = dcSvc_->driftStatus();
    QCOMPARE(drift.slave, -1);
    JitterStats jitter = dcSvc_->jitterStatistics();
    QCOMPARE(jitter.sampleCount, 0);
  }

  // DC configure fails when disconnected
  // Test DC configure fails when disconnected
  void testDcConfigureWhenDisconnected() {
    QVERIFY(!dcSvc_->configureSync(0, 1000, 0));
  }

  // Master state changed signal not emitted on refresh when disconnected
  // Verify master state changed signal on refresh
  void testMasterStateChangedSignal() {
    QSignalSpy spy(masterSvc_, &MasterManagerService::masterStateChanged);
    masterSvc_->refresh();
    QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(MasterManagerPluginTest)
#include "master_manager_plugin_test.moc"
