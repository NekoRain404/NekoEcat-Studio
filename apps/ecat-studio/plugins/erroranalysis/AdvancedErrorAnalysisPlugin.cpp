#include "AdvancedErrorAnalysisPlugin.h"
#include "ErrorTimelineWidget.h"
#include "ErrorCorrelationWidget.h"
#include "services/AdvancedErrorAnalysisService.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

AdvancedErrorAnalysisPlugin::AdvancedErrorAnalysisPlugin(QObject *parent) {
  setParent(parent);
  analysisService_ = new AdvancedErrorAnalysisService(this);
  buildUi();
  populateTestData();
}

QString AdvancedErrorAnalysisPlugin::id() const {
  return "erroranalysis";
}
QString AdvancedErrorAnalysisPlugin::displayName() const {
  return "Error Analysis";
}
QString AdvancedErrorAnalysisPlugin::displayNameZh() const {
  return QStringLiteral("错误分析");
}
QIcon AdvancedErrorAnalysisPlugin::icon() const {
  return QIcon::fromTheme("dialog-error");
}
int AdvancedErrorAnalysisPlugin::defaultOrder() const { return 34; }
bool AdvancedErrorAnalysisPlugin::visible() const { return true; }

void AdvancedErrorAnalysisPlugin::activate() {}
void AdvancedErrorAnalysisPlugin::deactivate() {}

QWidget *AdvancedErrorAnalysisPlugin::widget() { return containerWidget_; }

QTableWidget *AdvancedErrorAnalysisPlugin::errorTable() const {
  return errorTable_;
}
ErrorTimelineWidget *AdvancedErrorAnalysisPlugin::timelineWidget() const {
  return timelineWidget_;
}
ErrorCorrelationWidget *AdvancedErrorAnalysisPlugin::correlationWidget() const {
  return correlationWidget_;
}
QLabel *AdvancedErrorAnalysisPlugin::summaryLabel() const {
  return summaryLabel_;
}

void AdvancedErrorAnalysisPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *filterRow = new QHBoxLayout;
  severityFilter_ = new QComboBox;
  severityFilter_->addItem(tr("All"), QString());
  severityFilter_->addItem(tr("Critical"), QStringLiteral("Critical"));
  severityFilter_->addItem(tr("Error"), QStringLiteral("Error"));
  severityFilter_->addItem(tr("Warning"), QStringLiteral("Warning"));
  severityFilter_->addItem(tr("Info"), QStringLiteral("Info"));
  filterRow->addWidget(severityFilter_);

  filter_ = new QLineEdit;
  filter_->setPlaceholderText(tr("Filter errors..."));
  filter_->setClearButtonEnabled(true);
  filterRow->addWidget(filter_);

  analyzeBtn_ = new QPushButton(tr("Analyze"));
  filterRow->addWidget(analyzeBtn_);

  exportBtn_ = new QPushButton(tr("Export Report"));
  filterRow->addWidget(exportBtn_);

  clearBtn_ = new QPushButton(tr("Clear"));
  filterRow->addWidget(clearBtn_);

  summaryLabel_ = new QLabel;
  filterRow->addWidget(summaryLabel_);

  mainLayout->addLayout(filterRow);

  auto *splitter = new QSplitter(Qt::Vertical);

  auto *topWidget = new QWidget;
  auto *topLayout = new QVBoxLayout(topWidget);
  topLayout->setContentsMargins(0, 0, 0, 0);

  auto *tabs = new QTabWidget;

  errorTable_ = new QTableWidget;
  errorTable_->setColumnCount(6);
  errorTable_->setHorizontalHeaderLabels(
      {tr("ID"), tr("Timestamp"), tr("Slave"), tr("Category"),
       tr("Severity"), tr("Message")});
  errorTable_->horizontalHeader()->setStretchLastSection(true);
  errorTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  errorTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tabs->addTab(errorTable_, tr("Error Log"));

  timelineWidget_ = new ErrorTimelineWidget;
  tabs->addTab(timelineWidget_, tr("Timeline"));

  topLayout->addWidget(tabs);
  splitter->addWidget(topWidget);

  correlationWidget_ = new ErrorCorrelationWidget;
  splitter->addWidget(correlationWidget_);

  mainLayout->addWidget(splitter);

  connect(filter_, &QLineEdit::textChanged, this, [this]() {
    const QString needle = filter_->text().trimmed();
    const QString sev =
        severityFilter_ ? severityFilter_->currentData().toString() : QString();
    for (int row = 0; row < errorTable_->rowCount(); ++row) {
      bool match = true;
      if (!sev.isEmpty()) {
        const auto *item = errorTable_->item(row, 4);
        match = item && item->text() == sev;
      }
      if (match && !needle.isEmpty()) {
        match = false;
        for (int col = 0; col < errorTable_->columnCount() && !match; ++col) {
          const auto *item = errorTable_->item(row, col);
          match = item && item->text().contains(needle, Qt::CaseInsensitive);
        }
      }
      errorTable_->setRowHidden(row, !match);
    }
    int visible = 0;
    for (int row = 0; row < errorTable_->rowCount(); ++row)
      if (!errorTable_->isRowHidden(row)) ++visible;
    summaryLabel_->setText(tr("%1 of %2 errors shown")
                               .arg(visible)
                               .arg(errorTable_->rowCount()));
  });

  connect(severityFilter_,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this]() { filter_->textChanged(filter_->text()); });

  connect(analyzeBtn_, &QPushButton::clicked, this,
          &AdvancedErrorAnalysisPlugin::runAnalysis);
  connect(exportBtn_, &QPushButton::clicked, this,
          [this]() { exportReport(containerWidget_); });
  connect(clearBtn_, &QPushButton::clicked, this, [this]() {
    errorTable_->setRowCount(0);
    timelineWidget_->clearEvents();
    correlationWidget_->clear();
    analysisService_->clearHistory();
    summaryLabel_->setText(tr("Cleared"));
  });
}

