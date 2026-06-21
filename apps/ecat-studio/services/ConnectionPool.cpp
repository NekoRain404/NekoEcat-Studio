#include "ConnectionPool.h"

// ConnectionPool.cpp — Bounded TCP socket pool with health-check reaping
//
// Implementation notes:
//   - Maintains separate available/active lists with mutex-guarded access
//   - Creates connections on demand up to config_.maxConnections
//   - Periodic health check reaps disconnected sockets from the available list

ConnectionPool::ConnectionPool(const ConnectionConfig &config, QObject *parent)
    : QObject(parent), config_(config) {
  healthCheckTimer_ = new QTimer(this);
  connect(healthCheckTimer_, &QTimer::timeout, this, &ConnectionPool::performHealthCheck);
  healthCheckTimer_->start(config_.healthCheckIntervalMs);
}

ConnectionPool::~ConnectionPool() {
  QMutexLocker locker(&mutex_);
  for (auto *conn : available_) {
    conn->disconnect();
    conn->deleteLater();
  }
  for (auto *conn : active_) {
    conn->disconnect();
    conn->deleteLater();
  }
}

// Returns a pooled socket or creates a new one; returns nullptr if pool exhausted
QTcpSocket *ConnectionPool::acquire() {
  QMutexLocker locker(&mutex_);

  if (!available_.isEmpty()) {
    auto *conn = available_.takeLast();
    active_.append(conn);
    emit connectionAcquired(conn);
    return conn;
  }

  if (totalCreated_ < config_.maxConnections) {
    auto *conn = createConnection();
    if (conn) {
      active_.append(conn);
      emit connectionAcquired(conn);
      return conn;
    }
  }

  emit poolExhausted();
  return nullptr;
}

void ConnectionPool::release(QTcpSocket *connection) {
  if (!connection) return;

  QMutexLocker locker(&mutex_);
  auto it = active_.indexOf(connection);
  if (it != -1) {
    active_.removeAt(it);
    if (connection->state() == QAbstractSocket::ConnectedState) {
      available_.append(connection);
    } else {
      destroyConnection(connection);
    }
    emit connectionReleased(connection);
  }
}

int ConnectionPool::activeConnections() const {
  QMutexLocker locker(&mutex_);
  return active_.size();
}

int ConnectionPool::availableConnections() const {
  QMutexLocker locker(&mutex_);
  return available_.size();
}

bool ConnectionPool::isHealthy() const {
  QMutexLocker locker(&mutex_);
  return !available_.isEmpty() || totalCreated_ < config_.maxConnections;
}

// Iterates available sockets and destroys any that have disconnected
void ConnectionPool::performHealthCheck() {
  QMutexLocker locker(&mutex_);
  QVector<QTcpSocket *> toRemove;

  for (auto *conn : available_) {
    if (conn->state() != QAbstractSocket::ConnectedState) {
      toRemove.append(conn);
    }
  }

  for (auto *conn : toRemove) {
    available_.removeOne(conn);
    destroyConnection(conn);
  }
}

void ConnectionPool::onConnectionDisconnected() {
  auto *conn = qobject_cast<QTcpSocket *>(sender());
  if (!conn) return;

  QMutexLocker locker(&mutex_);
  active_.removeOne(conn);
  available_.removeOne(conn);
  destroyConnection(conn);
  emit connectionError("Connection disconnected");
}

// Connects to the configured host:port with timeout, wires disconnect signal
QTcpSocket *ConnectionPool::createConnection() {
  auto *conn = new QTcpSocket(this);
  conn->connectToHost(config_.host, config_.port);

  if (!conn->waitForConnected(config_.connectionTimeoutMs)) {
    delete conn;
    return nullptr;
  }

  connect(conn, &QTcpSocket::disconnected, this, &ConnectionPool::onConnectionDisconnected);
  ++totalCreated_;
  return conn;
}

void ConnectionPool::destroyConnection(QTcpSocket *connection) {
  if (connection) {
    connection->disconnect();
    connection->deleteLater();
    --totalCreated_;
  }
}
