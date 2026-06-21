// EtherCATUpdateServiceTest — Tests for EtherCATUpdateService
//
// Test coverage:
//   - Update check, start, and cancel
//   - Update history and unique ID generation
//   - Download and install (valid + invalid)
//   - Update rollback
//   - Signal emission for update progress

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATUpdateService.h"

class EtherCATUpdateServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Check for updates and verify status and ID
  // Check for updates on a slave
  void testCheckForUpdates() {
    EtherCATUpdateService svc(nullptr, nullptr);
    auto result = svc.checkForUpdates(1);
    QCOMPARE(result.position, 1);
    QCOMPARE(result.status, QStringLiteral("Available"));
    QVERIFY(!result.id.isEmpty());
  }

  // Start update and verify version, status, and progress
  // Start update and verify completion
  void testStartUpdate() {
    EtherCATUpdateService svc(nullptr, nullptr);
    auto result = svc.startUpdate(1, "2.1.0");
    QCOMPARE(result.position, 1);
    QCOMPARE(result.version, QStringLiteral("2.1.0"));
    QCOMPARE(result.status, QStringLiteral("Completed"));
    QCOMPARE(result.progress, 100);
  }

  // Cancel update when none in progress returns false
  // Cancel update when none in progress
  void testCancelUpdate() {
    EtherCATUpdateService svc(nullptr, nullptr);
    bool cancelled = svc.cancelUpdate();
    QVERIFY(!cancelled);
  }

  // Update history tracks multiple checks
  // Update history tracks all checks
  void testGetUpdateHistory() {
    EtherCATUpdateService svc(nullptr, nullptr);
    svc.checkForUpdates(1);
    svc.checkForUpdates(2);
    auto history = svc.getUpdateHistory();
    QCOMPARE(history.size(), 2);
    QCOMPARE(history[0].position, 1);
    QCOMPARE(history[1].position, 2);
  }

  // Verify updateProgressChanged signal on start
  // updateProgressChanged signal fires on start
  void testUpdateSignal() {
    EtherCATUpdateService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateProgressChanged);
    svc.startUpdate(1, "2.0.0");
    QCOMPARE(spy.count(), 1);
  }

  // Verify updateProgressChanged signal on check
  // updateProgressChanged signal fires on check
  void testCheckSignal() {
    EtherCATUpdateService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateProgressChanged);
    svc.checkForUpdates(1);
    QCOMPARE(spy.count(), 1);
  }

  // Each update check generates a unique ID
  // Each update check gets unique ID
  void testUniqueIds() {
    EtherCATUpdateService svc(nullptr, nullptr);
    auto r1 = svc.checkForUpdates(1);
    auto r2 = svc.checkForUpdates(2);
    QVERIFY(r1.id != r2.id);
  }

  // Parameterless checkForUpdates returns empty list
  // Check for updates (no position) returns empty
  void testCheckForUpdatesOverload() {
    EtherCATUpdateService svc(nullptr, nullptr);
    auto updates = svc.checkForUpdates();
    QVERIFY(updates.isEmpty());
  }

  // Download update with valid URL succeeds
  // Download update with valid URL
  void testDownloadUpdateValid() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    u.downloadUrl = "http://example.com/update.bin";
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateDownloaded);
    QVERIFY(svc.downloadUpdate(u));
    QCOMPARE(spy.count(), 1);
  }

  // Download update with no URL fails
  // Download fails with no URL
  void testDownloadUpdateNoUrl() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    QVERIFY(!svc.downloadUpdate(u));
  }

  // Install update with valid version succeeds
  // Install update with valid version
  void testInstallUpdateValid() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    u.version = "1.0";
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateInstalled);
    QVERIFY(svc.installUpdate(u));
    QCOMPARE(spy.count(), 1);
  }

  // Install update with no version fails
  // Install fails with no version
  void testInstallUpdateNoVersion() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    QVERIFY(!svc.installUpdate(u));
  }

  // Rollback update succeeds
  // Rollback an installed update
  void testRollbackUpdate() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    u.version = "1.0";
    QVERIFY(svc.rollbackUpdate(u));
  }
};

QTEST_MAIN(EtherCATUpdateServiceTest)
#include "ethercat_update_service_test.moc"
