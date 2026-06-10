#include "EthercatTypes.h"

QJsonObject toJson(const SlaveInfo &slave)
{
    return {
        {"position", slave.position},
        {"state", slave.state},
        {"flags", slave.flags},
        {"name", slave.name},
        {"rawLine", slave.rawLine},
    };
}

QJsonArray toJson(const QVector<SlaveInfo> &slaves)
{
    QJsonArray array;
    for (const auto &slave : slaves) {
        array.append(toJson(slave));
    }
    return array;
}

SlaveInfo slaveFromJson(const QJsonObject &object)
{
    SlaveInfo slave;
    slave.position = object.value("position").toInt(-1);
    slave.state = object.value("state").toString();
    slave.flags = object.value("flags").toString();
    slave.name = object.value("name").toString();
    slave.rawLine = object.value("rawLine").toString();
    return slave;
}

QVector<SlaveInfo> slavesFromJson(const QJsonArray &array)
{
    QVector<SlaveInfo> slaves;
    slaves.reserve(array.size());
    for (const auto &value : array) {
        if (value.isObject()) {
            slaves.append(slaveFromJson(value.toObject()));
        }
    }
    return slaves;
}

