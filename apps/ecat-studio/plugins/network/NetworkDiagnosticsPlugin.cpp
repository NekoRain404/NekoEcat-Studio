#include "NetworkDiagnosticsPlugin.h"
#include "services/NetworkDiagnosticsService.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>

NetworkDiagnosticsPlugin::NetworkDiagnosticsPlugin(
    NetworkDiagnosticsService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &NetworkDiagnosticsService::healthUpdated, this,
          [this](const NetworkHealth &) { updateDisplay(); });
}

QString NetworkDiagnosticsPlugin::id() const { return "network"; }
QString NetworkDiagnosticsPlugin::displayName() const {
  return "Network Diagnostics";
}
QString NetworkDiagnosticsPlugin::displayNameZh() const {
  return QStringLiteral("网络诊断");
}
QIcon NetworkDiagnosticsPlugin::icon() const {
  return QIcon::fromTheme("network-wired");
}
int NetworkDiagnosticsPlugin::defaultOrder() const { return 135; }
bool NetworkDiagnosticsPlugin::visible() const { return true; }

void NetworkDiagnosticsPlugin::activate() {}
void NetworkDiagnosticsPlugin::deactivate() {}

QWidget *NetworkDiagnosticsPlugin::widget() { return containerWidget_; }

void NetworkDiagnosticsPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(10);

  auto *toolbar = new QHBoxLayout;
  toolbar->setSpacing(8);

  startStopBtn_ = new QPushButton(tr("Start Monitoring"));
  toolbar->addWidget(startStopBtn_);

  resetBtn_ = new QPushButton(tr("Reset Counters"));
  toolbar->addWidget(resetBtn_);

  exportBtn_ = new QPushButton(tr("Export Report"));
  toolbar->addWidget(exportBtn_);

  statusLabel_ = new QLabel(tr("Stopped"));
  toolbar->addWidget(statusLabel_);

  toolbar->addStretch();

  healthLabel_ = new QLabel(tr("Health: --"));
  toolbar->addWidget(healthLabel_);

  bandwidthLabel_ = new QLabel(tr("Bandwidth: --"));
  toolbar->addWidget(bandwidthLabel_);

  latencyLabel_ = new QLabel(tr("Latency: --"));
  toolbar->addWidget(latencyLabel_);

  layout->addLayout(toolbar);

  portTable_ = new QTableWidget;
  portTable_->setColumnCount(5);
  portTable_->setHorizontalHeaderLabels(
      {tr("Port"), tr("Link"), tr("Speed"), tr("Duplex"), tr("Errors")});
  portTable_->horizontalHeader()->setStretchLastSection(true);
  portTable_->verticalHeader()->setVisible(false);
  portTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  portTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  portTable_->setShowGrid(false);
  portTable_->setAlternatingRowColors(true);
  layout->addWidget(portTable_, 1);

  errorTable_ = new QTableWidget;
  errorTable_->setColumnCount(2);
  errorTable_->setHorizontalHeaderLabels({tr("Counter"), tr("Value")});
  errorTable_->horizontalHeader()->setStretchLastSection(true);
  errorTable_->verticalHeader()->setVisible(false);
  errorTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  errorTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  errorTable_->setShowGrid(false);
  errorTable_->setAlternatingRowColors(true);

  auto addCounter = [&](const QString &name) {
    int r = errorTable_->rowCount();
    errorTable_->insertRow(r);
    errorTable_->setItem(r, 0, new QTableWidgetItem(name));
    errorTable_->setItem(r, 1, new QTableWidgetItem("0"));
  };
  addCounter(tr("CRC Errors"));
  addCounter(tr("Frame Errors"));
  addCounter(tr("Lost Frames"));
  addCounter(tr("Overrun Errors"));

  layout->addWidget(errorTable_);

  connect(startStopBtn_, &QPushButton::clicked, this, [this]() {
    if (service_->isMonitoring()) {
      service_->stopMonitoring();
      startStopBtn_->setText(tr("Start Monitoring"));
      statusLabel_->setText(tr("Stopped"));
    } else {
      service_->startMonitoring();
      startStopBtn_->setText(tr("Stop Monitoring"));
      statusLabel_->setText(tr("Running"));
    }
  });

  connect(resetBtn_, &QPushButton::clicked, this, [this]() {
    service_->resetErrorCounters();
    updateDisplay();
  });

  connect(exportBtn_, &QPushButton::clicked, this,
          &NetworkDiagnosticsPlugin::exportReport);
}

