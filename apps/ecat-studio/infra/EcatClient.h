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
/// Every public method that communicates with the daemon sends a specific
/// JSON-RPC command (noted in its @brief) and emits a Qt signal when the
/// correlated response arrives.
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
  ///        Initialises the TCP socket, timeout sweep timer (2s interval),
  ///        and auto-reconnect infrastructure. No daemon command is sent.
  /// @param parent Qt parent object.
  explicit EcatClient(QObject *parent = nullptr);

  /// @brief Destructor. Blocks socket signals to prevent callbacks during teardown.
  ///        No daemon command is sent.
  ~EcatClient() override;

  // -- Connection management --

  /// @brief Initiate TCP connection to the daemon on 127.0.0.1:5877.
  ///        No-op if already connected or connecting. No JSON-RPC command is sent.
  void connectToDaemon();

  /// @brief Whether the client is currently connected to the daemon.
  /// @return true if the connection state is Connected, false otherwise.
  bool isConnected() const;

  /// @brief Get the current IgH master index target injected into every request.
  /// @return Master index string (defaults to "0").
  QString masterTarget() const;

  /// @brief Set the IgH master index target for subsequent JSON-RPC requests.
  /// @param target Master index (e.g. "0", "1"). An empty or trimmed string falls back to "0".
  void setMasterTarget(const QString &target);

  /// @brief Get the current connection state machine state.
  /// @return One of Disconnected, Connecting, Connected, or Reconnecting.
  ConnectionState connectionState() const;

  /// @brief Connect to a specific daemon host address and port (overrides default).
  ///        No-op if already connected or connecting. No JSON-RPC command is sent.
  /// @param address Target IP address.
  /// @param port Target TCP port (default 5877).
  void connectToHost(const QHostAddress &address, quint16 port);

  // -- Host & master --

  /// @brief Ping the daemon to verify connectivity and retrieve version info.
  ///        Sends the "ping" JSON-RPC command.
  ///        Emits daemonInfo() with the name and version string on success.
  void ping();

  /// @brief Request host-level diagnostics from the daemon host.
  ///        Sends the "hostDiagnostics" JSON-RPC command.
  ///        Emits hostDiagnosticsReady() with an array of diagnostic check results.
  void hostDiagnostics();

  /// @brief Query master information (timing, DC config, topology).
  ///        Sends the "master" JSON-RPC command.
  ///        Emits masterText() with the formatted master info text.
  void master();

  // -- Slave operations --

  /// @brief Scan all slaves on the bus and emit the deserialised slave list.
  ///        Sends the "scan" JSON-RPC command.
  ///        Emits slavesChanged() with the vector of SlaveInfo results.
  void scan();

  /// @brief Force a bus rescan to rediscover slaves, then auto-refresh the list.
  ///        Sends the "rescan" JSON-RPC command.
  ///        Emits commandSucceeded() on completion, then internally calls scan().
  void rescan();

  /// @brief Get detailed info text for a specific slave.
  ///        Sends the "slaveInfo" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  ///        Emits slaveTextResult("Info", position, text) on response.
  void slaveInfo(int position);

  /// @brief List PDO (Process Data Object) mappings for a specific slave.
  ///        Sends the "pdos" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  ///        Emits slaveTextResult("PDO", position, text) on response.
  void pdos(int position);

  /// @brief List the SDO (Service Data Object) dictionary for a specific slave.
  ///        Sends the "sdos" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  ///        Emits slaveTextResult("SDO", position, text) on response.
  void sdos(int position);

  /// @brief Fetch the ESI/XML device description for a specific slave.
  ///        Sends the "xml" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  ///        Emits slaveTextResult("ESI XML", position, text) on response.
  void xml(int position);

  // -- SDO read/write --

  /// @brief Upload (read) an SDO value from a slave.
  ///        Sends the "upload" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  /// @param index SDO index (hex string, e.g. "0x6040").
  /// @param subIndex SDO sub-index (hex string, e.g. "0x00").
  ///        Emits sdoValue(position, index, subIndex, value) on success.
  void upload(int position, const QString &index, const QString &subIndex);

  /// @brief Download (write) an SDO value to a slave.
  ///        Sends the "download" JSON-RPC command.
  ///        On success the client automatically issues an upload() to verify.
  /// @param position Slave bus position (0-based).
  /// @param index SDO index (hex string).
  /// @param subIndex SDO sub-index (hex string).
  /// @param value Value to write.
  /// @param type Data type hint (e.g. "uint32", "string", "octet_string").
  ///        Emits commandSucceeded() and then triggers upload() for readback.
  void download(int position, const QString &index, const QString &subIndex,
                const QString &value, const QString &type);

  /// @brief Apply a batch of startup SDO writes.
  ///        Sends the "applyStartupSdos" JSON-RPC command.
  /// @param items JSON array of {position, index, subIndex, value, type} objects.
  ///        Emits startupSdoResults() with per-row results and commandSucceeded()
  ///        with a summary. Reports individual failures via errorMessage().
  void applyStartupSdos(const QJsonArray &items);

  // -- State control --

  /// @brief Set the EtherCAT AL state of a single slave.
  ///        Sends the "setState" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  /// @param state Target state string ("OP", "PREOP", "SAFEOP", "INIT").
  ///        Emits commandSucceeded() on success, then triggers scan() to refresh.
  void setState(int position, const QString &state);

  /// @brief Set the EtherCAT AL state of all slaves on the bus.
  ///        Sends the "setAllStates" JSON-RPC command.
  /// @param state Target state for all slaves ("OP", "PREOP", "SAFEOP", "INIT").
  ///        Emits commandSucceeded() on success, then triggers scan() to refresh.
  void setAllStates(const QString &state);

  // -- Free-run mode --

  /// @brief Start free-run PDO output mode for cyclic data exchange.
  ///        Sends the "freeRunStart" JSON-RPC command.
  ///        Emits freeRunChanged(true, status), freeRunTelemetry(), and commandSucceeded().
  void freeRunStart();

  /// @brief Stop free-run PDO output mode and release the IgH master.
  ///        Sends the "freeRunStop" JSON-RPC command.
  ///        Emits freeRunChanged(false, status), freeRunTelemetry(), and commandSucceeded().
  void freeRunStop();

  /// @brief Query current free-run status and cycle telemetry without side effects.
  ///        Sends the "freeRunStatus" JSON-RPC command.
  ///        Emits freeRunChanged(running, status) and freeRunTelemetry().
  void freeRunStatus();

  // -- Real-time test --

  /// @brief Start a real-time cycle timing test on the current master.
  ///        Sends the "rtTestStart" JSON-RPC command.
  /// @param cycleUsec Target cycle period in microseconds (default 1000 = 1 kHz).
  ///        Emits rtTestTelemetry() and commandSucceeded() on response.
  void rtTestStart(int cycleUsec = 1000);

  /// @brief Stop the running real-time cycle timing test.
  ///        Sends the "rtTestStop" JSON-RPC command.
  ///        Emits rtTestTelemetry() and commandSucceeded() on response.
  void rtTestStop();

  /// @brief Query current RT test state and telemetry without side effects.
  ///        Sends the "rtTestStatus" JSON-RPC command.
  ///        Emits rtTestTelemetry() on response.
  void rtTestStatus();

  // -- Distributed clocks --

  /// @brief Query DC (Distributed Clocks) synchronisation status for all slaves.
  ///        Sends the "dcSyncStatus" JSON-RPC command.
  ///        Emits dcSyncStatusResult() with sync status data.
  void dcSyncStatus();

  /// @brief Query DC configuration from a specific slave's ESI description.
  ///        Sends the "dcConfigure" JSON-RPC command.
  /// @param position Slave bus position to query as DC reference candidate.
  ///        Emits dcConfigureResult() and commandSucceeded().
  void dcConfigure(int position);

  /// @brief Activate distributed clocks with the specified reference clock slave.
  ///        Sends the "dcActivate" JSON-RPC command.
  /// @param refClockSlave Position of the DC reference clock slave (0-based).
  ///        Emits dcActivateResult() and commandSucceeded().
  void dcActivate(int refClockSlave);

  /// @brief Deactivate distributed clocks.
  ///        Sends the "dcDeactivate" JSON-RPC command.
  ///        Emits dcDeactivateResult() and commandSucceeded().
  void dcDeactivate();

  // -- AL event log --

  /// @brief Read the application layer event log from the daemon.
  ///        Sends the "alEventLog" JSON-RPC command.
  /// @param limit Maximum number of events to return (default 100).
  ///        Emits alEventLogResult() with the event entries.
  void alEventLog(int limit = 100);

  /// @brief Clear the daemon's application layer event history.
  ///        Sends the "alEventClear" JSON-RPC command.
  ///        Emits commandSucceeded() on completion.
  void alEventClear();

  // -- Adapter / backend management --

  /// @brief Enumerate network adapters available on the daemon host.
  ///        Sends the "listAdapters" JSON-RPC command.
  ///        Emits adaptersListResult() with the adapter list.
  void listAdapters();

  /// @brief Set the network adapter used by the IgH EtherCAT Master.
  ///        Sends the "setAdapter" JSON-RPC command (may require daemon restart).
  /// @param name Adapter name (e.g. "eth0").
  ///        Emits commandSucceeded() on completion.
  void setAdapter(const QString &name);

  /// @brief Switch the daemon backend mode between CLI and native.
  ///        Sends the "setBackend" JSON-RPC command.
  /// @param mode "cli" or "native".
  ///        Emits backendModeChanged() with the new backend and mode.
  void setBackendMode(const QString &mode);

  /// @brief Query the current daemon backend mode.
  ///        Sends the "getBackend" JSON-RPC command.
  ///        Emits backendModeChanged() with the current backend and mode.
  void getBackendMode();

  // -- FoE (File over EtherCAT) --

  /// @brief Read a file from a slave via FoE protocol.
  ///        The daemon reads the file and saves it to the given path.
  ///        Sends the "foeRead" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  /// @param filePath Destination file path on the daemon host.
  ///        Emits foeReadResult(position, filePath, fileSize) and commandSucceeded().
  void foeRead(int position, const QString &filePath);

  /// @brief Write a file to a slave via FoE protocol.
  ///        The daemon reads from the given path and transmits to the slave.
  ///        Sends the "foeWrite" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  /// @param filePath Source file path on the daemon host.
  /// @param password FoE password (default 0 = no password).
  ///        Emits foeWriteResult(position, bytesWritten) and commandSucceeded().
  void foeWrite(int position, const QString &filePath, quint32 password = 0);

  // -- SoE (Servo over EtherCAT) --

  /// @brief Read an SoE IDN (Identification Number) from a slave.
  ///        Sends the "soeRead" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  /// @param idn IDN string (e.g. "P-0-0150") or numeric IDN.
  /// @param drive Drive number (0-7, default 0).
  /// @param type Optional data type hint (e.g. "uint16").
  ///        Emits soeReadResult(position, idn, value) on response.
  void soeRead(int position, const QString &idn, int drive = 0, const QString &type = QString());

  /// @brief Write an SoE IDN (Identification Number) to a slave.
  ///        Sends the "soeWrite" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  /// @param idn IDN string or numeric.
  /// @param drive Drive number (0-7, default 0).
  /// @param type Optional data type hint.
  ///        Emits soeWriteResult(position, idn) and commandSucceeded() on response.
  void soeWrite(int position, const QString &idn, const QString &value,
                int drive = 0, const QString &type = QString());

  // -- EoE (Ethernet over EtherCAT) --

  /// @brief Query EoE (Ethernet over EtherCAT) support status for a slave.
  ///        Sends the "eoeStatus" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  ///        Emits eoeStatusResult(position, data) with status information.
  void eoeStatus(int position);

  /// @brief Configure IP address for an EoE-capable slave.
  ///        Sends the "eoeConfigureIp" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  /// @param ip IP address (e.g. "192.168.1.100").
  /// @param subnet Subnet mask (e.g. "255.255.255.0").
  /// @param gateway Default gateway (optional).
  /// @param dns DNS server (optional).
  ///        Emits eoeIpConfigured() and commandSucceeded() on response.
  void eoeConfigureIp(int position, const QString &ip, const QString &subnet,
                      const QString &gateway = QString(), const QString &dns = QString());

  /// @brief Read current IP configuration from an EoE-capable slave.
  ///        Sends the "eoeGetIp" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  ///        Emits eoeIpResult(position, data) with the IP configuration.
  void eoeGetIp(int position);

  /// @brief Get EoE frame transmission statistics for a slave.
  ///        Sends the "eoeStats" JSON-RPC command.
  /// @param position Slave bus position (0-based).
  ///        Emits eoeStatsResult(position, data) with frame statistics.
  void eoeStats(int position);

  // -- Redundancy --

  /// @brief Query cable redundancy subsystem status.
  ///        Sends the "redundancyStatus" JSON-RPC command.
  ///        Emits redundancyStatusResult() with status data.
  void redundancyStatus();

  /// @brief Enable cable redundancy on the secondary communication path.
  ///        Sends the "redundancyEnable" JSON-RPC command.
  ///        Emits redundancyCommandResult("enable", success, message).
  void redundancyEnable();

  /// @brief Disable cable redundancy.
  ///        Sends the "redundancyDisable" JSON-RPC command.
  ///        Emits redundancyCommandResult("disable", success, message).
  void redundancyDisable();

  /// @brief Perform failover from primary to secondary communication path.
  ///        Sends the "redundancyFailover" JSON-RPC command.
  ///        Emits redundancyCommandResult("failover", success, message).
  void redundancyFailover();

  /// @brief Perform failback from secondary to primary communication path.
  ///        Sends the "redundancyFailback" JSON-RPC command.
  ///        Emits redundancyCommandResult("failback", success, message).
  void redundancyFailback();

  /// @brief Get redundancy event history from the daemon.
  ///        Sends the "redundancyHistory" JSON-RPC command.
  /// @param limit Maximum number of events to return (default 100).
  ///        Emits redundancyHistoryResult() with event entries.
  void redundancyHistory(int limit = 100);

  // -- Online Change (runtime reconfiguration) --

  /// @brief Preview an online (runtime) configuration change without applying it.
  ///        Sends the "onlineChangePreview" JSON-RPC command.
  /// @param changes Array of {position, index, subIndex, value, type} objects.
  ///        Emits onlineChangePreviewResult() with the preview data.
  void onlineChangePreview(const QJsonArray &changes);

  /// @brief Apply an online (runtime) configuration change.
  ///        Affected slaves are transitioned to PREOP, SDOs are written, then
  ///        slaves are restored to the target state.
  ///        Sends the "onlineChangeApply" JSON-RPC command.
  /// @param changes Array of {position, index, subIndex, value, type} objects.
  /// @param targetState State to restore affected slaves to (default "OP").
  ///        Emits onlineChangeApplyResult() with the result data.
  void onlineChangeApply(const QJsonArray &changes, const QString &targetState = "OP");

  /// @brief Query whether an online change operation is currently in progress.
  ///        Sends the "onlineChangeStatus" JSON-RPC command.
  ///        Emits onlineChangeStatusResult() with the status data.
  void onlineChangeStatus();

  // -- Configuration --

  /// @brief Set the request timeout for daemon JSON-RPC calls.
  ///        This is a local configuration change — no daemon command is sent.
  /// @param ms Timeout in milliseconds (default 10000). Must be positive;
  ///        values <= 0 are clamped to the default.
  void setRequestTimeout(int ms);

  /// @brief Enable or disable automatic reconnection on connection loss.
  ///        This is a local configuration change — no daemon command is sent.
  /// @param enable true to enable auto-reconnect (default), false to disable
  ///        and stop any in-progress reconnect timers.
  void enableAutoReconnect(bool enable);

  /// @brief Whether automatic reconnection is currently enabled.
  /// @return true if auto-reconnect is active, false otherwise.
  bool autoReconnectEnabled() const;

