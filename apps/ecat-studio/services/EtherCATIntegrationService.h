#pragma once

// EtherCATIntegrationService — external integration request facade.
//
// PLC, SCADA, MES, ERP, and data-sync operations fail closed until a live
// integration backend is wired.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>

class EventBus;
class EcatClient;

struct PlcConfig {
  QString ipAddress;
  int port = 0;
  QString protocol;
  int timeout = 5000;
};

struct ScadaConfig {
  QString serverUrl;
  QString username;
  QString password;
};

struct MesConfig {
  QString endpoint;
  QString apiKey;
  QString version;
};

struct ErpConfig {
  QString host;
  QString database;
  QString credentials;
};

class EtherCATIntegrationService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATIntegrationService(EventBus *bus, EcatClient *client,
                                      QObject *parent = nullptr);

  bool connectToPLC(const PlcConfig &config);
  bool connectToSCADA(const ScadaConfig &config);
  bool connectToMES(const MesConfig &config);
  bool connectToERP(const ErpConfig &config);
  bool syncData(const QString &system);

signals:
  void connectedToSystem(const QString &system);
  void dataSynced(const QString &system, int recordCount);

private:
  EventBus *bus_;
  EcatClient *client_;
  bool backendReady() const;
};
