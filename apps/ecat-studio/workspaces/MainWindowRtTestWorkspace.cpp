// Real-time stability test workspace: controls, live statistics, and cycle timeline.
#include "MainWindow.h"

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QCheckBox>
#include <QComboBox>
#include <QFontMetrics>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QAbstractAxis>

// ── Rolling latency graph widget ────────────────────────────────────────────
// Custom chart that renders cycle-time history as a filled area plot with
// min/max envelope and avg line, using QtCharts for pixel-accurate rendering.
class RtTestLatencyChart : public QChartView {
public:
    explicit RtTestLatencyChart(QWidget *parent = nullptr)
        : QChartView(parent)
    {
        auto *chart = new QChart;
        chart->setTitle("Cycle Latency (µs)");
        chart->legend()->hide();
        chart->setMargins(QMargins(4, 4, 4, 4));
        chart->setBackgroundBrush(QColor("#1e1e2e"));
        chart->setTitleBrush(QColor("#cdd6f4"));
        chart->setPlotAreaBackgroundBrush(QColor("#181825"));
        chart->setPlotAreaBackgroundVisible(true);

        // Avg series (main line).
        avgSeries_ = new QLineSeries;
        avgSeries_->setPen(QPen(QColor("#89b4fa"), 2));
        avgSeries_->setName("Avg");

        // Max series (upper envelope).
        maxSeries_ = new QLineSeries;
        maxSeries_->setPen(QPen(QColor("#f38ba8"), 1, Qt::DashLine));
        maxSeries_->setName("Max");

        // Min series (lower envelope).
        minSeries_ = new QLineSeries;
        minSeries_->setPen(QPen(QColor("#a6e3a1"), 1, Qt::DashLine));
        minSeries_->setName("Min");

        // Jitter band (area between min and avg).
        jitterUpper_ = new QLineSeries;
        jitterLower_ = new QLineSeries;
        jitterArea_ = new QAreaSeries(jitterUpper_, jitterLower_);
        jitterArea_->setBrush(QColor("#f9e2af"));
        jitterArea_->setPen(Qt::NoPen);
        jitterArea_->setName("Jitter");

        chart->addSeries(avgSeries_);
        chart->addSeries(maxSeries_);
        chart->addSeries(minSeries_);
        chart->addSeries(jitterArea_);

        setChart(chart);
        setRenderHint(QPainter::Antialiasing);
        setStyleSheet("background: transparent;");
    }

    // Push new data points and refresh the chart axes.
    void updateData(const QJsonArray &avg, const QJsonArray &minArr,
                    const QJsonArray &maxArr)
    {
        auto seriesReplace = [](QLineSeries *s, const QJsonArray &arr) {
            s->clear();
            for (int i = 0; i < arr.size(); ++i) {
                s->append(i, arr[i].toDouble());
            }
        };
        seriesReplace(avgSeries_, avg);
        seriesReplace(maxSeries_, maxArr);
        seriesReplace(minSeries_, minArr);

        // Rebuild jitter area between min and avg.
        jitterUpper_->clear();
        jitterLower_->clear();
        for (int i = 0; i < avg.size(); ++i) {
            jitterUpper_->append(i, avg[i].toDouble());
            jitterLower_->append(i, i < minArr.size() ? minArr[i].toDouble() : avg[i].toDouble());
        }

        // Auto-scale axes with 10% padding on Y.
        auto axes = chart()->axes();
        QAbstractAxis *yAxis = nullptr;
        QAbstractAxis *xAxis = nullptr;
        for (auto *axis : axes) {
            if (axis->orientation() == Qt::Vertical) yAxis = axis;
            else xAxis = axis;
        }
        if (!avg.isEmpty() && yAxis) {
            double lo = 1e18, hi = 0;
            for (int i = 0; i < avg.size(); ++i) {
                double v = avg[i].toDouble();
                double mn = i < minArr.size() ? minArr[i].toDouble() : v;
                double mx = i < maxArr.size() ? maxArr[i].toDouble() : v;
                lo = qMin(lo, mn);
                hi = qMax(hi, mx);
            }
            double pad = qMax((hi - lo) * 0.1, 10.0);
            yAxis->setRange(lo - pad, hi + pad);
        }
        if (xAxis) {
            xAxis->setRange(0, qMax(avg.size() - 1, 1));
        }
    }

private:
    QLineSeries *avgSeries_ = nullptr;
    QLineSeries *maxSeries_ = nullptr;
    QLineSeries *minSeries_ = nullptr;
    QLineSeries *jitterUpper_ = nullptr;
    QLineSeries *jitterLower_ = nullptr;
    QAreaSeries *jitterArea_ = nullptr;
};

