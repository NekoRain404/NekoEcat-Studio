// ExportPluginTest — Tests for ExportPlugin
//
// Test coverage:
//   - Plugin identity (id, display name, Chinese name)
//   - Default order and visibility
//   - Widget creation

#include <QTest>
#include <QApplication>

#include "plugins/export/ExportPlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class ExportPluginTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  ServiceContainer *container_ = nullptr;

private slots:
  // Set up service container for each test
  void init() {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
  }
  // Tear down service container after each test
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  // Verify plugin id, display name, and Chinese name
  void testIdentity() {
    ExportPlugin p(container_);
    QCOMPARE(p.id(), QString("export"));
    QCOMPARE(p.displayName(), QString("Export"));
    QCOMPARE(p.displayNameZh(), QString("导出"));
  }

  // Verify default order is 85
  void testDefaultOrder() {
    ExportPlugin p(container_);
    QCOMPARE(p.defaultOrder(), 85);
  }

  // Plugin is visible by default
  void testVisible() {
    ExportPlugin p(container_);
    QVERIFY(p.visible());
  }

  // Plugin widget is not null
  void testWidgetNotNull() {
    ExportPlugin p(container_);
    QVERIFY(p.widget() != nullptr);
  }
};

QTEST_MAIN(ExportPluginTest)
#include "export_plugin_test.moc"
