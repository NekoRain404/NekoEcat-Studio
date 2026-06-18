// DcSyncHandler — DC sync status via CLI + optional ecrt enrichment.
#include "DcSyncHandler.h"

#include "CommandDispatcher.h"
#include "EcatService.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>

#include <ecrt.h>

// ─── Construction ──────────────────────────────────────────────────────────

DcSyncHandler::DcSyncHandler(EcatService *backend)
    : backend_(backend)
{
}

void DcSyncHandler::setBackend(EcatService *backend)
{
    backend_ = backend;
}

// ─── CLI helper ────────────────────────────────────────────────────────────

QString DcSyncHandler::runCliCommand(const QString &master,
                                     const QStringList &args) const
{
    // Prefer EcatService backend when available (consistent with other handlers).
    if (backend_ && args.size() == 1 && args.first() == "master") {
        return backend_->masterText(master);
    }

    // Fallback: direct CLI invocation for commands the backend doesn't cover.
    QProcess proc;
    QStringList fullArgs;
    const QString trimmed = master.trimmed();
    if (!trimmed.isEmpty()) {
        fullArgs << "-m" << trimmed;
    }
    fullArgs << args;
    proc.setProgram("ethercat");
    proc.setArguments(fullArgs);
    proc.start();
    proc.waitForFinished(5000);
    return QString::fromUtf8(proc.readAllStandardOutput());
}

// ─── JSON-RPC entry point ──────────────────────────────────────────────────

QJsonObject DcSyncHandler::handle(const QString &id, const QJsonObject &params)
{
    const QString master = params.value("master").toString("0").trimmed();

    const QString slaveOutput = runCliCommand(
        master, {"slaves", "-v"});
    const QString masterOutput = runCliCommand(
        master, {"master"});

    if (slaveOutput.isEmpty() && masterOutput.isEmpty()) {
        return CommandDispatcher::failure(id,
            "Failed to query DC status. Is the EtherCAT master running?");
    }

    const int refClock = detectRefClock(masterOutput);
    const auto slaves = queryDcStatus(slaveOutput);

    QJsonArray slaveArr;
    for (const auto &info : slaves) {
        slaveArr.append(slaveInfoToJson(info));
    }

    QJsonObject result;
    result["refClock"] = refClock;
    result["hasRefClock"] = (refClock >= 0);
    result["slaves"] = slaveArr;

    return CommandDispatcher::success(id, result);
}

// ─── Optional ecrt enrichment ──────────────────────────────────────────────

void DcSyncHandler::update(ec_master_t *master, int slaveCount)
{
    if (!master || slaveCount <= 0) {
        return;
    }

    // Query reference clock time for sync quality assessment.
    uint32_t refTime = 0;
    ecrt_master_reference_clock_time(master, &refTime);

    // Queue and process the sync monitor datagram for jitter measurement.
    ecrt_master_sync_monitor_queue(master);
    uint32_t maxDiff = ecrt_master_sync_monitor_process(master);
    (void)refTime;
    (void)maxDiff;
}

// ─── Parse DC info from `ethercat slaves -v` output ────────────────────────

