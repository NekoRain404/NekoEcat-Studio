// Real-time stability test workspace: controls, live statistics, and latency chart.
#include "MainWindow.h"

#include <QComboBox>
#include <QLineEdit>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPainter>
#include <QtMath>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

// ── High-performance QPainter latency chart (no QtCharts dependency) ─────────
// Direct QPainter rendering — 10-100x faster than QtCharts for streaming data.
class RtTestLatencyChart : public QWidget {
    static constexpr int kMax = 3000;
public:
    explicit RtTestLatencyChart(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumSize(300, 200);
    }

    void appendData(const QJsonArray &avg, const QJsonArray &minArr,
                    const QJsonArray &maxArr)
    {
        for (int i = 0; i < avg.size(); ++i) {
            bufAvg_.append(avg[i].toDouble());
            bufMin_.append(i < minArr.size() ? minArr[i].toDouble() : avg[i].toDouble());
            bufMax_.append(i < maxArr.size() ? maxArr[i].toDouble() : avg[i].toDouble());
        }
        while (bufAvg_.size() > kMax) {
            bufAvg_.removeFirst();
            bufMin_.removeFirst();
            bufMax_.removeFirst();
        }
        // Recompute Y range.
        if (!bufMin_.isEmpty()) {
            yLo_ = *std::min_element(bufMin_.begin(), bufMin_.end());
            yHi_ = *std::max_element(bufMax_.begin(), bufMax_.end());
            double pad = qMax((yHi_ - yLo_) * 0.15, 20.0);
            yLo_ -= pad;
            yHi_ += pad;
        }
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRect r = rect().adjusted(48, 24, -8, -20); // margins for labels
        // Background.
        p.fillRect(rect(), QColor("#1e1e2e"));
        // Title.
        p.setPen(QColor("#cdd6f4"));
        p.setFont(QFont("sans-serif", 10, QFont::Bold));
        p.drawText(rect().adjusted(8, 4, 0, 0), Qt::AlignTop, "Cycle Latency (µs)");

        if (bufAvg_.size() < 2 || r.width() < 10 || r.height() < 10) return;

        const int n = bufAvg_.size();
        const int xOff = kMax - n;  // right-align: empty bins on the left
        const double xScale = static_cast<double>(r.width()) / (kMax - 1);
        const double yRange = yHi_ - yLo_;
        const double yScale = yRange > 0 ? static_cast<double>(r.height()) / yRange : 1.0;

        auto toY = [&](double val) {
            return r.bottom() - (val - yLo_) * yScale;
        };
        auto toX = [&](int i) {
            return r.left() + i * xScale;
        };

        // Grid lines (horizontal).
        p.setPen(QPen(QColor("#313244"), 1));
        const double yStep = qPow(10, std::floor(std::log10(qMax(yRange / 5, 1.0))));
        for (double y = std::ceil(yLo_ / yStep) * yStep; y <= yHi_; y += yStep) {
            int py = static_cast<int>(toY(y));
            if (py >= r.top() && py <= r.bottom()) {
                p.drawLine(r.left(), py, r.right(), py);
                p.setPen(QColor("#6c7086"));
                p.setFont(QFont("monospace", 7));
                p.drawText(r.left() - 46, py - 6, 44, 12, Qt::AlignRight | Qt::AlignVCenter,
                           QString::number(y, 'f', 0));
                p.setPen(QPen(QColor("#313244"), 1));
            }
        }

        // Draw max (pink dashed) as filled area under it.
        {
            QColor maxFill("#f38ba8");
            maxFill.setAlpha(25);
            QPolygonF poly;
            poly << QPointF(toX(0), r.bottom());
            for (int i = 0; i < n; ++i) {
                poly << QPointF(toX(i), toY(bufMax_[i]));
            }
            poly << QPointF(toX(n - 1), r.bottom());
            p.setPen(Qt::NoPen);
            p.setBrush(maxFill);
            p.drawPolygon(poly);
        }

        // Draw max line (pink dashed).
        {
            QVector<QPointF> pts(n);
            for (int i = 0; i < n; ++i) pts[i] = QPointF(toX(i), toY(bufMax_[i]));
            p.setPen(QPen(QColor("#f38ba8"), 1, Qt::DashLine));
            p.setBrush(Qt::NoBrush);
            p.drawPolyline(pts);
        }

        // Draw min line (green dashed).
        {
            QVector<QPointF> pts(n);
            for (int i = 0; i < n; ++i) pts[i] = QPointF(toX(i), toY(bufMin_[i]));
            p.setPen(QPen(QColor("#a6e3a1"), 1, Qt::DashLine));
            p.drawPolyline(pts);
        }

        // Draw avg line (solid blue, thickest).
        {
            QVector<QPointF> pts(n);
            for (int i = 0; i < n; ++i) pts[i] = QPointF(toX(i), toY(bufAvg_[i]));
            p.setPen(QPen(QColor("#89b4fa"), 2));
            p.drawPolyline(pts);
        }

        // Legend (bottom-right).
        p.setFont(QFont("monospace", 8));
        int lx = r.right() - 180;
        int ly = r.bottom() + 4;
        auto legendItem = [&](const QColor &c, const QString &label) {
            p.setPen(c);
            p.drawLine(lx, ly + 6, lx + 16, ly + 6);
            p.drawText(lx + 20, ly, label);
            lx += 60;
        };
        legendItem(QColor("#89b4fa"), "Avg");
        legendItem(QColor("#f38ba8"), "Max");
        legendItem(QColor("#a6e3a1"), "Min");
    }

private:
    QVector<double> bufAvg_, bufMin_, bufMax_;
    double yLo_ = 0, yHi_ = 1000;
    bool ySnapped_ = false;
};

