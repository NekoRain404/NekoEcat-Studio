// ProtocolAnalyzerPluginTest — Tests for ProtocolAnalyzerPlugin and service
//
// Test coverage:
//   - Plugin identity, default order, visibility, widget, service accessor
//   - Capture start/stop state management
//   - Protocol filter configuration
//   - Statistics defaults and JSON export
//   - Frame capture, clear, and retrieval
//   - Frame type and direction enum values
#include <QTest>
#include <QApplication>
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

  // Test frame capture and clear resets frameCount
  void testServiceClearFrames() {
    ProtocolAnalyzerService svc;
    svc.startCapture();
    QTest::qWait(100);
    svc.stopCapture();
    QVERIFY(svc.frameCount() > 0);
    svc.clearFrames();
    QCOMPARE(svc.frameCount(), 0);
  }

  // Test getFrames returns bounded results with valid timestamps
  void testServiceGetFrames() {
    ProtocolAnalyzerService svc;
    svc.startCapture();
    QTest::qWait(100);
    svc.stopCapture();
    auto frames = svc.getFrames(5);
    QVERIFY(frames.size() <= 5);
    QVERIFY(frames.size() > 0);
    QVERIFY(frames.first().timestamp > 0);
    QVERIFY(!frames.first().decodedSummary.isEmpty());
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
