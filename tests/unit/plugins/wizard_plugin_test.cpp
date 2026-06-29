// WizardPluginTest — Tests for Wizard Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Initial state (wizard count, history, running state)
//   - Wizard list widget
//   - Start/finish/cancel wizard
//   - Step navigation (next/previous with boundary checks)
//   - Step and history table structure
//   - Add custom wizards
//   - Multiple wizard runs
//   - Custom wizard steps
//   - Export history
#include <QFile>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTest>
#include <QSignalSpy>
#include <QListWidget>
#include <QTableWidget>
#include <QTextEdit>
#include "plugins/wizard/WizardPlugin.h"

class WizardPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, order, and visibility
  void testPluginIdentity() {
    WizardPlugin plugin;

    QCOMPARE(plugin.id(), QString("wizard"));
    QCOMPARE(plugin.displayName(), QString("Wizard"));
    QCOMPARE(plugin.displayNameZh(), QString("向导"));
    QCOMPARE(plugin.defaultOrder(), 220);
    QCOMPARE(plugin.visible(), true);
  }

  // Widget should be created successfully
  void testWidgetCreation() {
    WizardPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Initial state: 6 wizards, no history, not running
  void testInitialState() {
    WizardPlugin plugin;

    QCOMPARE(plugin.wizardCount(), 6);
    QCOMPARE(plugin.historyCount(), 0);
    QCOMPARE(plugin.isRunning(), false);
    QCOMPARE(plugin.currentStep(), 0);
    QCOMPARE(plugin.totalSteps(), 0);
  }

  // Wizard list widget has 6 items
  void testWizardList() {
    WizardPlugin plugin;

    QListWidget *list = plugin.wizardList();
    QVERIFY(list != nullptr);
    QCOMPARE(list->count(), 6);
  }

  // Start a wizard and verify running state and signal
  void testStartWizard() {
    WizardPlugin plugin;
    QSignalSpy startSpy(&plugin, &WizardPlugin::wizardStarted);

    plugin.startWizard("network_setup");
    QCOMPARE(plugin.isRunning(), true);
    QCOMPARE(plugin.currentStep(), 0);
    QCOMPARE(plugin.totalSteps(), 4);
    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(startSpy.at(0).at(0).toString(), QString("network_setup"));
  }

  // Navigate forward and backward through steps
  void testStepNavigation() {
    WizardPlugin plugin;
    QSignalSpy stepSpy(&plugin, &WizardPlugin::stepChanged);

    plugin.startWizard("network_setup");
    QCOMPARE(plugin.currentStep(), 0);
    QCOMPARE(stepSpy.count(), 1);

    plugin.nextStep();
    QCOMPARE(plugin.currentStep(), 1);
    QCOMPARE(stepSpy.count(), 2);

    plugin.nextStep();
    QCOMPARE(plugin.currentStep(), 2);
    QCOMPARE(stepSpy.count(), 3);

    plugin.previousStep();
    QCOMPARE(plugin.currentStep(), 1);
    QCOMPARE(stepSpy.count(), 4);
  }

  // Cannot advance past last step
  void testNextStepBoundary() {
    WizardPlugin plugin;

    plugin.startWizard("slave_discovery");
    plugin.nextStep();
    plugin.nextStep();
    QCOMPARE(plugin.currentStep(), 2);

    plugin.nextStep();
    QCOMPARE(plugin.currentStep(), 2);
  }

  // Cannot go back before first step
  void testPreviousStepBoundary() {
    WizardPlugin plugin;

    plugin.startWizard("network_setup");
    QCOMPARE(plugin.currentStep(), 0);

    plugin.previousStep();
    QCOMPARE(plugin.currentStep(), 0);
  }

  // Finish wizard and verify signal and history
  void testFinishWizard() {
    WizardPlugin plugin;
    QSignalSpy finishSpy(&plugin, &WizardPlugin::wizardFinished);

    plugin.startWizard("pdo_mapping");
    plugin.nextStep();
    plugin.finishWizard(true);

    QCOMPARE(plugin.isRunning(), false);
    QCOMPARE(plugin.historyCount(), 1);
    QCOMPARE(finishSpy.count(), 1);
    QCOMPARE(finishSpy.at(0).at(1).toBool(), true);
  }

  // Cancel wizard does not add to history
  void testCancelWizard() {
    WizardPlugin plugin;

    plugin.startWizard("sdo_config");
    plugin.nextStep();
    plugin.cancelWizard();

    QCOMPARE(plugin.isRunning(), false);
    QCOMPARE(plugin.historyCount(), 0);
  }

  // Step table has 3 columns
  void testStepTable() {
    WizardPlugin plugin;

    QTableWidget *table = plugin.stepTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 3);
  }

  // History table shows completed wizard
  void testHistoryTable() {
    WizardPlugin plugin;

    plugin.startWizard("network_setup");
    plugin.finishWizard(true);

    QTableWidget *table = plugin.historyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
  }

  // Add a custom wizard entry
  void testAddWizard() {
    WizardPlugin plugin;
    int initial = plugin.wizardCount();

    WizardPlugin::WizardEntry entry;
    entry.id = "custom";
    entry.name = "Custom Wizard";
    entry.category = "Custom";
    entry.description = "A custom wizard";
    entry.stepCount = 2;

    plugin.addWizard(entry);
    QCOMPARE(plugin.wizardCount(), initial + 1);
  }

  // Run multiple wizards and verify history
  void testMultipleWizards() {
    WizardPlugin plugin;

    plugin.startWizard("network_setup");
    plugin.finishWizard(true);

    plugin.startWizard("slave_discovery");
    plugin.finishWizard(false);

    QCOMPARE(plugin.historyCount(), 2);
  }

  // Set custom wizard steps and verify count
  void testSetWizardSteps() {
    WizardPlugin plugin;

    plugin.startWizard("network_setup");
    QVector<WizardPlugin::WizardStep> customSteps = {
        {"Custom Step 1", "Do this", "Tip 1"},
        {"Custom Step 2", "Do that", "Tip 2"},
    };
    plugin.setWizardSteps("network_setup", customSteps);
    QCOMPARE(plugin.totalSteps(), 2);
  }

  // Export wizard history to CSV file
  void testExportHistory() {
    WizardPlugin plugin;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    plugin.startWizard("network_setup");
    plugin.finishWizard(true);

    const QString path = dir.filePath("wizard_history_test.csv");
    QVERIFY(plugin.exportHistory(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString csv = QString::fromUtf8(file.readAll());
    QVERIFY(csv.contains(QStringLiteral("network_setup,Network Setup,")));
    QVERIFY(csv.contains(QStringLiteral(",success\n")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!plugin.exportHistory(QString()));
    QVERIFY(!plugin.exportHistory(dir.path()));
  }
};

QTEST_MAIN(WizardPluginTest)
#include "wizard_plugin_test.moc"
