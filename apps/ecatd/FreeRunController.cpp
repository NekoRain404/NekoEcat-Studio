// ecrt-based Free Run process image controller for real-time I/O.
#include "FreeRunController.h"

#include <QJsonArray>
#include <QHash>
#include <QProcess>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStringList>

#include <cerrno>
#include <sched.h>
#include <sys/mman.h>
#include <thread>

#include <time.h>

namespace {
constexpr int64_t NsecPerSec = 1000000000LL;

// CLOCK_MONOTONIC avoids NTP jumps; ecrt requires absolute nanosecond timestamps for DC sync.
uint64_t monotonicNsec()
{
    timespec ts {};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * NsecPerSec + static_cast<uint64_t>(ts.tv_nsec);
}
}

FreeRunController::FreeRunController(QObject *parent)
    // The controller starts idle; call start() to acquire an IgH master.
    : QObject(parent)
{
}

FreeRunController::~FreeRunController()
{
    // Ensure the real-time thread is stopped and ecrt resources are released.
    stop();
}

bool FreeRunController::start(uint32_t masterIndex, QString *error)
{
    // Acquire exclusive access to the IgH master, auto-discover the slave topology
    // via the ethercat CLI, register all PDO entries into a single process data domain,
    // then spin up the real-time cycle thread at ~1 kHz.
    if (running_) {
        if (masterIndex == activeMasterIndex_) {
            return true;
        }
        if (error) {
            *error = QString("Free Run is already active on master %1. Stop it before switching masters.").arg(activeMasterIndex_);
        }
        return false;
    }

    std::vector<SlaveSpec> slaves;
    if (!buildConfiguration(masterIndex, &slaves, error)) {
        return false;
    }
    if (slaves.empty()) {
        if (error) {
            *error = "No EtherCAT slaves found for Free Run.";
        }
        return false;
    }

    master_ = ecrt_request_master(masterIndex);
    if (!master_) {
        if (error) {
            *error = QString("Failed to request IgH master %1. Stop other EtherCAT applications and retry.").arg(masterIndex);
        }
        cleanup();
        return false;
    }

    domain_ = ecrt_master_create_domain(master_);
    if (!domain_) {
        if (error) {
            *error = "Failed to create EtherCAT process data domain.";
        }
        cleanup();
        return false;
    }

    ecrt_master_set_send_interval(master_, 1000000);

    runtimeSlaves_.clear();
    registrations_.clear();
    runtimeEntries_.clear();
    offsets_.clear();
    bitPositions_.clear();

    int totalEntries = 0;
    for (const auto &slave : slaves) {
        for (const auto &sync : slave.syncs) {
            for (const auto &pdo : sync.pdos) {
                totalEntries += static_cast<int>(pdo.entries.size());
            }
        }
    }
    offsets_.resize(totalEntries);
    bitPositions_.resize(totalEntries);

    int entryCursor = 0;
    for (const auto &slave : slaves) {
        auto *config = ecrt_master_slave_config(master_, slave.alias, slave.position, slave.vendorId, slave.productCode);
        if (!config) {
            if (error) {
                *error = QString("Failed to create slave config for position %1.").arg(slave.position);
            }
            cleanup();
            return false;
        }

        RuntimeSlave runtime;
        runtime.spec = slave;
        runtime.entryStorage.reserve(slave.syncs.size());

        for (const auto &sync : slave.syncs) {
            for (const auto &pdo : sync.pdos) {
                std::vector<ec_pdo_entry_info_t> entries;
                entries.reserve(pdo.entries.size());
                for (const auto &entry : pdo.entries) {
                    entries.push_back({entry.index, entry.subindex, entry.bitLength});
                    registrations_.push_back({
                        slave.alias,
                        slave.position,
                        slave.vendorId,
                        slave.productCode,
                        entry.index,
                        entry.subindex,
                        &offsets_[entryCursor],
                        &bitPositions_[entryCursor],
                    });
                    runtimeEntries_.push_back({
                        slave.position,
                        sync.index,
                        sync.direction == EC_DIR_OUTPUT ? "RxPDO" : "TxPDO",
                        pdo.index,
                        entry.index,
                        entry.subindex,
                        entry.bitLength,
                        entry.name,
                        &offsets_[entryCursor],
                        &bitPositions_[entryCursor],
                    });
                    ++entryCursor;
                }
                runtime.entryStorage.push_back(std::move(entries));
            }
        }

        int pdoCursor = 0;
        for (const auto &sync : slave.syncs) {
            for (const auto &pdo : sync.pdos) {
                auto &entries = runtime.entryStorage[pdoCursor++];
                runtime.pdoStorage.push_back({pdo.index, static_cast<unsigned int>(entries.size()), entries.data()});
            }
        }

        pdoCursor = 0;
        for (const auto &sync : slave.syncs) {
            ec_pdo_info_t *pdoData = sync.pdos.empty() ? nullptr : runtime.pdoStorage.data() + pdoCursor;
            runtime.syncStorage.push_back({
                sync.index,
                sync.direction,
                static_cast<unsigned int>(sync.pdos.size()),
                pdoData,
                sync.watchdog,
            });
            pdoCursor += static_cast<int>(sync.pdos.size());
        }
        runtime.syncStorage.push_back({0xff});

        if (ecrt_slave_config_pdos(config, EC_END, runtime.syncStorage.data())) {
            if (error) {
                *error = QString("Failed to configure PDOs for slave %1.").arg(slave.position);
            }
            cleanup();
            return false;
        }
        runtimeSlaves_.push_back(std::move(runtime));
    }

    registrations_.push_back({});
    if (ecrt_domain_reg_pdo_entry_list(domain_, registrations_.data())) {
        if (error) {
            *error = "Failed to register PDO entries in the process data domain.";
        }
        cleanup();
        return false;
    }

    if (ecrt_master_activate(master_)) {
        if (error) {
            *error = "Failed to activate IgH master for Free Run.";
        }
        cleanup();
        return false;
    }

    domainData_ = ecrt_domain_data(domain_);
    if (!domainData_) {
        if (error) {
            *error = "Failed to obtain domain process data pointer.";
        }
        cleanup();
        return false;
    }

    running_ = true;
    activeMasterIndex_ = masterIndex;
    cycleCount_ = 0;
    wcErrorCount_ = 0;
    {
        std::lock_guard<std::mutex> lock(cycleMutex_);
        minCycleNsec_ = INT64_MAX;
        maxCycleNsec_ = 0;
        totalCycleNsec_ = 0;
    }
    {
        std::lock_guard<std::mutex> lock(telemetryMutex_);
        status_ = QString("Running on master %1, %2 slave(s), %3 PDO entries").arg(masterIndex).arg(slaves.size()).arg(totalEntries);
    }
    thread_ = std::thread(&FreeRunController::loop, this);
    return true;
}

