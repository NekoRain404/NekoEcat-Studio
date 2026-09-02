// Real-time cycle timing controller for EtherCAT bus stability testing.
#include "RtTestController.h"

#include <QJsonArray>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <numeric>
#include <sched.h>
#include <sys/mman.h>
#include <thread>

namespace {
constexpr int64_t NsecPerSec = 1000000000LL;
}

// High-resolution monotonic clock — avoids NTP jumps that CLOCK_REALTIME would introduce.
uint64_t RtTestController::monotonicNsec() {
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * NsecPerSec + static_cast<uint64_t>(ts.tv_nsec);
}

RtTestController::RtTestController(QObject* parent) : QObject(parent) {}

RtTestController::~RtTestController() {
    stop();
}

bool RtTestController::start(uint32_t masterIndex, int cycleUsec, QString* error) {
    // Clamp the requested cycle time to a sane real-time range. A value of 0 or
    // negative would otherwise produce an invalid timespec busy-loop.
    if (cycleUsec < 500 || cycleUsec > 1000000) {
        if (error) {
            *error = QString("cycleUsec %1 out of range; use 500..1000000 usec.").arg(cycleUsec);
        }
        return false;
    }

    // Prevent double-start; if already running on the same master, treat as success.
    if (running_) {
        if (masterIndex == activeMasterIndex_) {
            return true;
        }
        if (error) {
            *error = QString("RT test is already running on master %1.").arg(activeMasterIndex_);
        }
        return false;
    }

    // Acquire exclusive access to the IgH master.
    master_ = ecrt_request_master(masterIndex);
    if (!master_) {
        if (error) {
            *error = QString("Failed to request IgH master %1. "
                             "Stop other EtherCAT applications and retry.")
                         .arg(masterIndex);
        }
        cleanup();
        return false;
    }

    // Create a process data domain — required by ecrt even if we don't register entries.
    // The domain allocates the shared memory region used by ecrt_master_receive/send.
    domain_ = ecrt_master_create_domain(master_);
    if (!domain_) {
        if (error) {
            *error = "Failed to create EtherCAT process data domain.";
        }
        cleanup();
        return false;
    }

    // Activate the master — this transitions the master to OP and starts the stack.
    if (ecrt_master_activate(master_)) {
        if (error) {
            *error = "Failed to activate IgH master.";
        }
        cleanup();
        return false;
    }

    domainData_ = ecrt_domain_data(domain_);
    // domainData_ may be nullptr for an empty domain — that's fine, we only measure timing.

    activeMasterIndex_ = masterIndex;
    cycleCount_ = 0;
    errorCount_ = 0;
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        minCycleNsec_ = INT64_MAX;
        maxCycleNsec_ = 0;
        totalCycleNsec_ = 0;
        recentCycles_.clear();
        recentCycles_.reserve(kRollingWindow);
        status_ = "Running";
    }
    running_ = true;

    // Launch the real-time cycle thread. SCHED_FIFO with elevated priority ensures
    // the kernel scheduler gives this thread precedence over normal processes.
    thread_ = std::thread(&RtTestController::loop, this, cycleUsec);
    return true;
}

void RtTestController::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
    cleanup();
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        status_ = "Stopped";
    }
}

bool RtTestController::running() const {
    return running_;
}

QString RtTestController::status() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return status_;
}

