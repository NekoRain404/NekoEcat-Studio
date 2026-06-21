#pragma once

// SdoCacheService — per-slave SDO dictionary/value/metadata cache with TTL and eviction.
//
// This service provides specialized caching for SDO data with per-slave
// isolation. It handles:
//   - SDO dictionary caching per slave
//   - SDO value caching with TTL expiration
//   - Multiple eviction policies (LRU, LFU, FIFO)
//   - Cache statistics tracking (hits, misses)
//   - Per-slave cache invalidation
//   - Thread-safe access via read-write locks
//
// Usage:
//   SdoCacheService cache;
//   SdoCacheConfig config;
//   config.maxEntriesPerSlave = 2048;
//   config.ttlMs = 60000;
//   config.evictionPolicy = SdoCacheConfig::LRU;
//   cache.setConfig(config);
//   // Cache dictionary
//   SdoDictionary dict;
//   dict << SdoDictionaryEntry{"0x6000", "0x01", "Velocity", "INT16", 16, "ro"};
//   cache.cacheDictionary(0, dict);
//   // Cache value
//   cache.cacheValue(0, "0x6000", "0x01", "1000");
//   QString value = cache.getCachedValue(0, "0x6000", "0x01");
//   qint64 hits = cache.hitCount();
//   qint64 misses = cache.missCount();
//
// Thread safety:
//   All methods are thread-safe. The cache uses QReadWriteLock for
//   concurrent read access and exclusive write access.
//
// Performance:
//   - Dictionary lookup is O(1) by slave position
//   - Value lookup is O(1) average case (hash lookup)
//   - Eviction is O(1) for LRU/LFU/FIFO
//   - Memory usage is bounded by maxEntriesPerSlave * slave count

#include <QObject>
#include <QString>
#include <QHash>
#include <QVector>
#include <QReadWriteLock>
#include <QElapsedTimer>
#include <list>

// Represents a single SDO dictionary entry.
struct SdoDictionaryEntry {
  QString index;          // SDO index in hex format
  QString subIndex;       // SDO subindex in hex format
  QString name;           // SDO name
  QString dataType;       // Data type (e.g., "INT16", "UINT8")
  int bitSize = 0;        // Bit size of the data
  QString accessType;     // Access type (ro, wo, rw)
  QString defaultValue;   // Default value
  QString description;    // Human-readable description
};

// SDO dictionary for a slave.
using SdoDictionary = QVector<SdoDictionaryEntry>;

// Represents a cached SDO value.
struct SdoCacheEntry {
  QString value;            // Cached value
  qint64 timestampMs = 0;   // Cache timestamp (ms since epoch)
  qint64 accessCount = 0;   // Number of times accessed
};

// Cache configuration.
struct SdoCacheConfig {
  int maxEntriesPerSlave = 2048;  // Maximum entries per slave
  int ttlMs = 60000;              // Time-to-live in milliseconds
  
  // Eviction policy enumeration.
  enum EvictionPolicy { 
    LRU,   // Least Recently Used
    LFU,   // Least Frequently Used
    FIFO   // First In, First Out
  };
  EvictionPolicy evictionPolicy = LRU;  // Default eviction policy
};

class SdoCacheService : public QObject {
  Q_OBJECT
public:
  explicit SdoCacheService(QObject *parent = nullptr);

  // Cache an SDO dictionary for a slave.
  // @param position  Slave position
  // @param dict      SDO dictionary to cache
  void cacheDictionary(int position, const SdoDictionary &dict);

  // Get the cached SDO dictionary for a slave.
  // @param position  Slave position
  // @return SdoDictionary (empty if not cached)
  SdoDictionary getCachedDictionary(int position) const;

  // Cache an SDO value for a slave.
  // @param position  Slave position
  // @param index     SDO index in hex format
  // @param subIndex  SDO subindex in hex format
  // @param value     Value to cache
  void cacheValue(int position, const QString &index,
                  const QString &subIndex, const QString &value);

  // Get a cached SDO value for a slave.
  // @param position  Slave position
  // @param index     SDO index in hex format
  // @param subIndex  SDO subindex in hex format
  // @return Cached value (empty if not cached or expired)
  QString getCachedValue(int position, const QString &index,
                         const QString &subIndex) const;

  // Invalidate all cache entries for a slave.
  // @param position  Slave position
  void invalidateCache(int position);

  // Invalidate all cache entries for all slaves.
  void invalidateAll();

  // Get the number of slaves with cached data.
  // @return Number of cached slaves
  int cachedSlaveCount() const;

  // Get the cache size for a specific slave.
  // @param position  Slave position
  // @return Number of cached entries
  int cacheSize(int position) const;

  // Get the total cache hit count.
  // @return Number of cache hits
  qint64 hitCount() const { return hits_; }

  // Get the total cache miss count.
  // @return Number of cache misses
  qint64 missCount() const { return misses_; }

  // Set the cache configuration.
  // @param config  SdoCacheConfig structure
  void setConfig(const SdoCacheConfig &config);

  // Get the current cache configuration.
  // @return SdoCacheConfig structure
  SdoCacheConfig config() const { return config_; }

signals:
  // Emitted when cache is updated for a slave.
  // @param position  Slave position
  void cacheUpdated(int position);

  // Emitted when cache is invalidated for a slave.
  // @param position  Slave position
  void cacheInvalidated(int position);

private:
  // Internal LRU node structure.
  struct LruNode {
    QString key;           // Cache key
    SdoCacheEntry entry;   // Cached entry
  };

  // Remove expired entries for a slave.
  void evictExpired(int position);

  // Evict entries if cache exceeds max size.
  void evictIfNeeded(int position);

  // Create a cache key from index and subindex.
  QString makeKey(const QString &index, const QString &subIndex) const;

  mutable QReadWriteLock lock_;  // Thread-safe read-write lock
  SdoCacheConfig config_;        // Cache configuration
  
  // Per-slave cache storage
  QHash<int, SdoDictionary> dictionaries_;  // Cached dictionaries
  QHash<int, std::list<LruNode>> valueOrder_;  // LRU order per slave
  QHash<int, QHash<QString, std::list<LruNode>::iterator>> valueIndex_;  // Key to iterator mapping
  
  QElapsedTimer clock_;    // Timer for timestamps
  qint64 hits_ = 0;       // Total cache hits
  qint64 misses_ = 0;     // Total cache misses
};