void FreeRunController::stop()
{
    // Signal the cycle thread to exit and release all ecrt resources.
    if (running_) {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }
    cleanup();
    {
        std::lock_guard<std::mutex> lock(telemetryMutex_);
        status_ = "Stopped";
    }
}

bool FreeRunController::running() const
{
    return running_;
}

// Thread-safe status string for display.
QString FreeRunController::status() const
{
    std::lock_guard<std::mutex> lock(telemetryMutex_);
    return status_;
}

// Snapshot of master/domain state and per-entry live values; mutex-protected
// because the real-time thread writes masterState_/domainState_ concurrently.
QJsonObject FreeRunController::telemetry() const
{
    std::lock_guard<std::mutex> lock(telemetryMutex_);
    std::lock_guard<std::mutex> cycleLock(cycleMutex_);
    const auto cycles = cycleCount_.load();
    int64_t minNs = minCycleNsec_, maxNs = maxCycleNsec_, avgNs = 0, jitterNs = 0;
    if (minNs == INT64_MAX) minNs = 0;
    if (cycles > 0) {
        avgNs = totalCycleNsec_ / static_cast<int64_t>(cycles);
        jitterNs = std::max(maxNs - avgNs, avgNs - minNs);
    }
    return {
        {"running", running_.load()},
        {"status", status_},
        {"master", static_cast<int>(activeMasterIndex_)},
        {"cycles", QString::number(cycleCount_.load())},
        {"slavesResponding", static_cast<int>(masterState_.slaves_responding)},
        {"alStates", static_cast<int>(masterState_.al_states)},
        {"alStateText", alStateText(masterState_.al_states)},
        {"linkUp", static_cast<bool>(masterState_.link_up)},
        {"workingCounter", static_cast<int>(domainState_.working_counter)},
        {"wcState", static_cast<int>(domainState_.wc_state)},
        {"wcStateText", wcStateText(domainState_.wc_state)},
        {"wcErrors", static_cast<qint64>(wcErrorCount_.load())},
        {"minCycleUsec", static_cast<double>(minNs) / 1000.0},
        {"maxCycleUsec", static_cast<double>(maxNs) / 1000.0},
        {"avgCycleUsec", static_cast<double>(avgNs) / 1000.0},
        {"jitterUsec", static_cast<double>(jitterNs) / 1000.0},
        {"redundancyActive", static_cast<int>(domainState_.redundancy_active)},
        {"pdoEntries", static_cast<int>(registrations_.empty() ? 0 : registrations_.size() - 1)},
        {"configuredSlaves", static_cast<int>(runtimeSlaves_.size())},
        {"entries", entryTelemetryLocked()},
    };
}

