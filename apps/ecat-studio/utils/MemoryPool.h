#pragma once

// MemoryPool — fixed-size object pool for frequent allocations.
// Thread-safe. Provides statistics for monitoring.

#include <atomic>
#include <cstddef>
#include <new>
#include <QHash>
#include <QMutex>
#include <QVector>

template <typename T> class MemoryPool {
public:
    static constexpr int kDefaultPoolSize = 256;

    explicit MemoryPool(int poolSize = kDefaultPoolSize) : capacity_(poolSize) {
        pool_.reserve(capacity_);
        for (int i = 0; i < capacity_; ++i) {
            pool_.append(new T());
        }
        freeList_.resize(capacity_);
        for (int i = 0; i < capacity_; ++i) {
            freeList_[i] = i;
        }
        freeCount_ = capacity_;
    }

    ~MemoryPool() {
        for (T* obj : pool_) {
            delete obj;
        }
    }

    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    T* allocate() {
        QMutexLocker locker(&mutex_);
        ++stats_.totalAllocations;
        if (freeCount_ == 0) {
            ++stats_.overflowCount;
            return new T();
        }

        --freeCount_;
        int idx = freeList_[freeCount_];
        T* obj = pool_[idx];
        allocated_[obj] = idx;

        int currentUsed = capacity_ - freeCount_;
        if (currentUsed > stats_.peakUsage) {
            stats_.peakUsage = currentUsed;
        }
        return obj;
    }

    void deallocate(T* ptr) {
        if (!ptr)
            return;

        QMutexLocker locker(&mutex_);
        auto it = allocated_.find(ptr);
        if (it != allocated_.end()) {
            freeList_[freeCount_] = it.value();
            ++freeCount_;
            allocated_.erase(it);
            ++stats_.totalDeallocations;
        } else {
            delete ptr;
            ++stats_.externalDeallocations;
        }
    }

    int capacity() const { return capacity_; }
    int available() const {
        QMutexLocker locker(&mutex_);
        return freeCount_;
    }

    struct Stats {
        int totalAllocations = 0;
        int totalDeallocations = 0;
        int peakUsage = 0;
        int overflowCount = 0;
        int externalDeallocations = 0;
    };

    Stats stats() const {
        QMutexLocker locker(&mutex_);
        return stats_;
    }

    void resetStats() {
        QMutexLocker locker(&mutex_);
        stats_ = Stats();
    }

private:
    mutable QMutex mutex_;
    int capacity_;
    int freeCount_ = 0;
    QVector<T*> pool_;
    QVector<int> freeList_;
    QHash<T*, int> allocated_;
    Stats stats_;
};
