#pragma once

// EcatClient — Qt network client for communicating with the ecatd daemon.
//
// Wraps QTcpSocket to send JSON commands and receive responses over TCP.
// Manages connection state, timeouts, and reconnection logic.
// Used by MainWindow to issue SDO reads/writes, PDO queries, and topology scans.
//
// Wire protocol: newline-delimited JSON with JSON-RPC-style id/method/params.
// Each request registers a typed callback handler that fires when the
// correlated response arrives. Supports auto-reconnect with exponential backoff.

#include "EthercatTypes.h"

#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <functional>

/// @brief TCP connection state machine for the daemon connection.
enum class ConnectionState {
    Disconnected,   ///< No active connection.
    Connecting,     ///< TCP connect in progress.
    Connected,      ///< Connection established and ready.
    Reconnecting,   ///< Lost connection, attempting to reconnect.
};

/// @brief JSON-over-TCP client for the ecatd runtime daemon.
///
/// Provides typed methods for every daemon command. Each method sends a
/// JSON-RPC request and connects the response to a Qt signal. The client
/// handles framing (newline-delimited JSON), request correlation by ID,
/// timeout sweeping, and automatic reconnection with exponential backoff.
///
/// Usage:
///   auto *client = new EcatClient(this);
///   connect(client, &EcatClient::slavesChanged, this, &MainWindow::onSlavesChanged);
///   client->connectToDaemon();
///   client->scan();
class EcatClient : public QObject {
  Q_OBJECT

public:
  /// @brief Construct a new EcatClient.
  /// @param parent Qt parent object.
  explicit EcatClient(QObject *parent = nullptr);
  ~EcatClient() override;

  // -- Connection management --

  /// @brief Initiate TCP connection to the daemon (default 127.0.0.1:5877).
  void connectToDaemon();

  /// @brief Whether the client is currently connected to the daemon.
  bool isConnected() const;

  /// @brief Get the current master index target (e.g. "0").
  QString masterTarget() const;

  /// @brief Set the master index target for subsequent commands.
  void setMasterTarget(const QString &target);

  /// @brief Get the current connection state.
  ConnectionState connectionState() const;

  /// @brief Connect to a specific host and port (overrides default daemon address).
  /// @param address Target IP address.
  /// @param port Target TCP port.
  void connectToHost(const QHostAddress &address, quint16 port);

  // -- Host & master --

  /// @brief Ping the daemon to verify connectivity.
  void ping();

  /// @brief Request host-level diagnostics (network, CPU, IgH driver status).
  void hostDiagnostics();

  /// @brief Query master information (timing, DC config, topology).
  void master();

  // -- Slave operations --

  /// @brief Scan all slaves on the bus. Emits slavesChanged() with results.
  void scan();

  /// @brief Force a bus rescan to rediscover slaves.
  void rescan();

  /// @brief Get detailed info for a specific slave.
  /// @param position Slave bus position.
  void slaveInfo(int position);

  /// @brief List PDO mappings for a specific slave.
  /// @param position Slave bus position.
  void pdos(int position);

  /// @brief List SDO catalog for a specific slave.
  /// @param position Slave bus position.
  void sdos(int position);

  /// @brief Get ESI/XML description for a specific slave.
  /// @param position Slave bus position.
  void xml(int position);

  // -- SDO read/write --

  /// @brief Upload (read) an SDO value from a slave.
  /// @param position Slave bus position.
  /// @param index SDO index (hex string).
  /// @param subIndex SDO sub-index (hex string).
  void upload(int position, const QString &index, const QString &subIndex);

  /// @brief Download (write) an SDO value to a slave.
  /// @param position Slave bus position.
  /// @param index SDO index (hex string).
  /// @param subIndex SDO sub-index (hex string).
  /// @param value Value to write.
  /// @param type Data type (e.g. "uint32", "string").
  void download(int position, const QString &index, const QString &subIndex,
                const QString &value, const QString &type);

  /// @brief Apply a batch of startup SDO writes.
  /// @param items JSON array of {position, index, subIndex, value, type} objects.
  void applyStartupSdos(const QJsonArray &items);

  // -- State control --

  /// @brief Set the operational state of a single slave.
  /// @param position Slave bus position.
  /// @param state Target state ("OP", "PREOP", "SAFEOP", "INIT").
  void setState(int position, const QString &state);

  /// @brief Set the operational state of all slaves on the bus.
  /// @param state Target state for all slaves.
  void setAllStates(const QString &state);

  // -- Free-run mode --

  /// @brief Start free-run PDO output mode.
  void freeRunStart();

  /// @brief Stop free-run PDO output mode.
  void freeRunStop();

  /// @brief Query current free-run status and telemetry.
  void freeRunStatus();

  // -- Real-time test --

  /// @brief Start a real-time cycle timing test.
  /// @param cycleUsec Target cycle period in microseconds (default 1000 = 1kHz).
  void rtTestStart(int cycleUsec = 1000);

  /// @brief Stop the real-time cycle timing test.
  void rtTestStop();

  /// @brief Query current RT test telemetry.
  void rtTestStatus();

  // -- Distributed clocks --

  /// @brief Query DC sync status for all slaves.
  void dcSyncStatus();

  /// @brief Configure DC sync for a specific slave.
  /// @param position Slave bus position to configure as DC reference.
  void dcConfigure(int position);

