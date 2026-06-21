#include "WatchPlugin.h"
#include "services/ServiceContainer.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>

WatchPlugin::WatchPlugin(ServiceContainer *container, QObject *parent)
    : container_(container) {
  if (parent) setParent(parent);
  buildUi();
}

// ── Identity ──────────────────────────────────────────────────────────
QString WatchPlugin::id() const { return "watch"; }
QString WatchPlugin::displayName() const { return "Watch"; }
QString WatchPlugin::displayNameZh() const { return QStringLiteral("监视"); }
QIcon WatchPlugin::icon() const { return QIcon::fromTheme("utilities-system-monitor"); }
int WatchPlugin::defaultOrder() const { return 30; }
bool WatchPlugin::visible() const { return true; }

void WatchPlugin::activate() {}
void WatchPlugin::deactivate() {}
void WatchPlugin::onSettingsChanged(const AppSettings &) {}
void WatchPlugin::onConnectionChanged(bool) {}

QWidget *WatchPlugin::widget() { return containerWidget_; }

// ── UI construction ───────────────────────────────────────────────────
void WatchPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(0, 0, 0, 0);

  auto *filterRow = new QHBoxLayout;
  filter_ = new QLineEdit;
  filter_->setPlaceholderText(tr("Filter watch rows..."));
  filter_->setClearButtonEnabled(true);
  filterRow->addWidget(filter_);

  autoRefresh_ = new QCheckBox(tr("Auto refresh"));
  filterRow->addWidget(autoRefresh_);

  refreshInterval_ = new QComboBox;
  refreshInterval_->addItem(tr("500 ms"), 500);
  refreshInterval_->addItem(tr("1 s"), 1000);
  refreshInterval_->addItem(tr("2 s"), 2000);
  refreshInterval_->addItem(tr("5 s"), 5000);
  refreshInterval_->setCurrentIndex(1);
  refreshInterval_->setEnabled(false);
  filterRow->addWidget(refreshInterval_);

  summaryLabel_ = new QLabel;
  filterRow->addWidget(summaryLabel_);

  layout->addLayout(filterRow);

  table_ = new QTableWidget;
  table_->setColumnCount(12);
  table_->setHorizontalHeaderLabels(
      {tr("Time"), tr("Slave"), tr("Index"), tr("Sub"), tr("Value"),
       tr("Decoded"), tr("Type"), tr("Mode"), tr("Baseline"), tr("Delta"),
       tr("Startup"), tr("Startup Delta")});
  table_->horizontalHeader()->setStretchLastSection(true);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  layout->addWidget(table_);

  connect(filter_, &QLineEdit::textChanged, this, &WatchPlugin::filterChanged);
  connect(autoRefresh_, &QCheckBox::toggled, this,
          [this]() { emit watchModified(); });
}

// ── Table access ──────────────────────────────────────────────────────
QTableWidget *WatchPlugin::watchTable() const { return table_; }
QLineEdit *WatchPlugin::filterInput() const { return filter_; }
QCheckBox *WatchPlugin::autoRefreshCheckBox() const { return autoRefresh_; }
QComboBox *WatchPlugin::refreshIntervalCombo() const { return refreshInterval_; }
QLabel *WatchPlugin::summaryLabel() const { return summaryLabel_; }

// ── Table management ──────────────────────────────────────────────────
void WatchPlugin::ensureWatchTable() {
  if (!table_) return;
  if (table_->columnCount() != 12) {
    table_->setColumnCount(12);
  }
  table_->setHorizontalHeaderLabels(
      {tr("Time"), tr("Slave"), tr("Index"), tr("Sub"), tr("Value"),
       tr("Decoded"), tr("Type"), tr("Mode"), tr("Baseline"), tr("Delta"),
       tr("Startup"), tr("Startup Delta")});
}

void WatchPlugin::setWatchHeaders(const QStringList &headers) {
  if (!table_) return;
  table_->setColumnCount(headers.size());
  table_->setHorizontalHeaderLabels(headers);
}

void WatchPlugin::updateWatchRow(int row, const QStringList &columns) {
  if (!table_ || row < 0 || row >= table_->rowCount()) return;
  for (int c = 0; c < columns.size() && c < table_->columnCount(); ++c) {
    auto *item = table_->item(row, c);
    if (!item) {
      item = new QTableWidgetItem(columns[c]);
      table_->setItem(row, c, item);
    } else {
      item->setText(columns[c]);
    }
  }
}

void WatchPlugin::insertWatchRow(int row, const QStringList &columns) {
  if (!table_) return;
  table_->insertRow(row);
  for (int c = 0; c < columns.size() && c < table_->columnCount(); ++c) {
    table_->setItem(row, c, new QTableWidgetItem(columns[c]));
  }
}

void WatchPlugin::removeWatchRow(int row) {
  if (!table_ || row < 0 || row >= table_->rowCount()) return;
  table_->removeRow(row);
}

void WatchPlugin::clearWatch() {
  if (!table_) return;
  table_->clearContents();
  table_->setRowCount(0);
  ensureWatchTable();
}

// ── Selection ─────────────────────────────────────────────────────────
int WatchPlugin::currentRow() const {
  return table_ ? table_->currentRow() : -1;
}

void WatchPlugin::selectRow(int row) {
  if (table_) table_->selectRow(row);
}

int WatchPlugin::rowCount() const { return table_ ? table_->rowCount() : 0; }

bool WatchPlugin::isRowHidden(int row) const {
  return table_ ? table_->isRowHidden(row) : true;
}

void WatchPlugin::resizeColumnsToContents() {
  if (table_) table_->resizeColumnsToContents();
}

// ── Summary ───────────────────────────────────────────────────────────
void WatchPlugin::setSummary(const QString &text) {
  if (summaryLabel_) summaryLabel_->setText(text);
}
