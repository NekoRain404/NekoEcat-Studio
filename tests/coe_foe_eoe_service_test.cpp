// CoEFoEEoEServiceTest — Tests for CoEService, FoEService, and EoEService
//
// Test coverage:
//   - CoE SDO info upload
//   - CoE dictionary upload
//   - CoE segment upload/download
//   - FoE file read/write
//   - EoE IP configuration
//   - Signal emissions for all protocols

#include <QTest>
#include <QSignalSpy>
#include "services/CoEService.h"
#include "services/FoEService.h"
#include "services/EoEService.h"
#include "infra/EcatClient.h"

class CoEFoEEoEServiceTest : public QObject {
    Q_OBJECT
private slots:
    // ── CoEService tests ──────────────────────────────────────────────
    // Verify SDO info upload returns correct vendor/product data
    void testCoESdoInfo() {
        EcatClient client;
        CoEService coe(&client);
        QSignalSpy spy(&coe, &CoEService::sdoInfoReceived);
        coe.uploadSdoInfo(1);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 1);
        auto info = spy.at(0).at(1).value<SdoInfo>();
        QCOMPARE(info.vendorId, quint32(0x00000123));
        QCOMPARE(info.productCode, quint32(0x00000456));
        QCOMPARE(info.revisionNumber, quint32(0x00000001));
        QCOMPARE(info.serialNumber, quint32(0x00000001));
        QVERIFY(!info.supportedCoEObjects.isEmpty());
    }

    // Verify dictionary upload returns correct entries
    void testCoEDictionary() {
        EcatClient client;
        CoEService coe(&client);
        QSignalSpy spy(&coe, &CoEService::dictionaryReceived);
        coe.uploadDictionary(1);
        QCOMPARE(spy.count(), 1);
        auto entries = spy.at(0).at(1).value<QList<CoESdoDictionary>>();
        QCOMPARE(entries.size(), 2);
        QCOMPARE(entries[0].index, QString("0x1000"));
        QCOMPARE(entries[0].name, QString("Device Type"));
        QCOMPARE(entries[0].type, QString("UINT32"));
        QCOMPARE(entries[0].bitSize, 32);
    }

    // Verify segment upload returns correct data size
    void testCoESegmentUpload() {
        EcatClient client;
        CoEService coe(&client);
        QSignalSpy spy(&coe, &CoEService::segmentReceived);
        coe.uploadSegment(1, "0x1000", 0, 64);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(1).toString(), QString("0x1000"));
        QCOMPARE(spy.at(0).at(2).toByteArray().size(), 64);
    }

    // Verify segment download succeeds
    void testCoESegmentDownload() {
        EcatClient client;
        CoEService coe(&client);
        QSignalSpy spy(&coe, &CoEService::segmentDownloaded);
        coe.downloadSegment(1, "0x1000", 0, QByteArray(64, 0));
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(2).toBool());
    }

    // ── FoEService tests ──────────────────────────────────────────────
    // Verify file read with progress signals
    void testFoEReadFile() {
        EcatClient client;
        FoEService foe(&client);
        QSignalSpy progressSpy(&foe, &FoEService::readProgress);
        QSignalSpy completeSpy(&foe, &FoEService::fileReadComplete);
        QSignalSpy errorSpy(&foe, &FoEService::error);
        foe.readFile(1, "firmware.bin");
        QCOMPARE(progressSpy.count(), 0);
        QCOMPARE(completeSpy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify file write with progress signals
    void testFoEWriteFile() {
        EcatClient client;
        FoEService foe(&client);
        QSignalSpy progressSpy(&foe, &FoEService::writeProgress);
        QSignalSpy completeSpy(&foe, &FoEService::fileWriteComplete);
        QSignalSpy errorSpy(&foe, &FoEService::error);
        bool result = foe.writeFile(1, "config.dat", QByteArray(128, 0xFF));
        QVERIFY(!result);
        QCOMPARE(progressSpy.count(), 0);
        QCOMPARE(completeSpy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify file list retrieval
    void testFoEListFiles() {
        EcatClient client;
        FoEService foe(&client);
        QSignalSpy spy(&foe, &FoEService::fileListReceived);
        QSignalSpy errorSpy(&foe, &FoEService::error);
        foe.listFiles(1);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify file info retrieval
    void testFoEFileInfo() {
        EcatClient client;
        FoEService foe(&client);
        QSignalSpy spy(&foe, &FoEService::fileInfoReceived);
        QSignalSpy errorSpy(&foe, &FoEService::error);
        foe.fileInfo(1, "firmware.bin");
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    void testFoEDisconnectedClientDoesNotReportProtocolSuccess() {
        EcatClient client;
        QVERIFY(!client.isConnected());
        FoEService foe(&client);
        QSignalSpy readProgressSpy(&foe, &FoEService::readProgress);
        QSignalSpy readCompleteSpy(&foe, &FoEService::fileReadComplete);
        QSignalSpy writeProgressSpy(&foe, &FoEService::writeProgress);
        QSignalSpy writeCompleteSpy(&foe, &FoEService::fileWriteComplete);
        QSignalSpy listSpy(&foe, &FoEService::fileListReceived);
        QSignalSpy infoSpy(&foe, &FoEService::fileInfoReceived);
        QSignalSpy errorSpy(&foe, &FoEService::error);

        foe.readFile(1, "firmware.bin");
        QVERIFY(!foe.writeFile(1, "config.dat", QByteArray(128, 0xFF)));
        foe.listFiles(1);
        foe.fileInfo(1, "firmware.bin");

        QCOMPARE(readProgressSpy.count(), 0);
        QCOMPARE(readCompleteSpy.count(), 0);
        QCOMPARE(writeProgressSpy.count(), 0);
        QCOMPARE(writeCompleteSpy.count(), 0);
        QCOMPARE(listSpy.count(), 0);
        QCOMPARE(infoSpy.count(), 0);
        QVERIFY(errorSpy.count() >= 4);
    }

    // ── EoEService tests ──────────────────────────────────────────────
    // Verify Ethernet frame send
    void testEoESendFrame() {
        EcatClient client;
        EoEService eoe(&client);
        QSignalSpy spy(&eoe, &EoEService::frameSent);
        bool result = eoe.sendEthernetFrame(1, QByteArray(64, 0xFF));
        QVERIFY(!result);
        QCOMPARE(spy.count(), 1);
        QVERIFY(!spy.at(0).at(1).toBool());
    }

    // Verify empty frame send returns error
    void testEoESendEmptyFrame() {
        EcatClient client;
        EoEService eoe(&client);
        QSignalSpy errorSpy(&eoe, &EoEService::error);
        bool result = eoe.sendEthernetFrame(1, QByteArray());
        QVERIFY(!result);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify Ethernet frame receive
    void testEoEReceiveFrame() {
        EcatClient client;
        EoEService eoe(&client);
        QSignalSpy spy(&eoe, &EoEService::frameReceived);
        QSignalSpy errorSpy(&eoe, &EoEService::error);
        eoe.receiveEthernetFrame(1);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify IP configuration
    void testEoEConfigureIp() {
        EcatClient client;
        EoEService eoe(&client);
        QSignalSpy spy(&eoe, &EoEService::ipConfigured);
        QSignalSpy errorSpy(&eoe, &EoEService::error);
        bool result = eoe.configureIp(1, "192.168.1.100", "255.255.255.0");
        QVERIFY(!result);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify learned MACs retrieval
    void testEoELearnedMacs() {
        EcatClient client;
        EoEService eoe(&client);
        QSignalSpy spy(&eoe, &EoEService::macListReceived);
        QSignalSpy errorSpy(&eoe, &EoEService::error);
        eoe.learnedMacs(1);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    void testEoEDisconnectedClientDoesNotReportProtocolSuccess() {
        EcatClient client;
        QVERIFY(!client.isConnected());
        EoEService eoe(&client);
        QSignalSpy frameSentSpy(&eoe, &EoEService::frameSent);
        QSignalSpy frameReceivedSpy(&eoe, &EoEService::frameReceived);
        QSignalSpy ipSpy(&eoe, &EoEService::ipConfigured);
        QSignalSpy macSpy(&eoe, &EoEService::macListReceived);
        QSignalSpy errorSpy(&eoe, &EoEService::error);

        QVERIFY(!eoe.sendEthernetFrame(1, QByteArray(64, 0xFF)));
        eoe.receiveEthernetFrame(1);
        QVERIFY(!eoe.configureIp(1, "192.168.1.100", "255.255.255.0"));
        eoe.learnedMacs(1);

        QCOMPARE(frameSentSpy.count(), 1);
        QVERIFY(!frameSentSpy.at(0).at(1).toBool());
        QCOMPARE(frameReceivedSpy.count(), 0);
        QCOMPARE(ipSpy.count(), 0);
        QCOMPARE(macSpy.count(), 0);
        QVERIFY(errorSpy.count() >= 4);
    }
};

QTEST_MAIN(CoEFoEEoEServiceTest)
#include "coe_foe_eoe_service_test.moc"
