// DigitalTwinStudioPluginTest — Tests for DigitalTwinStudioPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget and initial state
//   - 3D model add/remove
//   - Sync status add/remove
//   - Simulation parameter add/remove
//   - Prediction data add/remove
//   - Tab and table widgets
//   - Data export and signal emissions

// DigitalTwinStudioPluginTest — Tests for DigitalTwinStudioPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - 3D model add/remove
//   - Twin sync status add/remove
//   - Simulation parameter add/remove
//   - Prediction data add/remove
//   - Tab and table widget existence
//   - Export and signal emissions

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include "plugins/digitaltwinstudio/DigitalTwinStudioPlugin.h"

class DigitalTwinStudioPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, order, and visibility
  void testPluginIdentity() {
    DigitalTwinStudioPlugin plugin;
    QCOMPARE(plugin.id(), QString("digitaltwinstudio"));
    QCOMPARE(plugin.displayName(), QString("Digital Twin Studio"));
    QCOMPARE(plugin.displayNameZh(), QString("数字孪生工作室"));
    QCOMPARE(plugin.defaultOrder(), 370);
    QCOMPARE(plugin.visible(), true);
  }
  // Verify main widget is created
  // Verify main widget is created
  void testWidgetCreation() {
    DigitalTwinStudioPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }
  // Verify all initial counts are zero
  // Verify initial counts are zero
  void testInitialState() {
    DigitalTwinStudioPlugin plugin;
    QCOMPARE(plugin.modelCount(), 0);
    QCOMPARE(plugin.syncStatusCount(), 0);
    QCOMPARE(plugin.simulationParamCount(), 0);
    QCOMPARE(plugin.predictionCount(), 0);
  }
  // Verify adding a 3D model increments count
  // Verify adding a 3D model increments count
  void testAddModel() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::Model3D m;
    m.name = "motor";
    m.filePath = "/models/motor.obj";
    m.rotX = 0.0; m.rotY = 0.0; m.rotZ = 0.0;
    m.zoom = 1.0;
    plugin.addModel(m);
    QCOMPARE(plugin.modelCount(), 1);
  }
  // Verify removing a 3D model decrements count
  // Verify removing a 3D model decrements count
  void testRemoveModel() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::Model3D m;
    m.name = "motor"; m.filePath = "/models/motor.obj";
    m.rotX = 0.0; m.rotY = 0.0; m.rotZ = 0.0; m.zoom = 1.0;
    plugin.addModel(m);
    QCOMPARE(plugin.modelCount(), 1);
    plugin.removeModel(0);
    QCOMPARE(plugin.modelCount(), 0);
  }
  // Verify adding a sync status entry increments count
  // Verify adding a sync status entry increments count
  void testAddSyncStatus() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::TwinSyncStatus s;
    s.deviceId = "dev0"; s.status = "synced";
    s.latency = 1.5; s.lastSync = "2024-01-01T00:00:00";
    plugin.addSyncStatus(s);
    QCOMPARE(plugin.syncStatusCount(), 1);
  }
  // Verify removing a sync status entry decrements count
  // Verify removing a sync status entry decrements count
  void testRemoveSyncStatus() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::TwinSyncStatus s;
    s.deviceId = "dev0"; s.status = "synced";
    s.latency = 1.5; s.lastSync = "2024-01-01T00:00:00";
    plugin.addSyncStatus(s);
    QCOMPARE(plugin.syncStatusCount(), 1);
    plugin.removeSyncStatus(0);
    QCOMPARE(plugin.syncStatusCount(), 0);
  }
  // Verify adding a simulation parameter increments count
  // Verify adding a simulation parameter increments count
  void testAddSimulationParam() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::SimulationParam p;
    p.name = "speed"; p.value = 100.0;
    p.minVal = 0.0; p.maxVal = 200.0; p.unit = "rpm";
    plugin.addSimulationParam(p);
    QCOMPARE(plugin.simulationParamCount(), 1);
  }
  // Verify removing a simulation parameter decrements count
  // Verify removing a simulation parameter decrements count
  void testRemoveSimulationParam() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::SimulationParam p;
    p.name = "speed"; p.value = 100.0;
    p.minVal = 0.0; p.maxVal = 200.0; p.unit = "rpm";
    plugin.addSimulationParam(p);
    QCOMPARE(plugin.simulationParamCount(), 1);
    plugin.removeSimulationParam(0);
    QCOMPARE(plugin.simulationParamCount(), 0);
  }
  // Verify adding prediction data increments count
  // Verify adding a prediction increments count
  void testAddPrediction() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::PredictionData p;
    p.metric = "temperature"; p.currentValue = 50.0;
    p.predictedValue = 55.0; p.confidence = 0.85; p.trend = "increasing";
    plugin.addPrediction(p);
    QCOMPARE(plugin.predictionCount(), 1);
  }
  // Verify removing prediction data decrements count
  // Verify removing a prediction decrements count
  void testRemovePrediction() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::PredictionData p;
    p.metric = "temperature"; p.currentValue = 50.0;
    p.predictedValue = 55.0; p.confidence = 0.85; p.trend = "increasing";
    plugin.addPrediction(p);
    QCOMPARE(plugin.predictionCount(), 1);
    plugin.removePrediction(0);
    QCOMPARE(plugin.predictionCount(), 0);
  }
  // Verify tab widget is created
  // Verify tabs widget exists
  void testTabs() {
    DigitalTwinStudioPlugin plugin;
    QVERIFY(plugin.tabs() != nullptr);
  }
  // Verify model table widget is created
  // Verify model table widget exists
  void testModelTable() {
    DigitalTwinStudioPlugin plugin;
    QVERIFY(plugin.modelTable() != nullptr);
  }
  // Verify sync table widget is created
  // Verify sync table widget exists
  void testSyncTable() {
    DigitalTwinStudioPlugin plugin;
    QVERIFY(plugin.syncTable() != nullptr);
  }
  // Verify simulation table widget is created
  // Verify simulation table widget exists
  void testSimulationTable() {
    DigitalTwinStudioPlugin plugin;
    QVERIFY(plugin.simulationTable() != nullptr);
  }
  // Verify prediction table widget is created
  // Verify prediction table widget exists
  void testPredictionTable() {
    DigitalTwinStudioPlugin plugin;
    QVERIFY(plugin.predictionTable() != nullptr);
  }
  // Verify export produces non-empty JSON
  // Verify export produces non-empty JSON
  void testExportData() {
    DigitalTwinStudioPlugin plugin;
    DigitalTwinStudioPlugin::Model3D m;
    m.name = "motor"; m.filePath = "/models/motor.obj";
    m.rotX = 0.0; m.rotY = 0.0; m.rotZ = 0.0; m.zoom = 1.0;
    plugin.addModel(m);
    QString json = plugin.exportData();
    QVERIFY(!json.isEmpty());
  }
  // Verify status label widget is created
  // Verify status label widget exists
  void testStatusLabel() {
    DigitalTwinStudioPlugin plugin;
    QVERIFY(plugin.statusLabel() != nullptr);
  }
  // Verify modelAdded signal is emitted on add
  // Verify modelAdded signal is emitted on add
  void testModelAddedSignal() {
    DigitalTwinStudioPlugin plugin;
    QSignalSpy spy(&plugin, &DigitalTwinStudioPlugin::modelAdded);
    DigitalTwinStudioPlugin::Model3D m;
    m.name = "motor"; m.filePath = "/models/motor.obj";
    m.rotX = 0.0; m.rotY = 0.0; m.rotZ = 0.0; m.zoom = 1.0;
    plugin.addModel(m);
    QCOMPARE(spy.count(), 1);
  }
  // Verify predictionUpdated signal is emitted on add
  // Verify predictionUpdated signal is emitted on add
  void testPredictionUpdatedSignal() {
    DigitalTwinStudioPlugin plugin;
    QSignalSpy spy(&plugin, &DigitalTwinStudioPlugin::predictionUpdated);
    DigitalTwinStudioPlugin::PredictionData p;
    p.metric = "temperature"; p.currentValue = 50.0;
    p.predictedValue = 55.0; p.confidence = 0.85; p.trend = "increasing";
    plugin.addPrediction(p);
    QCOMPARE(spy.count(), 1);
  }
};

QTEST_MAIN(DigitalTwinStudioPluginTest)
#include "digitaltwinstudio_plugin_test.moc"
