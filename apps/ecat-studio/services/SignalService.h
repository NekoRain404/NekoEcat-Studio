#pragma once

// SignalService — manages signal channels for the Signal Analyzer plugin.
// Tracks per-channel time-series data, computes running statistics, and
// accumulates samples pushed through EventBus::signalData.

#include <QObject>
#include <QVector>
#include <QString>
#include <cstdint>

class QTimer;
class EventBus;

// Describes a single subscribed signal channel.
struct SignalChannelInfo {
  int id = -1;
  QString name;
  int slave = -1;
  QString index;
  QString subIndex;
  QVector<double> values;
  QVector<qint64> timestamps;
  static constexpr int kMaxPoints = 10000;
};

// Running statistics for a channel's visible data window.
struct ChannelStats {
  double min = 0.0;
  double max = 0.0;
  double avg = 0.0;
  double stddev = 0.0;
};

class SignalService : public QObject {
  Q_OBJECT
public:
  explicit SignalService(EventBus *bus, QObject *parent = nullptr);

  // Channel management.
  int addChannel(const QString &name, int slave,
                 const QString &idx, const QString &sub);
  void removeChannel(int channelId);
  QVector<SignalChannelInfo> channels() const;

  // Statistics for a specific channel.
  ChannelStats stats(int channelId) const;

  // Polling control (placeholder — actual daemon integration pending).
  void startPolling(int intervalMs = 100);
  void stopPolling();

  // Push data directly into a channel (for testing or external injection).
  void pushData(int channelId, const QVector<double> &values,
                const QVector<qint64> &timestamps);

signals:
  void channelDataUpdated(int channelId);
  void channelAdded(int channelId);
  void channelRemoved(int channelId);
  void error(const QString &msg);

private slots:
  void handleSignalData(int channel, const QVector<double> &values,
                        const QVector<qint64> &timestamps);

private:
  EventBus *bus_;
  QTimer *pollTimer_ = nullptr;
  QVector<SignalChannelInfo> channels_;
  int nextId_ = 1;
};