// ── Compact jitter sparkline (QPainter) ─────────────────────────────────────
class RtTestJitterSpark : public QWidget {
public:
    explicit RtTestJitterSpark(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedHeight(48);
    }

    void pushSample(double jitterUs) {
        samples_.append(jitterUs);
        if (samples_.size() > kMax) samples_.removeFirst();
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRect r = rect().adjusted(1, 1, -1, -1);
        p.fillRect(rect(), QColor("#181825"));
        p.setPen(QPen(QColor("#45475a"), 1));
        p.drawRect(rect().adjusted(0, 0, -1, -1));
        if (samples_.size() < 2) return;

        double maxVal = 0;
        for (double v : samples_) maxVal = qMax(maxVal, v);
        maxVal = qMax(maxVal, 50.0);
        const double scale = r.height() / maxVal;

        QPolygonF poly;
        poly << QPointF(r.left(), r.bottom());
        for (int i = 0; i < samples_.size(); ++i) {
            double x = r.left() + (i * r.width()) / (kMax - 1);
            double y = r.bottom() - samples_[i] * scale;
            poly << QPointF(x, y);
        }
        poly << QPointF(r.left() + ((samples_.size() - 1) * r.width()) / (kMax - 1), r.bottom());
        QColor fill("#89b4fa");
        fill.setAlpha(50);
        p.setBrush(fill);
        p.setPen(QPen(QColor("#89b4fa"), 1));
        p.drawPolygon(poly);

        p.setPen(QColor("#cdd6f4"));
        p.setFont(QFont("monospace", 8));
        p.drawText(r.adjusted(4, 2, 0, 0), Qt::AlignTop,
                   QString("Jitter: %1 µs").arg(samples_.last(), 0, 'f', 1));
    }

private:
    static constexpr int kMax = 200;
    QVector<double> samples_;
};

