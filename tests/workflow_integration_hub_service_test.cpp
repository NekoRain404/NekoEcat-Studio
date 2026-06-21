// WorkflowIntegrationHubServiceTest — Tests for WorkflowIntegrationHubService
//
// Test coverage:
//   - System, data, process, and service integration
//   - Signal emission for integration and data sync
//   - Validation of endpoints and required fields

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowIntegrationHubService.h"

class WorkflowIntegrationHubServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testIntegrateSystem() {
    WorkflowIntegrationHubService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationHubService::integrationConnected);
    SystemConfig config;
    config.name = "erp";
    config.type = "sap";
    config.endpoint = "https://erp.example.com";
    config.credentials = "token";
    QVERIFY(svc.integrateSystem(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "erp");
  }

  void testIntegrateData() {
    WorkflowIntegrationHubService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationHubService::dataSynced);
    DataConfig config;
    config.source = "db-source";
    config.destination = "db-dest";
    QVERIFY(svc.integrateData(config));
    QCOMPARE(spy.count(), 1);
  }

  void testIntegrateProcess() {
    WorkflowIntegrationHubService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationHubService::integrationConnected);
    ProcessConfig config;
    config.workflow = "order-process";
    QVERIFY(svc.integrateProcess(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "order-process");
  }

  void testIntegrateService() {
    WorkflowIntegrationHubService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationHubService::integrationConnected);
    ServiceConfig config;
    config.service = "payment";
    config.endpoint = "https://pay.example.com";
    config.protocol = "REST";
    config.timeout = 10;
    QVERIFY(svc.integrateService(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "payment");
  }

  void testInvalidSystemEndpoint() {
    WorkflowIntegrationHubService svc;
    SystemConfig config;
    config.name = "bad";
    config.endpoint = "ftp://bad";
    QVERIFY(!svc.integrateSystem(config));
  }

  void testEmptySystemName() {
    WorkflowIntegrationHubService svc;
    SystemConfig config;
    config.name = "";
    config.endpoint = "https://ok.com";
    QVERIFY(!svc.integrateSystem(config));
  }

  void testEmptyDataSource() {
    WorkflowIntegrationHubService svc;
    DataConfig config;
    config.source = "";
    config.destination = "dest";
    QVERIFY(!svc.integrateData(config));
  }

  void testEmptyProcessWorkflow() {
    WorkflowIntegrationHubService svc;
    ProcessConfig config;
    config.workflow = "";
    QVERIFY(!svc.integrateProcess(config));
  }

  void testInvalidServiceEndpoint() {
    WorkflowIntegrationHubService svc;
    ServiceConfig config;
    config.service = "svc";
    config.endpoint = "not-a-url";
    QVERIFY(!svc.integrateService(config));
  }
};

QTEST_MAIN(WorkflowIntegrationHubServiceTest)
#include "workflow_integration_hub_service_test.moc"
