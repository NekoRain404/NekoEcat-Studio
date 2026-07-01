#include "CoEService.h"
#include "EcatClient.h"
#include "SdoCacheService.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTimer>

// CoEService.cpp — CANopen over EtherCAT (CoE) SDO dictionary, segment I/O,
//                   and Emergency object handling
//
// Implementation notes:
//   - Wraps EcatClient for SDO info upload, dictionary browsing, segmented transfer
//   - Emergency methods (0x1001, 0x1003) use a synchronous QEventLoop wrapper
//     around the async EcatClient::upload() to return QJsonObject results
//   - Generic info, dictionary, and segmented transfer do not emit success
//     signals until daemon-backed CoE support exists

// ─── Synchronous SDO upload helper ───────────────────────────────────────────
// Blocks the calling (GUI) thread until the daemon responds or timeout fires.
// Returns {value, error} where error is empty on success.
namespace {

struct SdoResult {
    QString value;
    QString error;
};

SdoResult syncSdoUpload(EcatClient *client, int position,
                        const QString &index, const QString &subIndex,
                        int timeoutMs = 3000) {
    SdoResult result;

    if (!client->isConnected()) {
        result.error = "Daemon is not connected";
        return result;
    }

    QEventLoop loop;
    bool gotResponse = false;

    // Capture the sdoValue signal filtered to our specific request.
    QMetaObject::Connection valueConn = QObject::connect(
        client, &EcatClient::sdoValue,
        [&](int pos, const QString &idx, const QString &sub, const QString &val) {
            if (pos == position && idx == index && sub == subIndex) {
                result.value = val;
                gotResponse = true;
                loop.quit();
            }
        });

    // Safety timeout so we never block forever.
    // On timeout, report a clear error identifying which SDO timed out.
    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(&timeout, &QTimer::timeout, &loop, [&]() {
        if (!gotResponse) {
            result.error = QString("SDO upload timed out after %1ms (0x%2:%3 @ slave %4)")
                               .arg(timeoutMs)
                               .arg(index)
                               .arg(subIndex)
                               .arg(position);
            loop.quit();
        }
    });
    timeout.start(timeoutMs);

    client->upload(position, index, subIndex);
    loop.exec();

    QObject::disconnect(valueConn);
    timeout.stop();

    return result;
}

// Parse a string value from the daemon into a quint64.
// Handles both hex ("0x1F") and decimal ("31") formats.
quint64 parseValue(const QString &str, bool *ok = nullptr) {
    QString trimmed = str.trimmed();
    if (trimmed.startsWith("0x", Qt::CaseInsensitive)) {
        return trimmed.toULongLong(ok, 16);
    }
    return trimmed.toULongLong(ok, 10);
}

} // anonymous namespace

CoEService::CoEService(EcatClient *client, SdoCacheService *sdoCache, QObject *parent)
    : QObject(parent), client_(client), sdoCache_(sdoCache) {
    connect(client_, &EcatClient::errorMessage, this,
            [this](const QString &msg) { emit error(msg); });

    // Connect slaveTextResult to our parser; reorder args to match slot signature.
    connect(client_, &EcatClient::slaveTextResult, this,
            [this](const QString &title, int position, const QString &text) {
                onSdoText(position, title, text);
            });
}

void CoEService::uploadSdoInfo(int position) {
    Q_UNUSED(position);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot upload CoE SDO info: EtherCAT daemon is not connected");
        return;
    }
    emit error("CoE SDO info upload requires daemon backend support");
}

void CoEService::uploadDictionary(int position) {
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot upload CoE dictionary: EtherCAT daemon is not connected");
        return;
    }
    emit error(QString("Fetching SDO dictionary from slave %1...").arg(position));
    client_->sdos(position);
}

