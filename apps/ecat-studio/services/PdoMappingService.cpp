#include "PdoMappingService.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

// PdoMappingService.cpp — Manages PDO mappings per slave with validation and JSON import/export
//
// Implementation notes:
//   - Validates index, bit size (max 1500*8), and total mapped size per slave
//   - Supports configure (upsert by index+subIndex), discover, import, and export
//   - Export/import format: JSON object with position and mappings array

namespace {
bool isValidPosition(int position) {
    return position >= 0;
}

bool isHexAddress(const QString& value) {
    static const QRegularExpression re(QStringLiteral("^0x[0-9A-Fa-f]+$"));
    return re.match(value.trimmed()).hasMatch();
}
} // namespace

PdoMappingService::PdoMappingService(QObject* parent) : QObject(parent) {}

QVector<PdoMapping> PdoMappingService::discoverMappings(int position) {
    return mappings_.value(position);
}

bool PdoMappingService::configureMapping(int position, const PdoMapping& mapping) {
    if (!isValidPosition(position)) {
        emit error(QStringLiteral("Invalid slave position: %1").arg(position));
        return false;
    }
    if (mapping.slavePosition != position) {
        emit error(QStringLiteral("Mapping slave position does not match target position"));
        return false;
    }

    PdoValidationResult vr = validateMapping(mapping);
    if (!vr.valid) {
        emit error(vr.errorMessage);
        return false;
    }

    auto& vec = mappings_[position];
    for (auto& m : vec) {
        if (m.index == mapping.index && m.subIndex == mapping.subIndex) {
            m = mapping;
            emit mappingChanged(position, mapping);
            return true;
        }
    }
    vec.append(mapping);
    emit mappingChanged(position, mapping);
    return true;
}

PdoValidationResult PdoMappingService::validateMapping(const PdoMapping& mapping) const {
    PdoValidationResult r;
    r.maxBitSize = 1500 * 8;

    if (mapping.slavePosition < 0) {
        r.valid = false;
        r.errorMessage = QStringLiteral("Slave position must be non-negative");
        return r;
    }
    if (mapping.index.trimmed().isEmpty()) {
        r.valid = false;
        r.errorMessage = QStringLiteral("Index cannot be empty");
        return r;
    }
    if (!isHexAddress(mapping.index)) {
        r.valid = false;
        r.errorMessage = QStringLiteral("Index must be a hexadecimal address");
        return r;
    }
    if (!mapping.subIndex.trimmed().isEmpty() && !isHexAddress(mapping.subIndex)) {
        r.valid = false;
        r.errorMessage = QStringLiteral("Sub-index must be a hexadecimal address");
        return r;
    }
    if (mapping.bitSize <= 0) {
        r.valid = false;
        r.errorMessage = QStringLiteral("Bit size must be positive");
        return r;
    }
    if (mapping.bitSize > r.maxBitSize) {
        r.valid = false;
        r.errorMessage = QStringLiteral("Bit size exceeds maximum (%1)").arg(r.maxBitSize);
        return r;
    }

    auto it = mappings_.constFind(mapping.slavePosition);
    if (it != mappings_.constEnd()) {
        int total = 0;
        for (const auto& m : *it) {
            if (m.enabled)
                total += m.bitSize;
        }
        r.totalBitSize = total + mapping.bitSize;
    }
    return r;
}

bool PdoMappingService::exportMapping(int position, const QString& filePath) const {
    if (!isValidPosition(position) || filePath.isEmpty())
        return false;

    auto it = mappings_.constFind(position);
    if (it == mappings_.constEnd())
        return false;

    QJsonArray arr;
    for (const auto& m : *it) {
        QJsonObject obj;
        obj[QStringLiteral("index")] = m.index;
        obj[QStringLiteral("subIndex")] = m.subIndex;
        obj[QStringLiteral("name")] = m.name;
        obj[QStringLiteral("dataType")] = m.dataType;
        obj[QStringLiteral("bitSize")] = m.bitSize;
        obj[QStringLiteral("direction")] =
            m.direction == PdoDirection::Output ? QStringLiteral("output") : QStringLiteral("input");
        obj[QStringLiteral("enabled")] = m.enabled;
        arr.append(obj);
    }

    QJsonObject root;
    root[QStringLiteral("position")] = position;
    root[QStringLiteral("mappings")] = arr;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    const QByteArray bytes = QJsonDocument(root).toJson();
    if (file.write(bytes) != bytes.size() || !file.flush())
        return false;
    return true;
}

