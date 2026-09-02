// DataLoggerPluginTest — Tests for DataLoggerPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Log filter add/remove
//   - Log entry add/clear and signal emission
//   - Log file management
//   - Max file size/count configuration
//   - Tab and table widget existence
//   - Export and source statistics

#include "plugins/datalogger/DataLoggerPlugin.h"
#include <QLabel>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTabWidget>
#include <QTest>
#include <QTextEdit>

class DataLoggerPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin id, display names, order, and visibility
    void testPluginIdentity() {
        DataLoggerPlugin plugin;

        QCOMPARE(plugin.id(), QString("datalogger"));
        QCOMPARE(plugin.displayName(), QString("Data Logger"));
        QCOMPARE(plugin.displayNameZh(), QString("数据记录器"));
        QCOMPARE(plugin.defaultOrder(), 245);
        QCOMPARE(plugin.visible(), false);
    }

    // Verify main widget is created
    void testWidgetCreation() {
        DataLoggerPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify initial counts are zero and defaults are set
    void testInitialState() {
        DataLoggerPlugin plugin;

        QCOMPARE(plugin.filterCount(), 0);
        QCOMPARE(plugin.logEntryCount(), 0);
        QCOMPARE(plugin.logFileCount(), 0);
        QCOMPARE(plugin.maxFileSize(), 10485760);
        QCOMPARE(plugin.maxFileCount(), 10);
    }

    // Verify adding a log filter increments count
    void testAddFilter() {
        DataLoggerPlugin plugin;

        DataLoggerPlugin::LogFilter filter;
        filter.name = "Error Filter";
        filter.source = "ecat0";
        filter.level = "error";
        filter.enabled = true;

        plugin.addFilter(filter);
        QCOMPARE(plugin.filterCount(), 1);
    }

    // Verify removing a filter decrements count
    void testRemoveFilter() {
        DataLoggerPlugin plugin;

        DataLoggerPlugin::LogFilter filter;
        filter.name = "Error Filter";
        filter.source = "ecat0";
        filter.level = "error";
        filter.enabled = true;

        plugin.addFilter(filter);
        QCOMPARE(plugin.filterCount(), 1);

        plugin.removeFilter(0);
        QCOMPARE(plugin.filterCount(), 0);
    }

    // Verify adding a log entry increments count and emits signal
    void testAddLogEntry() {
        DataLoggerPlugin plugin;
        QSignalSpy spy(&plugin, &DataLoggerPlugin::logEntryAdded);

        DataLoggerPlugin::LogEntry entry;
        entry.timestamp = QDateTime::currentDateTime();
        entry.source = "ecat0";
        entry.level = "info";
        entry.message = "Test log message";

        plugin.addLogEntry(entry);
        QCOMPARE(plugin.logEntryCount(), 1);
        QCOMPARE(spy.count(), 1);
    }

    // Verify clearing log entries resets count to zero
    void testClearLogEntries() {
        DataLoggerPlugin plugin;

        DataLoggerPlugin::LogEntry entry;
        entry.timestamp = QDateTime::currentDateTime();
        entry.source = "ecat0";
        entry.level = "info";
        entry.message = "Test log message";

        plugin.addLogEntry(entry);
        plugin.addLogEntry(entry);
        QCOMPARE(plugin.logEntryCount(), 2);

        plugin.clearLogEntries();
        QCOMPARE(plugin.logEntryCount(), 0);
    }

    // Verify adding a log file increments count
    void testAddLogFile() {
        DataLoggerPlugin plugin;

        DataLoggerPlugin::LogFile file;
        file.path = "/var/log/ecat.log";
        file.sizeBytes = 1024;
        file.createdAt = QDateTime::currentDateTime();
        file.entryCount = 100;

        plugin.addLogFile(file);
        QCOMPARE(plugin.logFileCount(), 1);
    }

    // Verify removing a log file decrements count
    void testRemoveLogFile() {
        DataLoggerPlugin plugin;

        DataLoggerPlugin::LogFile file;
        file.path = "/var/log/ecat.log";
        file.sizeBytes = 1024;
        file.createdAt = QDateTime::currentDateTime();
        file.entryCount = 100;

        plugin.addLogFile(file);
        QCOMPARE(plugin.logFileCount(), 1);

        plugin.removeLogFile(0);
        QCOMPARE(plugin.logFileCount(), 0);
    }

    // Verify max file size can be updated
    void testMaxFileSize() {
        DataLoggerPlugin plugin;

        plugin.setMaxFileSize(20971520);
        QCOMPARE(plugin.maxFileSize(), 20971520);
    }

    // Verify max file count can be updated
    void testMaxFileCount() {
        DataLoggerPlugin plugin;

        plugin.setMaxFileCount(20);
        QCOMPARE(plugin.maxFileCount(), 20);
    }

    // Verify tabs widget exists
    void testTabs() {
        DataLoggerPlugin plugin;
        QVERIFY(plugin.tabs() != nullptr);
    }

    // Verify filters table widget exists
    void testFiltersTable() {
        DataLoggerPlugin plugin;
        QVERIFY(plugin.filtersTable() != nullptr);
    }

    // Verify log files table widget exists
    void testLogFilesTable() {
        DataLoggerPlugin plugin;
        QVERIFY(plugin.logFilesTable() != nullptr);
    }

    // Verify log viewer widget exists
    void testLogViewer() {
        DataLoggerPlugin plugin;
        QVERIFY(plugin.logViewer() != nullptr);
    }

    // Verify statistics table widget exists
    void testStatisticsTable() {
        DataLoggerPlugin plugin;
        QVERIFY(plugin.statisticsTable() != nullptr);
    }

    // Verify export produces non-empty JSON
    void testExportLogData() {
        DataLoggerPlugin plugin;

        DataLoggerPlugin::LogEntry entry;
        entry.timestamp = QDateTime::currentDateTime();
        entry.source = "ecat0";
        entry.level = "info";
        entry.message = "Test log message";

        plugin.addLogEntry(entry);
        QString json = plugin.exportLogData();
        QVERIFY(!json.isEmpty());
    }

    // Verify source statistics counts per source
    void testSourceStatistics() {
        DataLoggerPlugin plugin;

        DataLoggerPlugin::LogEntry entry1;
        entry1.timestamp = QDateTime::currentDateTime();
        entry1.source = "ecat0";
        entry1.level = "info";
        entry1.message = "Message from ecat0";

        DataLoggerPlugin::LogEntry entry2;
        entry2.timestamp = QDateTime::currentDateTime();
        entry2.source = "ecat1";
        entry2.level = "error";
        entry2.message = "Message from ecat1";

        plugin.addLogEntry(entry1);
        plugin.addLogEntry(entry2);

        QMap<QString, int> stats = plugin.getSourceStatistics();
        QCOMPARE(stats.value("ecat0"), 1);
        QCOMPARE(stats.value("ecat1"), 1);
    }

    // Verify status label widget exists
    void testStatusLabel() {
        DataLoggerPlugin plugin;
        QVERIFY(plugin.statusLabel() != nullptr);
    }
};

QTEST_MAIN(DataLoggerPluginTest)
#include "datalogger_plugin_test.moc"
