#include "PdoConfigurationService.h"

#include <QRegularExpression>
#include <QSet>

namespace {
bool isValidPosition(int position) {
    return position >= 0;
}

bool isHexAddress(const QString& value) {
    static const QRegularExpression re(QStringLiteral("^0x[0-9A-Fa-f]+$"));
    return re.match(value.trimmed()).hasMatch();
}
} // namespace

PdoConfigurationService::PdoConfigurationService(QObject* parent) : QObject(parent) {}

bool PdoConfigurationService::configurePdoMapping(int position, const PdoMappingConfig& config) {
    if (!isValidPosition(position)) {
        configurationError(position, QStringLiteral("Invalid slave position: %1").arg(position));
        return false;
    }

    QString error;
    if (!validateMapping(config, error)) {
        configurationError(position, error);
        return false;
    }

    auto& slave = ensureSlave(position);
    auto& mappings = slave.mappings;
    for (auto& m : mappings) {
        if (m.index == config.index && m.subIndex == config.subIndex) {
            m = config;
            slave.status.mappingConfigured = true;
            return true;
        }
    }
    mappings.append(config);
    slave.status.mappingConfigured = true;
    return true;
}

bool PdoConfigurationService::configurePdoAssignment(int position, const PdoAssignmentConfig& config) {
    if (!isValidPosition(position)) {
        configurationError(position, QStringLiteral("Invalid slave position: %1").arg(position));
        return false;
    }

    QString error;
    if (!validateAssignment(config, error)) {
        configurationError(position, error);
        return false;
    }

    auto& slave = ensureSlave(position);
    auto& assignments = slave.assignments;
    for (auto& a : assignments) {
        if (a.smIndex == config.smIndex) {
            a = config;
            slave.status.assignmentConfigured = true;
            return true;
        }
    }
    assignments.append(config);
    slave.status.assignmentConfigured = true;
    return true;
}

bool PdoConfigurationService::configureSyncManager(int position, const PdoSyncManagerConfig& config) {
    if (!isValidPosition(position)) {
        configurationError(position, QStringLiteral("Invalid slave position: %1").arg(position));
        return false;
    }
    if (config.smIndex < 0 || config.smIndex > 7) {
        configurationError(position, QStringLiteral("Invalid sync manager index: %1").arg(config.smIndex));
        return false;
    }
    if (config.startAddress < 0) {
        configurationError(position, QStringLiteral("Sync manager start address cannot be negative"));
        return false;
    }
    if (config.enable && config.length <= 0) {
        configurationError(position, QStringLiteral("Enabled sync manager length must be positive"));
        return false;
    }

    auto& slave = ensureSlave(position);
    auto& sms = slave.smConfigs;
    for (auto& sm : sms) {
        if (sm.smIndex == config.smIndex) {
            sm = config;
            slave.status.syncManagerConfigured = true;
            return true;
        }
    }
    sms.append(config);
    slave.status.syncManagerConfigured = true;
    return true;
}

bool PdoConfigurationService::configureDcSync(int position, const DcSyncConfig& config) {
    if (!isValidPosition(position)) {
        configurationError(position, QStringLiteral("Invalid slave position: %1").arg(position));
        return false;
    }
    if (config.sync0CycleTime < 0 || config.sync1CycleTime < 0 || config.sync0ShiftTime < 0 ||
        config.sync1ShiftTime < 0) {
        configurationError(position, QStringLiteral("DC sync timing values cannot be negative"));
        return false;
    }

    auto& slave = ensureSlave(position);
    slave.dcSync = config;
    slave.status.dcSyncConfigured = true;
    return true;
}

bool PdoConfigurationService::applyConfiguration(int position) {
    if (!isValidPosition(position)) {
        configurationError(position, QStringLiteral("Invalid slave position: %1").arg(position));
        return false;
    }

    auto it = configs_.find(position);
    if (it == configs_.end()) {
        configurationError(position, QStringLiteral("No configuration found for position %1").arg(position));
        return false;
    }

    auto& slave = *it;
    if (!slave.status.mappingConfigured) {
        configurationError(position, QStringLiteral("PDO mappings not configured for position %1").arg(position));
        return false;
    }

    slave.status.position = position;
    slave.status.lastError = QStringLiteral("PDO configuration apply requires a connected EtherCAT backend");
    configurationError(position, slave.status.lastError);
    return false;
}

QVector<PdoMappingConfig> PdoConfigurationService::pdoMappings(int position) const {
    auto it = configs_.constFind(position);
    if (it == configs_.constEnd())
        return {};
    return it->mappings;
}

QVector<PdoAssignmentConfig> PdoConfigurationService::pdoAssignments(int position) const {
    auto it = configs_.constFind(position);
    if (it == configs_.constEnd())
        return {};
    return it->assignments;
}

QVector<PdoSyncManagerConfig> PdoConfigurationService::syncManagers(int position) const {
    auto it = configs_.constFind(position);
    if (it == configs_.constEnd())
        return {};
    return it->smConfigs;
}

DcSyncConfig PdoConfigurationService::dcSyncConfig(int position) const {
    auto it = configs_.constFind(position);
    if (it == configs_.constEnd())
        return {};
    return it->dcSync;
}

PdoConfigurationStatus PdoConfigurationService::configurationStatus(int position) const {
    auto it = configs_.constFind(position);
    if (it == configs_.constEnd()) {
        PdoConfigurationStatus s;
        s.position = position;
        return s;
    }
    return it->status;
}

PdoConfigurationService::SlaveConfig& PdoConfigurationService::ensureSlave(int position) {
    if (!configs_.contains(position)) {
        SlaveConfig sc;
        sc.status.position = position;
        configs_[position] = sc;
    }
    return configs_[position];
}

bool PdoConfigurationService::validateMapping(const PdoMappingConfig& config, QString& error) const {
    if (config.index.trimmed().isEmpty()) {
        error = QStringLiteral("PDO index cannot be empty");
        return false;
    }
    if (!isHexAddress(config.index)) {
        error = QStringLiteral("PDO index must be a hexadecimal address");
        return false;
    }
    if (!config.subIndex.trimmed().isEmpty() && !isHexAddress(config.subIndex)) {
        error = QStringLiteral("PDO sub-index must be a hexadecimal address");
        return false;
    }
    if (config.bitSize <= 0) {
        error = QStringLiteral("Bit size must be positive");
        return false;
    }
    if (config.bitSize > 1500 * 8) {
        error = QStringLiteral("Bit size exceeds maximum (%1)").arg(1500 * 8);
        return false;
    }
    return true;
}

bool PdoConfigurationService::validateAssignment(const PdoAssignmentConfig& config, QString& error) const {
    if (config.smIndex < 0 || config.smIndex > 7) {
        error = QStringLiteral("Invalid sync manager index: %1").arg(config.smIndex);
        return false;
    }
    if (config.pdoIndices.isEmpty()) {
        error = QStringLiteral("PDO assignment must contain at least one PDO index");
        return false;
    }
    QSet<QString> seen;
    for (const auto& pdoIndex : config.pdoIndices) {
        const QString normalized = pdoIndex.trimmed();
        if (normalized.isEmpty()) {
            error = QStringLiteral("PDO assignment index cannot be empty");
            return false;
        }
        if (!isHexAddress(normalized)) {
            error = QStringLiteral("PDO assignment index must be a hexadecimal address");
            return false;
        }
        if (seen.contains(normalized)) {
            error = QStringLiteral("PDO assignment contains duplicate index: %1").arg(normalized);
            return false;
        }
        seen.insert(normalized);
    }
    return true;
}
