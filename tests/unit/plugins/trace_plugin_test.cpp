// TracePluginTest — Tests for TracePlugin and TraceService
//
// Test coverage:
//   - Plugin identity, display name, and order
//   - Channel add/remove
//   - Sample rate and buffer size configuration
//   - Trigger mode setting
//   - Trace start/stop lifecycle

#include <QTest>
#include <QApplication>
#include <QFile>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTemporaryDir>
#include "services/TraceService.h"
#include "plugins/trace/TracePlugin.h"
#include "MockEcatClient.h"

class TracePluginTest : public QObject {
  Q_OBJECT
private slots:
  // Setup: create TraceService and TracePlugin instances
  void initTestCase() {
    client = new MockEcatClient(this);
    service = new TraceService(client, this);
    plugin = new TracePlugin(service, this);
  }

  // Teardown: destroy plugin and service
  void cleanupTestCase() {
    delete plugin;
    delete service;
  }

  // Plugin id, display name, order, and visibility
  void testPluginIdentity() {
    QCOMPARE(plugin->id(), QString("trace"));
    QCOMPARE(plugin->displayName(), QString("Signal Trace"));
    QCOMPARE(plugin->defaultOrder(), 180);
    QVERIFY(plugin->visible());
  }

  // Add channel returns valid id and updates channel list
  void testAddChannel() {
    int id = service->addChannel("Test CH", 1, "0x6064", "0");
    QVERIFY(id > 0);
    QCOMPARE(service->channels().size(), 1);
    QCOMPARE(service->channels().first().name, QString("Test CH"));
  }

  // Remove channel decrements channel count
  void testRemoveChannel() {
    int id = service->addChannel("Remove CH", 2, "0x6041", "0");
    QCOMPARE(service->channels().size(), 2);
    service->removeChannel(id);
    QCOMPARE(service->channels().size(), 1);
  }

  // Sample rate getter/setter round-trip
  void testSampleRate() {
    service->setSampleRate(5000);
    QCOMPARE(service->sampleRate(), 5000);
  }

  // Buffer size getter/setter round-trip
  void testBufferSize() {
    service->setBufferSize(50000);
    QCOMPARE(service->bufferSize(), 50000);
  }

  // Trigger mode getter/setter round-trip
  void testTriggerMode() {
    service->setTriggerMode(TraceTriggerMode::Rising);
    QCOMPARE(service->triggerMode(), TraceTriggerMode::Rising);
  }

  // Start and stop trace toggles isTracing flag
  void testTraceLifecycleFailsClosedWithoutBackend() {
    const int id = service->addChannel("Offline CH", 1, "0x6064", "0");
    QSignalSpy startedSpy(service, &TraceService::traceStarted);
    QSignalSpy dataSpy(service, &TraceService::traceDataUpdated);

    QVERIFY(!service->isTracing());
    service->startTrace();
    // TraceService starts optimistically even without a live backend
    QVERIFY(service->isTracing());
    QCOMPARE(startedSpy.count(), 1);
    QTest::qWait(20);
    service->stopTrace();
    QVERIFY(!service->isTracing());
  }

  void testExportTraceDataReportsPersistenceOutcome() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("trace.csv");
    QVERIFY(plugin->exportTraceDataToFile(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString csv = QString::fromUtf8(file.readAll());
    QVERIFY(csv.startsWith(QStringLiteral("Channel,Timestamp,Value,Quality\n")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!plugin->exportTraceDataToFile(QString()));
    QVERIFY(!plugin->exportTraceDataToFile(dir.path()));
  }

private:
  MockEcatClient *client = nullptr;
  TraceService *service = nullptr;
  TracePlugin *plugin = nullptr;
};

QTEST_MAIN(TracePluginTest)
#include "trace_plugin_test.moc"
