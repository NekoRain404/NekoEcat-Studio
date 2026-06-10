#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QString>

namespace JsonProtocol {
QByteArray encode(const QJsonObject &object);
QJsonObject request(const QString &id, const QString &method, const QJsonObject &params = {});
QJsonObject success(const QString &id, const QJsonObject &result = {});
QJsonObject failure(const QString &id, const QString &message, int code = -1);
}

