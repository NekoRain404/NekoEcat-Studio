// DataPipelineServiceTest — Tests for DataPipelineService
//
// Test coverage:
//   - Default state and buffer size
//   - Stage add/remove operations
//   - Data processing (empty and with data)
//   - Signal validity

#include "services/DataPipelineService.h"
#include <QSignalSpy>
#include <QTest>

class DataPipelineServiceTest : public QObject {
    Q_OBJECT
private:
    DataPipelineService* svc_ = nullptr;

private slots:
    void init() { svc_ = new DataPipelineService(this); }

    void cleanup() {
        delete svc_;
        svc_ = nullptr;
    }

    // Verify default stage count, throughput, and buffer size
    void testDefaultState() {
        QCOMPARE(svc_->stageCount(), 0);
        QCOMPARE(svc_->throughput(), 0.0);
        QCOMPARE(svc_->bufferSize(), 65536);
    }

    // Verify adding a stage returns correct index
    void testAddStage() {
        int idx = svc_->addStage("filter", {{"type", "range"}});
        QCOMPARE(idx, 0);
        QCOMPARE(svc_->stageCount(), 1);
    }

    // Verify adding multiple stages increments count
    void testAddMultipleStages() {
        svc_->addStage("filter", {{"type", "range"}});
        svc_->addStage("transform", {{"type", "scale"}});
        svc_->addStage("aggregate", {{"type", "sum"}});
        QCOMPARE(svc_->stageCount(), 3);
    }

    // Verify removing a stage decrements count
    void testRemoveStage() {
        svc_->addStage("filter", {{"type", "range"}});
        svc_->addStage("transform", {{"type", "scale"}});
        svc_->removeStage(0);
        QCOMPARE(svc_->stageCount(), 1);
    }

    // Verify removing invalid index is a no-op
    void testRemoveStageInvalid() {
        svc_->removeStage(99);
        QCOMPARE(svc_->stageCount(), 0);
    }

    // Verify buffer size can be changed
    void testSetBufferSize() {
        svc_->setBufferSize(2048);
        QCOMPARE(svc_->bufferSize(), 2048);
    }

    // Verify processing empty data does not crash
    void testProcessEmpty() {
        QByteArray data;
        svc_->process(data);
    }

    void testProcessWithoutRunningPipelineDoesNotEmitCompletion() {
        QSignalSpy finishSpy(svc_, &DataPipelineService::pipelineFinished);
        QSignalSpy dataSpy(svc_, &DataPipelineService::dataProcessed);

        const QByteArray result = svc_->process(QByteArray("raw"));

        QCOMPARE(result, QByteArray("raw"));
        QCOMPARE(finishSpy.count(), 0);
        QCOMPARE(dataSpy.count(), 0);
    }

    void testProcessWithoutStagesDoesNotEmitCompletion() {
        svc_->start();
        QSignalSpy finishSpy(svc_, &DataPipelineService::pipelineFinished);
        QSignalSpy dataSpy(svc_, &DataPipelineService::dataProcessed);

        const QByteArray result = svc_->process(QByteArray("raw"));

        QCOMPARE(result, QByteArray("raw"));
        QCOMPARE(finishSpy.count(), 0);
        QCOMPARE(dataSpy.count(), 0);
    }

    // Verify processing with data does not crash
    void testProcessWithData() {
        QByteArray data("Hello, World!");
        svc_->start();
        svc_->addStage("transform", {{"offset", 1}});
        svc_->process(data);
    }

    // Verify all expected signals are valid
    void testSignals() {
        QSignalSpy stageSpy(svc_, &DataPipelineService::stageCompleted);
        QSignalSpy finishSpy(svc_, &DataPipelineService::pipelineFinished);
        QSignalSpy dataSpy(svc_, &DataPipelineService::dataProcessed);
        QVERIFY(stageSpy.isValid());
        QVERIFY(finishSpy.isValid());
        QVERIFY(dataSpy.isValid());
    }
};

QTEST_MAIN(DataPipelineServiceTest)
#include "data_pipeline_service_test.moc"
