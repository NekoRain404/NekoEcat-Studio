#include "FreeRunController.h"

#include <QJsonArray>
#include <QHash>
#include <QProcess>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStringList>

#include <chrono>
#include <thread>

namespace {
constexpr int64_t NsecPerSec = 1000000000LL;

uint64_t monotonicNsec()
{
    timespec ts {};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * NsecPerSec + static_cast<uint64_t>(ts.tv_nsec);
}
}

FreeRunController::FreeRunController(QObject *parent)
    : QObject(parent)
{
}

FreeRunController::~FreeRunController()
{
    stop();
}

bool FreeRunController::start(uint32_t masterIndex, QString *error)
{
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
    status_ = QString("Running on master %1, %2 slave(s), %3 PDO entries").arg(masterIndex).arg(slaves.size()).arg(totalEntries);
    thread_ = std::thread(&FreeRunController::loop, this);
    return true;
}

void FreeRunController::stop()
{
    if (running_) {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }
    cleanup();
    status_ = "Stopped";
}

bool FreeRunController::running() const
{
    return running_;
}

QString FreeRunController::status() const
{
    return status_;
}

QJsonObject FreeRunController::telemetry() const
{
    std::lock_guard<std::mutex> lock(telemetryMutex_);
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
        {"redundancyActive", static_cast<int>(domainState_.redundancy_active)},
        {"pdoEntries", static_cast<int>(registrations_.empty() ? 0 : registrations_.size() - 1)},
        {"configuredSlaves", static_cast<int>(runtimeSlaves_.size())},
        {"entries", entryTelemetryLocked()},
    };
}

bool FreeRunController::buildConfiguration(uint32_t masterIndex, std::vector<SlaveSpec> *slaves, QString *error) const
{
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
    using namespace std::chrono_literals;
    while (running_) {
        ecrt_master_application_time(master_, monotonicNsec());
        ecrt_master_receive(master_);
        ecrt_domain_process(domain_);
        {
            std::lock_guard<std::mutex> lock(telemetryMutex_);
            ecrt_master_state(master_, &masterState_);
            ecrt_domain_state(domain_, &domainState_);
        }
        ecrt_domain_queue(domain_);
        ecrt_master_send(master_);
        ++cycleCount_;
        std::this_thread::sleep_for(1ms);
    }
}

QString FreeRunController::alStateText(unsigned int alStates) const
{
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
    QString cleaned = name;
    cleaned.remove(QRegularExpression(R"(\s*\[[^\]]+\]\s*)"));
    return cleaned.trimmed();
}

QString FreeRunController::entryDisplayName(const RuntimeEntry &entry) const
{
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
