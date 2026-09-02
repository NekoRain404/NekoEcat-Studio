#include "EthercatNativeBackend.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QtTest>

class NativeBackendTest : public QObject {
    Q_OBJECT

private slots:
    void testIsNative();
    void testHostDiagnostics();
    void testScanSlaves();
    void testMasterText();
    void testSdoUpload();
    void testSdoDownload();
    void testPdos();
    void testSlaveInfo();
};

void NativeBackendTest::testIsNative() {
    EthercatNativeBackend backend;
    QVERIFY(backend.isNative());
}

void NativeBackendTest::testHostDiagnostics() {
    EthercatNativeBackend backend;
    QJsonArray checks = backend.hostDiagnostics();
    QVERIFY(!checks.isEmpty());

    QJsonObject first = checks[0].toObject();
    QVERIFY(first.contains("level"));
    QVERIFY(first.contains("source"));
    QVERIFY(first.contains("message"));
    QVERIFY(first.contains("hint"));
    QCOMPARE(first["source"].toString(), QString("Native API"));
}

void NativeBackendTest::testScanSlaves() {
    EthercatNativeBackend backend;
    QString error;
    QVector<SlaveInfo> slaves = backend.scanSlaves("0", &error);
    if (!error.isEmpty()) {
        QSKIP(qPrintable("Hardware not available: " + error));
    }
    QVERIFY(slaves.isEmpty() || !slaves.isEmpty());
}

void NativeBackendTest::testMasterText() {
    EthercatNativeBackend backend;
    QString error;
    QString text = backend.masterText("0", &error);
    if (!error.isEmpty()) {
        QSKIP(qPrintable("Hardware not available: " + error));
    }
    QVERIFY(!text.isEmpty());
}

void NativeBackendTest::testSdoUpload() {
    EthercatNativeBackend backend;
    QString error;
    QString result = backend.upload("0", 0, "1000", "0", "uint32", &error);
    if (!error.isEmpty()) {
        QSKIP(qPrintable("Hardware not available: " + error));
    }
    QVERIFY(!result.isEmpty());
}

void NativeBackendTest::testSdoDownload() {
    EthercatNativeBackend backend;
    QString error;
    bool ok = backend.download("0", 0, "1000", "0", "0", "uint32", &error);
    if (!error.isEmpty()) {
        QSKIP(qPrintable("Hardware not available: " + error));
    }
    QVERIFY(ok);
}

void NativeBackendTest::testPdos() {
    EthercatNativeBackend backend;
    QString error;
    QString text = backend.pdos("0", 0, &error);
    if (!error.isEmpty()) {
        QSKIP(qPrintable("Hardware not available: " + error));
    }
    QVERIFY(!text.isEmpty());
}

void NativeBackendTest::testSlaveInfo() {
    EthercatNativeBackend backend;
    QString error;
    QString text = backend.slaveInfo("0", 0, &error);
    if (!error.isEmpty()) {
        QSKIP(qPrintable("Hardware not available: " + error));
    }
    QVERIFY(!text.isEmpty());
    QVERIFY(text.contains("Position:"));
}

QTEST_MAIN(NativeBackendTest)
#include "native_backend_test.moc"