bool FreeRunController::buildConfiguration(uint32_t masterIndex, std::vector<SlaveSpec> *slaves, QString *error) const
{
    // Auto-discover slave topology by parsing `ethercat slaves` output for positions,
    // then `ethercat cstruct` for the exact PDO mapping that ecrt needs to configure the domain.
    int exitCode = 0;
    QString stdErr;
    const QString scan = runEthercat(masterIndex, {"slaves"}, &exitCode, &stdErr);
    if (exitCode != 0) {
        if (error) {
            *error = stdErr.isEmpty() ? scan : stdErr;
        }
        return false;
    }

    const QRegularExpression lineRe(R"(^\s*(\d+)\s+)");
    for (const auto &line : scan.split('\n', Qt::SkipEmptyParts)) {
        const auto match = lineRe.match(line);
        if (!match.hasMatch()) {
            continue;
        }
        const uint16_t position = static_cast<uint16_t>(match.captured(1).toUShort());
        const QString cstruct = runEthercat(masterIndex, {"cstruct", "-p", QString::number(position)}, &exitCode, &stdErr);
        if (exitCode != 0) {
            if (error) {
                *error = stdErr.isEmpty() ? cstruct : stdErr;
            }
            return false;
        }
        SlaveSpec slave;
        if (!parseCStruct(position, cstruct, &slave, error)) {
            return false;
        }
        const QString pdos = runEthercat(masterIndex, {"pdos", "-p", QString::number(position)}, &exitCode, &stdErr);
        if (exitCode == 0) {
            applyPdoNames(pdos, &slave);
        }
        slaves->push_back(std::move(slave));
    }
    return true;
}