QJsonObject RtTestController::telemetry() const {
    unsigned long long cycles = 0;
    const unsigned long long errors = errorCount_.load();

    int64_t minNs = 0, maxNs = 0, avgNs = 0, jitterNs = 0;
    QJsonArray recent;
    QString statusSnapshot;

    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        cycles = cycleCount_.load();
        minNs = minCycleNsec_;
        maxNs = maxCycleNsec_;
        statusSnapshot = status_;
        if (minNs == INT64_MAX)
            minNs = 0;

        if (cycles > 0) {
            avgNs = totalCycleNsec_ / static_cast<int64_t>(cycles);
            jitterNs = std::max(maxNs - avgNs, avgNs - minNs);
        }

        const int step = 1;
        for (int i = 0; i < static_cast<int>(recentCycles_.size()); i += step) {
            recent.append(static_cast<double>(recentCycles_[i]) / 1000.0);
        }
    }

    const double lossRate = cycles > 0 ? static_cast<double>(errors) / static_cast<double>(cycles) * 100.0 : 0.0;

    QJsonObject obj;
    obj["running"] = running_.load();
    obj["status"] = statusSnapshot;
    obj["cycles"] = static_cast<qint64>(cycles);
    obj["errors"] = static_cast<qint64>(errors);
    obj["lossRate"] = lossRate;
    obj["minUsec"] = static_cast<double>(minNs) / 1000.0;
    obj["maxUsec"] = static_cast<double>(maxNs) / 1000.0;
    obj["avgUsec"] = static_cast<double>(avgNs) / 1000.0;
    obj["jitterUsec"] = static_cast<double>(jitterNs) / 1000.0;
    obj["recent"] = recent;
    return obj;
}

void RtTestController::loop(int cycleUsec) {
    // Elevate to real-time scheduling — failure is non-fatal, just means less deterministic timing.
    struct sched_param param {};
    param.sched_priority = 80;
    if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        // Non-fatal: will still run, just with less deterministic timing.
    }

    // Lock all current and future memory pages to prevent page faults during the RT loop.
    mlockall(MCL_CURRENT | MCL_FUTURE);

    const int64_t cycleNsec = static_cast<int64_t>(cycleUsec) * 1000LL;
    uint64_t wakeupTime = monotonicNsec();
    uint64_t prevTime = wakeupTime;

    while (running_) {
        // Schedule next wake-up using absolute time to avoid drift accumulation.
        wakeupTime += cycleNsec;

        // Sleep until next cycle — clock_nanosleep with TIMER_ABSTIME avoids drift.
        struct timespec wake {};
        wake.tv_sec = static_cast<time_t>(wakeupTime / NsecPerSec);
        wake.tv_nsec = static_cast<long>(wakeupTime % NsecPerSec);
        int sleepErr = 0;
        while ((sleepErr = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake, nullptr)) == EINTR) {
            // Retry on signal interruption.
        }
        if (sleepErr != 0) {
            // EINVAL (invalid time) or another non-EINTR error — break instead
            // of spinning at full CPU.
            running_ = false;
            break;
        }

        if (!running_)
            break;

        // Measure the actual cycle interval (wall-clock time since previous iteration).
        const uint64_t now = monotonicNsec();
        const int64_t cycleDelta = static_cast<int64_t>(now - prevTime);
        prevTime = now;

        // Execute the ecrt receive/process/send cycle — this is what actually exercises the bus.
        ecrt_master_receive(master_);
        if (domainData_) {
            ecrt_domain_process(domain_);
        }
        ecrt_domain_queue(domain_);
        int ret = ecrt_master_send(master_);

        // Track timing statistics.
        {
            std::lock_guard<std::mutex> lock(statsMutex_);
            if (cycleDelta < minCycleNsec_)
                minCycleNsec_ = cycleDelta;
            if (cycleDelta > maxCycleNsec_)
                maxCycleNsec_ = cycleDelta;
            totalCycleNsec_ += cycleDelta;
            if (static_cast<int>(recentCycles_.size()) < kRollingWindow) {
                recentCycles_.push_back(cycleDelta);
            } else {
                recentCycles_[cycleCount_ % kRollingWindow] = cycleDelta;
            }
            ++cycleCount_;
        }

        if (ret < 0) {
            ++errorCount_;
        }
    }

    munlockall();

    // Restore normal scheduling before exiting.
    struct sched_param normal {};
    sched_setscheduler(0, SCHED_OTHER, &normal);
}

void RtTestController::cleanup() {
    if (master_) {
        ecrt_master_deactivate(master_);
        ecrt_release_master(master_);
        master_ = nullptr;
    }
    domain_ = nullptr;
    domainData_ = nullptr;
}
