#include "EventBus.h"

EventBus::EventBus(QObject *parent) : QObject(parent) {}

void EventBus::emitSlaveChanged(const QVector<SlaveInfo> &slaves) { emit slaveChanged(slaves); }
void EventBus::emitSdoValue(int pos, const QString &idx, const QString &sub, const QString &val) { emit sdoValueReceived(pos, idx, sub, val); }
void EventBus::emitConnectionStateChanged(bool connected) { emit connectionStateChanged(connected); }
void EventBus::emitFreeRunTelemetry(const QJsonObject &tel) { emit freeRunTelemetry(tel); }
void EventBus::emitTopologyChanged(const QVector<SlaveInfo> &slaves) { emit topologyChanged(slaves); }
void EventBus::emitDcSyncUpdate(const QJsonObject &data) { emit dcSyncUpdate(data); }
void EventBus::emitAlEvent(const QJsonObject &event) { emit alEvent(event); }
void EventBus::emitSignalData(int channel, const QVector<double> &values, const QVector<qint64> &timestamps) { emit signalData(channel, values, timestamps); }
