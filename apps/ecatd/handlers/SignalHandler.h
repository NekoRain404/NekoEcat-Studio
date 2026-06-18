#pragma once
// SignalHandler — multi-channel ring buffer for real-time signal data.
// Stores up to 10,000 samples per channel, supports subscribe/unsubscribe/poll.

#include <QJsonObject>
#include <QVector>
#include <QString>
#include <QMutex>
#include <cstdint>

struct SignalSample {
  double value = 0.0;
  int64_t timestampMs = 0;
};

struct SignalChannel {
  int id = -1;
  QString name;
  int slave = -1;
  QString index;
  QString subIndex;
  QVector<SignalSample> samples;
  int maxSamples = 10000;
};

class SignalHandler {
public:
  // Subscribe a new signal channel. Returns channel ID.
  int subscribe(const QString &name, int slave, const QString &index, const QString &subIndex);

  // Unsubscribe a channel.
  void unsubscribe(int channelId);

  // Push data to a channel (called by FreeRunController or periodic poll).
  void push(int channelId, double value, int64_t timestampMs = 0);

  // Handle JSON-RPC signalPoll request.
  QJsonObject handlePoll(const QString &id, const QJsonObject &params);

  // Return list of active channels.
  QVector<SignalChannel> channels() const;

private:
  mutable QMutex mutex_;
  QVector<SignalChannel> channels_;
  int nextChannelId_ = 1;
  static constexpr int kMaxSamples = 10000;
};
