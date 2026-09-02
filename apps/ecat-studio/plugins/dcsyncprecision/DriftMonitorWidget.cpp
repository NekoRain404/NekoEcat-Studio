#include "DriftMonitorWidget.h"

#include <QPainter>
#include <QPen>
#include <QtMath>

DriftMonitorWidget::DriftMonitorWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(300, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void DriftMonitorWidget::addSample(double drift) {
    samples_.append(drift);
    while (samples_.size() > maxSamples_)
        samples_.removeFirst();
    update();
}

void DriftMonitorWidget::setThreshold(double ns) {
    threshold_ = ns;
    update();
}

void DriftMonitorWidget::setHistorySize(int count) {
    maxSamples_ = count;
    while (samples_.size() > maxSamples_)
        samples_.removeFirst();
}

void DriftMonitorWidget::clear() {
    samples_.clear();
    update();
}

QColor DriftMonitorWidget::colorForDrift(double drift) const {
    double absD = qAbs(drift);
    if (absD > threshold_)
        return QColor("#ef4444");
    if (absD > threshold_ * 0.7)
        return QColor("#f59e0b");
    return QColor("#22c55e");
}

void DriftMonitorWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor("#1e1e2e"));

    const int margin = 50;
    const int bottomMargin = 40;
    QRect area(margin, 10, width() - margin - 10, height() - bottomMargin - 10);

    if (!area.isValid())
        return;

    drawGrid(p, area);
    drawThresholdLines(p, area);
    drawDriftCurve(p, area);
    drawLegend(p, area);
}

void DriftMonitorWidget::drawGrid(QPainter& p, const QRect& area) {
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

    double maxVal = threshold_ * 1.5;
    if (!samples_.isEmpty()) {
        for (double s : samples_) {
            if (qAbs(s) > maxVal)
                maxVal = qAbs(s) * 1.2;
        }
    }

    for (int i = 0; i <= hLines; ++i) {
        int y = area.top() + (area.height() * i) / hLines;
        double val = maxVal - (2.0 * maxVal * i / hLines);
        p.drawText(2, y + 4, QString::number(val, 'f', 0) + " ns");
    }
}

void DriftMonitorWidget::drawThresholdLines(QPainter& p, const QRect& area) {
    double maxVal = threshold_ * 1.5;
    if (!samples_.isEmpty()) {
        for (double s : samples_) {
            if (qAbs(s) > maxVal)
                maxVal = qAbs(s) * 1.2;
        }
    }

    auto yPos = [&](double val) -> int {
        double normalized = (maxVal - val) / (2.0 * maxVal);
        return area.top() + static_cast<int>(normalized * area.height());
    };

    p.setPen(QPen(QColor("#ef444480"), 1, Qt::DashLine));
    int yThresh = yPos(threshold_);
    if (yThresh >= area.top() && yThresh <= area.bottom()) {
        p.drawLine(area.left(), yThresh, area.right(), yThresh);
    }
    int yNegThresh = yPos(-threshold_);
    if (yNegThresh >= area.top() && yNegThresh <= area.bottom()) {
        p.drawLine(area.left(), yNegThresh, area.right(), yNegThresh);
    }

    p.setPen(QPen(QColor("#f59e0b60"), 1, Qt::DotLine));
    int yWarn = yPos(threshold_ * 0.7);
    if (yWarn >= area.top() && yWarn <= area.bottom()) {
        p.drawLine(area.left(), yWarn, area.right(), yWarn);
    }
    int yNegWarn = yPos(-threshold_ * 0.7);
    if (yNegWarn >= area.top() && yNegWarn <= area.bottom()) {
        p.drawLine(area.left(), yNegWarn, area.right(), yNegWarn);
    }
}

void DriftMonitorWidget::drawDriftCurve(QPainter& p, const QRect& area) {
    if (samples_.size() < 2)
        return;

    double maxVal = threshold_ * 1.5;
    for (double s : samples_) {
        if (qAbs(s) > maxVal)
            maxVal = qAbs(s) * 1.2;
    }

    auto yPos = [&](double val) -> int {
        double normalized = (maxVal - val) / (2.0 * maxVal);
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
        p.setBrush(colorForDrift(last));
        p.drawEllipse(curve.last(), 4, 4);
    }
}

void DriftMonitorWidget::drawLegend(QPainter& p, const QRect& area) {
    int x = area.right() - 180;
    int y = area.top() + 10;

    p.setPen(QColor("#cccccc"));
    QFont f = p.font();
    f.setPointSize(8);
    p.setFont(f);

    auto drawItem = [&](const QColor& color, const QString& text, int& yy) {
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawRect(x, yy, 10, 10);
        p.setPen(QColor("#cccccc"));
        p.drawText(x + 16, yy + 10, text);
        yy += 18;
    };

    drawItem(QColor("#22c55e"), tr("OK (< 70%% threshold)"), y);
    drawItem(QColor("#f59e0b"), tr("Warning (70-100%% threshold)"), y);
    drawItem(QColor("#ef4444"), tr("Error (> threshold)"), y);
}
