// ProtocolAnalyzerPluginTest — Tests for ProtocolAnalyzerPlugin and service
//
// Test coverage:
//   - Plugin identity, default order, visibility, widget, service accessor
//   - Capture start/stop state management
//   - Protocol filter configuration
//   - Statistics defaults and JSON export
//   - Capture fails closed without synthetic frames
//   - Frame type and direction enum values
#include <QTest>
#include <QApplication>
#include <QFile>
#include <QSignalSpy>
#include "plugins/protocol/ProtocolAnalyzerPlugin.h"
#include "plugins/protocol/ProtocolAnalyzerService.h"

class ProtocolAnalyzerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, displayName, and displayNameZh
  void testPluginIdentity() {
    ProtocolAnalyzerService svc;
    ProtocolAnalyzerPlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("protocol"));
    QCOMPARE(plugin.displayName(), QString("Protocol Analyzer"));
    QCOMPARE(plugin.displayNameZh(), QStringLiteral("协议分析器"));
  }

  // Verify plugin defaultOrder is 105
  void testPluginDefaultOrder() {
    ProtocolAnalyzerService svc;
    ProtocolAnalyzerPlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 105);
  }

  // Verify plugin is visible
  void testPluginVisible() {
    ProtocolAnalyzerService svc;
    ProtocolAnalyzerPlugin plugin(&svc);
    QVERIFY(plugin.visible());
  }

  // Verify plugin widget is non-null
  void testPluginWidgetNotNull() {
    ProtocolAnalyzerService svc;
    ProtocolAnalyzerPlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify plugin service() returns the injected service pointer
  void testPluginServiceAccessor() {
    ProtocolAnalyzerService svc;
    ProtocolAnalyzerPlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  // Test capture start/stop state toggling
  void testServiceCapture() {
    ProtocolAnalyzerService svc;
    QVERIFY(!svc.isCapturing());
    svc.startCapture();
    QVERIFY(svc.isCapturing());
    svc.stopCapture();
    QVERIFY(!svc.isCapturing());
  }

  // Test protocol filter set/get round-trip
  void testServiceFilter() {
    ProtocolAnalyzerService svc;
    ProtocolFilter f;
    f.enabled = true;
    f.frameType = ProtocolFrameType::CoE;
    f.direction = ProtocolDirection::TX;
    svc.setFilter(f);
    auto stored = svc.filter();
    QVERIFY(stored.enabled);
    QCOMPARE(stored.frameType, ProtocolFrameType::CoE);
    QCOMPARE(stored.direction, ProtocolDirection::TX);
  }

  // Verify default statistics are all zeros
  void testServiceStatisticsDefault() {
    ProtocolAnalyzerService svc;
    auto stats = svc.statistics();
    QCOMPARE(stats.totalFrames, 0);
    QCOMPARE(stats.errorFrames, 0);
    QCOMPARE(stats.txFrames, 0);
    QCOMPARE(stats.rxFrames, 0);
  }

  // Verify statistics JSON contains all expected fields
  void testServiceStatisticsJson() {
    ProtocolAnalyzerService svc;
    auto json = svc.statisticsJson();
    QVERIFY(json.contains("totalFrames"));
    QVERIFY(json.contains("errorFrames"));
    QVERIFY(json.contains("txFrames"));
    QVERIFY(json.contains("rxFrames"));
    QVERIFY(json.contains("bandwidthBps"));
  }

  // Test capture does not synthesize frames without a real backend
  void testServiceCaptureDoesNotSynthesizeFrames() {
    ProtocolAnalyzerService svc;
    QSignalSpy frameSpy(&svc, &ProtocolAnalyzerService::frameCaptured);
    QSignalSpy statsSpy(&svc, &ProtocolAnalyzerService::statisticsUpdated);
    QVERIFY(frameSpy.isValid());
    QVERIFY(statsSpy.isValid());

    svc.startCapture();
    QTest::qWait(100);
    svc.stopCapture();

    QCOMPARE(svc.frameCount(), 0);
    QCOMPARE(svc.getFrames(5).size(), 0);
    QCOMPARE(frameSpy.count(), 0);
    QCOMPARE(statsSpy.count(), 0);
    QCOMPARE(svc.statistics().totalFrames, 0);
    QCOMPARE(svc.statistics().bandwidthBps, 0.0);
  }

  // Test clear resets empty capture state and emits a zero-stat update
  void testServiceClearFrames() {
    ProtocolAnalyzerService svc;
    QSignalSpy statsSpy(&svc, &ProtocolAnalyzerService::statisticsUpdated);
    QVERIFY(statsSpy.isValid());

    svc.clearFrames();

    QCOMPARE(svc.frameCount(), 0);
    QCOMPARE(statsSpy.count(), 1);
  }

  // Test getFrames remains empty without backend-captured frames
  void testServiceGetFramesRequiresBackendFrames() {
    ProtocolAnalyzerService svc;
    svc.startCapture();
    QTest::qWait(100);
    svc.stopCapture();

    auto frames = svc.getFrames(5);
    QVERIFY(frames.isEmpty());
  }

  void testSourceDoesNotContainSyntheticCaptureGenerator() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp"));
    QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("generateFrame")),
             "Protocol analyzer must not keep a synthetic frame generator");
    QVERIFY2(!text.contains(QStringLiteral("QByteArray(64")),
             "Protocol analyzer must not synthesize frame payloads");
    QVERIFY2(!text.contains(QStringLiteral("tickCount_ % 5")),
             "Protocol analyzer must not cycle fake protocol frame types");
  }

  // Verify ProtocolFrameType enum values
  void testFrameTypes() {
    ProtocolFrame frame;
    frame.frameType = ProtocolFrameType::EtherCAT;
    QCOMPARE(frame.frameType, ProtocolFrameType::EtherCAT);
    frame.frameType = ProtocolFrameType::CoE;
    QCOMPARE(frame.frameType, ProtocolFrameType::CoE);
  }

  // Verify ProtocolDirection enum values
  void testDirectionEnum() {
    ProtocolFrame frame;
    frame.direction = ProtocolDirection::TX;
    QCOMPARE(frame.direction, ProtocolDirection::TX);
    frame.direction = ProtocolDirection::RX;
    QCOMPARE(frame.direction, ProtocolDirection::RX);
  }
};

QTEST_MAIN(ProtocolAnalyzerPluginTest)
#include "protocol_analyzer_plugin_test.moc"
