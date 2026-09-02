#include "ReportPlugin.h"
#include <QComboBox>
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QUuid>
#include <QVBoxLayout>

ReportPlugin::ReportPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    reportTemplates_ = {
        {"network_overview", "Network Overview", "Complete network topology and slave list", "HTML"},
        {"diagnostics_report", "Diagnostics Report", "Detailed diagnostics with error analysis", "PDF"},
        {"slave_detail", "Slave Detail Report", "Per-slave configuration and status", "Markdown"},
        {"sdo_inventory", "SDO Inventory", "Full SDO dictionary export", "HTML"},
        {"performance_summary", "Performance Summary", "Bus performance metrics and charts", "PDF"},
    };
    dataSources_ = {
        {"topology", "Network Topology", "topology", true},
        {"slaves", "Slave Information", "slave_info", true},
        {"sdos", "SDO Dictionary", "sdo_data", true},
        {"diagnostics", "Diagnostics Log", "diag_events", true},
        {"performance", "Performance Metrics", "perf_data", false},
        {"alarms", "Alarm History", "alarms", false},
    };
    buildUi();
}

QString ReportPlugin::id() const {
    return "report";
}
QString ReportPlugin::displayName() const {
    return "Report Generator";
}
QString ReportPlugin::displayNameZh() const {
    return "报告生成器";
}
int ReportPlugin::defaultOrder() const {
    return 230;
}
bool ReportPlugin::visible() const {
    return false;
}

void ReportPlugin::activate() {}
void ReportPlugin::deactivate() {}

QWidget* ReportPlugin::widget() {
    if (!containerWidget_)
        buildUi();
    return containerWidget_;
}

void ReportPlugin::addReportTemplate(const ReportTemplate& tmpl) {
    reportTemplates_.append(tmpl);
    rebuildTemplateTable();
}

int ReportPlugin::reportTemplateCount() const {
    return reportTemplates_.size();
}

void ReportPlugin::addDataSource(const DataSource& source) {
    dataSources_.append(source);
    rebuildDataSourceTable();
}

void ReportPlugin::toggleDataSource(int index, bool enabled) {
    if (index >= 0 && index < dataSources_.size()) {
        dataSources_[index].enabled = enabled;
        rebuildDataSourceTable();
    }
}

int ReportPlugin::dataSourceCount() const {
    return dataSources_.size();
}

void ReportPlugin::selectTemplate(int index) {
    if (index < 0 || index >= reportTemplates_.size())
        return;
    selectedTemplate_ = index;
    selectedFormat_ = reportTemplates_[index].format;
    updatePreview();
    emit templateSelected(index);
}

void ReportPlugin::selectFormat(const QString& format) {
    selectedFormat_ = format;
    updatePreview();
}

void ReportPlugin::generateReport() {
    if (selectedTemplate_ < 0 || selectedTemplate_ >= reportTemplates_.size())
        return;
    const auto& tmpl = reportTemplates_[selectedTemplate_];

    ReportRecord rec;
    rec.id = QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
    rec.templateName = tmpl.name;
    rec.format = selectedFormat_;
    rec.generatedAt = QDateTime::currentDateTime();
    rec.filePath = QString("/tmp/report_%1.%2").arg(rec.id).arg(selectedFormat_.toLower());

    history_.append(rec);
    rebuildHistoryTable();

    if (statusLabel_) {
        statusLabel_->setText(tr("Generated: %1").arg(tmpl.name));
    }
    emit reportGenerated(rec.id);
}

int ReportPlugin::reportCount() const {
    return history_.size();
}

QTableWidget* ReportPlugin::templateTable() const {
    return templateTable_;
}
QTableWidget* ReportPlugin::dataSourceTable() const {
    return dataSourceTable_;
}
QTableWidget* ReportPlugin::historyTable() const {
    return historyTable_;
}
QTextEdit* ReportPlugin::previewView() const {
    return previewView_;
}
QLabel* ReportPlugin::statusLabel() const {
    return statusLabel_;
}

bool ReportPlugin::exportReport(const QString& path) {
    if (!previewView_)
        return false;
    if (path.isEmpty())
        return false;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&f);
    out << previewView_->toPlainText();
    return out.status() == QTextStream::Ok && f.flush();
}

bool ReportPlugin::exportHistory(const QString& path) {
    if (path.isEmpty())
        return false;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&f);
    for (const auto& h : history_) {
        out << h.id << "," << h.templateName << "," << h.format << "," << h.generatedAt.toString(Qt::ISODate) << ","
            << h.filePath << "\n";
    }
    return out.status() == QTextStream::Ok && f.flush();
}

void ReportPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QHBoxLayout(containerWidget_);
    auto* splitter = new QSplitter;

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);

    templateTable_ = new QTableWidget;
    templateTable_->setColumnCount(3);
    templateTable_->setHorizontalHeaderLabels({"Template", "Description", "Format"});
    leftLayout->addWidget(new QLabel("Report Templates"));
    leftLayout->addWidget(templateTable_);

    dataSourceTable_ = new QTableWidget;
    dataSourceTable_->setColumnCount(3);
    dataSourceTable_->setHorizontalHeaderLabels({"Source", "Type", "Enabled"});
    leftLayout->addWidget(new QLabel("Data Sources"));
    leftLayout->addWidget(dataSourceTable_);

    auto* btnRow = new QWidget;
    auto* btnLayout = new QHBoxLayout(btnRow);
    generateBtn_ = new QPushButton("Generate");
    exportBtn_ = new QPushButton("Export");
    exportHistoryBtn_ = new QPushButton("Export History");
    btnLayout->addWidget(generateBtn_);
    btnLayout->addWidget(exportBtn_);
    btnLayout->addWidget(exportHistoryBtn_);
    leftLayout->addWidget(btnRow);

    splitter->addWidget(leftPanel);

    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);

    tabs_ = new QTabWidget;
    previewView_ = new QTextEdit;
    previewView_->setReadOnly(true);
    tabs_->addTab(previewView_, "Preview");

    historyTable_ = new QTableWidget;
    historyTable_->setColumnCount(4);
    historyTable_->setHorizontalHeaderLabels({"Template", "Format", "Generated", "Path"});
    tabs_->addTab(historyTable_, "History");

    rightLayout->addWidget(tabs_);

    statusLabel_ = new QLabel("Ready");
    rightLayout->addWidget(statusLabel_);

    splitter->addWidget(rightPanel);
    mainLayout->addWidget(splitter);

    rebuildTemplateTable();
    rebuildDataSourceTable();

    connect(templateTable_, &QTableWidget::cellClicked, this, [this](int row, int) { selectTemplate(row); });
    connect(generateBtn_, &QPushButton::clicked, this, &ReportPlugin::generateReport);
    connect(exportBtn_, &QPushButton::clicked, this, [this]() { exportReport("/tmp/report_export.txt"); });
    connect(exportHistoryBtn_, &QPushButton::clicked, this, [this]() { exportHistory("/tmp/report_history.csv"); });
}

void ReportPlugin::rebuildTemplateTable() {
    if (!templateTable_)
        return;
    templateTable_->setRowCount(reportTemplates_.size());
    for (int i = 0; i < reportTemplates_.size(); ++i) {
        const auto& t = reportTemplates_[i];
        templateTable_->setItem(i, 0, new QTableWidgetItem(t.name));
        templateTable_->setItem(i, 1, new QTableWidgetItem(t.description));
        templateTable_->setItem(i, 2, new QTableWidgetItem(t.format));
    }
}

void ReportPlugin::rebuildDataSourceTable() {
    if (!dataSourceTable_)
        return;
    dataSourceTable_->setRowCount(dataSources_.size());
    for (int i = 0; i < dataSources_.size(); ++i) {
        const auto& ds = dataSources_[i];
        dataSourceTable_->setItem(i, 0, new QTableWidgetItem(ds.name));
        dataSourceTable_->setItem(i, 1, new QTableWidgetItem(ds.type));
        dataSourceTable_->setItem(i, 2, new QTableWidgetItem(ds.enabled ? "Yes" : "No"));
    }
}

void ReportPlugin::rebuildHistoryTable() {
    if (!historyTable_)
        return;
    historyTable_->setRowCount(history_.size());
    for (int i = 0; i < history_.size(); ++i) {
        const auto& h = history_[i];
        historyTable_->setItem(i, 0, new QTableWidgetItem(h.templateName));
        historyTable_->setItem(i, 1, new QTableWidgetItem(h.format));
        historyTable_->setItem(i, 2, new QTableWidgetItem(h.generatedAt.toString(Qt::ISODate)));
        historyTable_->setItem(i, 3, new QTableWidgetItem(h.filePath));
    }
}

void ReportPlugin::updatePreview() {
    if (!previewView_)
        return;
    if (selectedTemplate_ < 0 || selectedTemplate_ >= reportTemplates_.size()) {
        previewView_->setText("Select a report template to preview.");
        return;
    }
    const auto& tmpl = reportTemplates_[selectedTemplate_];
    QString preview = QString("=== %1 ===\n\n").arg(tmpl.name);
    preview += "Description: " + tmpl.description + "\n";
    preview += "Format: " + selectedFormat_ + "\n\n";
    preview += "Data Sources:\n";
    for (const auto& ds : dataSources_) {
        if (ds.enabled) {
            preview += "  - " + ds.name + " (" + ds.type + ")\n";
        }
    }
    preview += "\n[Report content will be generated here]";
    previewView_->setText(preview);
}
