/// @brief AlarmPlugin unit tests.
///
/// @details Tests the Alarm workspace plugin's identity, UI construction,
/// alarm management, filtering, and logging integration. Verifies that the
/// plugin correctly implements the WorkspacePlugin interface and provides
/// the expected alarm management functionality.
///
/// @par Test Coverage
///   - Plugin identity (id, displayName, defaultOrder, visible)
///   - Widget construction and non-null checks
///   - Alarm raised: table population with single alarm
///   - Multiple alarms: table handles 3 alarms correctly
///   - Alarm service signals: raised, acknowledged, cleared
///   - Active alarms: filters correctly after acknowledge
///   - Alarm history: returns correct subset
///   - Logging service: log entry creation and retrieval
///   - Log level filtering: respects minimum level
///   - Log export: writes to file correctly
///
/// @par Test Dependencies
///   - Qt6::Test (QTest framework)
///   - Qt6::Widgets (for QTableWidget, QDir)
///   - AlarmPlugin, AlarmService, LoggingService
///
/// @par Test Environment
///   - Requires QT_QPA_PLATFORM=offscreen for widget-based tests
///   - Creates AlarmService and LoggingService per test (no daemon required)

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QDir>
#include "plugins/alarm/AlarmPlugin.h"
#include "services/AlarmService.h"
#include "services/LoggingService.h"

/// @brief Test suite for AlarmPlugin alarm management, filtering, and logging integration.
class AlarmPluginTest : public QObject {
  Q_OBJECT
private slots:
  /// @brief Verifies plugin identity returns correct id, displayName, defaultOrder, and visible.
  void testPluginIdentity() {
    AlarmService alarmSvc;
    LoggingService logSvc;
    AlarmPlugin plugin(&alarmSvc, &logSvc);

    QCOMPARE(plugin.id(), QString("alarm"));
    QCOMPARE(plugin.displayName(), QString("Alarms"));
    QCOMPARE(plugin.defaultOrder(), 110);
    QCOMPARE(plugin.visible(), true);
  }

  // Test that widget() returns a non-null QWidget.
  void testWidgetCreation() {
    AlarmService alarmSvc;
    LoggingService logSvc;
    AlarmPlugin plugin(&alarmSvc, &logSvc);

    QVERIFY(plugin.widget() != nullptr);
  }

