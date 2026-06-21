#include "TopologyPlugin.h"
#include "TopologyGraphWidget.h"
#include "services/EventBus.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

TopologyPlugin::TopologyPlugin(EventBus *bus, QObject *parent)
    : bus_(bus) {
  if (parent) setParent(parent);
  buildUi();

  connect(bus_, &EventBus::slaveChanged, this,
          &TopologyPlugin::handleSlaveChanged);
}

// ── Identity ──────────────────────────────────────────────────────────
QString TopologyPlugin::id() const { return "topology"; }
QString TopologyPlugin::displayName() const { return "Topology"; }
QString TopologyPlugin::displayNameZh() const { return QStringLiteral("拓扑"); }
int TopologyPlugin::defaultOrder() const { return 15; }
bool TopologyPlugin::visible() const { return true; }

QWidget *TopologyPlugin::widget() { return container_; }

// ── UI construction ───────────────────────────────────────────────────
void TopologyPlugin::buildUi() {
  container_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(container_);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(4);

  // Toolbar
  auto *toolbar = new QHBoxLayout;
  toolbar->setContentsMargins(4, 4, 4, 0);

  layoutCombo_ = new QComboBox;
  layoutCombo_->addItem(tr("Linear Layout"), 0);
  layoutCombo_->addItem(tr("Tree Layout"), 1);
  toolbar->addWidget(layoutCombo_);

  auto *zoomInBtn = new QPushButton(tr("Zoom In"));
  auto *zoomOutBtn = new QPushButton(tr("Zoom Out"));
  auto *fitBtn = new QPushButton(tr("Fit"));
  toolbar->addWidget(zoomInBtn);
  toolbar->addWidget(zoomOutBtn);
  toolbar->addWidget(fitBtn);
  toolbar->addStretch();

  mainLayout->addLayout(toolbar);

  // Graph widget
  graph_ = new TopologyGraphWidget;
  mainLayout->addWidget(graph_);

  // Connections
  connect(layoutCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int index) {
            graph_->setLayoutMode(index == 0
                                      ? TopologyGraphWidget::Layout::Linear
                                      : TopologyGraphWidget::Layout::Tree);
          });

  connect(zoomInBtn, &QPushButton::clicked, graph_, &TopologyGraphWidget::zoomIn);
  connect(zoomOutBtn, &QPushButton::clicked, graph_, &TopologyGraphWidget::zoomOut);
  connect(fitBtn, &QPushButton::clicked, graph_, &TopologyGraphWidget::fitToView);
}

// ── Update handler ────────────────────────────────────────────────────
void TopologyPlugin::handleSlaveChanged(const QVector<SlaveInfo> &slaves) {
  graph_->setSlaves(slaves);
}
