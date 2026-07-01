#include "TraceService.h"
#include "infra/EcatClient.h"

#include <QDateTime>

// TraceService.cpp — Multi-channel trace data collection via SDO upload.
//
// Implementation notes:
//   - Supports up to kMaxChannels simultaneous trace channels with auto-naming
//   - Data collection uses periodic SDO upload requests via EcatClient
//   - Each trace channel is bound to a slave SDO entry (position/index/subIndex)
//   - Buffer wraps around when full, discarding oldest samples

TraceService::TraceService(EcatClient *client, QObject *parent)
    : QObject(parent), timer_(new QTimer(this)), client_(client) {
  connect(timer_, &QTimer::timeout, this, &TraceService::tick);
  if (client_) {
    connect(client_, &EcatClient::sdoValue,
            this, &TraceService::onSdoValue);
  }
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
  int interval = 1000 / qMax(1, sampleRate_);
  timer_->start(interval);
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
  if (!tracing_ || !client_) return;

  pendingRequests_.clear();
  for (const auto &ch : channels_) {
    QString key = QStringLiteral("%1:%2:%3")
                      .arg(ch.slave)
                      .arg(ch.index)
                      .arg(ch.subIndex);
    pendingRequests_.insert(key, ch.id);
    client_->upload(ch.slave, ch.index, ch.subIndex);
  }
}

void TraceService::onSdoValue(int position, const QString &index,
                               const QString &subIndex, const QString &value) {
  if (!tracing_) return;

  QString key = QStringLiteral("%1:%2:%3")
                    .arg(position)
                    .arg(index)
                    .arg(subIndex);
  auto it = pendingRequests_.find(key);
  if (it == pendingRequests_.end()) return;

  int channelId = it.value();
  pendingRequests_.erase(it);

  // Find the matching channel and append the data point.
  for (auto &ch : channels_) {
    if (ch.id != channelId) continue;
    if (ch.slave != position || ch.index != index || ch.subIndex != subIndex)
      continue;

    TracePoint pt;
    pt.timestamp = QDateTime::currentMSecsSinceEpoch();
    pt.value = value.toDouble();
    pt.channelId = channelId;
    pt.quality = 100;

    ch.data.append(pt);

    // Trim oldest samples when buffer exceeds the configured size.
    while (ch.data.size() > bufferSize_) {
      ch.data.removeFirst();
    }

    emit traceDataUpdated(ch.id, ch.data);
    return;
  }
}