signals:
  // -- Connection signals --
  void connected();                                  ///< Emitted when TCP connection is established.
  void connectionStateChanged(ConnectionState state); ///< Emitted on every state transition.
  void disconnected();                               ///< Emitted when connection is lost.
  void connectionLost();                             ///< Emitted when an established connection is dropped (distinct from per-request failures).
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

  // -- SoE signals --
  void soeReadResult(int position, const QString &idn, const QString &value);  ///< SoE IDN read result.
  void soeWriteResult(int position, const QString &idn);                       ///< SoE IDN write complete.

  // -- EoE signals --
  void eoeStatusResult(int position, const QJsonObject &data);    ///< EoE status query result.
  void eoeIpConfigured(int position, const QString &ip);          ///< EoE IP configured.
  void eoeIpResult(int position, const QJsonObject &data);        ///< EoE IP readback result.
  void eoeStatsResult(int position, const QJsonObject &data);     ///< EoE statistics result.

  // -- Redundancy signals --
  void redundancyStatusResult(const QJsonObject &data);           ///< Redundancy status.
  void redundancyCommandResult(const QString &command, bool success, const QString &message); ///< Redundancy command result.
  void redundancyHistoryResult(const QJsonObject &data);          ///< Redundancy event history.

  // -- Online change signals --
  void onlineChangePreviewResult(const QJsonObject &data);        ///< Online change preview.
  void onlineChangeApplyResult(const QJsonObject &data);          ///< Online change apply result.
  void onlineChangeStatusResult(const QJsonObject &data);         ///< Online change status.

