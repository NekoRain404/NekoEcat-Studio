#pragma once

// IgH ethercat CLI wrapper: parses stdout into structured domain objects.


#include "EthercatTypes.h"
#include "EcatService.h"

#include <QObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>

class EthercatCliBackend : public QObject, public EcatService {
    Q_OBJECT

    // Wraps the IgH `ethercat` CLI to provide structured access to master state,
    // slave info, PDO/SDO dictionaries, and SDO read/write operations.
    // All methods are synchronous and shell out to the CLI; not suitable for real-time use.
public:
    explicit EthercatCliBackend(QObject *parent = nullptr);

    QString masterText(const QString &master, QString *error = nullptr) const override;
    QVector<SlaveInfo> scanSlaves(const QString &master, QString *error = nullptr) const override;
    QString slaveInfo(const QString &master, int position, QString *error = nullptr) const override;
    QString slaveXml(const QString &master, int position, QString *error = nullptr) const override;
    QString pdos(const QString &master, int position, QString *error = nullptr) const override;
    QString sdos(const QString &master, int position, QString *error = nullptr) const override;
    QString upload(const QString &master, int position, const QString &index, const QString &subIndex, QString *error = nullptr) const override;
    bool download(const QString &master, int position, const QString &index, const QString &subIndex, const QString &value, const QString &type, QString *error = nullptr) const override;
    bool setState(const QString &master, int position, const QString &state, QString *error = nullptr) const override;
    bool setAllStates(const QString &master, const QString &state, QString *error = nullptr) const override;
    bool rescan(const QString &master, QString *error = nullptr) const override;
    QJsonArray hostDiagnostics(QString *error = nullptr) const override;

private:
    QString run(const QString &master, const QStringList &arguments, int *exitCode, QString *stdErr) const;
    QVector<SlaveInfo> parseSlaves(const QString &text) const;
};
