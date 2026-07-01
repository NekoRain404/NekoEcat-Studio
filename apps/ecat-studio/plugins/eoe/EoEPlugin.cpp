// EoEPlugin — Ethernet over EtherCAT workspace plugin implementation.

#include "EoEPlugin.h"

#include "services/EoEService.h"
#include "services/EventBus.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>

EoEPlugin::EoEPlugin(EoEService *eoeService, EventBus *eventBus,
                     QObject *parent)
    : eoeService_(eoeService), eventBus_(eventBus)
{
    if (parent) setParent(parent);
    // Connect EoE service signals.
    connect(eoeService_, &EoEService::statusReceived,
            this, &EoEPlugin::onStatusReceived);
    connect(eoeService_, &EoEService::ipConfigured,
            this, &EoEPlugin::onIpConfigured);
    connect(eoeService_, &EoEService::ipReadback,
            this, &EoEPlugin::onIpReadback);
    connect(eoeService_, &EoEService::statsReceived,
            this, &EoEPlugin::onStatsReceived);
    connect(eoeService_, &EoEService::error,
            this, &EoEPlugin::onError);

    // Listen for topology changes (slave scan updates).
    connect(eventBus_, &EventBus::topologyChanged,
            this, &EoEPlugin::onSlaveScanComplete);
}

QString EoEPlugin::id() const { return "eoe"; }
QString EoEPlugin::displayName() const { return "EoE"; }
QString EoEPlugin::displayNameZh() const { return QStringLiteral("以太网透传"); }
int EoEPlugin::defaultOrder() const { return 155; }
bool EoEPlugin::visible() const { return true; }

QWidget *EoEPlugin::widget() {
    if (!container_) buildUi();
    return container_;
}

void EoEPlugin::buildUi() {
    container_ = new QWidget();
    auto *mainLayout = new QVBoxLayout(container_);

    // Top: slave table + status.
    auto *topSplitter = new QSplitter(Qt::Horizontal);

    // Slave list.
    slaveTable_ = new QTableWidget(0, 3);
    slaveTable_->setHorizontalHeaderLabels({"Pos", "Name", "State"});
    slaveTable_->horizontalHeader()->setStretchLastSection(true);
    slaveTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    slaveTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    slaveTable_->setMaximumWidth(400);
    connect(slaveTable_, &QTableWidget::itemSelectionChanged,
            this, &EoEPlugin::querySelectedSlaveStatus);
    topSplitter->addWidget(slaveTable_);

    // Status group.
    statusGroup_ = new QGroupBox(tr("EoE Status"));
    auto *statusLayout = new QGridLayout(statusGroup_);
    statusLayout->addWidget(new QLabel(tr("EoE Support:")), 0, 0);
    eoeSupportLabel_ = new QLabel("—");
    statusLayout->addWidget(eoeSupportLabel_, 0, 1);
    statusLayout->addWidget(new QLabel(tr("IP Config:")), 1, 0);
    ipConfigSupportLabel_ = new QLabel("—");
    statusLayout->addWidget(ipConfigSupportLabel_, 1, 1);
    statusLayout->addWidget(new QLabel(tr("Current IP:")), 2, 0);
    currentIpLabel_ = new QLabel("—");
    statusLayout->addWidget(currentIpLabel_, 2, 1);
    topSplitter->addWidget(statusGroup_);

    mainLayout->addWidget(topSplitter);

    // Middle: IP configuration.
    ipGroup_ = new QGroupBox(tr("IP Configuration"));
    auto *ipLayout = new QGridLayout(ipGroup_);
    ipLayout->addWidget(new QLabel(tr("IP:")), 0, 0);
    ipEdit_ = new QLineEdit("192.168.1.100");
    ipLayout->addWidget(ipEdit_, 0, 1);
    ipLayout->addWidget(new QLabel(tr("Subnet:")), 0, 2);
    subnetEdit_ = new QLineEdit("255.255.255.0");
    ipLayout->addWidget(subnetEdit_, 0, 3);
    ipLayout->addWidget(new QLabel(tr("Gateway:")), 1, 0);
    gatewayEdit_ = new QLineEdit;
    gatewayEdit_->setPlaceholderText("optional");
    ipLayout->addWidget(gatewayEdit_, 1, 1);
    ipLayout->addWidget(new QLabel(tr("DNS:")), 1, 2);
    dnsEdit_ = new QLineEdit;
    dnsEdit_->setPlaceholderText("optional");
    ipLayout->addWidget(dnsEdit_, 1, 3);

    configIpBtn_ = new QPushButton(tr("Configure IP"));
    connect(configIpBtn_, &QPushButton::clicked,
            this, &EoEPlugin::configureSelectedSlaveIp);
    ipLayout->addWidget(configIpBtn_, 2, 0, 1, 2);

    readIpBtn_ = new QPushButton(tr("Read IP"));
    connect(readIpBtn_, &QPushButton::clicked,
            this, &EoEPlugin::querySelectedSlaveIp);
    ipLayout->addWidget(readIpBtn_, 2, 2, 1, 2);

    mainLayout->addWidget(ipGroup_);

    // Bottom: statistics.
    statsGroup_ = new QGroupBox(tr("EoE Statistics"));
    auto *statsLayout = new QGridLayout(statsGroup_);
    statsLayout->addWidget(new QLabel(tr("TX Frames:")), 0, 0);
    txFramesLabel_ = new QLabel("—");
    statsLayout->addWidget(txFramesLabel_, 0, 1);
    statsLayout->addWidget(new QLabel(tr("RX Frames:")), 0, 2);
    rxFramesLabel_ = new QLabel("—");
    statsLayout->addWidget(rxFramesLabel_, 0, 3);
    statsLayout->addWidget(new QLabel(tr("TX Errors:")), 1, 0);
    txErrorsLabel_ = new QLabel("—");
    statsLayout->addWidget(txErrorsLabel_, 1, 1);
    statsLayout->addWidget(new QLabel(tr("RX Errors:")), 1, 2);
    rxErrorsLabel_ = new QLabel("—");
    statsLayout->addWidget(rxErrorsLabel_, 1, 3);

    refreshStatsBtn_ = new QPushButton(tr("Refresh Stats"));
    connect(refreshStatsBtn_, &QPushButton::clicked,
            this, &EoEPlugin::querySelectedSlaveStats);
    statsLayout->addWidget(refreshStatsBtn_, 2, 0, 1, 4);

    mainLayout->addWidget(statsGroup_);

    // Status log.
    statusLog_ = new QLabel;
    statusLog_->setStyleSheet("color: gray; font-size: 11px;");
    mainLayout->addWidget(statusLog_);
}