// ─── SDO text result parser (IgH ethercat sdos format) ─────────────────────
void CoEService::onSdoText(int position, const QString &title, const QString &text) {
    if (title != "SDO") {
        return;
    }

    // IgH format examples:
    //   0x1000:01  Device type        uint32  ro
    //   0x1000    Device type        uint32  ro
    //   0x1600:00  Receive PDO 1      uint8   ro
    static const QRegularExpression re(
        R"(^(0x[0-9a-fA-F]+)(?::(0x[0-9a-fA-F]+))?\s+(.+?)\s+(uint\d+|int\d+|octet_string|visible_string|BOOL|BIT\d+)\s+(ro|wo|rw)\s*$)");

    QVector<SdoDictionaryEntry> dict;
    QList<CoESdoDictionary> entries;

    const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QRegularExpressionMatch match = re.match(line);
        if (!match.hasMatch()) {
            continue;
        }

        const QString index    = match.captured(1);
        QString subIndex       = match.captured(2);
        const QString name     = match.captured(3).trimmed();
        const QString dataType = match.captured(4);
        const QString access   = match.captured(5);

        // Lines without a subindex specify the top-level index entry (subindex 0x00).
        if (subIndex.isEmpty()) {
            subIndex = QStringLiteral("0x00");
        }

        // Build cache entry.
        SdoDictionaryEntry cacheEntry;
        cacheEntry.index      = index;
        cacheEntry.subIndex   = subIndex;
        cacheEntry.name       = name;
        cacheEntry.dataType   = dataType;
        cacheEntry.accessType = access;
        dict.append(cacheEntry);

        // Build signal entry.
        CoESdoDictionary sdoEntry;
        sdoEntry.index      = index;
        sdoEntry.name       = name;
        sdoEntry.type       = dataType;
        sdoEntry.bitSize    = 0;  // IgH format doesn't convey bit size directly
        sdoEntry.accessType = (access == QStringLiteral("ro") ? 1 :
                               access == QStringLiteral("wo") ? 2 : 3);
        entries.append(sdoEntry);
    }

    if (dict.isEmpty()) {
        emit error(QString("Failed to parse SDO dictionary for slave %1: "
                           "no entries matched the IgH ethercat sdos format")
                       .arg(position));
        return;
    }

    sdoCache_->cacheDictionary(position, dict);
    emit dictionaryReceived(position, entries);
}

void CoEService::uploadSegment(int position, const QString &index,
                                int offset, int size) {
    Q_UNUSED(position);
    Q_UNUSED(index);
    Q_UNUSED(offset);
    Q_UNUSED(size);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot upload CoE segment: EtherCAT daemon is not connected");
        return;
    }
    emit error("CoE segmented upload requires daemon backend support");
}

void CoEService::downloadSegment(int position, const QString &index,
                                  int offset, const QByteArray &data) {
    Q_UNUSED(offset);
    Q_UNUSED(data);
    if (!client_ || !client_->isConnected()) {
        emit error("Cannot download CoE segment: EtherCAT daemon is not connected");
        emit segmentDownloaded(position, index, false);
        return;
    }
    emit error("CoE segmented download requires daemon backend support");
    emit segmentDownloaded(position, index, false);
}

// ─── Emergency object handling ───────────────────────────────────────────────

// Read Error Register (0x1001:00) — standard CANopen 8-bit error register.
// Each bit indicates a category of error. Returns decoded bit flags.
QJsonObject CoEService::readErrorRegister(int slavePosition) {
    QJsonObject response;

    SdoResult res = syncSdoUpload(client_, slavePosition, "0x1001", "0x00");
    if (!res.error.isEmpty()) {
        // Provide a specific hint if the object doesn't exist (common for
        // non-CoE slaves or slaves that don't implement 0x1001).
        if (res.error.contains("does not exist", Qt::CaseInsensitive) ||
            res.error.contains("0x06020000", Qt::CaseInsensitive) ||
            res.error.contains("not found", Qt::CaseInsensitive)) {
            response["success"] = false;
            response["error"] = QString(
                "Slave %1 does not support CoE Error Register (0x1001). "
                "The device may not be a CANopen-over-EtherCAT device.")
                .arg(slavePosition);
        } else {
            response["success"] = false;
            response["error"] = res.error;
        }
        return response;
    }

    bool ok = false;
    quint8 errorByte = static_cast<quint8>(parseValue(res.value, &ok));
    if (!ok) {
        response["success"] = false;
        response["error"] = QString("Failed to parse Error Register value: %1")
                                .arg(res.value);
        return response;
    }

    response["success"] = true;
    response["value"] = static_cast<int>(errorByte);
    response["valueHex"] = QString("0x%1").arg(errorByte, 2, 16, QChar('0'));

    // Decode individual bits per CANopen specification (DS301 / DS402).
    response["genericError"]          = static_cast<bool>(errorByte & 0x01);
    response["current"]               = static_cast<bool>(errorByte & 0x02);
    response["voltage"]               = static_cast<bool>(errorByte & 0x04);
    response["temperature"]           = static_cast<bool>(errorByte & 0x08);
    response["communicationError"]    = static_cast<bool>(errorByte & 0x10);
    response["deviceProfileSpecific"] = static_cast<bool>(errorByte & 0x20);
    response["manufacturerSpecific"]  = static_cast<bool>(errorByte & 0x40);

    return response;
}

