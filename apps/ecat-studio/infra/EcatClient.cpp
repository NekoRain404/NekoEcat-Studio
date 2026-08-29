// JSON-over-TCP client for communicating with the ecatd runtime daemon.
#include "EcatClient.h"

#include "JsonProtocol.h"

#include <QDateTime>

#include <QJsonDocument>
#include <QJsonObject>

EcatClient::~EcatClient() {
  socket_.blockSignals(true);
}

// Client constructor: initialize timeout sweep timer (2s interval, 10s request timeout).
EcatClient::EcatClient(QObject *parent) : QObject(parent) {
  // Wire Qt socket signals for connection lifecycle and incoming data.
  connect(&socket_, &QTcpSocket::connected, this, [this] {
    if (connectTimeoutTimer_) connectTimeoutTimer_->stop();
    setConnectionState(ConnectionState::Connected);
    consecutiveFailures_ = 0;
    connectFailures_ = 0;
    pendingPing_ = false;
    reconnectIntervalMs_ = 2000;
    if (requestSweepTimer_) requestSweepTimer_->start(2000);
    emit connected();
    emit reconnected();
    if (autoReconnectEnabled_) setupAutoReconnect();
  });
  connect(&socket_, &QTcpSocket::disconnected, this, [this] {
    setConnectionState(ConnectionState::Disconnected);
    QHash<QString, Handler> pending;
    pending.swap(handlers_);
    requestTimestamps_.clear();
    QJsonObject error;
    error["code"] = -1;
    error["message"] = "Connection lost";
    for (auto it = pending.begin(); it != pending.end(); ++it) {
      it.value()(error);
    }
    emit disconnected();
    // Distinguish connection loss from per-request failures so consumers can
    // flush connection-scoped state without reacting to individual errors.
    emit connectionLost();
    // No requests can be in flight while disconnected; stop the sweep timer.
    if (requestSweepTimer_) requestSweepTimer_->stop();
    // Start auto-reconnect if enabled.
    scheduleReconnect();
  });
  connect(&socket_, &QTcpSocket::readyRead, this, &EcatClient::readSocket);
  connect(&socket_, &QTcpSocket::errorOccurred, this,
          [this](QAbstractSocket::SocketError error) {
            if (connectTimeoutTimer_) connectTimeoutTimer_->stop();

            // Only count transport failures that happen while a connect is
            // being attempted (initial or reconnect); errors on an established
            // connection are followed by the disconnected signal instead.
            const bool connectAttemptFailed =
                (connectionState_ == ConnectionState::Connecting ||
                 connectionState_ == ConnectionState::Reconnecting);
            if (connectAttemptFailed) {
              ++connectFailures_;
            }

            if (error == QAbstractSocket::ConnectionRefusedError) {
              emit errorMessage(QString("Connection refused (attempt %1)").arg(connectFailures_));
            } else if (error == QAbstractSocket::SocketTimeoutError) {
              emit errorMessage("Connection timed out");
            } else {
              emit errorMessage(socket_.errorString());
            }

            if (connectAttemptFailed) {
              setConnectionState(ConnectionState::Disconnected);
              if (connectFailures_ >= kMaxConnectAttempts) {
                emit reconnectFailed(kMaxConnectAttempts);
              } else {
                // A failed initial connect never fires the disconnected
                // signal, so the reconnect timer must be started here.
                scheduleReconnect();
              }
            }
          });

  // Request timeout sweep — evicts stale handlers every 2s.  Only active
  // while connected (no requests can be in flight otherwise).
  requestSweepTimer_ = new QTimer(this);
  requestSweepTimer_->setInterval(2000);
  connect(requestSweepTimer_, &QTimer::timeout, this, &EcatClient::sweepTimedOutRequests);

  // Initialize auto-reconnect timer (single-shot, started on disconnect).
  reconnectTimer_ = new QTimer(this);
  reconnectTimer_->setSingleShot(true);
  connect(reconnectTimer_, &QTimer::timeout, this, &EcatClient::attemptReconnect);
}

// Connect to ecatd's localhost TCP port; no-op if already connected or connecting.
void EcatClient::connectToDaemon() {
  if (connectionState_ == ConnectionState::Connected ||
      connectionState_ == ConnectionState::Connecting) {
    return;
  }
  setConnectionState(ConnectionState::Connecting);
  setupConnectTimeout();
  socket_.connectToHost(host_, port_);
}

