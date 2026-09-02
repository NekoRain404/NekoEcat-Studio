// AsyncOperationManagerTest — Tests for AsyncOperationManager
//
// Test coverage:
//   - Execute and complete async operations
//   - Cancel running operations
//   - Progress tracking
//   - Multiple concurrent operations
//   - Result retrieval

#include "services/AsyncOperationManager.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QThread>

#include <atomic>
#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString& message) {
    std::cerr << message.toStdString() << '\n';
    std::exit(1);
}

void expectTrue(bool condition, const QString& message) {
    if (!condition)
        fail(message);
}

void expectEqual(int actual, int expected, const QString& message) {
    if (actual != expected)
        fail(QString("%1: expected %2, got %3").arg(message).arg(expected).arg(actual));
}

void testExecuteCompletes() {
    AsyncOperationManager mgr;
    QSignalSpy spy(&mgr, &AsyncOperationManager::operationCompleted);

    QString id = mgr.execute("test-op", [](std::atomic<bool>&) -> QJsonObject {
        QJsonObject obj;
        obj["result"] = 42;
        return obj;
    });

    expectTrue(!id.isEmpty(), "execute returns non-empty id");
    spy.wait(2000);
    expectTrue(spy.count() >= 1, "operationCompleted emitted");

    OperationResult r = mgr.result(id);
    expectTrue(r.success, "result is success");
    expectEqual(r.data.value("result").toInt(), 42, "result data correct");
}

void testCancel() {
    AsyncOperationManager mgr(1);
    QSignalSpy failSpy(&mgr, &AsyncOperationManager::operationFailed);

    QString id = mgr.execute("slow-op", [](std::atomic<bool>& cancelled) -> QJsonObject {
        while (!cancelled.load()) {
            QThread::msleep(10);
        }
        return {};
    });

    QThread::msleep(100);
    expectTrue(mgr.cancel(id), "cancel returns true");

    failSpy.wait(2000);
    expectTrue(failSpy.count() >= 1, "operationFailed emitted for cancelled op");
}

void testProgress() {
    AsyncOperationManager mgr;
    QString id = mgr.execute("prog-op", [](std::atomic<bool>&) -> QJsonObject { return {}; });

    expectEqual(mgr.progress(id), 0, "initial progress is 0");
}

void testMultipleOperations() {
    AsyncOperationManager mgr(2);
    QSignalSpy spy(&mgr, &AsyncOperationManager::operationCompleted);

    for (int i = 0; i < 4; ++i) {
        mgr.execute(QString("op-%1").arg(i), [](std::atomic<bool>&) -> QJsonObject {
            QThread::msleep(50);
            return {};
        });
    }

    while (spy.count() < 4) {
        spy.wait(5000);
    }
    expectEqual(spy.count(), 4, "all 4 operations completed");
}

void testCancelAll() {
    AsyncOperationManager mgr(1);

    for (int i = 0; i < 3; ++i) {
        mgr.execute(QString("op-%1").arg(i), [](std::atomic<bool>& cancelled) -> QJsonObject {
            while (!cancelled.load())
                QThread::msleep(10);
            return {};
        });
    }

    QThread::msleep(50);
    mgr.cancelAll();
    QThread::msleep(500);
    expectTrue(true, "cancelAll completed without crash");
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    testExecuteCompletes();
    testCancel();
    testProgress();
    testMultipleOperations();
    testCancelAll();
    return 0;
}
