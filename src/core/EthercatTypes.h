#pragma once

// Shared EtherCAT domain types: SlaveInfo, master state, port info.


#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

struct SlaveInfo {
    int position = -1;
    QString state;
    QString flags;
    QString name;
    QString rawLine;
};

struct MasterInfo {
    QString rawText;
};

QJsonObject toJson(const SlaveInfo &slave);
QJsonArray toJson(const QVector<SlaveInfo> &slaves);
SlaveInfo slaveFromJson(const QJsonObject &object);
QVector<SlaveInfo> slavesFromJson(const QJsonArray &array);

