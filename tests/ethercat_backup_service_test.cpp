// EtherCATBackupServiceTest — Tests for EtherCATBackupService
//
// Test coverage:
//   - Default and custom backup directory
//   - Full, incremental, differential, and selective backup
//   - Restore backup and restore non-existent file
//   - Backup and restore signal emission

#include <QTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include "services/EtherCATBackupService.h"

class EtherCATBackupServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify default backup directory is ./backups
  void testDefaultState() {
    EtherCATBackupService svc;
    QCOMPARE(svc.backupDirectory(), QStringLiteral("./backups"));
  }

  // Verify setBackupDirectory updates the path
  void testSetBackupDirectory() {
    EtherCATBackupService svc;
    svc.setBackupDirectory(QStringLiteral("/tmp/mybackups"));
    QCOMPARE(svc.backupDirectory(), QStringLiteral("/tmp/mybackups"));
  }

  // Verify full backup succeeds and emits signal
  void testCreateFullBackup() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    QSignalSpy spy(&svc, &EtherCATBackupService::backupCompleted);
    auto result = svc.createFullBackup();
    QVERIFY(result.success);
    QVERIFY(!result.backupPath.isEmpty());
    QVERIFY(result.backupSize > 0);
    QCOMPARE(spy.count(), 1);
  }

  // Verify incremental backup contains changed configs
  void testCreateIncrementalBackup() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    auto result = svc.createIncrementalBackup();
    QVERIFY(result.success);
    QVERIFY(result.items.contains(QStringLiteral("changed_configs")));
  }

  // Verify differential backup contains all configs
  void testCreateDifferentialBackup() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    auto result = svc.createDifferentialBackup();
    QVERIFY(result.success);
    QVERIFY(result.items.contains(QStringLiteral("all_configs")));
  }

  // Verify selective backup includes only specified items
  void testCreateSelectiveBackup() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    QStringList items;
    items << QStringLiteral("master_config") << QStringLiteral("timing_config");
    auto result = svc.createSelectiveBackup(items);
    QVERIFY(result.success);
    QCOMPARE(result.items.size(), 2);
  }

  // Verify restore from valid backup succeeds and emits signal
  void testRestoreBackup() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    auto backup = svc.createFullBackup();
    QVERIFY(backup.success);

    QSignalSpy spy(&svc, &EtherCATBackupService::restoreCompleted);
    bool ok = svc.restoreBackup(backup.backupPath);
    QVERIFY(ok);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
  }

  // Verify restore from non-existent file fails
  void testRestoreNonExistentFile() {
    EtherCATBackupService svc;
    QSignalSpy spy(&svc, &EtherCATBackupService::restoreCompleted);
    bool ok = svc.restoreBackup(QStringLiteral("/nonexistent/backup.json"));
    QVERIFY(!ok);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
  }

  // Verify all backup types emit backupCompleted signal
  void testBackupSignals() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    QSignalSpy fullSpy(&svc, &EtherCATBackupService::backupCompleted);
    svc.createFullBackup();
    QCOMPARE(fullSpy.count(), 1);

    QSignalSpy incrSpy(&svc, &EtherCATBackupService::backupCompleted);
    svc.createIncrementalBackup();
    QCOMPARE(incrSpy.count(), 1);

    QSignalSpy diffSpy(&svc, &EtherCATBackupService::backupCompleted);
    svc.createDifferentialBackup();
    QCOMPARE(diffSpy.count(), 1);
  }
};

QTEST_MAIN(EtherCATBackupServiceTest)
#include "ethercat_backup_service_test.moc"
