// CacheServiceTest — Tests for CacheService
//
// Test coverage:
//   - Basic put/get operations
//   - Overwrite existing keys
//   - Invalidate single key
//   - Invalidate all entries
//   - LRU eviction policy
//   - TTL expiration

#include "services/CacheService.h"

#include <QCoreApplication>
#include <QThread>
#include <QElapsedTimer>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) fail(message);
}

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected)
    fail(QString("%1: expected %2, got %3").arg(message).arg(expected).arg(actual));
}

void expectEqual(const QByteArray &actual, const QByteArray &expected,
                 const QString &message) {
  if (actual != expected)
    fail(QString("%1: expected '%2', got '%3'")
             .arg(message, QString::fromUtf8(expected), QString::fromUtf8(actual)));
}

void testBasicPutGet() {
  CacheService cache(100, 30000);
  cache.put("key1", "value1");
  QByteArray val;
  expectTrue(cache.get("key1", val), "get returns true for existing key");
  expectEqual(val, QByteArray("value1"), "get returns correct value");
  expectEqual(cache.size(), 1, "size is 1 after one put");
}

void testOverwrite() {
  CacheService cache(100, 30000);
  cache.put("key1", "old");
  cache.put("key1", "new");
  QByteArray val;
  expectTrue(cache.get("key1", val), "overwrite: key still exists");
  expectEqual(val, QByteArray("new"), "overwrite: value updated");
  expectEqual(cache.size(), 1, "overwrite: size unchanged");
}

void testInvalidate() {
  CacheService cache(100, 30000);
  cache.put("key1", "value1");
  cache.invalidate("key1");
  QByteArray val;
  expectTrue(!cache.get("key1", val), "invalidate: key removed");
  expectEqual(cache.size(), 0, "invalidate: size is 0");
}

void testInvalidateAll() {
  CacheService cache(100, 30000);
  cache.put("a", "1");
  cache.put("b", "2");
  cache.invalidateAll();
  expectEqual(cache.size(), 0, "invalidateAll: size is 0");
}

void testLruEviction() {
  CacheService cache(2, 30000);
  cache.put("a", "1");
  cache.put("b", "2");
  cache.put("c", "3");
  expectEqual(cache.size(), 2, "lru: evicts oldest when full");
  QByteArray val;
  expectTrue(!cache.get("a", val), "lru: oldest entry evicted");
  expectTrue(cache.get("b", val), "lru: second entry still exists");
  expectTrue(cache.get("c", val), "lru: newest entry still exists");
}

void testLruAccessOrder() {
  CacheService cache(2, 30000);
  cache.put("a", "1");
  cache.put("b", "2");
  QByteArray val;
  cache.get("a", val);
  cache.put("c", "3");
  expectTrue(cache.get("a", val), "lru-access: accessed entry survives eviction");
  expectTrue(!cache.get("b", val), "lru-access: untouched entry evicted");
}

void testTtlExpiration() {
  CacheService cache(100, 50);
  cache.put("key1", "value1", 50);
  QByteArray val;
  expectTrue(cache.get("key1", val), "ttl: entry exists before expiry");
  QThread::msleep(100);
  expectTrue(!cache.get("key1", val), "ttl: entry expired after ttl");
}

void testNoTtl() {
  CacheService cache(100, 0);
  cache.put("key1", "value1", 0);
  QThread::msleep(50);
  QByteArray val;
  expectTrue(cache.get("key1", val), "no-ttl: entry persists without expiry");
}

void testContains() {
  CacheService cache(100, 30000);
  cache.put("key1", "value1");
  expectTrue(cache.contains("key1"), "contains: true for existing key");
  expectTrue(!cache.contains("key2"), "contains: false for missing key");
}

void testMaxSize() {
  CacheService cache(500, 30000);
  expectEqual(cache.maxSize(), 500, "maxSize returns configured value");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testBasicPutGet();
  testOverwrite();
  testInvalidate();
  testInvalidateAll();
  testLruEviction();
  testLruAccessOrder();
  testTtlExpiration();
  testNoTtl();
  testContains();
  testMaxSize();
  return 0;
}
