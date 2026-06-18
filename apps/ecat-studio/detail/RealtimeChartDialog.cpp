// RealtimeChartDialog — live value chart for Free Run entries.
// Renders a scrolling line chart using QPainter, no QtCharts dependency.

#include "RealtimeChartDialog.h"

#include <QBrush>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFont>
#include <QFormLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSizePolicy>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

// ════════════════════════════════════════════════════════════════════
//  ChartWidget — custom QWidget that paints the line chart
// ════════════════════════════════════════════════════════════════════

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);
}

void ChartWidget::addPoint(double value)
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (points_.isEmpty()) startTimeMs_ = now;

    ChartPoint pt; pt.value = value; pt.timestampMs = now - startTimeMs_; points_.append(pt);

    /* Keep a rolling window of maxPoints. */
    if (points_.size() > 300) {
        points_.removeFirst();
    }
    recalcRange();
    update();
}

void ChartWidget::clear()
{
    points_.clear();
    startTimeMs_ = 0;
    yMin_ = 0; yMax_ = 1; yRange_ = 1;
    update();
}

void ChartWidget::setLabel(const QString &name, const QString &unit)
{
    seriesName_ = name;
    unit_ = unit;
}

void ChartWidget::recalcRange()
{
    if (points_.isEmpty()) { yMin_ = 0; yMax_ = 1; yRange_ = 1; return; }
    yMin_ = points_.first().value;
    yMax_ = points_.first().value;
    for (const auto &p : points_) {
        if (p.value < yMin_) yMin_ = p.value;
        if (p.value > yMax_) yMax_ = p.value;
    }
    /* Add 10% padding. */
    const double pad = qMax(qAbs(yMax_ - yMin_) * 0.1, 0.5);
    yMin_ -= pad;
    yMax_ += pad;
    yRange_ = yMax_ - yMin_;
    if (yRange_ < 1e-9) yRange_ = 1.0;
}

void ChartWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int W = width();
    const int H = height();
    const int marginL = 60, marginR = 16, marginT = 30, marginB = 40;
    const int chartW = W - marginL - marginR;
    const int chartH = H - marginT - marginB;

    /* Background */
    p.fillRect(rect(), palette().window());

    if (points_.size() < 2 || chartW < 10 || chartH < 10) {
        p.setPen(palette().color(QPalette::PlaceholderText));
        p.drawText(rect(), Qt::AlignCenter,
                   seriesName_.isEmpty() ? "Waiting for data..." : seriesName_);
        return;
    }

    /* ── Grid lines and Y-axis labels ─────────────────────────── */
    p.setPen(QPen(palette().color(QPalette::Mid), 1, Qt::DashLine));
    QFont smallFont = p.font();
    smallFont.setPointSizeF(8);
    p.setFont(smallFont);
    p.setPen(palette().color(QPalette::PlaceholderText));

    const int gridLines = 5;
    for (int i = 0; i <= gridLines; ++i) {
        const double ratio = static_cast<double>(i) / gridLines;
        const int y = marginT + static_cast<int>(chartH * (1.0 - ratio));
        const double val = yMin_ + yRange_ * ratio;

        /* Grid line */
        p.setPen(QPen(palette().color(QPalette::Mid), 0.5, Qt::DotLine));
        p.drawLine(marginL, y, W - marginR, y);

        /* Y label */
        p.setPen(palette().color(QPalette::Text));
        QString label;
        if (qAbs(val) >= 1000)
            label = QString::number(val, 'f', 0);
        else if (qAbs(val) >= 1)
            label = QString::number(val, 'f', 2);
        else
            label = QString::number(val, 'f', 4);
        p.drawText(QRect(0, y - 8, marginL - 4, 16),
                   Qt::AlignRight | Qt::AlignVCenter, label);
    }

    /* ── X-axis time labels ───────────────────────────────────── */
    p.setPen(palette().color(QPalette::PlaceholderText));
    const qint64 tMin = points_.first().timestampMs;
    const qint64 tMax = points_.last().timestampMs;
    const qint64 tRange = qMax(tMax - tMin, 1LL);
    for (int i = 0; i <= 4; ++i) {
        const double ratio = static_cast<double>(i) / 4.0;
        const int x = marginL + static_cast<int>(chartW * ratio);
        const qint64 t = tMin + static_cast<qint64>(tRange * ratio);
        const double sec = t / 1000.0;
        p.drawText(QRect(x - 30, H - marginB + 4, 60, 20),
                   Qt::AlignCenter, QString::number(sec, 'f', 1) + "s");
    }

    /* ── Line path ────────────────────────────────────────────── */
    QPainterPath path;
    for (int i = 0; i < points_.size(); ++i) {
        const double xRatio = static_cast<double>(points_[i].timestampMs - tMin) / tRange;
        const double yRatio = (points_[i].value - yMin_) / yRange_;
        const int x = marginL + static_cast<int>(chartW * xRatio);
        const int y = marginT + static_cast<int>(chartH * (1.0 - yRatio));
        if (i == 0) path.moveTo(x, y);
        else path.lineTo(x, y);
    }

    /* Fill under the curve */
    QPainterPath fillPath = path;
    fillPath.lineTo(marginL + chartW, marginT + chartH);
    fillPath.lineTo(marginL, marginT + chartH);
    fillPath.closeSubpath();
    QLinearGradient grad(0, marginT, 0, marginT + chartH);
    grad.setColorAt(0.0, QColor(59, 130, 246, 40));
    grad.setColorAt(1.0, QColor(59, 130, 246, 5));
    p.fillPath(fillPath, grad);

    /* Stroke the line */
    p.setPen(QPen(QColor(59, 130, 246), 2));
    p.drawPath(path);

    /* Current value dot */
    if (!points_.isEmpty()) {
        const auto &last = points_.last();
        const double xRatio = static_cast<double>(last.timestampMs - tMin) / tRange;
        const double yRatio = (last.value - yMin_) / yRange_;
        const int x = marginL + static_cast<int>(chartW * xRatio);
        const int y = marginT + static_cast<int>(chartH * (1.0 - yRatio));
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
    p.drawText(QRect(marginL, 4, chartW, 22), Qt::AlignLeft,
               seriesName_.isEmpty() ? "Value" : seriesName_);
}

