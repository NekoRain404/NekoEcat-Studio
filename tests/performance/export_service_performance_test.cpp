#include <QTest>
#include <QSignalSpy>
#include <QElapsedTimer>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "services/ExportService.h"

class ExportServicePerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCsvExportThroughput() {
    ExportService svc;
    QTableWidget table(1000, 10);
    for (int r = 0; r < 1000; r++)
      for (int c = 0; c < 10; c++)
        table.setItem(r, c, new QTableWidgetItem(QString("cell_%1_%2").arg(r).arg(c)));

    QElapsedTimer timer;
    timer.start();
    svc.exportToCsv(&table, "/tmp/test_export.csv");
    qint64 elapsed = timer.elapsed();

    QVERIFY(elapsed < 5000);
    qDebug() << "CSV export 1000x10 table:" << elapsed << "ms";
  }

  void testJsonExportThroughput() {
    ExportService svc;
    QTableWidget table(500, 5);
    for (int r = 0; r < 500; r++)
      for (int c = 0; c < 5; c++)
        table.setItem(r, c, new QTableWidgetItem(QString("val_%1_%2").arg(r).arg(c)));

    QElapsedTimer timer;
    timer.start();
    svc.exportToJson(&table, "/tmp/test_export.json");
    qint64 elapsed = timer.elapsed();

    QVERIFY(elapsed < 5000);
    qDebug() << "JSON export 500x5 table:" << elapsed << "ms";
  }

  void testSignalEmission() {
    ExportService svc;
    QSignalSpy completedSpy(&svc, &ExportService::exportCompleted);
    QSignalSpy failedSpy(&svc, &ExportService::exportFailed);
    QVERIFY(completedSpy.isValid());
    QVERIFY(failedSpy.isValid());
  }
};

QTEST_MAIN(ExportServicePerformanceTest)
#include "export_service_performance_test.moc"
