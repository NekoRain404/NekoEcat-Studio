#pragma once

// EcatClient — Qt network client for communicating with the ecatd daemon.
//
// Wraps QLocalSocket to send JSON commands and receive responses.
// Manages connection state, timeouts, and reconnection logic.
// Used by MainWindow to issue SDO reads/writes, PDO queries, and topology scans.

// JSON-over-TCP client for communicating with the ecatd runtime daemon.


#include "EthercatTypes.h"

#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <functional>

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
};

class EcatClient : public QObject {
  Q_OBJECT

  // TCP client for the ecatd runtime daemon. Uses newline-delimited JSON with a
  // JSON-RPC-style id/method/params convention. Each request registers a typed
  // callback handler that fires when the correlated response arrives.
public:
  explicit EcatClient(QObject *parent = nullptr);

  void connectToDaemon();
  bool isConnected() const;
  QString masterTarget() const;
  void setMasterTarget(const QString &target);

  void ping();
  void hostDiagnostics();
  void master();
  void scan();
  void rescan();
  void slaveInfo(int position);
  void pdos(int position);
  void sdos(int position);
  void xml(int position);
  void upload(int position, const QString &index, const QString &subIndex);
  void download(int position, const QString &index, const QString &subIndex,
                const QString &value, const QString &type);
  void applyStartupSdos(const QJsonArray &items);
  void setState(int position, const QString &state);
  void setAllStates(const QString &state);
  void freeRunStart();
  void freeRunStop();
  void freeRunStatus();
  void rtTestStart(int cycleUsec = 1000);
  void rtTestStop();
  void dcSyncStatus();
  void rtTestStatus();
  void alEventLog(int limit = 100);
  void alEventClear();
  void listAdapters();
  void setAdapter(const QString &name);
  void setRequestTimeout(int ms);
  ConnectionState connectionState() const;
  void connectToHost(const QHostAddress &address, quint16 port);

  // Auto-reconnect control.
  void enableAutoReconnect(bool enable);
  bool autoReconnectEnabled() const;

signals:
  void connected();
  void connectionStateChanged(ConnectionState state);
  void disconnected();
  void errorMessage(const QString &message);
  void daemonInfo(const QString &text);
  void hostDiagnosticsReady(const QJsonArray &checks);
  void masterText(const QString &text);
  void slavesChanged(const QVector<SlaveInfo> &slaves);
  void slaveTextResult(const QString &title, int position, const QString &text);
  void sdoValue(int position, const QString &index, const QString &subIndex,
                const QString &value);
  void startupSdoResults(const QJsonArray &results);
  void commandSucceeded(const QString &message);
  void freeRunChanged(bool running, const QString &status);
  void freeRunTelemetry(const QJsonObject &telemetry);
  void rtTestTelemetry(const QJsonObject &telemetry);
  void dcSyncStatusResult(const QJsonObject &data);
  void alEventLogResult(const QJsonObject &data);
  void adaptersListResult(const QJsonObject &data);
  void reconnected();  // Emitted after successful auto-reconnect.

private slots:
  void attemptReconnect();
  void readSocket();

private:
  using Handler = std::function<void(const QJsonObject &)>;

  void send(const QString &method, const QJsonObject &params, Handler handler);
  void handleLine(const QByteArray &line);

  QTcpSocket socket_;
  QByteArray buffer_;
  int nextId_ = 1;
  QString masterTarget_ = "0";
  QHash<QString, Handler> handlers_;

  ConnectionState connectionState_ = ConnectionState::Disconnected;
  void setConnectionState(ConnectionState state);

  static constexpr int kDefaultRequestTimeoutMs = 10000;
  int requestTimeoutMs_ = kDefaultRequestTimeoutMs;
  QTimer *requestSweepTimer_ = nullptr;
  QHash<QString, qint64> requestTimestamps_;
  void sweepTimedOutRequests();

  // Auto-reconnect state.
  bool autoReconnectEnabled_ = true;
  QTimer *heartbeatTimer_ = nullptr;    // Pings daemon every 5s.
  QTimer *reconnectTimer_ = nullptr;    // Fires at increasing intervals.
  int consecutiveFailures_ = 0;         // Reset on successful ping.
  int reconnectIntervalMs_ = 2000;      // Exponential backoff: 2→4→8→16→30s.
  static constexpr int kMaxReconnectMs = 30000;
  static constexpr int kMaxConsecutiveFailures = 3;
  void setupAutoReconnect();
};