// ── Jitter sparkline widget (lightweight QPainter, no QtCharts dependency) ──
// Draws a real-time scrolling waveform of jitter values.
class RtTestJitterSpark : public QWidget {
public:
    explicit RtTestJitterSpark(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(60);
        setMaximumHeight(80);
    }

    void pushSample(double jitterUs) {
        samples_.append(jitterUs);
        if (samples_.size() > kMaxSamples) {
            samples_.removeFirst();
        }
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRect r = rect().adjusted(2, 2, -2, -2);
        // Background.
        p.fillRect(rect(), QColor("#181825"));
        // Border.
        p.setPen(QPen(QColor("#45475a"), 1));
        p.drawRect(rect().adjusted(0, 0, -1, -1));

        if (samples_.size() < 2) return;

        // Scale: map jitter range to pixel height.
        double maxVal = 0;
        for (double v : samples_) maxVal = qMax(maxVal, v);
        maxVal = qMax(maxVal, 50.0); // Minimum scale
        const double scale = r.height() / maxVal;

        // Draw filled area.
        QPolygonF poly;
        poly << QPointF(r.left(), r.bottom());
        for (int i = 0; i < samples_.size(); ++i) {
            double x = r.left() + (i * r.width()) / (kMaxSamples - 1);
            double y = r.bottom() - samples_[i] * scale;
            poly << QPointF(x, y);
        }
        poly << QPointF(r.left() + ((samples_.size() - 1) * r.width()) / (kMaxSamples - 1), r.bottom());
        QColor sparkFill("#89b4fa"); sparkFill.setAlpha(40); p.setBrush(sparkFill);
        p.setPen(QPen(QColor("#89b4fa"), 1));
        p.drawPolygon(poly);

        // Current value label.
        p.setPen(QColor("#cdd6f4"));
        p.setFont(QFont("monospace", 9));
        p.drawText(r.adjusted(4, 2, 0, 0), Qt::AlignTop,
                   QString("Jitter: %1 µs").arg(samples_.last(), 0, 'f', 1));
    }

private:
    static constexpr int kMaxSamples = 200;
    QVector<double> samples_;
};

