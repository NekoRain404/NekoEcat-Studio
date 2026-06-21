#pragma once

// CacheService — generic LRU cache with TTL expiration for EtherCAT data.
// Thread-safe via QReadWriteLock. Supports per-cache-type configuration.
//
// This service provides a generic caching layer for frequently accessed
// EtherCAT data. It handles:
//   - LRU (Least Recently Used) eviction policy
//   - TTL (Time To Live) expiration for cache entries
//   - Per-cache-type configuration (SDO, Topology, PDO, ESI)
//   - Thread-safe access via read-write locks
//   - Cache invalidation (single key or all)
//
// Usage:
//   CacheService cache(1000, 30000);  // 1000 entries, 30s TTL
//   cache.put("key", data, 60000);    // Cache with 60s TTL
//   QByteArray value;
//   if (cache.get("key", value)) {
//     // Use cached value
//   }
//   cache.invalidate("key");          // Remove specific entry
//   cache.invalidateAll();            // Clear entire cache
//
// Thread safety:
//   All methods are thread-safe. The cache uses QReadWriteLock for
//   concurrent read access and exclusive write access.
//
// Performance:
//   - Get operations are O(1) average case (hash lookup)
//   - Put operations are O(1) average case
//   - Eviction is O(1) for LRU removal
//   - Memory usage is bounded by maxSize

#include <QObject>
#include <QString>
#include <QReadWriteLock>
#include <QElapsedTimer>
#include <QHash>
#include <list>

// Represents a single cache entry with expiration time.
struct CacheEntry {
  QByteArray data;         // Cached data
  qint64 expiresAt = 0;   // Expiration timestamp (ms since epoch)
};

class CacheService : public QObject {
  Q_OBJECT
public:
  static constexpr int kDefaultMaxSize = 1000;    // Default maximum cache entries
  static constexpr int kDefaultTtlMs = 30000;     // Default TTL in milliseconds

  explicit CacheService(int maxSize = kDefaultMaxSize,
                        int defaultTtlMs = kDefaultTtlMs,
                        QObject *parent = nullptr);

  // Retrieve a value from the cache.
  // @param key    Cache key to look up
  // @param value  Output parameter for the cached value
  // @return true if the key exists and is not expired
  bool get(const QString &key, QByteArray &value);

  // Store a value in the cache.
  // @param key     Cache key
  // @param value   Value to cache
  // @param ttlMs   Time-to-live in milliseconds (-1 for default)
  void put(const QString &key, const QByteArray &value, int ttlMs = -1);

  // Remove a specific entry from the cache.
  // @param key  Cache key to invalidate
  void invalidate(const QString &key);

  // Remove all entries from the cache.
  void invalidateAll();

  // Get the current number of entries in the cache.
  // @return Number of cached entries
  int size() const;

  // Get the maximum number of entries the cache can hold.
  // @return Maximum cache size
  int maxSize() const;

  // Check if a key exists in the cache (without retrieving the value).
  // @param key  Cache key to check
  // @return true if the key exists and is not expired
  bool contains(const QString &key) const;

  // Configure cache settings for SDO data.
  // @param maxSize  Maximum number of SDO entries
  // @param ttlMs    TTL for SDO entries in milliseconds
  void setSdoCacheConfig(int maxSize, int ttlMs);

  // Configure cache settings for topology data.
  // @param maxSize  Maximum number of topology entries
  // @param ttlMs    TTL for topology entries in milliseconds
  void setTopologyCacheConfig(int maxSize, int ttlMs);

  // Configure cache settings for PDO data.
  // @param maxSize  Maximum number of PDO entries
  // @param ttlMs    TTL for PDO entries in milliseconds
  void setPdoCacheConfig(int maxSize, int ttlMs);

  // Configure cache settings for ESI data.
  // @param maxSize  Maximum number of ESI entries
  // @param ttlMs    TTL for ESI entries in milliseconds
  void setEsiCacheConfig(int maxSize, int ttlMs);

signals:
  // Emitted when a cache entry is evicted (due to LRU or TTL expiration).
  // @param key  Cache key that was evicted
  void entryEvicted(const QString &key);

  // Emitted when the entire cache is invalidated.
  void cacheInvalidated();

private:
  // Remove expired entries from the cache.
  void evictExpired();

  // Remove the least recently used entry from the cache.
  void evictLru();

  // Internal node structure for the LRU list.
  struct Node {
    QString key;         // Cache key
    CacheEntry entry;    // Cached data with expiration
  };

  mutable QReadWriteLock lock_;  // Thread-safe read-write lock
  int maxSize_;                  // Maximum cache entries
  int defaultTtlMs_;             // Default TTL in milliseconds

  std::list<Node> order_;                           // LRU order (front = most recent)
  QHash<QString, std::list<Node>::iterator> index_; // Key to iterator mapping
};
