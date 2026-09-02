// EcatChartWidget — implementation.  See header for interface documentation.
#include "EcatChartWidget.h"
#include "services/ChartService.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

const QColor EcatChartWidget::kColors[kColorCount] = {
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

EcatChartWidget::EcatChartWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(300, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
}

void EcatChartWidget::setChartType(ChartType type) {
    type_ = type;
    update();
}

void EcatChartWidget::setTitle(const QString& title) {
    title_ = title;
    update();
}

void EcatChartWidget::setLabels(const QStringList& labels) {
    labels_ = labels;
    update();
}

void EcatChartWidget::setDatasets(const QVector<ChartDataset>& datasets) {
    datasets_ = datasets;
    update();
}

void EcatChartWidget::setGaugeValue(double value, double min, double max) {
    gaugeValue_ = value;
    gaugeMin_ = min;
    gaugeMax_ = max;
    update();
}

void EcatChartWidget::mouseMoveEvent(QMouseEvent* event) {
    mousePos_ = event->pos();
    update();
}

// ── Paint ─────────────────────────────────────────────────────────────

void EcatChartWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    p.fillRect(rect(), QColor(30, 30, 30));

    const QRect area(kMarginLeft, kMarginTop, width() - kMarginLeft - kMarginRight,
                     height() - kMarginTop - kMarginBottom);
    if (area.width() < 10 || area.height() < 10)
        return;

    drawTitle(p, area);

    switch (type_) {
        case Line:
            drawLineChart(p, area);
            break;
        case Bar:
            drawBarChart(p, area);
            break;
        case Pie:
            drawPieChart(p, area);
            break;
        case Scatter:
            drawScatterChart(p, area);
            break;
        case Gauge:
            drawGaugeChart(p, area);
            break;
    }

    if (type_ != Gauge && type_ != Pie) {
        drawLegend(p, area);
    }

    drawTooltip(p);
}

void EcatChartWidget::drawTitle(QPainter& p, const QRect& area) {
    Q_UNUSED(area);
    p.setPen(QColor(220, 220, 220));
    QFont f = font();
    f.setPointSize(qMax(10, f.pointSize() + 2));
    f.setBold(true);
    p.setFont(f);
    p.drawText(0, 2, width(), kMarginTop - 4, Qt::AlignCenter, title_);
}