void AdvancedErrorAnalysisPlugin::populateTestData() {
  struct TestEntry {
    int slave;
    QString cat;
    QString sev;
    QString msg;
  };
  QVector<TestEntry> tests = {
      {0, "Communication", "Error", "Lost frame on port 0"},
      {1, "Device", "Warning", "Slave not responding"},
      {0, "Communication", "Error", "CRC error on port 1"},
      {2, "Configuration", "Warning", "PDO mapping mismatch"},
      {3, "Protocol", "Critical", "AL state timeout"},
      {0, "Communication", "Error", "Frame collision detected"},
      {1, "Device", "Error", "Watchdog timeout"},
      {4, "Communication", "Warning", "Link lost on port 3"},
      {2, "Configuration", "Error", "SDO download failed"},
      {0, "Communication", "Critical", "Bus down"},
  };

  QVector<TimelineEvent> timelineEvents;
  QDateTime base = QDateTime::currentDateTime().addSecs(-3600);

  for (int i = 0; i < tests.size(); ++i) {
    const auto &t = tests[i];
    QDateTime ts = base.addSecs(i * 400);

    int row = errorTable_->rowCount();
    errorTable_->insertRow(row);
    errorTable_->setItem(row, 0,
                          new QTableWidgetItem(QString::number(i + 1)));
    errorTable_->setItem(row, 1, new QTableWidgetItem(ts.toString("hh:mm:ss")));
    errorTable_->setItem(row, 2,
                          new QTableWidgetItem(QString::number(t.slave)));
    errorTable_->setItem(row, 3, new QTableWidgetItem(t.cat));
    errorTable_->setItem(row, 4, new QTableWidgetItem(t.sev));
    errorTable_->setItem(row, 5, new QTableWidgetItem(t.msg));

    AdvancedErrorInfo info;
    info.id = i + 1;
    info.timestamp = ts;
    info.slavePosition = t.slave;
    info.category = t.cat;
    info.severity = t.sev;
    info.message = t.msg;
    analysisService_->addError(info);

    TimelineEvent te;
    te.timestamp = ts;
    te.severity = t.sev;
    te.message = t.msg;
    te.slavePosition = t.slave;
    timelineEvents.append(te);
  }

  timelineWidget_->setEvents(timelineEvents);
  summaryLabel_->setText(
      tr("%1 errors loaded").arg(errorTable_->rowCount()));
}

void AdvancedErrorAnalysisPlugin::runAnalysis() {
  auto errors = analysisService_->errorHistory();
  analysisService_->detectPatterns(errors);
  auto matrix = analysisService_->analyzeCorrelation(errors);
  analysisService_->predictErrors(errors);

  QVector<CorrelationDisplayEntry> displayEntries;
  for (const auto &e : matrix.entries) {
    CorrelationDisplayEntry de;
    de.typeA = e.errorTypeA;
    de.typeB = e.errorTypeB;
    de.value = e.correlation;
    de.relationship = e.relationship;
    displayEntries.append(de);
  }
  correlationWidget_->setCorrelationData(displayEntries);

  if (!errors.isEmpty()) {
    auto rca = analysisService_->analyzeRootCause(errors.first());
    RootCauseDisplay rd;
    rd.errorType = rca.errorType;
    rd.rootCause = rca.rootCause;
    rd.confidence = rca.confidence;
    rd.factors = rca.contributingFactors;
    rd.actions = rca.recommendedActions;
    correlationWidget_->setRootCause(rd);

    QStringList recs;
    for (const auto &p :
         analysisService_->detectPatterns(errors))
      recs.append(p.description);
    for (const auto &r : rca.recommendedActions) recs.append(r);
    correlationWidget_->setRecommendations(recs);
  }
}

void AdvancedErrorAnalysisPlugin::exportReport(QWidget *parentWidget) {
  const QString path = QFileDialog::getSaveFileName(
      parentWidget, tr("Export Error Analysis Report"), QString(),
      "Markdown (*.md);;Text (*.txt)");
  if (path.isEmpty()) return;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

  QTextStream out(&file);
  out << "# Error Analysis Report\n\n";
  out << "## Error Log\n\n";
  out << "| " << tr("ID") << " | " << tr("Timestamp") << " | " << tr("Slave")
      << " | " << tr("Category") << " | " << tr("Severity") << " | "
      << tr("Message") << " |\n";
  out << "| --- | --- | --- | --- | --- | --- |\n";
  for (int row = 0; row < errorTable_->rowCount(); ++row) {
    for (int col = 0; col < errorTable_->columnCount(); ++col) {
      const auto *item = errorTable_->item(row, col);
      out << (item ? item->text() : QString()) << " | ";
    }
    out << "\n";
  }

  out << "\n## Correlation Analysis\n\n";
  auto *ct = correlationWidget_->correlationTable();
  for (int row = 0; row < ct->rowCount(); ++row) {
    out << "- " << ct->item(row, 0)->text() << " <-> "
        << ct->item(row, 1)->text() << " ("
        << ct->item(row, 3)->text() << ")\n";
  }

  out << "\n## Root Cause Analysis\n\n";
  out << correlationWidget_->rootCauseText()->toPlainText() << "\n";

  out << "\n## Recommendations\n\n";
  out << correlationWidget_->recommendationsText()->toPlainText() << "\n";
}