// Current connection state (Disconnected → Connecting → Connected → Reconnecting).
ConnectionState EcatClient::connectionState() const {
  return connectionState_;
}

// Update connection state and emit signal if changed.
void EcatClient::setConnectionState(ConnectionState state) {
  if (connectionState_ != state) {
    connectionState_ = state;
    emit connectionStateChanged(state);
  }
}

// Connect to a specific host address and port (for remote daemon access).
void EcatClient::connectToHost(const QHostAddress &address, quint16 port) {
  if (connectionState_ == ConnectionState::Connected ||
      connectionState_ == ConnectionState::Connecting) {
    return;
  }
  // Remember the target so auto-reconnect dials the SAME host/port.
  host_ = address;
  port_ = port;
  setConnectionState(ConnectionState::Connecting);
  setupConnectTimeout();
  socket_.connectToHost(host_, port_);
}

// Set up connection timeout timer — aborts if connect hangs beyond kConnectTimeoutMs.
void EcatClient::setupConnectTimeout() {
  if (!connectTimeoutTimer_) {
    connectTimeoutTimer_ = new QTimer(this);
    connectTimeoutTimer_->setSingleShot(true);
    connect(connectTimeoutTimer_, &QTimer::timeout, this, [this]() {
      if (connectionState_ == ConnectionState::Connecting ||
          connectionState_ == ConnectionState::Reconnecting) {
        socket_.abort();
        setConnectionState(ConnectionState::Disconnected);
        ++connectFailures_;
        emit errorMessage(QString("Connection timed out after %1ms").arg(kConnectTimeoutMs));
        if (connectFailures_ >= kMaxConnectAttempts) {
          emit reconnectFailed(kMaxConnectAttempts);
        } else {
          scheduleReconnect();
        }
      }
    });
  }
  connectTimeoutTimer_->start(kConnectTimeoutMs);
}

// True if the TCP socket is in ConnectedState.
bool EcatClient::isConnected() const {
  return connectionState_ == ConnectionState::Connected;
}

// The IgH master index injected into every request's params as "master".
// Defaults to "0" for single-master installations.
QString EcatClient::masterTarget() const { return masterTarget_; }

// Switches the target EtherCAT master for daemon communication
void EcatClient::setMasterTarget(const QString &target) {
  // Normalize master target — empty string falls back to "0".
  const QString trimmed = target.trimmed();
  masterTarget_ = trimmed.isEmpty() ? "0" : trimmed;
}

// Verify daemon is alive and retrieve its version info.
void EcatClient::ping() {
  send("ping", {}, [this](const QJsonObject &result) {
    emit daemonInfo(QString("%1 %2").arg(result.value("name").toString(),
                                         result.value("version").toString()));
  });
}

// Request pre-flight host diagnostics from the daemon.
void EcatClient::hostDiagnostics() {
  send("hostDiagnostics", {}, [this](const QJsonObject &result) {
    emit hostDiagnosticsReady(result.value("checks").toArray());
  });
}

// Get raw `ethercat master` text for display.
void EcatClient::master() {
  send("master", {}, [this](const QJsonObject &result) {
    emit masterText(result.value("text").toString());
  });
}

// Enumerate all slaves on the bus and emit the deserialized list.
void EcatClient::scan() {
  send("scan", {}, [this](const QJsonObject &result) {
    emit slavesChanged(slavesFromJson(result.value("slaves").toArray()));
  });
}

// Trigger bus rescan, then auto-refresh the slave list.
void EcatClient::rescan() {
  send("rescan", {}, [this](const QJsonObject &) {
    emit commandSucceeded("Bus rescan requested");
    scan();
  });
}

// Fetch verbose info for a single slave.
void EcatClient::slaveInfo(int position) {
  send("slaveInfo", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("Info", position,
                              result.value("text").toString());
       });
}

// Fetch PDO dictionary for a single slave.
void EcatClient::pdos(int position) {
  send("pdos", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("PDO", position, result.value("text").toString());
       });
}

// Fetch SDO dictionary for a single slave.
void EcatClient::sdos(int position) {
  send("sdos", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("SDO", position, result.value("text").toString());
       });
}

// Fetch ESI XML descriptor for a single slave.
void EcatClient::xml(int position) {
  send("xml", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("ESI XML", position,
                              result.value("text").toString());
       });
}

