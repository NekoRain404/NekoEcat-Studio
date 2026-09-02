// CommandDispatcherTest — Tests for CommandDispatcher
//
// Test coverage:
//   - Dispatch unregistered method returns error
//   - Dispatch calls registered handler
//   - Handler receives correct params
//   - Handler failure propagation
//   - Multiple handler registration

#include "CommandDispatcher.h"

#include <QCoreApplication>
#include <QJsonObject>

#include <cstdlib>
#include <iostream>

namespace {

int failures = 0;

void fail(const QString& msg) {
    std::cerr << msg.toStdString() << '\n';
    ++failures;
}

void expectTrue(bool cond, const QString& msg) {
    if (!cond)
        fail(msg);
}

void expectEqual(const QString& actual, const QString& expected, const QString& msg) {
    if (actual != expected)
        fail(QString("%1: expected '%2', got '%3'").arg(msg, expected, actual));
}

void expectEqual(int actual, int expected, const QString& msg) {
    if (actual != expected)
        fail(QString("%1: expected %2, got %3").arg(msg).arg(expected).arg(actual));
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // Test 1: dispatch returns unknown-method error for unregistered method
    {
        CommandDispatcher dispatcher;
        QJsonObject request = {{"id", "1"}, {"method", "nonexistent"}, {"params", {}}};
        QJsonObject response = dispatcher.dispatch(request);
        expectEqual(response["id"].toString(), QString("1"), "T1: id echoed");
        expectTrue(!response["ok"].toBool(), "T1: ok is false");
        expectTrue(response["error"].toObject()["message"].toString().contains("nonexistent"),
                   "T1: error mentions method name");
    }

    // Test 2: dispatch calls registered handler
    {
        CommandDispatcher dispatcher;
        bool called = false;
        dispatcher.registerHandler("ping", [&](const QString& id, const QJsonObject&) -> QJsonObject {
            called = true;
            return CommandDispatcher::success(id, {{"name", "ecatd"}});
        });
        QJsonObject response = dispatcher.dispatch({{"id", "42"}, {"method", "ping"}, {"params", {}}});
        expectTrue(called, "T2: handler was called");
        expectEqual(response["id"].toString(), QString("42"), "T2: id echoed");
        expectTrue(response["ok"].toBool(), "T2: ok is true");
        expectEqual(response["result"].toObject()["name"].toString(), QString("ecatd"), "T2: result correct");
    }

    // Test 3: handler receives correct params
    {
        CommandDispatcher dispatcher;
        QJsonObject receivedParams;
        dispatcher.registerHandler("test", [&](const QString& id, const QJsonObject& params) -> QJsonObject {
            receivedParams = params;
            return CommandDispatcher::success(id);
        });
        dispatcher.dispatch({{"id", "7"}, {"method", "test"}, {"params", QJsonObject{{"pos", 3}}}});
        expectEqual(receivedParams["pos"].toInt(), 3, "T3: params passed through");
    }

    // Test 4: handler returning failure propagates correctly
    {
        CommandDispatcher dispatcher;
        dispatcher.registerHandler("fail", [](const QString& id, const QJsonObject&) -> QJsonObject {
            return CommandDispatcher::failure(id, "boom");
        });
        QJsonObject response = dispatcher.dispatch({{"id", "9"}, {"method", "fail"}, {"params", {}}});
        expectTrue(!response["ok"].toBool(), "T4: failure ok=false");
        expectEqual(response["error"].toObject()["message"].toString(), QString("boom"), "T4: error message");
    }

    // Test 5: registerHandler overwrites previous handler
    {
        CommandDispatcher dispatcher;
        dispatcher.registerHandler("dup", [](const QString& id, const QJsonObject&) -> QJsonObject {
            return CommandDispatcher::success(id, {{"v", 1}});
        });
        dispatcher.registerHandler("dup", [](const QString& id, const QJsonObject&) -> QJsonObject {
            return CommandDispatcher::success(id, {{"v", 2}});
        });
        QJsonObject response = dispatcher.dispatch({{"id", "10"}, {"method", "dup"}, {"params", {}}});
        expectEqual(response["result"].toObject()["v"].toInt(), 2, "T5: last registration wins");
    }

    // Test 6: empty method name returns error
    {
        CommandDispatcher dispatcher;
        QJsonObject response = dispatcher.dispatch({{"id", "11"}, {"method", ""}, {"params", {}}});
        expectTrue(!response["ok"].toBool(), "T6: empty method fails");
    }

    // Test 7: missing id still produces valid response
    {
        CommandDispatcher dispatcher;
        dispatcher.registerHandler("noop", [](const QString& id, const QJsonObject&) -> QJsonObject {
            return CommandDispatcher::success(id);
        });
        QJsonObject response = dispatcher.dispatch({{"method", "noop"}, {"params", {}}});
        expectTrue(response["ok"].toBool(), "T7: missing id still works");
    }

    if (failures > 0) {
        std::cerr << failures << " test(s) FAILED\n";
        return 1;
    }
    std::cout << "All command_dispatcher_test PASSED\n";
    return 0;
}
