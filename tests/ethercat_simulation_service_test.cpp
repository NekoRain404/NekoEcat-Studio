// EtherCATSimulationServiceTest — Tests for EtherCATSimulationService
//
// Test coverage:
//   - Virtual slave creation, removal, and duplicate handling
//   - Simulation start/stop (including edge cases)
//   - Virtual slave lookup and configuration
//   - Simulation state tracking with multiple slaves

#include <QTest>
#include "services/EtherCATSimulationService.h"

class EtherCATSimulationServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify default simulation state is not running with zero slaves
  // Default state: not running, zero slaves
  void testDefaultState() {
    EtherCATSimulationService svc;
    SimulationState state = svc.simulationState();
    QVERIFY(!state.running);
    QCOMPARE(state.slaveCount, 0);
    QCOMPARE(state.frameCount, 0);
    QCOMPARE(state.errorCount, 0);
  }

  // Create a virtual slave and verify position and config
  // Create virtual slave at position
  void testCreateVirtualSlave() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 1;
    cfg.name = "TestSlave";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 4;
    cfg.outputSize = 4;
    int pos = svc.createVirtualSlave(cfg);
    QCOMPARE(pos, 1);
    QCOMPARE(svc.virtualSlaves().size(), 1);
    QCOMPARE(svc.virtualSlaves().at(0).config.name, QString("TestSlave"));
  }

  // Duplicate slave position returns -1
  // Duplicate slave position returns -1
  void testCreateDuplicateSlave() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 1;
    cfg.name = "Slave1";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 4;
    cfg.outputSize = 4;
    cfg.cycleTimeUs = 1000;
    svc.createVirtualSlave(cfg);
    int pos = svc.createVirtualSlave(cfg);
    QCOMPARE(pos, -1);
    QCOMPARE(svc.virtualSlaves().size(), 1);
  }

  // Remove a virtual slave and verify empty list
  // Remove existing virtual slave
  void testRemoveVirtualSlave() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 1;
    cfg.name = "Slave1";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 4;
    cfg.outputSize = 4;
    cfg.cycleTimeUs = 1000;
    svc.createVirtualSlave(cfg);
    bool ok = svc.removeVirtualSlave(1);
    QVERIFY(ok);
    QCOMPARE(svc.virtualSlaves().size(), 0);
  }

  // Remove nonexistent slave returns false
  // Remove nonexistent slave fails
  void testRemoveNonexistentSlave() {
    EtherCATSimulationService svc;
    bool ok = svc.removeVirtualSlave(99);
    QVERIFY(!ok);
  }

  // Start simulation with no slaves fails
  // Start simulation fails with no slaves
  void testStartSimulationNoSlaves() {
    EtherCATSimulationService svc;
    bool ok = svc.startSimulation();
    QVERIFY(!ok);
  }

  // Start simulation with a slave and verify running state
  // Start simulation with slave succeeds
  void testStartSimulation() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 1;
    cfg.name = "Slave1";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 4;
    cfg.outputSize = 4;
    cfg.cycleTimeUs = 1000;
    svc.createVirtualSlave(cfg);
    bool ok = svc.startSimulation();
    QVERIFY(ok);
    QVERIFY(svc.simulationState().running);
    QCOMPARE(svc.simulationState().slaveCount, 1);
  }

  // Start simulation when already running fails
  // Start already running simulation fails
  void testStartAlreadyRunning() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 1;
    cfg.name = "Slave1";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 4;
    cfg.outputSize = 4;
    cfg.cycleTimeUs = 1000;
    svc.createVirtualSlave(cfg);
    svc.startSimulation();
    bool ok = svc.startSimulation();
    QVERIFY(!ok);
  }

  // Stop running simulation and verify stopped state
  // Stop running simulation
  void testStopSimulation() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 1;
    cfg.name = "Slave1";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 4;
    cfg.outputSize = 4;
    cfg.cycleTimeUs = 1000;
    svc.createVirtualSlave(cfg);
    svc.startSimulation();
    bool ok = svc.stopSimulation();
    QVERIFY(ok);
    QVERIFY(!svc.simulationState().running);
  }

  // Stop when not running returns false
  // Stop when not running fails
  void testStopNotRunning() {
    EtherCATSimulationService svc;
    bool ok = svc.stopSimulation();
    QVERIFY(!ok);
  }

  // Look up virtual slave by position and verify config
  // Lookup virtual slave by position
  void testVirtualSlaveAt() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 5;
    cfg.name = "Slave5";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 8;
    cfg.outputSize = 8;
    cfg.cycleTimeUs = 2000;
    svc.createVirtualSlave(cfg);
    VirtualSlave s = svc.virtualSlaveAt(5);
    QCOMPARE(s.config.position, 5);
    QCOMPARE(s.config.name, QString("Slave5"));
    QCOMPARE(s.config.inputSize, 8);
    QVERIFY(s.online);
  }

  // Verify simulation state fields after start
  // Simulation state reflects running and config
  void testSimulationState() {
    EtherCATSimulationService svc;
    SimulationSlaveConfig cfg;
    cfg.position = 1;
    cfg.name = "Slave1";
    cfg.vendorId = "0x00000001";
    cfg.productCode = "0x00000002";
    cfg.inputSize = 4;
    cfg.outputSize = 4;
    cfg.cycleTimeUs = 1000;
    svc.createVirtualSlave(cfg);
    svc.startSimulation();
    SimulationState state = svc.simulationState();
    QVERIFY(state.running);
    QCOMPARE(state.slaveCount, 1);
    QCOMPARE(state.cycleTimeUs, 1000);
    QVERIFY(state.timestampMs > 0);
  }

  // Create multiple slaves and verify count and state
  // Multiple slaves in simulation
  void testMultipleSlaves() {
    EtherCATSimulationService svc;
    for (int i = 1; i <= 5; i++) {
      SimulationSlaveConfig cfg;
      cfg.position = i;
      cfg.name = QString("Slave%1").arg(i);
      cfg.vendorId = "0x00000001";
      cfg.productCode = "0x00000002";
      cfg.inputSize = 4;
      cfg.outputSize = 4;
      cfg.cycleTimeUs = 1000;
      svc.createVirtualSlave(cfg);
    }
    QCOMPARE(svc.virtualSlaves().size(), 5);
    svc.startSimulation();
    QCOMPARE(svc.simulationState().slaveCount, 5);
  }
};

QTEST_MAIN(EtherCATSimulationServiceTest)
#include "ethercat_simulation_service_test.moc"