// SDO upload (read) — emits the retrieved value via sdoValue signal.
void EcatClient::upload(int position, const QString &index,
                        const QString &subIndex) {
  send("upload",
       {{"position", position}, {"index", index}, {"subIndex", subIndex}},
       [this, position, index, subIndex](const QJsonObject &result) {
         // Don't emit sdoValue on error — the error is already reported via errorMessage.
         if (result.contains("error")) return;
         emit sdoValue(position, index, subIndex,
                       result.value("value").toString());
       });
}

// SDO download (write) — auto-reads back via upload() to verify the write took effect.
void EcatClient::download(int position, const QString &index,
                          const QString &subIndex, const QString &value,
                          const QString &type) {
  send("download",
       {{"position", position},
        {"index", index},
        {"subIndex", subIndex},
        {"value", value},
        {"type", type}},
       [this, position, index, subIndex](const QJsonObject &result) {
         // Don't emit success or auto-read on error.
         if (result.contains("error")) return;
         emit commandSucceeded(QString("SDO download complete #%1: %2:%3")
                                   .arg(position)
                                   .arg(index, subIndex));
         upload(position, index, subIndex);
       });
}

// Batch-apply a table of SDO settings at startup; reports per-row success/failure.
void EcatClient::applyStartupSdos(const QJsonArray &items) {
  send("applyStartupSdos", {{"items", items}},
       [this](const QJsonObject &result) {
         const int applied = result.value("applied").toInt();
         const int failed = result.value("failed").toInt();
         emit startupSdoResults(result.value("results").toArray());
         emit commandSucceeded(
             QString("Startup SDO apply complete: %1 applied, %2 failed")
                 .arg(applied)
                 .arg(failed));
         if (failed > 0) {
           const auto failures = result.value("failures").toArray();
           QStringList messages;
    // Iterate over collection
           for (const auto &failure : failures) {
             const auto object = failure.toObject();
             messages << QString("#%1 %2:%3 %4")
                             .arg(object.value("position").toInt())
                             .arg(object.value("index").toString(),
                                  object.value("subIndex").toString(),
                                  object.value("error").toString());
           }
           emit errorMessage(messages.join(" | "));
         }
       });
}

// Request AL state transition for a single slave, then auto-rescan to reflect the change.
void EcatClient::setState(int position, const QString &state) {
  send("setState", {{"position", position}, {"state", state}},
       [this, state](const QJsonObject &result) {
         if (result.contains("error")) return;
         emit commandSucceeded(QString("State request sent: %1").arg(state));
         scan();
       });
}

// Broadcast AL state transition to all slaves, then auto-rescan.
void EcatClient::setAllStates(const QString &state) {
  send("setAllStates", {{"state", state}}, [this, state](const QJsonObject &result) {
    if (result.contains("error")) return;
    emit commandSucceeded(QString("All-state request sent: %1").arg(state));
    scan();
  });
}

// Start real-time Free Run I/O on the current master target.
void EcatClient::freeRunStart() {
  send("freeRunStart", {}, [this](const QJsonObject &result) {
    emit freeRunChanged(true, result.value("status").toString("Running"));
    emit freeRunTelemetry(result);
    emit commandSucceeded("Free Run started");
  });
}

// Stop Free Run and release the IgH master.
void EcatClient::freeRunStop() {
  send("freeRunStop", {}, [this](const QJsonObject &result) {
    emit freeRunChanged(false, result.value("status").toString("Stopped"));
    emit freeRunTelemetry(result);
    emit commandSucceeded("Free Run stopped");
  });
}

// Poll current Free Run state and telemetry without side effects.
void EcatClient::freeRunStatus() {
  send("freeRunStatus", {}, [this](const QJsonObject &result) {
    emit freeRunChanged(result.value("running").toBool(),
                        result.value("status").toString());
    emit freeRunTelemetry(result);
  });
}

// Start real-time cycle timing test on the current master target.
void EcatClient::rtTestStart(int cycleUsec) {
  send("rtTestStart", {{"cycleUsec", cycleUsec}},
       [this](const QJsonObject &result) {
         emit rtTestTelemetry(result);
         emit commandSucceeded("RT stability test started");
       });
}