private slots:
  void attemptReconnect();
  void readSocket();

private:
  using Handler = std::function<void(const QJsonObject &)>;

  void send(const QString &method, const QJsonObject &params, Handler handler);
  void handleLine(const QByteArray &line);
  void scheduleReconnect();

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
  QHostAddress host_ = QHostAddress::LocalHost;  // Configured daemon host.
  quint16 port_ = 5877;                          // Configured daemon port.
  bool autoReconnectEnabled_ = true;
  QTimer *heartbeatTimer_ = nullptr;    // Pings daemon every 5s.
  QTimer *reconnectTimer_ = nullptr;    // Fires at increasing intervals.
  int consecutiveFailures_ = 0;         // Missed heartbeat pings; reset on pong.
  int connectFailures_ = 0;             // Consecutive failed TCP connects; reset on connect.
  bool pendingPing_ = false;            // True while awaiting pong response.
  int reconnectIntervalMs_ = 2000;      // Exponential backoff: 2->4->8->16->30s.
  static constexpr int kMaxReconnectMs = 30000;
  static constexpr int kMaxConsecutiveFailures = 3;  // Heartbeat misses before declaring the link dead.
  static constexpr int kMaxConnectAttempts = 3;      // TCP connect attempts before giving up.
  void setupAutoReconnect();

  // Connection timeout tracking.
  QTimer *connectTimeoutTimer_ = nullptr;  // Fires if initial connect hangs.
  static constexpr int kConnectTimeoutMs = 10000;
  void setupConnectTimeout();
};
