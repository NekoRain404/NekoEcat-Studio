#pragma once

// IgH ethercat CLI wrapper: parses stdout into structured domain objects.


#include "EthercatTypes.h"

#include <QObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>

class EthercatCliBackend : public QObject {
    Q_OBJECT

    // Wraps the IgH `ethercat` CLI to provide structured access to master state,
    // slave info, PDO/SDO dictionaries, and SDO read/write operations.
    // All methods are synchronous and shell out to the CLI; not suitable for real-time use.
public:
    explicit EthercatCliBackend(QObject *parent = nullptr);

    QString masterText(const QString &master, QString *error = nullptr) const;
    QVector<SlaveInfo> scanSlaves(const QString &master, QString *error = nullptr) const;
    QString slaveInfo(const QString &master, int position, QString *error = nullptr) const;
    QString slaveXml(const QString &master, int position, QString *error = nullptr) const;
    QString pdos(const QString &master, int position, QString *error = nullptr) const;
    QString sdos(const QString &master, int position, QString *error = nullptr) const;
    QString upload(const QString &master, int position, const QString &index, const QString &subIndex, QString *error = nullptr) const;
    bool download(const QString &master, int position, const QString &index, const QString &subIndex, const QString &value, const QString &type, QString *error = nullptr) const;
    bool setState(const QString &master, int position, const QString &state, QString *error = nullptr) const;
    bool setAllStates(const QString &master, const QString &state, QString *error = nullptr) const;
    bool rescan(const QString &master, QString *error = nullptr) const;
    QJsonArray hostDiagnostics(QString *error = nullptr) const;

private:
    QString run(const QString &master, const QStringList &arguments, int *exitCode, QString *stdErr) const;
    QVector<SlaveInfo> parseSlaves(const QString &text) const;
};
