// DashboardPlugin — implementation.  See header for interface documentation.
#include "DashboardPlugin.h"
#include "EcatChartWidget.h"
#include "services/ChartService.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFrame>

DashboardPlugin::DashboardPlugin(ChartService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();
  setupDashboard();

  refreshTimer_ = new QTimer(this);
}

QString DashboardPlugin::id() const { return "dashboard"; }
QString DashboardPlugin::displayName() const { return "Dashboard"; }
QString DashboardPlugin::displayNameZh() const {
  return QStringLiteral("仪表盘");
}
int DashboardPlugin::defaultOrder() const { return 130; }
bool DashboardPlugin::visible() const { return false; }
QWidget *DashboardPlugin::widget() { return container_; }

int DashboardPlugin::gaugeCount() const { return gauges_.size(); }
int DashboardPlugin::counterCount() const { return counters_.size(); }

// ── UI construction ───────────────────────────────────────────────────

void DashboardPlugin::buildUi() {
  container_ = new QWidget;
  auto *root = new QVBoxLayout(container_);
  root->setContentsMargins(4, 4, 4, 4);

  // ── Top bar: controls ──
  auto *topBar = new QWidget;
  auto *topLayout = new QHBoxLayout(topBar);
  topLayout->setContentsMargins(0, 0, 0, 4);

  topLayout->addWidget(new QLabel(tr("Refresh interval (ms):")));
  refreshSpin_ = new QSpinBox;
  refreshSpin_->setRange(500, 30000);
  refreshSpin_->setValue(2000);
  refreshSpin_->setSingleStep(500);
  topLayout->addWidget(refreshSpin_);

  refreshBtn_ = new QPushButton(tr("Refresh Now"));
  topLayout->addWidget(refreshBtn_);

  topLayout->addStretch();
  root->addWidget(topBar);

  // ── Dashboard grid ──
  dashboardGrid_ = new QWidget;
  gridLayout_ = new QGridLayout(dashboardGrid_);
  gridLayout_->setSpacing(8);
  root->addWidget(dashboardGrid_, 1);

  // ── Connections ──
  connect(refreshSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
          this, [this](int ms) { refreshTimer_->setInterval(ms); });
  connect(refreshBtn_, &QPushButton::clicked, this, &DashboardPlugin::refresh);
}

void DashboardPlugin::setupDashboard() {
  // Gauges: CPU Load, Memory, EtherCAT Bandwidth, Bus Jitter
  const QStringList gaugeNames = {
      tr("CPU Load (%)"), tr("Memory (%)"), tr("Bandwidth (%)"), tr("Bus Jitter (us)")};
  const double gaugeMax[] = {100.0, 100.0, 100.0, 100.0};

  gauges_.clear();
  for (int i = 0; i < 4; ++i) {
    auto *frame = new QFrame;
    frame->setFrameStyle(QFrame::StyledPanel);
    auto *fl = new QVBoxLayout(frame);
    fl->setContentsMargins(4, 4, 4, 4);

    auto *lbl = new QLabel(gaugeNames[i]);
    lbl->setAlignment(Qt::AlignCenter);
    fl->addWidget(lbl);

    auto *cw = new EcatChartWidget;
    cw->setChartType(EcatChartWidget::Gauge);
    cw->setGaugeValue(0.0, 0, gaugeMax[i]);
    cw->setTitle(gaugeNames[i]);
    cw->setMinimumSize(180, 160);
    fl->addWidget(cw);

    gridLayout_->addWidget(frame, i / 2, i % 2);
    gauges_.append({cw, lbl});
  }

  // Counters: Slaves, TX Frames, RX Errors, Cycle Time
  const QStringList counterNames = {
      tr("Slaves"), tr("TX Frames"), tr("RX Errors"), tr("Cycle Time (us)")};
  const QString counterVals[] = {tr("No backend"), tr("No backend"),
                                 tr("No backend"), tr("No backend")};

  counters_.clear();
  for (int i = 0; i < 4; ++i) {
    auto *frame = new QFrame;
    frame->setFrameStyle(QFrame::StyledPanel);
    auto *fl = new QVBoxLayout(frame);
    fl->setContentsMargins(8, 8, 8, 8);

    auto *nameLbl = new QLabel(counterNames[i]);
    nameLbl->setAlignment(Qt::AlignCenter);
    fl->addWidget(nameLbl);

    auto *valLbl = new QLabel(counterVals[i]);
    valLbl->setAlignment(Qt::AlignCenter);
    QFont vf = valLbl->font();
    vf.setPointSize(vf.pointSize() + 6);
    vf.setBold(true);
    valLbl->setFont(vf);
    fl->addWidget(valLbl);

    gridLayout_->addWidget(frame, 2 + i / 2, i % 2);
    counters_.append(valLbl);
  }
}

void DashboardPlugin::refresh() {
  // Runtime dashboard metrics require backend evidence. Do not synthesize
  // changing counters or gauge values from a UI timer.
}
