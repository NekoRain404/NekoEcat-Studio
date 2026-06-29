#pragma once

// AlarmService — manages system alarms with severity levels, categories,
// and lifecycle states (Active → Acknowledged → Cleared). Provides signals
// for real-time alarm notifications and history queries.
//
// This service provides comprehensive alarm management for the EtherCAT
// system. It handles:
//   - Alarm creation with severity levels and categories
//   - Alarm lifecycle management (Active → Acknowledged → Cleared)
//   - Real-time alarm notifications via signals
//   - Alarm history queries and filtering
//   - Active alarm count tracking
//
// Usage:
//   ServiceContainer *container = ...;
//   AlarmService *alarm = container->alarm();
//   int alarmId = alarm->raiseAlarm(AlarmLevel::Error, AlarmCategory::Communication,
//                                   "Connection lost", "EcatClient");
//   alarm->acknowledgeAlarm(alarmId);
//   alarm->clearAlarm(alarmId);
//   QVector<Alarm> active = alarm->activeAlarms();
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   is thread-safe for concurrent access from the main thread.
//
// Performance:
//   - Alarm creation is O(1)
//   - Alarm lookup is O(n) where n is total alarms
//   - History queries are O(n) with optional count limit
//   - Memory usage is bounded by kMaxHistory (1000 alarms)

#include <QObject>
#include <QVector>
#include <QDateTime>

/// @brief Alarm severity levels.
enum class AlarmLevel {
  Info,      ///< Informational message.
  Warning,   ///< Warning condition.
  Error,     ///< Error condition.
  Critical   ///< Critical failure.
};

/// @brief Alarm categories for classification.
enum class AlarmCategory {
  Communication,  ///< Network/communication issues.
  Device,         ///< Device-related issues.
  Network,        ///< Network infrastructure issues.
  Configuration   ///< Configuration issues.
};

/// @brief Alarm lifecycle states.
enum class AlarmState {
  Active,        ///< Alarm is active and unacknowledged.
  Acknowledged,  ///< Alarm has been acknowledged.
  Cleared        ///< Alarm has been cleared.
};

/// @brief Represents a single alarm event with full lifecycle tracking.
struct Alarm {
  int id = 0;                           ///< Unique alarm ID.
  AlarmLevel level = AlarmLevel::Info;  ///< Severity level.
  AlarmCategory category = AlarmCategory::Communication; ///< Alarm category.
  AlarmState state = AlarmState::Active;  ///< Lifecycle state.
  QString message;                      ///< Human-readable alarm message.
  QString source;                       ///< Source component that raised the alarm.
  QDateTime timestamp;                  ///< When the alarm was raised.
  QDateTime acknowledgedAt;             ///< When the alarm was acknowledged.
  QDateTime clearedAt;                  ///< When the alarm was cleared.
};

/// @brief Manages system alarms with severity levels, categories, and lifecycle states.
///
/// Provides alarm creation, acknowledgement, clearing, and history queries.
/// Alarms follow a lifecycle: Active -> Acknowledged -> Cleared. Signals
/// provide real-time notifications for alarm state changes.
class AlarmService : public QObject {
  Q_OBJECT
public:
  /// @brief Construct the alarm service.
  /// @param parent  Parent QObject.
  explicit AlarmService(QObject *parent = nullptr);

  /// @brief Raise a new alarm.
  /// @param level     Severity level (Info, Warning, Error, Critical).
  /// @param category  Alarm category (Communication, Device, Network, Configuration).
  /// @param message   Human-readable alarm message.
  /// @param source    Source component that raised the alarm (optional).
  /// @return Unique alarm ID for future reference.
  int raiseAlarm(AlarmLevel level, AlarmCategory category,
                 const QString &message, const QString &source = QString());

  /// @brief Acknowledge an active alarm.
  /// @param alarmId  Alarm ID to acknowledge.
  /// @return true if alarm was found and acknowledged.
  bool acknowledgeAlarm(int alarmId);

  /// @brief Clear an acknowledged alarm.
  /// @param alarmId  Alarm ID to clear.
  /// @return true if alarm was found and cleared.
  bool clearAlarm(int alarmId);

  /// @brief Get all active (unacknowledged) alarms.
  /// @return Vector of active Alarm structures.
  QVector<Alarm> activeAlarms() const;

  /// @brief Get alarm history with optional count limit.
  /// @param count  Maximum number of alarms to return (default: 100).
  /// @return Vector of Alarm structures, most recent first.
  QVector<Alarm> alarmHistory(int count = 100) const;

  /// @brief Get a specific alarm by ID.
  /// @param alarmId  Alarm ID to look up.
  /// @return Alarm structure (empty if not found).
  Alarm alarmById(int alarmId) const;

  /// @brief Get the count of active (unacknowledged) alarms.
  /// @return Number of active alarms.
  int activeAlarmCount() const;

signals:
  /// @brief Emitted when a new alarm is raised.
  /// @param alarm  The new Alarm structure.
  void alarmRaised(const Alarm &alarm);

  /// @brief Emitted when an alarm is acknowledged.
  /// @param alarmId  Alarm ID that was acknowledged.
  void alarmAcknowledged(int alarmId);

  /// @brief Emitted when an alarm is cleared.
  /// @param alarmId  Alarm ID that was cleared.
  void alarmCleared(int alarmId);

private:
  QVector<Alarm> alarms_;  ///< All alarms (active, acknowledged, cleared).
  int nextId_ = 1;         ///< Next alarm ID to assign.
  /// @brief Maximum number of alarms retained in history.
  static constexpr int kMaxHistory = 1000;
};
