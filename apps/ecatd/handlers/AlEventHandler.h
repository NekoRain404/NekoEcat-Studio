#pragma once
// AlEventHandler — tracks EtherCAT Application Layer events and errors.
// Polls slave AL status via IgH CLI and maintains an event history.

#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QString>
#include <QDateTime>

struct AlEventEntry {
    qint64 timestampMs = 0;
    int slave = -1;
    QString slaveName;
    QString alStatusCode;  // hex code or state name
    QString description;
    QString severity;  // "Error", "Warning", "Info"
};

class AlEventHandler {
public:
    // Poll all slaves for AL status, append new events to history.
    void poll();

    // Return recent events as JSON.
    QJsonObject handle(const QString &id, const QJsonObject &params);

    // Clear event history.
    void clear();

    // Directly append an event (exposed for unit testing).
    void addEvent(const AlEventEntry &entry);

private:
    QVector<AlEventEntry> events_;
    static constexpr int kMaxEvents = 1000;

    // Previous AL status per slave (to detect changes).
    QVector<QString> previousAlStatus_;

    QString runCommand(const QString &cmd) const;
    QVector<AlEventEntry> pollSlaveAlStatus() const;
};
