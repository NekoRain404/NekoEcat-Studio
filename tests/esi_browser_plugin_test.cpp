#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

#include "plugins/esi/EsiParser.h"
#include "plugins/esi/EsiDeviceMatcher.h"
#include "services/EsiService.h"

class EsiBrowserPluginTest : public QObject {
    Q_OBJECT
private slots:
    void parserParsesValidEsiXml();
    void parserRejectsInvalidXml();
    void parserExtractsMultipleDevices();
    void parserExtractsSyncManagers();
    void parserValidatesStructure();
    void matcherMatchesByVendorProduct();
    void matcherDetectsNoMatch();
    void matcherGeneratesReport();
    void matcherExactMatch();
    void matcherPartialMatchRevision();
    void parserClearResetsState();
    void serviceImportsAndMatches();
    void parserMatchByVendorProduct();
    void serviceListsDevices();
};

static const char *kTestEsiXml = R"(<?xml version="1.0" encoding="UTF-8"?>
<EtherCATInfo>
  <Descriptions>
    <Device>
      <Type ProductCode="0x00000001" RevisionNo="0x00000001" VendorId="0x00000001">TestDevice</Type>
      <Name>Test Device Alpha</Name>
      <Desc>A test EtherCAT slave device</Desc>
      <Sm Index="0" Dir="out"/>
      <Sm Index="1" Dir="in"/>
      <Sm Index="2" Dir="out"/>
      <Sm Index="3" Dir="in"/>
    </Device>
    <Device>
      <Type ProductCode="0x00000002" RevisionNo="0x00000001" VendorId="0x00000001">TestDevice2</Type>
      <Name>Test Device Beta</Name>
    </Device>
  </Descriptions>
</EtherCATInfo>
)";

static QString writeToTempFile(const QByteArray &data) {
    static QTemporaryDir dir;
    QString path = dir.path() + "/esi_test.xml";
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write(data);
    f.close();
    return path;
}

void EsiBrowserPluginTest::parserParsesValidEsiXml() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    auto result = parser.parseFile(path);
    QVERIFY(result.valid);
    QCOMPARE(parser.deviceCount(), 2);
    auto dev = parser.deviceAt(0);
    QCOMPARE(dev.vendorId, 1);
    QCOMPARE(dev.productCode, 1);
    QCOMPARE(dev.revisionNo, 1);
    QCOMPARE(dev.description, QString("A test EtherCAT slave device"));
    QCOMPARE(dev.syncManagers.size(), 4);
    QVERIFY(!dev.deviceId.isEmpty());
}

void EsiBrowserPluginTest::parserRejectsInvalidXml() {
    EsiParser parser;
    auto result = parser.parseXml("<not valid xml><<<");
    QVERIFY(!result.valid);
}

void EsiBrowserPluginTest::parserExtractsMultipleDevices() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);
    QCOMPARE(parser.deviceCount(), 2);
    QCOMPARE(parser.deviceAt(1).productCode, 2);
}

void EsiBrowserPluginTest::parserExtractsSyncManagers() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);
    auto dev = parser.deviceAt(0);
    QCOMPARE(dev.syncManagers.size(), 4);
    QCOMPARE(dev.syncManagers[0].index, 0);
    QCOMPARE(dev.syncManagers[0].direction, QString("out"));
    QCOMPARE(dev.syncManagers[1].index, 1);
    QCOMPARE(dev.syncManagers[1].direction, QString("in"));
}

void EsiBrowserPluginTest::parserValidatesStructure() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString filePath = dir.path() + "/test.xml";

    QFile file(filePath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    QTextStream out(&file);
    out << kTestEsiXml;
    file.close();

    EsiParser parser;
    QVERIFY(parser.validateStructure(filePath));

    QString badPath = dir.path() + "/bad.xml";
    QFile badFile(badPath);
    QVERIFY(badFile.open(QIODevice::WriteOnly));
    badFile.write("<root><notesi/></root>");
    badFile.close();
    QVERIFY(!parser.validateStructure(badPath));
}

void EsiBrowserPluginTest::matcherMatchesByVendorProduct() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);

    EsiDeviceMatcher matcher(&parser);
    auto result = matcher.matchDevice(1, 1);
    QVERIFY(result.matched);
    QCOMPARE(result.matchLevel, QString("exact"));
    QVERIFY(result.differences.isEmpty());
}

void EsiBrowserPluginTest::matcherDetectsNoMatch() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);

    EsiDeviceMatcher matcher(&parser);
    auto result = matcher.matchDevice(0x99, 0x99);
    QVERIFY(!result.matched);
    QCOMPARE(result.matchLevel, QString("none"));
}

void EsiBrowserPluginTest::matcherGeneratesReport() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);

    EsiDeviceMatcher matcher(&parser);
    QVector<QPair<int, int>> devices = {{1, 1}, {1, 2}, {0x99, 0x99}};
    auto report = matcher.generateReport(devices);
    QCOMPARE(report.totalDevices, 3);
    QCOMPARE(report.matchedDevices, 2);
    QCOMPARE(report.unmatchedDevices, 1);
}

void EsiBrowserPluginTest::matcherExactMatch() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);

    EsiDeviceMatcher matcher(&parser);
    auto result = matcher.matchDevice(1, 1, 1);
    QVERIFY(result.matched);
    QCOMPARE(result.matchLevel, QString("exact"));
    QVERIFY(result.differences.isEmpty());
}

void EsiBrowserPluginTest::matcherPartialMatchRevision() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);

    EsiDeviceMatcher matcher(&parser);
    auto result = matcher.matchDevice(1, 1, 99);
    QVERIFY(result.matched);
    QCOMPARE(result.matchLevel, QString("partial"));
    QCOMPARE(result.differences.size(), 1);
    QVERIFY(result.differences[0].contains("Revision"));
}

void EsiBrowserPluginTest::parserClearResetsState() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);
    QCOMPARE(parser.deviceCount(), 2);
    parser.clear();
    QCOMPARE(parser.deviceCount(), 0);
}

void EsiBrowserPluginTest::serviceImportsAndMatches() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiService service;
    QVERIFY(service.importEsi(path));
    QCOMPARE(service.deviceCount(), 2);

    EsiDeviceInfo dev = service.matchDevice(1, 1);
    QCOMPARE(dev.vendorId, 1);
    QCOMPARE(dev.productCode, 1);

    EsiDeviceInfo noMatch = service.matchDevice(0x99, 0x99);
    QCOMPARE(noMatch.vendorId, 0);
}

void EsiBrowserPluginTest::parserMatchByVendorProduct() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiParser parser;
    parser.parseFile(path);

    EsiDeviceInfo dev = parser.matchDevice(1, 1);
    QCOMPARE(dev.vendorId, 1);
    QCOMPARE(dev.productCode, 1);
    QCOMPARE(dev.revisionNo, 1);

    EsiDeviceInfo dev2 = parser.matchDevice(1, 2);
    QCOMPARE(dev2.vendorId, 1);
    QCOMPARE(dev2.productCode, 2);

    EsiDeviceInfo noMatch = parser.matchDevice(0x99, 0x99);
    QCOMPARE(noMatch.vendorId, 0);
}

void EsiBrowserPluginTest::serviceListsDevices() {
    QString path = writeToTempFile(kTestEsiXml);
    EsiService service;
    service.importEsi(path);

    auto devices = service.listDevices();
    QCOMPARE(devices.size(), 2);
    QVERIFY(!devices[0].deviceId.isEmpty());
    QVERIFY(!devices[1].deviceId.isEmpty());
}

QTEST_MAIN(EsiBrowserPluginTest)
#include "esi_browser_plugin_test.moc"