void NetworkDiagnosticsPlugin::updateDisplay() {
  auto health = service_->currentHealth();
  auto errors = service_->errorCounters();

  portTable_->setRowCount(0);
  auto ports = service_->allPortStatus();
  for (const auto &p : ports) {
    int r = portTable_->rowCount();
    portTable_->insertRow(r);
    portTable_->setItem(r, 0, new QTableWidgetItem(QString::number(p.port)));
    portTable_->setItem(r, 1,
                        new QTableWidgetItem(p.linkUp ? tr("Up") : tr("Down")));
    portTable_->setItem(
        r, 2, new QTableWidgetItem(QString::number(p.speedMbps) + " Mbps"));
    portTable_->setItem(
        r, 3, new QTableWidgetItem(p.fullDuplex ? tr("Full") : tr("Half")));
    portTable_->setItem(r, 4,
                        new QTableWidgetItem(QString::number(p.errorCount)));
  }

  auto setError = [&](int row, const QString &val) {
    if (auto *item = errorTable_->item(row, 1)) item->setText(val);
  };
  setError(0, QString::number(errors.crc));
  setError(1, QString::number(errors.frame));
  setError(2, QString::number(errors.lost));
  setError(3, QString::number(errors.overrun));

  QString color;
  switch (health.overall) {
  case NetworkHealth::Status::Unknown:
    color = "#9ca3af";
    break;
  case NetworkHealth::Status::Good:
    color = "#22c55e";
    break;
  case NetworkHealth::Status::Degraded:
    color = "#f59e0b";
    break;
  case NetworkHealth::Status::Critical:
    color = "#ef4444";
    break;
  }
  healthLabel_->setText(
      QStringLiteral("<span style='color:%1'>%2: %3/%4 active</span>")
          .arg(color, tr("Health"), QString::number(health.activePorts),
               QString::number(health.portCount)));

  bandwidthLabel_->setText(
      tr("Bandwidth: %1%")
          .arg(QString::number(health.bandwidth * 100.0, 'f', 1)));
  latencyLabel_->setText(
      tr("Latency: %1 ms / Jitter: %2 ms")
          .arg(QString::number(health.latencyMs, 'f', 2),
               QString::number(health.jitterMs, 'f', 2)));
}

void NetworkDiagnosticsPlugin::exportReport() {
  QString path = QFileDialog::getSaveFileName(
      nullptr, tr("Export Diagnostics Report"), "network_diagnostics.csv",
      tr("CSV Files (*.csv)"));
  if (path.isEmpty()) return;

  exportReportToFile(path);
}

bool NetworkDiagnosticsPlugin::exportReportToFile(const QString &path) {
  if (path.isEmpty()) return false;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&file);
  out << "Port,Link,Speed,Duplex,Errors\n";
  auto ports = service_->allPortStatus();
  for (const auto &p : ports) {
    out << p.port << "," << (p.linkUp ? "Up" : "Down") << ","
        << p.speedMbps << " Mbps," << (p.fullDuplex ? "Full" : "Half") << ","
        << p.errorCount << "\n";
  }

  auto errors = service_->errorCounters();
  out << "\nCounter,Value\n";
  out << "CRC Errors," << errors.crc << "\n";
  out << "Frame Errors," << errors.frame << "\n";
  out << "Lost Frames," << errors.lost << "\n";
  out << "Overrun Errors," << errors.overrun << "\n";
  return out.status() == QTextStream::Ok && file.flush();
}
