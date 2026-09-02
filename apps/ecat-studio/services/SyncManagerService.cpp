#include "SyncManagerService.h"

// SyncManagerService.cpp — Sync Manager configuration facade (SM0-SM3)
//
// Implementation notes:
//   - Fails closed until backed by a live EtherCAT master path
//   - Keeps read APIs available for default/empty state inspection
//   - Validates SM index range and emits errors for out-of-bounds access

SyncManagerService::SyncManagerService(QObject* parent) : QObject(parent) {}

namespace {
bool validTarget(int position, int smIndex) {
    return position >= 0 && smIndex >= 0 && smIndex <= 3;
}

bool validPdoIndex(int pdoIndex) {
    return pdoIndex > 0 && pdoIndex <= 0xFFFF;
}
} // namespace

bool SyncManagerService::configureSyncManager(int position, int smIndex, const SyncManagerConfig& config) {
    if (!validTarget(position, smIndex)) {
        emit error(QStringLiteral("Invalid position or SM index"));
        return false;
    }
    if (config.smIndex != smIndex) {
        emit error(QStringLiteral("SM index mismatch"));
        return false;
    }
    Q_UNUSED(config);
    emit error(QStringLiteral("Sync Manager configuration requires a connected EtherCAT backend"));
    return false;
}

bool SyncManagerService::assignPdo(int position, int smIndex, int pdoIndex) {
    if (!validTarget(position, smIndex)) {
        emit error(QStringLiteral("Invalid position or SM index"));
        return false;
    }
    if (!validPdoIndex(pdoIndex)) {
        emit error(QStringLiteral("Invalid PDO index"));
        return false;
    }

    auto posIt = configs_.find(position);
    if (posIt == configs_.end()) {
        emit error(QStringLiteral("No configuration for position %1").arg(position));
        return false;
    }
    auto smIt = posIt->find(smIndex);
    if (smIt == posIt->end()) {
        emit error(QStringLiteral("SM %1 not configured").arg(smIndex));
        return false;
    }
    Q_UNUSED(pdoIndex);
    emit error(QStringLiteral("PDO assignment requires a connected EtherCAT backend"));
    return false;
}

bool SyncManagerService::setDirection(int position, int smIndex, SmDirection direction) {
    if (!validTarget(position, smIndex)) {
        emit error(QStringLiteral("Invalid position or SM index"));
        return false;
    }

    auto posIt = configs_.find(position);
    if (posIt == configs_.end()) {
        emit error(QStringLiteral("No configuration for position %1").arg(position));
        return false;
    }
    auto smIt = posIt->find(smIndex);
    if (smIt == posIt->end()) {
        emit error(QStringLiteral("SM %1 not configured").arg(smIndex));
        return false;
    }
    Q_UNUSED(direction);
    emit error(QStringLiteral("Sync Manager direction update requires a connected EtherCAT backend"));
    return false;
}

bool SyncManagerService::setWatchdog(int position, int smIndex, int timeout) {
    if (!validTarget(position, smIndex)) {
        emit error(QStringLiteral("Invalid position or SM index"));
        return false;
    }
    if (timeout < 0) {
        emit error(QStringLiteral("Invalid watchdog timeout"));
        return false;
    }

    auto posIt = configs_.find(position);
    if (posIt == configs_.end()) {
        emit error(QStringLiteral("No configuration for position %1").arg(position));
        return false;
    }
    auto smIt = posIt->find(smIndex);
    if (smIt == posIt->end()) {
        emit error(QStringLiteral("SM %1 not configured").arg(smIndex));
        return false;
    }
    Q_UNUSED(timeout);
    emit error(QStringLiteral("Sync Manager watchdog update requires a connected EtherCAT backend"));
    return false;
}

SyncManagerConfig SyncManagerService::syncManagerConfig(int position, int smIndex) const {
    return configs_.value(position).value(smIndex);
}

QVector<int> SyncManagerService::syncManagers(int position) const {
    return configs_.value(position).keys().toVector();
}
