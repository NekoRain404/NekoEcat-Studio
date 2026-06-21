// EtherCATDeploymentServiceTest — Tests for EtherCATDeploymentService
//
// Test coverage:
//   - Configuration deployment and rollback
//   - Deployment listing and status queries
//   - Position-based deployment (config, firmware, software, system)
//   - Deployment progress signals and unique ID generation

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATDeploymentService.h"

class EtherCATDeploymentServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Deploy configuration to target succeeds
  void testDeployConfiguration() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    auto result = svc.deployConfiguration("target-01", "config.xml");
    QCOMPARE(result.target, QStringLiteral("target-01"));
    QCOMPARE(result.config, QStringLiteral("config.xml"));
    QCOMPARE(result.status, QStringLiteral("Success"));
    QVERIFY(!result.id.isEmpty());
    QVERIFY(!result.timestamp.isEmpty());
  }

  // Rollback a deployment changes status
  void testRollbackDeployment() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    auto deploy = svc.deployConfiguration("target-01", "config.xml");
    auto rollback = svc.rollbackDeployment(deploy.id);
    QCOMPARE(rollback.status, QStringLiteral("RolledBack"));
    QCOMPARE(rollback.id, deploy.id);
  }

  // Rollback nonexistent deployment fails
  void testRollbackNonexistent() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    auto result = svc.rollbackDeployment("nonexistent");
    QCOMPARE(result.status, QStringLiteral("Failed"));
  }

  // List deployments returns all deployed items
  void testListDeployments() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    svc.deployConfiguration("target-01", "config-a.xml");
    svc.deployConfiguration("target-02", "config-b.xml");
    auto list = svc.listDeployments();
    QCOMPARE(list.size(), 2);
    QCOMPARE(list[0].target, QStringLiteral("target-01"));
    QCOMPARE(list[1].target, QStringLiteral("target-02"));
  }

  // Get deployment status by ID
  void testGetDeploymentStatus() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    auto deploy = svc.deployConfiguration("target-01", "config.xml");
    auto status = svc.getDeploymentStatus(deploy.id);
    QCOMPARE(status.status, QStringLiteral("Success"));
    QCOMPARE(status.target, QStringLiteral("target-01"));
  }

  // Get status for nonexistent deployment
  void testGetStatusNonexistent() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    auto status = svc.getDeploymentStatus("nonexistent");
    QCOMPARE(status.status, QStringLiteral("NotFound"));
  }

  // deploymentCompleted signal fires on deploy
  void testDeploymentSignal() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATDeploymentService::deploymentCompleted);
    svc.deployConfiguration("target-01", "config.xml");
    QCOMPARE(spy.count(), 1);
  }

  // deploymentCompleted signal fires on rollback
  void testRollbackSignal() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    auto deploy = svc.deployConfiguration("target-01", "config.xml");
    QSignalSpy spy(&svc, &EtherCATDeploymentService::deploymentCompleted);
    svc.rollbackDeployment(deploy.id);
    QCOMPARE(spy.count(), 1);
  }

  // Each deployment gets a unique ID
  void testUniqueIds() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    auto d1 = svc.deployConfiguration("target-01", "config.xml");
    auto d2 = svc.deployConfiguration("target-02", "config.xml");
    QVERIFY(d1.id != d2.id);
  }

  // Deploy config by position with progress signals
  void testDeployConfigByPosition() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    ConfigData d;
    d.configuration = QByteArray("cfg");
    d.version = "1.0";
    QSignalSpy startedSpy(&svc, &EtherCATDeploymentService::deploymentStarted);
    QSignalSpy completedSpy(&svc, &EtherCATDeploymentService::positionDeploymentCompleted);
    QVERIFY(svc.deployConfiguration(1, d));
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(completedSpy.at(0).at(1).toBool(), true);
  }

  // Deploy empty config by position fails
  void testDeployConfigByPositionEmpty() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    ConfigData d;
    QVERIFY(!svc.deployConfiguration(1, d));
  }

  // Deploy firmware by position
  void testDeployFirmwareByPosition() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    FirmwareData d;
    d.firmware = QByteArray("fw");
    d.version = "2.0";
    QSignalSpy spy(&svc, &EtherCATDeploymentService::positionDeploymentCompleted);
    QVERIFY(svc.deployFirmware(2, d));
    QCOMPARE(spy.count(), 1);
  }

  // Deploy empty firmware fails
  void testDeployFirmwareByPositionEmpty() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    FirmwareData d;
    QVERIFY(!svc.deployFirmware(1, d));
  }

  // Deploy software by position
  void testDeploySoftwareByPosition() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    SoftwareData d;
    d.software = QByteArray("sw");
    d.version = "3.0";
    QSignalSpy spy(&svc, &EtherCATDeploymentService::positionDeploymentCompleted);
    QVERIFY(svc.deploySoftware(3, d));
    QCOMPARE(spy.count(), 1);
  }

  // Deploy empty software fails
  void testDeploySoftwareByPositionEmpty() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    SoftwareData d;
    QVERIFY(!svc.deploySoftware(1, d));
  }

  // Deploy system by position
  void testDeploySystemByPosition() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    SystemData d;
    d.system = QByteArray("sys");
    d.version = "4.0";
    QSignalSpy spy(&svc, &EtherCATDeploymentService::positionDeploymentCompleted);
    QVERIFY(svc.deploySystem(4, d));
    QCOMPARE(spy.count(), 1);
  }

  // Deploy empty system fails
  void testDeploySystemByPositionEmpty() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    SystemData d;
    QVERIFY(!svc.deploySystem(1, d));
  }

  // Deployment progress emits 50% and 100% signals
  void testDeploymentProgressSignals() {
    EtherCATDeploymentService svc(nullptr, nullptr);
    ConfigData d;
    d.configuration = QByteArray("cfg");
    d.version = "1.0";
    QSignalSpy progressSpy(&svc, &EtherCATDeploymentService::deploymentProgress);
    svc.deployConfiguration(1, d);
    QCOMPARE(progressSpy.count(), 2);
    QCOMPARE(progressSpy.at(0).at(1).toInt(), 50);
    QCOMPARE(progressSpy.at(1).at(1).toInt(), 100);
  }
};

QTEST_MAIN(EtherCATDeploymentServiceTest)
#include "ethercat_deployment_service_test.moc"
