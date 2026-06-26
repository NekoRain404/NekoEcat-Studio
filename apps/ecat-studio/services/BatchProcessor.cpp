#include "BatchProcessor.h"
#include <QUuid>
#include <QThread>

// BatchProcessor.cpp — Generic threaded batch item processor with per-item callbacks
//
// Implementation notes:
//   - Each batch runs on a dedicated QThread with atomic cancellation
//   - Tracks completed/failed counts atomically for thread-safe progress
//   - BatchState is heap-allocated and cleaned up after thread completion

BatchProcessor::BatchProcessor(QObject *parent) : QObject(parent) {
}

QString BatchProcessor::startBatch(const QString &name, QVector<BatchItem> items, BatchItemFunc func) {
  QString batchId = QUuid::createUuid().toString(QUuid::WithoutBraces);

  auto *state = new BatchState;
  state->name = name;
  state->items = items;
  state->func = func;

  {
    QMutexLocker locker(&mutex_);
    batches_[batchId] = state;
  }

  emit batchStarted(batchId);

  QThread::create([this, batchId, state]() {
    for (int i = 0; i < state->items.size(); ++i) {
      if (state->cancelled.load()) {
        break;
      }

      auto &item = state->items[i];
      try {
        item.result = state->func(item.params, state->cancelled);
        item.success = true;
        state->completed.fetch_add(1);
      } catch (const std::exception &e) {
        item.error = QString::fromStdString(e.what());
        item.success = false;
        state->failed.fetch_add(1);
      }

      BatchProgress progress;
      progress.total = state->items.size();
      progress.completed = state->completed.load();
      progress.failed = state->failed.load();
      progress.percent = (progress.completed * 100) / progress.total;

      emit batchProgress(batchId, progress);
      emit itemCompleted(batchId, item);
    }

    if (state->cancelled.load()) {
      emit batchFailed(batchId, "Batch cancelled");
    } else if (state->failed.load() > 0) {
      emit batchFailed(batchId, QString("Batch failed: %1 item(s) failed").arg(state->failed.load()));
    } else {
      emit batchCompleted(batchId, state->items);
    }

    QMutexLocker locker(&mutex_);
    batches_.remove(batchId);
    delete state;
  })->start();

  return batchId;
}

bool BatchProcessor::cancelBatch(const QString &batchId) {
  QMutexLocker locker(&mutex_);
  auto it = batches_.find(batchId);
  if (it != batches_.end()) {
    it.value()->cancelled.store(true);
    return true;
  }
  return false;
}

BatchProgress BatchProcessor::progress(const QString &batchId) const {
  QMutexLocker locker(&mutex_);
  auto it = batches_.find(batchId);
  if (it != batches_.end()) {
    BatchProgress p;
    p.total = it.value()->items.size();
    p.completed = it.value()->completed.load();
    p.failed = it.value()->failed.load();
    p.percent = p.total > 0 ? (p.completed * 100) / p.total : 0;
    return p;
  }
  return BatchProgress();
}

QVector<BatchItem> BatchProcessor::results(const QString &batchId) const {
  QMutexLocker locker(&mutex_);
  auto it = batches_.find(batchId);
  if (it != batches_.end()) {
    return it.value()->items;
  }
  return {};
}
