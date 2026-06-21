#include "TraceService.h"
#include <QRandomGenerator>
#include <QtMath>

// TraceService.cpp — Multi-channel data tracing with configurable sample rate and triggers
//
// Implementation notes:
//   - Supports up to kMaxChannels simultaneous trace channels with auto-naming
//   - Timer-driven tick generates synthetic trace data (sine + noise) for demonstration
//   - Ring buffer per channel capped at bufferSize; emits traceDataUpdated per tick

TraceService::TraceService(QObject *parent)
    : QObject(parent), timer_(new QTimer(this)) {
  connect(timer_, &QTimer::timeout, this, &TraceService::tick);
  elapsed_.start();
}

int TraceService::addChannel(const QString &name, int slave,
                             const QString &index, const QString &subIndex) {
  if (channels_.size() >= kMaxChannels) return -1;

  TraceChannelConfig ch;
  ch.id = nextId_++;
  ch.slave = slave;
  ch.index = index;
  ch.subIndex = subIndex;
  ch.name = name.isEmpty()
      ? QStringLiteral("CH%1 [%2:%3.%4]").arg(ch.id).arg(slave).arg(index, subIndex)
      : name;
  channels_.append(ch);
  emit channelAdded(ch.id);
  return ch.id;
}

void TraceService::removeChannel(int channelId) {
  for (int i = 0; i < channels_.size(); ++i) {
    if (channels_[i].id == channelId) {
      channels_.removeAt(i);
      emit channelRemoved(channelId);
      return;
    }
  }
}

QVector<TraceChannelConfig> TraceService::channels() const {
  return channels_;
}

void TraceService::startTrace() {
  if (tracing_) return;
  tracing_ = true;
  tickCount_ = 0;
  elapsed_.start();
  timer_->start(1000 / sampleRate_);
  emit traceStarted();
}

void TraceService::stopTrace() {
  tracing_ = false;
  timer_->stop();
  emit traceStopped();
}

bool TraceService::isTracing() const { return tracing_; }

void TraceService::setSampleRate(int rate) {
  sampleRate_ = qMax(1, qMin(rate, 100000));
  if (tracing_) {
    timer_->setInterval(1000 / sampleRate_);
  }
}

int TraceService::sampleRate() const { return sampleRate_; }

void TraceService::setBufferSize(int size) {
  bufferSize_ = qMax(100, qMin(size, 1000000));
}

int TraceService::bufferSize() const { return bufferSize_; }

void TraceService::setTriggerMode(TraceTriggerMode mode) {
  triggerMode_ = mode;
}

TraceTriggerMode TraceService::triggerMode() const { return triggerMode_; }

QVector<TracePoint> TraceService::getTraceData(int channelId) const {
  for (const auto &ch : channels_) {
    if (ch.id == channelId) return ch.data;
  }
  return {};
}

void TraceService::tick() {
  ++tickCount_;
  const qint64 now = elapsed_.elapsed();

  for (auto &ch : channels_) {
    TracePoint pt;
    pt.timestamp = now;
    pt.channelId = ch.id;
    pt.value = qSin(tickCount_ * 0.01 * (1.0 + ch.id * 0.3)) * 50.0
               + (QRandomGenerator::global()->bounded(100) - 50) * 0.1;
    pt.quality = 100 - (QRandomGenerator::global()->bounded(5));

    ch.data.append(pt);
    if (ch.data.size() > bufferSize_) {
      ch.data.remove(0, ch.data.size() - bufferSize_);
    }

    emit traceDataUpdated(ch.id, ch.data);
  }
}
