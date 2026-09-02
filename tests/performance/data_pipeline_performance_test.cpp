#include "services/DataPipelineService.h"
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QTest>

class DataPipelinePerformanceTest : public QObject {
    Q_OBJECT
private slots:
    void testThroughput() {
        DataPipelineService svc;
        svc.setBufferSize(1024);

        QElapsedTimer timer;
        timer.start();

        const int count = 1000;
        QByteArray data(256, 'A');
        for (int i = 0; i < count; i++) {
            svc.process(data);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 5000);
        qDebug() << "DataPipeline throughput:" << count << "items in" << elapsed << "ms";
    }

    void testStageProcessing() {
        DataPipelineService svc;
        svc.addStage("filter", {{"type", "range"}, {"min", 0}, {"max", 100}});
        svc.addStage("transform", {{"type", "scale"}, {"factor", 2.0}});

        QCOMPARE(svc.stageCount(), 2);

        QElapsedTimer timer;
        timer.start();

        const int count = 100;
        QByteArray data(64, 'B');
        for (int i = 0; i < count; i++) {
            svc.process(data);
        }

        qint64 elapsed = timer.elapsed();
        QVERIFY(elapsed < 3000);
        qDebug() << "DataPipeline 2-stage:" << count << "items in" << elapsed << "ms";
    }

    void testSignalEmission() {
        DataPipelineService svc;
        QSignalSpy stageSpy(&svc, &DataPipelineService::stageCompleted);
        QSignalSpy finishSpy(&svc, &DataPipelineService::pipelineFinished);
        QSignalSpy dataSpy(&svc, &DataPipelineService::dataProcessed);
        QVERIFY(stageSpy.isValid());
        QVERIFY(finishSpy.isValid());
        QVERIFY(dataSpy.isValid());
    }

    void testThroughputMetric() {
        DataPipelineService svc;
        QCOMPARE(svc.throughput(), 0.0);
    }
};

QTEST_MAIN(DataPipelinePerformanceTest)
#include "data_pipeline_performance_test.moc"
