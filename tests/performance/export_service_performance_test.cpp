#include "services/ExportService.h"
#include <QElapsedTimer>
#include <QFile>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTemporaryDir>
#include <QTest>

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
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("test_export.csv"));
        timer.start();
        QVERIFY(svc.exportToCsv(&table, path));
        qint64 elapsed = timer.elapsed();

        QVERIFY(QFile::exists(path));
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
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("test_export.json"));
        timer.start();
        QVERIFY(svc.exportToJson(&table, path));
        qint64 elapsed = timer.elapsed();

        QVERIFY(QFile::exists(path));
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
