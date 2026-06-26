// EtherCATUpdateServiceTest — Tests for EtherCATUpdateService
//
// Test coverage:
//   - Update check/start fail closed without a live backend
//   - Update history is not synthesized offline
//   - Download/install/rollback fail closed without a live backend
//   - Success/progress signals are not synthesized offline

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATUpdateService.h"

class EtherCATUpdateServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Check for updates fails closed without a live backend.
  void testCheckForUpdatesFailsClosedWithoutBackend() {
    EtherCATUpdateService svc(nullptr, nullptr);
    auto result = svc.checkForUpdates(1);
    QCOMPARE(result.position, 1);
    QCOMPARE(result.status, QStringLiteral("Rejected"));
    QCOMPARE(result.progress, 0);
    QVERIFY(!result.id.isEmpty());
    QCOMPARE(svc.getUpdateHistory().size(), 0);
  }

  // Start update fails closed without a live backend.
  void testStartUpdateFailsClosedWithoutBackend() {
    EtherCATUpdateService svc(nullptr, nullptr);
    auto result = svc.startUpdate(1, "2.1.0");
    QCOMPARE(result.position, 1);
    QCOMPARE(result.version, QStringLiteral("2.1.0"));
    QCOMPARE(result.status, QStringLiteral("Rejected"));
    QCOMPARE(result.progress, 0);
    QCOMPARE(svc.getUpdateHistory().size(), 0);
  }

  // Cancel update when none in progress returns false
  // Cancel update when none in progress
  void testCancelUpdate() {
    EtherCATUpdateService svc(nullptr, nullptr);
    bool cancelled = svc.cancelUpdate();
    QVERIFY(!cancelled);
  }

  // Update history does not accumulate offline checks.
  void testGetUpdateHistoryDoesNotAccumulateOfflineChecks() {
    EtherCATUpdateService svc(nullptr, nullptr);
    svc.checkForUpdates(1);
    svc.checkForUpdates(2);
    auto history = svc.getUpdateHistory();
    QCOMPARE(history.size(), 0);
  }

  // updateProgressChanged is not emitted for offline start rejection.
  void testUpdateSignalNotEmittedWithoutBackend() {
    EtherCATUpdateService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateProgressChanged);
    svc.startUpdate(1, "2.0.0");
    QCOMPARE(spy.count(), 0);
  }

  // updateProgressChanged is not emitted for offline check rejection.
  void testCheckSignalNotEmittedWithoutBackend() {
    EtherCATUpdateService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateProgressChanged);
    svc.checkForUpdates(1);
    QCOMPARE(spy.count(), 0);
  }

  // Each rejected update request still gets a traceable unique ID.
  void testRejectedRequestsGetUniqueIds() {
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

  // Download update fails closed without a live backend.
  void testDownloadUpdateFailsClosedWithoutBackend() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    u.downloadUrl = "http://example.com/update.bin";
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateDownloaded);
    QVERIFY(!svc.downloadUpdate(u));
    QCOMPARE(spy.count(), 0);
  }

  // Download update with no URL fails
  // Download fails with no URL
  void testDownloadUpdateNoUrl() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    QVERIFY(!svc.downloadUpdate(u));
  }

  // Install update fails closed without a live backend.
  void testInstallUpdateFailsClosedWithoutBackend() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    u.version = "1.0";
    QSignalSpy spy(&svc, &EtherCATUpdateService::updateInstalled);
    QVERIFY(!svc.installUpdate(u));
    QCOMPARE(spy.count(), 0);
  }

  // Install update with no version fails
  // Install fails with no version
  void testInstallUpdateNoVersion() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    QVERIFY(!svc.installUpdate(u));
  }

  // Rollback update fails closed without a live backend.
  void testRollbackUpdateFailsClosedWithoutBackend() {
    EtherCATUpdateService svc(nullptr, nullptr);
    UpdateInfo u;
    u.version = "1.0";
    QVERIFY(!svc.rollbackUpdate(u));
  }
};

QTEST_MAIN(EtherCATUpdateServiceTest)
#include "ethercat_update_service_test.moc"
