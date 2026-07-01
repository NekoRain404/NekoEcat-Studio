#include "RtTestPlugin.h"
#include "services/ServiceContainer.h"
#include "infra/EcatClient.h"

#include <QComboBox>
#include <QGridLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

// ═════════════════════════════════════════════════════════════════════════════
// RtTestLatencyChart — high-performance QPainter latency chart (no QtCharts)
// ═════════════════════════════════════════════════════════════════════════════
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
        const QRect r = rect().adjusted(48, 24, -8, -20);
        p.fillRect(rect(), QColor("#1e1e2e"));
        p.setPen(QColor("#cdd6f4"));
        p.setFont(QFont("sans-serif", 10, QFont::Bold));
        p.drawText(rect().adjusted(8, 4, 0, 0), Qt::AlignTop, QObject::tr("Cycle Latency (\xC2\xB5s)"));

        if (bufAvg_.size() < 2 || r.width() < 10 || r.height() < 10) return;

        const int n = bufAvg_.size();
        const int xOff = kMax - n;
        const double xScale = static_cast<double>(r.width()) / (kMax - 1);
        const double yRange = yHi_ - yLo_;
        const double yScale = yRange > 0 ? static_cast<double>(r.height()) / yRange : 1.0;

        auto toY = [&](double val) { return r.bottom() - (val - yLo_) * yScale; };
        auto toX = [&](int i) { return r.left() + i * xScale; };

        // Grid lines
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

        // Max filled area
        {
            QColor maxFill("#f38ba8");
            maxFill.setAlpha(25);
            QPolygonF poly;
            poly << QPointF(toX(0), r.bottom());
            for (int i = 0; i < n; ++i)
                poly << QPointF(toX(i), toY(bufMax_[i]));
            poly << QPointF(toX(n - 1), r.bottom());
            p.setPen(Qt::NoPen);
            p.setBrush(maxFill);
            p.drawPolygon(poly);
        }

        // Max line (pink dashed)
        {
            QVector<QPointF> pts(n);
            for (int i = 0; i < n; ++i) pts[i] = QPointF(toX(i), toY(bufMax_[i]));
            p.setPen(QPen(QColor("#f38ba8"), 1, Qt::DashLine));
            p.setBrush(Qt::NoBrush);
            p.drawPolyline(pts);
        }

        // Min line (green dashed)
        {
            QVector<QPointF> pts(n);
            for (int i = 0; i < n; ++i) pts[i] = QPointF(toX(i), toY(bufMin_[i]));
            p.setPen(QPen(QColor("#a6e3a1"), 1, Qt::DashLine));
            p.drawPolyline(pts);
        }

        // Avg line (solid blue)
        {
            QVector<QPointF> pts(n);
            for (int i = 0; i < n; ++i) pts[i] = QPointF(toX(i), toY(bufAvg_[i]));
            p.setPen(QPen(QColor("#89b4fa"), 2));
            p.drawPolyline(pts);
        }

        // Legend
        p.setFont(QFont("monospace", 8));
        int lx = r.right() - 180;
        int ly = r.bottom() + 4;
        auto legendItem = [&](const QColor &c, const QString &label) {
            p.setPen(c);
            p.drawLine(lx, ly + 6, lx + 16, ly + 6);
            p.drawText(lx + 20, ly, label);
            lx += 60;
        };
        legendItem(QColor("#89b4fa"), QObject::tr("Avg"));
        legendItem(QColor("#f38ba8"), QObject::tr("Max"));
        legendItem(QColor("#a6e3a1"), QObject::tr("Min"));
    }

private:
    QVector<double> bufAvg_, bufMin_, bufMax_;
    double yLo_ = 0, yHi_ = 1000;
};

// ═════════════════════════════════════════════════════════════════════════════
// RtTestJitterSpark — compact jitter sparkline (QPainter)
// ═════════════════════════════════════════════════════════════════════════════
class RtTestJitterSpark : public QWidget {
    static constexpr int kMax = 200;
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
                   QObject::tr("Jitter: %1 \xC2\xB5s").arg(samples_.last(), 0, 'f', 1));
    }

private:
    QVector<double> samples_;
};

// ═════════════════════════════════════════════════════════════════════════════
// RtTestPlugin implementation
// ═════════════════════════════════════════════════════════════════════════════

RtTestPlugin::RtTestPlugin(ServiceContainer *container, QObject *parent)
    : container_(container), client_(container->client()) {
  if (parent) setParent(parent);
  buildUi();

  connect(client_, &EcatClient::rtTestTelemetry, this,
          &RtTestPlugin::handleRtTestTelemetry);
}

// ── Identity ──────────────────────────────────────────────────────────────
QString RtTestPlugin::id() const { return "rttest"; }
QString RtTestPlugin::displayName() const { return "RT Test"; }
QString RtTestPlugin::displayNameZh() const { return QStringLiteral("实时测试"); }
int RtTestPlugin::defaultOrder() const { return 75; }
bool RtTestPlugin::visible() const { return false; }

QIcon RtTestPlugin::icon() const { return QIcon::fromTheme("appointment-soon"); }

