#include "NetworkAnalyzerPlugin.h"
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

NetworkAnalyzerPlugin::NetworkAnalyzerPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString NetworkAnalyzerPlugin::id() const {
    return "networkanalyzer";
}
QString NetworkAnalyzerPlugin::displayName() const {
    return "Network Analyzer";
}
QString NetworkAnalyzerPlugin::displayNameZh() const {
    return "网络分析器";
}
int NetworkAnalyzerPlugin::defaultOrder() const {
    return 270;
}
bool NetworkAnalyzerPlugin::visible() const {
    return false;
}

void NetworkAnalyzerPlugin::activate() {}
void NetworkAnalyzerPlugin::deactivate() {}

QWidget* NetworkAnalyzerPlugin::widget() {
    if (!containerWidget_)
        buildUi();
    return containerWidget_;
}

void NetworkAnalyzerPlugin::startCapture() {
    capturing_ = true;
    if (startBtn_)
        startBtn_->setEnabled(false);
    if (stopBtn_)
        stopBtn_->setEnabled(true);
    if (statusLabel_)
        statusLabel_->setText("Capturing...");
    emit captureStarted();
}

void NetworkAnalyzerPlugin::stopCapture() {
    capturing_ = false;
    if (startBtn_)
        startBtn_->setEnabled(true);
    if (stopBtn_)
        stopBtn_->setEnabled(false);
    if (statusLabel_)
        statusLabel_->setText(QString("Stopped: %1 packets").arg(packets_.size()));
    emit captureStopped();
}

bool NetworkAnalyzerPlugin::isCapturing() const {
    return capturing_;
}

void NetworkAnalyzerPlugin::addPacket(const PacketEntry& packet) {
    packets_.append(packet);
    rebuildPacketTable();
    rebuildStatistics();
    emit packetAdded(packets_.size() - 1);
}

int NetworkAnalyzerPlugin::packetCount() const {
    return packets_.size();
}

void NetworkAnalyzerPlugin::clearPackets() {
    packets_.clear();
    filteredIndices_.clear();
    stats_.clear();
    rebuildPacketTable();
    rebuildStatistics();
}

void NetworkAnalyzerPlugin::addFilter(const FilterCondition& filter) {
    filters_.append(filter);
    rebuildFilterTable();
}

void NetworkAnalyzerPlugin::removeFilter(int index) {
    if (index >= 0 && index < filters_.size()) {
        filters_.removeAt(index);
        rebuildFilterTable();
    }
}

int NetworkAnalyzerPlugin::filterCount() const {
    return filters_.size();
}

void NetworkAnalyzerPlugin::applyFilters() {
    filteredIndices_.clear();
    for (int i = 0; i < packets_.size(); ++i) {
        const auto& p = packets_[i];
        bool match = true;
        for (const auto& f : filters_) {
            QString fieldValue;
            if (f.field == "source")
                fieldValue = p.source;
            else if (f.field == "dest")
                fieldValue = p.destination;
            else if (f.field == "protocol")
                fieldValue = p.protocol;
            if (f.op == "==" && fieldValue != f.value)
                match = false;
            else if (f.op == "contains" && !fieldValue.contains(f.value))
                match = false;
        }
        if (match)
            filteredIndices_.append(i);
    }
    rebuildPacketTable();
    if (statusLabel_)
        statusLabel_->setText(QString("Filtered: %1 of %2 packets").arg(filteredIndices_.size()).arg(packets_.size()));
}

int NetworkAnalyzerPlugin::filteredCount() const {
    return filteredIndices_.size();
}

bool NetworkAnalyzerPlugin::exportCapture(const QString& path) {
    if (path.isEmpty())
        return false;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&f);
    out << "Timestamp,Source,Destination,Protocol,Size,Summary\n";
    for (const auto& p : packets_) {
        out << p.timestamp.toString(Qt::ISODate) << "," << p.source << "," << p.destination << "," << p.protocol << ","
            << p.size << "," << p.summary << "\n";
    }
    return out.status() == QTextStream::Ok && f.flush();
}

