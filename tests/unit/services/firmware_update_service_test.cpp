// FirmwareUpdateServiceTest — Tests for FirmwareUpdateService
//
// Test coverage:
//   - Default state (not updating, zero progress)
//   - Cancel update when idle
//   - Signal validity (started, progress, completed, failed)
//   - Update check and start

#include <QTest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QThread>
#include "infra/EcatClient.h"
#include "services/FirmwareUpdateService.h"

class FirmwareUpdateServiceTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  FirmwareUpdateService *svc_ = nullptr;

  bool connectClientToDummyDaemon(QTcpServer &server) {
    if (!server.listen(QHostAddress::LocalHost, 0))
      return false;
    client_->connectToHost(QHostAddress::LocalHost, server.serverPort());
    for (int i = 0; i < 50 && !client_->isConnected(); ++i) {
      QCoreApplication::processEvents();
      QThread::msleep(10);
    }
    return client_->isConnected();
  }

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

  // Firmware updates must not be simulated by timer progress.
  void testStartUpdateFailsClosedWithConnectedDaemonOnly() {
    QTcpServer server;
    QVERIFY(connectClientToDummyDaemon(server));
    QSignalSpy startedSpy(svc_, &FirmwareUpdateService::updateStarted);
    QSignalSpy progressSpy(svc_, &FirmwareUpdateService::updateProgressChanged);
    QSignalSpy completedSpy(svc_, &FirmwareUpdateService::updateCompleted);

    QVERIFY(!svc_->startUpdate(0, "/tmp/firmware.bin"));
    QVERIFY(!svc_->isUpdating());
    QCOMPARE(svc_->updateProgress(), 0);
    QCOMPARE(startedSpy.count(), 0);
    QCOMPARE(progressSpy.count(), 0);
    QCOMPARE(completedSpy.count(), 0);
  }
};

QTEST_MAIN(FirmwareUpdateServiceTest)
#include "firmware_update_service_test.moc"
