// OscilloscopeService — implementation.  See header for interface documentation.
#include "OscilloscopeService.h"

#include <QRandomGenerator>
#include <QtMath>

OscilloscopeService::OscilloscopeService(QObject *parent)
    : QObject(parent), timer_(new QTimer(this)) {
  connect(timer_, &QTimer::timeout, this, &OscilloscopeService::tick);
}

int OscilloscopeService::addChannel(int slave, const QString &index,
                                     const QString &subIndex) {
  if (channels_.size() >= kMaxChannels) return -1;

  OscChannelConfig ch;
  ch.id = nextId_++;
  ch.slave = slave;
  ch.index = index;
  ch.subIndex = subIndex;
  ch.name = QStringLiteral("CH%1 [%2:%3.%4]")
                .arg(ch.id).arg(slave).arg(index, subIndex);
  channels_.append(ch);
  emit channelAdded(ch.id);
  return ch.id;
}

void OscilloscopeService::removeChannel(int channelId) {
  for (int i = 0; i < channels_.size(); ++i) {
    if (channels_[i].id == channelId) {
      channels_.removeAt(i);
      emit channelRemoved(channelId);
      return;
    }
  }
}

QVector<OscChannelConfig> OscilloscopeService::channels() const {
  return channels_;
}

void OscilloscopeService::setTimebase(int msPerDiv) {
  timebaseMs_ = qMax(1, msPerDiv);
  if (acquiring_) {
    timer_->setInterval(timebaseMs_);
  }
}

int OscilloscopeService::timebase() const { return timebaseMs_; }

void OscilloscopeService::setTriggerMode(OscTriggerMode mode) {
  triggerMode_ = mode;
}

OscTriggerMode OscilloscopeService::triggerMode() const {
  return triggerMode_;
}

void OscilloscopeService::setTriggerLevel(double level) {
  triggerLevel_ = level;
}

double OscilloscopeService::triggerLevel() const {
  return triggerLevel_;
}

void OscilloscopeService::startAcquisition() {
  if (acquiring_) return;
  acquiring_ = true;
  tickCount_ = 0;
  timer_->start(timebaseMs_);
}

void OscilloscopeService::stopAcquisition() {
  acquiring_ = false;
  timer_->stop();
}

bool OscilloscopeService::isAcquiring() const { return acquiring_; }

void OscilloscopeService::tick() {
  ++tickCount_;
  for (auto &ch : channels_) {
    // Generate synthetic sine + noise data for demonstration.
    const double t = tickCount_ * timebaseMs_ / 1000.0;
    const double val = qSin(t * 2.0 * M_PI * (1.0 + ch.id * 0.5)) * 50.0
                       + (QRandomGenerator::global()->bounded(100) - 50) * 0.1;
    ch.data.append(val);
    // Keep a rolling window of 2000 samples.
    if (ch.data.size() > 2000) {
      ch.data.remove(0, ch.data.size() - 2000);
    }
    emit waveformUpdated(ch.id, ch.data);
  }
}
