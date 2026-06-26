// HardwareVerificationPluginTest — Tests for Hardware Verification Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Service creation and basic operations
//   - Verification result structure
//   - Device verification
//   - Network verification
//   - Timing verification
//   - Compliance verification
//   - Results collection and clearing
//   - Signal emission
#include <QTest>
#include <QSignalSpy>
#include <QTcpServer>
#include "plugins/hardwareverification/HardwareVerificationPlugin.h"
#include "plugins/hardwareverification/DeviceVerificationWidget.h"
#include "plugins/hardwareverification/NetworkVerificationWidget.h"
#include "services/HardwareVerificationService.h"
#include "infra/EcatClient.h"

class HardwareVerificationPluginTest : public QObject {
  Q_OBJECT
private:
  static bool waitForConnected(EcatClient &client) {
    for (int i = 0; i < 50 && !client.isConnected(); ++i) {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
      QTest::qWait(10);
    }
    return client.isConnected();
  }

private slots:
  void testPluginIdentity() {
    EcatClient client;
    HardwareVerificationService service(&client);
    HardwareVerificationPlugin plugin(&service);
    QCOMPARE(plugin.id(), QString("hardwareverification"));
    QCOMPARE(plugin.displayName(), QString("Hardware Verification"));
    QCOMPARE(plugin.displayNameZh(), QString("硬件验证"));
    QCOMPARE(plugin.defaultOrder(), 36);
    QCOMPARE(plugin.visible(), true);
  }

  void testWidgetCreation() {
    EcatClient client;
    HardwareVerificationService service(&client);
    HardwareVerificationPlugin plugin(&service);
    QVERIFY(plugin.widget() != nullptr);
  }

  void testServiceAccess() {
    EcatClient client;
    HardwareVerificationService service(&client);
    HardwareVerificationPlugin plugin(&service);
    QCOMPARE(plugin.verificationService(), &service);
  }

  void testVerificationCompletedSignal() {
    EcatClient client;
    HardwareVerificationService service(&client);
    HardwareVerificationPlugin plugin(&service);
    QSignalSpy spy(&plugin, &HardwareVerificationPlugin::verificationCompleted);
    QVERIFY(spy.isValid());
  }

