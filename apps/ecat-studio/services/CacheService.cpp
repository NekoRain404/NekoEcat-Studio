#include "CacheService.h"

// CacheService.cpp — Generic LRU cache with TTL-based expiration
//
// Implementation notes:
//   - Doubly-linked list + QHash index for O(1) get/put/evict
//   - Thread-safe via QReadWriteLocker on every public method
//   - Evicts expired entries first, then LRU tail when at capacity

static qint64 nowMs() {
  QElapsedTimer t;
  t.start();
  return t.msecsSinceReference();
}

CacheService::CacheService(int maxSize, int defaultTtlMs, QObject *parent)
    : QObject(parent), maxSize_(maxSize), defaultTtlMs_(defaultTtlMs) {}

bool CacheService::get(const QString &key, QByteArray &value) {
  QWriteLocker locker(&lock_);

  auto it = index_.find(key);
  if (it == index_.end()) return false;

  auto listIt = *it;
  if (listIt->entry.expiresAt > 0 && nowMs() > listIt->entry.expiresAt) {
    order_.erase(listIt);
    index_.erase(it);
    return false;
  }

  order_.splice(order_.begin(), order_, listIt);
  value = listIt->entry.data;
  return true;
}

// Inserts or replaces an entry; evicts expired/LRU entries if at capacity
void CacheService::put(const QString &key, const QByteArray &value, int ttlMs) {
  QWriteLocker locker(&lock_);

  auto it = index_.find(key);
  if (it != index_.end()) {
    order_.erase(*it);
    index_.erase(it);
  }

  if (static_cast<int>(index_.size()) >= maxSize_) {
    evictExpired();
    while (static_cast<int>(index_.size()) >= maxSize_) {
      evictLru();
    }
  }

  Node node;
  node.key = key;
  node.entry.data = value;
  int ttl = (ttlMs >= 0) ? ttlMs : defaultTtlMs_;
  if (ttl > 0) {
    node.entry.expiresAt = nowMs() + ttl;
  }

  order_.push_front(node);
  index_[key] = order_.begin();
}

void CacheService::invalidate(const QString &key) {
  QWriteLocker locker(&lock_);
  auto it = index_.find(key);
  if (it != index_.end()) {
    order_.erase(*it);
    index_.erase(it);
  }
}

void CacheService::invalidateAll() {
  QWriteLocker locker(&lock_);
  order_.clear();
  index_.clear();
  emit cacheInvalidated();
}

int CacheService::size() const {
  QReadLocker locker(&lock_);
  return static_cast<int>(index_.size());
}

int CacheService::maxSize() const { return maxSize_; }

bool CacheService::contains(const QString &key) const {
  QReadLocker locker(&lock_);
  return index_.contains(key);
}

// Scans the list front-to-back and removes all TTL-expired entries
void CacheService::evictExpired() {
  qint64 now = nowMs();
  auto it = order_.begin();
  while (it != order_.end()) {
    if (it->entry.expiresAt > 0 && now > it->entry.expiresAt) {
      index_.remove(it->key);
      it = order_.erase(it);
    } else {
      ++it;
    }
  }
}

// Removes the least-recently-used entry (list tail) and emits entryEvicted
void CacheService::evictLru() {
  if (order_.empty()) return;
  const QString &key = order_.back().key;
  index_.remove(key);
  order_.pop_back();
  emit entryEvicted(key);
}

void CacheService::setSdoCacheConfig(int maxSize, int ttlMs) {
  Q_UNUSED(maxSize);
  Q_UNUSED(ttlMs);
}

void CacheService::setTopologyCacheConfig(int maxSize, int ttlMs) {
  Q_UNUSED(maxSize);
  Q_UNUSED(ttlMs);
}

void CacheService::setPdoCacheConfig(int maxSize, int ttlMs) {
  Q_UNUSED(maxSize);
  Q_UNUSED(ttlMs);
}

void CacheService::setEsiCacheConfig(int maxSize, int ttlMs) {
  Q_UNUSED(maxSize);
  Q_UNUSED(ttlMs);
}
