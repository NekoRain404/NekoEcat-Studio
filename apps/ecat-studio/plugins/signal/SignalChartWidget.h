#pragma once

// SignalChartWidget — QPainter-based multi-channel scrolling line chart.
// Renders time-series data with auto-range Y-axis, grid lines, axis labels,
// and per-channel colour coding.  No OpenGL dependency.

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>

struct SignalChannelInfo;

class SignalChartWidget : public QWidget {
  Q_OBJECT
public:
  explicit SignalChartWidget(QWidget *parent = nullptr);

  // Set the number of visible points (the scrolling window).
  void setVisiblePoints(int points);
  int visiblePoints() const;

  // Replace all channel data references.  The widget stores its own copy.
  struct ChannelData {
    QString name;
    QColor color;
    QVector<double> values;
  };

  void setChannelData(const QVector<ChannelData> &channels);
  void clearChannels();

  // 10 distinct, accessible colours cycled per channel.
  static constexpr int kColorCount = 10;
  static const QColor kColors[kColorCount];

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  void drawGrid(QPainter &p, const QRect &chartRect);
  void drawAxes(QPainter &p, const QRect &chartRect);
  void drawChannels(QPainter &p, const QRect &chartRect);

  QVector<ChannelData> channels_;
  int visiblePoints_ = 500;

  // Layout constants.
  static constexpr int kMarginLeft   = 60;
  static constexpr int kMarginRight  = 20;
  static constexpr int kMarginTop    = 16;
  static constexpr int kMarginBottom = 32;
};
