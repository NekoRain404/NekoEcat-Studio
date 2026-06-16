// Real-time stability test workspace: controls, live statistics, and cycle timeline.
#include "MainWindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>

// ── Build the RT Test workspace page ────────────────────────────────────────
// Creates the full RT test panel with controls, statistics, and a cycle timeline.
QWidget *MainWindow::buildRtTestPage()
{
    auto *page = new QWidget;
    page->setObjectName("rtTestPage");
    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(8);

    // ── Control Bar ────────────────────────────────────────────────────────
    auto *controlGroup = new QGroupBox(uiText("RT Stability Test", "实时稳定性测试"));
    controlGroup->setObjectName("rtTestControlGroup");
    auto *controlLayout = new QHBoxLayout(controlGroup);
    controlLayout->setContentsMargins(8, 6, 8, 6);
    controlLayout->setSpacing(8);

    rtTestStartButton_ = new QPushButton(uiText("Start Test", "开始测试"));
    rtTestStartButton_->setObjectName("rtTestStart");
    rtTestStopButton_ = new QPushButton(uiText("Stop Test", "停止测试"));
    rtTestStopButton_->setObjectName("rtTestStop");
    rtTestStopButton_->setEnabled(false);

    controlLayout->addWidget(rtTestStartButton_);
    controlLayout->addWidget(rtTestStopButton_);

    controlLayout->addSpacing(12);
    controlLayout->addWidget(new QLabel(uiText("Cycle:", "周期:")));
    rtTestCycleCombo_ = new QComboBox;
    rtTestCycleCombo_->setObjectName("rtTestCycleCombo");
    rtTestCycleCombo_->addItem("500 µs (2 kHz)", 500);
    rtTestCycleCombo_->addItem("1000 µs (1 kHz)", 1000);
    rtTestCycleCombo_->addItem("2000 µs (500 Hz)", 2000);
    rtTestCycleCombo_->addItem("5000 µs (200 Hz)", 5000);
    rtTestCycleCombo_->addItem("10000 µs (100 Hz)", 10000);
    rtTestCycleCombo_->setCurrentIndex(1); // Default 1 kHz
    controlLayout->addWidget(rtTestCycleCombo_);

    controlLayout->addStretch();
    rtTestStatusLabel_ = new QLabel(uiText("Status: Idle", "状态: 空闲"));
    rtTestStatusLabel_->setObjectName("rtTestStatus");
    controlLayout->addWidget(rtTestStatusLabel_);

    root->addWidget(controlGroup);

    // ── Splitter: Stats + Timeline ─────────────────────────────────────────
    auto *splitter = new QSplitter(Qt::Vertical);
    splitter->setObjectName("rtTestSplitter");

    // ── Statistics Panel ───────────────────────────────────────────────────
    auto *statsGroup = new QGroupBox(uiText("Live Statistics", "实时统计"));
    statsGroup->setObjectName("rtTestStatsGroup");
    auto *statsLayout = new QVBoxLayout(statsGroup);
    statsLayout->setContentsMargins(8, 6, 8, 6);
    statsLayout->setSpacing(4);

    // Row 1: Core timing metrics
    auto *timingRow = new QHBoxLayout;
    timingRow->setSpacing(16);
    auto addMetric = [&](const QString &label) -> QLabel * {
        auto *col = new QVBoxLayout;
        col->setSpacing(2);
        auto *title = new QLabel(label);
        title->setObjectName("rtTestMetricTitle");
        auto *value = new QLabel("--");
        value->setObjectName("rtTestMetricValue");
        col->addWidget(title);
        col->addWidget(value);
        timingRow->addLayout(col);
        return value;
    };
    rtTestMinLabel_ = addMetric(uiText("Min (µs)", "最小 (µs)"));
    rtTestMaxLabel_ = addMetric(uiText("Max (µs)", "最大 (µs)"));
    rtTestAvgLabel_ = addMetric(uiText("Avg (µs)", "平均 (µs)"));
    rtTestJitterLabel_ = addMetric(uiText("Jitter (µs)", "抖动 (µs)"));
    timingRow->addStretch();
    statsLayout->addLayout(timingRow);

    // Row 2: Counters
    auto *countRow = new QHBoxLayout;
    countRow->setSpacing(16);
    auto addCounter = [&](const QString &label) -> QLabel * {
        auto *col = new QVBoxLayout;
        col->setSpacing(2);
        auto *title = new QLabel(label);
        title->setObjectName("rtTestMetricTitle");
        auto *value = new QLabel("--");
        value->setObjectName("rtTestMetricValue");
        col->addWidget(title);
        col->addWidget(value);
        countRow->addLayout(col);
        return value;
    };
    rtTestCyclesLabel_ = addCounter(uiText("Cycles", "周期数"));
    rtTestErrorsLabel_ = addCounter(uiText("Errors", "错误数"));
    rtTestLossLabel_ = addCounter(uiText("Loss Rate", "丢包率"));
    rtTestDurationLabel_ = addCounter(uiText("Duration", "持续时间"));
    countRow->addStretch();
    statsLayout->addLayout(countRow);

    // Health indicator — a colored bar showing overall test health.
    rtTestHealthLabel_ = new QLabel;
    rtTestHealthLabel_->setObjectName("rtTestHealth");
    rtTestHealthLabel_->setFixedHeight(6);
    rtTestHealthLabel_->setStyleSheet("background: #334155; border-radius: 3px;");
    statsLayout->addWidget(rtTestHealthLabel_);

    splitter->addWidget(statsGroup);

    // ── Timeline Panel ─────────────────────────────────────────────────────
    auto *timelineGroup = new QGroupBox(uiText("Cycle Timeline", "周期时间线"));
    timelineGroup->setObjectName("rtTestTimelineGroup");
    auto *timelineLayout = new QVBoxLayout(timelineGroup);
    timelineLayout->setContentsMargins(8, 6, 8, 6);

    rtTestTimelineText_ = new QPlainTextEdit;
    rtTestTimelineText_->setObjectName("rtTestTimeline");
    rtTestTimelineText_->setReadOnly(true);
    rtTestTimelineText_->setMaximumBlockCount(5000);
    rtTestTimelineText_->setLineWrapMode(QPlainTextEdit::NoWrap);
    rtTestTimelineText_->setPlaceholderText(
        uiText("Start the RT test to see cycle timing data...",
               "开始 RT 测试以查看周期时序数据..."));
    timelineLayout->addWidget(rtTestTimelineText_);

    splitter->addWidget(timelineGroup);
    splitter->setSizes({200, 400});

    root->addWidget(splitter, 1);

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
// Updates all statistics labels, health bar, and appends data to the timeline.
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

    // Update button states.
    rtTestStartButton_->setEnabled(!running && client_.isConnected());
    rtTestStopButton_->setEnabled(running);
    rtTestCycleCombo_->setEnabled(!running);

    // Status label.
    rtTestStatusLabel_->setText(
        QString("%1: %2").arg(uiText("Status", "状态"), status));

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
    rtTestLossLabel_->setText(QString::number(lossRate, 'f', 3) + "%");

    // Duration estimate from cycle count and average cycle time.
    if (cycles > 0 && avgUs > 0) {
        const double seconds = static_cast<double>(cycles) * avgUs / 1000000.0;
        rtTestDurationLabel_->setText(formatDuration(seconds));
    }

    // Health bar color: green if jitter < 100µs, yellow if < 500µs, red otherwise.
    // Also turns red if loss rate > 0.1%.
    if (lossRate > 0.1 || jitterUs > 500) {
        rtTestHealthLabel_->setStyleSheet(
            "background: #ef4444; border-radius: 3px;");
    } else if (jitterUs > 100) {
        rtTestHealthLabel_->setStyleSheet(
            "background: #f59e0b; border-radius: 3px;");
    } else if (cycles > 0) {
        rtTestHealthLabel_->setStyleSheet(
            "background: #22c55e; border-radius: 3px;");
    } else {
        rtTestHealthLabel_->setStyleSheet(
            "background: #334155; border-radius: 3px;");
    }

    // Append timeline data — ASCII sparkline of recent cycle times.
    const auto recent = telemetry.value("recent").toArray();
    if (!recent.isEmpty() && running) {
        appendRtTestTimeline(recent, avgUs);
    }
}