QVector<DcSyncSlaveInfo>
DcSyncHandler::queryDcStatus(const QString &slaveVerboseOutput) const
{
    QVector<DcSyncSlaveInfo> result;
    if (slaveVerboseOutput.isEmpty())
        return result;

    const QStringList lines = slaveVerboseOutput.split('\n', Qt::SkipEmptyParts);

    DcSyncSlaveInfo current;
    bool inDcBlock = false;

    // Long-form header: "Alias 0x0000, Position 0x0000, ..."
    static const QRegularExpression headerRe(
        QStringLiteral("Alias\\s+0x[0-9a-fA-F]+,\\s+Position\\s+0x([0-9a-fA-F]+)"));

    // Short-form header: "0  0:0   OP  +  EL1008"
    static const QRegularExpression shortHeaderRe(
        QStringLiteral("^(\\d+)\\s+\\d+:\\d+\\s+"));

    // DC capability marker.
    static const QRegularExpression dcCapableRe(
        QStringLiteral("Distributed Clocks:"));

    // Jitter line.
    static const QRegularExpression jitterRe(
        QStringLiteral("Jitter:\\s+(-?\\d+)\\s+ns"));

    // Drift / offset line.
    static const QRegularExpression driftRe(
        QStringLiteral("(?:Drift|Offset):\\s+(-?\\d+)\\s+ns"));

    // Reference clock designation.
    static const QRegularExpression refClkRe(
        QStringLiteral("Reference Clock"));

    // System time line (non-zero implies syncing).
    static const QRegularExpression sysTimeRe(
        QStringLiteral("System Time:\\s+(-?\\d+)\\s+ns"));

    // Slave name from short-form lines: "+  EL1008"
    static const QRegularExpression nameRe(
        QStringLiteral("\\+\\s+(\\S+)$"));

    for (const QString &line : lines) {
        // Detect new slave block via the long-form header.
        auto headerMatch = headerRe.match(line);
        if (headerMatch.hasMatch()) {
            if (current.position >= 0)
                result.append(current);
            current = DcSyncSlaveInfo();
            current.position = headerMatch.captured(1).toInt(nullptr, 16);
            inDcBlock = false;
            continue;
        }

        // Detect new slave block via the short-form header.
        auto shortMatch = shortHeaderRe.match(line);
        if (shortMatch.hasMatch()) {
            if (current.position >= 0)
                result.append(current);
            current = DcSyncSlaveInfo();
            current.position = shortMatch.captured(1).toInt();
            auto nameMatch = nameRe.match(line);
            if (nameMatch.hasMatch())
                current.refClockName = nameMatch.captured(1);
            inDcBlock = false;
            continue;
        }

        // DC capability marker starts a DC detail block.
        if (dcCapableRe.match(line).hasMatch()) {
            current.dcCapable = true;
            inDcBlock = true;
            continue;
        }

        // Within a DC block, parse timing and sync lines.
        if (inDcBlock) {
            auto jitterMatch = jitterRe.match(line);
            if (jitterMatch.hasMatch()) {
                const int64_t val = jitterMatch.captured(1).toLongLong();
                if (current.jitterMinNs == 0 && current.jitterMaxNs == 0) {
                    current.jitterMinNs = val;
                    current.jitterMaxNs = val;
                } else {
                    current.jitterMinNs = qMin(current.jitterMinNs, val);
                    current.jitterMaxNs = qMax(current.jitterMaxNs, val);
                }
                current.jitterAvgNs = (current.jitterMinNs + current.jitterMaxNs) / 2;
            }

            auto driftMatch = driftRe.match(line);
            if (driftMatch.hasMatch())
                current.driftNs = driftMatch.captured(1).toLongLong();

            if (refClkRe.match(line).hasMatch())
                current.syncing = true;

            auto sysTimeMatch = sysTimeRe.match(line);
            if (sysTimeMatch.hasMatch() && sysTimeMatch.captured(1).toLongLong() != 0)
                current.syncing = true;
        }
    }

    // Append the final block.
    if (current.position >= 0)
        result.append(current);

    return result;
}

// ─── Detect reference clock from `ethercat master` output ──────────────────

int DcSyncHandler::detectRefClock(const QString &masterOutput) const
{
    if (masterOutput.isEmpty())
        return -1;

    // Pattern 1: "DC reference clock: Slave N"
    static const QRegularExpression dcRefRe(
        QStringLiteral("DC reference clock:\\s*Slave\\s+(\\d+)"));

    // Pattern 2: "Slave N: ... reference clock" (inline designation)
    static const QRegularExpression slaveRefRe(
        QStringLiteral("Slave\\s+(\\d+)\\b.*reference clock"));

    auto m1 = dcRefRe.match(masterOutput);
    if (m1.hasMatch())
        return m1.captured(1).toInt();

    auto m2 = slaveRefRe.match(masterOutput);
    if (m2.hasMatch())
        return m2.captured(1).toInt();

    return -1;
}

// ─── Convert slave info to JSON ────────────────────────────────────────────

QJsonObject DcSyncHandler::slaveInfoToJson(const DcSyncSlaveInfo &info) const
{
    QJsonObject obj;
    obj["pos"] = info.position;
    obj["dcCapable"] = info.dcCapable;
    obj["syncing"] = info.syncing;
    obj["driftNs"] = static_cast<qint64>(info.driftNs);
    obj["jitterMinNs"] = static_cast<qint64>(info.jitterMinNs);
    obj["jitterMaxNs"] = static_cast<qint64>(info.jitterMaxNs);
    obj["jitterAvgNs"] = static_cast<qint64>(info.jitterAvgNs);
    if (!info.refClockName.isEmpty()) {
        obj["refClockName"] = info.refClockName;
    }
    return obj;
}
