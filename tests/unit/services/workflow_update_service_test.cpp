// WorkflowUpdateServiceTest — Tests for Workflow Update Service
//
// Test coverage:
//   - Check for updates returns available updates
//   - Download, install, and rollback fail closed without backend
//   - Rejected update requests do not emit synthetic success signals
//   - Empty URL download returns false
//   - Empty version install returns false
//   - Empty version rollback returns false
//   - Multiple update types

#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowUpdateService.h"

class WorkflowUpdateServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testCheckForUpdatesEmpty() {
      WorkflowUpdateService svc;
      auto updates = svc.checkForUpdates();
      QVERIFY(updates.isEmpty());
  }

  void testDownloadUpdate() {
      WorkflowUpdateService svc;
      QSignalSpy spy(&svc, &WorkflowUpdateService::updateDownloaded);

      WfUpdateInfo info;
      info.type = WfUpdateType::Firmware;
      info.version = QStringLiteral("1.2.0");
      info.downloadUrl = QStringLiteral("https://example.com/fw.bin");

      QVERIFY(!svc.downloadUpdate(info));
      QCOMPARE(spy.count(), 0);
  }

  void testInstallUpdate() {
      WorkflowUpdateService svc;
      QSignalSpy spy(&svc, &WorkflowUpdateService::updateInstalled);

      WfUpdateInfo info;
      info.type = WfUpdateType::Software;
      info.version = QStringLiteral("2.0.0");

      QVERIFY(!svc.installUpdate(info));
      QCOMPARE(spy.count(), 0);
  }

  void testRollbackUpdate() {
      WorkflowUpdateService svc;

      WfUpdateInfo info;
      info.type = WfUpdateType::Configuration;
      info.version = QStringLiteral("1.0.0");

      QVERIFY(!svc.rollbackUpdate(info));
  }

  void testDownloadEmptyUrlReturnsFalse() {
      WorkflowUpdateService svc;
      WfUpdateInfo info;
      info.version = QStringLiteral("1.0.0");
      QVERIFY(!svc.downloadUpdate(info));
  }

  void testInstallEmptyVersionReturnsFalse() {
      WorkflowUpdateService svc;
      WfUpdateInfo info;
      QVERIFY(!svc.installUpdate(info));
  }

  void testRollbackEmptyVersionReturnsFalse() {
      WorkflowUpdateService svc;
      WfUpdateInfo info;
      QVERIFY(!svc.rollbackUpdate(info));
  }

  void testUpdateInfoFields() {
      WfUpdateInfo info;
      info.type = WfUpdateType::System;
      info.version = QStringLiteral("3.0.0");
      info.description = QStringLiteral("System update");
      info.size = 1024;
      info.checksum = QStringLiteral("abc123");
      info.releaseDate = QDateTime::fromString(QStringLiteral("2025-01-01"), Qt::ISODate);
      info.downloadUrl = QStringLiteral("https://example.com/update");

      QCOMPARE(info.type, WfUpdateType::System);
      QCOMPARE(info.version, QString("3.0.0"));
      QCOMPARE(info.description, QString("System update"));
      QCOMPARE(info.size, 1024);
      QCOMPARE(info.checksum, QString("abc123"));
      QCOMPARE(info.downloadUrl, QString("https://example.com/update"));
  }

  void testMultipleUpdateTypes() {
      WorkflowUpdateService svc;
      QSignalSpy spy(&svc, &WorkflowUpdateService::updateDownloaded);

      WfUpdateInfo fw;
      fw.type = WfUpdateType::Firmware;
      fw.version = QStringLiteral("1.0.0");
      fw.downloadUrl = QStringLiteral("https://example.com/fw");

      WfUpdateInfo sw;
      sw.type = WfUpdateType::Software;
      sw.version = QStringLiteral("2.0.0");
      sw.downloadUrl = QStringLiteral("https://example.com/sw");

      QVERIFY(!svc.downloadUpdate(fw));
      QVERIFY(!svc.downloadUpdate(sw));
      QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(WorkflowUpdateServiceTest)
#include "workflow_update_service_test.moc"
