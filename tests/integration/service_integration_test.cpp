// ServiceIntegrationTest — Tests for ServiceContainer integration
//
// Test coverage:
//   - All services are created by ServiceContainer
//   - Service dependency wiring
//   - Consistent client instance per container
//   - Event bus signal validity
//   - Safety controller state validation

#include <QTest>
#include <QSignalSpy>
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "services/SdoService.h"
#include "services/WatchService.h"
#include "services/TopologyService.h"
#include "services/DcSyncService.h"
#include "services/AlEventService.h"
#include "services/SignalService.h"
#include "services/WatchdogService.h"
#include "services/SafetyController.h"
#include "services/DiagnosticReportService.h"
#include "services/AlarmService.h"
#include "services/LoggingService.h"
#include "infra/EcatClient.h"

class ServiceIntegrationTest : public QObject {
  Q_OBJECT
private slots:
  // Verify all services are non-null from the container
  void testServiceCreation() {
    EcatClient cl;
    EventBus bus;
    ServiceContainer sc(&cl, &bus);
    QVERIFY(sc.client() != nullptr);
    QVERIFY(sc.eventBus() != nullptr);
    QVERIFY(sc.sdo() != nullptr);
    QVERIFY(sc.watch() != nullptr);
    QVERIFY(sc.topology() != nullptr);
    QVERIFY(sc.dcSync() != nullptr);
    QVERIFY(sc.alEvent() != nullptr);
    QVERIFY(sc.signal() != nullptr);
    QVERIFY(sc.watchdog() != nullptr);
    QVERIFY(sc.safety() != nullptr);
    QVERIFY(sc.diagnosticReport() != nullptr);
#ifdef ECAT_EXPERIMENTAL_SERVICES
    // AlarmService/LoggingService are only compiled when the experimental
    // services option is enabled (see apps/ecat-studio/CMakeLists.txt).
    QVERIFY(sc.alarm() != nullptr);
    QVERIFY(sc.logging() != nullptr);
#endif
  }

  // Verify service dependency instances are valid
  void testServiceDependencies() {
    EcatClient cl;
    EventBus bus;
    ServiceContainer sc(&cl, &bus);
    EventBus *eb = sc.eventBus();
    SdoService *sdo = sc.sdo();
    WatchService *watch = sc.watch();
    TopologyService *topology = sc.topology();
    DcSyncService *dcSync = sc.dcSync();
    AlEventService *alEvent = sc.alEvent();
    SignalService *signal = sc.signal();
    WatchdogService *watchdog = sc.watchdog();
    SafetyController *safety = sc.safety();
    DiagnosticReportService *diag = sc.diagnosticReport();

    QVERIFY(eb != nullptr);
    QVERIFY(sdo != nullptr);
    QVERIFY(watch != nullptr);
    QVERIFY(topology != nullptr);
    QVERIFY(dcSync != nullptr);
    QVERIFY(alEvent != nullptr);
    QVERIFY(signal != nullptr);
    QVERIFY(watchdog != nullptr);
    QVERIFY(safety != nullptr);
    QVERIFY(diag != nullptr);
  }

  // Verify consistent client instance and per-container isolation
  void testServiceConfiguration() {
    EcatClient cl;
    EventBus bus;
    ServiceContainer sc(&cl, &bus);
    EcatClient *client = sc.client();
    QVERIFY(client != nullptr);

    // Test that the same client instance is returned consistently
    QVERIFY(sc.client() == client);
    EcatClient cl2;
    EventBus bus2;
    ServiceContainer sc2(&cl2, &bus2);
    QVERIFY(sc2.client() != client);  // Different containers have different clients
  }

  // Verify event bus signals are valid
  void testServiceEventBusWiring() {
    EcatClient cl;
    EventBus localBus;
    ServiceContainer sc(&cl, &localBus);
    EventBus *bus = sc.eventBus();

    QSignalSpy slaveSpy(bus, &EventBus::slaveChanged);
    QSignalSpy connectionSpy(bus, &EventBus::connectionStateChanged);

    QVERIFY(slaveSpy.isValid());
    QVERIFY(connectionSpy.isValid());
  }

  // Test safety controller state transition validation
  void testSafetyControllerValidation() {
    EcatClient cl;
    EventBus bus;
    ServiceContainer sc(&cl, &bus);
    SafetyController *safety = sc.safety();
    QVERIFY(safety != nullptr);

    auto r = safety->validateStateTransition(8, 1);
    QCOMPARE(r.allowed, false);
    QVERIFY(r.reason.contains("OP"));

    auto r2 = safety->validateFreeRunStart(true);
    QCOMPARE(r2.allowed, true);
  }
};

QTEST_MAIN(ServiceIntegrationTest)
#include "service_integration_test.moc"