// ── Build the RT Test workspace page ────────────────────────────────────────
QWidget *MainWindow::buildRtTestPage()
{
    auto *page = new QWidget;
    page->setObjectName("rtTestPage");
    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(4);

    // ── Control Bar ────────────────────────────────────────────────────────
    auto *ctrl = new QHBoxLayout;
    ctrl->setSpacing(6);
    rtTestStartButton_ = new QPushButton(uiText("Start", "开始"));
    rtTestStartButton_->setMinimumWidth(56);
    rtTestStopButton_ = new QPushButton(uiText("Stop", "停止"));
    rtTestStopButton_->setMinimumWidth(56);
    rtTestStopButton_->setEnabled(false);
    ctrl->addWidget(rtTestStartButton_);
    ctrl->addWidget(rtTestStopButton_);
    ctrl->addSpacing(8);
    ctrl->addWidget(new QLabel(uiText("Cycle:", "周期:")));
    rtTestCycleCombo_ = new QComboBox;
    rtTestCycleCombo_->addItem("125 µs (8 kHz)", 125);
    rtTestCycleCombo_->addItem("250 µs (4 kHz)", 250);
    rtTestCycleCombo_->addItem("500 µs (2 kHz)", 500);
    rtTestCycleCombo_->addItem("1000 µs (1 kHz)", 1000);
    rtTestCycleCombo_->addItem("2000 µs (500 Hz)", 2000);
    rtTestCycleCombo_->addItem("5000 µs (200 Hz)", 5000);
    rtTestCycleCombo_->addItem("10000 µs (100 Hz)", 10000);
    rtTestCycleCombo_->addItem(uiText("Custom...", "自定义..."), -1);
    rtTestCycleCombo_->setCurrentIndex(3);
    ctrl->addWidget(rtTestCycleCombo_);

    // Custom cycle time input — editable field for non-standard frequencies.
    rtTestCustomCycle_ = new QLineEdit;
    rtTestCustomCycle_->setObjectName("rtTestCustomCycle");
    rtTestCustomCycle_->setPlaceholderText(uiText("µs", "µs"));
    rtTestCustomCycle_->setMaximumWidth(80);
    rtTestCustomCycle_->setVisible(false);
    ctrl->addWidget(rtTestCustomCycle_);

    // Frequency display — shows calculated Hz from the cycle time.
    rtTestFreqLabel_ = new QLabel("1 kHz");
    rtTestFreqLabel_->setStyleSheet("color: #89b4fa; font-weight: bold; min-width: 60px;");
    ctrl->addWidget(rtTestFreqLabel_);

    // Wire combo to show/hide custom input and update freq label.
    auto updateFreqLabel = [this]() {
        const int usec = rtTestCustomCycle_->isVisible()
            ? rtTestCustomCycle_->text().toInt()
            : rtTestCycleCombo_->currentData().toInt();
        if (usec > 0) {
            const double hz = 1000000.0 / usec;
            rtTestFreqLabel_->setText(hz >= 1000
                ? QString::number(hz / 1000.0, 'f', 1) + " kHz"
                : QString::number(hz, 'f', 0) + " Hz");
        }
    };
    connect(rtTestCycleCombo_, &QComboBox::currentIndexChanged, this,
        [this, updateFreqLabel](int idx) {
            const bool custom = rtTestCycleCombo_->itemData(idx).toInt() < 0;
            rtTestCustomCycle_->setVisible(custom);
            if (custom) rtTestCustomCycle_->setFocus();
            updateFreqLabel();
        });
    connect(rtTestCustomCycle_, &QLineEdit::textChanged, this, updateFreqLabel);
    ctrl->addStretch();
    rtTestStatusLabel_ = new QLabel(uiText("Idle", "空闲"));
    rtTestStatusLabel_->setStyleSheet("font-weight: bold;");
    ctrl->addWidget(rtTestStatusLabel_);
    root->addLayout(ctrl);

    // ── Main area: Left stats + Right chart ────────────────────────────────
    auto *splitter = new QSplitter(Qt::Horizontal);

    // ── Left: compact stats ────────────────────────────────────────────────
    auto *left = new QWidget;
    auto *leftLay = new QVBoxLayout(left);
    leftLay->setContentsMargins(0, 0, 0, 0);
    leftLay->setSpacing(6);

    // Timing grid (2x2, tight).
    auto *grid = new QGridLayout;
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(2);
    auto stat = [&](const QString &label, int r, int c) -> QLabel * {
        auto *t = new QLabel(label);
        t->setObjectName("rtTestMetricTitle");
        t->setAlignment(Qt::AlignCenter);
        auto *v = new QLabel("--");
        v->setObjectName("rtTestMetricValue");
        v->setAlignment(Qt::AlignCenter);
        v->setStyleSheet("font-size: 18px; font-weight: bold;");
        grid->addWidget(t, r * 2, c);
        grid->addWidget(v, r * 2 + 1, c);
        return v;
    };
    rtTestMinLabel_      = stat(uiText("Min µs", "最小"), 0, 0);
    rtTestMaxLabel_      = stat(uiText("Max µs", "最大"), 0, 1);
    rtTestAvgLabel_      = stat(uiText("Avg µs", "平均"), 1, 0);
    rtTestJitterLabel_   = stat(uiText("Jitter µs", "抖动"), 1, 1);
    leftLay->addLayout(grid);

    // Counters (compact row).
    auto *cnt = new QHBoxLayout;
    cnt->setSpacing(6);
    auto ctr = [&](const QString &label) -> QLabel * {
        auto *col = new QVBoxLayout;
        col->setSpacing(0);
        auto *t = new QLabel(label);
        t->setObjectName("rtTestMetricTitle");
        t->setAlignment(Qt::AlignCenter);
        auto *v = new QLabel("--");
        v->setObjectName("rtTestMetricValue");
        v->setAlignment(Qt::AlignCenter);
        v->setStyleSheet("font-size: 14px;");
        col->addWidget(t);
        col->addWidget(v);
        cnt->addLayout(col);
        return v;
    };
    rtTestCyclesLabel_   = ctr(uiText("Cycles", "周期数"));
    rtTestErrorsLabel_   = ctr(uiText("Errors", "错误"));
    rtTestLossLabel_     = ctr(uiText("Loss %", "丢包%"));
    rtTestDurationLabel_ = ctr(uiText("Time", "时间"));
    leftLay->addLayout(cnt);

    // Health bar + jitter spark.
    rtTestHealthLabel_ = new QLabel;
    rtTestHealthLabel_->setFixedHeight(4);
    rtTestHealthLabel_->setStyleSheet("background: #45475a; border-radius: 2px;");
    leftLay->addWidget(rtTestHealthLabel_);

    rtTestJitterSpark_ = new RtTestJitterSpark;
    leftLay->addWidget(rtTestJitterSpark_);

    leftLay->addStretch();

    splitter->addWidget(left);

    // ── Right: chart ───────────────────────────────────────────────────────
    rtTestChart_ = new RtTestLatencyChart;
    splitter->addWidget(rtTestChart_);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);

    root->addWidget(splitter, 1);

    // ── Timeline log ───────────────────────────────────────────────────────
    rtTestTimelineText_ = new QPlainTextEdit;
    rtTestTimelineText_->setReadOnly(true);
    rtTestTimelineText_->setMaximumBlockCount(1000);
    rtTestTimelineText_->setMaximumHeight(80);
    rtTestTimelineText_->setPlaceholderText(
        uiText("Cycle log...", "周期日志..."));
    root->addWidget(rtTestTimelineText_);

    // ── Wire signals ───────────────────────────────────────────────────────
    connect(rtTestStartButton_, &QPushButton::clicked, this, [this] {
        const int usec = rtTestCustomCycle_->isVisible()
            ? rtTestCustomCycle_->text().toInt()
            : rtTestCycleCombo_->currentData().toInt();
        if (usec > 0) client_.rtTestStart(usec);
    });
    connect(rtTestStopButton_, &QPushButton::clicked, this, [this] {
        client_.rtTestStop();
    });

    return page;
}

