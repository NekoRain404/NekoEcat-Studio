// DeviceManagerServiceTest — Tests for DeviceManagerService
//
// Test coverage:
//   - Default state and empty device list
//   - Invalid/negative device info queries
//   - Remove invalid device
//   - Signal validity
//   - Device discovery and offline add rejection

#include <QTest>
#include <QSignalSpy>
#include "infra/EcatClient.h"
#include "services/DeviceManagerService.h"

class DeviceManagerServiceTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  DeviceManagerService *svc_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    svc_ = new DeviceManagerService(client_, this);
  }

  void cleanup() {
    delete svc_;
    svc_ = nullptr;
    delete client_;
    client_ = nullptr;
  }

  // Verify default device count is zero and list is empty
  void testDefaultState() {
    QCOMPARE(svc_->deviceCount(), 0);
    QVERIFY(svc_->deviceList().isEmpty());
  }

  // Verify deviceInfo returns empty for out-of-range index
  void testDeviceInfoInvalid() {
    auto info = svc_->deviceInfo(99);
    QVERIFY(info.isEmpty());
  }

  // Verify deviceInfo returns empty for negative index
  void testDeviceInfoNegative() {
    auto info = svc_->deviceInfo(-1);
    QVERIFY(info.isEmpty());
  }

  // Verify removing invalid index does not change count
  void testRemoveDeviceInvalid() {
    svc_->removeDevice(99);
    QCOMPARE(svc_->deviceCount(), 0);
  }

  // Verify device signals are valid
  void testSignals() {
    QSignalSpy discoveredSpy(svc_, &DeviceManagerService::deviceDiscovered);
    QSignalSpy removedSpy(svc_, &DeviceManagerService::deviceRemoved);
    QSignalSpy stateSpy(svc_, &DeviceManagerService::deviceStateChanged);
    QVERIFY(discoveredSpy.isValid());
    QVERIFY(removedSpy.isValid());
    QVERIFY(stateSpy.isValid());
  }

  // Verify discoverDevices runs without error
  void testDiscoverDevices() {
    svc_->discoverDevices();
    QCOMPARE(svc_->deviceCount(), 0);
  }

  // Offline addDevice must not synthesize a device without bus evidence.
  void testAddDeviceFailsClosedWithoutBackendEvidence() {
    QSignalSpy changedSpy(svc_, &DeviceManagerService::deviceListChanged);
    QVERIFY(!svc_->addDevice(0));
    QCOMPARE(svc_->deviceCount(), 0);
    QVERIFY(svc_->deviceList().isEmpty());
    QCOMPARE(changedSpy.count(), 0);
  }

  // Offline scan must not synthesize a list-changed event.
  void testStartScanDoesNotEmitOfflineListChanged() {
    QSignalSpy changedSpy(svc_, &DeviceManagerService::deviceListChanged);
    svc_->startScan();
    QCOMPARE(changedSpy.count(), 0);
    QCOMPARE(svc_->deviceCount(), 0);
  }
};

QTEST_MAIN(DeviceManagerServiceTest)
#include "device_manager_service_test.moc"