// Stop the RT stability test and release the IgH master.
void EcatClient::rtTestStop() {
  send("rtTestStop", {}, [this](const QJsonObject &result) {
    emit rtTestTelemetry(result);
    emit commandSucceeded("RT stability test stopped");
  });
}

// Poll current RT test state and telemetry without side effects.
void EcatClient::rtTestStatus() {
  send("rtTestStatus", {}, [this](const QJsonObject &result) {
    emit rtTestTelemetry(result);
  });
}

// Request DC sync diagnostics from the daemon.
void EcatClient::dcSyncStatus() {
  send("dcSyncStatus", {}, [this](const QJsonObject &result) {
    emit dcSyncStatusResult(result);
  });
}

// Query DC configuration from a slave's ESI XML descriptor.
void EcatClient::dcConfigure(int position) {
  send("dcConfigure", {{"position", position}},
       [this](const QJsonObject &result) {
         emit dcConfigureResult(result);
         emit commandSucceeded("DC configuration queried");
       });
}

// Activate DC synchronization with the specified reference clock slave.
void EcatClient::dcActivate(int refClockSlave) {
  send("dcActivate", {{"refClockSlave", refClockSlave}},
       [this](const QJsonObject &result) {
         emit dcActivateResult(result);
         emit commandSucceeded("DC activation requested");
       });
}

// Deactivate DC synchronization.
void EcatClient::dcDeactivate() {
  send("dcDeactivate", {}, [this](const QJsonObject &result) {
    emit dcDeactivateResult(result);
    emit commandSucceeded("DC deactivated");
  });
}

// Accumulate bytes and split on newlines to extract complete JSON response frames.
void EcatClient::readSocket() {
  buffer_ += socket_.readAll();
  int newline = -1;
  while ((newline = buffer_.indexOf('\n')) >= 0) {
    const auto line = buffer_.left(newline);
    buffer_.remove(0, newline + 1);
    handleLine(line);
  }
}

// Sends a JSON command to the daemon and returns a pending request ID
void EcatClient::send(const QString &method, const QJsonObject &params,
                      Handler handler) {
  // Stamp master target into params, assign a unique request ID, register
  // the response handler, and write the newline-delimited JSON frame.
  if (!isConnected()) {
    emit errorMessage("ecatd is not connected");
    // Fail the caller's handler so pending entries are not left orphaned.
    if (handler) {
      QJsonObject errorResult;
      errorResult["error"] = true;
      errorResult["errorMessage"] = "ecatd is not connected";
      errorResult["errorCode"] = -1;
      handler(errorResult);
    }
    return;
  }

  const QString id = QString::number(nextId_++);
  handlers_.insert(id, std::move(handler));
  requestTimestamps_.insert(id, QDateTime::currentMSecsSinceEpoch());
  QJsonObject scopedParams = params;
  scopedParams.insert("master", masterTarget_);
  socket_.write(
      JsonProtocol::encode(JsonProtocol::request(id, method, scopedParams)));
}

// Dispatch a response to the handler registered for this request ID.
// Emits errorMessage on protocol errors or daemon-side failures.
void EcatClient::handleLine(const QByteArray &line) {
  const auto document = QJsonDocument::fromJson(line);
  if (!document.isObject()) {
    emit errorMessage("Invalid response from ecatd");
    return;
  }

  const auto object = document.object();
  const QString id = object.value("id").toString();
  const auto handler = handlers_.take(id);
  requestTimestamps_.remove(id);
  if (!object.value("ok").toBool()) {
    const QJsonObject errorObj = object.value("error").toObject();
    const QString errorMsg = errorObj.value("message").toString("Unknown runtime error");
    // Include request ID in error for matching with specific requests.
    emit errorMessage(QString("[%1] %2").arg(id, errorMsg));
    // Also invoke the handler with the error info so consumers can handle failures.
    // The handler receives a JSON object with an "error" key indicating failure.
    if (handler) {
      QJsonObject errorResult;
      errorResult["error"] = true;
      errorResult["errorMessage"] = errorMsg;
      errorResult["errorCode"] = errorObj.value("code").toInt(-1);
      handler(errorResult);
    }
    return;
  }
  if (handler) {
    handler(object.value("result").toObject());
  }
}

