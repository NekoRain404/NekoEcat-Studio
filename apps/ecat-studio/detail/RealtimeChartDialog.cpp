// RealtimeChartDialog — scrolling live chart for Free Run entries.
// Up to 10,000 points, switchable X-axis (Seconds / Samples).

#include "RealtimeChartDialog.h"

#include <QBrush>
#include <QComboBox>
#include <QDateTime>
#include <QFont>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QtMath>
#include <QVBoxLayout>

static const int kMaxPoints = 10000;

// ════════════════════════════════════════════════════════════════════
//  ChartWidget — custom QPainter line chart with scrolling
// ════════════════════════════════════════════════════════════════════

ChartWidget::ChartWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ChartWidget::addPoint(double value) {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (points_.isEmpty())
        startTimeMs_ = now;

    ChartPoint pt;
    pt.value = value;
    pt.timestampMs = now - startTimeMs_;
    points_.append(pt);

    /* Hard cap at kMaxPoints. */
    if (points_.size() > kMaxPoints) {
        points_.removeFirst();
    }
    update();
}

void ChartWidget::clear() {
    points_.clear();
    startTimeMs_ = 0;
    yMin_ = 0;
    yMax_ = 1;
    yRange_ = 1;
    update();
}

void ChartWidget::setLabel(const QString& name) {
    seriesName_ = name;
}

void ChartWidget::setXAxisMode(ChartXAxisMode mode) {
    xAxisMode_ = mode;
    update();
}

void ChartWidget::setVisibleWindow(int points) {
    visibleWindow_ = qMax(points, 10);
    update();
}

// Recalculate Y range over the visible window only.
void ChartWidget::recalcVisibleRange(double& visYMin, double& visYMax) {
    if (points_.isEmpty()) {
        visYMin = 0;
        visYMax = 1;
        return;
    }

    /* Only consider the last visibleWindow_ points for Y scaling. */
    const int start = qMax(0, points_.size() - visibleWindow_);
    visYMin = points_[start].value;
    visYMax = points_[start].value;
    for (int i = start; i < points_.size(); ++i) {
        if (points_[i].value < visYMin)
            visYMin = points_[i].value;
        if (points_[i].value > visYMax)
            visYMax = points_[i].value;
    }
    const double pad = qMax(qAbs(visYMax - visYMin) * 0.15, 0.5);
    visYMin -= pad;
    visYMax += pad;
}

void ChartWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int W = width();
    const int H = height();
    const int mL = 64, mR = 16, mT = 28, mB = 36;
    const int cW = W - mL - mR;
    const int cH = H - mT - mB;

    /* Background */
    p.fillRect(rect(), palette().window());

    if (points_.size() < 2 || cW < 10 || cH < 10) {
        p.setPen(palette().color(QPalette::PlaceholderText));
        QFont f = p.font();
        f.setPointSizeF(10);
        p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, points_.isEmpty() ? "Waiting for data..." : "Plotting...");
        return;
    }

    /* Visible window: the last N points. */
    const int visCount = qMin(visibleWindow_, points_.size());
    const int visStart = points_.size() - visCount;

    /* Y range over visible window. */
    double visYMin, visYMax;
    recalcVisibleRange(visYMin, visYMax);
    const double visYRange = (visYMax - visYMin) < 1e-9 ? 1.0 : (visYMax - visYMin);

    /* X range: either time or sample index. */
    const qint64 xMin = points_[visStart].timestampMs;
    const qint64 xMax = points_.last().timestampMs;
    const qint64 xRange = qMax(xMax - xMin, 1LL);

    /* ── Grid lines + Y labels ────────────────────────────────── */
    QFont smallFont = p.font();
    smallFont.setPointSizeF(8);
    p.setFont(smallFont);

    const int gridLines = 5;
    for (int i = 0; i <= gridLines; ++i) {
        const double ratio = static_cast<double>(i) / gridLines;
        const int y = mT + static_cast<int>(cH * (1.0 - ratio));
        const double val = visYMin + visYRange * ratio;

        p.setPen(QPen(palette().color(QPalette::Mid), 0.5, Qt::DotLine));
        p.drawLine(mL, y, W - mR, y);

        p.setPen(palette().color(QPalette::Text));
        QString label;
        if (qAbs(val) >= 10000)
            label = QString::number(val, 'f', 0);
        else if (qAbs(val) >= 1)
            label = QString::number(val, 'f', 2);
        else
            label = QString::number(val, 'f', 4);
        p.drawText(QRect(0, y - 8, mL - 4, 16), Qt::AlignRight | Qt::AlignVCenter, label);
    }

    /* ── X-axis labels ────────────────────────────────────────── */
    p.setPen(palette().color(QPalette::PlaceholderText));
    for (int i = 0; i <= 4; ++i) {
        const double ratio = static_cast<double>(i) / 4.0;
        const int x = mL + static_cast<int>(cW * ratio);
        QString label;
        if (xAxisMode_ == ChartXAxisMode::Samples) {
            const int sampleIdx = visStart + static_cast<int>(visCount * ratio);
            label = QString::number(sampleIdx);
        } else {
            const double sec = (xMin + static_cast<qint64>(xRange * ratio)) / 1000.0;
            label = QString::number(sec, 'f', 1) + "s";
        }
        p.drawText(QRect(x - 30, H - mB + 4, 60, 20), Qt::AlignCenter, label);
    }

    /* ── Scrolling line ───────────────────────────────────────── */
    QPainterPath path;
    for (int i = 0; i < visCount; ++i) {
        const int idx = visStart + i;
        double xRatio;
        if (xAxisMode_ == ChartXAxisMode::Samples) {
            xRatio = static_cast<double>(i) / qMax(visCount - 1, 1);
        } else {
            xRatio = static_cast<double>(points_[idx].timestampMs - xMin) / xRange;
        }
        const double yRatio = (points_[idx].value - visYMin) / visYRange;
        const int x = mL + static_cast<int>(cW * xRatio);
        const int y = mT + static_cast<int>(cH * (1.0 - yRatio));
        if (i == 0)
            path.moveTo(x, y);
        else
            path.lineTo(x, y);
    }

    /* Fill under curve */
    QPainterPath fillPath = path;
    fillPath.lineTo(mL + cW, mT + cH);
    fillPath.lineTo(mL, mT + cH);
    fillPath.closeSubpath();
    QLinearGradient grad(0, mT, 0, mT + cH);
    grad.setColorAt(0.0, QColor(59, 130, 246, 35));
    grad.setColorAt(1.0, QColor(59, 130, 246, 3));
    p.fillPath(fillPath, grad);

    /* Stroke line */
    p.setPen(QPen(QColor(59, 130, 246), 2));
    p.drawPath(path);

    /* Current value dot */
    if (!points_.isEmpty()) {
        const auto& last = points_.last();
        double xRatio;
        if (xAxisMode_ == ChartXAxisMode::Samples) {
            xRatio = 1.0;
        } else {
            xRatio = static_cast<double>(last.timestampMs - xMin) / xRange;
        }
        const double yRatio = (last.value - visYMin) / visYRange;
        const int x = mL + static_cast<int>(cW * xRatio);
        const int y = mT + static_cast<int>(cH * (1.0 - yRatio));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(59, 130, 246));
        p.drawEllipse(QPoint(x, y), 5, 5);
        p.setBrush(QColor(255, 255, 255));
        p.drawEllipse(QPoint(x, y), 2, 2);
    }

    /* Title */
    p.setPen(palette().color(QPalette::Text));
    QFont titleFont = p.font();
    titleFont.setPointSizeF(10);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.drawText(QRect(mL, 2, cW, 20), Qt::AlignLeft, seriesName_.isEmpty() ? "Value" : seriesName_);
}

// ════════════════════════════════════════════════════════════════════
//  RealtimeChartDialog — controls + chart + stats
// ════════════════════════════════════════════════════════════════════

