// DiagnosticServiceTest — Tests for DiagnosticService
//
// Test coverage:
//   - System diagnostics
//   - Performance diagnostics
//   - Network diagnostics
//   - Device diagnostics
//   - Report without connection
//   - Timestamp and message validation
//   - Multiple diagnostics runs

// DiagnosticServiceTest — Tests for DiagnosticService
//
// Test coverage:
//   - System, performance, network, and device diagnostics
//   - Report status without connection
//   - Report timestamp, message, and item names
//   - Device diagnostics details with position

#include <QTest>
#include <QSignalSpy>
#include "services/DiagnosticService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class DiagnosticServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify system diagnostics returns correct category and items
  void testSystemDiagnostics() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    QSignalSpy spy(&svc, &DiagnosticService::diagnosticCompleted);
    auto report = svc.runSystemDiagnostics();
    QCOMPARE(report.category, DiagnosticCategory::System);
    QVERIFY(!report.items.isEmpty());
    QCOMPARE(spy.count(), 1);
  }

  // Verify performance diagnostics category and items
  // Verify performance diagnostics returns correct category
  void testPerformanceDiagnostics() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runPerformanceDiagnostics();
    QCOMPARE(report.category, DiagnosticCategory::Performance);
    QVERIFY(!report.items.isEmpty());
  }

  void testPerformanceDiagnosticsDoNotPassWithoutTelemetry() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runPerformanceDiagnostics();
    QVERIFY(report.status != DiagnosticStatus::Pass);
    QVERIFY(report.message.contains(QStringLiteral("warning"), Qt::CaseInsensitive) ||
            report.message.contains(QStringLiteral("failure"), Qt::CaseInsensitive));
  }

  // Verify network diagnostics category and items
  // Verify network diagnostics returns correct category
  void testNetworkDiagnostics() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runNetworkDiagnostics();
    QCOMPARE(report.category, DiagnosticCategory::Network);
    QVERIFY(!report.items.isEmpty());
  }

  // Verify device diagnostics category and items
  // Verify device diagnostics returns correct category
  void testDeviceDiagnostics() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runDeviceDiagnostics(0);
    QCOMPARE(report.category, DiagnosticCategory::Device);
    QVERIFY(!report.items.isEmpty());
  }

  void testDeviceDiagnosticsDoNotPassWithoutDeviceEvidence() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runDeviceDiagnostics(0);
    QVERIFY(report.status != DiagnosticStatus::Pass);
    QVERIFY(!report.recommendations.isEmpty());
  }

  // Verify system report fails without connection
  // Verify system report shows Fail status without connection
  void testSystemReportWithoutConnection() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runSystemDiagnostics();
    QCOMPARE(report.status, DiagnosticStatus::Fail);
    QVERIFY(!report.recommendations.isEmpty());
  }

  // Verify report timestamp is within expected range
  // Verify report timestamp is within expected range
  void testReportTimestamp() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto before = QDateTime::currentDateTime();
    auto report = svc.runSystemDiagnostics();
    auto after = QDateTime::currentDateTime();
    QVERIFY(report.timestamp >= before);
    QVERIFY(report.timestamp <= after);
  }

  // Verify report message is non-empty
  // Verify report message is not empty
  void testReportMessage() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runSystemDiagnostics();
    QVERIFY(!report.message.isEmpty());
  }

  // Verify all diagnostic item names are non-empty
  // Verify all diagnostic items have non-empty names
  void testItemNameNotEmpty() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runPerformanceDiagnostics();
    for (const auto &item : report.items) {
      QVERIFY(!item.name.isEmpty());
    }
  }

  // Verify device diagnostics include position detail
  // Verify device diagnostics includes position in details
  void testDeviceDiagnosticsDetails() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    auto report = svc.runDeviceDiagnostics(5);
    bool found = false;
    for (const auto &item : report.items) {
      if (item.details.contains("position")) {
        QCOMPARE(item.details["position"].toInt(), 5);
        found = true;
      }
    }
    QVERIFY(found);
  }

  // Verify multiple diagnostic types can run sequentially
  // Verify multiple diagnostic types can run sequentially
  void testMultipleDiagnostics() {
    EventBus bus;
    EcatClient client;
    DiagnosticService svc(&bus, &client);
    svc.runSystemDiagnostics();
    svc.runPerformanceDiagnostics();
    svc.runNetworkDiagnostics();
    svc.runDeviceDiagnostics(0);
  }
};

QTEST_MAIN(DiagnosticServiceTest)
#include "diagnostic_service_test.moc"
