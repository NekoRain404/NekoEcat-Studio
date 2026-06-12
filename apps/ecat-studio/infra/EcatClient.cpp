// JSON-over-TCP client for communicating with the ecatd runtime daemon.
#include "EcatClient.h"

#include "JsonProtocol.h"

#include <QJsonDocument>
#include <QJsonObject>

EcatClient::EcatClient(QObject *parent) : QObject(parent) {
  // Wire Qt socket signals for connection lifecycle and incoming data.
  connect(&socket_, &QTcpSocket::connected, this, &EcatClient::connected);
  connect(&socket_, &QTcpSocket::disconnected, this, &EcatClient::disconnected);
  connect(&socket_, &QTcpSocket::readyRead, this, &EcatClient::readSocket);
  connect(&socket_, &QTcpSocket::errorOccurred, this,
          [this](QAbstractSocket::SocketError) {
            emit errorMessage(socket_.errorString());
          });
}

// Connect to ecatd's localhost TCP port; no-op if already connected or connecting.
void EcatClient::connectToDaemon() {
  if (socket_.state() == QAbstractSocket::ConnectedState ||
      socket_.state() == QAbstractSocket::ConnectingState) {
    return;
  }
  socket_.connectToHost(QHostAddress::LocalHost, 5877);
}

bool EcatClient::isConnected() const {
  return socket_.state() == QAbstractSocket::ConnectedState;
}

// The IgH master index injected into every request's params as "master".
// Defaults to "0" for single-master installations.
QString EcatClient::masterTarget() const { return masterTarget_; }

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
       [this, position, index, subIndex](const QJsonObject &) {
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
       [this, state](const QJsonObject &) {
         emit commandSucceeded(QString("State request sent: %1").arg(state));
         scan();
       });
}

// Broadcast AL state transition to all slaves, then auto-rescan.
void EcatClient::setAllStates(const QString &state) {
  send("setAllStates", {{"state", state}}, [this, state](const QJsonObject &) {
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

void EcatClient::send(const QString &method, const QJsonObject &params,
                      Handler handler) {
  // Stamp master target into params, assign a unique request ID, register
  // the response handler, and write the newline-delimited JSON frame.
  if (!isConnected()) {
    emit errorMessage("ecatd is not connected");
    return;
  }

  const QString id = QString::number(nextId_++);
  handlers_.insert(id, std::move(handler));
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
  if (!object.value("ok").toBool()) {
    emit errorMessage(
        object.value("error").toObject().value("message").toString(
            "Unknown runtime error"));
    return;
  }
  if (handler) {
    handler(object.value("result").toObject());
  }
}
