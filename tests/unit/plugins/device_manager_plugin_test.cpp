// DeviceManagerPluginTest — Tests for DeviceManagerPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Visibility and widget creation
//   - Activate/deactivate lifecycle
//   - Service accessor and device table

#include <QTest>
#include <QApplication>
#include "infra/EcatClient.h"
#include "services/DeviceManagerService.h"
#include "plugins/devicemanager/DeviceManagerPlugin.h"

class DeviceManagerPluginTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  DeviceManagerService *svc_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    svc_ = new DeviceManagerService(client_, this);
  }
  void cleanup() {
    delete svc_;
    svc_ = nullptr;
    delete client_;
    client_ = nullptr;
  }

  // Verify plugin id, display names (EN/ZH)
  void testIdentity() {
    DeviceManagerPlugin p(svc_);
    QCOMPARE(p.id(), QString("devicemanager"));
    QCOMPARE(p.displayName(), QString("Device Manager"));
    QVERIFY(!p.displayNameZh().isEmpty());
  }

  // Verify default order is positive
  void testDefaultOrder() {
    DeviceManagerPlugin p(svc_);
    QVERIFY(p.defaultOrder() > 0);
  }

  // Verify plugin is visible
  void testVisible() {
    DeviceManagerPlugin p(svc_);
    QVERIFY(p.visible());
  }

  // Verify main widget is created
  void testWidgetNotNull() {
    DeviceManagerPlugin p(svc_);
    QVERIFY(p.widget() != nullptr);
  }

  // Verify activate and deactivate lifecycle
  void testActivateDeactivate() {
    DeviceManagerPlugin p(svc_);
    p.activate();
    p.deactivate();
  }

  // Verify service accessor returns injected service
  void testServiceAccessor() {
    DeviceManagerPlugin p(svc_);
    QCOMPARE(p.service(), svc_);
  }

  // Verify device table widget is created
  void testDeviceTableNotNull() {
    DeviceManagerPlugin p(svc_);
    QVERIFY(p.deviceTable() != nullptr);
  }
};

QTEST_MAIN(DeviceManagerPluginTest)
#include "device_manager_plugin_test.moc"
