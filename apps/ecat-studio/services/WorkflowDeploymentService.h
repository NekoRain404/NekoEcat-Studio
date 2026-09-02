#pragma once

// WorkflowDeploymentService -- deployment request facade for configuration,
// firmware, software, and system updates to EtherCAT slaves in NekoEcat Studio.
// Until a real deployment backend is wired, validated requests fail closed and
// do not emit synthetic started/progress/completed signals.
//
// Thread safety: main (GUI) thread only.

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QString>

struct WfConfigData {
    QByteArray configuration;
    QString version;
    QString checksum;
};

struct WfFirmwareData {
    QByteArray firmware;
    QString version;
    QString checksum;
};

struct WfSoftwareData {
    QByteArray software;
    QString version;
    QString checksum;
};

struct WfSystemData {
    QByteArray system;
    QString version;
    QString checksum;
};

class WorkflowDeploymentService : public QObject {
    Q_OBJECT
public:
    explicit WorkflowDeploymentService(QObject* parent = nullptr);

    bool deployConfiguration(int position, const WfConfigData& data);
    bool deployFirmware(int position, const WfFirmwareData& data);
    bool deploySoftware(int position, const WfSoftwareData& data);
    bool deploySystem(int position, const WfSystemData& data);

signals:
    void deploymentStarted(int position, const QString& type);
    void deploymentProgress(int position, int progress);
    void deploymentCompleted(int position, bool success);

private:
    bool validateChecksum(const QByteArray& payload, const QString& checksum);
    bool executeDeployment(int position, const QString& type, const QByteArray& payload, const QString& version);
};
