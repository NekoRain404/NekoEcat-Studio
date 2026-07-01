// DataPipelinePluginTest — Tests for DataPipelinePlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Visibility and widget creation
//   - Activate/deactivate lifecycle
//   - Service accessor and stage table

#include <QTest>
#include <QApplication>
#include "services/DataPipelineService.h"
#include "plugins/datapipeline/DataPipelinePlugin.h"

class DataPipelinePluginTest : public QObject {
  Q_OBJECT
private:
  DataPipelineService *svc_ = nullptr;

private slots:
  void init() { svc_ = new DataPipelineService(this); }
  void cleanup() {
    delete svc_;
    svc_ = nullptr;
  }

  // Verify plugin id, display names (EN/ZH)
  void testIdentity() {
    DataPipelinePlugin p(svc_);
    QCOMPARE(p.id(), QString("datapipeline"));
    QCOMPARE(p.displayName(), QString("Data Pipeline"));
    QVERIFY(!p.displayNameZh().isEmpty());
  }

  // Verify default order is positive
  void testDefaultOrder() {
    DataPipelinePlugin p(svc_);
    QVERIFY(p.defaultOrder() > 0);
  }

  // Verify plugin is visible
  void testVisible() {
    DataPipelinePlugin p(svc_);
    QVERIFY(!p.visible());
  }

  // Verify main widget is created
  void testWidgetNotNull() {
    DataPipelinePlugin p(svc_);
    QVERIFY(p.widget() != nullptr);
  }

  // Verify activate and deactivate lifecycle
  void testActivateDeactivate() {
    DataPipelinePlugin p(svc_);
    p.activate();
    p.deactivate();
  }

  // Verify service accessor returns injected service
  void testServiceAccessor() {
    DataPipelinePlugin p(svc_);
    QCOMPARE(p.service(), svc_);
  }

  // Verify stage table widget is created
  void testStageTableNotNull() {
    DataPipelinePlugin p(svc_);
    QVERIFY(p.stageTable() != nullptr);
  }
};

QTEST_MAIN(DataPipelinePluginTest)
#include "data_pipeline_plugin_test.moc"
