#pragma once

// WorkflowIntegrationHubService -- central hub for integrating workflows with
// external systems, data sources, processes, and services.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QJsonObject>

struct SystemConfig {
  QString name;
  QString type;
  QString endpoint;
  QString credentials;
};

struct DataConfig {
  QString source;
  QString destination;
  QJsonObject mapping;
  QJsonObject transformation;
};

struct ProcessConfig {
  QString workflow;
  QJsonObject triggers;
  QJsonObject actions;
  QJsonObject conditions;
};

struct ServiceConfig {
  QString service;
  QString endpoint;
  QString protocol;
  int timeout = 30;
};

class WorkflowIntegrationHubService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowIntegrationHubService(QObject *parent = nullptr);

  bool integrateSystem(const SystemConfig &config);
  bool integrateData(const DataConfig &config);
  bool integrateProcess(const ProcessConfig &config);
  bool integrateService(const ServiceConfig &config);

signals:
  void integrationConnected(const QString &system);
  void dataSynced(const QString &system, int recordCount);

private:
  bool validateEndpoint(const QString &endpoint) const;
};
