#pragma once

// JSON-over-TCP client for communicating with the ecatd runtime daemon.


#include "EthercatTypes.h"

#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>
#include <functional>

class EcatClient : public QObject {
  Q_OBJECT

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

signals:
  void connected();
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

private slots:
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
};
