// SignalChartWidget — implementation.  See header for interface documentation.
#include "SignalChartWidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QtMath>

const QColor SignalChartWidget::kColors[kColorCount] = {
    QColor(31, 119, 180),  // blue
    QColor(255, 127, 14),  // orange
    QColor(44, 160, 44),   // green
    QColor(214, 39, 40),   // red
    QColor(148, 103, 189), // purple
    QColor(140, 86, 75),   // brown
    QColor(227, 119, 194), // pink
    QColor(127, 127, 127), // grey
    QColor(188, 189, 34),  // olive
    QColor(23, 190, 207),  // cyan
};

SignalChartWidget::SignalChartWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(200, 120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void SignalChartWidget::setVisiblePoints(int points) {
    visiblePoints_ = qMax(10, points);
    update();
}

int SignalChartWidget::visiblePoints() const {
    return visiblePoints_;
}

void SignalChartWidget::setChannelData(const QVector<ChannelData>& channels) {
    channels_ = channels;
    update();
}

void SignalChartWidget::clearChannels() {
    channels_.clear();
    update();
}

// ── Paint ─────────────────────────────────────────────────────────────

void SignalChartWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Background.
    p.fillRect(rect(), QColor(30, 30, 30));

    const QRect chartRect(kMarginLeft, kMarginTop, width() - kMarginLeft - kMarginRight,
                          height() - kMarginTop - kMarginBottom);

    if (chartRect.width() < 10 || chartRect.height() < 10)
        return;

    drawGrid(p, chartRect);
    drawChannels(p, chartRect);
    drawAxes(p, chartRect);
}

void SignalChartWidget::drawGrid(QPainter& p, const QRect& chartRect) {
    QPen gridPen(QColor(60, 60, 60), 1, Qt::DotLine);
    p.setPen(gridPen);

    // Horizontal grid lines (5 divisions).
    for (int i = 0; i <= 5; ++i) {
        const int y = chartRect.top() + i * chartRect.height() / 5;
        p.drawLine(chartRect.left(), y, chartRect.right(), y);
    }

    // Vertical grid lines (10 divisions).
    for (int i = 0; i <= 10; ++i) {
        const int x = chartRect.left() + i * chartRect.width() / 10;
        p.drawLine(x, chartRect.top(), x, chartRect.bottom());
    }
}

void SignalChartWidget::drawAxes(QPainter& p, const QRect& chartRect) {
    p.setPen(QColor(180, 180, 180));

    // Compute global Y range across all channels' visible data.
    double yMin = 0.0, yMax = 1.0;
    bool first = true;
    for (const auto& ch : channels_) {
        const int start = qMax(0, ch.values.size() - visiblePoints_);
        for (int i = start; i < ch.values.size(); ++i) {
            const double v = ch.values[i];
            if (first || v < yMin)
                yMin = v;
            if (first || v > yMax)
                yMax = v;
            first = false;
        }
    }
    if (qFuzzyCompare(yMin, yMax)) {
        yMin -= 1.0;
        yMax += 1.0;
    }
    const double yRange = yMax - yMin;

    // Y-axis labels (5 divisions).
    QFont labelFont = font();
    labelFont.setPointSize(qMax(8, labelFont.pointSize() - 1));
    p.setFont(labelFont);
    for (int i = 0; i <= 5; ++i) {
        const int y = chartRect.top() + i * chartRect.height() / 5;
        const double val = yMax - i * yRange / 5.0;
        p.drawText(2, y - 8, chartRect.left() - 8, 16, Qt::AlignRight | Qt::AlignVCenter, QString::number(val, 'g', 4));
    }

    // X-axis label.
    p.drawText(chartRect.left(), chartRect.bottom() + 4, chartRect.width(), 20, Qt::AlignCenter, tr("Time (samples)"));
}

void SignalChartWidget::drawChannels(QPainter& p, const QRect& chartRect) {
    if (channels_.isEmpty()) {
        p.setPen(QColor(120, 120, 120));
        p.drawText(chartRect, Qt::AlignCenter, tr("No channels"));
        return;
    }

    // Compute global Y range (same logic as drawAxes, kept in sync).
    double yMin = 0.0, yMax = 1.0;
    bool first = true;
    for (const auto& ch : channels_) {
        const int start = qMax(0, ch.values.size() - visiblePoints_);
        for (int i = start; i < ch.values.size(); ++i) {
            const double v = ch.values[i];
            if (first || v < yMin)
                yMin = v;
            if (first || v > yMax)
                yMax = v;
            first = false;
        }
    }
    if (qFuzzyCompare(yMin, yMax)) {
        yMin -= 1.0;
        yMax += 1.0;
    }
    const double yRange = yMax - yMin;

    // Draw each channel.
    for (int ci = 0; ci < channels_.size(); ++ci) {
        const auto& ch = channels_[ci];
        const int start = qMax(0, ch.values.size() - visiblePoints_);
        const int count = ch.values.size() - start;
        if (count < 2)
            continue;

        QPen linePen(ch.color.isValid() ? ch.color : kColors[ci % kColorCount], 1.5, Qt::SolidLine, Qt::RoundCap,
                     Qt::RoundJoin);
        p.setPen(linePen);

        QPainterPath path;
        for (int i = 0; i < count; ++i) {
            const double xFrac = static_cast<double>(i) / (count - 1);
            const double yFrac = (ch.values[start + i] - yMin) / yRange;
            const double px = chartRect.left() + xFrac * chartRect.width();
            const double py = chartRect.bottom() - yFrac * chartRect.height();
            if (i == 0)
                path.moveTo(px, py);
            else
                path.lineTo(px, py);
        }
        p.drawPath(path);

        // Legend entry (top-right area).
        const int legendY = chartRect.top() + 4 + ci * 16;
        if (legendY + 12 < chartRect.bottom()) {
            p.fillRect(chartRect.right() - 100, legendY, 10, 10, linePen.brush().color());
            p.setPen(QColor(200, 200, 200));
            p.drawText(chartRect.right() - 86, legendY, 80, 12, Qt::AlignLeft | Qt::AlignVCenter, ch.name);
            p.setPen(linePen); // restore for next channel
        }
    }
}
