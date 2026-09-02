#include "MockEventBus.h"

MockEventBus::MockEventBus(QObject* parent) : EventBus(parent) {
    connectSignals();
}

QVector<SignalRecord> MockEventBus::records() const {
    return records_;
}

int MockEventBus::signalCount(const QString& name) const {
    int count = 0;
    for (const auto& r : records_) {
        if (r.name == name)
            count++;
    }
    return count;
}

void MockEventBus::clearRecords() {
    records_.clear();
}

void MockEventBus::setDelayMs(int ms) {
    delayMs_ = ms;
}

void MockEventBus::recordSignal(const QString& name, const QVariantList& args) {
    if (delayMs_ > 0)
        QThread::msleep(delayMs_);
    records_.append({name, args});
}

void MockEventBus::connectSignals() {
    connect(this, &EventBus::slaveChanged, this, &MockEventBus::onSlaveChanged);
    connect(this, &EventBus::sdoValueReceived, this, &MockEventBus::onSdoValueReceived);
    connect(this, &EventBus::connectionStateChanged, this, &MockEventBus::onConnectionStateChanged);
    connect(this, &EventBus::topologyChanged, this, &MockEventBus::onTopologyChanged);
}

void MockEventBus::onSlaveChanged(const QVector<SlaveInfo>& slaves) {
    recordSignal("slaveChanged", {QVariant::fromValue(slaves)});
}

void MockEventBus::onSdoValueReceived(int pos, const QString& idx, const QString& sub, const QString& val) {
    recordSignal("sdoValueReceived", {pos, idx, sub, val});
}

void MockEventBus::onConnectionStateChanged(bool connected) {
    recordSignal("connectionStateChanged", {connected});
}

void MockEventBus::onTopologyChanged(const QVector<SlaveInfo>& slaves) {
    recordSignal("topologyChanged", {QVariant::fromValue(slaves)});
}
