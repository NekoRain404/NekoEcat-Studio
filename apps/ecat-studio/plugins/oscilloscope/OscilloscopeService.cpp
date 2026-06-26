// OscilloscopeService — implementation.  See header for interface documentation.
#include "OscilloscopeService.h"

OscilloscopeService::OscilloscopeService(QObject *parent)
    : QObject(parent) {}

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
}

void OscilloscopeService::stopAcquisition() {
  acquiring_ = false;
}

bool OscilloscopeService::isAcquiring() const { return acquiring_; }
