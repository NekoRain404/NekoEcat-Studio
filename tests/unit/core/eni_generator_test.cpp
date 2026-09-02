// Tests for EniGenerator — ENI XML generation from slave configs.

#include <QTest>
#include <QXmlStreamReader>

#include "EniGenerator.h"

class EniGeneratorTest : public QObject {
    Q_OBJECT
private slots:
    // Empty generator produces well-formed XML with a master but no slaves.
    void testEmptyDocumentWellFormed() {
        EniGenerator gen;
        gen.setMasterName("Test Master");
        const QString xml = gen.generate();

        QVERIFY(xml.contains("<EtherCATConfig"));
        QVERIFY(xml.contains("Test Master"));
        QVERIFY(xml.contains("</EtherCATConfig>"));

        // Must be parseable XML.
        QXmlStreamReader reader(xml);
        while (!reader.atEnd())
            reader.readNext();
        QVERIFY(!reader.hasError());
    }

    // A single slave is rendered with identity fields.
    void testSingleSlaveIdentity() {
        EniGenerator gen;
        EniSlaveConfig cfg;
        cfg.position = 0;
        cfg.name = "EK1100";
        cfg.vendorId = 2;
        cfg.productCode = 0x044c2c52;
        cfg.revisionNo = 0x00110000;
        gen.addSlave(cfg);

        const QString xml = gen.generate();
        QVERIFY(xml.contains("EK1100"));
        QVERIFY(xml.contains("<VendorId>2</VendorId>"));
        QVERIFY(xml.contains(QString("<ProductCode>%1</ProductCode>").arg(0x044c2c52u)));
        QCOMPARE(gen.slaveCount(), 1);

        QXmlStreamReader reader(xml);
        while (!reader.atEnd())
            reader.readNext();
        QVERIFY(!reader.hasError());
    }

    // Mailbox protocols are emitted only when supported.
    void testMailboxProtocols() {
        EniGenerator gen;
        EniSlaveConfig cfg;
        cfg.position = 0;
        cfg.name = "Servo";
        cfg.supportsCoE = true;
        cfg.supportsSoE = true;
        gen.addSlave(cfg);

        const QString xml = gen.generate();
        QVERIFY(xml.contains("<CoE/>"));
        QVERIFY(xml.contains("<SoE/>"));
        QVERIFY(!xml.contains("<EoE/>"));
        QVERIFY(!xml.contains("<FoE/>"));
    }

    // PDO mappings produce RxPdo/TxPdo elements with entries.
    void testPdoMapping() {
        EniGenerator gen;
        EniSlaveConfig cfg;
        cfg.position = 1;
        cfg.name = "EL2008";

        EniPdo pdo;
        pdo.index = 0x1600;
        pdo.name = "Outputs";
        pdo.isTxPdo = false;
        EniPdoEntry entry;
        entry.index = 0x7000;
        entry.subIndex = 1;
        entry.bitLength = 1;
        entry.name = "Output 1";
        pdo.entries.append(entry);
        cfg.pdos.append(pdo);
        gen.addSlave(cfg);

        const QString xml = gen.generate();
        QVERIFY(xml.contains("<RxPdo"));
        QVERIFY(xml.contains("#x1600"));
        QVERIFY(xml.contains("#x7000"));
        QVERIFY(xml.contains("<BitLen>1</BitLen>"));
        QVERIFY(xml.contains("Output 1"));

        QXmlStreamReader reader(xml);
        while (!reader.atEnd())
            reader.readNext();
        QVERIFY(!reader.hasError());
    }

    // Startup commands produce InitCmd elements.
    void testStartupCommands() {
        EniGenerator gen;
        EniSlaveConfig cfg;
        cfg.position = 0;
        cfg.name = "Drive";
        EniStartupCommand cmd;
        cmd.transition = "PS";
        cmd.index = 0x6060;
        cmd.subIndex = 0;
        cmd.dataHex = "08";
        cmd.comment = "Mode of operation = CSP";
        cfg.startupCmds.append(cmd);
        gen.addSlave(cfg);

        const QString xml = gen.generate();
        QVERIFY(xml.contains("<InitCmd>"));
        QVERIFY(xml.contains("<Transition>PS</Transition>"));
        QVERIFY(xml.contains("#x6060"));
        QVERIFY(xml.contains("Mode of operation"));
    }

    // XML special characters in names are escaped.
    void testXmlEscaping() {
        EniGenerator gen;
        EniSlaveConfig cfg;
        cfg.position = 0;
        cfg.name = "A & B <Drive>";
        gen.addSlave(cfg);

        const QString xml = gen.generate();
        QVERIFY(xml.contains("A &amp; B &lt;Drive&gt;"));
        QVERIFY(!xml.contains("A & B <Drive>"));

        QXmlStreamReader reader(xml);
        while (!reader.atEnd())
            reader.readNext();
        QVERIFY(!reader.hasError());
    }

    // Multiple slaves get distinct physical addresses.
    void testMultipleSlavesAddresses() {
        EniGenerator gen;
        for (int i = 0; i < 3; ++i) {
            EniSlaveConfig cfg;
            cfg.position = i;
            cfg.name = QString("Slave%1").arg(i);
            gen.addSlave(cfg);
        }
        const QString xml = gen.generate();
        QCOMPARE(gen.slaveCount(), 3);
        QVERIFY(xml.contains("<PhysAddr>1001</PhysAddr>"));
        QVERIFY(xml.contains("<PhysAddr>1002</PhysAddr>"));
        QVERIFY(xml.contains("<PhysAddr>1003</PhysAddr>"));

        QXmlStreamReader reader(xml);
        while (!reader.atEnd())
            reader.readNext();
        QVERIFY(!reader.hasError());
    }

    // clear() resets the builder.
    void testClear() {
        EniGenerator gen;
        EniSlaveConfig cfg;
        cfg.position = 0;
        cfg.name = "Slave";
        gen.addSlave(cfg);
        QCOMPARE(gen.slaveCount(), 1);
        gen.clear();
        QCOMPARE(gen.slaveCount(), 0);
    }
};

QTEST_MAIN(EniGeneratorTest)
#include "eni_generator_test.moc"
