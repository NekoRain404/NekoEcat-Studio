// OscilloscopeWidget — implementation.  See header for interface documentation.
#include "OscilloscopeWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

const QColor OscilloscopeWidget::kColors[kColorCount] = {
    QColor(0, 255, 0),     // green — classic scope CH1
    QColor(255, 255, 0),   // yellow — CH2
    QColor(0, 200, 255),   // cyan — CH3
    QColor(255, 0, 255),   // magenta — CH4
    QColor(255, 128, 0),   // orange — CH5
    QColor(128, 255, 128), // light green — CH6
    QColor(128, 128, 255), // light blue — CH7
    QColor(255, 128, 128), // light red — CH8
};

OscilloscopeWidget::OscilloscopeWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(300, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void OscilloscopeWidget::setChannelData(const QVector<ChannelData>& channels) {
    channels_ = channels;
    update();
}

void OscilloscopeWidget::clearChannels() {
    channels_.clear();
    update();
}

void OscilloscopeWidget::setTimeDivisions(int divisions) {
    hDivisions_ = qMax(2, divisions);
    update();
}

void OscilloscopeWidget::setVoltageDivisions(int divisions) {
    vDivisions_ = qMax(2, divisions);
    update();
}

void OscilloscopeWidget::setTriggerLevel(double level) {
    triggerLevel_ = level;
    update();
}

void OscilloscopeWidget::setTriggerVisible(bool visible) {
    triggerVisible_ = visible;
    update();
}

void OscilloscopeWidget::setCursorEnabled(bool enabled) {
    cursorEnabled_ = enabled;
    update();
}

void OscilloscopeWidget::setCursorX(double xFrac) {
    cursorX_ = xFrac;
    update();
}

void OscilloscopeWidget::setCursorY(double yFrac) {
    cursorY_ = yFrac;
    update();
}

void OscilloscopeWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!cursorEnabled_)
        return;
    const QRect area(kMarginLeft, kMarginTop, width() - kMarginLeft - kMarginRight,
                     height() - kMarginTop - kMarginBottom);
    if (!area.contains(event->pos()))
        return;
    cursorX_ = static_cast<double>(event->pos().x() - area.left()) / area.width();
    cursorY_ = static_cast<double>(event->pos().y() - area.top()) / area.height();
    update();
}

void OscilloscopeWidget::mousePressEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    if (event->button() == Qt::RightButton) {
        cursorEnabled_ = !cursorEnabled_;
        update();
    }
}

// ── Paint ─────────────────────────────────────────────────────────────

void OscilloscopeWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    p.fillRect(rect(), QColor(0, 0, 0));

    const QRect area(kMarginLeft, kMarginTop, width() - kMarginLeft - kMarginRight,
                     height() - kMarginTop - kMarginBottom);
    if (area.width() < 10 || area.height() < 10)
        return;

    drawGrid(p, area);
    drawTrigger(p, area);
    drawWaveforms(p, area);
    if (cursorEnabled_)
        drawCursors(p, area);
    drawMeasurements(p, area);
}

void OscilloscopeWidget::drawGrid(QPainter& p, const QRect& area) {
    // Major grid.
    p.setPen(QPen(QColor(40, 40, 40), 1));
    for (int i = 0; i <= hDivisions_; ++i) {
        const int x = area.left() + i * area.width() / hDivisions_;
        p.drawLine(x, area.top(), x, area.bottom());
    }
    for (int i = 0; i <= vDivisions_; ++i) {
        const int y = area.top() + i * area.height() / vDivisions_;
        p.drawLine(area.left(), y, area.right(), y);
    }

    // Minor grid (sub-divisions of 5).
    p.setPen(QPen(QColor(25, 25, 25), 1));
    for (int i = 0; i < hDivisions_ * 5; ++i) {
        const int x = area.left() + i * area.width() / (hDivisions_ * 5);
        p.drawLine(x, area.top(), x, area.bottom());
    }
    for (int i = 0; i < vDivisions_ * 5; ++i) {
        const int y = area.top() + i * area.height() / (vDivisions_ * 5);
        p.drawLine(area.left(), y, area.right(), y);
    }

    // Center cross emphasis.
    p.setPen(QPen(QColor(60, 60, 60), 1));
    const int cx = area.left() + area.width() / 2;
    const int cy = area.top() + area.height() / 2;
    p.drawLine(cx, area.top(), cx, area.bottom());
    p.drawLine(area.left(), cy, area.right(), cy);
}