  /// @brief Activate distributed clocks with the specified reference clock slave.
  /// @param refClockSlave Position of the DC reference clock slave.
  void dcActivate(int refClockSlave);

  /// @brief Deactivate distributed clocks.
  void dcDeactivate();

  // -- AL event log --

  /// @brief Read the application layer event log.
  /// @param limit Maximum number of events to return (default 100).
  void alEventLog(int limit = 100);

  /// @brief Clear the application layer event log.
  void alEventClear();

  // -- Adapter / backend management --

  /// @brief List available network adapters.
  void listAdapters();

  /// @brief Set the network adapter used by IgH EtherCAT Master.
  /// @param name Adapter name (e.g. "eth0").
  void setAdapter(const QString &name);

  /// @brief Set the backend mode (CLI vs native).
  /// @param mode "cli" or "native".
  void setBackendMode(const QString &mode);

  /// @brief Query the current backend mode.
  void getBackendMode();

  // -- FoE (File over EtherCAT) --

  /// @brief Read a file from a slave via FoE.
  /// @param position Slave bus position.
  /// @param filePath Remote file path on the slave.
  void foeRead(int position, const QString &filePath);

  /// @brief Write a file to a slave via FoE.
  /// @param position Slave bus position.
  /// @param filePath Remote file path on the slave.
  /// @param password FoE password (default 0).
  void foeWrite(int position, const QString &filePath, quint32 password = 0);

  // -- Configuration --

  /// @brief Set the request timeout in milliseconds.
  /// @param ms Timeout in ms (default 10000).
  void setRequestTimeout(int ms);

  /// @brief Enable or disable automatic reconnection on disconnect.
  /// @param enable true to enable auto-reconnect.
  void enableAutoReconnect(bool enable);

  /// @brief Whether auto-reconnect is currently enabled.
  bool autoReconnectEnabled() const;

signals:
  // -- Connection signals --
  void connected();                                  ///< Emitted when TCP connection is established.
  void connectionStateChanged(ConnectionState state); ///< Emitted on every state transition.
  void disconnected();                               ///< Emitted when connection is lost.
  void errorMessage(const QString &message);         ///< Emitted on protocol or transport errors.
  void reconnected();                                ///< Emitted after successful auto-reconnect.
  void reconnecting(int attempt, int intervalMs);    ///< Emitted before each reconnect attempt.
  void reconnectFailed(int maxAttempts);             ///< Emitted when max failures reached.

  // -- Data signals --
  void daemonInfo(const QString &text);                        ///< Daemon version/info text.
  void hostDiagnosticsReady(const QJsonArray &checks);        ///< Host diagnostic results.
  void masterText(const QString &text);                        ///< Master info text.
  void slavesChanged(const QVector<SlaveInfo> &slaves);        ///< Slave list after scan/rescan.
  void slaveTextResult(const QString &title, int position, const QString &text); ///< Slave detail text.
  void sdoValue(int position, const QString &index, const QString &subIndex,
                const QString &value);                         ///< SDO upload result.
  void startupSdoResults(const QJsonArray &results);          ///< Batch SDO write results.
  void commandSucceeded(const QString &message);               ///< Generic success confirmation.

  // -- Free-run signals --
  void freeRunChanged(bool running, const QString &status);   ///< Free-run state change.
  void freeRunTelemetry(const QJsonObject &telemetry);        ///< Free-run cycle telemetry.

  // -- RT test signals --
  void rtTestTelemetry(const QJsonObject &telemetry);         ///< RT test timing telemetry.

  // -- DC sync signals --
  void dcSyncStatusResult(const QJsonObject &data);           ///< DC sync status data.
  void dcConfigureResult(const QJsonObject &data);            ///< DC configure result.
  void dcActivateResult(const QJsonObject &data);             ///< DC activate result.
  void dcDeactivateResult(const QJsonObject &data);           ///< DC deactivate result.

  // -- AL event signals --
  void alEventLogResult(const QJsonObject &data);             ///< AL event log entries.

  // -- Adapter/backend signals --
  void adaptersListResult(const QJsonObject &data);           ///< Network adapter list.
  void backendModeChanged(const QString &backend, const QString &mode); ///< Backend mode changed.

  // -- FoE signals --
  void foeReadResult(int position, const QString &filePath, qint64 fileSize);  ///< FoE read complete.
  void foeWriteResult(int position, qint64 bytesWritten);                      ///< FoE write complete.

private slots:
  void attemptReconnect();
  void readSocket();

private:
  using Handler = std::function<void(const QJsonObject &)>;

  void send(const QString &method, const QJsonObject &params, Handler handler);
  void handleLine(const QByteArray &line);

  QTcpSocket socket_;
  QByteArray buffer_;
  quint64 nextId_ = 1;
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
  bool pendingPing_ = false;            // True while awaiting pong response.
  int reconnectIntervalMs_ = 2000;      // Exponential backoff: 2→4→8→16→30s.
  static constexpr int kMaxReconnectMs = 30000;
  static constexpr int kMaxConsecutiveFailures = 3;
  void setupAutoReconnect();

  // Connection timeout tracking.
  QTimer *connectTimeoutTimer_ = nullptr;  // Fires if initial connect hangs.
  static constexpr int kConnectTimeoutMs = 10000;
  void setupConnectTimeout();
};
