// EoEHandler — Ethernet over EtherCAT (EoE) protocol operations.
//
// Provides EoE status queries, IP configuration via SDO, and statistics
// monitoring for EtherCAT slaves that support the EoE mailbox protocol.
//
// EoE IP configuration uses standard SDO objects:
//   0x8000:01 — IP address (uint32, network byte order)
//   0x8000:02 — Subnet mask (uint32)
//   0x8000:03 — Default gateway (uint32)
//   0x8000:04 — DNS server (uint32)
//
// The exact indices may vary by slave manufacturer; these are the EtherCAT
// standard EoE IP parameter offsets. The handler also checks slave ESI/XML
// for EoE mailbox protocol declaration.

#include "EoEHandler.h"

#include "CommandDispatcher.h"
#include "EcatService.h"

#include <QProcess>
#include <QRegularExpression>

// ─── Construction ──────────────────────────────────────────────────────────

EoEHandler::EoEHandler(EcatService* backend) : backend_(backend) {}

// ─── EoE Status Query ─────────────────────────────────────────────────────

QJsonObject EoEHandler::handleEoeStatus(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();
    const int position = params.value("position").toInt(-1);

    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }

    if (!backend_) {
        return CommandDispatcher::failure(id, "No backend available.");
    }

    // Check if slave supports EoE by examining ESI/XML for mailbox protocols.
    QString error;
    const bool hasEoe = slaveHasEoe(master, position, &error);
    if (!error.isEmpty() && !hasEoe) {
        return CommandDispatcher::failure(id, error);
    }

    // Try to read EoE-specific SDOs to determine IP configuration support.
    bool hasIpConfig = false;
    uint32_t ipValue = 0;
    QString ipError;
    if (hasEoe) {
        hasIpConfig = readEoeSdo(master, position, "0x8000", "0x01", &ipValue, &ipError);
    }

    QJsonObject result;
    result["supported"] = hasEoe;
    result["hasIpConfig"] = hasIpConfig;
    if (hasIpConfig) {
        uint8_t ipBytes[4];
        ipBytes[0] = static_cast<uint8_t>(ipValue & 0xFF);
        ipBytes[1] = static_cast<uint8_t>((ipValue >> 8) & 0xFF);
        ipBytes[2] = static_cast<uint8_t>((ipValue >> 16) & 0xFF);
        ipBytes[3] = static_cast<uint8_t>((ipValue >> 24) & 0xFF);
        result["currentIp"] = formatIp(ipBytes);
    }
    result["position"] = position;
    return CommandDispatcher::success(id, result);
}

// ─── IP Configuration via SDO ─────────────────────────────────────────────

QJsonObject EoEHandler::handleEoeConfigureIp(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();
    const int position = params.value("position").toInt(-1);
    const QString ip = params.value("ip").toString().trimmed();
    const QString subnet = params.value("subnet").toString().trimmed();
    const QString gateway = params.value("gateway").toString().trimmed();
    const QString dns = params.value("dns").toString().trimmed();

    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }
    if (ip.isEmpty() || subnet.isEmpty()) {
        return CommandDispatcher::failure(id, "Missing 'ip' or 'subnet' parameter.");
    }

    // Validate IP addresses.
    uint8_t ipBytes[4], subnetBytes[4];
    if (!parseIp(ip, ipBytes)) {
        return CommandDispatcher::failure(id, QString("Invalid IP address: '%1'.").arg(ip));
    }
    if (!parseIp(subnet, subnetBytes)) {
        return CommandDispatcher::failure(id, QString("Invalid subnet mask: '%1'.").arg(subnet));
    }

    // Encode IP as uint32 (little-endian, matching EtherCAT convention).
    const uint32_t ipValue = static_cast<uint32_t>(ipBytes[0]) | (static_cast<uint32_t>(ipBytes[1]) << 8) |
                             (static_cast<uint32_t>(ipBytes[2]) << 16) | (static_cast<uint32_t>(ipBytes[3]) << 24);
    const uint32_t subnetValue = static_cast<uint32_t>(subnetBytes[0]) | (static_cast<uint32_t>(subnetBytes[1]) << 8) |
                                 (static_cast<uint32_t>(subnetBytes[2]) << 16) |
                                 (static_cast<uint32_t>(subnetBytes[3]) << 24);

    // Write IP address SDO.
    QString error;
    if (!writeEoeSdo(master, position, "0x8000", "0x01", ipValue, &error)) {
        return CommandDispatcher::failure(id, QString("Failed to set IP: %1").arg(error));
    }

    // Write subnet mask SDO.
    if (!writeEoeSdo(master, position, "0x8000", "0x02", subnetValue, &error)) {
        return CommandDispatcher::failure(id, QString("Failed to set subnet: %1").arg(error));
    }

    // Write gateway if provided.
    if (!gateway.isEmpty()) {
        uint8_t gwBytes[4];
        if (parseIp(gateway, gwBytes)) {
            const uint32_t gwValue = static_cast<uint32_t>(gwBytes[0]) | (static_cast<uint32_t>(gwBytes[1]) << 8) |
                                     (static_cast<uint32_t>(gwBytes[2]) << 16) |
                                     (static_cast<uint32_t>(gwBytes[3]) << 24);
            writeEoeSdo(master, position, "0x8000", "0x03", gwValue, &error);
        }
    }

    // Write DNS if provided.
    if (!dns.isEmpty()) {
        uint8_t dnsBytes[4];
        if (parseIp(dns, dnsBytes)) {
            const uint32_t dnsValue = static_cast<uint32_t>(dnsBytes[0]) | (static_cast<uint32_t>(dnsBytes[1]) << 8) |
                                      (static_cast<uint32_t>(dnsBytes[2]) << 16) |
                                      (static_cast<uint32_t>(dnsBytes[3]) << 24);
            writeEoeSdo(master, position, "0x8000", "0x04", dnsValue, &error);
        }
    }

    QJsonObject result;
    result["success"] = true;
    result["ip"] = ip;
    result["subnet"] = subnet;
    if (!gateway.isEmpty())
        result["gateway"] = gateway;
    if (!dns.isEmpty())
        result["dns"] = dns;
    result["message"] = QString("EoE IP configured for slave %1: %2/%3").arg(position).arg(ip).arg(subnet);
    return CommandDispatcher::success(id, result);
}

