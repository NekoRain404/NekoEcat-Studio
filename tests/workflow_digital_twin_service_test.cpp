// WorkflowDigitalTwinServiceTest — Tests for WorkflowDigitalTwinService
//
// Test coverage:
//   - Digital twin creation, lookup, and removal
//   - Physical device synchronization
//   - Scenario simulation
//   - Behavior prediction with confidence scoring
//   - Signal emission for twin lifecycle events

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowDigitalTwinService.h"

class WorkflowDigitalTwinServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateDigitalTwin() {
    WorkflowDigitalTwinService svc;
    QSignalSpy spy(&svc, &WorkflowDigitalTwinService::digitalTwinCreated);
    WfDigitalTwin dt = svc.createDigitalTwin(1);
    QCOMPARE(dt.position, 1);
    QCOMPARE(dt.state, QString("Created"));
    QCOMPARE(dt.syncStatus, WfTwinSyncStatus::Never);
    QCOMPARE(spy.count(), 1);
  }

  void testCreateMultipleTwins() {
    WorkflowDigitalTwinService svc;
    svc.createDigitalTwin(1);
    svc.createDigitalTwin(2);
    QCOMPARE(svc.allTwins().size(), 2);
  }

  void testTwinLookup() {
    WorkflowDigitalTwinService svc;
    svc.createDigitalTwin(5);
    WfDigitalTwin dt = svc.twin(5);
    QCOMPARE(dt.position, 5);
    QVERIFY(!dt.model.isEmpty());
  }

  void testTwinNotFound() {
    WorkflowDigitalTwinService svc;
    WfDigitalTwin dt = svc.twin(99);
    QCOMPARE(dt.position, 0);
  }

  void testSyncWithPhysical() {
    WorkflowDigitalTwinService svc;
    svc.createDigitalTwin(1);
    QSignalSpy spy(&svc, &WorkflowDigitalTwinService::syncCompleted);
    QVERIFY(svc.syncWithPhysical(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
    QVERIFY(spy.at(0).at(1).toBool());
    WfDigitalTwin dt = svc.twin(1);
    QCOMPARE(dt.syncStatus, WfTwinSyncStatus::Synced);
    QCOMPARE(dt.state, QString("Synced"));
  }

  void testSyncWithPhysicalNotFound() {
    WorkflowDigitalTwinService svc;
    QVERIFY(!svc.syncWithPhysical(99));
  }

  void testSimulateScenario() {
    WorkflowDigitalTwinService svc;
    QSignalSpy spy(&svc, &WorkflowDigitalTwinService::simulationFinished);
    WfTwinScenario scenario;
    scenario.name = "stress_test";
    scenario.description = "Stress test scenario";
    scenario.parameters["load"] = 100;
    scenario.parameters["duration"] = 5000;
    WfTwinSimulationResult result = svc.simulateScenario(scenario);
    QCOMPARE(result.scenarioName, QString("stress_test"));
    QCOMPARE(result.status, WfTwinSimulationStatus::Completed);
    QVERIFY(result.success);
    QVERIFY(result.outputs.size() > 0);
    QCOMPARE(spy.count(), 1);
  }

  void testSimulateScenarioEmpty() {
    WorkflowDigitalTwinService svc;
    WfTwinScenario scenario;
    scenario.name = "empty";
    WfTwinSimulationResult result = svc.simulateScenario(scenario);
    QVERIFY(result.success);
    QCOMPARE(result.outputs.size(), 0);
  }

  void testPredictBehavior() {
    WorkflowDigitalTwinService svc;
    QVector<WfTwinDataPoint> data;
    for (int i = 0; i < 10; i++) {
      WfTwinDataPoint dp;
      dp.value = i * 1.5;
      dp.timestamp = QDateTime::currentDateTime().addSecs(i * 60);
      data.append(dp);
    }
    WfTwinPrediction pred = svc.predictBehavior(data);
    QCOMPARE(pred.model, QString("LinearRegression"));
    QCOMPARE(pred.forecast.size(), 5);
    QVERIFY(pred.confidence > 0.5);
  }

  void testPredictBehaviorEmpty() {
    WorkflowDigitalTwinService svc;
    WfTwinPrediction pred = svc.predictBehavior({});
    QCOMPARE(pred.forecast.size(), 5);
    QCOMPARE(pred.confidence, 0.5);
  }

  void testRemoveTwin() {
    WorkflowDigitalTwinService svc;
    svc.createDigitalTwin(1);
    QVERIFY(svc.removeTwin(1));
    QCOMPARE(svc.allTwins().size(), 0);
  }

  void testRemoveTwinNotFound() {
    WorkflowDigitalTwinService svc;
    QVERIFY(!svc.removeTwin(99));
  }

  void testAllTwins() {
    WorkflowDigitalTwinService svc;
    QCOMPARE(svc.allTwins().size(), 0);
    svc.createDigitalTwin(1);
    svc.createDigitalTwin(2);
    svc.createDigitalTwin(3);
    QCOMPARE(svc.allTwins().size(), 3);
  }

  void testDigitalTwinCreatedSignal() {
    WorkflowDigitalTwinService svc;
    QSignalSpy spy(&svc, &WorkflowDigitalTwinService::digitalTwinCreated);
    QVERIFY(spy.isValid());
    svc.createDigitalTwin(1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
  }

  void testSyncCompletedSignal() {
    WorkflowDigitalTwinService svc;
    svc.createDigitalTwin(1);
    QSignalSpy spy(&svc, &WorkflowDigitalTwinService::syncCompleted);
    QVERIFY(spy.isValid());
    svc.syncWithPhysical(1);
    QCOMPARE(spy.count(), 1);
  }

  void testSimulationFinishedSignal() {
    WorkflowDigitalTwinService svc;
    QSignalSpy spy(&svc, &WorkflowDigitalTwinService::simulationFinished);
    QVERIFY(spy.isValid());
    WfTwinScenario scenario;
    scenario.name = "test";
    svc.simulateScenario(scenario);
    QCOMPARE(spy.count(), 1);
  }

  void testSyncUpdatesState() {
    WorkflowDigitalTwinService svc;
    svc.createDigitalTwin(1);
    WfDigitalTwin before = svc.twin(1);
    QCOMPARE(before.syncStatus, WfTwinSyncStatus::Never);
    svc.syncWithPhysical(1);
    WfDigitalTwin after = svc.twin(1);
    QCOMPARE(after.syncStatus, WfTwinSyncStatus::Synced);
    QVERIFY(after.lastSync.isValid());
  }

  void testMultipleSimulations() {
    WorkflowDigitalTwinService svc;
    for (int i = 0; i < 5; i++) {
      WfTwinScenario scenario;
      scenario.name = QString("scenario_%1").arg(i);
      WfTwinSimulationResult result = svc.simulateScenario(scenario);
      QVERIFY(result.success);
    }
  }

  void testPredictConfidence() {
    WorkflowDigitalTwinService svc;
    QVector<WfTwinDataPoint> shortData;
    for (int i = 0; i < 3; i++) {
      WfTwinDataPoint dp;
      dp.value = i;
      shortData.append(dp);
    }
    WfTwinPrediction predShort = svc.predictBehavior(shortData);
    QCOMPARE(predShort.confidence, 0.5);

    QVector<WfTwinDataPoint> longData;
    for (int i = 0; i < 10; i++) {
      WfTwinDataPoint dp;
      dp.value = i;
      longData.append(dp);
    }
    WfTwinPrediction predLong = svc.predictBehavior(longData);
    QVERIFY(predLong.confidence > 0.5);
  }
};

QTEST_MAIN(WorkflowDigitalTwinServiceTest)
#include "workflow_digital_twin_service_test.moc"
