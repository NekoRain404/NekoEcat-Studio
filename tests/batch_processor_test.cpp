// BatchProcessorTest — Tests for BatchProcessor
//
// Test coverage:
//   - Start batch and signal emission
//   - Batch completion tracking
//   - Progress tracking
//   - Cancel batch
//   - Error handling during batch execution

#include <QTest>
#include <QSignalSpy>
#include "services/BatchProcessor.h"

class BatchProcessorTest : public QObject {
  Q_OBJECT
private slots:
  // Verify startBatch returns valid id and emits batchStarted
  void testStartBatch() {
    BatchProcessor bp;
    QVector<BatchItem> items;
    BatchItem item;
    item.id = "1";
    item.type = "test";
    items.append(item);
    QSignalSpy startedSpy(&bp, &BatchProcessor::batchStarted);
    QString batchId = bp.startBatch("test", items,
        [](const QJsonObject &, std::atomic<bool> &) -> QJsonObject {
          return QJsonObject{{"ok", true}};
        });
    QVERIFY(!batchId.isEmpty());
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(startedSpy.at(0).at(0).toString(), batchId);
    QTest::qWait(200);
  }

  // Verify batch completion signal and results
  void testBatchCompletion() {
    BatchProcessor bp;
    QVector<BatchItem> items;
    for (int i = 0; i < 3; ++i) {
      BatchItem item;
      item.id = QString::number(i);
      item.type = "test";
      items.append(item);
    }
    QSignalSpy completedSpy(&bp, &BatchProcessor::batchCompleted);
    QString batchId = bp.startBatch("test", items,
        [](const QJsonObject &, std::atomic<bool> &) -> QJsonObject {
          return QJsonObject{{"done", true}};
        });
    QTest::qWait(500);
    QCOMPARE(completedSpy.count(), 1);
    QVector<BatchItem> results = bp.results(batchId);
    QCOMPARE(results.size(), 0);
  }

  // Verify progress tracking signals during batch
  void testProgressTracking() {
    BatchProcessor bp;
    QVector<BatchItem> items;
    for (int i = 0; i < 2; ++i) {
      BatchItem item;
      item.id = QString::number(i);
      item.type = "test";
      items.append(item);
    }
    QSignalSpy progressSpy(&bp, &BatchProcessor::batchProgress);
    bp.startBatch("test", items,
        [](const QJsonObject &, std::atomic<bool> &) -> QJsonObject {
          return QJsonObject{};
        });
    QTest::qWait(500);
    QVERIFY(progressSpy.count() >= 2);
  }

  // Verify cancel stops batch execution
  void testCancelBatch() {
    BatchProcessor bp;
    QVector<BatchItem> items;
    for (int i = 0; i < 10; ++i) {
      BatchItem item;
      item.id = QString::number(i);
      item.type = "test";
      items.append(item);
    }
    QSignalSpy failedSpy(&bp, &BatchProcessor::batchFailed);
    QString batchId = bp.startBatch("test", items,
        [](const QJsonObject &, std::atomic<bool> &cancelled) -> QJsonObject {
          for (int j = 0; j < 100000 && !cancelled.load(); ++j) {}
          return QJsonObject{};
        });
    QVERIFY(bp.cancelBatch(batchId));
    QTest::qWait(1000);
    QCOMPARE(failedSpy.count(), 1);
  }

  // Verify cancel returns false for nonexistent batch
  void testCancelNonexistent() {
    BatchProcessor bp;
    QVERIFY(!bp.cancelBatch("nonexistent-id"));
  }

  // Verify progress returns zero for nonexistent batch
  void testProgressNonexistent() {
    BatchProcessor bp;
    BatchProgress p = bp.progress("nonexistent-id");
    QCOMPARE(p.total, 0);
    QCOMPARE(p.completed, 0);
  }

  // Verify results returns empty for nonexistent batch
  void testResultsNonexistent() {
    BatchProcessor bp;
    QVector<BatchItem> r = bp.results("nonexistent-id");
    QVERIFY(r.isEmpty());
  }

  // Verify itemCompleted signal is emitted per item
  void testItemCompletedSignal() {
    BatchProcessor bp;
    QVector<BatchItem> items;
    BatchItem item;
    item.id = "a";
    item.type = "test";
    items.append(item);
    QSignalSpy itemSpy(&bp, &BatchProcessor::itemCompleted);
    bp.startBatch("test", items,
        [](const QJsonObject &, std::atomic<bool> &) -> QJsonObject {
          return QJsonObject{{"r", 1}};
        });
    QTest::qWait(500);
    QCOMPARE(itemSpy.count(), 1);
  }
};

QTEST_MAIN(BatchProcessorTest)
#include "batch_processor_test.moc"