// ─── Read Current IP Configuration ────────────────────────────────────────

QJsonObject EoEHandler::handleEoeGetIp(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();
    const int position = params.value("position").toInt(-1);

    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }

    QString error;
    uint32_t ipValue = 0, subnetValue = 0, gwValue = 0, dnsValue = 0;

    bool ok = readEoeSdo(master, position, "0x8000", "0x01", &ipValue, &error);
    if (!ok) {
        return CommandDispatcher::failure(id, QString("Failed to read IP: %1").arg(error));
    }
    readEoeSdo(master, position, "0x8000", "0x02", &subnetValue, &error);
    readEoeSdo(master, position, "0x8000", "0x03", &gwValue, &error);
    readEoeSdo(master, position, "0x8000", "0x04", &dnsValue, &error);

    auto toIpString = [](uint32_t val) -> QString {
        uint8_t bytes[4] = {static_cast<uint8_t>(val & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                            static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 24) & 0xFF)};
        return EoEHandler::formatIp(bytes);
    };

    QJsonObject result;
    result["ip"] = toIpString(ipValue);
    result["subnet"] = toIpString(subnetValue);
    result["gateway"] = toIpString(gwValue);
    result["dns"] = toIpString(dnsValue);
    result["position"] = position;
    return CommandDispatcher::success(id, result);
}

// ─── EoE Statistics ───────────────────────────────────────────────────────

QJsonObject EoEHandler::handleEoeStats(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();
    const int position = params.value("position").toInt(-1);

    if (position < 0) {
        return CommandDispatcher::failure(id, "Missing or invalid 'position' parameter.");
    }

    // EoE statistics are typically at 0x8000:10-0x8000:13 or vendor-specific.
    // Try reading standard locations; return zeros if not supported.
    QString error;
    uint32_t txFrames = 0, rxFrames = 0, txErrors = 0, rxErrors = 0;

    readEoeSdo(master, position, "0x8000", "0x10", &txFrames, &error);
    readEoeSdo(master, position, "0x8000", "0x11", &rxFrames, &error);
    readEoeSdo(master, position, "0x8000", "0x12", &txErrors, &error);
    readEoeSdo(master, position, "0x8000", "0x13", &rxErrors, &error);

    QJsonObject result;
    result["txFrames"] = static_cast<qint64>(txFrames);
    result["rxFrames"] = static_cast<qint64>(rxFrames);
    result["txErrors"] = static_cast<qint64>(txErrors);
    result["rxErrors"] = static_cast<qint64>(rxErrors);
    result["position"] = position;
    return CommandDispatcher::success(id, result);
}

// ─── IP Parsing Helpers ───────────────────────────────────────────────────

bool EoEHandler::parseIp(const QString& ip, uint8_t out[4]) {
    static QRegularExpression re(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");
    const auto match = re.match(ip.trimmed());
    if (!match.hasMatch())
        return false;

    for (int i = 0; i < 4; ++i) {
        bool ok = false;
        const int val = match.captured(i + 1).toInt(&ok);
        if (!ok || val < 0 || val > 255)
            return false;
        out[i] = static_cast<uint8_t>(val);
    }
    return true;
}

QString EoEHandler::formatIp(const uint8_t data[4]) {
    return QString("%1.%2.%3.%4").arg(data[0]).arg(data[1]).arg(data[2]).arg(data[3]);
}

// ─── SDO Read/Write Helpers ───────────────────────────────────────────────

bool EoEHandler::readEoeSdo(const QString& master, int position, const QString& index, const QString& subIndex,
                            uint32_t* value, QString* error) const {
    if (!backend_) {
        if (error)
            *error = "No backend available";
        return false;
    }

    const QString result = backend_->upload(master, position, index, subIndex, "uint32", error);
    if (error && !error->isEmpty())
        return false;

    bool ok = false;
    const uint val = result.toUInt(&ok);
    if (!ok) {
        if (error)
            *error = QString("Failed to parse SDO value: '%1'").arg(result);
        return false;
    }
    if (value)
        *value = static_cast<uint32_t>(val);
    return true;
}

bool EoEHandler::writeEoeSdo(const QString& master, int position, const QString& index, const QString& subIndex,
                             uint32_t value, QString* error) const {
    if (!backend_) {
        if (error)
            *error = "No backend available";
        return false;
    }

    return backend_->download(master, position, index, subIndex, QString::number(value), "uint32", error);
}

// ─── EoE Support Detection ────────────────────────────────────────────────

bool EoEHandler::slaveHasEoe(const QString& master, int position, QString* error) const {
    if (!backend_) {
        if (error)
            *error = "No backend available";
        return false;
    }

    // Check slave ESI/XML for EoE mailbox protocol support.
    const QString xml = backend_->slaveXml(master, position, error);
    if (error && !error->isEmpty())
        return false;

    // Look for EoE protocol declaration in the XML.
    // Standard pattern: <Mailbox> ... <EoE /> ... </Mailbox>
    return xml.contains("EoE", Qt::CaseInsensitive) || xml.contains("Ethernet over EtherCAT", Qt::CaseInsensitive);
}
