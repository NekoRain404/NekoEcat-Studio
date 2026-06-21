#pragma once

// ProtocolAnalyzerService — capture backend for the Protocol Analyzer plugin.
// Simulates EtherCAT frame capture, filtering, and statistics.
//
// DEMO STUB — This service generates synthetic data for UI demonstration.
// Replace with real hardware integration for production use.

#include <QObject>
#include <QVector>
#include <QString>
#include <QJsonObject>
#include <QTimer>

enum class ProtocolFrameType { EtherCAT, CoE, EoE, FoE, SoE, Unknown };
enum class ProtocolDirection { TX, RX };

struct ProtocolFrame {
  qint64 timestamp = 0;
  ProtocolDirection direction = ProtocolDirection::TX;
  ProtocolFrameType frameType = ProtocolFrameType::EtherCAT;
  quint16 wkc = 0;
  QByteArray data;
  QString decodedSummary;
};

struct ProtocolFilter {
  bool enabled = false;
  ProtocolFrameType frameType = ProtocolFrameType::EtherCAT;
  int slave = -1;
  ProtocolDirection direction = ProtocolDirection::TX;
};

struct ProtocolStatistics {
  int totalFrames = 0;
  int errorFrames = 0;
  int txFrames = 0;
  int rxFrames = 0;
  int coeFrames = 0;
  int eoeFrames = 0;
  int foeFrames = 0;
  int soeFrames = 0;
  double bandwidthBps = 0.0;
};

class ProtocolAnalyzerService : public QObject {
  Q_OBJECT
public:
  explicit ProtocolAnalyzerService(QObject *parent = nullptr);

  void startCapture();
  void stopCapture();
  bool isCapturing() const;

  void setFilter(const ProtocolFilter &filter);
  ProtocolFilter filter() const;

  QVector<ProtocolFrame> getFrames(int count) const;
  int frameCount() const;

  ProtocolStatistics statistics() const;
  QJsonObject statisticsJson() const;

  void clearFrames();

  static constexpr int kMaxFrames = 10000;

signals:
  void frameCaptured(const ProtocolFrame &frame);
  void statisticsUpdated(const ProtocolStatistics &stats);

private:
  void generateFrame();

  QVector<ProtocolFrame> frames_;
  QTimer *timer_ = nullptr;
  ProtocolFilter filter_;
  ProtocolStatistics stats_;
  bool capturing_ = false;
  int tickCount_ = 0;
};
