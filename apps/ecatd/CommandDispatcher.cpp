// Command dispatch table implementation.
#include "CommandDispatcher.h"

void CommandDispatcher::registerHandler(const QString &method, Handler handler) {
    handlers_[method.toStdString()] = std::move(handler);
}

QJsonObject CommandDispatcher::dispatch(const QJsonObject &request) const {
    const QString id = request.value("id").toString();
    const QString method = request.value("method").toString();
    const QJsonObject params = request.value("params").toObject();

    if (method.isEmpty()) {
        return failure(id, "Missing method name");
    }

    auto it = handlers_.find(method.toStdString());
    if (it == handlers_.end()) {
        return failure(id, QString("Unknown method: %1").arg(method));
    }
    return it->second(id, params);
}

QJsonObject CommandDispatcher::success(const QString &id, const QJsonObject &result) {
    return {{"id", id}, {"ok", true}, {"result", result}};
}

QJsonObject CommandDispatcher::failure(const QString &id, const QString &message, int code) {
    QJsonObject err;
    err["message"] = message;
    err["code"] = code;
    return {
        {"id", id},
        {"ok", false},
        {"error", err}
    };
}
