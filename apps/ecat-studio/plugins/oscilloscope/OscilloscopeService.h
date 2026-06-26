#pragma once

// OscilloscopeService — acquisition backend for the Oscilloscope plugin.
// Manages up to 8 channels, each bound to a slave SDO entry.
// Until a real acquisition backend is wired, startAcquisition() only tracks
// acquisition state and does not synthesize waveform samples.

#include <QObject>
#include <QVector>
#include <QString>
#include <QTimer>

enum class OscTriggerMode { Auto, Normal, Single };

struct OscChannelConfig {
  int id = -1;
  int slave = 0;
  QString index;
  QString subIndex;
  QString name;
  QVector<double> data;
};

class OscilloscopeService : public QObject {
  Q_OBJECT
public:
  explicit OscilloscopeService(QObject *parent = nullptr);

  int addChannel(int slave, const QString &index, const QString &subIndex);
  void removeChannel(int channelId);
  QVector<OscChannelConfig> channels() const;

  void setTimebase(int msPerDiv);
  int timebase() const;

  void setTriggerMode(OscTriggerMode mode);
  OscTriggerMode triggerMode() const;

  void setTriggerLevel(double level);
  double triggerLevel() const;

  void startAcquisition();
  void stopAcquisition();
  bool isAcquiring() const;

  static constexpr int kMaxChannels = 8;

signals:
  void waveformUpdated(int channelId, const QVector<double> &data);
  void channelAdded(int channelId);
  void channelRemoved(int channelId);

private:
  QVector<OscChannelConfig> channels_;
  int nextId_ = 1;
  int timebaseMs_ = 100;
  OscTriggerMode triggerMode_ = OscTriggerMode::Auto;
  double triggerLevel_ = 0.0;
  bool acquiring_ = false;
};
