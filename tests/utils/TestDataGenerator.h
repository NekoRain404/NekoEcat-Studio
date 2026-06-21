#pragma once

/// @brief Utility for generating test data with configurable patterns.
///
/// @details TestDataGenerator provides static factory methods for creating
/// realistic test data (SlaveInfo, SDO values, PDO mappings, topologies)
/// with different data patterns. This enables systematic testing of edge
/// cases, boundary conditions, and error scenarios.
///
/// Data Patterns:
///   - **Random**: Random values within valid ranges
///   - **Sequential**: Incrementing values starting from 0
///   - **Boundary**: Edge-case values (0, max, min)
///   - **Error**: Invalid/malformed values for error handling tests
///
/// Usage:
/// @code
///   auto slaves = TestDataGenerator::generateSlaveInfo(10, DataPattern::Sequential);
///   QCOMPARE(slaves.size(), 10);
///   QCOMPARE(slaves[0].position, 0);
///
///   auto topology = TestDataGenerator::generateTopology(3, 4);
///   QCOMPARE(topology.size(), 12);  // depth=3, breadth=4
/// @endcode
///
/// @par Test Coverage
///   - SlaveInfo generation with various patterns
///   - SDO value generation for dictionary tests
///   - PDO mapping generation for process data tests
///   - Topology generation for tree/graph tests
///
/// @see SlaveInfo, SdoValue, PdoMapping

#include "EthercatTypes.h"

#include <QString>
#include <QVector>

/// Represents an SDO value for test data generation.
struct SdoValue {
    int position = 0;      ///< Slave position on the bus
    QString index;         ///< SDO index (e.g. "0x6040")
    QString subIndex;      ///< SDO subindex (e.g. "0x00")
    QString value;         ///< SDO value as string
    QString type;          ///< SDO data type (e.g. "UINT16", "INT32")
};

/// Represents a PDO mapping entry for test data generation.
struct PdoMapping {
    int position = 0;      ///< Slave position on the bus
    QString syncManager;   ///< Sync manager index (e.g. "SM2", "SM3")
    QString pdoIndex;      ///< PDO index (e.g. "0x1600")
    QString index;         ///< Mapped object index (e.g. "0x6040")
    QString subIndex;      ///< Mapped object subindex (e.g. "0x00")
    int bits = 0;          ///< Bit length of the mapping
    QString name;          ///< Object name
};

/// Enumerates the available data generation patterns.
enum class DataPattern {
    Random,     ///< Random values within valid ranges
    Sequential, ///< Incrementing values starting from 0
    Boundary,   ///< Edge-case values (0, max, min)
    Error,      ///< Invalid/malformed values for error handling tests
};

class TestDataGenerator {
public:
    /// Generates a list of SlaveInfo entries with the specified pattern.
    /// @param count    Number of slaves to generate
    /// @param pattern  Data generation pattern
    static QVector<SlaveInfo> generateSlaveInfo(int count, DataPattern pattern = DataPattern::Sequential);
    /// Generates a list of SDO values with the specified pattern.
    /// @param count    Number of SDO values to generate
    /// @param pattern  Data generation pattern
    static QVector<SdoValue> generateSdoValues(int count, DataPattern pattern = DataPattern::Sequential);
    /// Generates a list of PDO mappings with the specified pattern.
    /// @param count    Number of PDO mappings to generate
    /// @param pattern  Data generation pattern
    static QVector<PdoMapping> generatePdoMappings(int count, DataPattern pattern = DataPattern::Sequential);
    /// Generates a tree topology with the specified depth and breadth.
    /// @param depth    Number of levels in the tree
    /// @param breadth  Number of children per node
    static QVector<SlaveInfo> generateTopology(int depth, int breadth);

private:
    static SlaveInfo makeSlave(int position, DataPattern pattern);
    static SdoValue makeSdo(int position, int index, DataPattern pattern);
    static PdoMapping makePdo(int position, int index, DataPattern pattern);
};
