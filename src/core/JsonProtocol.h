#pragma once

// Newline-delimited JSON framing protocol for GUI-daemon communication.
//
// Wire format: each message is one compact JSON object terminated by '\n'.
// Both the daemon (ecatd) and the client (EcatClient) split on newlines to
// frame individual messages. Messages follow a JSON-RPC-style convention:
//   - Requests:  { "id": "1", "method": "scan", "params": {} }
//   - Success:   { "id": "1", "result": { ... } }
//   - Failure:   { "id": "1", "error": { "code": -1, "message": "..." } }

#include <QByteArray>
#include <QJsonObject>
#include <QString>

namespace JsonProtocol {

/// @brief Encode a JSON object to a newline-terminated byte array.
/// @param object The JSON object to encode.
/// @return Compact JSON bytes followed by '\n'.
QByteArray encode(const QJsonObject &object);

/// @brief Build a JSON-RPC-style request envelope.
/// @param id Unique request identifier for response correlation.
/// @param method The command name (e.g. "scan", "upload", "setState").
/// @param params Optional parameters object.
/// @return Request JSON object ready to encode and send.
QJsonObject request(const QString &id, const QString &method, const QJsonObject &params = {});

/// @brief Build a success response envelope.
/// @param id The request ID this response correlates to.
/// @param result Optional result payload.
/// @return Success response JSON object.
QJsonObject success(const QString &id, const QJsonObject &result = {});

/// @brief Build a failure response envelope.
/// @param id The request ID this response correlates to.
/// @param error_message Human-readable error description.
/// @param code Optional error code (default -1).
/// @return Failure response JSON object with error.code and error.message.
QJsonObject failure(const QString &id, const QString &error_message, int code = -1);

}