bool FreeRunController::parseCStruct(uint16_t position, const QString &text, SlaveSpec *slave, QString *error) const
{
    // Extract vendor/product IDs and the full PDO/sync tree from IgH's generated C struct.
    // Entry names come from inline `/* ... */` comments that IgH places after each entry.
    slave->position = position;

    const auto vendor = QRegularExpression(R"(Vendor ID:\s+0x([0-9a-fA-F]+))").match(text);
    const auto product = QRegularExpression(R"(Product code:\s+0x([0-9a-fA-F]+))").match(text);
    if (!vendor.hasMatch() || !product.hasMatch()) {
        if (error) {
            *error = QString("Failed to parse vendor/product from cstruct for slave %1.").arg(position);
        }
        return false;
    }
    slave->vendorId = vendor.captured(1).toUInt(nullptr, 16);
    slave->productCode = product.captured(1).toUInt(nullptr, 16);

    std::vector<EntrySpec> allEntries;
    const QRegularExpression entryRe(R"(\{0x([0-9a-fA-F]+),\s*0x([0-9a-fA-F]+),\s*(\d+)\})");
    auto entryIt = entryRe.globalMatch(text);
    while (entryIt.hasNext()) {
        const auto m = entryIt.next();
        QString name;
        const int commentStart = text.indexOf("/*", m.capturedEnd());
        const int nextLine = text.indexOf('\n', m.capturedEnd());
        if (commentStart >= 0 && (nextLine < 0 || commentStart < nextLine)) {
            const int commentEnd = text.indexOf("*/", commentStart);
            if (commentEnd > commentStart) {
                name = text.mid(commentStart + 2, commentEnd - commentStart - 2).trimmed();
            }
        }
        allEntries.push_back({
            static_cast<uint16_t>(m.captured(1).toUShort(nullptr, 16)),
            static_cast<uint8_t>(m.captured(2).toUShort(nullptr, 16)),
            static_cast<uint8_t>(m.captured(3).toUShort()),
            name,
        });
    }

    std::vector<PdoSpec> allPdos;
    const QRegularExpression pdoRe(R"(\{0x([0-9a-fA-F]+),\s*(\d+),\s*slave_\d+_pdo_entries\s*\+\s*(\d+)\})");
    auto pdoIt = pdoRe.globalMatch(text);
    while (pdoIt.hasNext()) {
        const auto m = pdoIt.next();
        PdoSpec pdo;
        pdo.index = static_cast<uint16_t>(m.captured(1).toUShort(nullptr, 16));
        const int count = m.captured(2).toInt();
        const int start = m.captured(3).toInt();
        for (int i = 0; i < count && start + i < static_cast<int>(allEntries.size()); ++i) {
            pdo.entries.push_back(allEntries[start + i]);
        }
        allPdos.push_back(std::move(pdo));
    }

    const QRegularExpression syncRe(R"(\{(\d+),\s*(EC_DIR_[A-Z]+),\s*(\d+),\s*(?:slave_\d+_pdos\s*\+\s*(\d+)|NULL),\s*(EC_WD_[A-Z]+)\})");
    auto syncIt = syncRe.globalMatch(text);
    while (syncIt.hasNext()) {
        const auto m = syncIt.next();
        SyncSpec sync;
        sync.index = static_cast<uint8_t>(m.captured(1).toUShort());
        sync.direction = m.captured(2) == "EC_DIR_OUTPUT" ? EC_DIR_OUTPUT : EC_DIR_INPUT;
        sync.watchdog = m.captured(5) == "EC_WD_ENABLE" ? EC_WD_ENABLE : EC_WD_DISABLE;
        const int count = m.captured(3).toInt();
        const int start = m.captured(4).isEmpty() ? 0 : m.captured(4).toInt();
        for (int i = 0; i < count && start + i < static_cast<int>(allPdos.size()); ++i) {
            sync.pdos.push_back(allPdos[start + i]);
        }
        slave->syncs.push_back(std::move(sync));
    }

    if (slave->syncs.empty()) {
        if (error) {
            *error = QString("No PDO sync information found for slave %1.").arg(position);
        }
        return false;
    }
    return true;
}

