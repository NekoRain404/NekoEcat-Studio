// EtherCATBackupServiceTest — Tests for EtherCATBackupService
//
// Test coverage:
//   - Default and custom backup directory
//   - Full, incremental, differential, and selective backup fail closed without a source
//   - Restore backup fails closed without a restore backend

#include <QTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QFile>
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

  // Verify full backup fails closed without a live source.
  void testCreateFullBackupFailsClosedWithoutSource() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    QSignalSpy spy(&svc, &EtherCATBackupService::backupCompleted);
    auto result = svc.createFullBackup();
    QVERIFY(!result.success);
    QVERIFY(result.backupPath.isEmpty());
    QCOMPARE(result.backupSize, 0);
    QCOMPARE(spy.count(), 0);
  }

  // Verify incremental backup fails closed without a live source.
  void testCreateIncrementalBackupFailsClosedWithoutSource() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    auto result = svc.createIncrementalBackup();
    QVERIFY(!result.success);
    QVERIFY(result.items.isEmpty());
  }

  // Verify differential backup fails closed without a live source.
  void testCreateDifferentialBackupFailsClosedWithoutSource() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    auto result = svc.createDifferentialBackup();
    QVERIFY(!result.success);
    QVERIFY(result.items.isEmpty());
  }

  // Verify selective backup fails closed without a live source.
  void testCreateSelectiveBackupFailsClosedWithoutSource() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    QStringList items;
    items << QStringLiteral("master_config") << QStringLiteral("timing_config");
    auto result = svc.createSelectiveBackup(items);
    QVERIFY(!result.success);
    QVERIFY(result.items.isEmpty());
  }

  // Verify restore does not treat local JSON as a successful device restore.
  void testRestoreBackupFailsClosedWithoutBackend() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    const QString backupPath = dir.path() + QStringLiteral("/backup.json");
    QFile f(backupPath);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("{\"type\":\"full\"}");
    f.close();

    QSignalSpy spy(&svc, &EtherCATBackupService::restoreCompleted);
    bool ok = svc.restoreBackup(backupPath);
    QVERIFY(!ok);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
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

  // Verify rejected backup requests do not emit completed signals.
  void testBackupSignalsNotEmittedWithoutSource() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    svc.setBackupDirectory(dir.path());

    QSignalSpy fullSpy(&svc, &EtherCATBackupService::backupCompleted);
    svc.createFullBackup();
    QCOMPARE(fullSpy.count(), 0);

    QSignalSpy incrSpy(&svc, &EtherCATBackupService::backupCompleted);
    svc.createIncrementalBackup();
    QCOMPARE(incrSpy.count(), 0);

    QSignalSpy diffSpy(&svc, &EtherCATBackupService::backupCompleted);
    svc.createDifferentialBackup();
    QCOMPARE(diffSpy.count(), 0);
  }

  void testImplementationDoesNotContainSyntheticBackupPayloads() {
    QFile source(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/services/EtherCATBackupService.cpp"));
    QVERIFY(source.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(source.readAll());

    QVERIFY2(!text.contains(QStringLiteral("master_config")),
             "Backup service must not hard-code EtherCAT backup payloads");
    QVERIFY2(!text.contains(QStringLiteral("emit restoreCompleted(true)")),
             "Backup service must not report local JSON validation as restore success");
  }
};

QTEST_MAIN(EtherCATBackupServiceTest)
#include "ethercat_backup_service_test.moc"
