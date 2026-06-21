#pragma once

// DataCache — specialized cache for SDO/PDO data with optimized access patterns.
// Provides batch operations, prefetching, and statistics.
//
// This service provides specialized caching for EtherCAT SDO/PDO data.
// It handles:
//   - Single key get/put operations
//   - Batch get/put operations for multiple keys
//   - Data prefetching with custom loaders
//   - Cache statistics tracking (hits, misses, evictions, prefetches)
//   - LRU eviction with configurable max entries
//   - TTL-based expiration
//   - Thread-safe access via read-write locks
//
// Usage:
//   DataCache cache(5000, 30000);  // 5000 entries, 30s TTL
//   cache.put("key", data, 60000);  // Cache with 60s TTL
//   QByteArray value;
//   if (cache.get("key", value)) {
//     // Use cached value
//   }
//   // Batch operations
//   QStringList keys = {"key1", "key2", "key3"};
//   QHash<QString, QByteArray> results;
//   cache.batchGet(keys, results);
//   // Prefetch
//   cache.prefetch(keys, [](const QString &key) {
//     return fetchData(key);
//   });
//   CacheStats stats = cache.stats();
//
// Thread safety:
//   All methods are thread-safe. The cache uses QReadWriteLock for
//   concurrent read access and exclusive write access.
//
// Performance:
//   - Get operations are O(1) average case (hash lookup)
//   - Put operations are O(1) average case
//   - Batch operations are O(n) where n is number of keys
//   - Prefetching loads data in background threads
//   - Memory usage is bounded by maxEntries

#include <QObject>
#include <QByteArray>
#include <QHash>
#include <QReadWriteLock>
#include <QElapsedTimer>
#include <QVector>
#include <functional>

// Cache performance statistics.
struct CacheStats {
  int hits = 0;                    // Number of cache hits
  int misses = 0;                  // Number of cache misses
  int evictions = 0;               // Number of evictions (LRU or TTL)
  int prefetches = 0;              // Number of prefetch operations
  qint64 totalAccessTimeUs = 0;   // Total access time in microseconds
};

class DataCache : public QObject {
  Q_OBJECT
public:
  static constexpr int kDefaultMaxEntries = 5000;    // Default maximum entries
  static constexpr int kDefaultTtlMs = 30000;        // Default TTL in milliseconds

  explicit DataCache(int maxEntries = kDefaultMaxEntries,
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

  // Retrieve multiple values from the cache.
  // @param keys     List of keys to look up
  // @param results  Output hash of key-value pairs (only found entries)
  void batchGet(const QStringList &keys, QHash<QString, QByteArray> &results);

  // Store multiple values in the cache.
  // @param entries  Hash of key-value pairs to cache
  // @param ttlMs    Time-to-live in milliseconds (-1 for default)
  void batchPut(const QHash<QString, QByteArray> &entries, int ttlMs = -1);

  // Remove a specific entry from the cache.
  // @param key  Cache key to invalidate
  void invalidate(const QString &key);

  // Remove all entries from the cache.
  void invalidateAll();

  // Prefetch data for multiple keys using a custom loader.
  // @param keys    List of keys to prefetch
  // @param loader  Function to load data for a key
  void prefetch(const QStringList &keys, std::function<QByteArray(const QString &)> loader);

  // Get cache performance statistics.
  // @return CacheStats structure
  CacheStats stats() const;

  // Reset cache performance statistics.
  void resetStats();

signals:
  // Emitted when a cache entry is evicted.
  // @param key  Cache key that was evicted
  void entryEvicted(const QString &key);

  // Emitted when the entire cache is invalidated.
  void cacheInvalidated();

private:
  // Remove expired entries from the cache.
  void evictExpired();

  // Remove the least recently used entry from the cache.
  void evictLru();

  // Internal cache entry structure.
  struct Entry {
    QByteArray data;         // Cached data
    qint64 expiresAt = 0;   // Expiration timestamp (ms since epoch)
    qint64 lastAccess = 0;  // Last access timestamp (ms since epoch)
    int accessCount = 0;    // Number of times accessed
  };

  mutable QReadWriteLock lock_;  // Thread-safe read-write lock
  int maxEntries_;               // Maximum cache entries
  int defaultTtlMs_;             // Default TTL in milliseconds
  QHash<QString, Entry> entries_; // Cache entries by key
  CacheStats stats_;             // Cache performance statistics
};
