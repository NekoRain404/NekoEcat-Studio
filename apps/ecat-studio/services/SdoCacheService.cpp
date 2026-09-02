#include "SdoCacheService.h"

// SdoCacheService.cpp — Thread-safe LRU/LFU cache for SDO dictionary and value entries
//
// Implementation notes:
//   - QReadWriteLock protects concurrent reads and writes
//   - LRU eviction uses std::list splice for O(1) promotion
//   - Per-position dictionaries cached separately from individual values

SdoCacheService::SdoCacheService(QObject* parent) : QObject(parent) {
    clock_.start();
}

QString SdoCacheService::makeKey(const QString& index, const QString& subIndex) const {
    return index + QStringLiteral(":") + subIndex;
}

void SdoCacheService::cacheDictionary(int position, const SdoDictionary& dict) {
    QWriteLocker locker(&lock_);
    dictionaries_[position] = dict;
    emit cacheUpdated(position);
}

SdoDictionary SdoCacheService::getCachedDictionary(int position) const {
    QReadLocker locker(&lock_);
    return dictionaries_.value(position);
}

void SdoCacheService::cacheValue(int position, const QString& index, const QString& subIndex, const QString& value) {
    QWriteLocker locker(&lock_);
    const QString key = makeKey(index, subIndex);
    auto& idx = valueIndex_[position];
    auto& order = valueOrder_[position];

    auto it = idx.find(key);
    if (it != idx.end()) {
        it.value()->entry.value = value;
        it.value()->entry.timestampMs = clock_.elapsed();
        it.value()->entry.accessCount++;
        if (config_.evictionPolicy == SdoCacheConfig::LRU) {
            order.splice(order.begin(), order, it.value());
        }
    } else {
        evictIfNeeded(position);
        LruNode node;
        node.key = key;
        node.entry.value = value;
        node.entry.timestampMs = clock_.elapsed();
        node.entry.accessCount = 1;
        order.push_front(std::move(node));
        idx[key] = order.begin();
    }
    emit cacheUpdated(position);
}

QString SdoCacheService::getCachedValue(int position, const QString& index, const QString& subIndex) const {
    QWriteLocker locker(&lock_);
    const QString key = makeKey(index, subIndex);
    auto posIdx = valueIndex_.constFind(position);
    if (posIdx == valueIndex_.constEnd()) {
        ++const_cast<SdoCacheService*>(this)->misses_;
        return {};
    }
    auto it = posIdx->constFind(key);
    if (it == posIdx->constEnd()) {
        ++const_cast<SdoCacheService*>(this)->misses_;
        return {};
    }

    qint64 now = clock_.elapsed();
    if (config_.ttlMs > 0 && (now - it.value()->entry.timestampMs) > config_.ttlMs) {
        ++const_cast<SdoCacheService*>(this)->misses_;
        return {};
    }

    it.value()->entry.accessCount++;
    if (config_.evictionPolicy == SdoCacheConfig::LRU) {
        auto& order = const_cast<SdoCacheService*>(this)->valueOrder_[position];
        order.splice(order.begin(), order, it.value());
    }

    ++const_cast<SdoCacheService*>(this)->hits_;
    return it.value()->entry.value;
}

void SdoCacheService::invalidateCache(int position) {
    QWriteLocker locker(&lock_);
    dictionaries_.remove(position);
    valueOrder_.remove(position);
    valueIndex_.remove(position);
    emit cacheInvalidated(position);
}

void SdoCacheService::invalidateAll() {
    QWriteLocker locker(&lock_);
    dictionaries_.clear();
    valueOrder_.clear();
    valueIndex_.clear();
    hits_ = 0;
    misses_ = 0;
    emit cacheInvalidated(-1);
}

int SdoCacheService::cachedSlaveCount() const {
    QReadLocker locker(&lock_);
    return valueIndex_.size();
}

int SdoCacheService::cacheSize(int position) const {
    QReadLocker locker(&lock_);
    return valueIndex_.value(position).size();
}

void SdoCacheService::setConfig(const SdoCacheConfig& config) {
    QWriteLocker locker(&lock_);
    config_ = config;
}

void SdoCacheService::evictExpired(int position) {
    auto orderIt = valueOrder_.find(position);
    auto idxIt = valueIndex_.find(position);
    if (orderIt == valueOrder_.end() || idxIt == valueIndex_.end())
        return;

    qint64 now = clock_.elapsed();
    auto& order = orderIt.value();
    auto& idx = idxIt.value();

    auto node = order.rbegin();
    while (node != order.rend() && config_.ttlMs > 0 && (now - node->entry.timestampMs) > config_.ttlMs) {
        idx.remove(node->key);
        auto toErase = std::next(node).base();
        ++node;
        order.erase(toErase);
    }
}

void SdoCacheService::evictIfNeeded(int position) {
    evictExpired(position);
    auto orderIt = valueOrder_.find(position);
    auto idxIt = valueIndex_.find(position);
    if (orderIt == valueOrder_.end() || idxIt == valueIndex_.end())
        return;

    auto& order = orderIt.value();
    auto& idx = idxIt.value();

    while (idx.size() >= config_.maxEntriesPerSlave && !order.empty()) {
        auto& victim = order.back();
        idx.remove(victim.key);
        order.pop_back();
    }
}
