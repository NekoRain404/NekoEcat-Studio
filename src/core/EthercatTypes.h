#pragma once

// Shared EtherCAT domain types: SlaveInfo, master state, port info.


#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

struct SlaveInfo {
    // Represents a single EtherCAT slave discovered on the bus.
    // position comes from `ethercat slaves` output; rawLine preserves the CLI text for debugging.
    int position = -1;
    QString state;
    QString flags;
    QString name;
    QString rawLine;

    bool operator==(const SlaveInfo &other) const = default;
};

struct MasterInfo {
    // Raw `ethercat master` output (timing, DC info, topology) for text-mode display.
    QString rawText;
};

QJsonObject toJson(const SlaveInfo &slave);
QJsonArray toJson(const QVector<SlaveInfo> &slaves);
SlaveInfo slaveFromJson(const QJsonObject &object);
QVector<SlaveInfo> slavesFromJson(const QJsonArray &array);
