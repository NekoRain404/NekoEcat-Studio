#pragma once

// EniGenerator — generates EtherCAT Network Information (ENI) XML.
//
// ENI (EtherCAT Network Information, ETG.2100) is the standardized XML file
// that describes a configured EtherCAT network: the master configuration and
// each slave's identity, mailbox protocols, process data, and initialization
// commands. EtherCAT masters consume an ENI file to bring up the bus.
//
// This generator builds an ENI document from scanned topology (SlaveInfo) plus
// optional per-slave detail (vendor/product IDs, PDO mappings, startup SDOs).
// The output conforms to the EtherCATInfo/EtherCATConfig schema structure used
// by mainstream tools, sufficient for import into TwinCAT and other masters.
//
// Usage:
//   EniGenerator gen;
//   gen.setMasterName("NekoEcat Master");
//   for (const auto &slave : slaves) {
//       EniSlaveConfig cfg;
//       cfg.position = slave.position;
//       cfg.name = slave.name;
//       cfg.vendorId = ...; cfg.productCode = ...;
//       gen.addSlave(cfg);
//   }
//   QString xml = gen.generate();
//
// Thread safety:
//   Not thread-safe. Use from a single thread; the generator holds mutable
//   builder state between addSlave() and generate().

#include <QString>
#include <QVector>

// Startup command: an SDO write applied during a state transition.
struct EniStartupCommand {
    QString transition;   // e.g. "PS" (PreOp->SafeOp), "SO" (SafeOp->Op)
    quint16 index = 0;    // SDO index
    quint8 subIndex = 0;  // SDO sub-index
    QString dataHex;      // Value as hex bytes (no 0x prefix)
    QString comment;      // Human-readable comment
};

// PDO entry within a sync manager assignment.
struct EniPdoEntry {
    quint16 index = 0;     // Object index
    quint8 subIndex = 0;   // Object sub-index
    quint8 bitLength = 0;  // Entry bit length
    QString name;          // Entry name
};

// A PDO (RxPdo/TxPdo) with its entries.
struct EniPdo {
    quint16 index = 0;          // PDO index (e.g. 0x1600 RxPdo, 0x1A00 TxPdo)
    QString name;               // PDO name
    bool isTxPdo = false;       // true = TxPdo (input), false = RxPdo (output)
    QVector<EniPdoEntry> entries;
};

// Full per-slave ENI configuration.
struct EniSlaveConfig {
    int position = -1;          // Bus position
    quint16 alias = 0;          // Configured station alias (0 if none)
    QString name;               // Product/device name
    quint32 vendorId = 0;       // Vendor ID
    quint32 productCode = 0;    // Product code
    quint32 revisionNo = 0;     // Revision number

    // Mailbox protocol support flags.
    bool supportsCoE = false;
    bool supportsEoE = false;
    bool supportsFoE = false;
    bool supportsSoE = false;

    QVector<EniPdo> pdos;                   // PDO mappings
    QVector<EniStartupCommand> startupCmds; // Init/startup SDO commands
};

class EniGenerator {
public:
    EniGenerator() = default;

    // Set the master device name written into the ENI <Config><Master> section.
    void setMasterName(const QString &name) { masterName_ = name; }

    // Set the cycle time (microseconds) for the cyclic process data frame.
    void setCycleTimeUs(int us) { cycleTimeUs_ = us; }

    // Add a slave configuration to the document.
    void addSlave(const EniSlaveConfig &slave);

    // Clear all accumulated slave configurations.
    void clear();

    // Number of slaves added so far.
    int slaveCount() const { return slaves_.size(); }

    // Generate the ENI XML document as a string. Does not modify builder state,
    // so it may be called multiple times.
    QString generate() const;

private:
    QString masterName_ = "EtherCAT Master";
    int cycleTimeUs_ = 1000;
    QVector<EniSlaveConfig> slaves_;

    // Build the <Slave> element for one slave config.
    QString buildSlaveElement(const EniSlaveConfig &slave) const;

    // Build the <ProcessData> sync/PDO sub-tree for one slave.
    QString buildProcessData(const EniSlaveConfig &slave) const;

    // Build the <Mailbox> element listing supported protocols.
    QString buildMailbox(const EniSlaveConfig &slave) const;

    // Build the <InitCmds> from startup commands for a transition group.
    QString buildInitCmds(const EniSlaveConfig &slave) const;

    // XML-escape a string for safe element/attribute content.
    static QString xmlEscape(const QString &text);
};
