#include "PdoMappingEditorPlugin.h"
#include "PdoMappingCanvas.h"
#include "PdoMappingValidator.h"
#include "services/PdoMappingService.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

PdoMappingEditorPlugin::PdoMappingEditorPlugin(PdoMappingService *pdoService, QObject *parent)
    : pdoService_(pdoService) {
  if (parent) setParent(parent);
  buildUi();
  populateSampleData();
}

QString PdoMappingEditorPlugin::id() const { return "pdomapping"; }
QString PdoMappingEditorPlugin::displayName() const { return "PDO Mapping"; }
QString PdoMappingEditorPlugin::displayNameZh() const { return "PDO 映射"; }
int PdoMappingEditorPlugin::defaultOrder() const { return 25; }
bool PdoMappingEditorPlugin::visible() const { return true; }

void PdoMappingEditorPlugin::activate() {}
void PdoMappingEditorPlugin::deactivate() {}

QWidget *PdoMappingEditorPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void PdoMappingEditorPlugin::setSlavePosition(int position) {
  slavePosition_ = position;
  if (slaveCombo_) {
    slaveCombo_->setCurrentIndex(position);
  }
  rebuildPdoTree();
}

int PdoMappingEditorPlugin::slavePosition() const { return slavePosition_; }

void PdoMappingEditorPlugin::addPdoEntry(int smIndex, const QString &index,
                                          const QString &subIndex, const QString &name,
                                          const QString &dataType, int bitSize, bool isOutput) {
  auto sms = canvas_->syncManagers();
  for (auto &sm : sms) {
    if (sm.index == smIndex) {
      PdoCanvasEntry entry;
      entry.index = index;
      entry.subIndex = subIndex;
      entry.name = name;
      entry.dataType = dataType;
      entry.bitSize = bitSize;
      entry.direction = isOutput ? PdoEntryDirection::Output : PdoEntryDirection::Input;
      entry.smIndex = smIndex;
      entry.enabled = true;
      sm.entries.append(entry);
      break;
    }
  }
  canvas_->setSyncManagers(sms);
  rebuildPdoTree();
  emit mappingChanged(slavePosition_);
}

void PdoMappingEditorPlugin::removePdoEntry(int smIndex, int entryIndex) {
  auto sms = canvas_->syncManagers();
  for (auto &sm : sms) {
    if (sm.index == smIndex && entryIndex >= 0 && entryIndex < sm.entries.size()) {
      sm.entries.removeAt(entryIndex);
      break;
    }
  }
  canvas_->setSyncManagers(sms);
  rebuildPdoTree();
  emit mappingChanged(slavePosition_);
}

int PdoMappingEditorPlugin::pdoEntryCount(int smIndex) const {
  auto sms = canvas_->syncManagers();
  for (const auto &sm : sms) {
    if (sm.index == smIndex) return sm.entries.size();
  }
  return 0;
}

void PdoMappingEditorPlugin::validateMapping() {
  auto sms = canvas_->syncManagers();
  auto report = PdoMappingValidator::validate(sms);
  hasErrors_ = !report.valid;
  errorCount_ = report.errors.size();

  canvas_->clearAllErrors();
  for (const auto &err : report.errors) {
    if (err.entryIndex >= 0) {
      canvas_->setErrorHighlight(err.smIndex, err.entryIndex, true);
    }
  }

  updateValidationDisplay();
  emit validationCompleted(report.valid, report.errors.size());
}

bool PdoMappingEditorPlugin::hasErrors() const { return hasErrors_; }
int PdoMappingEditorPlugin::errorCount() const { return errorCount_; }

bool PdoMappingEditorPlugin::exportMapping(const QString &filePath) {
  if (!pdoService_) return false;
  return pdoService_->exportMapping(slavePosition_, filePath);
}

bool PdoMappingEditorPlugin::importMapping(const QString &filePath) {
  if (!pdoService_) return false;
  bool ok = pdoService_->importMapping(slavePosition_, filePath);
  if (ok) {
    rebuildPdoTree();
    emit mappingChanged(slavePosition_);
  }
  return ok;
}

PdoMappingCanvas *PdoMappingEditorPlugin::canvas() const { return canvas_; }
QTreeWidget *PdoMappingEditorPlugin::pdoTree() const { return pdoTree_; }
QTableWidget *PdoMappingEditorPlugin::propertyTable() const { return propertyTable_; }
QTextEdit *PdoMappingEditorPlugin::validationPanel() const { return validationPanel_; }

void PdoMappingEditorPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  auto *toolbar = new QWidget;
  auto *toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(0, 0, 0, 0);

  slaveCombo_ = new QComboBox;
  slaveCombo_->addItem(tr("Slave 0"), 0);
  slaveCombo_->addItem(tr("Slave 1"), 1);
  slaveCombo_->addItem(tr("Slave 2"), 2);
  slaveCombo_->addItem(tr("Slave 3"), 3);
  toolbarLayout->addWidget(new QLabel(tr("Slave:")));
  toolbarLayout->addWidget(slaveCombo_);

  addEntryBtn_ = new QPushButton(tr("Add Entry"));
  removeEntryBtn_ = new QPushButton(tr("Remove Entry"));
  validateBtn_ = new QPushButton(tr("Validate"));
  exportBtn_ = new QPushButton(tr("Export"));
  importBtn_ = new QPushButton(tr("Import"));
  toolbarLayout->addWidget(addEntryBtn_);
  toolbarLayout->addWidget(removeEntryBtn_);
  toolbarLayout->addStretch();
  toolbarLayout->addWidget(validateBtn_);
  toolbarLayout->addWidget(exportBtn_);
  toolbarLayout->addWidget(importBtn_);
  mainLayout->addWidget(toolbar);

  mainSplitter_ = new QSplitter(Qt::Horizontal);

  pdoTree_ = new QTreeWidget;
  pdoTree_->setHeaderLabels({tr("PDO Entry"), tr("Index"), tr("Size")});
  pdoTree_->setMinimumWidth(200);
  mainSplitter_->addWidget(pdoTree_);

  canvas_ = new PdoMappingCanvas;
  mainSplitter_->addWidget(canvas_);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(0, 0, 0, 0);

  propertyTable_ = new QTableWidget;
  propertyTable_->setColumnCount(2);
  propertyTable_->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  propertyTable_->horizontalHeader()->setStretchLastSection(true);
  propertyTable_->setMinimumHeight(150);
  rightLayout->addWidget(new QLabel(tr("Properties")));
  rightLayout->addWidget(propertyTable_);

  validationPanel_ = new QTextEdit;
  validationPanel_->setReadOnly(true);
  validationPanel_->setMaximumHeight(150);
  validationPanel_->setPlaceholderText(tr("Click 'Validate' to check mapping..."));
  rightLayout->addWidget(new QLabel(tr("Validation")));
  rightLayout->addWidget(validationPanel_);

  mainSplitter_->addWidget(rightPanel);
  mainSplitter_->setSizes({200, 400, 250});
  mainLayout->addWidget(mainSplitter_);

  statusLabel_ = new QLabel(tr("Ready"));
  mainLayout->addWidget(statusLabel_);

  rebuildPdoTree();

  connect(slaveCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
    slavePosition_ = slaveCombo_->itemData(idx).toInt();
    rebuildPdoTree();
  });
  connect(validateBtn_, &QPushButton::clicked, this, &PdoMappingEditorPlugin::validateMapping);
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export PDO Mapping"), "",
                                                 tr("JSON Files (*.json)"));
    if (!path.isEmpty()) exportMapping(path);
  });
  connect(importBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getOpenFileName(containerWidget_, tr("Import PDO Mapping"), "",
                                                 tr("JSON Files (*.json)"));
    if (!path.isEmpty()) importMapping(path);
  });
  connect(addEntryBtn_, &QPushButton::clicked, this, [this]() {
    addPdoEntry(2, "0x6000", "0x00", tr("New Entry"), "UINT16", 16, false);
  });
  connect(removeEntryBtn_, &QPushButton::clicked, this, [this]() {
    int sm = canvas_->selectedSmIndex();
    int entry = canvas_->selectedEntryIndex();
    if (sm >= 0 && entry >= 0) {
      removePdoEntry(sm, entry);
    }
  });
  connect(canvas_, &PdoMappingCanvas::entrySelected, this,
          [this](int smIdx, int entryIdx) {
            auto sms = canvas_->syncManagers();
            for (const auto &sm : sms) {
              if (sm.index == smIdx && entryIdx >= 0 && entryIdx < sm.entries.size()) {
                const auto &e = sm.entries[entryIdx];
                propertyTable_->setRowCount(6);
                propertyTable_->setItem(0, 0, new QTableWidgetItem(tr("Index")));
                propertyTable_->setItem(0, 1, new QTableWidgetItem(e.index));
                propertyTable_->setItem(1, 0, new QTableWidgetItem(tr("SubIndex")));
                propertyTable_->setItem(1, 1, new QTableWidgetItem(e.subIndex));
                propertyTable_->setItem(2, 0, new QTableWidgetItem(tr("Name")));
                propertyTable_->setItem(2, 1, new QTableWidgetItem(e.name));
                propertyTable_->setItem(3, 0, new QTableWidgetItem(tr("Data Type")));
                propertyTable_->setItem(3, 1, new QTableWidgetItem(e.dataType));
                propertyTable_->setItem(4, 0, new QTableWidgetItem(tr("Bit Size")));
                propertyTable_->setItem(4, 1, new QTableWidgetItem(QString::number(e.bitSize)));
                propertyTable_->setItem(5, 0, new QTableWidgetItem(tr("Direction")));
                propertyTable_->setItem(5, 1, new QTableWidgetItem(
                    e.direction == PdoEntryDirection::Input ? tr("Input") : tr("Output")));
                break;
              }
            }
          });
}

