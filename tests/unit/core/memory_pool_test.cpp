// MemoryPoolTest — Tests for MemoryPool
//
// Test coverage:
//   - Allocate and deallocate lifecycle
//   - Pool exhaustion and overflow allocation
//   - Statistics tracking and reset
//   - Capacity and available counts

#include "utils/MemoryPool.h"

#include <QCoreApplication>
#include <QVector>
#include <QString>

#include <cstdlib>
#include <iostream>

namespace {

struct TestObj {
  int value = 0;
  QString label;
};

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

// Allocate returns usable object, deallocate restores availability
// Test basic allocate and deallocate cycle
void testAllocateDeallocate() {
  MemoryPool<TestObj> pool(10);
  TestObj *obj = pool.allocate();
  expectTrue(obj != nullptr, "allocate returns non-null");
  obj->value = 42;
  expectEqual(obj->value, 42, "allocated object is usable");
  pool.deallocate(obj);
  expectEqual(pool.available(), 10, "available restored after deallocate");
}

// Pool exhaustion triggers overflow allocation and tracks count
// Test pool exhaustion triggers overflow allocation
void testPoolExhaustion() {
  MemoryPool<TestObj> pool(3);
  QVector<TestObj *> objs;
  for (int i = 0; i < 3; ++i) {
    objs.append(pool.allocate());
  }
  expectEqual(pool.available(), 0, "pool exhausted after 3 allocations");

  TestObj *overflow = pool.allocate();
  expectTrue(overflow != nullptr, "overflow allocation returns non-null");
  auto s = pool.stats();
  expectEqual(s.overflowCount, 1, "overflow count is 1");

  pool.deallocate(overflow);
  for (TestObj *o : objs) pool.deallocate(o);
}

// Statistics track total allocations and peak usage
// Test statistics tracking for allocations and peak usage
void testStats() {
  MemoryPool<TestObj> pool(5);
  QVector<TestObj *> objs;
  for (int i = 0; i < 7; ++i) {
    objs.append(pool.allocate());
  }
  auto s = pool.stats();
  expectEqual(s.totalAllocations, 7, "totalAllocations is 7");
  expectEqual(s.peakUsage, 5, "peakUsage capped at pool capacity");
  // Release all allocations so the pool has no leaked blocks.
  for (TestObj *o : objs) pool.deallocate(o);
}

// Capacity and available return configured values
// Test capacity and available return configured values
void testCapacity() {
  MemoryPool<TestObj> pool(100);
  expectEqual(pool.capacity(), 100, "capacity returns configured value");
  expectEqual(pool.available(), 100, "initial available equals capacity");
}

// Reset stats clears allocation counters
// Test resetStats clears allocation counters
void testResetStats() {
  MemoryPool<TestObj> pool(5);
  pool.allocate();
  pool.resetStats();
  auto s = pool.stats();
  expectEqual(s.totalAllocations, 0, "resetStats clears allocations");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testAllocateDeallocate();
  testPoolExhaustion();
  testStats();
  testCapacity();
  testResetStats();
  return 0;
}
