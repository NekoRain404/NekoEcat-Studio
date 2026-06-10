#include "EcatClient.h"

#include "JsonProtocol.h"

#include <QJsonDocument>
#include <QJsonObject>

EcatClient::EcatClient(QObject *parent) : QObject(parent) {
  connect(&socket_, &QTcpSocket::connected, this, &EcatClient::connected);
  connect(&socket_, &QTcpSocket::disconnected, this, &EcatClient::disconnected);
  connect(&socket_, &QTcpSocket::readyRead, this, &EcatClient::readSocket);
  connect(&socket_, &QTcpSocket::errorOccurred, this,
          [this](QAbstractSocket::SocketError) {
            emit errorMessage(socket_.errorString());
          });
}

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

QString EcatClient::masterTarget() const { return masterTarget_; }

void EcatClient::setMasterTarget(const QString &target) {
  const QString trimmed = target.trimmed();
  masterTarget_ = trimmed.isEmpty() ? "0" : trimmed;
}

void EcatClient::ping() {
  send("ping", {}, [this](const QJsonObject &result) {
    emit daemonInfo(QString("%1 %2").arg(result.value("name").toString(),
                                         result.value("version").toString()));
  });
}

void EcatClient::hostDiagnostics() {
  send("hostDiagnostics", {}, [this](const QJsonObject &result) {
    emit hostDiagnosticsReady(result.value("checks").toArray());
  });
}

void EcatClient::master() {
  send("master", {}, [this](const QJsonObject &result) {
    emit masterText(result.value("text").toString());
  });
}

void EcatClient::scan() {
  send("scan", {}, [this](const QJsonObject &result) {
    emit slavesChanged(slavesFromJson(result.value("slaves").toArray()));
  });
}

void EcatClient::rescan() {
  send("rescan", {}, [this](const QJsonObject &) {
    emit commandSucceeded("Bus rescan requested");
    scan();
  });
}

void EcatClient::slaveInfo(int position) {
  send("slaveInfo", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("Info", position,
                              result.value("text").toString());
       });
}

void EcatClient::pdos(int position) {
  send("pdos", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("PDO", position, result.value("text").toString());
       });
}

void EcatClient::sdos(int position) {
  send("sdos", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("SDO", position, result.value("text").toString());
       });
}

void EcatClient::xml(int position) {
  send("xml", {{"position", position}},
       [this, position](const QJsonObject &result) {
         emit slaveTextResult("ESI XML", position,
                              result.value("text").toString());
       });
}

void EcatClient::upload(int position, const QString &index,
                        const QString &subIndex) {
  send("upload",
       {{"position", position}, {"index", index}, {"subIndex", subIndex}},
       [this, position, index, subIndex](const QJsonObject &result) {
         emit sdoValue(position, index, subIndex,
                       result.value("value").toString());
       });
}

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

void EcatClient::setState(int position, const QString &state) {
  send("setState", {{"position", position}, {"state", state}},
       [this, state](const QJsonObject &) {
         emit commandSucceeded(QString("State request sent: %1").arg(state));
         scan();
       });
}

void EcatClient::setAllStates(const QString &state) {
  send("setAllStates", {{"state", state}}, [this, state](const QJsonObject &) {
    emit commandSucceeded(QString("All-state request sent: %1").arg(state));
    scan();
  });
}

void EcatClient::freeRunStart() {
  send("freeRunStart", {}, [this](const QJsonObject &result) {
    emit freeRunChanged(true, result.value("status").toString("Running"));
    emit freeRunTelemetry(result);
    emit commandSucceeded("Free Run started");
  });
}

void EcatClient::freeRunStop() {
  send("freeRunStop", {}, [this](const QJsonObject &result) {
    emit freeRunChanged(false, result.value("status").toString("Stopped"));
    emit freeRunTelemetry(result);
    emit commandSucceeded("Free Run stopped");
  });
}

void EcatClient::freeRunStatus() {
  send("freeRunStatus", {}, [this](const QJsonObject &result) {
    emit freeRunChanged(result.value("running").toBool(),
                        result.value("status").toString());
    emit freeRunTelemetry(result);
  });
}

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
