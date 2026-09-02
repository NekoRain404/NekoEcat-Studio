#pragma once

// DeviceManagerService — manages EtherCAT device discovery and configuration.
//
// Provides device discovery via EcatClient and maintains a local device list
// only from bus evidence reported by the daemon.
//
// This service provides device management capabilities for the EtherCAT
// network. It handles:
//   - Device discovery via bus scanning
//   - Device list management from discovered slave evidence
//   - Device information queries
//   - Device state monitoring
//   - Device configuration management
//
// Usage:
//   ServiceContainer *container = ...;
//   DeviceManagerService *deviceMgr = container->deviceManager();
//   deviceMgr->discoverDevices();
//   QVector<DeviceInfo> devices = deviceMgr->allDevices();
//   QJsonObject info = deviceMgr->deviceInfo(0);
//   // addDevice() returns false until a real add/configure backend exists.
//   deviceMgr->addDevice(1);
//   deviceMgr->configureDevice(0);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - Device discovery is O(n) where n is number of slaves
//   - Device lookup is O(1) by position
//   - Device list updates are O(1) for add/remove

#include <QJsonObject>
#include <QObject>
#include <QVector>

class EcatClient;

// Information about a discovered device.
struct DeviceInfo {
    int address = 0;  // Device address on the bus
    QString name;     // Device name
    QString state;    // Device state (OP, SAFEOP, PREOP, INIT)
    QString vendor;   // Vendor name
    QString product;  // Product name
    int revision = 0; // Hardware revision
};

class DeviceManagerService : public QObject {
    Q_OBJECT
public:
    explicit DeviceManagerService(EcatClient* client, QObject* parent = nullptr);

    // Discover devices on the EtherCAT bus.
    void discoverDevices();

    // Start a device scan.
    void startScan();

    // Add a device to the managed list.
    // Fails closed until a backend can confirm a real device/configuration.
    // @param position  Device position on the bus
    // @return true if device was added successfully
    bool addDevice(int position);

    // Remove a device from the managed list.
    // @param position  Device position on the bus
    // @return true if device was removed successfully
    bool removeDevice(int position);

    // Configure a device.
    // @param position  Device position on the bus
    void configureDevice(int position);

    // Reset all managed devices.
    void resetDevices();

    // Get device information as JSON.
    // @param position  Device position on the bus
    // @return JSON object with device information
    QJsonObject deviceInfo(int position) const;

    // Get the list of all managed devices as JSON.
    // @return Vector of JSON objects
    QVector<QJsonObject> deviceList() const;

    // Get the list of all discovered devices.
    // @return Vector of DeviceInfo structures
    QVector<DeviceInfo> allDevices() const;

    // Get the number of managed devices.
    // @return Number of devices
    int deviceCount() const;

signals:
    // Emitted when a new device is discovered.
    // @param device  JSON object with device information
    void deviceDiscovered(const QJsonObject& device);

    // Emitted when a device is removed.
    // @param position  Device position
    void deviceRemoved(int position);

    // Emitted when a device state changes.
    // @param position  Device position
    // @param state     New device state
    void deviceStateChanged(int position, const QString& state);

    // Emitted when the device list changes.
    void deviceListChanged();

private:
    EcatClient* client_;              // TCP client to ecatd daemon
    QVector<QJsonObject> devices_;    // Managed devices as JSON
    QVector<DeviceInfo> deviceInfos_; // Discovered devices
};
