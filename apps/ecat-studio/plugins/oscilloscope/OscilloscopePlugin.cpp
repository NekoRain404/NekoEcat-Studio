// OscilloscopePlugin — implementation.  See header for interface documentation.
#include "OscilloscopePlugin.h"
#include "OscilloscopeWidget.h"
#include "OscilloscopeService.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QGroupBox>

OscilloscopePlugin::OscilloscopePlugin(OscilloscopeService *service,
                                         QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &OscilloscopeService::waveformUpdated,
          this, &OscilloscopePlugin::refreshWaveforms);
  connect(service_, &OscilloscopeService::channelAdded,
          this, [this](int) { refreshWaveforms(); });
  connect(service_, &OscilloscopeService::channelRemoved,
          this, [this](int) { refreshWaveforms(); });
}

QString OscilloscopePlugin::id() const { return "oscilloscope"; }
QString OscilloscopePlugin::displayName() const { return "Oscilloscope"; }
QString OscilloscopePlugin::displayNameZh() const {
  return QStringLiteral("示波器");
}
int OscilloscopePlugin::defaultOrder() const { return 100; }
bool OscilloscopePlugin::visible() const { return true; }

QWidget *OscilloscopePlugin::widget() { return container_; }

// ── UI construction ───────────────────────────────────────────────────

void OscilloscopePlugin::buildUi() {
  container_ = new QWidget;
  auto *root = new QHBoxLayout(container_);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // ── Left panel: channel list + controls ──
  auto *leftPanel = new QWidget;
  leftPanel->setFixedWidth(220);
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  leftLayout->addWidget(new QLabel(tr("Channels")));

  channelList_ = new QListWidget;
  leftLayout->addWidget(channelList_);

  auto *btnRow = new QWidget;
  auto *btnLayout = new QHBoxLayout(btnRow);
  btnLayout->setContentsMargins(0, 0, 0, 0);
  auto *addBtn = new QPushButton(tr("+"));
  addBtn->setToolTip(tr("Add channel"));
  auto *removeBtn = new QPushButton(tr("-"));
  removeBtn->setToolTip(tr("Remove selected"));
  btnLayout->addWidget(addBtn);
  btnLayout->addWidget(removeBtn);
  leftLayout->addWidget(btnRow);

  // Timebase.
  auto *tbGroup = new QGroupBox(tr("Timebase"));
  auto *tbLayout = new QVBoxLayout(tbGroup);
  timebaseCombo_ = new QComboBox;
  timebaseCombo_->addItems({"1 ms", "2 ms", "5 ms", "10 ms", "20 ms",
                             "50 ms", "100 ms", "500 ms", "1 s", "5 s", "10 s"});
  timebaseCombo_->setCurrentIndex(2);
  tbLayout->addWidget(timebaseCombo_);
  leftLayout->addWidget(tbGroup);

  // Trigger.
  auto *trigGroup = new QGroupBox(tr("Trigger"));
  auto *trigLayout = new QVBoxLayout(trigGroup);
  triggerModeCombo_ = new QComboBox;
  triggerModeCombo_->addItems({tr("Auto"), tr("Normal"), tr("Single")});
  trigLayout->addWidget(triggerModeCombo_);
  triggerLevelSpin_ = new QSpinBox;
  triggerLevelSpin_->setRange(-100, 100);
  triggerLevelSpin_->setValue(0);
  triggerLevelSpin_->setSuffix(tr(" %"));
  trigLayout->addWidget(triggerLevelSpin_);
  leftLayout->addWidget(trigGroup);

  // Run/Stop + Cursor.
  runStopBtn_ = new QPushButton(tr("Run"));
  runStopBtn_->setCheckable(true);
  leftLayout->addWidget(runStopBtn_);

  cursorBtn_ = new QPushButton(tr("Cursor"));
  cursorBtn_->setCheckable(true);
  leftLayout->addWidget(cursorBtn_);

  cursorLabel_ = new QLabel;
  cursorLabel_->setWordWrap(true);
  leftLayout->addWidget(cursorLabel_);

  leftLayout->addStretch();
  root->addWidget(leftPanel);

  // ── Center: scope display ──
  scope_ = new OscilloscopeWidget;
  root->addWidget(scope_, 1);

  // ── Connections ──
  connect(addBtn, &QPushButton::clicked, this,
          &OscilloscopePlugin::showAddChannelDialog);
  connect(removeBtn, &QPushButton::clicked, this,
          &OscilloscopePlugin::removeSelectedChannel);
  connect(runStopBtn_, &QPushButton::clicked, this,
          &OscilloscopePlugin::toggleAcquisition);
  connect(timebaseCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &OscilloscopePlugin::onTimebaseChanged);
  connect(triggerModeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &OscilloscopePlugin::onTriggerModeChanged);
  connect(triggerLevelSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
          this, [this](int val) { service_->setTriggerLevel(val); });
  connect(cursorBtn_, &QPushButton::clicked, this, [this](bool checked) {
    cursorActive_ = checked;
    scope_->setCursorEnabled(checked);
    if (!checked) cursorLabel_->clear();
  });
}