  // Test that raising an alarm populates the table with 1 row.
  // Setup: Raise alarm with Error level, Communication category.
  // Assert: Table has 1 row, message column matches.
  void testAlarmRaisedPopulatesTable() {
    AlarmService alarmSvc;
    LoggingService logSvc;
    AlarmPlugin plugin(&alarmSvc, &logSvc);

    alarmSvc.raiseAlarm(AlarmLevel::Error, AlarmCategory::Communication,
                        "Test alarm", "TestSource");

    QTableWidget *table = plugin.widget()->findChild<QTableWidget *>();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->item(0, 6)->text(), QString("Test alarm"));
  }

  // Test that raising multiple alarms populates the table correctly.
  // Setup: Raise 3 alarms with different levels and categories.
  /// @brief Verifies table has 3 rows when multiple alarms are raised.
  /// @details Tests that raising 3 alarms with different levels and categories
  /// correctly populates the table with 3 rows.
  void testMultipleAlarms() {
    AlarmService alarmSvc;
    LoggingService logSvc;
    AlarmPlugin plugin(&alarmSvc, &logSvc);

    alarmSvc.raiseAlarm(AlarmLevel::Info, AlarmCategory::Communication,
                        "Info message");
    alarmSvc.raiseAlarm(AlarmLevel::Warning, AlarmCategory::Device,
                        "Warning message");
    alarmSvc.raiseAlarm(AlarmLevel::Critical, AlarmCategory::Network,
                        "Critical message");

    QTableWidget *table = plugin.widget()->findChild<QTableWidget *>();
    QCOMPARE(table->rowCount(), 3);
  }

  /// @brief Verifies AlarmService emits correct signals for lifecycle events.
  /// @details Tests that raising, acknowledging, and clearing an alarm
  /// emits the corresponding signals exactly once.
  void testAlarmServiceSignals() {
    AlarmService alarmSvc;
    QSignalSpy raisedSpy(&alarmSvc, &AlarmService::alarmRaised);
    QSignalSpy ackedSpy(&alarmSvc, &AlarmService::alarmAcknowledged);
    QSignalSpy clearedSpy(&alarmSvc, &AlarmService::alarmCleared);

    int id = alarmSvc.raiseAlarm(AlarmLevel::Error, AlarmCategory::Device,
                                 "Test");
    QCOMPARE(raisedSpy.count(), 1);

    alarmSvc.acknowledgeAlarm(id);
    QCOMPARE(ackedSpy.count(), 1);

    alarmSvc.clearAlarm(id);
    QCOMPARE(clearedSpy.count(), 1);
  }

  /// @brief Verifies activeAlarms() filters out acknowledged alarms.
  /// @details Tests that after acknowledging one of two alarms, only the
  /// unacknowledged alarm remains in the active alarms list.
  void testActiveAlarms() {
    AlarmService alarmSvc;

    int id1 = alarmSvc.raiseAlarm(AlarmLevel::Info, AlarmCategory::Communication, "A1");
    int id2 = alarmSvc.raiseAlarm(AlarmLevel::Error, AlarmCategory::Device, "A2");
    alarmSvc.acknowledgeAlarm(id1);

    QCOMPARE(alarmSvc.activeAlarms().size(), 1);
    QCOMPARE(alarmSvc.activeAlarms()[0].id, id2);
  }

  /// @brief Verifies alarmHistory() returns the correct number of entries.
  /// @details Tests that requesting history with different limits returns
  /// the expected number of entries (capped at actual count).
  void testAlarmHistory() {
    AlarmService alarmSvc;

    for (int i = 0; i < 5; ++i) {
      alarmSvc.raiseAlarm(AlarmLevel::Info, AlarmCategory::Communication,
                          QString("Msg %1").arg(i));
    }

    QCOMPARE(alarmSvc.alarmHistory(3).size(), 3);
    QCOMPARE(alarmSvc.alarmHistory(10).size(), 5);
  }

  /// @brief Verifies LoggingService creates and retrieves log entries.
  /// @details Tests that logging an entry emits the correct signal and
  /// the entry can be retrieved with the expected message.
  void testLoggingService() {
    LoggingService logSvc;
    QSignalSpy spy(&logSvc, &LoggingService::logEntryAdded);

    logSvc.log(LogLevel::Info, LogCategory::System, "Test log", "TestSource");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(logSvc.getLogs(1).size(), 1);
    QCOMPARE(logSvc.getLogs(1)[0].message, QString("Test log"));
  }

  /// @brief Verifies log level filtering respects the minimum level.
  /// @details Tests that setting the log level to Warning filters out
  /// Debug and Info messages, keeping only Warning and Error.
  void testLogLevelFiltering() {
    LoggingService logSvc;
    logSvc.setLogLevel(LogLevel::Warning);

    logSvc.log(LogLevel::Debug, LogCategory::System, "Debug msg");
    logSvc.log(LogLevel::Info, LogCategory::System, "Info msg");
    logSvc.log(LogLevel::Warning, LogCategory::System, "Warning msg");
    logSvc.log(LogLevel::Error, LogCategory::System, "Error msg");

    QCOMPARE(logSvc.getLogs().size(), 2);
  }

  /// @brief Verifies log export writes to a file correctly.
  /// @details Tests that exporting logs to a file creates the file with
  /// the expected content, then cleans up the temporary file.
  void testLoggingExport() {
    LoggingService logSvc;
    logSvc.log(LogLevel::Info, LogCategory::System, "Export test");

    QString path = QDir::tempPath() + "/test_export.log";
    QVERIFY(logSvc.exportLogs(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QString content = file.readAll();
    QVERIFY(content.contains("Export test"));
    file.close();
    QFile::remove(path);
  }
};

QTEST_MAIN(AlarmPluginTest)
#include "alarm_plugin_test.moc"
