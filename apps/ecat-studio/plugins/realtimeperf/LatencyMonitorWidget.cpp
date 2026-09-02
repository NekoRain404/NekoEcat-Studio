// LatencyMonitorWidget — scrolling line chart of EtherCAT cycle latency.
//
// Follows the same QPainter pattern as DriftMonitorWidget: dark background,
// grid lines, threshold indicators, and a color-coded polyline.

#include "LatencyMonitorWidget.h"
#include "services/RealtimePerformanceService.h"

#include <QPainter>
#include <QPen>
#include <QtMath>

LatencyMonitorWidget::LatencyMonitorWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(300, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void LatencyMonitorWidget::addSample(double latencyUs) {
    samples_.append(latencyUs);
    while (samples_.size() > maxSamples_)
        samples_.removeFirst();
    update();
}

void LatencyMonitorWidget::setThreshold(double us) {
    thresholdUs_ = us;
    update();
}

void LatencyMonitorWidget::setHistorySize(int count) {
    maxSamples_ = count;
    while (samples_.size() > maxSamples_)
        samples_.removeFirst();
}

void LatencyMonitorWidget::clear() {
    samples_.clear();
    update();
}

void LatencyMonitorWidget::updateMetrics(const LatencyMetrics& m) {
    minUs_ = m.minUs;
    maxUs_ = m.maxUs;
    avgUs_ = m.avgUs;
    stddevUs_ = m.stddevUs;
    if (!samples_.isEmpty()) {
        addSample(m.avgUs);
    } else {
        addSample(m.avgUs);
    }
}

QColor LatencyMonitorWidget::colorForLatency(double latencyUs) const {
    if (latencyUs > thresholdUs_)
        return QColor("#ef4444");
    if (latencyUs > thresholdUs_ * 0.7)
        return QColor("#f59e0b");
    return QColor("#22c55e");
}

void LatencyMonitorWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor("#1e1e2e"));

    const int margin = 60;
    const int bottomMargin = 40;
    QRect area(margin, 10, width() - margin - 10, height() - bottomMargin - 10);

    if (!area.isValid())
        return;

    drawGrid(p, area);
    drawThresholdLines(p, area);
    drawLatencyCurve(p, area);
    drawStats(p, area);
}

void LatencyMonitorWidget::drawBackground(QPainter& p, const QRect& area) {
    p.fillRect(area, QColor("#252540"));
}

void LatencyMonitorWidget::drawGrid(QPainter& p, const QRect& area) {
    p.setPen(QPen(QColor("#3e3e5e"), 1));
    const int hLines = 6;
    for (int i = 0; i <= hLines; ++i) {
        int y = area.top() + (area.height() * i) / hLines;
        p.drawLine(area.left(), y, area.right(), y);
    }
    const int vLines = 8;
    for (int i = 0; i <= vLines; ++i) {
        int x = area.left() + (area.width() * i) / vLines;
        p.drawLine(x, area.top(), x, area.bottom());
    }

    p.setPen(QColor("#8888aa"));
    QFont f = p.font();
    f.setPointSize(8);
    p.setFont(f);

    double maxVal = thresholdUs_ * 1.5;
    if (!samples_.isEmpty()) {
        for (double s : samples_) {
            if (s > maxVal)
                maxVal = s * 1.2;
        }
    }

    for (int i = 0; i <= hLines; ++i) {
        int y = area.top() + (area.height() * i) / hLines;
        double val = maxVal - (maxVal * i / hLines);
        p.drawText(2, y + 4, QString::number(val, 'f', 0) + " us");
    }
}

void LatencyMonitorWidget::drawThresholdLines(QPainter& p, const QRect& area) {
    double maxVal = thresholdUs_ * 1.5;
    if (!samples_.isEmpty()) {
        for (double s : samples_) {
            if (s > maxVal)
                maxVal = s * 1.2;
        }
    }

    auto yPos = [&](double val) -> int {
        double normalized = (maxVal - val) / maxVal;
        return area.top() + static_cast<int>(normalized * area.height());
    };

    p.setPen(QPen(QColor("#ef444480"), 1, Qt::DashLine));
    int yThresh = yPos(thresholdUs_);
    if (yThresh >= area.top() && yThresh <= area.bottom()) {
        p.drawLine(area.left(), yThresh, area.right(), yThresh);
    }

    p.setPen(QPen(QColor("#f59e0b60"), 1, Qt::DotLine));
    int yWarn = yPos(thresholdUs_ * 0.7);
    if (yWarn >= area.top() && yWarn <= area.bottom()) {
        p.drawLine(area.left(), yWarn, area.right(), yWarn);
    }
}

void LatencyMonitorWidget::drawLatencyCurve(QPainter& p, const QRect& area) {
    if (samples_.size() < 2)
        return;

    double maxVal = thresholdUs_ * 1.5;
    for (double s : samples_) {
        if (s > maxVal)
            maxVal = s * 1.2;
    }

    auto yPos = [&](double val) -> int {
        double normalized = (maxVal - val) / maxVal;
        return area.top() + static_cast<int>(normalized * area.height());
    };

    QPolygonF curve;
    for (int i = 0; i < samples_.size(); ++i) {
        double x = area.left() + (static_cast<double>(i) / (samples_.size() - 1)) * area.width();
        double y = yPos(samples_[i]);
        curve << QPointF(
            x, qBound(static_cast<qreal>(area.top()), static_cast<qreal>(y), static_cast<qreal>(area.bottom())));
    }

    p.setPen(QPen(QColor("#60a5fa"), 2));
    p.drawPolyline(curve);

    if (!samples_.isEmpty()) {
        double last = samples_.last();
        p.setPen(Qt::NoPen);
        p.setBrush(colorForLatency(last));
        p.drawEllipse(curve.last(), 4, 4);
    }
}

void LatencyMonitorWidget::drawStats(QPainter& p, const QRect& area) {
    int x = area.left() + 8;
    int y = area.top() + 14;

    p.setPen(QColor("#cccccc"));
    QFont f = p.font();
    f.setPointSize(8);
    p.setFont(f);

    auto drawLine = [&](const QString& label, const QString& value, int& yy) {
        p.setPen(QColor("#8888aa"));
        p.drawText(x, yy, label);
        p.setPen(QColor("#cccccc"));
        p.drawText(x + 70, yy, value);
        yy += 16;
    };

    drawLine(tr("Min:"), QString::number(minUs_, 'f', 1) + " us", y);
    drawLine(tr("Max:"), QString::number(maxUs_, 'f', 1) + " us", y);
    drawLine(tr("Avg:"), QString::number(avgUs_, 'f', 1) + " us", y);
    drawLine(tr("Stddev:"), QString::number(stddevUs_, 'f', 1) + " us", y);
    drawLine(tr("Samples:"), QString::number(samples_.size()), y);
}
