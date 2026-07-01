// ProtocolAnalyzerPlugin — implementation.  See header for interface documentation.
#include "ProtocolAnalyzerPlugin.h"
#include "ProtocolAnalyzerService.h"

#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QDateTime>
#include <QFile>

ProtocolAnalyzerPlugin::ProtocolAnalyzerPlugin(
    ProtocolAnalyzerService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &ProtocolAnalyzerService::frameCaptured,
          this, &ProtocolAnalyzerPlugin::onFrameCaptured);
  connect(service_, &ProtocolAnalyzerService::statisticsUpdated,
          this, &ProtocolAnalyzerPlugin::updateStatistics);
}

QString ProtocolAnalyzerPlugin::id() const { return "protocol"; }
QString ProtocolAnalyzerPlugin::displayName() const {
  return "Protocol Analyzer";
}
QString ProtocolAnalyzerPlugin::displayNameZh() const {
  return QStringLiteral("协议分析器");
}
int ProtocolAnalyzerPlugin::defaultOrder() const { return 105; }
bool ProtocolAnalyzerPlugin::visible() const { return false; }

QWidget *ProtocolAnalyzerPlugin::widget() { return container_; }

// ── UI construction ───────────────────────────────────────────────────

void ProtocolAnalyzerPlugin::buildUi() {
  container_ = new QWidget;
  auto *root = new QVBoxLayout(container_);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // ── Toolbar ──
  auto *toolbar = new QWidget;
  auto *tbLayout = new QHBoxLayout(toolbar);
  tbLayout->setContentsMargins(4, 4, 4, 4);

  captureBtn_ = new QPushButton(tr("Start Capture"));
  captureBtn_->setCheckable(true);
  tbLayout->addWidget(captureBtn_);

  clearBtn_ = new QPushButton(tr("Clear"));
  tbLayout->addWidget(clearBtn_);

  exportBtn_ = new QPushButton(tr("Export PCAP"));
  tbLayout->addWidget(exportBtn_);

  tbLayout->addStretch();
  root->addWidget(toolbar);

  // ── Filter bar ──
  auto *filterBar = new QWidget;
  auto *fbLayout = new QHBoxLayout(filterBar);
  fbLayout->setContentsMargins(4, 0, 4, 4);

  fbLayout->addWidget(new QLabel(tr("Type:")));
  filterTypeCombo_ = new QComboBox;
  filterTypeCombo_->addItems({"All", "EtherCAT", "CoE", "EoE", "FoE", "SoE"});
  fbLayout->addWidget(filterTypeCombo_);

  fbLayout->addWidget(new QLabel(tr("Direction:")));
  filterDirCombo_ = new QComboBox;
  filterDirCombo_->addItems({"All", "TX", "RX"});
  fbLayout->addWidget(filterDirCombo_);

  fbLayout->addWidget(new QLabel(tr("Slave:")));
  filterSlaveSpin_ = new QSpinBox;
  filterSlaveSpin_->setRange(-1, 255);
  filterSlaveSpin_->setValue(-1);
  filterSlaveSpin_->setSpecialValueText(tr("Any"));
  fbLayout->addWidget(filterSlaveSpin_);

  filterBtn_ = new QPushButton(tr("Apply Filter"));
  fbLayout->addWidget(filterBtn_);
  fbLayout->addStretch();
  root->addWidget(filterBar);

  // ── Frame table ──
  frameTable_ = new QTableWidget;
  frameTable_->setColumnCount(6);
  frameTable_->setHorizontalHeaderLabels(
      {tr("Time"), tr("Dir"), tr("Type"), tr("WKC"), tr("Len"), tr("Summary")});
  frameTable_->horizontalHeader()->setStretchLastSection(true);
  frameTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  frameTable_->setAlternatingRowColors(true);
  frameTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  root->addWidget(frameTable_, 1);

  // ── Stats bar ──
  statsLabel_ = new QLabel;
  statsLabel_->setFixedHeight(24);
  statsLabel_->setObjectName("protocolStatsLabel");
  root->addWidget(statsLabel_);

  // ── Connections ──
  connect(captureBtn_, &QPushButton::clicked, this,
          &ProtocolAnalyzerPlugin::toggleCapture);
  connect(clearBtn_, &QPushButton::clicked, this,
          &ProtocolAnalyzerPlugin::clearFrames);
  connect(filterBtn_, &QPushButton::clicked, this,
          &ProtocolAnalyzerPlugin::applyFilter);
  connect(exportBtn_, &QPushButton::clicked, this,
          &ProtocolAnalyzerPlugin::exportPcap);
}

// ── Actions ───────────────────────────────────────────────────────────

