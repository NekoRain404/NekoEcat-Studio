#include "IoVariablePlugin.h"
#include "services/ServiceContainer.h"

#include <QComboBox>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>

IoVariablePlugin::IoVariablePlugin(ServiceContainer *container,
                                   QObject *parent)
    : container_(container) {
  if (parent) setParent(parent);
  buildUi();
}

// ── Identity ──────────────────────────────────────────────────────────
QString IoVariablePlugin::id() const { return "iovariable"; }
QString IoVariablePlugin::displayName() const { return "I/O Variables"; }
QString IoVariablePlugin::displayNameZh() const { return QStringLiteral("I/O 变量"); }
QIcon IoVariablePlugin::icon() const { return QIcon::fromTheme("view-sort-ascending"); }
int IoVariablePlugin::defaultOrder() const { return 40; }
bool IoVariablePlugin::visible() const { return true; }

void IoVariablePlugin::activate() {}
void IoVariablePlugin::deactivate() {}
void IoVariablePlugin::onSettingsChanged(const AppSettings &) {}
void IoVariablePlugin::onConnectionChanged(bool) {}

QWidget *IoVariablePlugin::widget() { return containerWidget_; }

// ── UI construction ───────────────────────────────────────────────────
void IoVariablePlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(0, 0, 0, 0);

  // Filter row
  auto *filterRow = new QHBoxLayout;
  scopeFilter_ = new QComboBox;
  scopeFilter_->addItem(tr("All"), QString("all"));
  scopeFilter_->addItem(tr("Process"), QString("process"));
  scopeFilter_->addItem(tr("Watch"), QString("watch"));
  scopeFilter_->addItem(tr("Startup"), QString("startup"));
  scopeFilter_->addItem(tr("PLC Issues"), QString("plcIssues"));
  filterRow->addWidget(scopeFilter_);

  filter_ = new QLineEdit;
  filter_->setPlaceholderText(tr("Filter I/O variables..."));
  filter_->setClearButtonEnabled(true);
  filterRow->addWidget(filter_);

  summaryLabel_ = new QLabel;
  filterRow->addWidget(summaryLabel_);

  layout->addLayout(filterRow);

  // Table
  table_ = new QTableWidget;
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->horizontalHeader()->setStretchLastSection(true);
  layout->addWidget(table_);

  // Detail label
  detailLabel_ = new QLabel;
  detailLabel_->setWordWrap(true);
  layout->addWidget(detailLabel_);

  // Connect filter signals
  connect(filter_, &QLineEdit::textChanged, this, &IoVariablePlugin::filterChanged);
  connect(scopeFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &IoVariablePlugin::scopeFilterChanged);
  connect(table_, &QTableWidget::currentCellChanged, this,
          [this](int row, int, int, int) { emit rowSelectionChanged(row); });
}

// ── Table population ──────────────────────────────────────────────────
void IoVariablePlugin::setRows(const QStringList &headers,
                               const QList<QStringList> &rows) {
  table_->setColumnCount(headers.size());
  table_->setHorizontalHeaderLabels(headers);
  table_->setRowCount(rows.size());

  for (int r = 0; r < rows.size(); ++r) {
    const QStringList &cols = rows[r];
    for (int c = 0; c < cols.size() && c < headers.size(); ++c) {
      table_->setItem(r, c, new QTableWidgetItem(cols[c]));
    }
  }
  table_->resizeColumnsToContents();
}

// ── Summary label ─────────────────────────────────────────────────────
void IoVariablePlugin::setSummary(const QString &text,
                                  const QString &severity) {
  summaryLabel_->setText(text);
  if (!severity.isEmpty()) {
    summaryLabel_->setProperty("severity", severity);
  }
}

void IoVariablePlugin::setSummaryToolTip(const QString &tip) {
  summaryLabel_->setToolTip(tip);
}

// ── Detail label ──────────────────────────────────────────────────────
void IoVariablePlugin::setDetail(const QString &text,
                                 const QString &severity) {
  detailLabel_->setText(text);
  if (!severity.isEmpty()) {
    detailLabel_->setProperty("severity", severity);
  }
}

void IoVariablePlugin::setDetailToolTip(const QString &tip) {
  detailLabel_->setToolTip(tip);
}

// ── Selection ─────────────────────────────────────────────────────────
int IoVariablePlugin::currentRow() const { return table_->currentRow(); }

void IoVariablePlugin::setCurrentCell(int row, int column) {
  table_->setCurrentCell(row, column);
}

int IoVariablePlugin::rowCount() const { return table_->rowCount(); }

bool IoVariablePlugin::isRowHidden(int row) const {
  return table_->isRowHidden(row);
}

void IoVariablePlugin::setRowHidden(int row, bool hidden) {
  table_->setRowHidden(row, hidden);
}

void IoVariablePlugin::resizeColumnsToContents() {
  table_->resizeColumnsToContents();
}

// ── Accessors ─────────────────────────────────────────────────────────
QTableWidget *IoVariablePlugin::ioVariableTable() const { return table_; }
QLineEdit *IoVariablePlugin::ioVariableFilter() const { return filter_; }
QComboBox *IoVariablePlugin::ioVariableScopeFilter() const { return scopeFilter_; }
QLabel *IoVariablePlugin::ioVariableSummaryLabel() const { return summaryLabel_; }
QLabel *IoVariablePlugin::ioVariableDetailLabel() const { return detailLabel_; }
