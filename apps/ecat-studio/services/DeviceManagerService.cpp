#include "DeviceManagerService.h"
#include "infra/EcatClient.h"
#include "EthercatTypes.h"

// DeviceManagerService.cpp — Discovers and manages EtherCAT slave devices
//
// Implementation notes:
//   - Listens to EcatClient::slavesChanged to auto-populate device list
//   - Maintains both QJsonObject and DeviceInfo representations of each device
//   - Supports scan/reset operations with signal notifications
//   - Does not synthesize devices without slave evidence from the daemon

DeviceManagerService::DeviceManagerService(EcatClient *client,
                                           QObject *parent)
    : QObject(parent), client_(client) {
  connect(client_, &EcatClient::slavesChanged, this,
          [this](const QVector<SlaveInfo> &slaves) {
            devices_.clear();
            deviceInfos_.clear();
            for (const auto &s : slaves) {
              QJsonObject dev;
              dev["position"] = s.position;
              dev["name"] = s.name;
              dev["state"] = s.state;
              dev["flags"] = s.flags;
              devices_.append(dev);

              DeviceInfo info;
              info.address = s.position;
              info.name = s.name;
              info.state = s.state;
              deviceInfos_.append(info);

              emit deviceDiscovered(dev);
            }
            emit deviceListChanged();
          });
}

void DeviceManagerService::discoverDevices() {
  if (!client_ || !client_->isConnected()) return;
  client_->scan();
}

bool DeviceManagerService::addDevice(int position) {
  Q_UNUSED(position);
  return false;
}

bool DeviceManagerService::removeDevice(int position) {
  for (int i = 0; i < devices_.size(); ++i) {
    if (devices_[i]["position"].toInt() == position) {
      devices_.remove(i);
      emit deviceRemoved(position);
      return true;
    }
  }
  return false;
}

QJsonObject DeviceManagerService::deviceInfo(int position) const {
  for (const auto &dev : devices_) {
    if (dev["position"].toInt() == position) return dev;
  }
  return {};
}

QVector<QJsonObject> DeviceManagerService::deviceList() const {
  return devices_;
}

int DeviceManagerService::deviceCount() const { return devices_.size(); }

void DeviceManagerService::startScan() {
  const int previousCount = devices_.size();
  discoverDevices();
  if (!client_ || !client_->isConnected()) return;
  if (devices_.size() != previousCount) emit deviceListChanged();
}

void DeviceManagerService::configureDevice(int position) {
  Q_UNUSED(position);
}

void DeviceManagerService::resetDevices() {
  devices_.clear();
  deviceInfos_.clear();
  emit deviceListChanged();
}

QVector<DeviceInfo> DeviceManagerService::allDevices() const {
  return deviceInfos_;
}