void FreeRunController::applyPdoNames(const QString &text, SlaveSpec *slave) const
{
    // Fill in human-readable names from `ethercat pdos` verbose output where cstruct had none.
    // This enriches the telemetry display with symbolic PDO entry names.
    if (!slave) {
        return;
    }

    QHash<QString, QString> names;
    const QRegularExpression entryRe(R"(^\s+PDO entry\s+(0x[0-9a-fA-F]+):([0-9a-fA-F]+),\s+\d+\s+bit,\s+\"(.+)\")");
    for (const auto &line : text.split('\n')) {
        const auto match = entryRe.match(line);
        if (!match.hasMatch()) {
            continue;
        }
        const QString key = QString("%1:%2")
                                .arg(match.captured(1).toUShort(nullptr, 16), 4, 16, QLatin1Char('0'))
                                .arg(match.captured(2).toUShort(nullptr, 16), 2, 16, QLatin1Char('0'));
        const QString name = match.captured(3).trimmed();
        if (!name.isEmpty()) {
            names.insert(key, name);
        }
    }
    if (names.isEmpty()) {
        return;
    }

    for (auto &sync : slave->syncs) {
        for (auto &pdo : sync.pdos) {
            for (auto &entry : pdo.entries) {
                if (!entry.name.trimmed().isEmpty()) {
                    continue;
                }
                const QString key = QString("%1:%2")
                                        .arg(entry.index, 4, 16, QLatin1Char('0'))
                                        .arg(entry.subindex, 2, 16, QLatin1Char('0'));
                entry.name = names.value(key);
            }
        }
    }
}

QString FreeRunController::runEthercat(uint32_t masterIndex, const QStringList &arguments, int *exitCode, QString *stdErr) const
{
    // Shell out to the IgH `ethercat` CLI with a specific master index.
    // Used during configuration discovery, not in the real-time loop.
    QProcess process;
    process.setProgram("ethercat");
    QStringList scopedArguments = {"-m", QString::number(masterIndex)};
    scopedArguments << arguments;
    process.setArguments(scopedArguments);
    process.start();
    if (!process.waitForStarted(3000)) {
        if (exitCode) {
            *exitCode = -1;
        }
        if (stdErr) {
            *stdErr = "Failed to start ethercat CLI.";
        }
        return {};
    }
    process.waitForFinished(10000);
    if (exitCode) {
        *exitCode = process.exitCode();
    }
    if (stdErr) {
        *stdErr = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    }
    return QString::fromLocal8Bit(process.readAllStandardOutput());
}

