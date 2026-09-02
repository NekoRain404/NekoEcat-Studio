#include "JitterAnalysisWidget.h"

#include <QPainter>
#include <QPen>
#include <QtMath>

JitterAnalysisWidget::JitterAnalysisWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(300, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void JitterAnalysisWidget::setJitterData(double min, double max, double avg, double stddev, int sampleCount) {
    min_ = min;
    max_ = max;
    avg_ = avg;
    stddev_ = stddev;
    sampleCount_ = sampleCount;
    trendHistory_.append(avg);
    while (trendHistory_.size() > maxTrendSamples_)
        trendHistory_.removeFirst();
    update();
}

void JitterAnalysisWidget::setHistogram(const QVector<int>& bins, double binWidth, double minVal) {
    histogram_ = bins;
    histBinWidth_ = binWidth;
    histMin_ = minVal;
    update();
}

void JitterAnalysisWidget::clear() {
    min_ = max_ = avg_ = stddev_ = 0.0;
    sampleCount_ = 0;
    histogram_.clear();
    trendHistory_.clear();
    update();
}

void JitterAnalysisWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#1e1e2e"));

    const int margin = 10;
    int halfH = (height() - margin * 3) / 2;

    QRect statsRect(margin, margin, width() - 2 * margin, 60);
    QRect histRect(margin, 80, (width() - 3 * margin) / 2, halfH - 80);
    QRect trendRect(histRect.right() + margin, 80, (width() - 3 * margin) / 2, halfH - 80);

    drawStatsPanel(p, statsRect);
    drawHistogram(p, histRect);
    drawTrendLine(p, trendRect);
}

void JitterAnalysisWidget::drawStatsPanel(QPainter& p, const QRect& area) {
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#2a2a3e"));
    p.drawRoundedRect(area, 4, 4);

    QFont f = p.font();
    f.setPointSize(9);
    f.setBold(true);
    p.setFont(f);
    p.setPen(QColor("#cccccc"));

    int colW = area.width() / 5;
    int y = area.top() + 20;

    auto drawStat = [&](int col, const QString& label, const QString& value) {
        int x = area.left() + col * colW + 8;
        p.setPen(QColor("#8888aa"));
        f.setBold(false);
        f.setPointSize(8);
        p.setFont(f);
        p.drawText(x, y, label);
        p.setPen(QColor("#ffffff"));
        f.setPointSize(10);
        f.setBold(true);
        p.setFont(f);
        p.drawText(x, y + 18, value);
    };

    drawStat(0, tr("Min"), QString::number(min_, 'f', 1) + " ns");
    drawStat(1, tr("Max"), QString::number(max_, 'f', 1) + " ns");
    drawStat(2, tr("Avg"), QString::number(avg_, 'f', 1) + " ns");
    drawStat(3, tr("Stddev"), QString::number(stddev_, 'f', 1) + " ns");
    drawStat(4, tr("Samples"), QString::number(sampleCount_));
}

void JitterAnalysisWidget::drawHistogram(QPainter& p, const QRect& area) {
    p.setPen(QColor("#8888aa"));
    QFont f = p.font();
    f.setPointSize(8);
    p.setFont(f);
    p.drawText(area.topLeft() + QPoint(0, -4), tr("Jitter Histogram"));

    p.setPen(QPen(QColor("#3e3e5e"), 1));
    p.setBrush(QColor("#2a2a3e"));
    p.drawRect(area);

    if (histogram_.isEmpty()) {
        p.setPen(QColor("#666688"));
        p.drawText(area, Qt::AlignCenter, tr("No data"));
        return;
    }

    int maxCount = 1;
    for (int v : histogram_) {
        if (v > maxCount)
            maxCount = v;
    }

    int barCount = histogram_.size();
    double barW = static_cast<double>(area.width()) / barCount;

    for (int i = 0; i < barCount; ++i) {
        if (histogram_[i] == 0)
            continue;
        double barH = (static_cast<double>(histogram_[i]) / maxCount) * area.height() * 0.9;
        int x = area.left() + static_cast<int>(i * barW);
        int w = static_cast<int>(barW) - 1;
        if (w < 1)
            w = 1;
        QRect barRect(x, area.bottom() - static_cast<int>(barH), w, static_cast<int>(barH));

        double ratio = static_cast<double>(i) / barCount;
        QColor color;
        if (ratio < 0.33)
            color = QColor("#22c55e");
        else if (ratio < 0.66)
            color = QColor("#f59e0b");
        else
            color = QColor("#ef4444");

        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawRect(barRect);
    }

    p.setPen(QColor("#666688"));
    p.drawText(area.bottomLeft() + QPoint(4, 12), QString::number(histMin_, 'f', 0) + " ns");
    p.drawText(area.bottomRight() + QPoint(-40, 12),
               QString::number(histMin_ + histBinWidth_ * barCount, 'f', 0) + " ns");
}

void JitterAnalysisWidget::drawTrendLine(QPainter& p, const QRect& area) {
    p.setPen(QColor("#8888aa"));
    QFont f = p.font();
    f.setPointSize(8);
    p.setFont(f);
    p.drawText(area.topLeft() + QPoint(0, -4), tr("Jitter Trend"));

    p.setPen(QPen(QColor("#3e3e5e"), 1));
    p.setBrush(QColor("#2a2a3e"));
    p.drawRect(area);

    if (trendHistory_.size() < 2) {
        p.setPen(QColor("#666688"));
        p.drawText(area, Qt::AlignCenter, tr("Collecting data..."));
        return;
    }

    double tMin = trendHistory_.first();
    double tMax = trendHistory_.first();
    for (double v : trendHistory_) {
        if (v < tMin)
            tMin = v;
        if (v > tMax)
            tMax = v;
    }
    double range = tMax - tMin;
    if (range < 1.0)
        range = 1.0;
    tMin -= range * 0.1;
    tMax += range * 0.1;
    range = tMax - tMin;

    QPolygonF curve;
    for (int i = 0; i < trendHistory_.size(); ++i) {
        double x = area.left() + (static_cast<double>(i) / (trendHistory_.size() - 1)) * area.width();
        double normalized = (trendHistory_[i] - tMin) / range;
        double y = area.bottom() - normalized * area.height();
        curve << QPointF(x, y);
    }

    p.setPen(QPen(QColor("#a78bfa"), 2));
    p.drawPolyline(curve);

    if (stddev_ > 0 && avg_ > 0) {
        double normalized = (avg_ - tMin) / range;
        int yAvg = area.bottom() - static_cast<int>(normalized * area.height());
        p.setPen(QPen(QColor("#a78bfa40"), 1, Qt::DashLine));
        p.drawLine(area.left(), yAvg, area.right(), yAvg);
        p.setPen(QColor("#a78bfa"));
        p.drawText(area.right() - 30, yAvg - 4, "avg");
    }
}
