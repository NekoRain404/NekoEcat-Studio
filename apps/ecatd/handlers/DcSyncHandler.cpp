// DcSyncHandler — DC sync status via CLI + optional ecrt enrichment.
#include "DcSyncHandler.h"

#include "../CommandDispatcher.h"
#include "EcatService.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>

#include <ecrt.h>

// ─── Construction ──────────────────────────────────────────────────────────

DcSyncHandler::DcSyncHandler(EcatService *backend)
    : backend_(backend)
{
}

// ─── JSON-RPC entry point ──────────────────────────────────────────────────

QJsonObject DcSyncHandler::handle(const QString &id, const QJsonObject &params)
{
    if (!backend_) {
        return CommandDispatcher::failure(id, "No CLI backend available for DC sync query.");
    }

    const QString master = params.value("master").toString("0").trimmed();
    QString error;
    const QString text = backend_->masterText(master, &error);

    if (!error.isEmpty()) {
        return CommandDispatcher::failure(id, error);
    }

    if (text.isEmpty()) {
        return CommandDispatcher::failure(id, "Empty response from ethercat master.");
    }

    rawMasterText_ = text;
    parseDcFromMasterText(text);

    return buildResponse(id);
}

// ─── Optional ecrt enrichment ──────────────────────────────────────────────

void DcSyncHandler::update(ec_master_t *master, int slaveCount)
{
    if (!master || slaveCount <= 0) {
        return;
    }

    slaveInfo_.clear();
    slaveInfo_.reserve(slaveCount);

    for (int i = 0; i < slaveCount; ++i) {
        DcSlaveInfo info;
        info.position = i;

        ec_slave_info_t si;
        if (ecrt_master_get_slave(master, static_cast<uint16_t>(i), &si) == 0) {
            // A slave is DC-capable if any port reports a non-zero delay to the next DC slave.
            bool hasDcPort = false;
            for (int p = 0; p < EC_MAX_PORTS; ++p) {
                if (si.ports[p].delay_to_next_dc > 0 || si.ports[p].receive_time > 0) {
                    hasDcPort = true;
                    break;
                }
            }
            info.dcCapable = hasDcPort;
            info.syncing = (si.al_state == 0x08); // OP state implies active sync
        }

        slaveInfo_.append(info);
    }

    // Query reference clock time.
    uint32_t refTime = 0;
    int rc = ecrt_master_reference_clock_time(master, &refTime);
    hasReferenceClock_ = (rc == 0);
    if (hasReferenceClock_) {
        refClockPosition_ = 0; // ecrt doesn't expose ref clock position directly
    }

    // Query sync monitor for jitter estimate.
    ecrt_master_sync_monitor_queue(master);
    uint32_t maxDiff = ecrt_master_sync_monitor_process(master);
    if (maxDiff != static_cast<uint32_t>(-1) && !slaveInfo_.isEmpty()) {
        // Distribute the max diff as a jitter estimate on the first slave.
        slaveInfo_[0].jitterNs = static_cast<int64_t>(maxDiff);
    }
}

// ─── Parse DC info from `ethercat master` text ─────────────────────────────

bool DcSyncHandler::parseDcFromMasterText(const QString &text)
{
    resetCache();

    // Patterns found in typical `ethercat master` output:
    //   "DC reference clock:    Slave 0"
    //   "Application time:      1234567890"
    //   "DC system time diff:   ..."
    //   "  DC: yes"  or  "  Distributed clocks: yes"
    // Exact wording varies by IgH version; we try several patterns.

    static const QRegularExpression refClockRe(
        R"(DC\s+reference\s+clock:\s*(?:Slave\s+)?(\d+))",
        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression appTimeRe(
        R"(Application\s+time:\s*(.+))",
        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression sysDiffRe(
        R"(system\s+time\s+diff(?:erence)?:\s*(.+))",
        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression dcAvailRe(
        R"(D(?:istributed\s+)?C(?:locks)?:\s*(yes|no|true|false|\d+))",
        QRegularExpression::CaseInsensitiveOption);

    const auto refMatch = refClockRe.match(text);
    if (refMatch.hasMatch()) {
        refClockPosition_ = refMatch.captured(1).toInt();
        hasReferenceClock_ = true;
    }

    const auto appMatch = appTimeRe.match(text);
    if (appMatch.hasMatch()) {
        applicationTime_ = appMatch.captured(1).trimmed();
    }

    const auto diffMatch = sysDiffRe.match(text);
    if (diffMatch.hasMatch()) {
        systemTimeDiff_ = diffMatch.captured(1).trimmed();
    }

    const auto dcMatch = dcAvailRe.match(text);
    if (dcMatch.hasMatch()) {
        const QString val = dcMatch.captured(1).toLower();
        dcAvailable_ = (val == "yes" || val == "true" || val == "1");
    } else {
        // If we found a reference clock, DC is at least partially available.
        dcAvailable_ = hasReferenceClock_;
    }

    return dcAvailable_;
}

// ─── Build the JSON response ───────────────────────────────────────────────

QJsonObject DcSyncHandler::buildResponse(const QString &id) const
{
    QJsonObject result;
    result["dcAvailable"] = dcAvailable_;
    result["hasRefClock"] = hasReferenceClock_;
    result["refClock"] = refClockPosition_;

    if (!applicationTime_.isEmpty()) {
        result["applicationTime"] = applicationTime_;
    }
    if (!systemTimeDiff_.isEmpty()) {
        result["systemTimeDiff"] = systemTimeDiff_;
    }

    QJsonArray slaves;
    for (const auto &info : slaveInfo_) {
        QJsonObject obj;
        obj["position"] = info.position;
        obj["dcCapable"] = info.dcCapable;
        obj["syncing"] = info.syncing;
        obj["driftNs"] = static_cast<qint64>(info.driftNs);
        obj["jitterNs"] = static_cast<qint64>(info.jitterNs);
        slaves.append(obj);
    }
    result["slaves"] = slaves;

    // Include raw CLI output for debugging / forward compatibility.
    if (!rawMasterText_.isEmpty()) {
        result["raw"] = rawMasterText_;
    }

    return CommandDispatcher::success(id, result);
}

// ─── Reset cached state ────────────────────────────────────────────────────

void DcSyncHandler::resetCache()
{
    dcAvailable_ = false;
    refClockPosition_ = -1;
    hasReferenceClock_ = false;
    applicationTime_.clear();
    systemTimeDiff_.clear();
    slaveInfo_.clear();
}