void ProtocolAnalyzerPlugin::toggleCapture() {
  if (service_->isCapturing()) {
    service_->stopCapture();
    captureBtn_->setText(tr("Start Capture"));
    captureBtn_->setChecked(false);
  } else {
    service_->startCapture();
    captureBtn_->setText(tr("Stop Capture"));
    captureBtn_->setChecked(true);
  }
}

void ProtocolAnalyzerPlugin::clearFrames() {
  service_->clearFrames();
  frameTable_->setRowCount(0);
  statsLabel_->clear();
}

void ProtocolAnalyzerPlugin::applyFilter() {
  ProtocolFilter f;
  const int typeIdx = filterTypeCombo_->currentIndex();
  f.enabled = (typeIdx != 0);
  if (typeIdx > 0) {
    f.frameType = static_cast<ProtocolFrameType>(typeIdx - 1);
  }
  const int dirIdx = filterDirCombo_->currentIndex();
  if (dirIdx == 1) f.direction = ProtocolDirection::TX;
  else if (dirIdx == 2) f.direction = ProtocolDirection::RX;

  f.slave = filterSlaveSpin_->value();
  service_->setFilter(f);
}

void ProtocolAnalyzerPlugin::onFrameCaptured(const ProtocolFrame &frame) {
  const int row = frameTable_->rowCount();
  frameTable_->insertRow(row);

  static const char *kTypeNames[] = {"EtherCAT", "CoE", "EoE", "FoE", "SoE", "?"};
  const int typeIdx = static_cast<int>(frame.frameType);

  frameTable_->setItem(row, 0, new QTableWidgetItem(
      QDateTime::fromMSecsSinceEpoch(frame.timestamp).toString("HH:mm:ss.zzz")));
  frameTable_->setItem(row, 1, new QTableWidgetItem(
      frame.direction == ProtocolDirection::TX ? "TX" : "RX"));
  frameTable_->setItem(row, 2, new QTableWidgetItem(kTypeNames[typeIdx]));
  frameTable_->setItem(row, 3, new QTableWidgetItem(QString::number(frame.wkc)));
  frameTable_->setItem(row, 4, new QTableWidgetItem(QString::number(frame.data.size())));
  frameTable_->setItem(row, 5, new QTableWidgetItem(frame.decodedSummary));

  // Auto-scroll to bottom.
  frameTable_->scrollToBottom();

  // Cap table rows.
  while (frameTable_->rowCount() > 5000) {
    frameTable_->removeRow(0);
  }
}

void ProtocolAnalyzerPlugin::updateStatistics(const ProtocolStatistics &stats) {
  statsLabel_->setText(QStringLiteral(
      "Frames: %1 | TX: %2 | RX: %3 | Errors: %4 | CoE: %5 | EoE: %6 | BW: %7 B/s")
      .arg(stats.totalFrames)
      .arg(stats.txFrames)
      .arg(stats.rxFrames)
      .arg(stats.errorFrames)
      .arg(stats.coeFrames)
      .arg(stats.eoeFrames)
      .arg(stats.bandwidthBps, 0, 'f', 1));
}

void ProtocolAnalyzerPlugin::exportPcap() {
  const QString path = QFileDialog::getSaveFileName(
      container_, tr("Export PCAP"), QString(), tr("PCAP files (*.pcap)"));
  if (path.isEmpty()) return;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly)) return;

  // PCAP global header.
  const quint32 magic = 0xa1b2c3d4;
  const quint16 major = 2, minor = 4;
  const quint32 thiszone = 0, sigfigs = 0, snaplen = 65535, network = 1;
  file.write(reinterpret_cast<const char *>(&magic), 4);
  file.write(reinterpret_cast<const char *>(&major), 2);
  file.write(reinterpret_cast<const char *>(&minor), 2);
  file.write(reinterpret_cast<const char *>(&thiszone), 4);
  file.write(reinterpret_cast<const char *>(&sigfigs), 4);
  file.write(reinterpret_cast<const char *>(&snaplen), 4);
  file.write(reinterpret_cast<const char *>(&network), 4);

  // Frame records.
  const auto frames = service_->getFrames(service_->frameCount());
  for (const auto &f : frames) {
    const quint32 ts_sec = static_cast<quint32>(f.timestamp / 1000);
    const quint32 ts_usec = static_cast<quint32>((f.timestamp % 1000) * 1000);
    const quint32 incl_len = static_cast<quint32>(f.data.size());
    const quint32 orig_len = incl_len;
    file.write(reinterpret_cast<const char *>(&ts_sec), 4);
    file.write(reinterpret_cast<const char *>(&ts_usec), 4);
    file.write(reinterpret_cast<const char *>(&incl_len), 4);
    file.write(reinterpret_cast<const char *>(&orig_len), 4);
    file.write(f.data);
  }
  file.close();
}
