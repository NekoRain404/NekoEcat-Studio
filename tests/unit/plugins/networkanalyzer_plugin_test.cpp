// NetworkAnalyzerPluginTest — Tests for NetworkAnalyzerPlugin
//
// Test coverage:
//   - Plugin identity, widget creation, and initial state
//   - Capture start/stop and packet management
//   - Packet table, selection, and decode view
//   - Statistics, filters, and export

#include <QFile>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include "plugins/networkanalyzer/NetworkAnalyzerPlugin.h"

class NetworkAnalyzerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, order, visibility
  void testPluginIdentity() {
    NetworkAnalyzerPlugin plugin;
    QCOMPARE(plugin.id(), QString("networkanalyzer"));
    QCOMPARE(plugin.displayName(), QString("Network Analyzer"));
    QCOMPARE(plugin.displayNameZh(), QString("网络分析器"));
    QCOMPARE(plugin.defaultOrder(), 270);
    QCOMPARE(plugin.visible(), true);
  }

  // Widget is created and not null
  // Check widget is created
  void testWidgetCreation() {
    NetworkAnalyzerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Initial state has zero packets and filters
  // Verify all initial counts are zero
  void testInitialState() {
    NetworkAnalyzerPlugin plugin;
    QCOMPARE(plugin.packetCount(), 0);
    QCOMPARE(plugin.filterCount(), 0);
    QCOMPARE(plugin.filteredCount(), 0);
    QCOMPARE(plugin.isCapturing(), false);
  }

  // Start and stop capture toggle state and emit signals
  // Test start and stop capture with signals
  void testStartStopCapture() {
    NetworkAnalyzerPlugin plugin;
    QSignalSpy startSpy(&plugin, &NetworkAnalyzerPlugin::captureStarted);
    QSignalSpy stopSpy(&plugin, &NetworkAnalyzerPlugin::captureStopped);

    plugin.startCapture();
    QCOMPARE(plugin.isCapturing(), true);
    QCOMPARE(startSpy.count(), 1);

    plugin.stopCapture();
    QCOMPARE(plugin.isCapturing(), false);
    QCOMPARE(stopSpy.count(), 1);
  }

  // Add packet increments count and emits signal
  // Test adding a packet entry with signal
  void testAddPacket() {
    NetworkAnalyzerPlugin plugin;
    QSignalSpy addSpy(&plugin, &NetworkAnalyzerPlugin::packetAdded);

    NetworkAnalyzerPlugin::PacketEntry pkt;
    pkt.timestamp = QDateTime::currentDateTime();
    pkt.source = "00:01:02:03:04:05";
    pkt.destination = "06:07:08:09:0a:0b";
    pkt.protocol = "EtherCAT";
    pkt.size = 64;
    pkt.summary = "Test packet";

    plugin.addPacket(pkt);
    QCOMPARE(plugin.packetCount(), 1);
    QCOMPARE(addSpy.count(), 1);
  }

  // Clear packets resets count to zero
  // Test clearing all packets
  void testClearPackets() {
    NetworkAnalyzerPlugin plugin;

    NetworkAnalyzerPlugin::PacketEntry pkt;
    pkt.timestamp = QDateTime::currentDateTime();
    pkt.source = "src";
    pkt.destination = "dst";
    pkt.protocol = "test";
    pkt.size = 32;
    pkt.summary = "test";
    plugin.addPacket(pkt);

    plugin.clearPackets();
    QCOMPARE(plugin.packetCount(), 0);
  }

  // Packet table has correct column count
  // Check packet table has 6 columns
  void testPacketTable() {
    NetworkAnalyzerPlugin plugin;
    QTableWidget *table = plugin.packetTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 6);
  }

  // Select packet emits selection signal
  // Test selecting a packet emits signal
  void testSelectPacket() {
    NetworkAnalyzerPlugin plugin;
    QSignalSpy selectSpy(&plugin, &NetworkAnalyzerPlugin::packetSelected);

    NetworkAnalyzerPlugin::PacketEntry pkt;
    pkt.timestamp = QDateTime::currentDateTime();
    pkt.source = "src";
    pkt.destination = "dst";
    pkt.protocol = "ECAT";
    pkt.size = 100;
    pkt.summary = "desc";
    plugin.addPacket(pkt);

    plugin.selectPacket(0);
    QCOMPARE(selectSpy.count(), 1);
  }

  // Decode view shows packet details after selection
  // Test decode view shows packet details
  void testDecodeView() {
    NetworkAnalyzerPlugin plugin;
    QTextEdit *decode = plugin.decodeView();
    QVERIFY(decode != nullptr);
    QVERIFY(decode->isReadOnly());

    NetworkAnalyzerPlugin::PacketEntry pkt;
    pkt.timestamp = QDateTime::currentDateTime();
    pkt.source = "AA:BB";
    pkt.destination = "CC:DD";
    pkt.protocol = "ECAT";
    pkt.size = 64;
    pkt.summary = "test frame";
    plugin.addPacket(pkt);

    plugin.selectPacket(0);
    QVERIFY(!decode->toPlainText().isEmpty());
  }

  // Statistics table updates after adding packets
  // Check statistics table structure and population
  void testStatisticsTable() {
    NetworkAnalyzerPlugin plugin;
    QTableWidget *table = plugin.statisticsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 3);

    NetworkAnalyzerPlugin::PacketEntry pkt;
    pkt.timestamp = QDateTime::currentDateTime();
    pkt.source = "A";
    pkt.destination = "B";
    pkt.protocol = "EtherCAT";
    pkt.size = 128;
    pkt.summary = "stat test";
    plugin.addPacket(pkt);

    QCOMPARE(table->rowCount(), 1);
  }

  // Add filter increments filter count
  // Test adding a filter
  void testAddFilter() {
    NetworkAnalyzerPlugin plugin;
    plugin.addFilter({"protocol", "==", "EtherCAT"});
    QCOMPARE(plugin.filterCount(), 1);
  }

  // Remove filter decrements filter count
  // Test removing a filter
  void testRemoveFilter() {
    NetworkAnalyzerPlugin plugin;
    plugin.addFilter({"protocol", "==", "EtherCAT"});
    plugin.removeFilter(0);
    QCOMPARE(plugin.filterCount(), 0);
  }

  // Filter table has correct column count
  // Check filter table has 3 columns
  void testFilterTable() {
    NetworkAnalyzerPlugin plugin;
    QTableWidget *table = plugin.filterTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 3);
  }

  // Apply filters reduces visible packet count
  // Test applying filters to packet list
  void testApplyFilters() {
    NetworkAnalyzerPlugin plugin;

    NetworkAnalyzerPlugin::PacketEntry p1;
    p1.timestamp = QDateTime::currentDateTime();
    p1.source = "A";
    p1.destination = "B";
    p1.protocol = "EtherCAT";
    p1.size = 64;
    p1.summary = "ecat";
    plugin.addPacket(p1);

    NetworkAnalyzerPlugin::PacketEntry p2;
    p2.timestamp = QDateTime::currentDateTime();
    p2.source = "C";
    p2.destination = "D";
    p2.protocol = "ARP";
    p2.size = 32;
    p2.summary = "arp";
    plugin.addPacket(p2);

    plugin.addFilter({"protocol", "==", "EtherCAT"});
    plugin.applyFilters();
    QCOMPARE(plugin.filteredCount(), 1);
  }

  // Status label is created and not null
  // Check status label exists
  void testStatusLabel() {
    NetworkAnalyzerPlugin plugin;
    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  // Export capture writes CSV file
  // Test export capture to CSV file
  void testExportCapture() {
    NetworkAnalyzerPlugin plugin;

    NetworkAnalyzerPlugin::PacketEntry pkt;
    pkt.timestamp = QDateTime::currentDateTime();
    pkt.source = "S";
    pkt.destination = "D";
    pkt.protocol = "ECAT";
    pkt.size = 64;
    pkt.summary = "export test";
    plugin.addPacket(pkt);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("capture_export_test.csv");
    QVERIFY(plugin.exportCapture(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString csv = QString::fromUtf8(file.readAll());
    QVERIFY(csv.startsWith(QStringLiteral("Timestamp,Source,Destination,Protocol,Size,Summary\n")));
    QVERIFY(csv.contains(QStringLiteral("S,D,ECAT,64,export test")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!plugin.exportCapture(QString()));
    QVERIFY(!plugin.exportCapture(dir.path()));
  }
};

QTEST_MAIN(NetworkAnalyzerPluginTest)
#include "networkanalyzer_plugin_test.moc"
