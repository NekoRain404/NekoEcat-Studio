#pragma once

// EtherCATDeploymentService — manages deployment operations for
// EtherCAT configurations to remote targets.
//
// Provides deploy, rollback, listing, and status tracking for
// configuration deployments across the network.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QString>

class EcatClient;
class EventBus;

struct DeploymentResult {
  QString id;
  QString target;
  QString config;
  QString status;
  QString timestamp;
  QString log;
};

struct ConfigData {
  QByteArray configuration;
  QString version;
  QByteArray checksum;
};

struct FirmwareData {
  QByteArray firmware;
  QString version;
  QByteArray checksum;
};

struct SoftwareData {
  QByteArray software;
  QString version;
  QByteArray checksum;
};

struct SystemData {
  QByteArray system;
  QString version;
  QByteArray checksum;
};

class EtherCATDeploymentService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATDeploymentService(EventBus *bus, EcatClient *client,
                                     QObject *parent = nullptr);

  DeploymentResult deployConfiguration(const QString &target,
                                       const QString &config);
  DeploymentResult rollbackDeployment(const QString &deploymentId);
  QVector<DeploymentResult> listDeployments();
  DeploymentResult getDeploymentStatus(const QString &deploymentId);

  bool deployConfiguration(int position, const ConfigData &data);
  bool deployFirmware(int position, const FirmwareData &data);
  bool deploySoftware(int position, const SoftwareData &data);
  bool deploySystem(int position, const SystemData &data);

signals:
  void deploymentCompleted(const DeploymentResult &result);
  void deploymentStarted(int position, const QString &type);
  void deploymentProgress(int position, int progress);
  void positionDeploymentCompleted(int position, bool success);

private:
  DeploymentResult makeResult(const QString &id, const QString &target,
                              const QString &config, const QString &status,
                              const QString &log);

  EventBus *bus_;
  EcatClient *client_;
  QVector<DeploymentResult> deployments_;
  int nextId_ = 1;
};
