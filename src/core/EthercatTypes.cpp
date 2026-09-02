// Shared EtherCAT domain types: SlaveInfo, master state, port info.
#include "EthercatTypes.h"

QJsonObject toJson(const SlaveInfo& slave) {
    // Serialize for wire transfer between daemon and GUI client.
    return {
        {"position", slave.position}, {"state", slave.state},     {"flags", slave.flags},
        {"name", slave.name},         {"rawLine", slave.rawLine},
    };
}

QJsonArray toJson(const QVector<SlaveInfo>& slaves) {
    // Batch-serialize the full slave list for the "scan" response.
    QJsonArray array;
    for (const auto& slave : slaves) {
        array.append(toJson(slave));
    }
    return array;
}

SlaveInfo slaveFromJson(const QJsonObject& object) {
    // Deserialize; position defaults to -1 to signal "not found" on parse failure.
    SlaveInfo slave;
    slave.position = object.value("position").toInt(-1);
    slave.state = object.value("state").toString();
    slave.flags = object.value("flags").toString();
    slave.name = object.value("name").toString();
    slave.rawLine = object.value("rawLine").toString();
    return slave;
}

QVector<SlaveInfo> slavesFromJson(const QJsonArray& array) {
    // Batch-deserialize, silently skipping malformed entries.
    QVector<SlaveInfo> slaves;
    slaves.reserve(array.size());
    for (const auto& value : array) {
        if (value.isObject()) {
            slaves.append(slaveFromJson(value.toObject()));
        }
    }
    return slaves;
}
