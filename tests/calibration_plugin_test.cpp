// CalibrationPluginTest — Tests for CalibrationPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Initial state and calibration type
//   - Required samples configuration
//   - Wizard navigation (next/prev step)
//   - Start/stop calibration
//   - Sample collection

#include <QTest>
#include <QSignalSpy>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextEdit>
#include "plugins/calibration/CalibrationPlugin.h"

class CalibrationPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display name, and default order
  void testPluginIdentity() {
    CalibrationPlugin plugin;

    QCOMPARE(plugin.id(), QString("calibration"));
    QCOMPARE(plugin.displayName(), QString("Calibration"));
    QCOMPARE(plugin.defaultOrder(), 210);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created
  void testWidgetCreation() {
    CalibrationPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial state is SelectDevice with no samples
  void testInitialState() {
    CalibrationPlugin plugin;

    QCOMPARE(plugin.currentStep(), CalibrationPlugin::WizardStep::SelectDevice);
    QCOMPARE(plugin.calibrationType(), CalibrationPlugin::CalibrationType::Full);
    QVERIFY(!plugin.isCalibrating());
    QCOMPARE(plugin.collectedSamples(), 0);
  }

  // Verify calibration type can be changed
  void testCalibrationType() {
    CalibrationPlugin plugin;

    plugin.setCalibrationType(CalibrationPlugin::CalibrationType::Offset);
    QCOMPARE(plugin.calibrationType(), CalibrationPlugin::CalibrationType::Offset);

    plugin.setCalibrationType(CalibrationPlugin::CalibrationType::Gain);
    QCOMPARE(plugin.calibrationType(), CalibrationPlugin::CalibrationType::Gain);
  }

  // Verify required samples configuration
  void testRequiredSamples() {
    CalibrationPlugin plugin;

    plugin.setRequiredSamples(100);
    QCOMPARE(plugin.requiredSamples(), 100);
  }

  // Verify wizard step navigation with signal emission
  void testWizardNavigation() {
    CalibrationPlugin plugin;
    QSignalSpy stepSpy(&plugin, &CalibrationPlugin::stepChanged);

    plugin.nextStep();
    QCOMPARE(plugin.currentStep(), CalibrationPlugin::WizardStep::Configure);
    QCOMPARE(stepSpy.count(), 1);

    plugin.prevStep();
    QCOMPARE(plugin.currentStep(), CalibrationPlugin::WizardStep::SelectDevice);
    QCOMPARE(stepSpy.count(), 2);
  }

  // Verify start calibration transitions to Collecting
  void testStartCalibration() {
    CalibrationPlugin plugin;

    plugin.setRequiredSamples(10);
    plugin.nextStep(); // to Configure
    plugin.startCalibration();

    QVERIFY(plugin.isCalibrating());
    QCOMPARE(plugin.currentStep(), CalibrationPlugin::WizardStep::Collecting);
  }

  // Verify stop calibration halts collection
  void testStopCalibration() {
    CalibrationPlugin plugin;

    plugin.nextStep();
    plugin.startCalibration();
    QVERIFY(plugin.isCalibrating());

    plugin.stopCalibration();
    QVERIFY(!plugin.isCalibrating());
  }

  void testCollectSample() {
    CalibrationPlugin plugin;
    QSignalSpy sampleSpy(&plugin, &CalibrationPlugin::sampleCollected);

    plugin.setRequiredSamples(5);
    plugin.nextStep();
    plugin.startCalibration();

    for (int i = 0; i < 5; ++i) {
      plugin.collectSample();
    }

    QCOMPARE(plugin.collectedSamples(), 5);
    QCOMPARE(sampleSpy.count(), 5);
    QVERIFY(!plugin.isCalibrating()); // auto-stop at required
  }

  void testDataTable() {
    CalibrationPlugin plugin;

    plugin.setRequiredSamples(3);
    plugin.nextStep();
    plugin.startCalibration();
    plugin.collectSample();
    plugin.collectSample();
    plugin.collectSample();

    QTableWidget *table = plugin.dataTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
  }

  void testAnalyzeResults() {
    CalibrationPlugin plugin;
    QSignalSpy completeSpy(&plugin, &CalibrationPlugin::calibrationComplete);

    plugin.setRequiredSamples(10);
    plugin.nextStep();
    plugin.startCalibration();
    for (int i = 0; i < 10; ++i) {
      plugin.collectSample();
    }

    plugin.analyzeResults();

    QVERIFY(plugin.offsetResult() != 0.0 || plugin.gainResult() != 1.0);
    QCOMPARE(completeSpy.count(), 1);
  }

  void testResultsView() {
    CalibrationPlugin plugin;

    plugin.setRequiredSamples(5);
    plugin.nextStep();
    plugin.startCalibration();
    for (int i = 0; i < 5; ++i) {
      plugin.collectSample();
    }
    plugin.analyzeResults();

    QTextEdit *view = plugin.resultsView();
    QVERIFY(view != nullptr);
    QVERIFY(!view->toPlainText().isEmpty());
  }

  void testHistory() {
    CalibrationPlugin plugin;

    plugin.addHistoryEntry("Slave 0", CalibrationPlugin::CalibrationType::Full,
                           0.5, 1.002, 0.3);
    QCOMPARE(plugin.historyCount(), 1);

    plugin.addHistoryEntry("Slave 1", CalibrationPlugin::CalibrationType::Offset,
                           0.1, 1.0, 0.1);
    QCOMPARE(plugin.historyCount(), 2);
  }

  void testHistoryTable() {
    CalibrationPlugin plugin;

    plugin.addHistoryEntry("Test", CalibrationPlugin::CalibrationType::Full,
                           0.0, 1.0, 0.0);

    QTableWidget *table = plugin.historyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
  }

  void testResetWizard() {
    CalibrationPlugin plugin;

    plugin.nextStep();
    plugin.nextStep();
    QCOMPARE(plugin.currentStep(), CalibrationPlugin::WizardStep::Collecting);

    plugin.resetWizard();
    QCOMPARE(plugin.currentStep(), CalibrationPlugin::WizardStep::SelectDevice);
    QCOMPARE(plugin.collectedSamples(), 0);
  }

  void testStepChangedSignal() {
    CalibrationPlugin plugin;
    QSignalSpy stepSpy(&plugin, &CalibrationPlugin::stepChanged);

    plugin.nextStep();
    plugin.nextStep();
    QCOMPARE(stepSpy.count(), 2);
  }
};

QTEST_MAIN(CalibrationPluginTest)
#include "calibration_plugin_test.moc"
