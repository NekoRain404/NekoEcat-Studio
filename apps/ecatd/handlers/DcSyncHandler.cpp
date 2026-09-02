// DcSyncHandler — DC sync status via CLI + optional ecrt enrichment.
#include "DcSyncHandler.h"

#include "CommandDispatcher.h"
#include "EcatService.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QXmlStreamReader>

#include <ecrt.h>

// ─── Construction ──────────────────────────────────────────────────────────

DcSyncHandler::DcSyncHandler(EcatService* backend) : backend_(backend) {}

void DcSyncHandler::setBackend(EcatService* backend) {
    backend_ = backend;
}

// ─── CLI helper ────────────────────────────────────────────────────────────

QString DcSyncHandler::runCliCommand(const QString& master, const QStringList& args) const {
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
    if (!proc.waitForFinished(5000)) {
        proc.kill();
        proc.waitForFinished(1000);
    }
    return QString::fromUtf8(proc.readAllStandardOutput());
}

// ─── JSON-RPC entry point ──────────────────────────────────────────────────

QJsonObject DcSyncHandler::handle(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();

    const QString slaveOutput = runCliCommand(master, {"slaves", "-v"});
    const QString masterOutput = runCliCommand(master, {"master"});

    if (slaveOutput.isEmpty() && masterOutput.isEmpty()) {
        return CommandDispatcher::failure(id, "Failed to query DC status. Is the EtherCAT master running?");
    }

    const int refClock = detectRefClock(masterOutput);
    const auto slaves = queryDcStatus(slaveOutput);

    QJsonArray slaveArr;
    for (const auto& info : slaves) {
        slaveArr.append(slaveInfoToJson(info));
    }

    QJsonObject result;
    result["refClock"] = refClock;
    result["hasRefClock"] = (refClock >= 0);
    result["slaves"] = slaveArr;

    // Include real-time DC sync data from ecrt API if available.
    {
        std::lock_guard<std::mutex> lock(rtMutex_);
        result["refClockTime"] = static_cast<qint64>(refClockTime_);
        result["syncMaxDiff"] = static_cast<qint64>(syncMaxDiff_);
        result["rtDataValid"] = rtDataValid_;
        result["dcActivated"] = dcActivated_;
        result["dcRefClockSlave"] = dcRefClockSlave_;
        result["assignActivate"] = static_cast<qint64>(activeDcConfig_.assignActivate);
        result["sync0CycleNs"] = static_cast<qint64>(activeDcConfig_.sync0CycleNs);
    }

    return CommandDispatcher::success(id, result);
}

// ─── Optional ecrt enrichment ──────────────────────────────────────────────

void DcSyncHandler::update(ec_master_t* master, int slaveCount) {
    if (!master || slaveCount <= 0) {
        return;
    }

    // If DC activation was requested, apply the reference clock selection now
    // that we have a live master handle.  ecrt_master_select_reference_clock()
    // accepts an ec_slave_config_t* (nullptr = auto-select), not a position.
    // Since we don't have the slave config pointer in this handler context,
    // we use nullptr to let the master auto-select.  The dcRefClockSlave_
    // position is stored for diagnostic purposes only.
    {
        std::lock_guard<std::mutex> lock(rtMutex_);
        if (dcActivated_) {
            ecrt_master_select_reference_clock(master, nullptr);
        }
    }

    // Query reference clock time for sync quality assessment.
    uint32_t refTime = 0;
    ecrt_master_reference_clock_time(master, &refTime);

    // Queue and process the sync monitor datagram for jitter measurement.
    ecrt_master_sync_monitor_queue(master);
    uint32_t maxDiff = ecrt_master_sync_monitor_process(master);

    // Store real-time data under lock for handle() to read.
    {
        std::lock_guard<std::mutex> lock(rtMutex_);
        refClockTime_ = refTime;
        syncMaxDiff_ = maxDiff;
        rtDataValid_ = true;
    }
}

// ─── Parse DC info from `ethercat slaves -v` output ────────────────────────

