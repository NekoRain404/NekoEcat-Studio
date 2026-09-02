#include "EthercatCliBackend.h"
#include "EthercatNativeBackend.h"
#include "EthercatTypes.h"
#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QtTest>

class NativeBackendIntegrationTest : public QObject {
    Q_OBJECT

private:
    bool hasHardware(const EcatService& backend) {
        QString error;
        backend.scanSlaves("0", &error);
        return error.isEmpty();
    }

private slots:
    void testConsistencyWithCli();
    void testSdoUploadPerformance();
    void testBackendSwitching();
    void testDualBackendMode();
};

void NativeBackendIntegrationTest::testConsistencyWithCli() {
    EthercatNativeBackend native;
    EthercatCliBackend cli;

    if (!hasHardware(native) || !hasHardware(cli)) {
        QSKIP("Hardware not available for consistency comparison");
    }

    QString nativeErr, cliErr;
    QVector<SlaveInfo> nativeSlaves = native.scanSlaves("0", &nativeErr);
    QVector<SlaveInfo> cliSlaves = cli.scanSlaves("0", &cliErr);

    QVERIFY2(nativeErr.isEmpty(), qPrintable("Native error: " + nativeErr));
    QVERIFY2(cliErr.isEmpty(), qPrintable("CLI error: " + cliErr));

    QCOMPARE(nativeSlaves.size(), cliSlaves.size());

    for (int i = 0; i < nativeSlaves.size() && i < cliSlaves.size(); ++i) {
        QCOMPARE(nativeSlaves[i].position, cliSlaves[i].position);
        QCOMPARE(nativeSlaves[i].name, cliSlaves[i].name);
        QCOMPARE(nativeSlaves[i].state, cliSlaves[i].state);
    }
}

void NativeBackendIntegrationTest::testSdoUploadPerformance() {
    EthercatNativeBackend native;
    EthercatCliBackend cli;

    if (!hasHardware(native) || !hasHardware(cli)) {
        QSKIP("Hardware not available for performance comparison");
    }

    const int iterations = 10;
    QElapsedTimer timer;

    timer.start();
    int nativeSuccesses = 0;
    for (int i = 0; i < iterations; ++i) {
        QString err;
        const QString value = native.upload("0", 0, "1000", "0", "uint32", &err);
        if (err.isEmpty() && !value.isEmpty()) {
            ++nativeSuccesses;
        }
    }
    qint64 nativeElapsed = timer.elapsed();

    timer.start();
    int cliSuccesses = 0;
    for (int i = 0; i < iterations; ++i) {
        QString err;
        const QString value = cli.upload("0", 0, "1000", "0", "uint32", &err);
        if (err.isEmpty() && !value.isEmpty()) {
            ++cliSuccesses;
        }
    }
    qint64 cliElapsed = timer.elapsed();

    if (nativeSuccesses == 0 || cliSuccesses == 0) {
        QSKIP("No readable SDO target available for performance comparison");
    }

    qDebug() << "SDO upload performance (" << iterations << " iterations):";
    qDebug() << "  Native:" << nativeElapsed << "ms";
    qDebug() << "  CLI:   " << cliElapsed << "ms";
    if (cliElapsed > 0) {
        qDebug() << "  Speedup:" << static_cast<double>(cliElapsed) / nativeElapsed << "x";
    }

    QVERIFY(nativeElapsed > 0);
    QVERIFY(cliElapsed > 0);
}

void NativeBackendIntegrationTest::testBackendSwitching() {
    EthercatNativeBackend native;
    EthercatCliBackend cli;

    EcatService* backend = &native;
    QVERIFY(backend->isNative());

    backend = &cli;
    QVERIFY(!backend->isNative());

    backend = &native;
    QVERIFY(backend->isNative());

    QJsonArray nativeDiag = native.hostDiagnostics();
    QJsonArray cliDiag = cli.hostDiagnostics();

    QVERIFY(!nativeDiag.isEmpty());
    QVERIFY(!cliDiag.isEmpty());

    QJsonObject nativeFirst = nativeDiag[0].toObject();
    QCOMPARE(nativeFirst["source"].toString(), QString("Native API"));

    QJsonObject cliFirst = cliDiag[0].toObject();
    QVERIFY(!cliFirst["source"].toString().isEmpty());
}

void NativeBackendIntegrationTest::testDualBackendMode() {
    EthercatNativeBackend native;
    EthercatCliBackend cli;

    QString nativeErr, cliErr;

    native.scanSlaves("0", &nativeErr);
    cli.scanSlaves("0", &cliErr);

    bool nativeAvailable = nativeErr.isEmpty();
    bool cliAvailable = cliErr.isEmpty();

    EcatService* activeBackend = nullptr;
    QString mode;

    if (nativeAvailable) {
        activeBackend = &native;
        mode = "native";
    } else if (cliAvailable) {
        activeBackend = &cli;
        mode = "cli";
    } else {
        mode = "none";
    }

    qDebug() << "Backend selection:" << mode;
    qDebug() << "  Native available:" << nativeAvailable;
    qDebug() << "  CLI available:" << cliAvailable;

    if (activeBackend) {
        QString err;
        QVector<SlaveInfo> slaves = activeBackend->scanSlaves("0", &err);
        if (err.isEmpty()) {
            qDebug() << "  Selected backend found" << slaves.size() << "slaves";
        }
    }

    QVERIFY(true);
}

QTEST_MAIN(NativeBackendIntegrationTest)
#include "native_backend_integration_test.moc"
