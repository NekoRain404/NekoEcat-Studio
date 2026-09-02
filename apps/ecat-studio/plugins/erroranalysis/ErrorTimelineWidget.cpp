#include "ErrorTimelineWidget.h"

#include <algorithm>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>

ErrorTimelineWidget::ErrorTimelineWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(200);
    setMouseTracking(true);
}

void ErrorTimelineWidget::setEvents(const QVector<TimelineEvent>& events) {
    events_ = events;
    std::sort(events_.begin(), events_.end(),
              [](const TimelineEvent& a, const TimelineEvent& b) { return a.timestamp < b.timestamp; });
    update();
}

void ErrorTimelineWidget::clearEvents() {
    events_.clear();
    update();
}

int ErrorTimelineWidget::eventCount() const {
    return events_.size();
}

void ErrorTimelineWidget::zoomIn() {
    zoomFactor_ = qMin(zoomFactor_ * 1.2, 10.0);
    update();
}

void ErrorTimelineWidget::zoomOut() {
    zoomFactor_ = qMax(zoomFactor_ / 1.2, 0.1);
    update();
}

void ErrorTimelineWidget::resetZoom() {
    zoomFactor_ = 1.0;
    panOffset_ = 0;
    update();
}

void ErrorTimelineWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(30, 30, 30));

    QRect timelineRect = rect().adjusted(60, 30, -20, -50);
    drawTimeline(painter, timelineRect);
    drawEvents(painter, timelineRect);
    drawLegend(painter, rect().adjusted(60, height() - 45, -20, -5));
}

void ErrorTimelineWidget::drawTimeline(QPainter& painter, const QRect& rect) {
    painter.setPen(QPen(QColor(100, 100, 100), 1));
    painter.drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());

    if (events_.isEmpty())
        return;

    qint64 totalSpan = events_.first().timestamp.msecsTo(events_.last().timestamp);
    if (totalSpan <= 0)
        totalSpan = 1;

    int markers = qMin(6, rect.width() / 100);
    for (int i = 0; i <= markers; ++i) {
        double frac = static_cast<double>(i) / markers;
        int x = rect.left() + static_cast<int>(frac * rect.width());
        painter.drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        QDateTime t = events_.first().timestamp.addMSecs(static_cast<qint64>(frac * totalSpan));
        painter.drawText(x - 30, rect.bottom() + 8, 60, 20, Qt::AlignHCenter, t.toString("hh:mm:ss"));
    }
}

void ErrorTimelineWidget::drawEvents(QPainter& painter, const QRect& rect) {
    if (events_.isEmpty())
        return;

    qint64 totalSpan = events_.first().timestamp.msecsTo(events_.last().timestamp);
    if (totalSpan <= 0)
        totalSpan = 1;

    for (const auto& ev : events_) {
        double frac = static_cast<double>(events_.first().timestamp.msecsTo(ev.timestamp)) / totalSpan;
        int x = rect.left() + static_cast<int>(frac * rect.width()) + panOffset_;
        if (x < rect.left() || x > rect.right())
            continue;

        QColor color;
        if (ev.severity == "Critical" || ev.severity == "Error")
            color = QColor(220, 50, 50);
        else if (ev.severity == "Warning")
            color = QColor(230, 180, 40);
        else
            color = QColor(80, 160, 220);

        int yBase = rect.bottom() - 10;
        painter.setPen(QPen(color, 2));
        painter.drawLine(x, yBase, x, yBase - 40);

        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPoint(x, yBase - 40), 5, 5);
    }
}

void ErrorTimelineWidget::drawLegend(QPainter& painter, const QRect& rect) {
    painter.setPen(QColor(200, 200, 200));
    int x = rect.left();

    auto drawLegendItem = [&](const QColor& c, const QString& label) {
        painter.setBrush(c);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(x, rect.top() + 5, 10, 10);
        painter.setPen(QColor(200, 200, 200));
        painter.drawText(x + 14, rect.top(), 80, 20, Qt::AlignLeft, label);
        x += 100;
    };

    drawLegendItem(QColor(220, 50, 50), tr("Error"));
    drawLegendItem(QColor(230, 180, 40), tr("Warning"));
    drawLegendItem(QColor(80, 160, 220), tr("Info"));
}

void ErrorTimelineWidget::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0)
        zoomIn();
    else
        zoomOut();
}

void ErrorTimelineWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
        dragStartX_ = event->pos().x();
}

void ErrorTimelineWidget::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        panOffset_ += event->pos().x() - dragStartX_;
        dragStartX_ = event->pos().x();
        update();
    }
}
