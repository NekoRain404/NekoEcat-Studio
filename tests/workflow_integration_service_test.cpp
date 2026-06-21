// WorkflowIntegrationServiceTest — Tests for Workflow Integration Service
//
// Test coverage:
//   - CI server integration (connect, signal emission)
//   - Issue tracker integration
//   - Communication platform integration (Slack)
//   - Documentation platform integration (Confluence)
//   - Invalid server URL handling
//   - Empty token and server validation

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowIntegrationService.h"

class WorkflowIntegrationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Connect to CI server and verify signal and config
  void testIntegrateWithCI() {
    WorkflowIntegrationService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationService::integrationConnected);
    CIConfig config;
    config.server = "https://ci.example.com";
    config.token = "test-token-123";
    config.project = "my-project";
    QVERIFY(svc.integrateWithCI(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "ci");
    QCOMPARE(svc.ciConfig().server, "https://ci.example.com");
    QCOMPARE(svc.ciConfig().token, "test-token-123");
  }

  // Connect to issue tracker and verify signal
  void testIntegrateWithIssueTracker() {
    WorkflowIntegrationService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationService::integrationConnected);
    IssueTrackerConfig config;
    config.server = "https://jira.example.com";
    config.token = "jira-token";
    config.project = "PROJ";
    QVERIFY(svc.integrateWithIssueTracker(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "issue_tracker");
  }

  // Connect to communication platform and verify signal
  void testIntegrateWithCommunication() {
    WorkflowIntegrationService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationService::integrationConnected);
    CommunicationConfig config;
    config.server = "https://slack.example.com";
    config.token = "slack-token";
    config.channel = "#general";
    QVERIFY(svc.integrateWithCommunication(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "communication");
  }

  // Connect to documentation platform and verify signal
  void testIntegrateWithDocumentation() {
    WorkflowIntegrationService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationService::integrationConnected);
    DocumentationConfig config;
    config.server = "https://confluence.example.com";
    config.token = "confluence-token";
    config.space = "DOCS";
    QVERIFY(svc.integrateWithDocumentation(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "documentation");
  }

  // Reject invalid server URL and emit error signal
  void testInvalidServer() {
    WorkflowIntegrationService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationService::integrationError);
    CIConfig config;
    config.server = "not-a-url";
    config.token = "token";
    QVERIFY(!svc.integrateWithCI(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), "ci");
  }

  // Reject empty token and emit error signal
  void testEmptyToken() {
    WorkflowIntegrationService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationService::integrationError);
    IssueTrackerConfig config;
    config.server = "https://jira.example.com";
    config.token = "";
    QVERIFY(!svc.integrateWithIssueTracker(config));
    QCOMPARE(spy.count(), 1);
  }

  // Reject empty server and emit error signal
  void testEmptyServer() {
    WorkflowIntegrationService svc;
    QSignalSpy spy(&svc, &WorkflowIntegrationService::integrationError);
    CommunicationConfig config;
    config.server = "";
    config.token = "token";
    QVERIFY(!svc.integrateWithCommunication(config));
    QCOMPARE(spy.count(), 1);
  }

  void testConfigPersistence() {
    WorkflowIntegrationService svc;
    CIConfig ciConfig;
    ciConfig.server = "https://ci.example.com";
    ciConfig.token = "ci-token";
    svc.integrateWithCI(ciConfig);
    QCOMPARE(svc.ciConfig().server, "https://ci.example.com");

    DocumentationConfig docConfig;
    docConfig.server = "https://docs.example.com";
    docConfig.token = "doc-token";
    svc.integrateWithDocumentation(docConfig);
    QCOMPARE(svc.documentationConfig().server, "https://docs.example.com");
    QCOMPARE(svc.ciConfig().server, "https://ci.example.com");
  }
};

QTEST_MAIN(WorkflowIntegrationServiceTest)
#include "workflow_integration_service_test.moc"
