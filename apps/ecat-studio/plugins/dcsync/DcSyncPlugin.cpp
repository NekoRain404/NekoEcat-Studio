#include "DcSyncPlugin.h"
#include "services/EventBus.h"
#include "services/DcSyncService.h"

#include <QHeaderView>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QVBoxLayout>

static constexpr int kColCount = 8;

DcSyncPlugin::DcSyncPlugin(EventBus *bus, DcSyncService *service,
                           QObject *parent)
    : bus_(bus), service_(service) {
  if (parent) setParent(parent);
  buildUi();

  // Live updates arrive through the EventBus (wired by MainWindow).
  connect(bus_, &EventBus::dcSyncUpdate, this,
          &DcSyncPlugin::handleDcSyncUpdate);
}

// ── Identity ──────────────────────────────────────────────────────────
QString DcSyncPlugin::id() const { return "dcsync"; }
QString DcSyncPlugin::displayName() const { return "DC Sync"; }
QString DcSyncPlugin::displayNameZh() const { return QStringLiteral("DC同步"); }
int DcSyncPlugin::defaultOrder() const { return 60; }
bool DcSyncPlugin::visible() const { return true; }

QWidget *DcSyncPlugin::widget() { return container_; }

// ── UI construction ───────────────────────────────────────────────────
void DcSyncPlugin::buildUi() {
  container_ = new QWidget;
  auto *layout = new QVBoxLayout(container_);
  layout->setContentsMargins(0, 0, 0, 0);

  table_ = new QTableWidget;
  table_->setColumnCount(kColCount);
  table_->setHorizontalHeaderLabels({
    tr("Position"), tr("Name"), tr("DC Capable"),
    tr("Syncing"), tr("Drift (ns)"),
    tr("Jitter Min"), tr("Jitter Max"), tr("Jitter Avg")
  });
  table_->horizontalHeader()->setStretchLastSection(true);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  layout->addWidget(table_);
}

// ── Update handler ────────────────────────────────────────────────────
void DcSyncPlugin::handleDcSyncUpdate(const QJsonObject &data) {
  populateTable(data);
}

void DcSyncPlugin::populateTable(const QJsonObject &data) {
  const QJsonArray slaves = data.value("slaves").toArray();

  // Reserve one extra row for the reference clock header when present.
  const int refPos = data.value("referenceClockPosition").toInt(-1);
  const int rowCount = slaves.size() + (refPos >= 0 ? 1 : 0);
  table_->setRowCount(rowCount);

  int row = 0;

  // Reference-clock summary row (merged look).
  if (refPos >= 0) {
    table_->setItem(row, 0, new QTableWidgetItem(QString::number(refPos)));
    table_->setItem(row, 1,
                    new QTableWidgetItem(data.value("referenceClockName").toString()));
    table_->setItem(row, 2, new QTableWidgetItem(tr("Ref Clock")));
    for (int c = 3; c < kColCount; ++c)
      table_->setItem(row, c, new QTableWidgetItem(QStringLiteral("--")));
    ++row;
  }

  // Per-slave rows.
  for (const auto &entry : slaves) {
    const QJsonObject s = entry.toObject();
    table_->setItem(row, 0,
                    new QTableWidgetItem(QString::number(s.value("position").toInt())));
    table_->setItem(row, 1,
                    new QTableWidgetItem(s.value("name").toString()));
    table_->setItem(row, 2,
                    new QTableWidgetItem(s.value("dcCapable").toBool() ? tr("Yes") : tr("No")));
    table_->setItem(row, 3,
                    new QTableWidgetItem(s.value("syncing").toBool() ? tr("Yes") : tr("No")));
    table_->setItem(row, 4,
                    new QTableWidgetItem(QString::number(s.value("driftNs").toDouble())));
    table_->setItem(row, 5,
                    new QTableWidgetItem(QString::number(s.value("jitterMin").toDouble())));
    table_->setItem(row, 6,
                    new QTableWidgetItem(QString::number(s.value("jitterMax").toDouble())));
    table_->setItem(row, 7,
                    new QTableWidgetItem(QString::number(s.value("jitterAvg").toDouble())));
    ++row;
  }
}