void OscilloscopeWidget::drawTrigger(QPainter& p, const QRect& area) {
    if (!triggerVisible_)
        return;
    // Map trigger level to Y. Assume ±100 range.
    const double frac = (triggerLevel_ + 100.0) / 200.0;
    const int y = area.bottom() - static_cast<int>(frac * area.height());
    if (y < area.top() || y > area.bottom())
        return;

    p.setPen(QPen(QColor(255, 80, 80), 1, Qt::DashDotLine));
    p.drawLine(area.left(), y, area.right(), y);

    // Arrow indicator on left edge.
    QPolygon arrow;
    arrow << QPoint(area.left(), y) << QPoint(area.left() - 8, y - 4) << QPoint(area.left() - 8, y + 4);
    p.setBrush(QColor(255, 80, 80));
    p.drawPolygon(arrow);
}

void OscilloscopeWidget::drawWaveforms(QPainter& p, const QRect& area) {
    if (channels_.isEmpty()) {
        p.setPen(QColor(80, 80, 80));
        p.drawText(area, Qt::AlignCenter, tr("No signal"));
        return;
    }

    // Find global Y range.
    double yMin = 0.0, yMax = 1.0;
    bool first = true;
    for (const auto& ch : channels_) {
        for (double v : ch.samples) {
            if (first || v < yMin)
                yMin = v;
            if (first || v > yMax)
                yMax = v;
            first = false;
        }
    }
    if (qFuzzyCompare(yMin + 1.0, yMax + 1.0)) {
        yMin -= 1.0;
        yMax += 1.0;
    }
    const double yRange = yMax - yMin;

    for (int ci = 0; ci < channels_.size(); ++ci) {
        const auto& ch = channels_[ci];
        if (ch.samples.size() < 2)
            continue;

        const QColor color = ch.color.isValid() ? ch.color : kColors[ci % kColorCount];
        p.setPen(QPen(color, 1.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        QPainterPath path;
        for (int i = 0; i < ch.samples.size(); ++i) {
            const double xFrac = static_cast<double>(i) / (ch.samples.size() - 1);
            const double yFrac = (ch.samples[i] - yMin) / yRange;
            const double px = area.left() + xFrac * area.width();
            const double py = area.bottom() - yFrac * area.height();
            if (i == 0)
                path.moveTo(px, py);
            else
                path.lineTo(px, py);
        }
        p.drawPath(path);

        // Channel label.
        const int labelY = area.top() + 4 + ci * 14;
        if (labelY + 10 < area.bottom()) {
            p.fillRect(area.right() - 110, labelY, 10, 10, color);
            p.setPen(QColor(200, 200, 200));
            QFont f = font();
            f.setPointSize(qMax(7, f.pointSize() - 2));
            p.setFont(f);
            p.drawText(area.right() - 96, labelY, 90, 12, Qt::AlignLeft | Qt::AlignVCenter, ch.name);
        }
    }
}

void OscilloscopeWidget::drawCursors(QPainter& p, const QRect& area) {
    // Vertical time cursor.
    const int cx = area.left() + static_cast<int>(cursorX_ * area.width());
    p.setPen(QPen(QColor(255, 255, 0, 150), 1, Qt::DashLine));
    p.drawLine(cx, area.top(), cx, area.bottom());

    // Horizontal voltage cursor.
    const int cy = area.top() + static_cast<int>(cursorY_ * area.height());
    p.drawLine(area.left(), cy, area.right(), cy);

    // Readout labels.
    p.setPen(QColor(255, 255, 0));
    QFont f = font();
    f.setPointSize(qMax(7, f.pointSize() - 2));
    p.setFont(f);
    p.drawText(cx + 4, area.top() + 2, 80, 14, Qt::AlignLeft, QStringLiteral("T:%1").arg(cursorX_, 0, 'f', 3));
    p.drawText(area.left() + 2, cy - 16, 80, 14, Qt::AlignLeft,
               QStringLiteral("V:%1").arg(cursorY_ * 200.0 - 100.0, 0, 'f', 2));
}

void OscilloscopeWidget::drawMeasurements(QPainter& p, const QRect& area) {
    // Bottom axis label.
    p.setPen(QColor(150, 150, 150));
    QFont f = font();
    f.setPointSize(qMax(7, f.pointSize() - 2));
    p.setFont(f);
    p.drawText(area.left(), area.bottom() + 4, area.width(), 20, Qt::AlignCenter, tr("Time (ms)"));

    // Y axis labels.
    for (int i = 0; i <= vDivisions_; ++i) {
        const int y = area.top() + i * area.height() / vDivisions_;
        const double val = 100.0 - i * 200.0 / vDivisions_;
        p.drawText(2, y - 7, kMarginLeft - 6, 14, Qt::AlignRight | Qt::AlignVCenter, QString::number(val, 'f', 1));
    }
}
