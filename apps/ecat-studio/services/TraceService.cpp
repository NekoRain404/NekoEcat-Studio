#include "TraceService.h"

// TraceService.cpp — Multi-channel trace configuration with fail-closed capture.
//
// Implementation notes:
//   - Supports up to kMaxChannels simultaneous trace channels with auto-naming
//   - Capture is intentionally disabled until a real backend is wired

TraceService::TraceService(QObject *parent)
    : QObject(parent), timer_(new QTimer(this)) {
  connect(timer_, &QTimer::timeout, this, &TraceService::tick);
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
  if (!tracing_) return;
}
