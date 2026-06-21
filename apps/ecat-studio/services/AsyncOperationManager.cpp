#include "AsyncOperationManager.h"

#include <QTimer>
#include <QThreadPool>

// AsyncOperationManager.cpp — Thread-pooled async operation executor with queuing
//
// Implementation notes:
//   - Maintains a configurable max concurrent limit; excess operations are queued
//   - Each operation runs on QThreadPool with cancellation support via atomic flag
//   - Automatic timeout via QTimer::singleShot per operation

AsyncOperationManager::AsyncOperationManager(int maxConcurrent, QObject *parent)
    : QObject(parent), maxConcurrent_(maxConcurrent) {}

AsyncOperationManager::~AsyncOperationManager() {
  cancelAll();
  QThreadPool::globalInstance()->waitForDone();
  qDeleteAll(operations_);
}

QString AsyncOperationManager::execute(const QString &name, OperationFunc func,
                                       OperationPriority priority, int timeoutMs) {
  QMutexLocker locker(&mutex_);

  QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);

  auto *op = new Operation();
  op->id = id;
  op->name = name;
  op->priority = priority;
  op->state = OperationState::Pending;
  op->func = std::move(func);
  op->timeoutMs = timeoutMs;

  operations_.insert(id, op);

  if (runningCount_ < maxConcurrent_) {
    op->state = OperationState::Running;
    ++runningCount_;

    auto *manager = this;
    auto opId = id;
    auto opFunc = op->func;
    auto *cancelled = &op->cancelled;

    QThreadPool::globalInstance()->start([manager, opId, opFunc, cancelled]() {
      OperationResult result;
      try {
        QJsonObject data = opFunc(*cancelled);
        if (cancelled->load()) {
          result.success = false;
          result.error = "Cancelled";
        } else {
          result.success = true;
          result.data = data;
        }
      } catch (const std::exception &e) {
        result.success = false;
        result.error = QString::fromUtf8(e.what());
      } catch (...) {
        result.success = false;
        result.error = "Unknown error";
      }
      QMetaObject::invokeMethod(manager, [manager, opId, result]() {
        manager->onOperationFinished(opId, result);
      }, Qt::QueuedConnection);
    });

    emit operationStarted(id);

    if (timeoutMs > 0) {
      QTimer::singleShot(timeoutMs, this, [this, id]() {
        cancel(id);
      });
    }
  } else {
    queue_.enqueue(id);
  }

  return id;
}

bool AsyncOperationManager::cancel(const QString &operationId) {
  QMutexLocker locker(&mutex_);

  auto it = operations_.find(operationId);
  if (it == operations_.end()) return false;

  Operation *op = *it;
  op->cancelled.store(true);

  if (op->state == OperationState::Pending) {
    op->state = OperationState::Cancelled;
    queue_.removeAll(operationId);
    return true;
  }

  if (op->state == OperationState::Running) {
    op->state = OperationState::Cancelled;
    return true;
  }

  return false;
}

bool AsyncOperationManager::isRunning(const QString &operationId) const {
  QMutexLocker locker(&mutex_);
  auto it = operations_.find(operationId);
  return it != operations_.end() && (*it)->state == OperationState::Running;
}

int AsyncOperationManager::progress(const QString &operationId) const {
  QMutexLocker locker(&mutex_);
  auto it = operations_.find(operationId);
  return it != operations_.end() ? (*it)->progress : 0;
}

OperationResult AsyncOperationManager::result(const QString &operationId) const {
  QMutexLocker locker(&mutex_);
  auto it = operations_.find(operationId);
  return it != operations_.end() ? (*it)->result
                                 : OperationResult{false, {}, "Not found"};
}

void AsyncOperationManager::cancelAll() {
  QMutexLocker locker(&mutex_);
  for (Operation *op : operations_) {
    if (op->state == OperationState::Pending || op->state == OperationState::Running) {
      op->cancelled.store(true);
      op->state = OperationState::Cancelled;
    }
  }
  queue_.clear();
}

void AsyncOperationManager::processQueue() {
  while (runningCount_ < maxConcurrent_ && !queue_.isEmpty()) {
    QString id = queue_.dequeue();
    auto it = operations_.find(id);
    if (it == operations_.end() || (*it)->state == OperationState::Cancelled) continue;

    Operation *op = *it;
    op->state = OperationState::Running;
    ++runningCount_;

    auto *manager = this;
    auto opId = op->id;
    auto opFunc = op->func;
    auto *cancelled = &op->cancelled;

    QThreadPool::globalInstance()->start([manager, opId, opFunc, cancelled]() {
      OperationResult result;
      try {
        QJsonObject data = opFunc(*cancelled);
        if (cancelled->load()) {
          result.success = false;
          result.error = "Cancelled";
        } else {
          result.success = true;
          result.data = data;
        }
      } catch (const std::exception &e) {
        result.success = false;
        result.error = QString::fromUtf8(e.what());
      } catch (...) {
        result.success = false;
        result.error = "Unknown error";
      }
      QMetaObject::invokeMethod(manager, [manager, opId, result]() {
        manager->onOperationFinished(opId, result);
      }, Qt::QueuedConnection);
    });

    emit operationStarted(opId);
  }
}

void AsyncOperationManager::onOperationFinished(const QString &operationId,
                                                const OperationResult &result) {
  OperationState finalState;
  {
    QMutexLocker locker(&mutex_);

    auto it = operations_.find(operationId);
    if (it == operations_.end()) return;

    Operation *op = *it;
    op->result = result;
    --runningCount_;

    if (op->state == OperationState::Cancelled) {
      finalState = OperationState::Cancelled;
    } else if (result.success) {
      op->state = OperationState::Completed;
      finalState = OperationState::Completed;
    } else {
      op->state = OperationState::Failed;
      finalState = OperationState::Failed;
    }
  }

  if (finalState == OperationState::Cancelled) {
    emit operationFailed(operationId, "Cancelled");
  } else if (finalState == OperationState::Completed) {
    emit operationCompleted(operationId, result);
  } else {
    emit operationFailed(operationId, result.error);
  }

  processQueue();
}
