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
#include "infra/EcatClient.h"

class ServiceIntegrationTest : public QObject {
  Q_OBJECT
private slots:
  void testServiceCreation() {
    ServiceContainer sc;
    QVERIFY(sc.client() != nullptr);
    QVERIFY(sc.eventBus() != nullptr);
    QVERIFY(sc.sdo() != nullptr);
    QVERIFY(sc.watch() != nullptr);
    QVERIFY(sc.topology() != nullptr);
    QVERIFY(sc.dcSync() != nullptr);
    QVERIFY(sc.alEvent() != nullptr);
    QVERIFY(sc.signal() != nullptr);
  }

  void testServiceDependencies() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();
    SdoService *sdo = sc.sdo();
    WatchService *watch = sc.watch();
    TopologyService *topology = sc.topology();
    DcSyncService *dcSync = sc.dcSync();
    AlEventService *alEvent = sc.alEvent();
    SignalService *signal = sc.signal();

    QVERIFY(bus != nullptr);
    QVERIFY(sdo != nullptr);
    QVERIFY(watch != nullptr);
    QVERIFY(topology != nullptr);
    QVERIFY(dcSync != nullptr);
    QVERIFY(alEvent != nullptr);
    QVERIFY(signal != nullptr);
  }

  void testServiceConfiguration() {
    ServiceContainer sc;
    EcatClient *client = sc.client();
    QVERIFY(client != nullptr);

    // Test that the same client instance is returned consistently
    QVERIFY(sc.client() == client);
    ServiceContainer sc2;
    QVERIFY(sc2.client() != client);  // Different containers have different clients
  }

  void testServiceEventBusWiring() {
    ServiceContainer sc;
    EventBus *bus = sc.eventBus();

    QSignalSpy slaveSpy(bus, &EventBus::slaveChanged);
    QSignalSpy connectionSpy(bus, &EventBus::connectionStateChanged);

    QVERIFY(slaveSpy.isValid());
    QVERIFY(connectionSpy.isValid());
  }
};

QTEST_MAIN(ServiceIntegrationTest)
#include "service_integration_test.moc"
