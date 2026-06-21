#pragma once

// OscilloscopeWidget — custom QPainter widget for multi-channel waveform display.
// Features grid with divisions, anti-aliased waveforms, time/voltage cursors,
// trigger level indicator, and channel color coding.

#include <QWidget>
#include <QVector>
#include <QColor>
#include <QString>

class OscilloscopeWidget : public QWidget {
  Q_OBJECT
public:
  explicit OscilloscopeWidget(QWidget *parent = nullptr);

  struct ChannelData {
    QString name;
    QColor color;
    QVector<double> samples;
  };

  void setChannelData(const QVector<ChannelData> &channels);
  void clearChannels();

  void setTimeDivisions(int divisions);
  void setVoltageDivisions(int divisions);

  void setTriggerLevel(double level);
  void setTriggerVisible(bool visible);

  void setCursorEnabled(bool enabled);
  void setCursorX(double xFrac);
  void setCursorY(double yFrac);

  static constexpr int kColorCount = 8;
  static const QColor kColors[kColorCount];

protected:
  void paintEvent(QPaintEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;

private:
  void drawGrid(QPainter &p, const QRect &area);
  void drawTrigger(QPainter &p, const QRect &area);
  void drawWaveforms(QPainter &p, const QRect &area);
  void drawCursors(QPainter &p, const QRect &area);
  void drawMeasurements(QPainter &p, const QRect &area);

  QVector<ChannelData> channels_;
  int hDivisions_ = 10;
  int vDivisions_ = 8;
  double triggerLevel_ = 0.0;
  bool triggerVisible_ = true;
  bool cursorEnabled_ = false;
  double cursorX_ = 0.5;
  double cursorY_ = 0.5;

  static constexpr int kMarginLeft   = 60;
  static constexpr int kMarginRight  = 20;
  static constexpr int kMarginTop    = 16;
  static constexpr int kMarginBottom = 32;
};
