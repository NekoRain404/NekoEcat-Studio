// Newline-delimited JSON framing protocol for GUI-daemon communication.
#include "JsonProtocol.h"

#include <QJsonDocument>

namespace JsonProtocol {

// Compact JSON + newline delimiter; the trailing '\n' is the frame boundary.
QByteArray encode(const QJsonObject& object) {
    return QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
}

// JSON-RPC-style request envelope: id correlates request to response.
QJsonObject request(const QString& id, const QString& method, const QJsonObject& params) {
    return {
        {"id", id},
        {"method", method},
        {"params", params},
    };
}

// Positive response envelope — "ok":true signals success to the client.
QJsonObject success(const QString& id, const QJsonObject& result) {
    return {
        {"id", id},
        {"ok", true},
        {"result", result},
    };
}

// Error response; code defaults to -1 for generic/uncategorized failures.
QJsonObject failure(const QString& id, const QString& message, int code) {
    return {
        {"id", id},
        {"ok", false},
        {"error", QJsonObject{{"code", code}, {"message", message}}},
    };
}

} // namespace JsonProtocol
