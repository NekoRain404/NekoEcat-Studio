#pragma once

/// @brief Custom QPainter widget for real-time latency visualization.
///
/// @details Draws a scrolling line chart of latency samples with color-coded
/// severity regions and threshold indicators.

#include <QWidget>
#include <QVector>

struct LatencyMetrics;

class LatencyMonitorWidget : public QWidget {
  Q_OBJECT
public:
  explicit LatencyMonitorWidget(QWidget *parent = nullptr);

  void addSample(double latencyUs);
  void setThreshold(double us);
  void setHistorySize(int count);
  void clear();

  void updateMetrics(const LatencyMetrics &m);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  void drawBackground(QPainter &p, const QRect &area);
  void drawGrid(QPainter &p, const QRect &area);
  void drawThresholdLines(QPainter &p, const QRect &area);
  void drawLatencyCurve(QPainter &p, const QRect &area);
  void drawStats(QPainter &p, const QRect &area);

  QColor colorForLatency(double latencyUs) const;

  QVector<double> samples_;
  double thresholdUs_ = 1000.0;
  int maxSamples_ = 200;
  double minUs_ = 0.0;
  double maxUs_ = 0.0;
  double avgUs_ = 0.0;
  double stddevUs_ = 0.0;
};