// Cleans up requests that exceeded the timeout threshold
void EcatClient::sweepTimedOutRequests() {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    QStringList timedOut;
    // Iterate over collection
    for (auto it = requestTimestamps_.begin(); it != requestTimestamps_.end(); ++it) {
        if (now - it.value() > requestTimeoutMs_) {
            timedOut.append(it.key());
        }
    }
    // Iterate over collection
    for (const QString &id : timedOut) {
        // Take the handler out of the map before invoking it so the container
        // never holds a stale pointer and re-entrancy is safe.
        const auto handler = handlers_.take(id);
        requestTimestamps_.remove(id);
        const QString errorMsg =
            QString("Request %1 timed out after %2ms").arg(id).arg(requestTimeoutMs_);
        emit errorMessage(errorMsg);
        // Mirror handleLine(): deliver an error result to the pending handler
        // so callers don't wait forever for a response that will never arrive.
        if (handler) {
            QJsonObject errorResult;
            errorResult["error"] = true;
            errorResult["errorMessage"] = errorMsg;
            errorResult["errorCode"] = -1;
            handler(errorResult);
        }
    }
}

// Sets the timeout duration for daemon requests in milliseconds
void EcatClient::setRequestTimeout(int ms) {
    requestTimeoutMs_ = ms > 0 ? ms : kDefaultRequestTimeoutMs;
}

// ─── New daemon RPC methods ────────────────────────────────────────────────

// Query AL event log from the daemon (limit controls max entries returned).
void EcatClient::alEventLog(int limit) {
  send("alEventLog", {{"limit", limit}}, [this](const QJsonObject &result) {
    emit alEventLogResult(result);
  });
}

// Clear the daemon's AL event history.
void EcatClient::alEventClear() {
  send("alEventClear", {}, [this](const QJsonObject &) {
    emit commandSucceeded("AL event log cleared");
  });
}

// Enumerate network adapters available on the daemon host.
void EcatClient::listAdapters() {
  send("listAdapters", {}, [this](const QJsonObject &result) {
    emit adaptersListResult(result);
  });
}

// Switch the IgH master's network adapter (requires daemon restart).
void EcatClient::setAdapter(const QString &name) {
  send("setAdapter", {{"adapter", name}}, [this, name](const QJsonObject &) {
    emit commandSucceeded(QString("Adapter set to %1").arg(name));
  });
}

void EcatClient::setBackendMode(const QString &mode) {
  QJsonObject params;
  params["mode"] = mode;
  send("setBackend", params, [this](const QJsonObject &result) {
    QString backend = result.value("backend").toString();
    QString mode = result.value("mode").toString();
    emit backendModeChanged(backend, mode);
  });
}

// Read firmware from a slave using FoE protocol (daemon saves to filePath).
void EcatClient::foeRead(int position, const QString &filePath) {
  send("foeRead",
       {{"position", position}, {"filePath", filePath}},
       [this, position, filePath](const QJsonObject &result) {
         const qint64 fileSize = static_cast<qint64>(result.value("fileSize").toDouble());
         emit foeReadResult(position, filePath, fileSize);
         emit commandSucceeded(result.value("message").toString());
       });
}

// Write firmware to a slave using FoE protocol (daemon reads from filePath).
void EcatClient::foeWrite(int position, const QString &filePath, quint32 password) {
  QJsonObject params;
  params["position"] = position;
  params["filePath"] = filePath;
  if (password != 0) {
    params["password"] = static_cast<qint64>(password);
  }
  send("foeWrite", params,
       [this, position](const QJsonObject &result) {
         const qint64 bytesWritten = static_cast<qint64>(result.value("bytesWritten").toDouble());
         emit foeWriteResult(position, bytesWritten);
         emit commandSucceeded(result.value("message").toString());
       });
}

void EcatClient::getBackendMode() {
  send("getBackend", {}, [this](const QJsonObject &result) {
    QString backend = result.value("backend").toString();
    QString mode = result.value("mode").toString();
    emit backendModeChanged(backend, mode);
  });
}

// ─── SoE (Servo over EtherCAT) ────────────────────────────────────────────

// Read an SoE IDN from a slave.
void EcatClient::soeRead(int position, const QString &idn, int drive, const QString &type) {
  QJsonObject params;
  params["position"] = position;
  params["idn"] = idn;
  params["drive"] = drive;
  if (!type.isEmpty()) params["type"] = type;
  send("soeRead", params,
       [this, position, idn](const QJsonObject &result) {
         emit soeReadResult(position, idn, result.value("value").toString());
       });
}

