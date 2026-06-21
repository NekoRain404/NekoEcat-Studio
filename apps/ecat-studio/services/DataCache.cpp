#include "DataCache.h"
#include <QDateTime>

// DataCache.cpp — Thread-safe key-value cache with TTL, prefetch, and batch ops
//
// Implementation notes:
//   - QHash-backed with QReadWriteLock for concurrent read access
//   - Tracks hit/miss/eviction stats for performance monitoring
//   - Supports batch get/put and async prefetch with caller-supplied loader

DataCache::DataCache(int maxEntries, int defaultTtlMs, QObject *parent)
    : QObject(parent), maxEntries_(maxEntries), defaultTtlMs_(defaultTtlMs) {
}

bool DataCache::get(const QString &key, QByteArray &value) {
  QElapsedTimer timer;
  timer.start();

  QWriteLocker locker(&lock_);

  auto it = entries_.find(key);
  if (it == entries_.end()) {
    ++stats_.misses;
    stats_.totalAccessTimeUs += timer.nsecsElapsed() / 1000;
    return false;
  }

  if (it->expiresAt > 0 && QDateTime::currentMSecsSinceEpoch() > it->expiresAt) {
    entries_.erase(it);
    ++stats_.misses;
    ++stats_.evictions;
    stats_.totalAccessTimeUs += timer.nsecsElapsed() / 1000;
    return false;
  }

  value = it->data;
  it->lastAccess = QDateTime::currentMSecsSinceEpoch();
  ++it->accessCount;
  ++stats_.hits;
  stats_.totalAccessTimeUs += timer.nsecsElapsed() / 1000;
  return true;
}

void DataCache::put(const QString &key, const QByteArray &value, int ttlMs) {
  QWriteLocker locker(&lock_);

  evictExpired();
  if (entries_.size() >= maxEntries_) {
    evictLru();
  }

  Entry entry;
  entry.data = value;
  entry.lastAccess = QDateTime::currentMSecsSinceEpoch();
  entry.expiresAt = (ttlMs > 0 || defaultTtlMs_ > 0)
      ? QDateTime::currentMSecsSinceEpoch() + (ttlMs > 0 ? ttlMs : defaultTtlMs_)
      : 0;
  entries_[key] = entry;
}

// Retrieves multiple keys in a single lock acquisition, skipping expired entries
void DataCache::batchGet(const QStringList &keys, QHash<QString, QByteArray> &results) {
  QWriteLocker locker(&lock_);

  qint64 now = QDateTime::currentMSecsSinceEpoch();
  for (const auto &key : keys) {
    auto it = entries_.find(key);
    if (it == entries_.end()) {
      ++stats_.misses;
      continue;
    }

    if (it->expiresAt > 0 && now > it->expiresAt) {
      entries_.erase(it);
      ++stats_.misses;
      ++stats_.evictions;
      continue;
    }

    results[key] = it->data;
    it->lastAccess = now;
    ++it->accessCount;
    ++stats_.hits;
  }
}

void DataCache::batchPut(const QHash<QString, QByteArray> &entries, int ttlMs) {
  QWriteLocker locker(&lock_);

  evictExpired();
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  int effectiveTtl = (ttlMs > 0) ? ttlMs : defaultTtlMs_;

  for (auto it = entries.begin(); it != entries.end(); ++it) {
    if (entries_.size() >= maxEntries_) {
      evictLru();
    }

    Entry entry;
    entry.data = it.value();
    entry.lastAccess = now;
    entry.expiresAt = effectiveTtl > 0 ? now + effectiveTtl : 0;
    entries_[it.key()] = entry;
  }
}

void DataCache::invalidate(const QString &key) {
  QWriteLocker locker(&lock_);
  entries_.remove(key);
  emit cacheInvalidated();
}

void DataCache::invalidateAll() {
  QWriteLocker locker(&lock_);
  entries_.clear();
  emit cacheInvalidated();
}

// Pre-populates cache for keys not yet present using the caller-supplied loader
void DataCache::prefetch(const QStringList &keys, std::function<QByteArray(const QString &)> loader) {
  QWriteLocker locker(&lock_);

  qint64 now = QDateTime::currentMSecsSinceEpoch();
  int effectiveTtl = defaultTtlMs_ > 0 ? defaultTtlMs_ : 30000;

  for (const auto &key : keys) {
    auto it = entries_.find(key);
    if (it != entries_.end() && (it->expiresAt == 0 || now <= it->expiresAt)) {
      continue;
    }

    QByteArray data = loader(key);
    if (!data.isEmpty()) {
      if (entries_.size() >= maxEntries_) {
        evictLru();
      }

      Entry entry;
      entry.data = data;
      entry.lastAccess = now;
      entry.expiresAt = now + effectiveTtl;
      entries_[key] = entry;
      ++stats_.prefetches;
    }
  }
}

CacheStats DataCache::stats() const {
  QReadLocker locker(&lock_);
  return stats_;
}

void DataCache::resetStats() {
  QWriteLocker locker(&lock_);
  stats_ = CacheStats();
}

// Scans all entries and removes those past their TTL, updating eviction stats
void DataCache::evictExpired() {
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  auto it = entries_.begin();
  while (it != entries_.end()) {
    if (it->expiresAt > 0 && now > it->expiresAt) {
      it = entries_.erase(it);
      ++stats_.evictions;
      emit entryEvicted(it.key());
    } else {
      ++it;
    }
  }
}

// Finds and removes the entry with the oldest lastAccess timestamp
void DataCache::evictLru() {
  if (entries_.isEmpty()) return;

  auto oldest = entries_.begin();
  for (auto it = entries_.begin(); it != entries_.end(); ++it) {
    if (it->lastAccess < oldest->lastAccess) {
      oldest = it;
    }
  }

  QString key = oldest.key();
  entries_.erase(oldest);
  ++stats_.evictions;
  emit entryEvicted(key);
}