// ── Handle RT test telemetry from daemon ────────────────────────────────────
void MainWindow::updateRtTestTelemetry(const QJsonObject &telemetry)
{
    const bool running = telemetry.value("running").toBool();
    const qint64 cycles = telemetry.value("cycles").toVariant().toLongLong();
    const qint64 errors = telemetry.value("errors").toVariant().toLongLong();
    const double lossRate = telemetry.value("lossRate").toDouble();
    const double minUs = telemetry.value("minUsec").toDouble();
    const double maxUs = telemetry.value("maxUsec").toDouble();
    const double avgUs = telemetry.value("avgUsec").toDouble();
    const double jitterUs = telemetry.value("jitterUsec").toDouble();
    const QJsonArray recent = telemetry.value("recent").toArray();

    // Buttons.
    rtTestStartButton_->setEnabled(!running && client_.isConnected());
    rtTestStopButton_->setEnabled(running);
    rtTestCycleCombo_->setEnabled(!running);
    rtTestCustomCycle_->setEnabled(!running);

    // Status.
    if (running) {
        rtTestStatusLabel_->setText(uiText("Running", "运行中"));
        rtTestStatusLabel_->setStyleSheet("color: #22c55e; font-weight: bold;");
    } else if (cycles > 0) {
        rtTestStatusLabel_->setText(uiText("Stopped", "已停止"));
        rtTestStatusLabel_->setStyleSheet("color: #f59e0b; font-weight: bold;");
    } else {
        rtTestStatusLabel_->setText(uiText("Idle", "空闲"));
        rtTestStatusLabel_->setStyleSheet("font-weight: bold;");
    }

    // Metrics.
    if (cycles > 0) {
        rtTestMinLabel_->setText(QString::number(minUs, 'f', 1));
        rtTestMaxLabel_->setText(QString::number(maxUs, 'f', 1));
        rtTestAvgLabel_->setText(QString::number(avgUs, 'f', 1));
        rtTestJitterLabel_->setText(QString::number(jitterUs, 'f', 1));
    }
    rtTestCyclesLabel_->setText(QString::number(cycles));
    rtTestErrorsLabel_->setText(QString::number(errors));
    rtTestLossLabel_->setText(QString::number(lossRate, 'f', 3));
    if (cycles > 0 && avgUs > 0) {
        rtTestDurationLabel_->setText(
            formatDuration(static_cast<double>(cycles) * avgUs / 1e6));
    }

    // Health bar.
    QString hc = "#45475a";
    if (cycles > 0) {
        hc = (lossRate > 0.1 || jitterUs > 500) ? "#ef4444"
             : (jitterUs > 100) ? "#f59e0b" : "#22c55e";
    }
    rtTestHealthLabel_->setStyleSheet(
        QString("background: %1; border-radius: 2px;").arg(hc));

    // Jitter spark.
    if (rtTestJitterSpark_) rtTestJitterSpark_->pushSample(jitterUs);

    // Chart — send all points directly (daemon already downsamples).
    if (rtTestChart_ && !recent.isEmpty() && cycles > 0) {
        QJsonArray avgA, minA, maxA;
        const int chunk = qMax(1, recent.size() / 3000);
        for (int i = 0; i < recent.size(); i += chunk) {
            double cMin = 1e18, cMax = 0, cSum = 0;
            int n = 0;
            for (int j = i; j < qMin(i + chunk, recent.size()); ++j) {
                double v = recent[j].toDouble();
                cMin = qMin(cMin, v);
                cMax = qMax(cMax, v);
                cSum += v;
                ++n;
            }
            avgA.append(cSum / n);
            minA.append(cMin);
            maxA.append(cMax);
        }
        rtTestChart_->appendData(avgA, minA, maxA);
    }

    // Periodic log entry.
    if (running && cycles > 0 && (cycles % 5000 == 0)) {
        rtTestTimelineText_->appendPlainText(
            QString("#%1  avg=%2µs  jit=%3µs  loss=%4%")
                .arg(cycles).arg(avgUs, 0, 'f', 1)
                .arg(jitterUs, 0, 'f', 1).arg(lossRate, 0, 'f', 3));
    }
}

void MainWindow::updateRtTestActionAvailability()
{
    if (!rtTestStartButton_) return;
    rtTestStartButton_->setEnabled(client_.isConnected() && !rtTestRunning_);
    rtTestStopButton_->setEnabled(rtTestRunning_);
}

QString MainWindow::formatDuration(double seconds) const
{
    if (seconds < 60) return QString::number(seconds, 'f', 1) + "s";
    if (seconds < 3600)
        return QString("%1m %2s").arg(int(seconds) / 60).arg(int(seconds) % 60);
    return QString("%1h %2m").arg(int(seconds) / 3600).arg((int(seconds) % 3600) / 60);
}
