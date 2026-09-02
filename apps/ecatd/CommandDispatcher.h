#pragma once

// String-keyed command dispatch table for JSON-RPC-style request routing.
// Replaces monolithic if/else chains with O(1) lookup and per-command handler functions.

#include <QJsonObject>
#include <QString>

#include <functional>
#include <unordered_map>

class CommandDispatcher {
public:
    // Handler signature: receives (id, params) and returns a full JSON response object.
    using Handler = std::function<QJsonObject(const QString& id, const QJsonObject& params)>;

    CommandDispatcher() = default;

    // Register a handler for a named command. Overwrites any previous handler for the same name.
    void registerHandler(const QString& method, Handler handler);

    // Look up the handler for request["method"] and invoke it.
    // Returns an unknown-method error if no handler is registered.
    QJsonObject dispatch(const QJsonObject& request) const;

    // Convenience: build a success/failure response envelope.
    static QJsonObject success(const QString& id, const QJsonObject& result = {});
    static QJsonObject failure(const QString& id, const QString& message, int code = -1);

private:
    std::unordered_map<std::string, Handler> handlers_;
};