// ── Channel management ────────────────────────────────────────────────

void OscilloscopePlugin::showAddChannelDialog() {
  QDialog dlg(container_);
  dlg.setWindowTitle(tr("Add Oscilloscope Channel"));
  auto *form = new QFormLayout(&dlg);

  auto *slaveSpin = new QSpinBox;
  slaveSpin->setRange(0, 255);
  form->addRow(tr("Slave"), slaveSpin);

  auto *idxEdit = new QLineEdit;
  idxEdit->setPlaceholderText(tr("e.g. 0x6064"));
  form->addRow(tr("Index"), idxEdit);

  auto *subEdit = new QLineEdit;
  subEdit->setText("0");
  form->addRow(tr("SubIndex"), subEdit);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  form->addRow(buttons);
  connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

  if (dlg.exec() == QDialog::Accepted) {
    const int id = service_->addChannel(slaveSpin->value(),
                                         idxEdit->text().trimmed(),
                                         subEdit->text().trimmed());
    if (id > 0) {
      const auto chs = service_->channels();
      const auto &ch = chs.last();
      channelList_->addItem(QStringLiteral("%1 [CH%2]").arg(ch.name).arg(id));
      refreshWaveforms();
    }
  }
}

void OscilloscopePlugin::removeSelectedChannel() {
  const int row = channelList_->currentRow();
  if (row < 0) return;
  const auto chs = service_->channels();
  if (row >= chs.size()) return;
  service_->removeChannel(chs[row].id);
  delete channelList_->takeItem(row);
  refreshWaveforms();
}

void OscilloscopePlugin::toggleAcquisition() {
  if (service_->isAcquiring()) {
    service_->stopAcquisition();
    runStopBtn_->setText(tr("Run"));
    runStopBtn_->setChecked(false);
  } else {
    service_->startAcquisition();
    runStopBtn_->setText(tr("Stop"));
    runStopBtn_->setChecked(true);
  }
}

// ── Waveform refresh ──────────────────────────────────────────────────

void OscilloscopePlugin::refreshWaveforms() {
  const auto chs = service_->channels();
  QVector<OscilloscopeWidget::ChannelData> display;
  display.reserve(chs.size());

  for (int i = 0; i < chs.size(); ++i) {
    OscilloscopeWidget::ChannelData cd;
    cd.name = chs[i].name;
    cd.color = OscilloscopeWidget::kColors[i % OscilloscopeWidget::kColorCount];
    cd.samples = chs[i].data;
    display.append(cd);
  }
  scope_->setChannelData(display);
  if (cursorActive_) updateCursorReadout();
}

void OscilloscopePlugin::onTimebaseChanged(int index) {
  static const int kMsPerDiv[] = {1, 2, 5, 10, 20, 50, 100, 500, 1000, 5000, 10000};
  if (index >= 0 && index < 11) {
    service_->setTimebase(kMsPerDiv[index]);
  }
}

void OscilloscopePlugin::onTriggerModeChanged(int index) {
  service_->setTriggerMode(static_cast<OscTriggerMode>(index));
}

void OscilloscopePlugin::updateCursorReadout() {
  // Placeholder — will be populated with actual cursor measurements.
  cursorLabel_->setText(tr("Cursor: move mouse over waveform"));
}
