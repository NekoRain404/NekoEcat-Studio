#pragma once

// Real-time cycle timing controller for EtherCAT bus stability testing.
// Measures ecrt receive/process/send cycle intervals to quantify latency,
// jitter, and packet loss — the key indicators of communication stability.

#include <QObject>
#include <QJsonObject>
#include <QString>
#include <atomic>
#include <mutex>
#include <thread>

#include <ecrt.h>

class RtTestController : public QObject {
    Q_OBJECT

    // Drives a continuous ecrt receive/process/send cycle at a configurable rate
    // (default ~1 kHz) and collects timing statistics for each cycle.
    // Reports min/max/avg latency, jitter (max deviation from avg), cycle count,
    // and a rolling window of recent cycle times for trend display.
public:
    explicit RtTestController(QObject *parent = nullptr);
    ~RtTestController() override;

    // Start the RT test on the given IgH master index.
    // cycleUsec: target cycle time in microseconds (default 1000 = 1 kHz).
    bool start(uint32_t masterIndex, int cycleUsec = 1000, QString *error = nullptr);
    void stop();
    bool running() const;
    QString status() const;

    // Snapshot of current test statistics.
    QJsonObject telemetry() const;

private:
    void loop(int cycleUsec);
    void cleanup();

    // High-resolution monotonic timestamp in nanoseconds.
    static uint64_t monotonicNsec();

    std::atomic_bool running_{false};
    std::atomic_ullong cycleCount_{0};
    std::atomic_ullong errorCount_{0};
    std::thread thread_;
    uint32_t activeMasterIndex_ = 0;
    QString status_ = "Stopped";

    // IgH handles — nullptr when not running.
    ec_master_t *master_ = nullptr;
    ec_domain_t *domain_ = nullptr;
    uint8_t *domainData_ = nullptr;

    // Timing statistics (protected by mutex, written by RT thread).
    mutable std::mutex statsMutex_;
    int64_t minCycleNsec_ = INT64_MAX;
    int64_t maxCycleNsec_ = 0;
    int64_t totalCycleNsec_ = 0;
    // Rolling window of recent cycle times (nanoseconds) for trend display.
    static constexpr int kRollingWindow = 2000;
    std::vector<int64_t> recentCycles_;
};
