// SimulationPluginTest — Tests for SimulationPlugin
//
// Test coverage:
//   - Plugin identity, visibility, and widget creation
//   - Initial simulation state and statistics
//   - Cycle time, slave count, and duration configuration
//   - Start/stop/pause/resume/step simulation
//   - Statistics reset, table, and latency
//   - Log view and clearing
//   - Frame processed and statistics updated signals

// SimulationPluginTest — Tests for SimulationPlugin
//
// Test coverage:
//   - Plugin identity and widget creation
//   - Initial simulation state (Idle, counters at zero)
//   - Cycle time, slave count, and duration configuration
//   - Start/stop simulation with state change signals
//   - Pause/resume simulation state transitions
//   - Step simulation and frame counting
//   - Statistics reset
//   - Statistics and data view table access
//   - Log entry add and clear
//   - Frame and statistics signal emissions
//   - Latency calculation
#include "plugins/simulation/SimulationPlugin.h"
#include <QFile>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTest>
#include <QTextEdit>

class SimulationPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin ID, display name, order, and visibility
    void testPluginIdentity() {
        SimulationPlugin plugin;

        QCOMPARE(plugin.id(), QString("simulation"));
        QCOMPARE(plugin.displayName(), QString("Simulation"));
        QCOMPARE(plugin.defaultOrder(), 205);
        QCOMPARE(plugin.visible(), false);
    }

    // Verify widget is created
    void testWidgetCreation() {
        SimulationPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify initial state is Idle with zero frame and error counts
    void testInitialState() {
        SimulationPlugin plugin;

        QCOMPARE(plugin.simulationState(), SimulationPlugin::SimState::Idle);
        QVERIFY(!plugin.isRunning());
        QVERIFY(!plugin.isPaused());
        QCOMPARE(plugin.frameCount(), 0);
        QCOMPARE(plugin.errorCount(), 0);
    }

    // Test cycle time configuration in microseconds
    void testCycleTimeConfig() {
        SimulationPlugin plugin;

        plugin.setCycleTimeUs(2000);
        QCOMPARE(plugin.cycleTimeUs(), 2000);
    }

    // Test slave count configuration
    void testSlaveCountConfig() {
        SimulationPlugin plugin;

        plugin.setSlaveCount(8);
        QCOMPARE(plugin.slaveCount(), 8);
    }

    // Test simulation duration configuration
    void testDurationConfig() {
        SimulationPlugin plugin;

        plugin.setSimulationDuration(30);
        QCOMPARE(plugin.simulationDuration(), 30);
    }

    // Test start and stop simulation transitions and signal emission
    void testStartStopSimulation() {
        SimulationPlugin plugin;
        QSignalSpy stateSpy(&plugin, &SimulationPlugin::simulationStateChanged);

        plugin.startSimulation();
        QCOMPARE(plugin.simulationState(), SimulationPlugin::SimState::Running);
        QVERIFY(plugin.isRunning());
        QCOMPARE(stateSpy.count(), 1);

        plugin.stopSimulation();
        QCOMPARE(plugin.simulationState(), SimulationPlugin::SimState::Idle);
        QVERIFY(!plugin.isRunning());
        QCOMPARE(stateSpy.count(), 2);
    }

    // Test pause and resume simulation state transitions
    void testPauseResumeSimulation() {
        SimulationPlugin plugin;

        plugin.startSimulation();
        QVERIFY(plugin.isRunning());

        plugin.pauseSimulation();
        QCOMPARE(plugin.simulationState(), SimulationPlugin::SimState::Paused);
        QVERIFY(plugin.isPaused());
        QVERIFY(!plugin.isRunning());

        plugin.startSimulation();
        QVERIFY(plugin.isRunning());
    }

    // Test step simulation increments frame count
    void testStepSimulation() {
        SimulationPlugin plugin;

        plugin.stepSimulation();
        QCOMPARE(plugin.frameCount(), 1);

        plugin.stepSimulation();
        QCOMPARE(plugin.frameCount(), 2);
    }

    // Test reset statistics zeroes frame and error counts
    void testResetStatistics() {
        SimulationPlugin plugin;

        plugin.stepSimulation();
        plugin.stepSimulation();
        plugin.stepSimulation();
        QVERIFY(plugin.frameCount() > 0);

        plugin.resetStatistics();
        QCOMPARE(plugin.frameCount(), 0);
        QCOMPARE(plugin.errorCount(), 0);
    }

    // Verify statistics table has correct dimensions
    void testStatisticsTable() {
        SimulationPlugin plugin;

        QTableWidget* table = plugin.statisticsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 6);
        QCOMPARE(table->columnCount(), 2);
    }

    // Verify data view table is created
    void testDataTable() {
        SimulationPlugin plugin;

        QTableWidget* table = plugin.dataViewTable();
        QVERIFY(table != nullptr);
    }

    // Test adding a log entry
    void testLogView() {
        SimulationPlugin plugin;

        plugin.addLogEntry("Test log entry");
        QVERIFY(plugin.logCount() > 0);
    }

    // Test clearing log entries
    void testClearLog() {
        SimulationPlugin plugin;

        plugin.addLogEntry("Entry 1");
        plugin.addLogEntry("Entry 2");
        QVERIFY(plugin.logCount() > 0);

        plugin.clearLog();
        QCOMPARE(plugin.logCount(), 0);
    }

    // Test frameProcessed signal is emitted on step
    void testFrameProcessedSignal() {
        SimulationPlugin plugin;
        QSignalSpy frameSpy(&plugin, &SimulationPlugin::frameProcessed);

        plugin.stepSimulation();
        QCOMPARE(frameSpy.count(), 1);
    }

    // Test statisticsUpdated signal is emitted on step
    void testStatisticsUpdatedSignal() {
        SimulationPlugin plugin;
        QSignalSpy statsSpy(&plugin, &SimulationPlugin::statisticsUpdated);

        plugin.stepSimulation();
        QCOMPARE(statsSpy.count(), 1);
    }

    // Verify average and max latency are positive after step
    void testLatencyCalculation() {
        SimulationPlugin plugin;

        plugin.stepSimulation();
        QVERIFY(plugin.averageLatencyUs() > 0.0);
        QVERIFY(plugin.maxLatencyUs() > 0.0);
    }

    void testExportResultsReportsPersistenceOutcome() {
        SimulationPlugin plugin;
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        plugin.stepSimulation();

        const QString path = dir.filePath("simulation_results.csv");
        QVERIFY(plugin.exportResults(path));
        QVERIFY(QFile::exists(path));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.exportResults(QString()));
        QVERIFY(!plugin.exportResults(dir.path()));
    }
};

QTEST_MAIN(SimulationPluginTest)
#include "simulation_plugin_test.moc"
