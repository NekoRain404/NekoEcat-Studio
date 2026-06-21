// WorkflowIntegrationHubServicePerformanceTest — Performance tests
//
// Test coverage:
//   - Bulk endpoint registration performance
//   - Bulk connection performance
//   - Query performance with many endpoints

#include <QTest>
#include <QElapsedTimer>
#include "services/WorkflowIntegrationHubService.h"

class WorkflowIntegrationHubServicePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testBulkRegistration() {
    WorkflowIntegrationHubService svc;
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 1000; ++i) {
      IntegrationEndpoint ep;
      ep.id = QStringLiteral("ep_%1").arg(i);
      ep.type = QStringLiteral("ci");
      ep.server = QStringLiteral("https://ci%1.example.com").arg(i);
      svc.registerEndpoint(ep);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(svc.endpointCount(), 1000);
    QVERIFY(elapsed < 100);
  }

  void testBulkConnection() {
    WorkflowIntegrationHubService svc;

    for (int i = 0; i < 100; ++i) {
      IntegrationEndpoint ep;
      ep.id = QStringLiteral("ep_%1").arg(i);
      ep.type = QStringLiteral("ci");
      ep.server = QStringLiteral("https://ci%1.example.com").arg(i);
      svc.registerEndpoint(ep);
    }

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 100; ++i) {
      svc.connectEndpoint(QStringLiteral("ep_%1").arg(i));
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(svc.connectedCount(), 100);
    QVERIFY(elapsed < 50);
  }

  void testQueryPerformance() {
    WorkflowIntegrationHubService svc;

    for (int i = 0; i < 500; ++i) {
      IntegrationEndpoint ep;
      ep.id = QStringLiteral("ep_%1").arg(i);
      ep.type = QStringLiteral("ci");
      ep.server = QStringLiteral("https://ci%1.example.com").arg(i);
      svc.registerEndpoint(ep);
    }

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 1000; ++i) {
      svc.endpoint(QStringLiteral("ep_%1").arg(i % 500));
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
  }
};

QTEST_MAIN(WorkflowIntegrationHubServicePerformanceTest)
#include "workflow_integration_hub_service_performance_test.moc"
