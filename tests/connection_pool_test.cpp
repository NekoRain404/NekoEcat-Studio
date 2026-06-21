// ConnectionPoolTest — Tests for ConnectionPool
//
// Test coverage:
//   - Initial state and configuration
//   - Connection acquire/release behavior
//   - Pool exhaustion signaling
//   - Health status checks
//   - Signal validity

#include <QTest>
#include <QSignalSpy>
#include "services/ConnectionPool.h"

class ConnectionPoolTest : public QObject {
  Q_OBJECT
private slots:
  // Verify pool starts with zero connections and healthy status
  void testInitialState() {
    ConnectionConfig cfg;
    cfg.maxConnections = 4;
    ConnectionPool pool(cfg);
    QCOMPARE(pool.activeConnections(), 0);
    QCOMPARE(pool.availableConnections(), 0);
    QVERIFY(pool.isHealthy());
  }

  // Verify custom port, max connections, and timeout are accepted
  void testCustomConfig() {
    ConnectionConfig cfg;
    cfg.port = 9999;
    cfg.maxConnections = 2;
    cfg.connectionTimeoutMs = 100;
    ConnectionPool pool(cfg);
    QCOMPARE(pool.activeConnections(), 0);
    QCOMPARE(pool.availableConnections(), 0);
  }

  // Acquire returns nullptr and emits poolExhausted when no server is available
  void testAcquireNoServer() {
    ConnectionConfig cfg;
    cfg.port = 19999;
    cfg.connectionTimeoutMs = 50;
    ConnectionPool pool(cfg);
    QSignalSpy spy(&pool, &ConnectionPool::poolExhausted);
    QTcpSocket *sock = pool.acquire();
    QVERIFY(sock == nullptr);
    QCOMPARE(spy.count(), 1);
  }

  // Releasing nullptr does not change active connection count
  void testReleaseNullptr() {
    ConnectionConfig cfg;
    cfg.connectionTimeoutMs = 50;
    ConnectionPool pool(cfg);
    pool.release(nullptr);
    QCOMPARE(pool.activeConnections(), 0);
  }

  // Pool reports healthy when max connections capacity is set
  void testIsHealthyWithCapacity() {
    ConnectionConfig cfg;
    cfg.maxConnections = 2;
    cfg.connectionTimeoutMs = 50;
    ConnectionPool pool(cfg);
    QVERIFY(pool.isHealthy());
  }

  // Repeated acquire calls emit poolExhausted signal each time
  void testAcquireExhaustsPool() {
    ConnectionConfig cfg;
    cfg.maxConnections = 1;
    cfg.port = 19999;
    cfg.connectionTimeoutMs = 50;
    ConnectionPool pool(cfg);
    QSignalSpy spy(&pool, &ConnectionPool::poolExhausted);
    pool.acquire();
    QCOMPARE(spy.count(), 1);
    pool.acquire();
    QCOMPARE(spy.count(), 2);
  }

  // All expected signals (acquired, released, error, exhausted) are valid
  void testSignalsExist() {
    ConnectionConfig cfg;
    cfg.connectionTimeoutMs = 50;
    ConnectionPool pool(cfg);
    QSignalSpy acqSpy(&pool, &ConnectionPool::connectionAcquired);
    QSignalSpy relSpy(&pool, &ConnectionPool::connectionReleased);
    QSignalSpy errSpy(&pool, &ConnectionPool::connectionError);
    QSignalSpy exhSpy(&pool, &ConnectionPool::poolExhausted);
    QVERIFY(acqSpy.isValid());
    QVERIFY(relSpy.isValid());
    QVERIFY(errSpy.isValid());
    QVERIFY(exhSpy.isValid());
  }
};

QTEST_MAIN(ConnectionPoolTest)
#include "connection_pool_test.moc"
