// SignalHandler — multi-channel ring buffer for real-time signal data.
#include "SignalHandler.h"

#include "CommandDispatcher.h"

#include <QJsonArray>
#include <QJsonObject>

// ─── Subscribe ─────────────────────────────────────────────────────────────

int SignalHandler::subscribe(const QString &name, int slave,
                             const QString &index, const QString &subIndex)
{
  const QMutexLocker lock(&mutex_);
  SignalChannel ch;
  ch.id = nextChannelId_++;
  ch.name = name;
  ch.slave = slave;
  ch.index = index;
  ch.subIndex = subIndex;
  ch.maxSamples = kMaxSamples;
  channels_.append(ch);
  return ch.id;
}

// ─── Unsubscribe ───────────────────────────────────────────────────────────

void SignalHandler::unsubscribe(int channelId)
{
  const QMutexLocker lock(&mutex_);
  for (int i = 0; i < channels_.size(); ++i) {
    if (channels_[i].id == channelId) {
      channels_.removeAt(i);
      return;
    }
  }
}

// ─── Push sample ───────────────────────────────────────────────────────────

void SignalHandler::push(int channelId, double value, int64_t timestampMs)
{
  const QMutexLocker lock(&mutex_);
  for (auto &ch : channels_) {
    if (ch.id == channelId) {
      SignalSample sample;
      sample.value = value;
      sample.timestampMs = timestampMs;
      ch.samples.append(sample);
      // Evict oldest samples when the ring buffer exceeds capacity.
      while (ch.samples.size() > ch.maxSamples) {
        ch.samples.removeFirst();
      }
      return;
    }
  }
}

// ─── handlePoll (JSON-RPC entry point) ─────────────────────────────────────

QJsonObject SignalHandler::handlePoll(const QString &id, const QJsonObject &params)
{
  const int64_t since = static_cast<int64_t>(params.value("since").toDouble(0));

  const QMutexLocker lock(&mutex_);

  QJsonArray channelArr;
  for (const auto &ch : channels_) {
    QJsonArray sampleArr;
    for (const auto &s : ch.samples) {
      // Filter by timestamp when a "since" value is provided.
      if (since > 0 && s.timestampMs <= since) {
        continue;
      }
      QJsonObject sampleObj;
      sampleObj["value"] = s.value;
      sampleObj["ts"] = static_cast<qint64>(s.timestampMs);
      sampleArr.append(sampleObj);
    }

    QJsonObject channelObj;
    channelObj["id"] = ch.id;
    channelObj["name"] = ch.name;
    channelObj["samples"] = sampleArr;
    channelArr.append(channelObj);
  }

  return CommandDispatcher::success(id, {{"channels", channelArr}});
}

// ─── channels accessor ─────────────────────────────────────────────────────

QVector<SignalChannel> SignalHandler::channels() const
{
  const QMutexLocker lock(&mutex_);
  return channels_;
}