void PdoMappingEditorPlugin::rebuildPdoTree() {
  if (!pdoTree_) return;
  pdoTree_->clear();
  auto sms = canvas_->syncManagers();
  for (const auto &sm : sms) {
    auto *smItem = new QTreeWidgetItem(pdoTree_);
    QString dirLabel = sm.direction == PdoEntryDirection::Input ? tr("IN") : tr("OUT");
    smItem->setText(0, tr("SM%1 [%2] %3").arg(sm.index).arg(dirLabel, sm.name));
    smItem->setExpanded(true);
    int totalBits = 0;
    for (const auto &entry : sm.entries) {
      auto *entryItem = new QTreeWidgetItem(smItem);
      entryItem->setText(0, entry.name);
      entryItem->setText(1, entry.index + "." + entry.subIndex);
      entryItem->setText(2, QString::number(entry.bitSize) + "b");
      totalBits += entry.bitSize;
    }
    smItem->setText(2, QString::number(totalBits) + "b");
  }
}

void PdoMappingEditorPlugin::rebuildPropertyTable() {
  if (!propertyTable_) return;
  propertyTable_->setRowCount(0);
}

void PdoMappingEditorPlugin::updateValidationDisplay() {
  if (!validationPanel_) return;
  auto sms = canvas_->syncManagers();
  auto report = PdoMappingValidator::validate(sms);

  QString text;
  if (report.valid) {
    text = tr("✓ Validation passed\n");
  } else {
    text = tr("✗ %1 error(s) found:\n").arg(report.errors.size());
    for (const auto &err : report.errors) {
      text += "  • " + PdoMappingValidator::errorString(err) + "\n";
    }
  }
  text += tr("\nInput: %1 bits | Output: %2 bits")
              .arg(report.totalInputBits)
              .arg(report.totalOutputBits);
  validationPanel_->setText(text);

  if (statusLabel_) {
    statusLabel_->setText(report.valid ? tr("Mapping valid") : tr("%1 errors").arg(report.errors.size()));
  }
}

void PdoMappingEditorPlugin::populateSampleData() {
  if (!canvas_) return;

  QVector<SyncManagerBlock> sms;

  SyncManagerBlock sm0;
  sm0.index = 0;
  sm0.name = tr("Mailbox Out");
  sm0.direction = PdoEntryDirection::Output;
  sm0.enabled = true;

  SyncManagerBlock sm1;
  sm1.index = 1;
  sm1.name = tr("Mailbox In");
  sm1.direction = PdoEntryDirection::Input;
  sm1.enabled = true;

  SyncManagerBlock sm2;
  sm2.index = 2;
  sm2.name = tr("RxPDO");
  sm2.direction = PdoEntryDirection::Output;
  sm2.enabled = true;
  sm2.entries = {
      {"0x6040", "0x00", tr("Control Word"), "UINT16", 16, PdoEntryDirection::Output, 2, true, false, false},
      {"0x607A", "0x00", tr("Target Position"), "INT32", 32, PdoEntryDirection::Output, 2, true, false, false},
      {"0x60FF", "0x00", tr("Target Velocity"), "INT32", 32, PdoEntryDirection::Output, 2, true, false, false},
      {"0x6071", "0x00", tr("Target Torque"), "INT16", 16, PdoEntryDirection::Output, 2, true, false, false},
  };

  SyncManagerBlock sm3;
  sm3.index = 3;
  sm3.name = tr("TxPDO");
  sm3.direction = PdoEntryDirection::Input;
  sm3.enabled = true;
  sm3.entries = {
      {"0x6041", "0x00", tr("Status Word"), "UINT16", 16, PdoEntryDirection::Input, 3, true, false, false},
      {"0x6064", "0x00", tr("Position Actual"), "INT32", 32, PdoEntryDirection::Input, 3, true, false, false},
      {"0x606C", "0x00", tr("Velocity Actual"), "INT32", 32, PdoEntryDirection::Input, 3, true, false, false},
      {"0x6077", "0x00", tr("Torque Actual"), "INT16", 16, PdoEntryDirection::Input, 3, true, false, false},
  };

  sms = {sm0, sm1, sm2, sm3};
  canvas_->setSyncManagers(sms);
  rebuildPdoTree();
}
