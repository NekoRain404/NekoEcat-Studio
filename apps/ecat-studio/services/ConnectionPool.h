#pragma once

// ConnectionPool — manages TCP connections to ecatd daemon.
// Provides connection reuse, health checking, and automatic reconnection.
//
// This service provides TCP connection pooling for efficient daemon
// communication. It handles:
//   - Connection creation and management
//   - Connection reuse for multiple requests
//   - Health checking at configurable intervals
//   - Automatic reconnection on connection loss
//   - Thread-safe connection acquisition and release
//   - Connection pool exhaustion handling
//
// Usage:
//   ConnectionConfig config;
//   config.host = QHostAddress::LocalHost;
//   config.port = 5877;
//   config.maxConnections = 4;
//   ConnectionPool pool(config);
//   QTcpSocket *conn = pool.acquire();
//   // Use connection
//   pool.release(conn);
//   int active = pool.activeConnections();
//   int available = pool.availableConnections();
//
// Thread safety:
//   All methods are thread-safe. The pool uses QMutex for concurrent
//   access protection.
//
// Performance:
//   - Connection acquisition is O(1) when connections are available
//   - Connection creation is O(1) for new connections
//   - Health checking is O(n) where n is number of connections
//   - Connection reuse reduces TCP overhead

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>
#include <QTimer>
#include <QHostAddress>
#include <QVector>

// Connection pool configuration.
struct ConnectionConfig {
  QHostAddress host = QHostAddress::LocalHost;  // Daemon host address
  quint16 port = 5877;                         // Daemon port
  int maxConnections = 4;                       // Maximum pool size
  int connectionTimeoutMs = 5000;               // Connection timeout (ms)
  int healthCheckIntervalMs = 30000;            // Health check interval (ms)
  int maxRetries = 3;                           // Maximum connection retries
};

class ConnectionPool : public QObject {
  Q_OBJECT
public:
  explicit ConnectionPool(const ConnectionConfig &config = ConnectionConfig(),
                          QObject *parent = nullptr);
  ~ConnectionPool() override;

  // Acquire a connection from the pool.
  // @return QTcpSocket pointer, or nullptr if pool is exhausted
  QTcpSocket *acquire();

  // Release a connection back to the pool.
  // @param connection  Connection to release
  void release(QTcpSocket *connection);

  // Get the number of active (acquired) connections.
  // @return Number of active connections
  int activeConnections() const;

  // Get the number of available (released) connections.
  // @return Number of available connections
  int availableConnections() const;

  // Check if the pool is healthy (has available connections).
  // @return true if pool is healthy
  bool isHealthy() const;

signals:
  // Emitted when a connection is acquired from the pool.
  // @param connection  The acquired connection
  void connectionAcquired(QTcpSocket *connection);

  // Emitted when a connection is released back to the pool.
  // @param connection  The released connection
  void connectionReleased(QTcpSocket *connection);

  // Emitted when a connection error occurs.
  // @param error  Human-readable error message
  void connectionError(const QString &error);

  // Emitted when the pool is exhausted (no available connections).
  void poolExhausted();

private slots:
  // Perform health check on all connections.
  void performHealthCheck();

  // Handle connection disconnection.
  void onConnectionDisconnected();

private:
  // Create a new TCP connection.
  QTcpSocket *createConnection();

  // Destroy a TCP connection.
  void destroyConnection(QTcpSocket *connection);

  ConnectionConfig config_;                // Pool configuration
  QVector<QTcpSocket *> available_;        // Available (released) connections
  QVector<QTcpSocket *> active_;           // Active (acquired) connections
  mutable QMutex mutex_;                   // Thread-safe mutex
  QTimer *healthCheckTimer_;               // Timer for health checks
  int totalCreated_ = 0;                   // Total connections created
};
