// LogicAnalyzerPluginTest — Tests for LogicAnalyzerPlugin
//
// Test coverage:
//   - Plugin identity and default order
//   - Widget creation and visibility

#include <QTest>
#include <QApplication>
#include "services/TraceService.h"
#include "plugins/logicanalyzer/LogicAnalyzerPlugin.h"

class LogicAnalyzerPluginTest : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() {
    service = new TraceService(this);
    plugin = new LogicAnalyzerPlugin(service, this);
  }

  void cleanupTestCase() {
    delete plugin;
    delete service;
  }

  // Verify plugin id, display names, order, visibility
  void testPluginIdentity() {
    QCOMPARE(plugin->id(), QString("logicanalyzer"));
    QCOMPARE(plugin->displayName(), QString("Logic Analyzer"));
    QCOMPARE(plugin->defaultOrder(), 185);
    QVERIFY(plugin->visible());
  }

  // Widget is created and not null
  // Check widget is created
  void testWidgetNotNull() {
    QVERIFY(plugin->widget() != nullptr);
  }

private:
  TraceService *service = nullptr;
  LogicAnalyzerPlugin *plugin = nullptr;
};

QTEST_MAIN(LogicAnalyzerPluginTest)
#include "logicanalyzer_plugin_test.moc"