bool PdoMappingService::importMapping(int position, const QString& filePath) {
    if (!isValidPosition(position) || filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject root = doc.object();
    const QJsonValue mappingsValue = root[QStringLiteral("mappings")];
    if (!mappingsValue.isArray())
        return false;
    QJsonArray arr = mappingsValue.toArray();

    QVector<PdoMapping> imported;
    for (const auto& v : arr) {
        if (!v.isObject())
            return false;
        QJsonObject obj = v.toObject();
        PdoMapping m;
        m.index = obj[QStringLiteral("index")].toString();
        m.subIndex = obj[QStringLiteral("subIndex")].toString();
        m.name = obj[QStringLiteral("name")].toString();
        m.dataType = obj[QStringLiteral("dataType")].toString();
        m.bitSize = obj[QStringLiteral("bitSize")].toInt();
        m.direction = obj[QStringLiteral("direction")].toString() == QStringLiteral("output") ? PdoDirection::Output
                                                                                              : PdoDirection::Input;
        m.slavePosition = position;
        m.enabled = obj[QStringLiteral("enabled")].toBool(true);
        PdoValidationResult vr = validateMapping(m);
        if (!vr.valid) {
            emit error(vr.errorMessage);
            return false;
        }
        imported.append(m);
    }

    mappings_[position] = imported;
    return true;
}

QVector<PdoMapping> PdoMappingService::currentMappings(int position) const {
    return mappings_.value(position);
}

MappingLayout PdoMappingService::getMappingLayout(int position) const {
    MappingLayout layout;
    auto it = mappings_.constFind(position);
    if (it == mappings_.constEnd())
        return layout;

    QHash<int, SyncManagerLayout> smMap;
    for (const auto& m : *it) {
        int smIdx = m.direction == PdoDirection::Output ? 2 : 3;
        if (!smMap.contains(smIdx)) {
            SyncManagerLayout sm;
            sm.smIndex = smIdx;
            sm.direction = m.direction;
            sm.enabled = true;
            smMap[smIdx] = sm;
        }
        PdoEntryLayout entry;
        entry.index = m.index;
        entry.subIndex = m.subIndex;
        entry.name = m.name;
        entry.dataType = m.dataType;
        entry.bitSize = m.bitSize;
        entry.direction = m.direction;
        entry.enabled = m.enabled;
        smMap[smIdx].pdoEntries.append(entry);
        smMap[smIdx].size += m.bitSize;
        layout.totalSize += m.bitSize;
    }

    for (auto it = smMap.begin(); it != smMap.end(); ++it) {
        layout.syncManagers.append(it.value());
    }

    return layout;
}

PdoValidationResult PdoMappingService::validateMappingLayout(const MappingLayout& layout) const {
    PdoValidationResult r;
    r.maxBitSize = 1500 * 8;
    r.totalBitSize = layout.totalSize;

    for (const auto& sm : layout.syncManagers) {
        if (sm.size > r.maxBitSize) {
            r.valid = false;
            r.errorMessage = QString("SM%1 total size %2 bits exceeds maximum %3 bits")
                                 .arg(sm.smIndex)
                                 .arg(sm.size)
                                 .arg(r.maxBitSize);
            return r;
        }
    }
    return r;
}

bool PdoMappingService::applyMappingLayout(int position, const MappingLayout& layout) {
    if (!isValidPosition(position)) {
        emit error(QStringLiteral("Invalid slave position: %1").arg(position));
        return false;
    }

    auto vr = validateMappingLayout(layout);
    if (!vr.valid) {
        emit error(vr.errorMessage);
        return false;
    }

    QVector<PdoMapping> newMappings;
    for (const auto& sm : layout.syncManagers) {
        for (const auto& entry : sm.pdoEntries) {
            PdoMapping m;
            m.index = entry.index;
            m.subIndex = entry.subIndex;
            m.name = entry.name;
            m.dataType = entry.dataType;
            m.bitSize = entry.bitSize;
            m.direction = entry.direction;
            m.slavePosition = position;
            m.enabled = entry.enabled;
            auto mappingValidation = validateMapping(m);
            if (!mappingValidation.valid) {
                emit error(mappingValidation.errorMessage);
                return false;
            }
            newMappings.append(m);
        }
    }

    mappings_[position] = newMappings;
    emit mappingLayoutChanged(position, layout);
    return true;
}