// Read Pre-defined Error Field (0x1003) — CANopen error history.
// Subindex 0x00 holds the number of errors; 0x01..N hold error codes (uint32).
QJsonObject CoEService::readErrorHistory(int slavePosition, int maxEntries) {
    QJsonObject response;

    // Read subindex 0x00 to get the number of stored errors.
    SdoResult countRes = syncSdoUpload(client_, slavePosition, "0x1003", "0x00");
    if (!countRes.error.isEmpty()) {
        if (countRes.error.contains("does not exist", Qt::CaseInsensitive) ||
            countRes.error.contains("0x06020000", Qt::CaseInsensitive) ||
            countRes.error.contains("not found", Qt::CaseInsensitive)) {
            response["success"] = false;
            response["error"] = QString(
                "Slave %1 does not support Pre-defined Error Field (0x1003). "
                "The device may not be a CANopen-over-EtherCAT device.")
                .arg(slavePosition);
        } else {
            response["success"] = false;
            response["error"] = countRes.error;
        }
        return response;
    }

    bool ok = false;
    int errorCount = static_cast<int>(parseValue(countRes.value, &ok));
    if (!ok) {
        response["success"] = false;
        response["error"] = QString("Failed to parse error count: %1")
                                .arg(countRes.value);
        return response;
    }

    // Clamp to maxEntries to avoid reading hundreds of entries.
    int entriesToRead = qMin(errorCount, maxEntries);

    QJsonArray errors;
    for (int i = 1; i <= entriesToRead; ++i) {
        QString subIndex = QString("0x%1").arg(i, 2, 16, QChar('0'));
        SdoResult errRes = syncSdoUpload(client_, slavePosition, "0x1003", subIndex, 2000);
        if (!errRes.error.isEmpty()) {
            // If a specific subindex fails, record it and continue.
            QJsonObject entry;
            entry["subIndex"] = i;
            entry["error"] = errRes.error;
            errors.append(entry);
            continue;
        }

        bool parseOk = false;
        quint32 errorCode = static_cast<quint32>(parseValue(errRes.value, &parseOk));
        QJsonObject entry;
        entry["subIndex"] = i;
        if (parseOk) {
            entry["errorCode"] = static_cast<qint64>(errorCode);
            entry["errorCodeHex"] = QString("0x%1").arg(errorCode, 8, 16, QChar('0'));
        } else {
            entry["rawValue"] = errRes.value;
        }
        errors.append(entry);
    }

    response["success"] = true;
    response["errorCount"] = errorCount;
    response["entriesRead"] = entriesToRead;
    response["errors"] = errors;

    return response;
}

// Read all emergency info for a slave — combines Error Register and Error History.
QJsonObject CoEService::readEmergencyInfo(int slavePosition) {
    QJsonObject response;
    response["slavePosition"] = slavePosition;

    QJsonObject errorRegister = readErrorRegister(slavePosition);
    QJsonObject errorHistory  = readErrorHistory(slavePosition);

    bool regOk = errorRegister.value("success").toBool();
    bool histOk = errorHistory.value("success").toBool();

    response["errorRegister"] = errorRegister;
    response["errorHistory"]  = errorHistory;

    // Overall success only if both sub-reads succeeded.
    response["success"] = regOk && histOk;

    if (!regOk && !histOk) {
        response["error"] = QString(
            "Slave %1 does not appear to support CoE Emergency objects "
            "(0x1001 Error Register and 0x1003 Error Field are both unavailable).")
            .arg(slavePosition);
    } else if (!regOk) {
        response["error"] = errorRegister.value("error").toString();
    } else if (!histOk) {
        response["error"] = errorHistory.value("error").toString();
    }

    return response;
}
