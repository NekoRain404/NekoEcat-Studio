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
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QRegularExpression>
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

  bool connectToFakeDaemon(QTcpServer &server) {
    if (!server.listen(QHostAddress::LocalHost, 0)) return false;

    QSignalSpy clientConnected(client_, &EcatClient::connected);
    client_->connectToHost(QHostAddress::LocalHost, server.serverPort());

    if (!server.waitForNewConnection(1000)) return false;
    QTcpSocket *socket = server.nextPendingConnection();
    if (!socket) return false;
    socket->setParent(&server);

    if (clientConnected.isEmpty()) {
      clientConnected.wait(1000);
    }
    return client_->isConnected();
  }

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

  void testConfigureDoesNotSucceedWithoutBackendAck() {
    QTcpServer server;
    QVERIFY(connectToFakeDaemon(server));

    MasterMgrConfig config;
    config.adapterName = "eth0";
    QVERIFY(!masterSvc_->configureMaster(config));
    QVERIFY(masterSvc_->masterInfo().adapterName != config.adapterName);
    QCOMPARE(masterSvc_->masterState(), MasterMgrState::Idle);
  }

  void testDiagnosticDoesNotPassWithoutMasterEvidence() {
    QTcpServer server;
    QVERIFY(connectToFakeDaemon(server));

    MasterMgrDiagnosticResult result = masterSvc_->diagnoseMaster();
    QVERIFY(!result.success);
    QVERIFY(result.summary.contains(QStringLiteral("evidence"),
                                    Qt::CaseInsensitive));
  }

  void testRestartDoesNotSucceedWithoutBackendAck() {
    QTcpServer server;
    QVERIFY(connectToFakeDaemon(server));

    QVERIFY(!masterSvc_->restartMaster());
    QCOMPARE(masterSvc_->masterState(), MasterMgrState::Idle);
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

  void testDcConfigureDoesNotSucceedWithoutBackendAck() {
    QTcpServer server;
    QVERIFY(connectToFakeDaemon(server));

    QSignalSpy spy(dcSvc_, &DistributedClockService::syncChanged);
    QVERIFY(!dcSvc_->configureSync(0, 1000, 0));
    QCOMPARE(spy.count(), 0);
  }

  void testSourceDoesNotContainSyntheticMasterSuccess() {
    QFile file(QStringLiteral(SOURCE_ROOT
                              "/apps/ecat-studio/services/MasterManagerService.cpp"));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("result.success = true")),
             "Master diagnostics must not synthesize a passing result.");
    QVERIFY2(!source.contains(QStringLiteral("Master operational")),
             "Master diagnostics must not report operational without evidence.");
    QVERIFY2(!source.contains(QRegularExpression(
                 QStringLiteral(R"(client_->setAdapter\s*\()"))),
             "Master configuration must not claim local setAdapter as success.");
    QVERIFY2(!source.contains(QRegularExpression(
                 QStringLiteral(R"(client_->rescan\s*\(\s*\))"))),
             "Master restart must not claim rescan request as restart success.");
  }

  void testSourceDoesNotContainSyntheticDcConfigSuccess() {
    QFile file(QStringLiteral(
        SOURCE_ROOT "/apps/ecat-studio/services/DistributedClockService.cpp"));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("emit syncChanged")),
             "DC syncChanged must not be emitted without backend acknowledgement.");
    QVERIFY2(!source.contains(QRegularExpression(
                 QStringLiteral(R"(configureSync[\s\S]*return\s+true\s*;)"))),
             "DC configureSync must not return true without backend acknowledgement.");
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
