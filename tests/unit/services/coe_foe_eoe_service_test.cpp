// CoEFoEEoEServiceTest — Tests for CoEService, FoEService, and EoEService
//
// Test coverage:
//   - CoE SDO info upload
//   - CoE dictionary upload
//   - CoE segment upload/download
//   - FoE file read/write
//   - EoE IP configuration
//   - Signal emissions for all protocols

#include "infra/EcatClient.h"
#include "services/CoEService.h"
#include "services/EoEService.h"
#include "services/FoEService.h"
#include "services/SdoCacheService.h"
#include <QFile>
#include <QSignalSpy>
#include <QTest>

class CoEFoEEoEServiceTest : public QObject {
    Q_OBJECT
private slots:
    // ── CoEService tests ──────────────────────────────────────────────
    // Verify SDO info upload returns correct vendor/product data
    void testCoESdoInfo() {
        EcatClient client;
        SdoCacheService sdoCache;
        CoEService coe(&client, &sdoCache);
        QSignalSpy spy(&coe, &CoEService::sdoInfoReceived);
        QSignalSpy errorSpy(&coe, &CoEService::error);
        coe.uploadSdoInfo(1);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify dictionary upload returns correct entries
    void testCoEDictionary() {
        EcatClient client;
        SdoCacheService sdoCache;
        CoEService coe(&client, &sdoCache);
        QSignalSpy spy(&coe, &CoEService::dictionaryReceived);
        QSignalSpy errorSpy(&coe, &CoEService::error);
        coe.uploadDictionary(1);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify segment upload returns correct data size
    void testCoESegmentUpload() {
        EcatClient client;
        SdoCacheService sdoCache;
        CoEService coe(&client, &sdoCache);
        QSignalSpy spy(&coe, &CoEService::segmentReceived);
        QSignalSpy errorSpy(&coe, &CoEService::error);
        coe.uploadSegment(1, "0x1000", 0, 64);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
    }

    // Verify segment download succeeds
    void testCoESegmentDownload() {
        EcatClient client;
        SdoCacheService sdoCache;
        CoEService coe(&client, &sdoCache);
        QSignalSpy spy(&coe, &CoEService::segmentDownloaded);
        QSignalSpy errorSpy(&coe, &CoEService::error);
        coe.downloadSegment(1, "0x1000", 0, QByteArray(64, 0));
        QCOMPARE(spy.count(), 1);
        QVERIFY(!spy.at(0).at(2).toBool());
        QCOMPARE(errorSpy.count(), 1);
    }

    void testCoEDisconnectedClientDoesNotReportProtocolSuccess() {
        EcatClient client;
        SdoCacheService sdoCache;
        QVERIFY(!client.isConnected());
        CoEService coe(&client, &sdoCache);
        QSignalSpy sdoInfoSpy(&coe, &CoEService::sdoInfoReceived);
        QSignalSpy dictionarySpy(&coe, &CoEService::dictionaryReceived);
        QSignalSpy segmentSpy(&coe, &CoEService::segmentReceived);
        QSignalSpy downloadSpy(&coe, &CoEService::segmentDownloaded);
        QSignalSpy errorSpy(&coe, &CoEService::error);

        coe.uploadSdoInfo(1);
        coe.uploadDictionary(1);
        coe.uploadSegment(1, "0x1000", 0, 64);
        coe.downloadSegment(1, "0x1000", 0, QByteArray(64, 0));

        QCOMPARE(sdoInfoSpy.count(), 0);
        QCOMPARE(dictionarySpy.count(), 0);
        QCOMPARE(segmentSpy.count(), 0);
        QCOMPARE(downloadSpy.count(), 1);
        QVERIFY(!downloadSpy.at(0).at(2).toBool());
        QVERIFY(errorSpy.count() >= 4);
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
        // macListReceived was removed — learnedMacs() only emits error
        QSignalSpy errorSpy(&eoe, &EoEService::error);
        eoe.learnedMacs(1);
        QCOMPARE(errorSpy.count(), 1);
    }

    void testEoEDisconnectedClientDoesNotReportProtocolSuccess() {
        EcatClient client;
        QVERIFY(!client.isConnected());
        EoEService eoe(&client);
        QSignalSpy frameSentSpy(&eoe, &EoEService::frameSent);
        QSignalSpy frameReceivedSpy(&eoe, &EoEService::frameReceived);
        QSignalSpy ipSpy(&eoe, &EoEService::ipConfigured);
        QSignalSpy errorSpy(&eoe, &EoEService::error);

        QVERIFY(!eoe.sendEthernetFrame(1, QByteArray(64, 0xFF)));
        eoe.receiveEthernetFrame(1);
        QVERIFY(!eoe.configureIp(1, "192.168.1.100", "255.255.255.0"));
        eoe.learnedMacs(1);

        QCOMPARE(frameSentSpy.count(), 1);
        QVERIFY(!frameSentSpy.at(0).at(1).toBool());
        QCOMPARE(frameReceivedSpy.count(), 0);
        QCOMPARE(ipSpy.count(), 0);
        QVERIFY(errorSpy.count() >= 4);
    }

    void testProtocolErrorsDoNotExposeNotImplementedText() {
        const QStringList paths = {
            QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/CoEService.cpp"),
            QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/FoEService.cpp"),
            QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/EoEService.cpp"),
        };

        for (const QString& path : paths) {
            QFile source(path);
            QVERIFY2(source.open(QIODevice::ReadOnly | QIODevice::Text),
                     qPrintable(QStringLiteral("Unable to open %1").arg(path)));
            const QString text = QString::fromUtf8(source.readAll());
            QVERIFY2(!text.contains(QStringLiteral("not implemented"), Qt::CaseInsensitive),
                     qPrintable(QStringLiteral("%1 must not expose unfinished implementation wording.").arg(path)));
        }
    }
};

QTEST_MAIN(CoEFoEEoEServiceTest)
#include "coe_foe_eoe_service_test.moc"