void EoEPlugin::onSlaveScanComplete(const QVector<SlaveInfo> &slaves) {
    if (!slaveTable_) return;
    slaveTable_->setRowCount(slaves.size());
    for (int i = 0; i < slaves.size(); ++i) {
        slaveTable_->setItem(i, 0, new QTableWidgetItem(QString::number(slaves[i].position)));
        slaveTable_->setItem(i, 1, new QTableWidgetItem(slaves[i].name));
        slaveTable_->setItem(i, 2, new QTableWidgetItem(slaves[i].state));
    }
    statusLog_->setText(tr("Found %1 slaves. Select one to query EoE status.").arg(slaves.size()));
}

void EoEPlugin::refreshSlaveList() {
    // Slave list is populated via topologyChanged signal.
    statusLog_->setText(tr("Waiting for slave scan..."));
}

void EoEPlugin::querySelectedSlaveStatus() {
    auto *item = slaveTable_->currentItem();
    if (!item) return;
    const int row = item->row();
    const int pos = slaveTable_->item(row, 0)->text().toInt();
    eoeService_->queryStatus(pos);
}

void EoEPlugin::configureSelectedSlaveIp() {
    auto *item = slaveTable_->currentItem();
    if (!item) {
        statusLog_->setText(tr("Select a slave first."));
        return;
    }
    const int row = item->row();
    const int pos = slaveTable_->item(row, 0)->text().toInt();
    const QString ip = ipEdit_->text().trimmed();
    const QString subnet = subnetEdit_->text().trimmed();
    if (ip.isEmpty() || subnet.isEmpty()) {
        statusLog_->setText(tr("IP and subnet are required."));
        return;
    }
    eoeService_->configureIp(pos, ip, subnet);
    statusLog_->setText(tr("Configuring IP for slave %1...").arg(pos));
}

void EoEPlugin::querySelectedSlaveIp() {
    auto *item = slaveTable_->currentItem();
    if (!item) return;
    const int row = item->row();
    const int pos = slaveTable_->item(row, 0)->text().toInt();
    eoeService_->queryIp(pos);
}

void EoEPlugin::querySelectedSlaveStats() {
    auto *item = slaveTable_->currentItem();
    if (!item) return;
    const int row = item->row();
    const int pos = slaveTable_->item(row, 0)->text().toInt();
    eoeService_->queryStats(pos);
}

void EoEPlugin::onStatusReceived(int position, const QJsonObject &data) {
    const bool supported = data.value("supported").toBool();
    const bool hasIp = data.value("hasIpConfig").toBool();
    const QString currentIp = data.value("currentIp").toString();

    eoeSupportLabel_->setText(supported ? tr("Yes") : tr("No"));
    eoeSupportLabel_->setStyleSheet(supported ? "color: green;" : "color: red;");
    ipConfigSupportLabel_->setText(hasIp ? tr("Yes") : tr("No"));
    currentIpLabel_->setText(currentIp.isEmpty() ? "—" : currentIp);

    statusLog_->setText(tr("Slave %1: EoE %2, IP config %3")
                            .arg(position)
                            .arg(supported ? "supported" : "not supported")
                            .arg(hasIp ? "available" : "unavailable"));
}

void EoEPlugin::onIpConfigured(int position, const QString &ip) {
    statusLog_->setText(tr("Slave %1 IP configured: %2").arg(position).arg(ip));
    currentIpLabel_->setText(ip);
}

void EoEPlugin::onIpReadback(int position, const QJsonObject &data) {
    const QString ip = data.value("ip").toString();
    const QString subnet = data.value("subnet").toString();
    const QString gw = data.value("gateway").toString();
    const QString dns = data.value("dns").toString();

    currentIpLabel_->setText(ip);
    ipEdit_->setText(ip);
    subnetEdit_->setText(subnet);
    gatewayEdit_->setText(gw);
    dnsEdit_->setText(dns);

    statusLog_->setText(tr("Slave %1 IP: %2/%3").arg(position).arg(ip).arg(subnet));
}

void EoEPlugin::onStatsReceived(int position, const QJsonObject &data) {
    txFramesLabel_->setText(QString::number(data.value("txFrames").toDouble()));
    rxFramesLabel_->setText(QString::number(data.value("rxFrames").toDouble()));
    txErrorsLabel_->setText(QString::number(data.value("txErrors").toDouble()));
    rxErrorsLabel_->setText(QString::number(data.value("rxErrors").toDouble()));

    statusLog_->setText(tr("Slave %1 stats updated").arg(position));
}

void EoEPlugin::onError(const QString &msg) {
    statusLog_->setText(tr("Error: %1").arg(msg));
    statusLog_->setStyleSheet("color: red; font-size: 11px;");
}
