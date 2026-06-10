#pragma once

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

public:
    explicit FreeRunController(QObject *parent = nullptr);
    ~FreeRunController() override;

    bool start(uint32_t masterIndex = 0, QString *error = nullptr);
    void stop();
    bool running() const;
    QString status() const;
    QJsonObject telemetry() const;

private:
    struct EntrySpec {
        uint16_t index = 0;
        uint8_t subindex = 0;
        uint8_t bitLength = 0;
        QString name;
    };

    struct PdoSpec {
        uint16_t index = 0;
        std::vector<EntrySpec> entries;
    };

    struct SyncSpec {
        uint8_t index = 0;
        ec_direction_t direction = EC_DIR_INVALID;
        ec_watchdog_mode_t watchdog = EC_WD_DEFAULT;
        std::vector<PdoSpec> pdos;
    };

    struct SlaveSpec {
        uint16_t alias = 0;
        uint16_t position = 0;
        uint32_t vendorId = 0;
        uint32_t productCode = 0;
        std::vector<SyncSpec> syncs;
    };

    struct RuntimeSlave {
        SlaveSpec spec;
        std::vector<std::vector<ec_pdo_entry_info_t>> entryStorage;
        std::vector<ec_pdo_info_t> pdoStorage;
        std::vector<ec_sync_info_t> syncStorage;
    };

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
    mutable std::mutex telemetryMutex_;
    ec_master_state_t masterState_ {};
    ec_domain_state_t domainState_ {};

    ec_master_t *master_ = nullptr;
    ec_domain_t *domain_ = nullptr;
    uint8_t *domainData_ = nullptr;
    std::vector<RuntimeSlave> runtimeSlaves_;
    std::vector<unsigned int> offsets_;
    std::vector<unsigned int> bitPositions_;
    std::vector<ec_pdo_entry_reg_t> registrations_;
    std::vector<RuntimeEntry> runtimeEntries_;
};