QTableWidget* NetworkAnalyzerPlugin::packetTable() const {
    return packetTable_;
}
QTableWidget* NetworkAnalyzerPlugin::statisticsTable() const {
    return statisticsTable_;
}
QTableWidget* NetworkAnalyzerPlugin::filterTable() const {
    return filterTable_;
}
QTextEdit* NetworkAnalyzerPlugin::decodeView() const {
    return decodeView_;
}
QLabel* NetworkAnalyzerPlugin::statusLabel() const {
    return statusLabel_;
}

void NetworkAnalyzerPlugin::selectPacket(int index) {
    if (index < 0 || index >= packets_.size())
        return;
    selectedPacket_ = index;
    updateDecodeView(index);
    emit packetSelected(index);
}

void NetworkAnalyzerPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    auto* splitter = new QSplitter(Qt::Vertical);

    auto* topPanel = new QWidget;
    auto* topLayout = new QVBoxLayout(topPanel);

    auto* controlRow = new QWidget;
    auto* controlLayout = new QHBoxLayout(controlRow);
    startBtn_ = new QPushButton("Start");
    stopBtn_ = new QPushButton("Stop");
    stopBtn_->setEnabled(false);
    clearBtn_ = new QPushButton("Clear");
    exportBtn_ = new QPushButton("Export");
    controlLayout->addWidget(startBtn_);
    controlLayout->addWidget(stopBtn_);
    controlLayout->addWidget(clearBtn_);
    controlLayout->addWidget(exportBtn_);
    topLayout->addWidget(controlRow);

    packetTable_ = new QTableWidget;
    packetTable_->setColumnCount(6);
    packetTable_->setHorizontalHeaderLabels({"#", "Time", "Source", "Dest", "Protocol", "Size"});
    topLayout->addWidget(packetTable_);

    splitter->addWidget(topPanel);

    auto* bottomPanel = new QWidget;
    auto* bottomLayout = new QVBoxLayout(bottomPanel);

    tabs_ = new QTabWidget;
    decodeView_ = new QTextEdit;
    decodeView_->setReadOnly(true);
    tabs_->addTab(decodeView_, "Decode");

    statisticsTable_ = new QTableWidget;
    statisticsTable_->setColumnCount(3);
    statisticsTable_->setHorizontalHeaderLabels({"Protocol", "Packets", "Bytes"});
    tabs_->addTab(statisticsTable_, "Statistics");

    auto* filterTab = new QWidget;
    auto* filterLayout = new QVBoxLayout(filterTab);
    auto* filterRow = new QWidget;
    auto* filterRowLayout = new QHBoxLayout(filterRow);
    filterFieldEdit_ = new QLineEdit;
    filterFieldEdit_->setPlaceholderText("Field (source/dest/protocol)");
    filterValueEdit_ = new QLineEdit;
    filterValueEdit_->setPlaceholderText("Value");
    addFilterBtn_ = new QPushButton("Add Filter");
    removeFilterBtn_ = new QPushButton("Remove");
    filterRowLayout->addWidget(filterFieldEdit_);
    filterRowLayout->addWidget(filterValueEdit_);
    filterRowLayout->addWidget(addFilterBtn_);
    filterRowLayout->addWidget(removeFilterBtn_);
    filterLayout->addWidget(filterRow);

    filterTable_ = new QTableWidget;
    filterTable_->setColumnCount(3);
    filterTable_->setHorizontalHeaderLabels({"Field", "Op", "Value"});
    filterLayout->addWidget(filterTable_);

    auto* applyFilterBtn = new QPushButton("Apply Filters");
    filterLayout->addWidget(applyFilterBtn);
    tabs_->addTab(filterTab, "Filters");

    bottomLayout->addWidget(tabs_);

    statusLabel_ = new QLabel("Ready");
    bottomLayout->addWidget(statusLabel_);

    splitter->addWidget(bottomPanel);
    mainLayout->addWidget(splitter);

    connect(startBtn_, &QPushButton::clicked, this, [this]() { startCapture(); });
    connect(stopBtn_, &QPushButton::clicked, this, [this]() { stopCapture(); });
    connect(clearBtn_, &QPushButton::clicked, this, [this]() { clearPackets(); });
    connect(packetTable_, &QTableWidget::cellClicked, this, [this](int row, int) { selectPacket(row); });
    connect(addFilterBtn_, &QPushButton::clicked, this,
            [this]() { addFilter({filterFieldEdit_->text(), "==", filterValueEdit_->text()}); });
    connect(removeFilterBtn_, &QPushButton::clicked, this, [this]() {
        int row = filterTable_->currentRow();
        if (row >= 0)
            removeFilter(row);
    });
    connect(applyFilterBtn, &QPushButton::clicked, this, [this]() { applyFilters(); });
}

