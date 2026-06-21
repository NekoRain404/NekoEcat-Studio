// EtherCATDigitalTwinServiceTest — Tests for EtherCATDigitalTwinService
//
// Test coverage:
//   - Digital twin creation, lookup, and removal
//   - Physical device synchronization
//   - Scenario simulation (stress test, empty, multiple)
//   - Behavior prediction with confidence scoring
//   - Signal emission for twin lifecycle events

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATDigitalTwinService.h"

class EtherCATDigitalTwinServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Create digital twin for position 1
  void testCreateDigitalTwin() {
    EtherCATDigitalTwinService svc;
    QSignalSpy spy(&svc, &EtherCATDigitalTwinService::digitalTwinCreated);
    DigitalTwin dt = svc.createDigitalTwin(1);
    QCOMPARE(dt.position, 1);
    QCOMPARE(dt.state, QString("Created"));
    QCOMPARE(dt.syncStatus, TwinSyncStatus::Never);
    QCOMPARE(spy.count(), 1);
  }

  // Create multiple twins and verify count
  void testCreateMultipleTwins() {
    EtherCATDigitalTwinService svc;
    svc.createDigitalTwin(1);
    svc.createDigitalTwin(2);
    QCOMPARE(svc.allTwins().size(), 2);
  }

  // Lookup twin by position
  void testTwinLookup() {
    EtherCATDigitalTwinService svc;
    svc.createDigitalTwin(5);
    DigitalTwin dt = svc.twin(5);
    QCOMPARE(dt.position, 5);
    QVERIFY(!dt.model.isEmpty());
  }

  // Lookup nonexistent twin returns position 0
  void testTwinNotFound() {
    EtherCATDigitalTwinService svc;
    DigitalTwin dt = svc.twin(99);
    QCOMPARE(dt.position, 0);
  }

  // Sync twin with physical device
  void testSyncWithPhysical() {
    EtherCATDigitalTwinService svc;
    svc.createDigitalTwin(1);
    QSignalSpy spy(&svc, &EtherCATDigitalTwinService::syncCompleted);
    QVERIFY(svc.syncWithPhysical(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
    QVERIFY(spy.at(0).at(1).toBool());
    DigitalTwin dt = svc.twin(1);
    QCOMPARE(dt.syncStatus, TwinSyncStatus::Synced);
    QCOMPARE(dt.state, QString("Synced"));
  }

  // Sync nonexistent twin fails
  void testSyncWithPhysicalNotFound() {
    EtherCATDigitalTwinService svc;
    QVERIFY(!svc.syncWithPhysical(99));
  }

  // Simulate stress test scenario
  void testSimulateScenario() {
    EtherCATDigitalTwinService svc;
    QSignalSpy spy(&svc, &EtherCATDigitalTwinService::simulationFinished);
    TwinScenario scenario;
    scenario.name = "stress_test";
    scenario.description = "Stress test scenario";
    scenario.parameters["load"] = 100;
    scenario.parameters["duration"] = 5000;
    TwinSimulationResult result = svc.simulateScenario(scenario);
    QCOMPARE(result.scenarioName, QString("stress_test"));
    QCOMPARE(result.status, TwinSimulationStatus::Completed);
    QVERIFY(result.success);
    QVERIFY(result.outputs.size() > 0);
    QCOMPARE(spy.count(), 1);
  }

  // Simulate empty scenario succeeds with no outputs
  void testSimulateScenarioEmpty() {
    EtherCATDigitalTwinService svc;
    TwinScenario scenario;
    scenario.name = "empty";
    TwinSimulationResult result = svc.simulateScenario(scenario);
    QVERIFY(result.success);
    QCOMPARE(result.outputs.size(), 0);
  }

  // Predict behavior from time-series data
  void testPredictBehavior() {
    EtherCATDigitalTwinService svc;
    QVector<TwinDataPoint> data;
    for (int i = 0; i < 10; i++) {
      TwinDataPoint dp;
      dp.value = i * 1.5;
      dp.timestamp = QDateTime::currentDateTime().addSecs(i * 60);
      data.append(dp);
    }
    TwinPrediction pred = svc.predictBehavior(data);
    QCOMPARE(pred.model, QString("LinearRegression"));
    QCOMPARE(pred.forecast.size(), 5);
    QVERIFY(pred.confidence > 0.5);
  }

  // Predict with empty data uses default confidence
  void testPredictBehaviorEmpty() {
    EtherCATDigitalTwinService svc;
    TwinPrediction pred = svc.predictBehavior({});
    QCOMPARE(pred.forecast.size(), 5);
    QCOMPARE(pred.confidence, 0.5);
  }

  // Remove existing twin
  void testRemoveTwin() {
    EtherCATDigitalTwinService svc;
    svc.createDigitalTwin(1);
    QVERIFY(svc.removeTwin(1));
    QCOMPARE(svc.allTwins().size(), 0);
  }

  // Remove nonexistent twin fails
  void testRemoveTwinNotFound() {
    EtherCATDigitalTwinService svc;
    QVERIFY(!svc.removeTwin(99));
  }

  // allTwins tracks created twins
  void testAllTwins() {
    EtherCATDigitalTwinService svc;
    QCOMPARE(svc.allTwins().size(), 0);
    svc.createDigitalTwin(1);
    svc.createDigitalTwin(2);
    svc.createDigitalTwin(3);
    QCOMPARE(svc.allTwins().size(), 3);
  }

  // digitalTwinCreated signal carries position
  void testDigitalTwinCreatedSignal() {
    EtherCATDigitalTwinService svc;
    QSignalSpy spy(&svc, &EtherCATDigitalTwinService::digitalTwinCreated);
    QVERIFY(spy.isValid());
    svc.createDigitalTwin(1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
  }

  // syncCompleted signal fires on sync
  void testSyncCompletedSignal() {
    EtherCATDigitalTwinService svc;
    svc.createDigitalTwin(1);
    QSignalSpy spy(&svc, &EtherCATDigitalTwinService::syncCompleted);
    QVERIFY(spy.isValid());
    svc.syncWithPhysical(1);
    QCOMPARE(spy.count(), 1);
  }

  // simulationFinished signal fires on simulation
  void testSimulationFinishedSignal() {
    EtherCATDigitalTwinService svc;
    QSignalSpy spy(&svc, &EtherCATDigitalTwinService::simulationFinished);
    QVERIFY(spy.isValid());
    TwinScenario scenario;
    scenario.name = "test";
    svc.simulateScenario(scenario);
    QCOMPARE(spy.count(), 1);
  }

  // Sync updates twin state and lastSync timestamp
  void testSyncUpdatesState() {
    EtherCATDigitalTwinService svc;
    svc.createDigitalTwin(1);
    DigitalTwin before = svc.twin(1);
    QCOMPARE(before.syncStatus, TwinSyncStatus::Never);
    svc.syncWithPhysical(1);
    DigitalTwin after = svc.twin(1);
    QCOMPARE(after.syncStatus, TwinSyncStatus::Synced);
    QVERIFY(after.lastSync.isValid());
  }

  // Run multiple simulations sequentially
  void testMultipleSimulations() {
    EtherCATDigitalTwinService svc;
    for (int i = 0; i < 5; i++) {
      TwinScenario scenario;
      scenario.name = QString("scenario_%1").arg(i);
      TwinSimulationResult result = svc.simulateScenario(scenario);
      QVERIFY(result.success);
    }
  }

  // Confidence increases with more data points
  void testPredictConfidence() {
    EtherCATDigitalTwinService svc;
    QVector<TwinDataPoint> shortData;
    for (int i = 0; i < 3; i++) {
      TwinDataPoint dp;
      dp.value = i;
      shortData.append(dp);
    }
    TwinPrediction predShort = svc.predictBehavior(shortData);
    QCOMPARE(predShort.confidence, 0.5);

    QVector<TwinDataPoint> longData;
    for (int i = 0; i < 10; i++) {
      TwinDataPoint dp;
      dp.value = i;
      longData.append(dp);
    }
    TwinPrediction predLong = svc.predictBehavior(longData);
    QVERIFY(predLong.confidence > 0.5);
  }
};

QTEST_MAIN(EtherCATDigitalTwinServiceTest)
#include "ethercat_digital_twin_service_test.moc"
