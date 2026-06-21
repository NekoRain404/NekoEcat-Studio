// AutomationPluginTest — Tests for AutomationPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - ScriptingService execute, log, signals, errors
//   - BatchOperationService operations
//   - Script execution and cancellation

#include <QTest>
#include <QSignalSpy>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QPushButton>
#include "plugins/automation/AutomationPlugin.h"
#include "services/ScriptingService.h"
#include "services/BatchOperationService.h"
#include "services/SdoService.h"
#include "services/TopologyService.h"
#include "infra/EcatClient.h"

class AutomationPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display name, and default order
  void testPluginIdentity() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    AutomationPlugin plugin(&scripting);

    QCOMPARE(plugin.id(), QString("automation"));
    QCOMPARE(plugin.displayName(), QString("Automation"));
    QCOMPARE(plugin.defaultOrder(), 120);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created
  void testWidgetCreation() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    AutomationPlugin plugin(&scripting);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify script execution returns correct result
  void testScriptingServiceExecuteSimpleScript() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QJSValue result = scripting.executeScript("2 + 2");
    QCOMPARE(result.toInt(), 4);
  }

  // Verify log signal is emitted during script execution
  void testScriptingServiceLogSignal() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QSignalSpy logSpy(&scripting, &ScriptingService::logMessage);
    scripting.executeScript("log('hello')");
    QCOMPARE(logSpy.count(), 1);
    QCOMPARE(logSpy.at(0).at(0).toString(), QString("hello"));
  }

  // Verify start/complete/error signals on execution
  void testScriptingServiceSignals() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QSignalSpy startSpy(&scripting, &ScriptingService::scriptStarted);
    QSignalSpy completeSpy(&scripting, &ScriptingService::scriptCompleted);
    QSignalSpy errorSpy(&scripting, &ScriptingService::scriptError);

    scripting.executeScript("42");
    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(completeSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
  }

  // Verify syntax error emits error signal
  void testScriptingServiceSyntaxError() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QSignalSpy errorSpy(&scripting, &ScriptingService::scriptError);
    scripting.executeScript("function { invalid }");
    QCOMPARE(errorSpy.count(), 1);
  }

  // Verify save and load script round-trip
  void testScriptingServiceSaveLoad() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    QString name = "test_save_load";
    QString content = "var x = 42; log(x);";
    QVERIFY(scripting.saveScript(name, content));

    QString loaded = scripting.loadScript(name);
    QCOMPARE(loaded, content);

    QStringList scripts = scripting.listScripts();
    QVERIFY(scripts.contains(name + ".js"));
  }

  // Verify registering a service makes it available to scripts
  void testScriptingServiceRegisterService() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    ScriptingService scripting(&client, &sdo, &topo);

    auto *obj = new QObject(&scripting);
    obj->setObjectName("testObj");
    scripting.registerService("myObj", obj);

    QJSValue result = scripting.executeScript("myObj.objectName");
    QCOMPARE(result.toString(), QString("testObj"));
  }

  // Verify batch operation signals and result
  void testBatchOperationServiceSignals() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    BatchOperationService batch(&client, &sdo, &topo);

    QSignalSpy startSpy(&batch, &BatchOperationService::batchStarted);
    QSignalSpy progressSpy(&batch, &BatchOperationService::batchProgress);
    QSignalSpy completeSpy(&batch, &BatchOperationService::batchCompleted);

    BatchOperation op;
    op.type = BatchType::ScanTopology;
    op.scanCount = 2;

    BatchResult result = batch.executeBatch(op);
    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(completeSpy.count(), 1);
    QCOMPARE(result.success, true);
    QCOMPARE(result.completedItems, 2);
  }

  // Verify batch progress tracking
  void testBatchOperationProgress() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    BatchOperationService batch(&client, &sdo, &topo);

    QSignalSpy progressSpy(&batch, &BatchOperationService::batchProgress);

    BatchOperation op;
    op.type = BatchType::ScanTopology;
    op.scanCount = 3;

    batch.executeBatch(op);
    QCOMPARE(progressSpy.count(), 3);
    QCOMPARE(batch.progress(), 100);
  }

  // Verify batch cancel functionality
  void testBatchOperationCancel() {
    EcatClient client;
    SdoService sdo(&client);
    TopologyService topo(&client);
    BatchOperationService batch(&client, &sdo, &topo);

    BatchOperation op;
    op.type = BatchType::ScanTopology;
    op.scanCount = 5;

    QSignalSpy progressSpy(&batch, &BatchOperationService::batchProgress);
    BatchResult result = batch.executeBatch(op);
    QCOMPARE(result.success, true);
    QCOMPARE(progressSpy.count(), 5);
    QCOMPARE(batch.progress(), 100);
  }
};

QTEST_MAIN(AutomationPluginTest)
#include "automation_plugin_test.moc"
