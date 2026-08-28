#pragma once

// ecrt-based Free Run process image controller for real-time I/O.


#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <ecrt.h>

#include "freerun_shm_mirror.h"
#include "nekoecat_shm.h"

#include <cstdint>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

class FreeRunController : public QObject {
    Q_OBJECT

    // ecrt-based real-time controller that drives the IgH process data cycle.
    // Discovers slave topology via CLI, configures the PDO domain via ecrt,
    // and runs a ~1 kHz receive/process/queue/send loop in a dedicated thread.
public:
    explicit FreeRunController(uint32_t cycleNsec = 1000000, QObject *parent = nullptr);
    ~FreeRunController() override;

    bool start(uint32_t masterIndex = 0, QString *error = nullptr);
    void stop();
    bool running() const;
    QString status() const;
    QJsonObject telemetry() const;

    // Expose the IgH master handle for DC sync enrichment.
    // Returns nullptr when Free Run is not active.
    ec_master_t *masterHandle() const { return master_; }

    // Number of discovered slaves on the bus (safe for the daemon's event
    // loop thread: runtimeSlaves_ is only mutated during start/stop, which
    // run on the same thread).
    int slaveCount() const { return static_cast<int>(runtimeSlaves_.size()); }

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

public:
    // Shared memory layout for external clients (double buffer + version) - public for tests + client
    // The layout itself lives in the canonical Qt-free header nekoecat_shm.h.

private:
    static constexpr size_t kMaxProcessDataSize = NEKOECAT_SHM_MAX_PROCESS_DATA_SIZE;
    static constexpr const char* kShmName = "/nekoecat_proc_0";

    bool buildConfiguration(uint32_t masterIndex, std::vector<SlaveSpec> *slaves, QString *error) const;
    bool parseCStruct(uint16_t position, const QString &text, SlaveSpec *slave, QString *error) const;
    void applyPdoNames(const QString &text, SlaveSpec *slave) const;
    QString runEthercat(uint32_t masterIndex, const QStringList &arguments, int *exitCode, QString *stdErr) const;
    bool initSharedMemory(uint32_t dataSize, QString *error);
    void cleanupSharedMemory();
    void mirrorToShm();
    std::vector<ShmMirrorEntry> buildMirrorEntries() const;
public:
    QJsonObject shmInfo() const;  // for RPC exposure: name, size, layout_version, entries[]
    void loop();
    void cleanup();

#ifdef UNIT_TEST
public:
    void testMirrorToShm() { mirrorToShm(); }
    void testSetDomainData(uint8_t* d, uint32_t s) { domainData_ = d; if (shm_header_) nekoecat_shm_store(&shm_header_->data_size, s, NEKOECAT_MO_RELAXED); }
    void testSetupTestShm(ShmHeader* h, uint8_t* buf0, uint8_t* buf1, uint32_t dsz) {
        shm_header_ = h; shm_data_[0] = buf0; shm_data_[1] = buf1; shm_stride_ = dsz;
        if (h) {
            nekoecat_shm_store(&h->data_size, dsz, NEKOECAT_MO_RELAXED);
            nekoecat_shm_store(&h->active_buffer, 0, NEKOECAT_MO_RELAXED);
            nekoecat_shm_store(&h->version, 0, NEKOECAT_MO_RELAXED);
            nekoecat_shm_store(&h->cycle_count, 0, NEKOECAT_MO_RELAXED);
            nekoecat_shm_store(&h->layout_version, NEKOECAT_SHM_LAYOUT_VERSION, NEKOECAT_MO_RELAXED);
        }
    }
    void testAddMirrorEntry(uint16_t slave, uint16_t idx, uint8_t sub, uint8_t blen, const char* dir, uint32_t off) {
        RuntimeEntry e{};
        e.slavePosition = slave; e.index = idx; e.subindex = sub; e.bitLength = blen;
        e.direction = QString::fromUtf8(dir);
        testOffs_.push_back(std::make_unique<unsigned int>(off));
        e.offset = testOffs_.back().get();
        runtimeEntries_.push_back(e);
    }
#endif
    QString alStateText(unsigned int alStates) const;
    QString wcStateText(ec_wc_state_t state) const;
    QJsonArray entryTelemetryLocked() const;
    QString normalizedEntryName(const QString &name) const;
    QString entryDisplayName(const RuntimeEntry &entry) const;
    QString entryMeaning(const RuntimeEntry &entry) const;
    QString readEntryRawValue(const RuntimeEntry &entry) const;
    QString readEntryDecodedValue(const RuntimeEntry &entry) const;

private:
    std::atomic_bool running_{false};
    std::atomic_ullong cycleCount_{0};
    std::atomic_ullong wcErrorCount_{0};  // Consecutive WC completeness errors.
    static constexpr int kWcErrorThreshold = 100;  // Update status after this many consecutive errors.
    // Cycle time statistics (nanoseconds) — lock-free atomics so the RT loop
    // never takes a mutex (telemetry() reads them relaxed; approximate values
    // are acceptable for diagnostics).
    std::atomic<int64_t> minCycleNsec_{INT64_MAX};
    std::atomic<int64_t> maxCycleNsec_{0};
    std::atomic<int64_t> totalCycleNsec_{0};
    std::thread thread_;
    uint32_t activeMasterIndex_ = 0;
    QString status_ = "Stopped";

    // Shared memory for external real-time clients
    int shm_fd_ = -1;
    void* shm_ptr_ = nullptr;
    size_t shm_size_ = 0;
    size_t shm_stride_ = 0;  // per-buffer stride actually allocated (== data_size)
    ShmHeader* shm_header_ = nullptr;
    uint8_t* shm_data_[2] = {nullptr, nullptr};
    // Protects masterState_ and domainState_ which are written by the RT thread.
    mutable std::mutex stateMutex_;
    ec_master_state_t masterState_ {};
    ec_domain_state_t domainState_ {};
    mutable std::mutex startStopMutex_;
    uint32_t cycleNsec_;
    mutable std::mutex statusMutex_;
    QString lastWarning_;

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
    // Precomputed mirror layout (built once at start(), reused by the RT loop).
    std::vector<ShmMirrorEntry> mirrorEntries_;
#ifdef UNIT_TEST
    std::vector<std::unique_ptr<unsigned int>> testOffs_;
#endif
};
