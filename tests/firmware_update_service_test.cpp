// FirmwareUpdateServiceTest — Tests for FirmwareUpdateService
//
// Test coverage:
//   - Default state (not updating, zero progress)
//   - Cancel update when idle
//   - Signal validity (started, progress, completed, failed)
//   - Update check and start

#include <QTest>
#include <QSignalSpy>
#include "infra/EcatClient.h"
#include "services/FirmwareUpdateService.h"

class FirmwareUpdateServiceTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  FirmwareUpdateService *svc_ = nullptr;

private slots:
  // Initialize service before each test
  void init() {
    client_ = new EcatClient(this);
    svc_ = new FirmwareUpdateService(client_, this);
  }

  // Cleanup service after each test
  void cleanup() {
    delete svc_;
    svc_ = nullptr;
    delete client_;
    client_ = nullptr;
  }

  // Default state: not updating, zero progress
  void testDefaultState() {
    QVERIFY(!svc_->isUpdating());
    QCOMPARE(svc_->updateProgress(), 0);
  }

  // Cancel update when idle is safe
  void testCancelUpdateWhenIdle() {
    svc_->cancelUpdate();
    QVERIFY(!svc_->isUpdating());
  }

  // All update signals are valid
  void testSignals() {
    QSignalSpy startedSpy(svc_, &FirmwareUpdateService::updateStarted);
    QSignalSpy progressSpy(svc_, &FirmwareUpdateService::updateProgressChanged);
    QSignalSpy completedSpy(svc_, &FirmwareUpdateService::updateCompleted);
    QSignalSpy failedSpy(svc_, &FirmwareUpdateService::updateFailed);
    QVERIFY(startedSpy.isValid());
    QVERIFY(progressSpy.isValid());
    QVERIFY(completedSpy.isValid());
    QVERIFY(failedSpy.isValid());
  }

  // Check for firmware updates on slave
  void testCheckForUpdates() {
    svc_->checkForUpdates(0);
  }

  // Start firmware update on slave
  void testStartUpdate() {
    svc_->startUpdate(0, "/tmp/firmware.bin");
  }
};

QTEST_MAIN(FirmwareUpdateServiceTest)
#include "firmware_update_service_test.moc"
