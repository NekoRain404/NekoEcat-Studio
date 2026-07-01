#include "SdoCachePlugin.h"
#include "services/SdoCacheService.h"

#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

SdoCachePlugin::SdoCachePlugin(SdoCacheService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &SdoCacheService::cacheUpdated, this,
          [this](int) { updateStats(); updateEntries(); });
  connect(service_, &SdoCacheService::cacheInvalidated, this,
          [this](int) { updateStats(); updateEntries(); });
}

QString SdoCachePlugin::id() const { return "sdocache"; }
QString SdoCachePlugin::displayName() const { return "SDO Cache"; }
QString SdoCachePlugin::displayNameZh() const {
  return QStringLiteral("SDO 缓存");
}
QIcon SdoCachePlugin::icon() const {
  return QIcon::fromTheme("drive-harddisk");
}
int SdoCachePlugin::defaultOrder() const { return 160; }
bool SdoCachePlugin::visible() const { return false; }

void SdoCachePlugin::activate() { updateStats(); updateEntries(); }
void SdoCachePlugin::deactivate() {}

QWidget *SdoCachePlugin::widget() { return containerWidget_; }

void SdoCachePlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(10);

  auto *statsGroup = new QGroupBox(tr("Cache Statistics"));
  auto *statsLayout = new QHBoxLayout(statsGroup);
  hitsLabel_ = new QLabel(tr("Hits: 0"));
  missesLabel_ = new QLabel(tr("Misses: 0"));
  hitRateLabel_ = new QLabel(tr("Hit Rate: 0%"));
  sizeLabel_ = new QLabel(tr("Size: 0"));
  statsLayout->addWidget(hitsLabel_);
  statsLayout->addWidget(missesLabel_);
  statsLayout->addWidget(hitRateLabel_);
  statsLayout->addWidget(sizeLabel_);
  statsLayout->addStretch();
  layout->addWidget(statsGroup);

  auto *configGroup = new QGroupBox(tr("Cache Configuration"));
  auto *configLayout = new QHBoxLayout(configGroup);
  configLayout->addWidget(new QLabel(tr("Max Size:")));
  maxSizeSpin_ = new QSpinBox;
  maxSizeSpin_->setRange(64, 65536);
  maxSizeSpin_->setValue(service_->config().maxEntriesPerSlave);
  configLayout->addWidget(maxSizeSpin_);
  configLayout->addWidget(new QLabel(tr("TTL (ms):")));
  ttlSpin_ = new QSpinBox;
  ttlSpin_->setRange(0, 600000);
  ttlSpin_->setValue(service_->config().ttlMs);
  configLayout->addWidget(ttlSpin_);
  configLayout->addWidget(new QLabel(tr("Eviction:")));
  evictionCombo_ = new QComboBox;
  evictionCombo_->addItems({tr("LRU"), tr("LFU"), tr("FIFO")});
  configLayout->addWidget(evictionCombo_);
  configLayout->addStretch();
  layout->addWidget(configGroup);

  connect(maxSizeSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [this](int val) {
            auto cfg = service_->config();
            cfg.maxEntriesPerSlave = val;
            service_->setConfig(cfg);
          });
  connect(ttlSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [this](int val) {
            auto cfg = service_->config();
            cfg.ttlMs = val;
            service_->setConfig(cfg);
          });
  connect(evictionCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int idx) {
            auto cfg = service_->config();
            cfg.evictionPolicy = static_cast<SdoCacheConfig::EvictionPolicy>(idx);
            service_->setConfig(cfg);
          });

  entriesTable_ = new QTableWidget;
  entriesTable_->setColumnCount(4);
  entriesTable_->setHorizontalHeaderLabels(
      {tr("Position"), tr("Index:SubIndex"), tr("Value"), tr("Accesses")});
  entriesTable_->horizontalHeader()->setStretchLastSection(true);
  entriesTable_->verticalHeader()->setVisible(false);
  entriesTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  entriesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  entriesTable_->setAlternatingRowColors(true);
  layout->addWidget(entriesTable_, 1);

  auto *toolbar = new QHBoxLayout;
  toolbar->setSpacing(8);
  clearBtn_ = new QPushButton(tr("Clear Cache"));
  toolbar->addWidget(clearBtn_);
  exportBtn_ = new QPushButton(tr("Export"));
  toolbar->addWidget(exportBtn_);
  importBtn_ = new QPushButton(tr("Import"));
  toolbar->addWidget(importBtn_);
  toolbar->addStretch();
  layout->addLayout(toolbar);

  connect(clearBtn_, &QPushButton::clicked, this,
          [this]() { service_->invalidateAll(); });
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getSaveFileName(
        containerWidget_, tr("Export Cache"), QString(),
        tr("JSON Files (*.json)"));
    if (!path.isEmpty()) {
      // Export cache data via dictionary dump per slave
    }
  });
  connect(importBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getOpenFileName(
        containerWidget_, tr("Import Cache"), QString(),
        tr("JSON Files (*.json)"));
    if (!path.isEmpty()) {
      // Import cache data
    }
  });
}

void SdoCachePlugin::updateStats() {
  qint64 hits = service_->hitCount();
  qint64 misses = service_->missCount();
  qint64 total = hits + misses;
  double rate = total > 0 ? (100.0 * hits / total) : 0.0;

  hitsLabel_->setText(tr("Hits: %1").arg(hits));
  missesLabel_->setText(tr("Misses: %1").arg(misses));
  hitRateLabel_->setText(tr("Hit Rate: %1%").arg(rate, 0, 'f', 1));

  int totalSize = 0;
  // Count across all cached slaves
  // We approximate by summing cached sizes
  sizeLabel_->setText(tr("Slaves: %1").arg(service_->cachedSlaveCount()));
}

void SdoCachePlugin::updateEntries() {
  entriesTable_->setRowCount(0);
  // The plugin displays available cached data
  // Actual entry enumeration would require exposing iteration from the service
}