// ── Build the RT Test workspace page ────────────────────────────────────────
QWidget *MainWindow::buildRtTestPage()
{
    auto *page = new QWidget;
    page->setObjectName("rtTestPage");
    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(6, 6, 6, 6);
    root->setSpacing(4);

    // ── Control Bar ────────────────────────────────────────────────────────
    auto *controlBar = new QHBoxLayout;
    controlBar->setSpacing(6);

    rtTestStartButton_ = new QPushButton(uiText("Start", "开始"));
    rtTestStartButton_->setObjectName("rtTestStart");
    rtTestStartButton_->setMinimumWidth(60);
    rtTestStopButton_ = new QPushButton(uiText("Stop", "停止"));
    rtTestStopButton_->setObjectName("rtTestStop");
    rtTestStopButton_->setMinimumWidth(60);
    rtTestStopButton_->setEnabled(false);

    controlBar->addWidget(rtTestStartButton_);
    controlBar->addWidget(rtTestStopButton_);

    controlBar->addSpacing(8);
    controlBar->addWidget(new QLabel(uiText("Cycle:", "周期:")));
    rtTestCycleCombo_ = new QComboBox;
    rtTestCycleCombo_->setObjectName("rtTestCycleCombo");
    rtTestCycleCombo_->addItem("125 µs (8 kHz)", 125);
    rtTestCycleCombo_->addItem("250 µs (4 kHz)", 250);
    rtTestCycleCombo_->addItem("500 µs (2 kHz)", 500);
    rtTestCycleCombo_->addItem("1000 µs (1 kHz)", 1000);
    rtTestCycleCombo_->addItem("2000 µs (500 Hz)", 2000);
    rtTestCycleCombo_->addItem("5000 µs (200 Hz)", 5000);
    rtTestCycleCombo_->addItem("10000 µs (100 Hz)", 10000);
    rtTestCycleCombo_->setCurrentIndex(3); // Default 1 kHz
    controlBar->addWidget(rtTestCycleCombo_);

    controlBar->addStretch();

    rtTestStatusLabel_ = new QLabel(uiText("Idle", "空闲"));
    rtTestStatusLabel_->setObjectName("rtTestStatus");
    rtTestStatusLabel_->setStyleSheet("font-weight: bold;");
    controlBar->addWidget(rtTestStatusLabel_);

    root->addLayout(controlBar);

    // ── Main splitter: Stats (left) + Chart (right) ───────────────────────
    auto *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->setObjectName("rtTestMainSplitter");

    // ── Left panel: Stats ──────────────────────────────────────────────────
    auto *statsWidget = new QWidget;
    auto *statsLayout = new QVBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(4);

    // Core timing (compact 2x2 grid).
    auto *timingGrid = new QGridLayout;
    timingGrid->setSpacing(4);
    auto makeStat = [&](const QString &label, int row, int col) -> QLabel * {
        auto *title = new QLabel(label);
        title->setObjectName("rtTestMetricTitle");
        auto *value = new QLabel("--");
        value->setObjectName("rtTestMetricValue");
        value->setAlignment(Qt::AlignCenter);
        timingGrid->addWidget(title, row * 2, col);
        timingGrid->addWidget(value, row * 2 + 1, col);
        return value;
    };
    rtTestMinLabel_  = makeStat(uiText("Min µs", "最小 µs"), 0, 0);
    rtTestMaxLabel_  = makeStat(uiText("Max µs", "最大 µs"), 0, 1);
    rtTestAvgLabel_  = makeStat(uiText("Avg µs", "平均 µs"), 1, 0);
    rtTestJitterLabel_ = makeStat(uiText("Jitter µs", "抖动 µs"), 1, 1);
    statsLayout->addLayout(timingGrid);

    // Counters (horizontal row).
    auto *countLayout = new QHBoxLayout;
    countLayout->setSpacing(8);
    auto makeCount = [&](const QString &label) -> QLabel * {
        auto *col = new QVBoxLayout;
        col->setSpacing(1);
        auto *t = new QLabel(label);
        t->setObjectName("rtTestMetricTitle");
        t->setAlignment(Qt::AlignCenter);
        auto *v = new QLabel("--");
        v->setObjectName("rtTestMetricValue");
        v->setAlignment(Qt::AlignCenter);
        col->addWidget(t);
        col->addWidget(v);
        countLayout->addLayout(col);
        return v;
    };
    rtTestCyclesLabel_ = makeCount(uiText("Cycles", "周期数"));
    rtTestErrorsLabel_ = makeCount(uiText("Errors", "错误"));
    rtTestLossLabel_   = makeCount(uiText("Loss %", "丢包%"));
    rtTestDurationLabel_ = makeCount(uiText("Time", "时间"));
    statsLayout->addLayout(countLayout);

    // Health bar.
    rtTestHealthLabel_ = new QLabel;
    rtTestHealthLabel_->setObjectName("rtTestHealth");
    rtTestHealthLabel_->setFixedHeight(4);
    statsLayout->addWidget(rtTestHealthLabel_);

    // Jitter sparkline.
    rtTestJitterSpark_ = new RtTestJitterSpark;
    statsLayout->addWidget(rtTestJitterSpark_);

    mainSplitter->addWidget(statsWidget);

    // ── Right panel: Latency chart ─────────────────────────────────────────
    rtTestChart_ = new RtTestLatencyChart;
    rtTestChart_->setMinimumWidth(400);
    mainSplitter->addWidget(rtTestChart_);

    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 3);

    root->addWidget(mainSplitter, 1);

    // ── Timeline log (collapsible) ─────────────────────────────────────────
    rtTestTimelineText_ = new QPlainTextEdit;
    rtTestTimelineText_->setObjectName("rtTestTimeline");
    rtTestTimelineText_->setReadOnly(true);
    rtTestTimelineText_->setMaximumBlockCount(2000);
    rtTestTimelineText_->setMaximumHeight(120);
    rtTestTimelineText_->setPlaceholderText(
        uiText("Cycle log will appear here...",
               "周期日志将在此显示..."));
    root->addWidget(rtTestTimelineText_);

    // ── Wire signals ───────────────────────────────────────────────────────
    connect(rtTestStartButton_, &QPushButton::clicked, this, [this] {
        const int cycleUsec = rtTestCycleCombo_->currentData().toInt();
        client_.rtTestStart(cycleUsec);
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
    const QString status = telemetry.value("status").toString();
    const qint64 cycles = telemetry.value("cycles").toVariant().toLongLong();
    const qint64 errors = telemetry.value("errors").toVariant().toLongLong();
    const double lossRate = telemetry.value("lossRate").toDouble();
    const double minUs = telemetry.value("minUsec").toDouble();
    const double maxUs = telemetry.value("maxUsec").toDouble();
    const double avgUs = telemetry.value("avgUsec").toDouble();
    const double jitterUs = telemetry.value("jitterUsec").toDouble();
    const QJsonArray recent = telemetry.value("recent").toArray();

    // Button states.
    rtTestStartButton_->setEnabled(!running && client_.isConnected());
    rtTestStopButton_->setEnabled(running);
    rtTestCycleCombo_->setEnabled(!running);

    // Status with color.
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

    // Core timing metrics.
    if (cycles > 0) {
        rtTestMinLabel_->setText(QString::number(minUs, 'f', 1));
        rtTestMaxLabel_->setText(QString::number(maxUs, 'f', 1));
        rtTestAvgLabel_->setText(QString::number(avgUs, 'f', 1));
        rtTestJitterLabel_->setText(QString::number(jitterUs, 'f', 1));
    }

    // Counters.
    rtTestCyclesLabel_->setText(QString::number(cycles));
    rtTestErrorsLabel_->setText(QString::number(errors));
    rtTestLossLabel_->setText(QString::number(lossRate, 'f', 3));

    // Duration.
    if (cycles > 0 && avgUs > 0) {
        const double seconds = static_cast<double>(cycles) * avgUs / 1000000.0;
        rtTestDurationLabel_->setText(formatDuration(seconds));
    }

    // Health bar color.
    QString color;
    if (cycles == 0) {
        color = "#45475a";
    } else if (lossRate > 0.1 || jitterUs > 500) {
        color = "#ef4444";
    } else if (jitterUs > 100) {
        color = "#f59e0b";
    } else {
        color = "#22c55e";
    }
    rtTestHealthLabel_->setStyleSheet(
        QString("background: %1; border-radius: 2px;").arg(color));

    // Update jitter sparkline.
    if (rtTestJitterSpark_) {
        rtTestJitterSpark_->pushSample(jitterUs);
    }

    // Update latency chart with recent sample arrays.
    if (rtTestChart_ && !recent.isEmpty()) {
        // Build separate avg/min/max arrays from the recent samples.
        // The daemon sends per-sample values; we derive min/max from chunks.
        QJsonArray avgArr, minArr, maxArr;
        const int chunk = qMax(1, recent.size() / 100);
        for (int i = 0; i < recent.size(); i += chunk) {
            double chunkMin = 1e18, chunkMax = 0, chunkSum = 0;
            int count = 0;
            for (int j = i; j < qMin(i + chunk, recent.size()); ++j) {
                double v = recent[j].toDouble();
                chunkMin = qMin(chunkMin, v);
                chunkMax = qMax(chunkMax, v);
                chunkSum += v;
                ++count;
            }
            avgArr.append(chunkSum / count);
            minArr.append(chunkMin);
            maxArr.append(chunkMax);
        }
        rtTestChart_->updateData(avgArr, minArr, maxArr);
    }

    // Append periodic summary to log.
    if (running && cycles > 0 && (cycles % 5000 == 0)) {
        rtTestTimelineText_->appendPlainText(
            QString("#%1  avg=%2µs  jit=%3µs  loss=%4%")
                .arg(cycles).arg(avgUs, 0, 'f', 1)
                .arg(jitterUs, 0, 'f', 1).arg(lossRate, 0, 'f', 3));
    }
}

// ── Update RT test action availability ─────────────────────────────────────
void MainWindow::updateRtTestActionAvailability()
{
    if (!rtTestStartButton_) return;
    const bool connected = client_.isConnected();
    rtTestStartButton_->setEnabled(connected && !rtTestRunning_);
    rtTestStopButton_->setEnabled(rtTestRunning_);
}

// ── Format seconds to human-readable duration ──────────────────────────────
QString MainWindow::formatDuration(double seconds) const
{
    if (seconds < 60) {
        return QString::number(seconds, 'f', 1) + "s";
    }
    if (seconds < 3600) {
        return QString("%1m %2s")
            .arg(static_cast<int>(seconds) / 60)
            .arg(static_cast<int>(seconds) % 60);
    }
    return QString("%1h %2m")
        .arg(static_cast<int>(seconds) / 3600)
        .arg((static_cast<int>(seconds) % 3600) / 60);
}
