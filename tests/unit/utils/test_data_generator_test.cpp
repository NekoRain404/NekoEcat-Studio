// TestDataGeneratorTest — Tests for Test Data Generator Utility
//
// Test coverage:
//   - Slave info generation (sequential pattern)
//   - Slave info generation (boundary pattern)
//   - Slave info generation (error pattern)
//   - SDO value generation
//   - PDO mapping generation
//   - Topology generation with branches
//   - Zero count edge case
#include <QTest>
#include <QSignalSpy>
#include "utils/TestDataGenerator.h"
#include "EthercatTypes.h"

class TestDataGeneratorTest : public QObject {
    Q_OBJECT
private slots:
    // Sequential positions with default OP state
    // Generate sequential slave info with correct positions and names
    void testGenerateSlaveInfoSequential() {
        auto slaves = TestDataGenerator::generateSlaveInfo(5);
        QCOMPARE(slaves.size(), 5);
        QCOMPARE(slaves[0].position, 0);
        QCOMPARE(slaves[4].position, 4);
        QCOMPARE(slaves[0].name, QString("Slave_0"));
        QCOMPARE(slaves[0].state, QString("OP"));
    }

    // Boundary pattern uses max position and empty name
    // Generate boundary-pattern slave info with edge values
    void testGenerateSlaveInfoBoundary() {
        auto slaves = TestDataGenerator::generateSlaveInfo(3, DataPattern::Boundary);
        QCOMPARE(slaves.size(), 3);
        QCOMPARE(slaves[0].position, 0);
        QCOMPARE(slaves[1].position, 255);
        QVERIFY(slaves[0].name.isEmpty());
        QCOMPARE(slaves[0].state, QString("INIT"));
    }

    // Error pattern sets position -1 and ERROR state
    // Generate error-pattern slave info with invalid state
    void testGenerateSlaveInfoError() {
        auto slaves = TestDataGenerator::generateSlaveInfo(1, DataPattern::Error);
        QCOMPARE(slaves.size(), 1);
        QCOMPARE(slaves[0].position, -1);
        QCOMPARE(slaves[0].state, QString("ERROR"));
    }

    // SDO values generated with expected index and type
    // Generate SDO values with correct count and types
    void testGenerateSdoValues() {
        auto sdos = TestDataGenerator::generateSdoValues(10);
        QCOMPARE(sdos.size(), 10);
        QCOMPARE(sdos[0].index, QString("0x6000"));
        QCOMPARE(sdos[0].type, QString("UINT16"));
    }

    // PDO mappings generated with sync manager and bit width
    // Generate PDO mappings with correct sync managers
    void testGeneratePdoMappings() {
        auto pdos = TestDataGenerator::generatePdoMappings(4);
        QCOMPARE(pdos.size(), 4);
        QCOMPARE(pdos[0].syncManager, QString("SM2"));
        QCOMPARE(pdos[0].bits, 16);
    }

    // Topology with branches creates distinct slave names and states
    // Generate topology with branches and verify naming
    void testGenerateTopology() {
        auto slaves = TestDataGenerator::generateTopology(3, 2);
        QCOMPARE(slaves.size(), 6);
        QCOMPARE(slaves[0].name, QString("Branch0_Slave0"));
        QCOMPARE(slaves[0].state, QString("OP"));
        QCOMPARE(slaves[2].name, QString("Branch1_Slave0"));
        QCOMPARE(slaves[2].state, QString("PREOP"));
    }

    // Zero count returns empty list
    // Zero count returns empty list
    void testZeroCount() {
        auto slaves = TestDataGenerator::generateSlaveInfo(0);
        QCOMPARE(slaves.size(), 0);
    }
};

QTEST_MAIN(TestDataGeneratorTest)
#include "test_data_generator_test.moc"