void RtTestPlugin::activate() {}
void RtTestPlugin::deactivate() {}
void RtTestPlugin::onSettingsChanged(const AppSettings &) {}

QWidget *RtTestPlugin::widget() { return containerWidget_; }

// ── Lifecycle ─────────────────────────────────────────────────────────────
void RtTestPlugin::onConnectionChanged(bool connected) {
  updateActionAvailability();
}

// ── UI construction ───────────────────────────────────────────────────────
void RtTestPlugin::buildUi() {
  containerWidget_ = new QWidget;
  containerWidget_->setObjectName("rtTestPage");
  auto *root = new QVBoxLayout(containerWidget_);
  root->setContentsMargins(4, 4, 4, 4);
  root->setSpacing(4);

  // Control bar
  auto *ctrl = new QHBoxLayout;
  ctrl->setSpacing(6);
  startButton_ = new QPushButton(tr("Start"));
  startButton_->setMinimumWidth(56);
  stopButton_ = new QPushButton(tr("Stop"));
  stopButton_->setMinimumWidth(56);
  stopButton_->setEnabled(false);
  ctrl->addWidget(startButton_);
  ctrl->addWidget(stopButton_);
  ctrl->addSpacing(8);
  ctrl->addWidget(new QLabel(tr("Cycle:")));

  cycleCombo_ = new QComboBox;
  cycleCombo_->addItem("125 \xC2\xB5s (8 kHz)", 125);
  cycleCombo_->addItem("250 \xC2\xB5s (4 kHz)", 250);
  cycleCombo_->addItem("500 \xC2\xB5s (2 kHz)", 500);
  cycleCombo_->addItem("1000 \xC2\xB5s (1 kHz)", 1000);
  cycleCombo_->addItem("2000 \xC2\xB5s (500 Hz)", 2000);
  cycleCombo_->addItem("5000 \xC2\xB5s (200 Hz)", 5000);
  cycleCombo_->addItem("10000 \xC2\xB5s (100 Hz)", 10000);
  cycleCombo_->addItem(tr("Custom..."), -1);
  cycleCombo_->setCurrentIndex(3);
  ctrl->addWidget(cycleCombo_);

  customCycle_ = new QLineEdit;
  customCycle_->setObjectName("rtTestCustomCycle");
  customCycle_->setPlaceholderText("\xC2\xB5s");
  customCycle_->setMaximumWidth(80);
  customCycle_->setVisible(false);
  ctrl->addWidget(customCycle_);

  freqLabel_ = new QLabel("1 kHz");
  freqLabel_->setObjectName("rtTestFreqLabel");
  ctrl->addWidget(freqLabel_);

  connect(cycleCombo_, &QComboBox::currentIndexChanged, this,
          [this](int idx) {
            const bool custom = cycleCombo_->itemData(idx).toInt() < 0;
            customCycle_->setVisible(custom);
            if (custom) customCycle_->setFocus();
            updateFreqLabel();
          });
  connect(customCycle_, &QLineEdit::textChanged, this,
          &RtTestPlugin::updateFreqLabel);
  ctrl->addStretch();

  statusLabel_ = new QLabel(tr("Idle"));
  statusLabel_->setObjectName("rtTestStatusLabel");
  ctrl->addWidget(statusLabel_);
  root->addLayout(ctrl);

  // Main area: left stats + right chart
  auto *splitter = new QSplitter(Qt::Horizontal);

  // Left: compact stats
  auto *left = new QWidget;
  auto *leftLay = new QVBoxLayout(left);
  leftLay->setContentsMargins(0, 0, 0, 0);
  leftLay->setSpacing(6);

  // Timing grid
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
    grid->addWidget(t, r * 2, c);
    grid->addWidget(v, r * 2 + 1, c);
    return v;
  };
  minLabel_    = stat(tr("Min \xC2\xB5s"), 0, 0);
  maxLabel_    = stat(tr("Max \xC2\xB5s"), 0, 1);
  avgLabel_    = stat(tr("Avg \xC2\xB5s"), 1, 0);
  jitterLabel_ = stat(tr("Jitter \xC2\xB5s"), 1, 1);
  leftLay->addLayout(grid);

  // Counters
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
    col->addWidget(t);
    col->addWidget(v);
    cnt->addLayout(col);
    return v;
  };
  cyclesLabel_   = ctr(tr("Cycles"));
  errorsLabel_   = ctr(tr("Errors"));
  lossLabel_     = ctr(tr("Loss %"));
  durationLabel_ = ctr(tr("Time"));
  leftLay->addLayout(cnt);

  // Health bar
  healthLabel_ = new QLabel;
  healthLabel_->setFixedHeight(4);
  healthLabel_->setObjectName("rtTestHealthBar");
  leftLay->addWidget(healthLabel_);

  // Jitter sparkline
  jitterSpark_ = new RtTestJitterSpark;
  leftLay->addWidget(jitterSpark_);
  leftLay->addStretch();
  splitter->addWidget(left);

  // Right: latency chart
  chart_ = new RtTestLatencyChart;
  splitter->addWidget(chart_);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 4);
  root->addWidget(splitter, 1);

  // Timeline log
  timelineText_ = new QPlainTextEdit;
  timelineText_->setReadOnly(true);
  timelineText_->setMaximumBlockCount(1000);
  timelineText_->setMaximumHeight(80);
  timelineText_->setPlaceholderText(tr("Cycle log..."));
  root->addWidget(timelineText_);

  // Wire control signals
  connect(startButton_, &QPushButton::clicked, this, [this] {
    const int usec = customCycle_->isVisible()
        ? customCycle_->text().toInt()
        : cycleCombo_->currentData().toInt();
    if (usec > 0) client_->rtTestStart(usec);
  });
  connect(stopButton_, &QPushButton::clicked, this, [this] {
    client_->rtTestStop();
  });
}