QVector<DcSyncSlaveInfo> DcSyncHandler::queryDcStatus(const QString& slaveVerboseOutput) const {
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
    static const QRegularExpression shortHeaderRe(QStringLiteral("^(\\d+)\\s+\\d+:\\d+\\s+"));

    // DC capability marker.
    static const QRegularExpression dcCapableRe(QStringLiteral("Distributed Clocks:"));

    // Jitter line.
    static const QRegularExpression jitterRe(QStringLiteral("Jitter:\\s+(-?\\d+)\\s+ns"));

    // Drift / offset line.
    static const QRegularExpression driftRe(QStringLiteral("(?:Drift|Offset):\\s+(-?\\d+)\\s+ns"));

    // Reference clock designation.
    static const QRegularExpression refClkRe(QStringLiteral("Reference Clock"));

    // System time line (non-zero implies syncing).
    static const QRegularExpression sysTimeRe(QStringLiteral("System Time:\\s+(-?\\d+)\\s+ns"));

    // Slave name from short-form lines: "+  EL1008"
    static const QRegularExpression nameRe(QStringLiteral("\\+\\s+(\\S+)$"));

    for (const QString& line : lines) {
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

int DcSyncHandler::detectRefClock(const QString& masterOutput) const {
    if (masterOutput.isEmpty())
        return -1;

    // Pattern 1: "DC reference clock: Slave N"
    static const QRegularExpression dcRefRe(QStringLiteral("DC reference clock:\\s*Slave\\s+(\\d+)"));

    // Pattern 2: "Slave N: ... reference clock" (inline designation)
    static const QRegularExpression slaveRefRe(QStringLiteral("Slave\\s+(\\d+)\\b.*reference clock"));

    auto m1 = dcRefRe.match(masterOutput);
    if (m1.hasMatch())
        return m1.captured(1).toInt();

    auto m2 = slaveRefRe.match(masterOutput);
    if (m2.hasMatch())
        return m2.captured(1).toInt();

    return -1;
}

// ─── Convert slave info to JSON ────────────────────────────────────────────

QJsonObject DcSyncHandler::slaveInfoToJson(const DcSyncSlaveInfo& info) const {
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

// ─── Convert DC config to JSON ──────────────────────────────────────────────

QJsonObject DcSyncHandler::dcConfigToJson(const DcConfig& config) const {
    QJsonObject obj;
    obj["assignActivate"] = static_cast<qint64>(config.assignActivate);
    obj["sync0CycleNs"] = static_cast<qint64>(config.sync0CycleNs);
    obj["sync0ShiftNs"] = static_cast<qint64>(config.sync0ShiftNs);
    obj["sync1CycleNs"] = static_cast<qint64>(config.sync1CycleNs);
    obj["sync1ShiftNs"] = static_cast<qint64>(config.sync1ShiftNs);
    obj["hasDcConfig"] = (config.assignActivate != 0);
    return obj;
}

// ─── Parse DC configuration from ESI XML ────────────────────────────────────

DcConfig DcSyncHandler::parseDcConfigFromXml(const QString& xmlText) const {
    DcConfig config;

    // Walk the XML looking for the <Dc> element with assign-activate and
    // <DcCycle> with Sync0/Sync1 cycle and shift times.
    //
    // Typical ESI structure:
    //   <Dc>
    //     <OpMode>
    //       <AssignActivate>#x0300</AssignActivate>
    //       <Sync0Cycle>1000000</Sync0Cycle>
    //       <Sync0Shift>0</Sync0Shift>
    //       <Sync1Cycle>0</Sync1Cycle>
    //       <Sync1Shift>0</Sync1Shift>
    //     </OpMode>
    //   </Dc>

    QXmlStreamReader xml(xmlText);
    bool inDc = false;
    bool inOpMode = false;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            const QString name = xml.name().toString();

            if (name == QLatin1String("Dc")) {
                inDc = true;
            } else if (inDc && (name == QLatin1String("OpMode") || name == QLatin1String("OpModeData"))) {
                inOpMode = true;
            } else if (inOpMode) {
                // The text content will be read by readElementText() below.
                // Hex values are prefixed with #x in ESI files.
                if (name == QLatin1String("AssignActivate")) {
                    QString val = xml.readElementText().trimmed();
                    if (val.startsWith(QLatin1String("#x"), Qt::CaseInsensitive)) {
                        config.assignActivate = val.mid(2).toUInt(nullptr, 16);
                    } else if (val.startsWith(QLatin1String("0x"), Qt::CaseInsensitive)) {
                        config.assignActivate = val.mid(2).toUInt(nullptr, 16);
                    } else {
                        config.assignActivate = val.toUInt();
                    }
                } else if (name == QLatin1String("Sync0Cycle")) {
                    config.sync0CycleNs = xml.readElementText().trimmed().toUInt();
                } else if (name == QLatin1String("Sync0Shift")) {
                    config.sync0ShiftNs = xml.readElementText().trimmed().toInt();
                } else if (name == QLatin1String("Sync1Cycle")) {
                    config.sync1CycleNs = xml.readElementText().trimmed().toUInt();
                } else if (name == QLatin1String("Sync1Shift")) {
                    config.sync1ShiftNs = xml.readElementText().trimmed().toInt();
                }
            }
        } else if (xml.isEndElement()) {
            const QString name = xml.name().toString();
            if (name == QLatin1String("OpMode") || name == QLatin1String("OpModeData")) {
                inOpMode = false;
            } else if (name == QLatin1String("Dc")) {
                inDc = false;
                break; // First Dc block is sufficient.
            }
        }
    }

    if (xml.hasError()) {
        // Return a default config — the caller will see assignActivate == 0
        // and can report the parse failure.
        return DcConfig{};
    }

    return config;
}

// ─── DC Configure: query ESI XML and extract DC parameters ──────────────────

QJsonObject DcSyncHandler::handleDcConfigure(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();
    const int position = params.value("position").toInt(-1);

    if (position < 0) {
        return CommandDispatcher::failure(id,
                                          "dcConfigure requires a 'position' parameter specifying the slave index.");
    }

    // Query ESI XML for the slave to extract DC configuration.
    const QString xmlOutput = runCliCommand(master, {"xml", QString::number(position)});

    if (xmlOutput.isEmpty()) {
        return CommandDispatcher::failure(id, QString("Failed to query ESI XML for slave %1. "
                                                      "Is the EtherCAT master running?")
                                                  .arg(position));
    }

    DcConfig config = parseDcConfigFromXml(xmlOutput);

    // Also query current DC status from verbose slave output for context.
    const QString slaveOutput = runCliCommand(master, {"slaves", "-v"});
    const auto slaves = queryDcStatus(slaveOutput);

    // Find the target slave's current DC state.
    bool dcCapable = false;
    bool syncing = false;
    for (const auto& info : slaves) {
        if (info.position == position) {
            dcCapable = info.dcCapable;
            syncing = info.syncing;
            break;
        }
    }

    // Store the configuration for later use by dcActivate.
    {
        std::lock_guard<std::mutex> lock(rtMutex_);
        activeDcConfig_ = config;
    }

    QJsonObject result;
    result["position"] = position;
    result["config"] = dcConfigToJson(config);
    result["dcCapable"] = dcCapable;
    result["syncing"] = syncing;

    return CommandDispatcher::success(id, result);
}

// ─── DC Activate: select reference clock and enable distributed clocks ──────

QJsonObject DcSyncHandler::handleDcActivate(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();

    // refClockSlave: explicit slave index, or -1 (default) for auto-detect.
    const int refClockSlave = params.value("refClockSlave").toInt(-1);

    // Resolve auto-detect: pick the first DC-capable slave from the bus.
    int effectiveRef = refClockSlave;
    if (refClockSlave < 0) {
        const QString slaveOutput = runCliCommand(master, {"slaves", "-v"});
        const auto slaves = queryDcStatus(slaveOutput);
        for (const auto& info : slaves) {
            if (info.dcCapable) {
                effectiveRef = info.position;
                break;
            }
        }
        if (effectiveRef < 0) {
            return CommandDispatcher::failure(
                id, "No DC-capable slave found on the bus for reference clock selection. "
                    "Specify 'refClockSlave' explicitly or ensure a DC-capable slave is connected.");
        }
    }

    // If a master handle is available (FreeRunController is running), call
    // ecrt_master_select_reference_clock() to activate DC sync at the ecrt level.
    bool ecrtRefSet = false;
    uint32_t assignActivate = 0;
    uint32_t sync0CycleNs = 0;
    {
        std::lock_guard<std::mutex> lock(rtMutex_);
        // Note: the actual ecrt_master_select_reference_clock() call happens in
        // FreeRunController's loop via update(). We record the intent here and
        // it will be picked up on the next cycle. This is the same pattern as
        // how rtDataValid_ works — the daemon polls us, and we enrich with live data.
        dcRefClockSlave_ = effectiveRef;
        dcActivated_ = true;
        ecrtRefSet = rtDataValid_; // True if a live master is active.
        assignActivate = activeDcConfig_.assignActivate;
        sync0CycleNs = activeDcConfig_.sync0CycleNs;
    }

    // If a FreeRunController master handle is available, directly invoke the
    // ecrt reference clock selection.  This requires the master pointer which
    // is only available through update() callbacks.  Record the activation
    // intent so the next update() cycle applies it.
    //
    // The actual call to ecrt_master_select_reference_clock() will be done
    // inside the next update() invocation when dcActivated_ is true.

    QJsonObject result;
    result["dcActivated"] = true;
    result["refClockSlave"] = effectiveRef;
    result["ecrtRefSet"] = ecrtRefSet;
    result["assignActivate"] = static_cast<qint64>(assignActivate);
    result["sync0CycleNs"] = static_cast<qint64>(sync0CycleNs);

    return CommandDispatcher::success(id, result);
}

// ─── DC Deactivate: reset DC activation state ──────────────────────────────

QJsonObject DcSyncHandler::handleDcDeactivate(const QString& id, const QJsonObject& params) {
    Q_UNUSED(params);

    {
        std::lock_guard<std::mutex> lock(rtMutex_);
        dcActivated_ = false;
        dcRefClockSlave_ = -1;
        activeDcConfig_ = DcConfig{};
    }

    QJsonObject result;
    result["dcActivated"] = false;
    result["message"] = "DC synchronization deactivated.";

    return CommandDispatcher::success(id, result);
}