// ════════════════════════════════════════════════════════════════════
//  RealtimeChartDialog — wraps ChartWidget with controls
// ════════════════════════════════════════════════════════════════════

RealtimeChartDialog::RealtimeChartDialog(const QString &entryName,
                                         const QString &entryKey,
                                         double initialValue,
                                         QWidget *parent)
    : QDialog(parent), entryKey_(entryKey), lastValue_(initialValue)
{
    setObjectName("realtimeChartDialog");
    setWindowTitle(tr("Real-time Monitor — %1").arg(entryName));
    setModal(false);
    resize(720, 440);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 10);
    layout->setSpacing(8);

    /* ── Info row ─────────────────────────────────────────────── */
    auto *infoRow = new QHBoxLayout;
    auto *nameLbl = new QLabel(entryName);
    nameLbl->setStyleSheet("font-weight: bold; font-size: 13px;");
    infoRow->addWidget(nameLbl);
    infoRow->addStretch(1);

    auto *intervalLbl = new QLabel(tr("Poll:"));
    intervalCombo_ = new QComboBox;
    intervalCombo_->addItem(tr("50 ms"), 50);
    intervalCombo_->addItem(tr("100 ms"), 100);
    intervalCombo_->addItem(tr("200 ms"), 200);
    intervalCombo_->addItem(tr("500 ms"), 500);
    intervalCombo_->addItem(tr("1 s"), 1000);
    intervalCombo_->setCurrentIndex(1);
    infoRow->addWidget(intervalLbl);
    infoRow->addWidget(intervalCombo_);

    recordBtn_ = new QPushButton(tr("▶ Start"));
    recordBtn_->setCheckable(true);
    infoRow->addWidget(recordBtn_);
    layout->addLayout(infoRow);

    /* ── Chart ────────────────────────────────────────────────── */
    chart_ = new ChartWidget;
    chart_->setLabel(entryName, "");
    layout->addWidget(chart_, 1);

    /* ── Stats row ────────────────────────────────────────────── */
    auto *statsRow = new QHBoxLayout;
    valueLabel_ = new QLabel;
    statsLabel_ = new QLabel;
    statsRow->addWidget(valueLabel_);
    statsRow->addStretch(1);
    statsRow->addWidget(statsLabel_);
    layout->addLayout(statsRow);

    /* ── Timer ────────────────────────────────────────────────── */
    pollTimer_ = new QTimer(this);
    pollTimer_->setSingleShot(false);
    connect(pollTimer_, &QTimer::timeout, this, &RealtimeChartDialog::onPollTick);

    connect(recordBtn_, &QPushButton::clicked, this, &RealtimeChartDialog::toggleRecording);
    connect(intervalCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) {
        if (pollTimer_->isActive()) {
            pollTimer_->setInterval(intervalCombo_->currentData().toInt());
        }
    });

    /* Seed first point. */
    feedValue(initialValue);
}

void RealtimeChartDialog::feedValue(double value)
{
    lastValue_ = value;
    valueLabel_->setText(tr("Current: %1").arg(value, 0, 'f', 4));
    updateStats(value);
    /* Always plot incoming data — Start/Stop only controls stats reset. */
    chart_->addPoint(value);
}

void RealtimeChartDialog::toggleRecording()
{
    recording_ = recordBtn_->isChecked();
    if (recording_) {
        recordBtn_->setText(tr("■ Stop"));
        pollTimer_->start(intervalCombo_->currentData().toInt());
        emit recordingChanged(true);
    } else {
        recordBtn_->setText(tr("▶ Start"));
        pollTimer_->stop();
        emit recordingChanged(false);
    }
}

void RealtimeChartDialog::onPollTick()
{
    /* The MainWindow poll cycle feeds values via feedValue(). */
    /* This timer just triggers a visual refresh of the current value. */
    /* Actual data comes from the Free Run polling mechanism. */
}

void RealtimeChartDialog::updateStats(double value)
{
    ++count_;
    sum_ += value;
    if (count_ == 1) { peak_ = value; trough_ = value; }
    if (value > peak_) peak_ = value;
    if (value < trough_) trough_ = value;
    const double avg = sum_ / count_;
    statsLabel_->setText(
        tr("Avg: %1  |  Peak: %2  |  Low: %3  |  Samples: %4")
            .arg(avg, 0, 'f', 4)
            .arg(peak_, 0, 'f', 4)
            .arg(trough_, 0, 'f', 4)
            .arg(count_));
}