// ── Telemetry handler ─────────────────────────────────────────────────────
void RtTestPlugin::handleRtTestTelemetry(const QJsonObject &telemetry) {
  const bool running = telemetry.value("running").toBool();
  running_ = running;
  const qint64 cycles = telemetry.value("cycles").toVariant().toLongLong();
  const qint64 errors = telemetry.value("errors").toVariant().toLongLong();
  const double lossRate = telemetry.value("lossRate").toDouble();
  const double minUs = telemetry.value("minUsec").toDouble();
  const double maxUs = telemetry.value("maxUsec").toDouble();
  const double avgUs = telemetry.value("avgUsec").toDouble();
  const double jitterUs = telemetry.value("jitterUsec").toDouble();
  const QJsonArray recent = telemetry.value("recent").toArray();

  // Buttons
  startButton_->setEnabled(!running && client_->isConnected());
  stopButton_->setEnabled(running);
  cycleCombo_->setEnabled(!running);
  customCycle_->setEnabled(!running);

  // Status
  if (running) {
    statusLabel_->setText(tr("Running"));
    statusLabel_->setProperty("state", "running");
  } else if (cycles > 0) {
    statusLabel_->setText(tr("Stopped"));
    statusLabel_->setProperty("state", "stopped");
  } else {
    statusLabel_->setText(tr("Idle"));
    statusLabel_->setProperty("state", "idle");
  }

  // Metrics
  if (cycles > 0) {
    minLabel_->setText(QString::number(minUs, 'f', 1));
    maxLabel_->setText(QString::number(maxUs, 'f', 1));
    avgLabel_->setText(QString::number(avgUs, 'f', 1));
    jitterLabel_->setText(QString::number(jitterUs, 'f', 1));
  }
  cyclesLabel_->setText(QString::number(cycles));
  errorsLabel_->setText(QString::number(errors));
  lossLabel_->setText(QString::number(lossRate, 'f', 3));
  if (cycles > 0 && avgUs > 0) {
    durationLabel_->setText(
        formatDuration(static_cast<double>(cycles) * avgUs / 1e6));
  }

  // Health bar
  if (cycles > 0) {
    if (lossRate > 0.1 || jitterUs > 500)
      healthLabel_->setProperty("health", "bad");
    else if (jitterUs > 100)
      healthLabel_->setProperty("health", "warn");
    else
      healthLabel_->setProperty("health", "good");
  } else {
    healthLabel_->setProperty("health", "idle");
  }

  // Jitter sparkline
  if (jitterSpark_) jitterSpark_->pushSample(jitterUs);

  // Chart — downsample to ~3000 points
  if (chart_ && !recent.isEmpty() && cycles > 0) {
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
    chart_->appendData(avgA, minA, maxA);
  }

  // Periodic log entry
  if (running && cycles > 0 && (cycles % 5000 == 0)) {
    timelineText_->appendPlainText(
        QString("#%1  avg=%2\xC2\xB5s  jit=%3\xC2\xB5s  loss=%4%")
            .arg(cycles).arg(avgUs, 0, 'f', 1)
            .arg(jitterUs, 0, 'f', 1).arg(lossRate, 0, 'f', 3));
  }

  updateActionAvailability();
}

// ── Helpers ───────────────────────────────────────────────────────────────
void RtTestPlugin::updateFreqLabel() {
  const int usec = customCycle_->isVisible()
      ? customCycle_->text().toInt()
      : cycleCombo_->currentData().toInt();
  if (usec > 0) {
    const double hz = 1000000.0 / usec;
    freqLabel_->setText(hz >= 1000
        ? QString::number(hz / 1000.0, 'f', 1) + " kHz"
        : QString::number(hz, 'f', 0) + " Hz");
  }
}

void RtTestPlugin::updateActionAvailability() {
  if (!startButton_) return;
  startButton_->setEnabled(client_->isConnected() && !running_);
  stopButton_->setEnabled(running_);
}

QString RtTestPlugin::formatDuration(double seconds) {
  if (seconds < 60) return QString::number(seconds, 'f', 1) + "s";
  if (seconds < 3600)
    return QString("%1m %2s").arg(int(seconds) / 60).arg(int(seconds) % 60);
  return QString("%1h %2m").arg(int(seconds) / 3600).arg((int(seconds) % 3600) / 60);
}
