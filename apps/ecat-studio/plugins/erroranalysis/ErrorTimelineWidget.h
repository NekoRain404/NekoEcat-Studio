#pragma once

// ErrorTimelineWidget — custom QPainter widget that renders a visual error
// timeline with severity-coded markers, zoom/pan, and event markers.

#include <QDateTime>
#include <QVector>
#include <QWidget>

struct TimelineEvent {
    QDateTime timestamp;
    QString severity;
    QString message;
    int slavePosition = -1;
};

class ErrorTimelineWidget : public QWidget {
    Q_OBJECT
public:
    explicit ErrorTimelineWidget(QWidget* parent = nullptr);

    void setEvents(const QVector<TimelineEvent>& events);
    void clearEvents();
    int eventCount() const;

    void zoomIn();
    void zoomOut();
    void resetZoom();

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void drawTimeline(QPainter& painter, const QRect& rect);
    void drawEvents(QPainter& painter, const QRect& rect);
    void drawLegend(QPainter& painter, const QRect& rect);

    QVector<TimelineEvent> events_;
    double zoomFactor_ = 1.0;
    int panOffset_ = 0;
    int dragStartX_ = 0;
};
