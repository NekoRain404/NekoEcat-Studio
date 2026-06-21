// WorkflowDeploymentServiceTest — Tests for Workflow Deployment Service
//
// Test coverage:
//   - Configuration deployment with valid data
//   - Firmware deployment with valid data
//   - Software deployment with valid data
//   - System deployment with valid data
//   - Empty payload rejection
//   - Empty version rejection
//   - Checksum validation
//   - Signal emissions (started, progress, completed)

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowDeploymentService.h"

class WorkflowDeploymentServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testDeployConfiguration() {
      WorkflowDeploymentService svc;
      QSignalSpy startedSpy(&svc, &WorkflowDeploymentService::deploymentStarted);
      QSignalSpy progressSpy(&svc, &WorkflowDeploymentService::deploymentProgress);
      QSignalSpy completedSpy(&svc, &WorkflowDeploymentService::deploymentCompleted);

      WfConfigData data;
      data.configuration = QByteArray("config_data");
      data.version = QStringLiteral("1.0.0");

      QVERIFY(svc.deployConfiguration(1, data));
      QCOMPARE(startedSpy.count(), 1);
      QCOMPARE(progressSpy.count(), 2);
      QCOMPARE(completedSpy.count(), 1);
      QCOMPARE(startedSpy.at(0).at(0).toInt(), 1);
      QCOMPARE(startedSpy.at(0).at(1).toString(), QString("configuration"));
      QCOMPARE(completedSpy.at(0).at(1).toBool(), true);
  }

  void testDeployFirmware() {
      WorkflowDeploymentService svc;
      QSignalSpy spy(&svc, &WorkflowDeploymentService::deploymentCompleted);

      WfFirmwareData data;
      data.firmware = QByteArray("firmware_bin");
      data.version = QStringLiteral("2.0.0");

      QVERIFY(svc.deployFirmware(2, data));
      QCOMPARE(spy.count(), 1);
      QCOMPARE(spy.at(0).at(0).toInt(), 2);
  }

  void testDeploySoftware() {
      WorkflowDeploymentService svc;
      QSignalSpy spy(&svc, &WorkflowDeploymentService::deploymentStarted);

      WfSoftwareData data;
      data.software = QByteArray("software_pkg");
      data.version = QStringLiteral("3.0.0");

      QVERIFY(svc.deploySoftware(3, data));
      QCOMPARE(spy.count(), 1);
      QCOMPARE(spy.at(0).at(1).toString(), QString("software"));
  }

  void testDeploySystem() {
      WorkflowDeploymentService svc;
      QSignalSpy spy(&svc, &WorkflowDeploymentService::deploymentCompleted);

      WfSystemData data;
      data.system = QByteArray("system_img");
      data.version = QStringLiteral("4.0.0");

      QVERIFY(svc.deploySystem(4, data));
      QCOMPARE(spy.count(), 1);
      QVERIFY(spy.at(0).at(1).toBool());
  }

  void testEmptyConfigurationReturnsFalse() {
      WorkflowDeploymentService svc;
      WfConfigData data;
      data.version = QStringLiteral("1.0.0");
      QVERIFY(!svc.deployConfiguration(1, data));
  }

  void testEmptyFirmwareReturnsFalse() {
      WorkflowDeploymentService svc;
      WfFirmwareData data;
      data.version = QStringLiteral("1.0.0");
      QVERIFY(!svc.deployFirmware(1, data));
  }

  void testEmptySoftwareReturnsFalse() {
      WorkflowDeploymentService svc;
      WfSoftwareData data;
      data.version = QStringLiteral("1.0.0");
      QVERIFY(!svc.deploySoftware(1, data));
  }

  void testEmptySystemReturnsFalse() {
      WorkflowDeploymentService svc;
      WfSystemData data;
      data.version = QStringLiteral("1.0.0");
      QVERIFY(!svc.deploySystem(1, data));
  }

  void testEmptyVersionReturnsFalse() {
      WorkflowDeploymentService svc;
      WfConfigData data;
      data.configuration = QByteArray("data");
      QVERIFY(!svc.deployConfiguration(1, data));
  }

  void testValidChecksum() {
      WorkflowDeploymentService svc;
      QSignalSpy spy(&svc, &WorkflowDeploymentService::deploymentCompleted);

      WfConfigData data;
      data.configuration = QByteArray("test");
      data.version = QStringLiteral("1.0.0");
      data.checksum = QStringLiteral("9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08");

      svc.deployConfiguration(1, data);
      QCOMPARE(spy.count(), 1);
  }

  void testProgressSignalValues() {
      WorkflowDeploymentService svc;
      QSignalSpy spy(&svc, &WorkflowDeploymentService::deploymentProgress);

      WfConfigData data;
      data.configuration = QByteArray("data");
      data.version = QStringLiteral("1.0.0");

      svc.deployConfiguration(5, data);
      QCOMPARE(spy.count(), 2);
      QCOMPARE(spy.at(0).at(1).toInt(), 50);
      QCOMPARE(spy.at(1).at(1).toInt(), 100);
  }
};

QTEST_MAIN(WorkflowDeploymentServiceTest)
#include "workflow_deployment_service_test.moc"