void FreeRunController::loop()
{
    // Real-time cycle: receive -> process -> sample state -> queue -> send,
    // at ~1 kHz using absolute-time clock_nanosleep to avoid drift accumulation.

    // Elevate to real-time scheduling — failure is non-fatal, just means less deterministic timing.
    struct sched_param param{};
    param.sched_priority = 80;
    sched_setscheduler(0, SCHED_FIFO, &param);

    // Lock all current and future memory pages to prevent page faults during the RT loop.
    mlockall(MCL_CURRENT | MCL_FUTURE);

    constexpr uint64_t cycleNsec = 1000000ULL; // 1 ms
    uint64_t wakeupTime = monotonicNsec();
    uint64_t prevTime = wakeupTime;

    while (running_) {
        // Schedule next wake-up using absolute time to avoid drift accumulation.
        wakeupTime += cycleNsec;

        // Sleep until next cycle — clock_nanosleep with TIMER_ABSTIME avoids drift.
        struct timespec wake{};
        wake.tv_sec = static_cast<time_t>(wakeupTime / NsecPerSec);
        wake.tv_nsec = static_cast<long>(wakeupTime % NsecPerSec);
        int sleepErr = 0;
        while ((sleepErr = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake, nullptr)) == EINTR) {
            // Retry on signal interruption.
        }
        if (sleepErr != 0) {
            // EINVAL (invalid time) or EFAULT — break to avoid spinning at full CPU.
            break;
        }

        if (!running_) break;

        // Measure actual cycle interval for jitter analysis.
        const uint64_t now = monotonicNsec();
        const int64_t cycleDelta = static_cast<int64_t>(now - prevTime);
        prevTime = now;
        {
            std::lock_guard<std::mutex> lock(cycleMutex_);
            if (cycleDelta < minCycleNsec_) minCycleNsec_ = cycleDelta;
            if (cycleDelta > maxCycleNsec_) maxCycleNsec_ = cycleDelta;
            totalCycleNsec_ += cycleDelta;
        }

        ecrt_master_application_time(master_, monotonicNsec());
        ecrt_master_receive(master_);
        ecrt_domain_process(domain_);
        {
            std::lock_guard<std::mutex> lock(telemetryMutex_);
            ecrt_master_state(master_, &masterState_);
            ecrt_domain_state(domain_, &domainState_);
            // Track consecutive WC errors for diagnostics.
            if (domainState_.wc_state != EC_WC_COMPLETE) {
                ++wcErrorCount_;
                // Update status when WC errors exceed threshold.
                if (wcErrorCount_ == kWcErrorThreshold) {
                    status_ = QString("WARNING: %1 consecutive WC errors — check bus wiring")
                                  .arg(wcErrorCount_.load());
                }
            } else {
                if (wcErrorCount_ > 0) {
                    // Recovered from WC errors — restore normal status.
                    status_ = QString("Running on master %1 — WC recovered after %2 errors")
                                  .arg(activeMasterIndex_)
                                  .arg(wcErrorCount_.load());
                }
                wcErrorCount_ = 0;
            }
        }
        ecrt_domain_queue(domain_);
        ecrt_master_send(master_);
        ++cycleCount_;
    }

    munlockall();

    // Restore normal scheduling before exiting.
    struct sched_param normal{};
    sched_setscheduler(0, SCHED_OTHER, &normal);
}

QString FreeRunController::alStateText(unsigned int alStates) const
{
    // Decode AL state bitmask into human-readable names (INIT/PREOP/SAFEOP/OP).
    QStringList states;
    if (alStates & 0x01) {
        states << "INIT";
    }
    if (alStates & 0x02) {
        states << "PREOP";
    }
    if (alStates & 0x04) {
        states << "SAFEOP";
    }
    if (alStates & 0x08) {
        states << "OP";
    }
    return states.isEmpty() ? "Unknown" : states.join(" | ");
}

QString FreeRunController::wcStateText(ec_wc_state_t state) const
{
    // Working counter completeness — indicates whether all registered slaves responded.
    switch (state) {
    case EC_WC_ZERO:
        return "Zero";
    case EC_WC_INCOMPLETE:
        return "Incomplete";
    case EC_WC_COMPLETE:
        return "Complete";
    default:
        return "Unknown";
    }
}

QJsonArray FreeRunController::entryTelemetryLocked() const
{
    // Build JSON array of all registered PDO entries with live values read from
    // the shared process data image. Called with telemetryMutex_ held.
    QJsonArray array;
    for (const auto &entry : runtimeEntries_) {
        array.append(QJsonObject{
            {"slave", static_cast<int>(entry.slavePosition)},
            {"sync", static_cast<int>(entry.syncIndex)},
            {"direction", entry.direction},
            {"pdo", QString("0x%1").arg(entry.pdoIndex, 4, 16, QLatin1Char('0'))},
            {"index", QString("0x%1").arg(entry.index, 4, 16, QLatin1Char('0'))},
            {"subindex", QString("0x%1").arg(entry.subindex, 2, 16, QLatin1Char('0'))},
            {"bits", static_cast<int>(entry.bitLength)},
            {"offset", entry.offset ? static_cast<int>(*entry.offset) : -1},
            {"bit", entry.bitPosition ? static_cast<int>(*entry.bitPosition) : -1},
            {"name", entry.name},
            {"displayName", entryDisplayName(entry)},
            {"rawValue", readEntryRawValue(entry)},
            {"decodedValue", readEntryDecodedValue(entry)},
            {"meaning", entryMeaning(entry)},
        });
    }
    return array;
}

