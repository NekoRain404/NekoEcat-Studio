#include <QTest>
#include <QElapsedTimer>
#include <QTemporaryDir>
#include <QFile>
#include "services/EtherCATBackupService.h"

class EtherCATBackupPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateFullBackupRejectionThroughput() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      QVERIFY(!svc.createFullBackup().success);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 createFullBackup() rejections:" << elapsed << "ms";
  }

  void testCreateIncrementalBackupRejectionThroughput() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      QVERIFY(!svc.createIncrementalBackup().success);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 createIncrementalBackup() rejections:" << elapsed << "ms";
  }

  void testCreateSelectiveBackupRejectionThroughput() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QStringList items;
    items << "master_config" << "timing_config";
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      QVERIFY(!svc.createSelectiveBackup(items).success);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 createSelectiveBackup() rejections:" << elapsed << "ms";
  }

  void testRestoreBackupRejectionThroughput() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    const QString backupPath = dir.path() + QStringLiteral("/backup.json");
    QFile f(backupPath);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("{\"type\":\"full\"}");
    f.close();
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      QVERIFY(!svc.restoreBackup(backupPath));
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 restoreBackup() rejections:" << elapsed << "ms";
  }

  void testMixedBackupRejectionThroughput() {
    EtherCATBackupService svc;
    QTemporaryDir dir;
    svc.setBackupDirectory(dir.path());
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 250; i++) {
      QVERIFY(!svc.createFullBackup().success);
      QVERIFY(!svc.createIncrementalBackup().success);
      QVERIFY(!svc.createDifferentialBackup().success);
      QVERIFY(!svc.createSelectiveBackup({"config"}).success);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "1000 mixed backup rejections:" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATBackupPerformanceTest)
#include "ethercat_backup_performance_test.moc"
