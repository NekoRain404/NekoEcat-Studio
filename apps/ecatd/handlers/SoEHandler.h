#pragma once
// SoEHandler — Servo over EtherCAT (SoE) IDN read/write via IgH CLI.
//
// SoE (Servo Drive Profile over EtherCAT, IEC 61800-7-304) accesses servo
// drive parameters by IDN (Identification Number) rather than CoE SDO index.
// IDNs are either standard data (S-x-yyyy) or product data (P-x-yyyy).
//
// Wraps `ethercat soe_read` and `ethercat soe_write` commands.
//
// Thread safety:
//   Each handler invocation runs a blocking QProcess. Call from the daemon's
//   event loop thread only.

#include <QJsonObject>
#include <QString>

class SoEHandler {
public:
    // Read an SoE IDN from a slave.
    // params: { "master": string, "position": int, "drive": int (optional, 0-7),
    //           "idn": string (e.g. "P-0-0150" or numeric), "type": string (optional) }
    // Returns: { "value": string, "idn": string, "type": string }
    QJsonObject handleSoeRead(const QString& id, const QJsonObject& params);

    // Write an SoE IDN to a slave.
    // params: { "master": string, "position": int, "drive": int (optional),
    //           "idn": string, "value": string, "type": string }
    // Returns: { "success": true, "idn": string }
    QJsonObject handleSoeWrite(const QString& id, const QJsonObject& params);

    // Validate an IDN string (S-x-yyyy, P-x-yyyy, or numeric). Exposed for testing.
    bool validateIdn(const QString& idn, QString* error) const;

private:
    // Run an ethercat CLI command and capture stdout/stderr.
    int runEthercatCommand(const QStringList& args, QString* output, QString* errorOutput, int timeoutMs = 10000) const;
};