QString FreeRunController::normalizedEntryName(const QString &name) const
{
    // Strip IgH's bracket annotations (e.g. "[0x6000:01]") for cleaner display.
    QString cleaned = name;
    cleaned.remove(QRegularExpression(R"(\s*\[[^\]]+\]\s*)"));
    return cleaned.trimmed();
}

QString FreeRunController::entryDisplayName(const RuntimeEntry &entry) const
{
    // Fall back to direction + hex address when no symbolic name is available.
    const QString cleaned = normalizedEntryName(entry.name);
    if (!cleaned.isEmpty()) {
        return cleaned;
    }
    return QString("%1 %2:%3")
        .arg(entry.direction,
             QString("0x%1").arg(entry.index, 4, 16, QLatin1Char('0')),
             QString("0x%1").arg(entry.subindex, 2, 16, QLatin1Char('0')));
}

QString FreeRunController::entryMeaning(const RuntimeEntry &entry) const
{
    // Infer physical meaning from entry name keywords for UI grouping
    // (e.g. "flow" -> Flow, "pressure" -> Pressure).
    const QString name = entryDisplayName(entry).toLower();
    if (name.contains("flow")) {
        return "Flow";
    }
    if (name.contains("pressure")) {
        return "Pressure";
    }
    if (name.contains("temperature")) {
        return "Temperature";
    }
    if (name.contains("position")) {
        return "Position";
    }
    if (name.contains("exception") || name.contains("status")) {
        return "Status";
    }
    if (name.contains("actuator") || name.contains("valve")) {
        return "Actuator";
    }
    return entry.direction;
}

QString FreeRunController::readEntryRawValue(const RuntimeEntry &entry) const
{
    // Read the raw integer value directly from the domain data buffer
    // using ecrt's width-specific read macros (1/8/16/32/64-bit).
    if (!domainData_ || !entry.offset || *entry.offset == static_cast<unsigned int>(-1)) {
        return {};
    }
    const uint8_t *data = domainData_ + *entry.offset;
    switch (entry.bitLength) {
    case 1:
        return QString::number(EC_READ_BIT(data, entry.bitPosition ? *entry.bitPosition : 0));
    case 8:
        return QString::number(EC_READ_U8(data));
    case 16:
        return QString::number(EC_READ_U16(data));
    case 32:
        return QString::number(EC_READ_U32(data));
    case 64:
        return QString::number(EC_READ_U64(data));
    default:
        return QString("raw %1 bit").arg(entry.bitLength);
    }
}

QString FreeRunController::readEntryDecodedValue(const RuntimeEntry &entry) const
{
    // Interpret 32-bit values as IEEE 754 floats (common for analog sensors);
    // other widths fall through to the raw integer representation.
    if (!domainData_ || !entry.offset || *entry.offset == static_cast<unsigned int>(-1)) {
        return {};
    }
    const uint8_t *data = domainData_ + *entry.offset;
    if (entry.bitLength == 32) {
        return QString::number(EC_READ_REAL(data), 'f', 6);
    }
    return readEntryRawValue(entry);
}

void FreeRunController::cleanup()
{
    // Release the IgH master/domain and reset all runtime state to idle.
    running_ = false;  // Ensure the RT thread will exit even if stop() was not called.
    if (thread_.joinable()) {
        thread_.join();
    }
    if (master_) {
        ecrt_release_master(master_);
    }
    master_ = nullptr;
    domain_ = nullptr;
    domainData_ = nullptr;
    runtimeSlaves_.clear();
    registrations_.clear();
    runtimeEntries_.clear();
    offsets_.clear();
    bitPositions_.clear();
    activeMasterIndex_ = 0;
    cycleCount_ = 0;
    std::lock_guard<std::mutex> lock(telemetryMutex_);
    masterState_ = {};
    domainState_ = {};
}
