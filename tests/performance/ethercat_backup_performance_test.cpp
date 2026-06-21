#include <QTest>
#include <QElapsedTimer>
#include <QTemporaryDir>
#include "services/EtherCATBackupService.h"

class EtherCATBackupPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateFullBackupPerformance() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.createFullBackup();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 createFullBackup() calls:" << elapsed << "ms";
  }

  void testCreateIncrementalBackupPerformance() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.createIncrementalBackup();
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 createIncrementalBackup() calls:" << elapsed << "ms";
  }

  void testCreateSelectiveBackupPerformance() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QStringList items;
    items << "master_config" << "timing_config";
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.createSelectiveBackup(items);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 createSelectiveBackup() calls:" << elapsed << "ms";
  }

  void testRestoreBackupPerformance() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    auto backup = svc.createFullBackup();
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.restoreBackup(backup.backupPath);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 restoreBackup() calls:" << elapsed << "ms";
  }

  void testMixedBackupPerformance() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 250; i++) {
      svc.createFullBackup();
      svc.createIncrementalBackup();
      svc.createDifferentialBackup();
      svc.createSelectiveBackup({"config"});
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 mixed backup calls:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATBackupPerformanceTest)
#include "ethercat_backup_performance_test.moc"