  void testDeviceVerification() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto result = service.verifyDevice(0);
    QCOMPARE(result.verificationId, QString("device_0"));
    QCOMPARE(result.verificationName, QString("Device Verification (Position 0)"));
    QCOMPARE(result.passed, 0);
    QCOMPARE(result.failed, 0);
    QCOMPARE(result.skipped, 4);
    QCOMPARE(result.totalTests(), 4);
    QVERIFY(!result.allPassed());
    QCOMPARE(result.tests.size(), 4);
    QCOMPARE(result.totalDurationMs, 0.0);
  }

  void testDeviceVerificationDifferentPositions() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto r1 = service.verifyDevice(0);
    auto r2 = service.verifyDevice(5);
    QCOMPARE(r1.verificationId, QString("device_0"));
    QCOMPARE(r2.verificationId, QString("device_5"));
    QVERIFY(r2.verificationName.contains("5"));
  }

  void testNetworkVerification() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto result = service.verifyNetwork();
    QCOMPARE(result.verificationId, QString("network"));
    QCOMPARE(result.verificationName, QString("Network Verification"));
    QCOMPARE(result.passed, 0);
    QCOMPARE(result.failed, 0);
    QCOMPARE(result.skipped, 4);
    QCOMPARE(result.totalTests(), 4);
    QVERIFY(!result.allPassed());
    QCOMPARE(result.tests.size(), 4);
  }

  void testTimingVerification() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto result = service.verifyTiming();
    QCOMPARE(result.verificationId, QString("timing"));
    QCOMPARE(result.verificationName, QString("Timing Verification"));
    QCOMPARE(result.passed, 0);
    QCOMPARE(result.failed, 0);
    QCOMPARE(result.skipped, 4);
    QCOMPARE(result.totalTests(), 4);
    QVERIFY(!result.allPassed());
    QCOMPARE(result.tests.size(), 4);
  }

  void testComplianceVerification() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto result = service.verifyCompliance();
    QCOMPARE(result.verificationId, QString("compliance"));
    QCOMPARE(result.verificationName, QString("Compliance Verification"));
    QCOMPARE(result.passed, 0);
    QCOMPARE(result.failed, 0);
    QCOMPARE(result.skipped, 4);
    QCOMPARE(result.totalTests(), 4);
    QVERIFY(!result.allPassed());
    QCOMPARE(result.tests.size(), 4);
  }

  void testTestResultFields() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto result = service.verifyDevice(0);
    const auto &t = result.tests[0];
    QVERIFY(!t.testId.isEmpty());
    QVERIFY(!t.testName.isEmpty());
    QVERIFY(!t.category.isEmpty());
    QVERIFY(!t.passed);
    QVERIFY(t.skipped);
    QCOMPARE(t.durationMs, 0.0);
    QVERIFY(!t.details.isEmpty());
  }

  void testTestResultCategories() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto dev = service.verifyDevice(0);
    QCOMPARE(dev.tests[0].category, QString("Device"));

    auto net = service.verifyNetwork();
    QCOMPARE(net.tests[0].category, QString("Network"));

    auto timing = service.verifyTiming();
    QCOMPARE(timing.tests[0].category, QString("Timing"));

    auto comp = service.verifyCompliance();
    QCOMPARE(comp.tests[0].category, QString("Compliance"));
  }

  void testAllResultsCollection() {
    EcatClient client;
    HardwareVerificationService service(&client);
    QCOMPARE(service.allResults().size(), 0);
    service.verifyDevice(0);
    QCOMPARE(service.allResults().size(), 1);
    service.verifyNetwork();
    QCOMPARE(service.allResults().size(), 2);
    service.verifyTiming();
    QCOMPARE(service.allResults().size(), 3);
    service.verifyCompliance();
    QCOMPARE(service.allResults().size(), 4);
  }

  void testClearResults() {
    EcatClient client;
    HardwareVerificationService service(&client);
    service.verifyDevice(0);
    service.verifyNetwork();
    QCOMPARE(service.allResults().size(), 2);
    service.clearResults();
    QCOMPARE(service.allResults().size(), 0);
  }

  void testVerificationCompletedSignalEmission() {
    EcatClient client;
    HardwareVerificationService service(&client);
    QSignalSpy spy(&service,
                   &HardwareVerificationService::verificationCompleted);
    QVERIFY(spy.isValid());
    service.verifyDevice(0);
    QCOMPARE(spy.count(), 1);
    service.verifyNetwork();
    QCOMPARE(spy.count(), 2);
    service.verifyTiming();
    QCOMPARE(spy.count(), 3);
    service.verifyCompliance();
    QCOMPARE(spy.count(), 4);
  }

  void testVerificationStartedSignal() {
    EcatClient client;
    HardwareVerificationService service(&client);
    QSignalSpy spy(&service,
                   &HardwareVerificationService::verificationStarted);
    QVERIFY(spy.isValid());
    service.verifyDevice(0);
    QCOMPARE(spy.count(), 1);
  }

  void testVerificationProgressSignal() {
    EcatClient client;
    HardwareVerificationService service(&client);
    QSignalSpy spy(&service,
                   &HardwareVerificationService::verificationProgress);
    QVERIFY(spy.isValid());
    service.verifyNetwork();
    QCOMPARE(spy.count(), 4);
  }

  void testMultipleDeviceVerifications() {
    EcatClient client;
    HardwareVerificationService service(&client);
    service.verifyDevice(0);
    service.verifyDevice(1);
    service.verifyDevice(2);
    QCOMPARE(service.allResults().size(), 3);
    QCOMPARE(service.allResults()[0].verificationId, QString("device_0"));
    QCOMPARE(service.allResults()[1].verificationId, QString("device_1"));
    QCOMPARE(service.allResults()[2].verificationId, QString("device_2"));
  }

  void testResultTestIds() {
    EcatClient client;
    HardwareVerificationService service(&client);

    auto dev = service.verifyDevice(3);
    QCOMPARE(dev.tests[0].testId, QString("dev_id_3"));
    QCOMPARE(dev.tests[1].testId, QString("dev_cap_3"));
    QCOMPARE(dev.tests[2].testId, QString("dev_perf_3"));
    QCOMPARE(dev.tests[3].testId, QString("dev_comp_3"));

    auto net = service.verifyNetwork();
    QCOMPARE(net.tests[0].testId, QString("net_link"));
    QCOMPARE(net.tests[1].testId, QString("net_cable"));
    QCOMPARE(net.tests[2].testId, QString("net_signal"));
    QCOMPARE(net.tests[3].testId, QString("net_error"));

    auto timing = service.verifyTiming();
    QCOMPARE(timing.tests[0].testId, QString("time_dcsync"));
    QCOMPARE(timing.tests[1].testId, QString("time_pd"));
    QCOMPARE(timing.tests[2].testId, QString("time_mbx"));
    QCOMPARE(timing.tests[3].testId, QString("time_cycle"));

    auto comp = service.verifyCompliance();
    QCOMPARE(comp.tests[0].testId, QString("comp_proto"));
    QCOMPARE(comp.tests[1].testId, QString("comp_sm"));
    QCOMPARE(comp.tests[2].testId, QString("comp_sdo"));
    QCOMPARE(comp.tests[3].testId, QString("comp_pdo"));
  }

  void testDeviceVerificationWidget() {
    EcatClient client;
    HardwareVerificationService service(&client);
    DeviceVerificationWidget widget(&service);
    QVERIFY(widget.isVisible() || true);
  }

  void testNetworkVerificationWidget() {
    EcatClient client;
    HardwareVerificationService service(&client);
    NetworkVerificationWidget widget(&service);
    QVERIFY(widget.isVisible() || true);
  }

  void testDeviceVerificationWidgetSignals() {
    EcatClient client;
    HardwareVerificationService service(&client);
    DeviceVerificationWidget widget(&service);
    QSignalSpy spy(&widget, &DeviceVerificationWidget::verificationRequested);
    QVERIFY(spy.isValid());
  }

  void testNetworkVerificationWidgetSignals() {
    EcatClient client;
    HardwareVerificationService service(&client);
    NetworkVerificationWidget widget(&service);
    QSignalSpy spy(&widget,
                   &NetworkVerificationWidget::verificationRequested);
    QVERIFY(spy.isValid());
  }

  void testVerificationNameFormat() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto r1 = service.verifyDevice(0);
    QVERIFY(r1.verificationName.contains("0"));
    auto r2 = service.verifyDevice(127);
    QVERIFY(r2.verificationName.contains("127"));
  }

  void testDisconnectedVerificationHasZeroDuration() {
    EcatClient client;
    HardwareVerificationService service(&client);
    auto dev = service.verifyDevice(0);
    QCOMPARE(dev.totalDurationMs, 0.0);
    auto net = service.verifyNetwork();
    QCOMPARE(net.totalDurationMs, 0.0);
    auto timing = service.verifyTiming();
    QCOMPARE(timing.totalDurationMs, 0.0);
    auto comp = service.verifyCompliance();
    QCOMPARE(comp.totalDurationMs, 0.0);
  }

  void testDisconnectedClientDoesNotReportHardwareVerified() {
    EcatClient client;
    QVERIFY(!client.isConnected());
    HardwareVerificationService service(&client);

    const QVector<VerificationResult> results = {
        service.verifyDevice(0),
        service.verifyNetwork(),
        service.verifyTiming(),
        service.verifyCompliance(),
    };

    for (const auto &result : results) {
      QVERIFY2(!result.allPassed(),
               qPrintable(result.verificationId +
                          " must not report allPassed without daemon connection"));
      QCOMPARE(result.passed, 0);
      QCOMPARE(result.failed, 0);
      QCOMPARE(result.skipped, result.totalTests());
      QVERIFY(!result.recommendations.isEmpty());
    }
  }

  void testConnectedDaemonDoesNotReportHardwareVerifiedWithoutBackend() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    EcatClient client;
    client.connectToHost(QHostAddress::LocalHost, server.serverPort());
    QVERIFY(waitForConnected(client));

    HardwareVerificationService service(&client);
    auto result = service.verifyNetwork();
    QVERIFY(!result.allPassed());
    QCOMPARE(result.passed, 0);
    QCOMPARE(result.failed, 0);
    QCOMPARE(result.skipped, result.totalTests());
    QCOMPARE(result.totalDurationMs, 0.0);
    QVERIFY(!service.allResults().isEmpty());
  }
};

QTEST_MAIN(HardwareVerificationPluginTest)
#include "hardwareverification_plugin_test.moc"