void NetworkAnalyzerPlugin::rebuildPacketTable() {
    if (!packetTable_)
        return;
    const auto& src = filteredIndices_.isEmpty() ? QVector<int>() : filteredIndices_;
    const bool useAll = src.isEmpty();
    const int count = useAll ? packets_.size() : src.size();
    packetTable_->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        const auto& p = packets_[useAll ? i : src[i]];
        packetTable_->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        packetTable_->setItem(i, 1, new QTableWidgetItem(p.timestamp.toString(Qt::ISODate)));
        packetTable_->setItem(i, 2, new QTableWidgetItem(p.source));
        packetTable_->setItem(i, 3, new QTableWidgetItem(p.destination));
        packetTable_->setItem(i, 4, new QTableWidgetItem(p.protocol));
        packetTable_->setItem(i, 5, new QTableWidgetItem(QString::number(p.size)));
    }
}

void NetworkAnalyzerPlugin::rebuildStatistics() {
    if (!statisticsTable_)
        return;
    QMap<QString, ProtocolStats> statMap;
    for (const auto& p : packets_) {
        auto& s = statMap[p.protocol];
        s.protocol = p.protocol;
        s.packetCount++;
        s.byteCount += p.size;
    }
    stats_ = statMap.values();
    statisticsTable_->setRowCount(stats_.size());
    for (int i = 0; i < stats_.size(); ++i) {
        statisticsTable_->setItem(i, 0, new QTableWidgetItem(stats_[i].protocol));
        statisticsTable_->setItem(i, 1, new QTableWidgetItem(QString::number(stats_[i].packetCount)));
        statisticsTable_->setItem(i, 2, new QTableWidgetItem(QString::number(stats_[i].byteCount)));
    }
}

void NetworkAnalyzerPlugin::rebuildFilterTable() {
    if (!filterTable_)
        return;
    filterTable_->setRowCount(filters_.size());
    for (int i = 0; i < filters_.size(); ++i) {
        filterTable_->setItem(i, 0, new QTableWidgetItem(filters_[i].field));
        filterTable_->setItem(i, 1, new QTableWidgetItem(filters_[i].op));
        filterTable_->setItem(i, 2, new QTableWidgetItem(filters_[i].value));
    }
}

void NetworkAnalyzerPlugin::updateDecodeView(int index) {
    if (!decodeView_ || index < 0 || index >= packets_.size())
        return;
    const auto& p = packets_[index];
    QString decode;
    decode += "Frame " + QString::number(index) + "\n";
    decode += "  Timestamp: " + p.timestamp.toString(Qt::ISODate) + "\n";
    decode += "  Source:      " + p.source + "\n";
    decode += "  Destination: " + p.destination + "\n";
    decode += "  Protocol:    " + p.protocol + "\n";
    decode += "  Size:        " + QString::number(p.size) + " bytes\n";
    decode += "  Summary:     " + p.summary + "\n";
    decodeView_->setText(decode);
}
