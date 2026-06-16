#pragma once

// Abstract interface for EtherCAT backend operations.
// Decouples the daemon dispatch layer from the CLI implementation.
// Allows alternative backends (ecrt, mock, etc.) without changing the daemon.

#include "EthercatTypes.h"

#include <QJsonArray>
#include <QString>

class EcatService {
public:
    virtual ~EcatService() = default;

    virtual QString masterText(const QString &master, QString *error = nullptr) const = 0;
    virtual QVector<SlaveInfo> scanSlaves(const QString &master, QString *error = nullptr) const = 0;
    virtual QString slaveInfo(const QString &master, int position, QString *error = nullptr) const = 0;
    virtual QString slaveXml(const QString &master, int position, QString *error = nullptr) const = 0;
    virtual QString pdos(const QString &master, int position, QString *error = nullptr) const = 0;
    virtual QString sdos(const QString &master, int position, QString *error = nullptr) const = 0;
    virtual QString upload(const QString &master, int position, const QString &index,
                           const QString &subIndex, QString *error = nullptr) const = 0;
    virtual bool download(const QString &master, int position, const QString &index,
                          const QString &subIndex, const QString &value,
                          const QString &type, QString *error = nullptr) const = 0;
    virtual bool setState(const QString &master, int position, const QString &state,
                          QString *error = nullptr) const = 0;
    virtual bool setAllStates(const QString &master, const QString &state,
                              QString *error = nullptr) const = 0;
    virtual bool rescan(const QString &master, QString *error = nullptr) const = 0;
    virtual QJsonArray hostDiagnostics(QString *error = nullptr) const = 0;
};
