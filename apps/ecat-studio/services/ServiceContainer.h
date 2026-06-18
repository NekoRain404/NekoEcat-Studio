#pragma once
// ServiceContainer — holds all service instances and shared EcatClient.
// Passed to plugins at construction time so they can access domain services
// without knowing about MainWindow or EcatClient directly.

#include <QObject>

class EcatClient;
class EventBus;
class SdoService;
class WatchService;
class TopologyService;
class DcSyncService;
class AlEventService;
class SignalService;

class ServiceContainer : public QObject {
  Q_OBJECT
public:
  explicit ServiceContainer(QObject *parent = nullptr);

  EcatClient *client() const { return client_; }
  EventBus *eventBus() const { return eventBus_; }
  SdoService *sdo() const { return sdo_; }
  WatchService *watch() const { return watch_; }
  TopologyService *topology() const { return topology_; }
  DcSyncService *dcSync() const { return dcSync_; }
  AlEventService *alEvent() const { return alEvent_; }
  SignalService *signal() const { return signal_; }

private:
  EcatClient *client_ = nullptr;
  EventBus *eventBus_ = nullptr;
  SdoService *sdo_ = nullptr;
  WatchService *watch_ = nullptr;
  TopologyService *topology_ = nullptr;
  DcSyncService *dcSync_ = nullptr;
  AlEventService *alEvent_ = nullptr;
  SignalService *signal_ = nullptr;
};