// Write an SoE IDN to a slave.
void EcatClient::soeWrite(int position, const QString &idn, const QString &value,
                          int drive, const QString &type) {
  QJsonObject params;
  params["position"] = position;
  params["idn"] = idn;
  params["value"] = value;
  params["drive"] = drive;
  if (!type.isEmpty()) params["type"] = type;
  send("soeWrite", params,
       [this, position, idn](const QJsonObject &result) {
         emit soeWriteResult(position, idn);
         emit commandSucceeded(result.value("message").toString());
       });
}

// ─── EoE (Ethernet over EtherCAT) ─────────────────────────────────────────

// Query EoE support status for a slave.
void EcatClient::eoeStatus(int position) {
  send("eoeStatus", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit eoeStatusResult(position, result);
       });
}

// Configure IP address for an EoE-capable slave.
void EcatClient::eoeConfigureIp(int position, const QString &ip, const QString &subnet,
                                 const QString &gateway, const QString &dns) {
  QJsonObject params;
  params["position"] = position;
  params["ip"] = ip;
  params["subnet"] = subnet;
  if (!gateway.isEmpty()) params["gateway"] = gateway;
  if (!dns.isEmpty()) params["dns"] = dns;
  send("eoeConfigureIp", params,
       [this, position, ip](const QJsonObject &result) {
         emit eoeIpConfigured(position, ip);
         emit commandSucceeded(result.value("message").toString());
       });
}

// Read current IP configuration from an EoE slave.
void EcatClient::eoeGetIp(int position) {
  send("eoeGetIp", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit eoeIpResult(position, result);
       });
}

// Get EoE statistics for a slave.
void EcatClient::eoeStats(int position) {
  send("eoeStats", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit eoeStatsResult(position, result);
       });
}

// ─── Redundancy ───────────────────────────────────────────────────────────

// Query redundancy status.
void EcatClient::redundancyStatus() {
  send("redundancyStatus", {},
       [this](const QJsonObject &result) {
         emit redundancyStatusResult(result);
       });
}

// Enable cable redundancy.
void EcatClient::redundancyEnable() {
  send("redundancyEnable", {},
       [this](const QJsonObject &result) {
         bool success = result.value("success").toBool();
         emit redundancyCommandResult("enable", success, result.value("message").toString());
       });
}

// Disable cable redundancy.
void EcatClient::redundancyDisable() {
  send("redundancyDisable", {},
       [this](const QJsonObject &result) {
         bool success = result.value("success").toBool();
         emit redundancyCommandResult("disable", success, result.value("message").toString());
       });
}

// Perform failover to secondary path.
void EcatClient::redundancyFailover() {
  send("redundancyFailover", {},
       [this](const QJsonObject &result) {
         bool success = result.value("success").toBool();
         emit redundancyCommandResult("failover", success, result.value("message").toString());
       });
}

// Perform failback to primary path.
void EcatClient::redundancyFailback() {
  send("redundancyFailback", {},
       [this](const QJsonObject &result) {
         bool success = result.value("success").toBool();
         emit redundancyCommandResult("failback", success, result.value("message").toString());
       });
}

// Get redundancy event history.
void EcatClient::redundancyHistory(int limit) {
  send("redundancyHistory", {{"limit", limit}},
       [this](const QJsonObject &result) {
         emit redundancyHistoryResult(result);
       });
}

// ─── Online Change ────────────────────────────────────────────────────────

// Preview an online change without applying it.
void EcatClient::onlineChangePreview(const QJsonArray &changes) {
  send("onlineChangePreview", {{"changes", changes}},
       [this](const QJsonObject &result) {
         emit onlineChangePreviewResult(result);
       });
}

// Apply an online change.
void EcatClient::onlineChangeApply(const QJsonArray &changes, const QString &targetState) {
  send("onlineChangeApply", {{"changes", changes}, {"targetState", targetState}},
       [this](const QJsonObject &result) {
         emit onlineChangeApplyResult(result);
       });
}

// Query online change status.
void EcatClient::onlineChangeStatus() {
  send("onlineChangeStatus", {},
       [this](const QJsonObject &result) {
         emit onlineChangeStatusResult(result);
       });
}

// ─── Auto-reconnect ────────────────────────────────────────────────────────