// ── Append a timeline block to the text panel ──────────────────────────────
// Renders cycle times as a compact ASCII histogram for quick visual scanning.
void MainWindow::appendRtTestTimeline(const QJsonArray &recent, double avgUsec)
{
    // Downsample to ~80 columns for display.
    const int columns = 80;
    const int step = std::max(1, static_cast<int>(recent.size()) / columns);

    // Build ASCII bar chart — each character represents a bucket of samples.
    // Height is proportional to deviation from average.
    QStringList lines;
    QString barLine;
    for (int i = 0; i < static_cast<int>(recent.size()); i += step) {
        const double us = recent[i].toDouble();
        const double deviation = us - avgUsec;
        // Map deviation to a character: · for near-avg, ▂▃▄▅▆▇ for increasing, ░ for negative.
        QChar ch;
        if (deviation < -50) {
            ch = QChar(0x00B7); // · (below avg)
        } else if (deviation < 10) {
            ch = QChar(0x00B7); // · (near avg)
        } else if (deviation < 30) {
            ch = QChar(0x2581); // ▂
        } else if (deviation < 60) {
            ch = QChar(0x2582); // ▃
        } else if (deviation < 100) {
            ch = QChar(0x2583); // ▄
        } else if (deviation < 200) {
            ch = QChar(0x2585); // ▅
        } else if (deviation < 400) {
            ch = QChar(0x2586); // ▆
        } else if (deviation < 800) {
            ch = QChar(0x2587); // ▇
        } else {
            ch = QChar(0x2588); // █ (severe spike)
        }
        barLine += ch;
    }
    if (!barLine.isEmpty()) {
        lines << barLine;
    }

    // Append a summary line with current stats.
    const qint64 cycles = rtTestCyclesLabel_->text().toLongLong();
    lines << QString("  #%1  avg=%2µs  jit=%3µs  loss=%4%")
                 .arg(cycles)
                 .arg(avgUsec, 0, 'f', 1)
                 .arg(rtTestJitterLabel_->text())
                 .arg(rtTestLossLabel_->text());

    rtTestTimelineText_->appendPlainText(lines.join('\n'));
}

// ── Update RT test action availability ─────────────────────────────────────
// Called from updateActionAvailability() to enable/disable RT test controls.
void MainWindow::updateRtTestActionAvailability()
{
    if (!rtTestStartButton_) return;
    const bool connected = client_.isConnected();
    rtTestStartButton_->setEnabled(connected && !rtTestRunning_);
    rtTestStopButton_->setEnabled(rtTestRunning_);
}

// ── Format a duration in seconds to a human-readable string ────────────────
QString MainWindow::formatDuration(double seconds) const
{
    if (seconds < 60) {
        return QString::number(seconds, 'f', 1) + "s";
    }
    if (seconds < 3600) {
        const int min = static_cast<int>(seconds) / 60;
        const int sec = static_cast<int>(seconds) % 60;
        return QString("%1m %2s").arg(min).arg(sec);
    }
    const int hours = static_cast<int>(seconds) / 3600;
    const int min = (static_cast<int>(seconds) % 3600) / 60;
    return QString("%1h %2m").arg(hours).arg(min);
}
