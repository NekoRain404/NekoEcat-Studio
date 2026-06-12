#pragma once

// Newline-delimited JSON framing protocol for GUI-daemon communication.


#include <QByteArray>
#include <QJsonObject>
#include <QString>

namespace JsonProtocol {
// Wire convention: every message is one compact JSON object followed by '\n'.
// The daemon and client split on newlines to frame individual messages.
QByteArray encode(const QJsonObject &object);
QJsonObject request(const QString &id, const QString &method, const QJsonObject &params = {});
QJsonObject success(const QString &id, const QJsonObject &result = {});
QJsonObject failure(const QString &id, const QString &message, int code = -1);
}
