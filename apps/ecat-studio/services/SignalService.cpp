// SignalService — implementation.  See header for interface documentation.
#include "SignalService.h"
#include "services/EventBus.h"

#include <QTimer>
#include <QtMath>

SignalService::SignalService(EventBus* bus, QObject* parent) : QObject(parent), bus_(bus) {
    // Receive data pushed through the EventBus by external sources.
    connect(bus_, &EventBus::signalData, this, &SignalService::handleSignalData);

    pollTimer_ = new QTimer(this);
    connect(pollTimer_, &QTimer::timeout, this, [this]() {
        // Placeholder: when EcatClient gains signal-poll methods, forward here.
        // For now the timer does nothing — data arrives via EventBus.
        Q_UNUSED(this);
    });
}

// ── Channel management ────────────────────────────────────────────────

int SignalService::addChannel(const QString& name, int slave, const QString& idx, const QString& sub) {
    SignalChannelInfo ch;
    ch.id = nextId_++;
    ch.name = name;
    ch.slave = slave;
    ch.index = idx;
    ch.subIndex = sub;
    ch.values.reserve(SignalChannelInfo::kMaxPoints);
    ch.timestamps.reserve(SignalChannelInfo::kMaxPoints);
    channels_.append(ch);
    emit channelAdded(ch.id);
    return ch.id;
}

void SignalService::removeChannel(int channelId) {
    for (int i = 0; i < channels_.size(); ++i) {
        if (channels_[i].id == channelId) {
            channels_.removeAt(i);
            emit channelRemoved(channelId);
            return;
        }
    }
}

QVector<SignalChannelInfo> SignalService::channels() const {
    return channels_;
}

// ── Statistics ────────────────────────────────────────────────────────

ChannelStats SignalService::stats(int channelId) const {
    ChannelStats s;
    for (const auto& ch : channels_) {
        if (ch.id != channelId || ch.values.isEmpty())
            return s;

        double lo = ch.values.first();
        double hi = lo;
        double sum = 0.0;
        for (double v : ch.values) {
            if (v < lo)
                lo = v;
            if (v > hi)
                hi = v;
            sum += v;
        }
        const int n = ch.values.size();
        s.min = lo;
        s.max = hi;
        s.avg = sum / n;

        // Standard deviation (population).
        double variance = 0.0;
        for (double v : ch.values) {
            const double d = v - s.avg;
            variance += d * d;
        }
        s.stddev = qSqrt(variance / n);
        break;
    }
    return s;
}

// ── Polling ───────────────────────────────────────────────────────────

void SignalService::startPolling(int intervalMs) {
    pollTimer_->setInterval(intervalMs);
    pollTimer_->start();
}

void SignalService::stopPolling() {
    pollTimer_->stop();
}

// ── Data injection ────────────────────────────────────────────────────

void SignalService::pushData(int channelId, const QVector<double>& values, const QVector<qint64>& timestamps) {
    for (auto& ch : channels_) {
        if (ch.id == channelId) {
            ch.values.append(values);
            ch.timestamps.append(timestamps);
            // Trim to max if we exceeded.
            while (ch.values.size() > SignalChannelInfo::kMaxPoints) {
                ch.values.removeFirst();
                ch.timestamps.removeFirst();
            }
            emit channelDataUpdated(channelId);
            return;
        }
    }
}

// ── EventBus handler ──────────────────────────────────────────────────

void SignalService::handleSignalData(int channel, const QVector<double>& values, const QVector<qint64>& timestamps) {
    pushData(channel, values, timestamps);
}
