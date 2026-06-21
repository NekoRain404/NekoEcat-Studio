#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include "services/WorkflowUpdateService.h"

class WorkflowUpdatePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCheckForUpdatesThroughput() {
    WorkflowUpdateService svc;
    QElapsedTimer timer;
    timer.start();

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) {
      svc.checkForUpdates();
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Check-for-updates throughput:" << iterations << "calls in" << elapsed << "ms";
  }

  void testDownloadUpdateThroughput() {
    WorkflowUpdateService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      WfUpdateInfo info;
      info.type = WfUpdateType::Firmware;
      info.version = QStringLiteral("1.0.%1").arg(i);
      info.downloadUrl = QStringLiteral("http://example.com/fw/%1").arg(i);
      svc.downloadUpdate(info);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Download update throughput:" << count << "in" << elapsed << "ms";
  }

  void testInstallUpdateThroughput() {
    WorkflowUpdateService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      WfUpdateInfo info;
      info.type = WfUpdateType::Software;
      info.version = QStringLiteral("2.0.%1").arg(i);
      svc.installUpdate(info);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Install update throughput:" << count << "in" << elapsed << "ms";
  }

  void testSignalThroughput() {
    WorkflowUpdateService svc;
    QSignalSpy downloadedSpy(&svc, &WorkflowUpdateService::updateDownloaded);
    QSignalSpy installedSpy(&svc, &WorkflowUpdateService::updateInstalled);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      WfUpdateInfo info;
      info.type = WfUpdateType::Configuration;
      info.version = QStringLiteral("1.0.%1").arg(i);
      info.downloadUrl = QStringLiteral("http://example.com/cfg/%1").arg(i);
      svc.downloadUpdate(info);
      svc.installUpdate(info);
    }

    qint64 elapsed = timer.elapsed();
    QCOMPARE(downloadedSpy.count(), count);
    QCOMPARE(installedSpy.count(), count);
    QVERIFY(elapsed < 5000);
    qDebug() << "Signal throughput:" << count * 2 << "signals in" << elapsed << "ms";
  }

  void testRollbackThroughput() {
    WorkflowUpdateService svc;
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      WfUpdateInfo info;
      info.type = WfUpdateType::System;
      info.version = QStringLiteral("3.0.%1").arg(i);
      svc.rollbackUpdate(info);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Rollback throughput:" << count << "in" << elapsed << "ms";
  }

  void testMemoryStability() {
    WorkflowUpdateService svc;

    for (int round = 0; round < 10; round++) {
      for (int i = 0; i < 100; i++) {
        WfUpdateInfo info;
        info.type = static_cast<WfUpdateType>(i % 4);
        info.version = QStringLiteral("1.%1.%2").arg(round).arg(i);
        info.downloadUrl = QStringLiteral("http://example.com/%1/%2").arg(round).arg(i);
        svc.downloadUpdate(info);
        svc.installUpdate(info);
        svc.rollbackUpdate(info);
      }
    }

    qDebug() << "Memory stability: 3000 update operations across 10 rounds";
  }
};

QTEST_MAIN(WorkflowUpdatePerformanceTest)
#include "workflow_update_performance_test.moc"