void EcatChartWidget::drawLineChart(QPainter& p, const QRect& area) {
    if (datasets_.isEmpty()) {
        p.setPen(QColor(120, 120, 120));
        p.drawText(area, Qt::AlignCenter, tr("No data"));
        return;
    }

    double yMin = 0.0, yMax = 1.0;
    bool first = true;
    for (const auto& ds : datasets_) {
        for (double v : ds.values) {
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

    // Grid.
    p.setPen(QPen(QColor(60, 60, 60), 1, Qt::DotLine));
    for (int i = 0; i <= 5; ++i) {
        const int y = area.top() + i * area.height() / 5;
        p.drawLine(area.left(), y, area.right(), y);
    }
    for (int i = 0; i <= 10; ++i) {
        const int x = area.left() + i * area.width() / 10;
        p.drawLine(x, area.top(), x, area.bottom());
    }

    // Y-axis labels.
    p.setPen(QColor(180, 180, 180));
    QFont lf = font();
    lf.setPointSize(qMax(7, lf.pointSize() - 1));
    p.setFont(lf);
    for (int i = 0; i <= 5; ++i) {
        const int y = area.top() + i * area.height() / 5;
        const double val = yMax - i * yRange / 5.0;
        p.drawText(2, y - 8, kMarginLeft - 8, 16, Qt::AlignRight | Qt::AlignVCenter, QString::number(val, 'g', 4));
    }

    // Lines.
    for (int di = 0; di < datasets_.size(); ++di) {
        const auto& ds = datasets_[di];
        if (ds.values.size() < 2)
            continue;

        const QColor color = ds.color.isValid() ? ds.color : kColors[di % kColorCount];
        p.setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        QPainterPath path;
        for (int i = 0; i < ds.values.size(); ++i) {
            const double xFrac = labels_.isEmpty()
                                     ? static_cast<double>(i) / (ds.values.size() - 1)
                                     : (labels_.size() > 1 ? static_cast<double>(i) / (labels_.size() - 1) : 0.0);
            const double yFrac = (ds.values[i] - yMin) / yRange;
            const double px = area.left() + xFrac * area.width();
            const double py = area.bottom() - yFrac * area.height();
            if (i == 0)
                path.moveTo(px, py);
            else
                path.lineTo(px, py);
        }
        p.drawPath(path);
    }
}

void EcatChartWidget::drawBarChart(QPainter& p, const QRect& area) {
    if (datasets_.isEmpty() || labels_.isEmpty()) {
        p.setPen(QColor(120, 120, 120));
        p.drawText(area, Qt::AlignCenter, tr("No data"));
        return;
    }

    double yMax = 0.0;
    for (const auto& ds : datasets_) {
        for (double v : ds.values) {
            if (v > yMax)
                yMax = v;
        }
    }
    if (qFuzzyIsNull(yMax))
        yMax = 1.0;

    // Grid.
    p.setPen(QPen(QColor(60, 60, 60), 1, Qt::DotLine));
    for (int i = 0; i <= 5; ++i) {
        const int y = area.top() + i * area.height() / 5;
        p.drawLine(area.left(), y, area.right(), y);
    }

    // Y-axis labels.
    p.setPen(QColor(180, 180, 180));
    QFont lf = font();
    lf.setPointSize(qMax(7, lf.pointSize() - 1));
    p.setFont(lf);
    for (int i = 0; i <= 5; ++i) {
        const int y = area.top() + i * area.height() / 5;
        const double val = yMax * (5 - i) / 5.0;
        p.drawText(2, y - 8, kMarginLeft - 8, 16, Qt::AlignRight | Qt::AlignVCenter, QString::number(val, 'g', 4));
    }

    const int nGroups = labels_.size();
    const int nSeries = datasets_.size();
    const double groupWidth = static_cast<double>(area.width()) / nGroups;
    const double barWidth = groupWidth / (nSeries + 1);

    for (int gi = 0; gi < nGroups; ++gi) {
        for (int si = 0; si < nSeries; ++si) {
            const auto& ds = datasets_[si];
            if (gi >= ds.values.size())
                continue;

            const QColor color = ds.color.isValid() ? ds.color : kColors[si % kColorCount];
            const double x = area.left() + gi * groupWidth + (si + 0.5) * barWidth;
            const double h = (ds.values[gi] / yMax) * area.height();
            const QRectF barRect(x, area.bottom() - h, barWidth * 0.9, h);
            p.fillRect(barRect, color);
        }
    }

    // X-axis labels.
    p.setPen(QColor(180, 180, 180));
    for (int gi = 0; gi < nGroups; ++gi) {
        const int cx = area.left() + static_cast<int>((gi + 0.5) * groupWidth);
        p.drawText(cx - 40, area.bottom() + 4, 80, 20, Qt::AlignCenter, labels_[gi]);
    }
}

void EcatChartWidget::drawPieChart(QPainter& p, const QRect& area) {
    if (datasets_.isEmpty() || datasets_[0].values.isEmpty()) {
        p.setPen(QColor(120, 120, 120));
        p.drawText(area, Qt::AlignCenter, tr("No data"));
        return;
    }

    const auto& values = datasets_[0].values;
    double total = 0.0;
    for (double v : values)
        total += qAbs(v);
    if (qFuzzyIsNull(total))
        return;

    const int diameter = qMin(area.width(), area.height()) - 40;
    const QRect pieRect(area.center().x() - diameter / 2, area.center().y() - diameter / 2, diameter, diameter);

    double startAngle = 0.0;
    for (int i = 0; i < values.size(); ++i) {
        const double span = (qAbs(values[i]) / total) * 360.0 * 16;
        const QColor color = (i < datasets_[0].color.isValid()) ? datasets_[0].color : kColors[i % kColorCount];
        p.setBrush(color);
        p.setPen(QPen(QColor(30, 30, 30), 1));
        p.drawPie(pieRect, static_cast<int>(startAngle), static_cast<int>(span));

        // Label.
        const double midAngle = (startAngle + span / 2) * M_PI / (180.0 * 16);
        const int lx = pieRect.center().x() + static_cast<int>(qCos(midAngle) * diameter * 0.35);
        const int ly = pieRect.center().y() - static_cast<int>(qSin(midAngle) * diameter * 0.35);
        const double pct = qAbs(values[i]) / total * 100.0;
        QString label = i < labels_.size() ? labels_[i] : QString::number(i);
        p.setPen(Qt::white);
        QFont lf = font();
        lf.setPointSize(qMax(7, lf.pointSize() - 1));
        p.setFont(lf);
        p.drawText(lx - 30, ly - 10, 60, 20, Qt::AlignCenter, QStringLiteral("%1\n%2%").arg(label).arg(pct, 0, 'f', 1));
        startAngle += span;
    }
}

void EcatChartWidget::drawScatterChart(QPainter& p, const QRect& area) {
    if (datasets_.isEmpty()) {
        p.setPen(QColor(120, 120, 120));
        p.drawText(area, Qt::AlignCenter, tr("No data"));
        return;
    }

    double xMin = 0.0, xMax = 1.0, yMin = 0.0, yMax = 1.0;
    bool first = true;
    for (const auto& ds : datasets_) {
        for (int i = 0; i + 1 < ds.values.size(); i += 2) {
            const double x = ds.values[i];
            const double y = ds.values[i + 1];
            if (first || x < xMin)
                xMin = x;
            if (first || x > xMax)
                xMax = x;
            if (first || y < yMin)
                yMin = y;
            if (first || y > yMax)
                yMax = y;
            first = false;
        }
    }
    if (qFuzzyCompare(xMin + 1.0, xMax + 1.0)) {
        xMin -= 1.0;
        xMax += 1.0;
    }
    if (qFuzzyCompare(yMin + 1.0, yMax + 1.0)) {
        yMin -= 1.0;
        yMax += 1.0;
    }
    const double xRange = xMax - xMin;
    const double yRange = yMax - yMin;

    // Grid.
    p.setPen(QPen(QColor(60, 60, 60), 1, Qt::DotLine));
    for (int i = 0; i <= 5; ++i) {
        const int y = area.top() + i * area.height() / 5;
        p.drawLine(area.left(), y, area.right(), y);
    }
    for (int i = 0; i <= 10; ++i) {
        const int x = area.left() + i * area.width() / 10;
        p.drawLine(x, area.top(), x, area.bottom());
    }

    // Points.
    for (int di = 0; di < datasets_.size(); ++di) {
        const auto& ds = datasets_[di];
        const QColor color = ds.color.isValid() ? ds.color : kColors[di % kColorCount];
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        for (int i = 0; i + 1 < ds.values.size(); i += 2) {
            const double px = area.left() + ((ds.values[i] - xMin) / xRange) * area.width();
            const double py = area.bottom() - ((ds.values[i + 1] - yMin) / yRange) * area.height();
            p.drawEllipse(QPointF(px, py), 3, 3);
        }
    }
}

void EcatChartWidget::drawGaugeChart(QPainter& p, const QRect& area) {
    const double range = gaugeMax_ - gaugeMin_;
    const double frac = range > 0 ? qBound(0.0, (gaugeValue_ - gaugeMin_) / range, 1.0) : 0.0;

    const int cx = area.center().x();
    const int cy = area.center().y() + 10;
    const int radius = qMin(area.width(), area.height()) / 2 - 20;

    // Background arc.
    p.setPen(QPen(QColor(60, 60, 60), 12, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(cx - radius, cy - radius, radius * 2, radius * 2, 225 * 16, -270 * 16);

    // Value arc.
    const int spanAngle = static_cast<int>(-270 * 16 * frac);
    QColor arcColor = QColor(44, 160, 44);
    if (frac > 0.7)
        arcColor = QColor(255, 127, 14);
    if (frac > 0.9)
        arcColor = QColor(214, 39, 40);
    p.setPen(QPen(arcColor, 12, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(cx - radius, cy - radius, radius * 2, radius * 2, 225 * 16, spanAngle);

    // Value text.
    p.setPen(QColor(220, 220, 220));
    QFont vf = font();
    vf.setPointSize(qMax(14, vf.pointSize() + 8));
    vf.setBold(true);
    p.setFont(vf);
    p.drawText(cx - 80, cy - 20, 160, 40, Qt::AlignCenter, QString::number(gaugeValue_, 'f', 1));

    // Min/Max labels.
    p.setPen(QColor(150, 150, 150));
    QFont sf = font();
    sf.setPointSize(qMax(7, sf.pointSize() - 1));
    p.setFont(sf);
    p.drawText(cx - radius - 20, cy + radius + 4, 60, 16, Qt::AlignCenter, QString::number(gaugeMin_, 'f', 0));
    p.drawText(cx + radius - 40, cy + radius + 4, 60, 16, Qt::AlignCenter, QString::number(gaugeMax_, 'f', 0));
}

void EcatChartWidget::drawLegend(QPainter& p, const QRect& area) {
    if (datasets_.isEmpty())
        return;
    p.setPen(QColor(200, 200, 200));
    QFont lf = font();
    lf.setPointSize(qMax(7, lf.pointSize() - 1));
    p.setFont(lf);

    for (int i = 0; i < datasets_.size(); ++i) {
        const int y = area.top() + 4 + i * 16;
        if (y + 12 > area.bottom())
            break;
        const QColor color = datasets_[i].color.isValid() ? datasets_[i].color : kColors[i % kColorCount];
        p.fillRect(area.right() - 100, y, 10, 10, color);
        p.drawText(area.right() - 86, y, 80, 12, Qt::AlignLeft | Qt::AlignVCenter, datasets_[i].name);
    }
}

void EcatChartWidget::drawTooltip(QPainter& p) {
    if (!rect().contains(mousePos_))
        return;
    if (type_ == Gauge || type_ == Pie)
        return;
    if (datasets_.isEmpty() || labels_.isEmpty())
        return;

    const QRect area(kMarginLeft, kMarginTop, width() - kMarginLeft - kMarginRight,
                     height() - kMarginTop - kMarginBottom);
    if (!area.contains(mousePos_))
        return;

    const int labelIdx = static_cast<int>((mousePos_.x() - area.left()) * labels_.size() / area.width());
    if (labelIdx < 0 || labelIdx >= labels_.size())
        return;

    QString tip = labels_[labelIdx];
    for (const auto& ds : datasets_) {
        if (labelIdx < ds.values.size()) {
            tip += QStringLiteral("\n%1: %2").arg(ds.name).arg(ds.values[labelIdx]);
        }
    }

    const QFontMetrics fm(p.font());
    const int tw = fm.horizontalAdvance(tip.split('\n').first()) + 12;
    const int th = fm.height() * tip.count('\n') + fm.height() + 8;
    const int tx = qMin(mousePos_.x() + 10, width() - tw);
    const int ty = qMax(mousePos_.y() - th, 0);

    p.fillRect(tx, ty, tw, th, QColor(0, 0, 0, 200));
    p.setPen(QColor(220, 220, 220));
    p.drawText(tx + 4, ty + 4, tw - 8, th - 8, Qt::TextWordWrap, tip);
}
