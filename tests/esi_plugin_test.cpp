// EsiPluginTest — Tests for EsiService and EsiPlugin
//
// Test coverage:
//   - ESI service empty list and match
//   - Plugin identity and ordering
//   - Plugin widget and service accessor
//   - Import/export with invalid paths

#include <QTest>
#include "services/EsiService.h"
#include "plugins/esi/EsiPlugin.h"

class EsiPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify ESI service starts with empty device list
  void testServiceListEmpty() {
    EsiService svc;
    QCOMPARE(svc.deviceCount(), 0);
    QVERIFY(svc.listDevices().isEmpty());
  }

  // Verify matchDevice returns zeros for unknown vendor/product
  void testServiceMatchNoDevice() {
    EsiService svc;
    auto dev = svc.matchDevice(0x1234, 0x5678);
    QCOMPARE(dev.vendorId, 0);
    QCOMPARE(dev.productCode, 0);
  }

  // Verify clear resets device count to zero
  void testServiceClear() {
    EsiService svc;
    svc.clear();
    QCOMPARE(svc.deviceCount(), 0);
  }

  // Verify plugin id, display names (EN/ZH)
  void testPluginIdentity() {
    EsiService svc;
    EsiPlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("esi"));
    QCOMPARE(plugin.displayName(), QString("ESI Repository"));
    QCOMPARE(plugin.displayNameZh(), QStringLiteral("ESI 仓库"));
  }

  // Verify default ordering value
  void testPluginDefaultOrder() {
    EsiService svc;
    EsiPlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 90);
  }

  // Verify plugin is visible
  void testPluginVisible() {
    EsiService svc;
    EsiPlugin plugin(&svc);
    QVERIFY(plugin.visible());
  }

  // Verify main widget is created
  void testPluginWidgetNotNull() {
    EsiService svc;
    EsiPlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify service accessor returns injected service
  void testPluginServiceAccessor() {
    EsiService svc;
    EsiPlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  // Verify device list widget is created
  void testPluginDeviceListNotNull() {
    EsiService svc;
    EsiPlugin plugin(&svc);
    QVERIFY(plugin.deviceList() != nullptr);
  }

  // Verify detail table widget is created
  void testPluginDetailTableNotNull() {
    EsiService svc;
    EsiPlugin plugin(&svc);
    QVERIFY(plugin.detailTable() != nullptr);
  }

  // Verify import with invalid path fails gracefully
  void testServiceImportInvalidPath() {
    EsiService svc;
    bool result = svc.importEsi("/nonexistent/path/file.xml");
    QVERIFY(!result);
    QCOMPARE(svc.deviceCount(), 0);
  }

  // Verify export with invalid device fails gracefully
  void testServiceExportInvalidDevice() {
    EsiService svc;
    bool result = svc.exportEsi("dead:beef", "/tmp/output.xml");
    QVERIFY(!result);
  }
};

QTEST_MAIN(EsiPluginTest)
#include "esi_plugin_test.moc"
