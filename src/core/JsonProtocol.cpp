#include "JsonProtocol.h"

#include <QJsonDocument>

namespace JsonProtocol {

QByteArray encode(const QJsonObject &object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
}

QJsonObject request(const QString &id, const QString &method, const QJsonObject &params)
{
    return {
        {"id", id},
        {"method", method},
        {"params", params},
    };
}

QJsonObject success(const QString &id, const QJsonObject &result)
{
    return {
        {"id", id},
        {"ok", true},
        {"result", result},
    };
}

QJsonObject failure(const QString &id, const QString &message, int code)
{
    return {
        {"id", id},
        {"ok", false},
        {"error", QJsonObject{{"code", code}, {"message", message}}},
    };
}

}