// Enable or disable the automatic reconnect mechanism.
void EcatClient::enableAutoReconnect(bool enable) {
  autoReconnectEnabled_ = enable;
  if (!enable) {
    if (reconnectTimer_) reconnectTimer_->stop();
    if (heartbeatTimer_) heartbeatTimer_->stop();
    consecutiveFailures_ = 0;
    connectFailures_ = 0;
  } else if (connectionState_ == ConnectionState::Connected) {
    setupAutoReconnect();
  }
}

bool EcatClient::autoReconnectEnabled() const {
  return autoReconnectEnabled_;
}

// Initialize heartbeat + reconnect timers. Called after successful connect.
void EcatClient::setupAutoReconnect() {
  // Heartbeat: ping daemon every 5s to detect silent disconnects.
  if (!heartbeatTimer_) {
    heartbeatTimer_ = new QTimer(this);
    connect(heartbeatTimer_, &QTimer::timeout, this, [this]() {
      if (connectionState_ != ConnectionState::Connected) return;
      if (pendingPing_) return;  // Don't send another ping while one is pending.
      pendingPing_ = true;
      send("ping", {}, [this](const QJsonObject &result) {
        Q_UNUSED(result);
        pendingPing_ = false;
        consecutiveFailures_ = 0;  // Reset on successful pong.
      });
      // If no pong arrives within 3.5s, count as failure.
      QTimer::singleShot(3500, this, [this]() {
        if (connectionState_ != ConnectionState::Connected) return;
        if (pendingPing_) {
          pendingPing_ = false;
          consecutiveFailures_++;
          if (consecutiveFailures_ >= kMaxConsecutiveFailures) {
            // The TCP link is alive but the daemon is unreachable. Declare the
            // connection dead and abort the socket so the normal disconnect
            // path runs (handler flush, disconnected/connectionLost signals,
            // reconnect scheduling) instead of leaving a permanent phantom
            // Disconnected state.  State is set first so the reconnect cannot
            // be skipped even if no disconnected event is delivered.
            setConnectionState(ConnectionState::Disconnected);
            socket_.abort();
            // Belt-and-suspenders in case the socket was already dead:
            // make sure a reconnect is armed even if no disconnected event
            // was delivered.
            scheduleReconnect();
          }
        }
      });
    });
  }
  heartbeatTimer_->start(5000);

  // Reconnect timer: fires with exponential backoff when disconnected.
  if (!reconnectTimer_) {
    reconnectTimer_ = new QTimer(this);
    reconnectTimer_->setSingleShot(true);
    connect(reconnectTimer_, &QTimer::timeout, this, &EcatClient::attemptReconnect);
  }
  consecutiveFailures_ = 0;
  reconnectIntervalMs_ = 2000;
}

// Capture whether a reconnect should be armed, avoiding double-starts when a
// disconnect is signalled by more than one path (socket disconnected vs. heartbeat).
void EcatClient::scheduleReconnect() {
  if (!autoReconnectEnabled_) return;
  if (!reconnectTimer_) return;
  if (reconnectTimer_->isActive()) return;
  reconnectTimer_->start(reconnectIntervalMs_);
}

// Attempt to reconnect to the configured daemon host with exponential backoff.
void EcatClient::attemptReconnect() {
  if (!autoReconnectEnabled_) return;
  if (connectionState_ == ConnectionState::Connected) return;

  if (connectFailures_ >= kMaxConnectAttempts) {
    emit reconnectFailed(kMaxConnectAttempts);
    return;
  }

  emit reconnecting(connectFailures_ + 1, reconnectIntervalMs_);
  setConnectionState(ConnectionState::Reconnecting);
  setupConnectTimeout();
  socket_.connectToHost(host_, port_);

  // Schedule next attempt if this one doesn't succeed within 5s.  The
  // connect-failure accounting and the reconnectFailed signal live in
  // errorOccurred / setupConnectTimeout, so a failed attempt is counted
  // exactly once.  Here we only advance the backoff and re-arm the timer.
  QTimer::singleShot(5000, this, [this]() {
    if (connectionState_ != ConnectionState::Connected && autoReconnectEnabled_ && connectFailures_ < kMaxConnectAttempts) {
      // Exponential backoff: 2s → 4s → 8s → 16s → 30s (cap).
      reconnectIntervalMs_ = qMin(reconnectIntervalMs_ * 2, kMaxReconnectMs);
      scheduleReconnect();
    }
  });
}

