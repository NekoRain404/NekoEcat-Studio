#include "FreeRunPlugin.h"
#include "services/FreeRunMonitoringService.h"
#include "services/ServiceContainer.h"

#include <QCheckBox>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>

FreeRunPlugin::FreeRunPlugin(ServiceContainer *container, QObject *parent)
    : container_(container) {
  if (parent) setParent(parent);
  buildUi();

  auto *frMon = container_->freeRunMonitor();
  connect(frMon, &FreeRunMonitoringService::processDataUpdated,
          this, [this](const FreeRunProcessData &data) {
    QStringList headers = {tr("Offset"), tr("Input (hex)"), tr("Output (hex)")};
    QList<QStringList> rows;
    const int maxLen = qMax(data.inputs.size(), data.outputs.size());
    for (int i = 0; i < maxLen; ++i) {
      QStringList row;
      row << QStringLiteral("0x%1").arg(i, 4, 16, QChar('0'));
      row << (i < data.inputs.size()
                  ? QString::number(data.inputs[i], 16).rightJustified(2, QChar('0'))
                  : QString());
      row << (i < data.outputs.size()
                  ? QString::number(data.outputs[i], 16).rightJustified(2, QChar('0'))
                  : QString());
      rows << row;
    }
    setEntryRows(headers, rows);
    setSummary(tr("%1 entries | cycle %2 | err %3")
                  .arg(maxLen).arg(data.cycleCount).arg(data.errorCount));
  });

  connect(frMon, &FreeRunMonitoringService::statusChanged,
          this, [this](const FreeRunStatus &status) {
    setSummary(tr("State: %1 | uptime %2 ms | active %3 slaves")
                  .arg(status.stateString).arg(status.uptimeMs)
                  .arg(status.activeSlaves));
    setFreeRunEnabled(status.state == FreeRunState::Running);
    setLastStatus(status.stateString);
  });

  connect(frMon, &FreeRunMonitoringService::errorOccurred,
          this, [this](const FreeRunErrorInfo &error) {
    setDetail(tr("Error [%1]: %2").arg(error.code, error.message));
  });
}

// ── Identity ──────────────────────────────────────────────────────────
QString FreeRunPlugin::id() const { return "freerun"; }
QString FreeRunPlugin::displayName() const { return "Free Run"; }
QString FreeRunPlugin::displayNameZh() const { return QStringLiteral("自由运行"); }
QIcon FreeRunPlugin::icon() const { return QIcon::fromTheme("media-playback-start"); }
int FreeRunPlugin::defaultOrder() const { return 35; }
bool FreeRunPlugin::visible() const { return true; }

void FreeRunPlugin::activate() {}
void FreeRunPlugin::deactivate() {}
void FreeRunPlugin::onSettingsChanged(const AppSettings &) {}
void FreeRunPlugin::onConnectionChanged(bool) {}

QWidget *FreeRunPlugin::widget() { return containerWidget_; }

// ── UI construction ───────────────────────────────────────────────────
void FreeRunPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(0, 0, 0, 0);

  auto *filterRow = new QHBoxLayout;
  filter_ = new QLineEdit;
  filter_->setPlaceholderText(tr("Filter free run entries..."));
  filter_->setClearButtonEnabled(true);
  filterRow->addWidget(filter_);

  changedOnly_ = new QCheckBox(tr("Changed only"));
  filterRow->addWidget(changedOnly_);

  summaryLabel_ = new QLabel;
  filterRow->addWidget(summaryLabel_);

  layout->addLayout(filterRow);

  entryTable_ = new QTableWidget;
  entryTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  entryTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  entryTable_->horizontalHeader()->setStretchLastSection(true);
  layout->addWidget(entryTable_);

  detailLabel_ = new QLabel;
  detailLabel_->setWordWrap(true);
  layout->addWidget(detailLabel_);
}

// ── Accessors ─────────────────────────────────────────────────────────
QTableWidget *FreeRunPlugin::entryTable() const { return entryTable_; }
QLineEdit *FreeRunPlugin::filter() const { return filter_; }
QCheckBox *FreeRunPlugin::changedOnly() const { return changedOnly_; }
QLabel *FreeRunPlugin::summaryLabel() const { return summaryLabel_; }
QLabel *FreeRunPlugin::detailLabel() const { return detailLabel_; }

// ── Table population ──────────────────────────────────────────────────
void FreeRunPlugin::setEntryRows(const QStringList &headers,
                                 const QList<QStringList> &rows) {
  entryTable_->setColumnCount(headers.size());
  entryTable_->setHorizontalHeaderLabels(headers);
  entryTable_->setRowCount(rows.size());

  for (int r = 0; r < rows.size(); ++r) {
    const QStringList &cols = rows[r];
    for (int c = 0; c < cols.size() && c < headers.size(); ++c) {
      entryTable_->setItem(r, c, new QTableWidgetItem(cols[c]));
    }
  }
  entryTable_->resizeColumnsToContents();
}

void FreeRunPlugin::setSummary(const QString &text) {
  summaryLabel_->setText(text);
}

void FreeRunPlugin::setDetail(const QString &text) {
  detailLabel_->setText(text);
}

// ── Entry lookup ──────────────────────────────────────────────────────
QString FreeRunPlugin::entryName(int row) const {
  const QString key = QString::number(row);
  return entryNames_.value(key);
}

QString FreeRunPlugin::entryValue(int row) const {
  const QString key = QString::number(row);
  return entryValues_.value(key);
}

void FreeRunPlugin::setEntryNames(const QHash<QString, QString> &names) {
  entryNames_ = names;
}

void FreeRunPlugin::setEntryValues(const QHash<QString, QString> &values) {
  entryValues_ = values;
}

// ── Chart management ──────────────────────────────────────────────────
void FreeRunPlugin::addOpenChart(RealtimeChartDialog *dialog) {
  openCharts_.append(dialog);
  emit chartOpened(dialog);
}

void FreeRunPlugin::removeOpenChart(RealtimeChartDialog *dialog) {
  openCharts_.removeOne(dialog);
  emit chartClosed(dialog);
}

QVector<RealtimeChartDialog *> FreeRunPlugin::openCharts() const {
  return openCharts_;
}

// ── Free Run state ────────────────────────────────────────────────────
bool FreeRunPlugin::freeRunEnabled() const { return freeRunEnabled_; }

void FreeRunPlugin::setFreeRunEnabled(bool enabled) {
  freeRunEnabled_ = enabled;
}

QString FreeRunPlugin::lastStatus() const { return lastStatus_; }

void FreeRunPlugin::setLastStatus(const QString &status) {
  lastStatus_ = status;
}
