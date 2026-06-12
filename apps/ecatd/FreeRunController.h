#pragma once

// ecrt-based Free Run process image controller for real-time I/O.


#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include <ecrt.h>

class FreeRunController : public QObject {
    Q_OBJECT

    // ecrt-based real-time controller that drives the IgH process data cycle.
    // Discovers slave topology via CLI, configures the PDO domain via ecrt,
    // and runs a ~1 kHz receive/process/queue/send loop in a dedicated thread.
public:
    explicit FreeRunController(QObject *parent = nullptr);
    ~FreeRunController() override;

    bool start(uint32_t masterIndex = 0, QString *error = nullptr);
    void stop();
    bool running() const;
    QString status() const;
    QJsonObject telemetry() const;

private:
    // Describes a single PDO entry from the cstruct output.
    struct EntrySpec {
        uint16_t index = 0;
        uint8_t subindex = 0;
        uint8_t bitLength = 0;
        QString name;
    };

    // A PDO containing an ordered list of entries.
    struct PdoSpec {
        uint16_t index = 0;
        std::vector<EntrySpec> entries;
    };

    // A sync manager channel with direction (input/output) and its PDO assignments.
    struct SyncSpec {
        uint8_t index = 0;
        ec_direction_t direction = EC_DIR_INVALID;
        ec_watchdog_mode_t watchdog = EC_WD_DEFAULT;
        std::vector<PdoSpec> pdos;
    };

    // Full slave description extracted from `ethercat cstruct` — vendor/product IDs and sync/PDO tree.
    struct SlaveSpec {
        uint16_t alias = 0;
        uint16_t position = 0;
        uint32_t vendorId = 0;
        uint32_t productCode = 0;
        std::vector<SyncSpec> syncs;
    };

    // Holds the ecrt-allocated storage that must stay alive for the duration of the domain.
    // The vectors back the raw C arrays that ecrt_slave_config_pdos expects.
    struct RuntimeSlave {
        SlaveSpec spec;
        std::vector<std::vector<ec_pdo_entry_info_t>> entryStorage;
        std::vector<ec_pdo_info_t> pdoStorage;
        std::vector<ec_sync_info_t> syncStorage;
    };

    // Per-entry metadata used during the real-time loop to read live values
    // from the domain data buffer at the registered offset.
    struct RuntimeEntry {
        uint16_t slavePosition = 0;
        uint8_t syncIndex = 0;
        QString direction;
        uint16_t pdoIndex = 0;
        uint16_t index = 0;
        uint8_t subindex = 0;
        uint8_t bitLength = 0;
        QString name;
        unsigned int *offset = nullptr;
        unsigned int *bitPosition = nullptr;
    };

    bool buildConfiguration(uint32_t masterIndex, std::vector<SlaveSpec> *slaves, QString *error) const;
    bool parseCStruct(uint16_t position, const QString &text, SlaveSpec *slave, QString *error) const;
    void applyPdoNames(const QString &text, SlaveSpec *slave) const;
    QString runEthercat(uint32_t masterIndex, const QStringList &arguments, int *exitCode, QString *stdErr) const;
    void loop();
    void cleanup();
    QString alStateText(unsigned int alStates) const;
    QString wcStateText(ec_wc_state_t state) const;
    QJsonArray entryTelemetryLocked() const;
    QString normalizedEntryName(const QString &name) const;
    QString entryDisplayName(const RuntimeEntry &entry) const;
    QString entryMeaning(const RuntimeEntry &entry) const;
    QString readEntryRawValue(const RuntimeEntry &entry) const;
    QString readEntryDecodedValue(const RuntimeEntry &entry) const;

    std::atomic_bool running_{false};
    std::atomic_ullong cycleCount_{0};
    std::thread thread_;
    uint32_t activeMasterIndex_ = 0;
    QString status_ = "Stopped";
    // Protects masterState_ and domainState_ which are written by the RT thread.
    mutable std::mutex telemetryMutex_;
    ec_master_state_t masterState_ {};
    ec_domain_state_t domainState_ {};

    // IgH master/domain handles; nullptr when not running.
    ec_master_t *master_ = nullptr;
    ec_domain_t *domain_ = nullptr;
    // Pointer into the domain's shared memory region — the process data image.
    uint8_t *domainData_ = nullptr;
    std::vector<RuntimeSlave> runtimeSlaves_;
    // Byte offsets returned by ecrt_domain_reg_pdo_entry_list for each registered entry.
    std::vector<unsigned int> offsets_;
    std::vector<unsigned int> bitPositions_;
    // Sentinelled list passed to ecrt to register all PDO entries in the domain.
    std::vector<ec_pdo_entry_reg_t> registrations_;
    std::vector<RuntimeEntry> runtimeEntries_;
};
