#pragma once

// EventBus — central signal hub for inter-plugin communication.
// Plugins emit events through the bus rather than knowing each other.

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QString>
#include "EthercatTypes.h"

class EventBus : public QObject {
  Q_OBJECT
public:
  explicit EventBus(QObject *parent = nullptr);

  void emitSlaveChanged(const QVector<SlaveInfo> &slaves);
  void emitSdoValue(int pos, const QString &idx, const QString &sub, const QString &val);
  void emitConnectionStateChanged(bool connected);
  void emitFreeRunTelemetry(const QJsonObject &tel);
  void emitTopologyChanged(const QVector<SlaveInfo> &slaves);
  void emitDcSyncUpdate(const QJsonObject &data);
  void emitAlEvent(const QJsonObject &event);
  void emitSignalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);

signals:
  void slaveChanged(const QVector<SlaveInfo> &slaves);
  void sdoValueReceived(int position, const QString &index, const QString &subIndex, const QString &value);
  void connectionStateChanged(bool connected);
  void freeRunTelemetry(const QJsonObject &telemetry);
  void topologyChanged(const QVector<SlaveInfo> &slaves);
  void dcSyncUpdate(const QJsonObject &data);
  void alEvent(const QJsonObject &event);
  void signalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps);
};
