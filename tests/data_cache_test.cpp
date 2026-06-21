// DataCacheTest — Tests for DataCache
//
// Test coverage:
//   - Initial stats state
//   - Put/get and cache miss
//   - Invalidation (single and all)
//   - Batch put/get operations
//   - Prefetch with loader callback
//   - Stats reset

#include <QTest>
#include <QSignalSpy>
#include "services/DataCache.h"

class DataCacheTest : public QObject {
  Q_OBJECT
private slots:
  // Verify cache starts with zero hits, misses, evictions, prefetches
  void testInitialState() {
    DataCache cache;
    CacheStats st = cache.stats();
    QCOMPARE(st.hits, 0);
    QCOMPARE(st.misses, 0);
    QCOMPARE(st.evictions, 0);
    QCOMPARE(st.prefetches, 0);
  }

  // Verify put stores value and get retrieves it with hit count
  void testPutAndGet() {
    DataCache cache;
    cache.put("key1", QByteArray("value1"));
    QByteArray val;
    QVERIFY(cache.get("key1", val));
    QCOMPARE(val, QByteArray("value1"));
    CacheStats st = cache.stats();
    QCOMPARE(st.hits, 1);
    QCOMPARE(st.misses, 0);
  }

  // Verify get on missing key returns false and increments misses
  void testGetMiss() {
    DataCache cache;
    QByteArray val;
    QVERIFY(!cache.get("missing", val));
    CacheStats st = cache.stats();
    QCOMPARE(st.misses, 1);
  }

  // Verify invalidate removes key and emits cacheInvalidated signal
  void testInvalidate() {
    DataCache cache;
    cache.put("key1", QByteArray("val"));
    QSignalSpy spy(&cache, &DataCache::cacheInvalidated);
    cache.invalidate("key1");
    QCOMPARE(spy.count(), 1);
    QByteArray val;
    QVERIFY(!cache.get("key1", val));
  }

  // Verify invalidateAll clears all entries
  void testInvalidateAll() {
    DataCache cache;
    cache.put("a", QByteArray("1"));
    cache.put("b", QByteArray("2"));
    QSignalSpy spy(&cache, &DataCache::cacheInvalidated);
    cache.invalidateAll();
    QCOMPARE(spy.count(), 1);
    QByteArray val;
    QVERIFY(!cache.get("a", val));
    QVERIFY(!cache.get("b", val));
  }

  // Verify batch put stores multiple entries and batchGet retrieves them
  void testBatchPutAndGet() {
    DataCache cache;
    QHash<QString, QByteArray> entries;
    entries["k1"] = QByteArray("v1");
    entries["k2"] = QByteArray("v2");
    entries["k3"] = QByteArray("v3");
    cache.batchPut(entries);
    QStringList keys = {"k1", "k2", "k3", "missing"};
    QHash<QString, QByteArray> results;
    cache.batchGet(keys, results);
    QCOMPARE(results.size(), 3);
    QCOMPARE(results["k1"], QByteArray("v1"));
    QCOMPARE(results["k2"], QByteArray("v2"));
    QCOMPARE(results["k3"], QByteArray("v3"));
  }

  // Verify prefetch loads entries via callback and increments prefetch count
  void testPrefetch() {
    DataCache cache;
    QStringList keys = {"p1", "p2"};
    cache.prefetch(keys, [](const QString &key) -> QByteArray {
      return QByteArray("prefetched_" + key.toUtf8());
    });
    QByteArray val;
    QVERIFY(cache.get("p1", val));
    QCOMPARE(val, QByteArray("prefetched_p1"));
    CacheStats st = cache.stats();
    QCOMPARE(st.prefetches, 2);
  }

  // Verify resetStats zeros out all counters
  void testResetStats() {
    DataCache cache;
    cache.put("k", QByteArray("v"));
    QByteArray val;
    cache.get("k", val);
    cache.get("missing", val);
    cache.resetStats();
    CacheStats st = cache.stats();
    QCOMPARE(st.hits, 0);
    QCOMPARE(st.misses, 0);
  }
};

QTEST_MAIN(DataCacheTest)
#include "data_cache_test.moc"
