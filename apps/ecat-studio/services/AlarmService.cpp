#include "AlarmService.h"

// AlarmService.cpp — In-memory alarm lifecycle (raise, acknowledge, clear)
//
// Implementation notes:
//   - Maintains a bounded circular buffer capped at kMaxHistory
//   - Emits Qt signals on every state transition for UI binding
//   - Queries filter by AlarmState for active/history views

AlarmService::AlarmService(QObject* parent) : QObject(parent) {}

// Creates a new active alarm, trims history, emits alarmRaised
int AlarmService::raiseAlarm(AlarmLevel level, AlarmCategory category, const QString& message, const QString& source) {
    Alarm alarm;
    alarm.id = nextId_++;
    alarm.level = level;
    alarm.category = category;
    alarm.state = AlarmState::Active;
    alarm.message = message;
    alarm.source = source;
    alarm.timestamp = QDateTime::currentDateTime();

    alarms_.append(alarm);
    if (alarms_.size() > kMaxHistory) {
        alarms_.removeFirst();
    }

    emit alarmRaised(alarm);
    return alarm.id;
}

bool AlarmService::acknowledgeAlarm(int alarmId) {
    for (auto& a : alarms_) {
        if (a.id == alarmId && a.state == AlarmState::Active) {
            a.state = AlarmState::Acknowledged;
            a.acknowledgedAt = QDateTime::currentDateTime();
            emit alarmAcknowledged(alarmId);
            return true;
        }
    }
    return false;
}

bool AlarmService::clearAlarm(int alarmId) {
    for (auto& a : alarms_) {
        if (a.id == alarmId && a.state != AlarmState::Cleared) {
            a.state = AlarmState::Cleared;
            a.clearedAt = QDateTime::currentDateTime();
            emit alarmCleared(alarmId);
            return true;
        }
    }
    return false;
}

QVector<Alarm> AlarmService::activeAlarms() const {
    QVector<Alarm> result;
    for (const auto& a : alarms_) {
        if (a.state == AlarmState::Active) {
            result.append(a);
        }
    }
    return result;
}

// Returns the most recent `count` alarms from the history buffer
QVector<Alarm> AlarmService::alarmHistory(int count) const {
    if (count >= alarms_.size()) {
        return alarms_;
    }
    return alarms_.mid(alarms_.size() - count);
}

Alarm AlarmService::alarmById(int alarmId) const {
    for (const auto& a : alarms_) {
        if (a.id == alarmId) {
            return a;
        }
    }
    return Alarm{};
}

int AlarmService::activeAlarmCount() const {
    int count = 0;
    for (const auto& a : alarms_) {
        if (a.state == AlarmState::Active) {
            ++count;
        }
    }
    return count;
}