RealtimeChartDialog::RealtimeChartDialog(const QString& entryName, const QString& entryKey, double initialValue,
                                         QWidget* parent)
    : QDialog(parent), entryKey_(entryKey), lastValue_(initialValue) {
    setObjectName("realtimeChartDialog");
    setWindowTitle(tr("Real-time Monitor — %1").arg(entryName));
    setModal(false);
    resize(800, 480);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 10);
    layout->setSpacing(8);

    /* ── Controls row ─────────────────────────────────────────── */
    auto* ctrlRow = new QHBoxLayout;

    auto* nameLbl = new QLabel(entryName);
    nameLbl->setStyleSheet("font-weight: bold; font-size: 13px;");
    ctrlRow->addWidget(nameLbl);
    ctrlRow->addStretch(1);

    /* X-axis unit selector */
    ctrlRow->addWidget(new QLabel(tr("X-axis:")));
    xAxisCombo_ = new QComboBox;
    xAxisCombo_->addItem(tr("Seconds"), 0);
    xAxisCombo_->addItem(tr("Samples"), 1);
    ctrlRow->addWidget(xAxisCombo_);
    connect(xAxisCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &RealtimeChartDialog::onXAxisModeChanged);

    /* Visible window size (unit depends on X-axis mode) */
    ctrlRow->addWidget(new QLabel(tr("Window:")));
    windowSpin_ = new QSpinBox;
    windowSpin_->setValue(200);
    ctrlRow->addWidget(windowSpin_);
    connect(windowSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int val) {
        if (xAxisCombo_->currentIndex() == 0) {
            /* Seconds mode: store seconds, convert to points. */
            equivalentSeconds_ = val;
            const int pts = qMax(1, val * 1000 / pollingIntervalMs_);
            chart_->setVisibleWindow(pts);
        } else {
            /* Samples mode: store points directly. */
            equivalentPoints_ = val;
            chart_->setVisibleWindow(val);
        }
    });

    /* Poll interval */
    ctrlRow->addWidget(new QLabel(tr("Poll:")));
    intervalCombo_ = new QComboBox;
    intervalCombo_->addItem(tr("50 ms"), 50);
    intervalCombo_->addItem(tr("100 ms"), 100);
    intervalCombo_->addItem(tr("200 ms"), 200);
    intervalCombo_->addItem(tr("500 ms"), 500);
    intervalCombo_->addItem(tr("1 s"), 1000);
    intervalCombo_->setCurrentIndex(1);
    ctrlRow->addWidget(intervalCombo_);
    connect(intervalCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &RealtimeChartDialog::updatePollingInterval);

    /* Start / Stop */
    recordBtn_ = new QPushButton(QString("▶ ") + tr("Start"));
    recordBtn_->setCheckable(true);
    ctrlRow->addWidget(recordBtn_);

    layout->addLayout(ctrlRow);

    /* ── Chart ────────────────────────────────────────────────── */
    chart_ = new ChartWidget;
    chart_->setLabel(entryName);
    layout->addWidget(chart_, 1);

    /* ── Stats row ────────────────────────────────────────────── */
    auto* statsRow = new QHBoxLayout;
    valueLabel_ = new QLabel;
    statsLabel_ = new QLabel;
    statsRow->addWidget(valueLabel_);
    statsRow->addStretch(1);
    statsRow->addWidget(statsLabel_);
    layout->addLayout(statsRow);

    connect(recordBtn_, &QPushButton::clicked, this, &RealtimeChartDialog::toggleRecording);

    /* Configure window spin for initial X-axis mode (Seconds). */
    updateAxisUi();

    /* Seed first point. */
    feedValue(initialValue);
}

void RealtimeChartDialog::feedValue(double value) {
    lastValue_ = value;
    valueLabel_->setText(tr("Current: %1").arg(value, 0, 'f', 4));
    updateStats(value);
    chart_->addPoint(value);
    totalPoints_++;
}

void RealtimeChartDialog::toggleRecording() {
    recording_ = recordBtn_->isChecked();
    if (recording_) {
        recordBtn_->setText(QString("■ ") + tr("Stop"));
    } else {
        recordBtn_->setText(QString("▶ ") + tr("Start"));
    }
}

void RealtimeChartDialog::updateStats(double value) {
    ++count_;
    sum_ += value;
    if (count_ == 1) {
        peak_ = value;
        trough_ = value;
    }
    if (value > peak_)
        peak_ = value;
    if (value < trough_)
        trough_ = value;
    const double avg = sum_ / count_;
    statsLabel_->setText(tr("Avg: %1  |  Peak: %2  |  Low: %3  |  Samples: %4  |  Total: %5")
                             .arg(avg, 0, 'f', 4)
                             .arg(peak_, 0, 'f', 4)
                             .arg(trough_, 0, 'f', 4)
                             .arg(count_)
                             .arg(totalPoints_));
}

// Reconfigure the window spin box's range, suffix, and value
// based on the current X-axis mode.
void RealtimeChartDialog::updateAxisUi() {
    const bool isSeconds = (xAxisCombo_->currentIndex() == 0);

    /* Block signals so setValue() doesn't trigger the lambda in the
       connection, which would overwrite the equivalent* fields. */
    windowSpin_->blockSignals(true);

    if (isSeconds) {
        windowSpin_->setRange(1, 300);
        windowSpin_->setSingleStep(1);
        windowSpin_->setSuffix(tr(" s"));
        windowSpin_->setValue(equivalentSeconds_);
        chart_->setVisibleWindow(qMax(1, equivalentSeconds_ * 1000 / pollingIntervalMs_));
    } else {
        windowSpin_->setRange(10, 10000);
        windowSpin_->setSingleStep(50);
        windowSpin_->setSuffix(tr(" pts"));
        windowSpin_->setValue(equivalentPoints_);
        chart_->setVisibleWindow(equivalentPoints_);
    }

    windowSpin_->blockSignals(false);
}

// X-axis mode changed: switch chart mode, then reconfigure window spin.
void RealtimeChartDialog::onXAxisModeChanged(int idx) {
    chart_->setXAxisMode(idx == 0 ? ChartXAxisMode::Seconds : ChartXAxisMode::Samples);
    updateAxisUi();
}

// Polling interval changed: recalculate seconds↔points conversion.
void RealtimeChartDialog::updatePollingInterval(int idx) {
    const int newMs = intervalCombo_->itemData(idx).toInt();
    if (newMs <= 0)
        return;
    pollingIntervalMs_ = newMs;
    emit pollingIntervalChanged(pollingIntervalMs_);

    /* Recalculate the current visible window with the new interval. */
    updateAxisUi();
}
