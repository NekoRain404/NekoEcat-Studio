#include "QuantumSecurityPlugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRandomGenerator>

QuantumSecurityPlugin::QuantumSecurityPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString QuantumSecurityPlugin::id() const { return "quantumsecurity"; }
QString QuantumSecurityPlugin::displayName() const { return "Quantum Security"; }
QString QuantumSecurityPlugin::displayNameZh() const { return "量子安全"; }
int QuantumSecurityPlugin::defaultOrder() const { return 380; }
bool QuantumSecurityPlugin::visible() const { return true; }

void QuantumSecurityPlugin::activate() {}
void QuantumSecurityPlugin::deactivate() {}

QWidget *QuantumSecurityPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void QuantumSecurityPlugin::addKey(const QuantumKey &key) {
  keys_.append(key);
  rebuildKeyTable();
  emit keyGenerated(key.id);
}

void QuantumSecurityPlugin::removeKey(int index) {
  if (index >= 0 && index < keys_.size()) {
    keys_.removeAt(index);
    rebuildKeyTable();
  }
}

int QuantumSecurityPlugin::keyCount() const { return keys_.size(); }

void QuantumSecurityPlugin::addRandom(const QuantumRandom &random) {
  randoms_.append(random);
  rebuildRandomTable();
  emit randomGenerated(random.id);
}

void QuantumSecurityPlugin::removeRandom(int index) {
  if (index >= 0 && index < randoms_.size()) {
    randoms_.removeAt(index);
    rebuildRandomTable();
  }
}

int QuantumSecurityPlugin::randomCount() const { return randoms_.size(); }

void QuantumSecurityPlugin::addEncryption(const QuantumEncryption &encryption) {
  encryptions_.append(encryption);
  rebuildEncryptionTable();
  emit encryptionCompleted(encryption.id, encryption.success);
}

void QuantumSecurityPlugin::removeEncryption(int index) {
  if (index >= 0 && index < encryptions_.size()) {
    encryptions_.removeAt(index);
    rebuildEncryptionTable();
  }
}

int QuantumSecurityPlugin::encryptionCount() const { return encryptions_.size(); }

void QuantumSecurityPlugin::addSignature(const QuantumSignature &signature) {
  signatures_.append(signature);
  rebuildSignatureTable();
}

void QuantumSecurityPlugin::removeSignature(int index) {
  if (index >= 0 && index < signatures_.size()) {
    signatures_.removeAt(index);
    rebuildSignatureTable();
  }
}

int QuantumSecurityPlugin::signatureCount() const { return signatures_.size(); }

void QuantumSecurityPlugin::verifySignature(int index) {
  if (index >= 0 && index < signatures_.size()) {
    signatures_[index].verified = true;
    rebuildSignatureTable();
    emit signatureVerified(signatures_[index].id, true);
  }
}

QString QuantumSecurityPlugin::exportReport() const {
  QJsonObject root;

  QJsonArray keysArr;
  for (const auto &k : keys_) {
    QJsonObject obj;
    obj["id"] = k.id;
    obj["algorithm"] = k.algorithm;
    obj["keySize"] = k.keySize;
    obj["createdAt"] = k.createdAt.toString(Qt::ISODate);
    obj["expiresAt"] = k.expiresAt.toString(Qt::ISODate);
    obj["active"] = k.active;
    keysArr.append(obj);
  }
  root["keys"] = keysArr;

  QJsonArray randomArr;
  for (const auto &r : randoms_) {
    QJsonObject obj;
    obj["id"] = r.id;
    obj["bitLength"] = r.bitLength;
    obj["entropy"] = r.entropy;
    obj["generatedAt"] = r.generatedAt.toString(Qt::ISODate);
    obj["source"] = r.source;
    randomArr.append(obj);
  }
  root["randoms"] = randomArr;

  QJsonArray encArr;
  for (const auto &e : encryptions_) {
    QJsonObject obj;
    obj["id"] = e.id;
    obj["algorithm"] = e.algorithm;
    obj["inputHash"] = e.inputHash;
    obj["outputHash"] = e.outputHash;
    obj["timestamp"] = e.timestamp.toString(Qt::ISODate);
    obj["success"] = e.success;
    encArr.append(obj);
  }
  root["encryptions"] = encArr;

  QJsonArray sigArr;
  for (const auto &s : signatures_) {
    QJsonObject obj;
    obj["id"] = s.id;
    obj["signer"] = s.signer;
    obj["messageHash"] = s.messageHash;
    obj["signature"] = s.signature;
    obj["timestamp"] = s.timestamp.toString(Qt::ISODate);
    obj["verified"] = s.verified;
    sigArr.append(obj);
  }
  root["signatures"] = sigArr;

  return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QTabWidget *QuantumSecurityPlugin::tabs() const { return tabs_; }
QTableWidget *QuantumSecurityPlugin::keyTable() const { return keyTable_; }
QTableWidget *QuantumSecurityPlugin::randomTable() const { return randomTable_; }
QTableWidget *QuantumSecurityPlugin::encryptionTable() const { return encryptionTable_; }
QTableWidget *QuantumSecurityPlugin::signatureTable() const { return signatureTable_; }
QLabel *QuantumSecurityPlugin::statusLabel() const { return statusLabel_; }

void QuantumSecurityPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  tabs_ = new QTabWidget;

  auto *keyTab = new QWidget;
  auto *keyLayout = new QVBoxLayout(keyTab);
  keyTable_ = new QTableWidget;
  keyTable_->setColumnCount(6);
  keyTable_->setHorizontalHeaderLabels({"ID", "Algorithm", "Key Size", "Created", "Expires", "Active"});
  keyTable_->horizontalHeader()->setStretchLastSection(true);
  keyLayout->addWidget(keyTable_);
  auto *keyBtnRow = new QWidget;
  auto *keyBtnLayout = new QHBoxLayout(keyBtnRow);
  addKeyBtn_ = new QPushButton("Add");
  removeKeyBtn_ = new QPushButton("Remove");
  keyBtnLayout->addWidget(addKeyBtn_);
  keyBtnLayout->addWidget(removeKeyBtn_);
  keyLayout->addWidget(keyBtnRow);
  tabs_->addTab(keyTab, "Quantum Keys");

  auto *randomTab = new QWidget;
  auto *randomLayout = new QVBoxLayout(randomTab);
  randomTable_ = new QTableWidget;
  randomTable_->setColumnCount(5);
  randomTable_->setHorizontalHeaderLabels({"ID", "Bit Length", "Entropy", "Generated", "Source"});
  randomTable_->horizontalHeader()->setStretchLastSection(true);
  randomLayout->addWidget(randomTable_);
  auto *randomBtnRow = new QWidget;
  auto *randomBtnLayout = new QHBoxLayout(randomBtnRow);
  generateRandomBtn_ = new QPushButton("Generate");
  removeRandomBtn_ = new QPushButton("Remove");
  randomBtnLayout->addWidget(generateRandomBtn_);
  randomBtnLayout->addWidget(removeRandomBtn_);
  randomLayout->addWidget(randomBtnRow);
  tabs_->addTab(randomTab, "Quantum Random");

  auto *encTab = new QWidget;
  auto *encLayout = new QVBoxLayout(encTab);
  encryptionTable_ = new QTableWidget;
  encryptionTable_->setColumnCount(6);
  encryptionTable_->setHorizontalHeaderLabels({"ID", "Algorithm", "Input Hash", "Output Hash", "Time", "Success"});
  encryptionTable_->horizontalHeader()->setStretchLastSection(true);
  encLayout->addWidget(encryptionTable_);
  auto *encBtnRow = new QWidget;
  auto *encBtnLayout = new QHBoxLayout(encBtnRow);
  addEncryptionBtn_ = new QPushButton("Encrypt");
  removeEncryptionBtn_ = new QPushButton("Remove");
  encBtnLayout->addWidget(addEncryptionBtn_);
  encBtnLayout->addWidget(removeEncryptionBtn_);
  encLayout->addWidget(encBtnRow);
  tabs_->addTab(encTab, "Quantum Encryption");

  auto *sigTab = new QWidget;
  auto *sigLayout = new QVBoxLayout(sigTab);
  signatureTable_ = new QTableWidget;
  signatureTable_->setColumnCount(6);
  signatureTable_->setHorizontalHeaderLabels({"ID", "Signer", "Message Hash", "Signature", "Time", "Verified"});
  signatureTable_->horizontalHeader()->setStretchLastSection(true);
  sigLayout->addWidget(signatureTable_);
  auto *sigBtnRow = new QWidget;
  auto *sigBtnLayout = new QHBoxLayout(sigBtnRow);
  addSignatureBtn_ = new QPushButton("Add");
  removeSignatureBtn_ = new QPushButton("Remove");
  verifySignatureBtn_ = new QPushButton("Verify");
  sigBtnLayout->addWidget(addSignatureBtn_);
  sigBtnLayout->addWidget(removeSignatureBtn_);
  sigBtnLayout->addWidget(verifySignatureBtn_);
  sigLayout->addWidget(sigBtnRow);
  tabs_->addTab(sigTab, "Quantum Signatures");

  mainLayout->addWidget(tabs_);

  exportBtn_ = new QPushButton("Export Security Report");
  mainLayout->addWidget(exportBtn_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  connect(addKeyBtn_, &QPushButton::clicked, this, [this]() {
    QuantumKey k;
    k.id = "qkey_" + QString::number(keys_.size());
    k.algorithm = "BB84";
    k.keySize = 256;
    k.createdAt = QDateTime::currentDateTime();
    k.expiresAt = QDateTime::currentDateTime().addDays(30);
    k.active = true;
    addKey(k);
  });
  connect(removeKeyBtn_, &QPushButton::clicked, this, [this]() {
    int row = keyTable_->currentRow();
    if (row >= 0) removeKey(row);
  });
  connect(generateRandomBtn_, &QPushButton::clicked, this, [this]() {
    QuantumRandom r;
    r.id = "qrng_" + QString::number(randoms_.size());
    r.bitLength = 256;
    r.entropy = "quantum_vacuum_fluctuation";
    r.generatedAt = QDateTime::currentDateTime();
    r.source = "QRNG-Device";
    addRandom(r);
  });
  connect(removeRandomBtn_, &QPushButton::clicked, this, [this]() {
    int row = randomTable_->currentRow();
    if (row >= 0) removeRandom(row);
  });
  connect(addEncryptionBtn_, &QPushButton::clicked, this, [this]() {
    QuantumEncryption e;
    e.id = "qenc_" + QString::number(encryptions_.size());
    e.algorithm = "AES-256-QKD";
    e.inputHash = "0x" + QString::number(QRandomGenerator::global()->bounded(0xFFFFFF), 16).rightJustified(6, '0');
    e.outputHash = "0x" + QString::number(QRandomGenerator::global()->bounded(0xFFFFFF), 16).rightJustified(6, '0');
    e.timestamp = QDateTime::currentDateTime();
    e.success = true;
    addEncryption(e);
  });
  connect(removeEncryptionBtn_, &QPushButton::clicked, this, [this]() {
    int row = encryptionTable_->currentRow();
    if (row >= 0) removeEncryption(row);
  });
  connect(addSignatureBtn_, &QPushButton::clicked, this, [this]() {
    QuantumSignature s;
    s.id = "qsig_" + QString::number(signatures_.size());
    s.signer = "quantum_signer_0";
    s.messageHash = "0x" + QString::number(QRandomGenerator::global()->bounded(0xFFFFFF), 16).rightJustified(6, '0');
    s.signature = "0x" + QString::number(QRandomGenerator::global()->bounded(0xFFFFFF), 16).rightJustified(6, '0');
    s.timestamp = QDateTime::currentDateTime();
    s.verified = false;
    addSignature(s);
  });
  connect(removeSignatureBtn_, &QPushButton::clicked, this, [this]() {
    int row = signatureTable_->currentRow();
    if (row >= 0) removeSignature(row);
  });
  connect(verifySignatureBtn_, &QPushButton::clicked, this, [this]() {
    int row = signatureTable_->currentRow();
    if (row >= 0) verifySignature(row);
  });
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    exportReport();
  });
}

void QuantumSecurityPlugin::rebuildKeyTable() {
  if (!keyTable_) return;
  keyTable_->setRowCount(keys_.size());
  for (int i = 0; i < keys_.size(); ++i) {
    const auto &k = keys_[i];
    keyTable_->setItem(i, 0, new QTableWidgetItem(k.id));
    keyTable_->setItem(i, 1, new QTableWidgetItem(k.algorithm));
    keyTable_->setItem(i, 2, new QTableWidgetItem(QString::number(k.keySize)));
    keyTable_->setItem(i, 3, new QTableWidgetItem(k.createdAt.toString(Qt::ISODate)));
    keyTable_->setItem(i, 4, new QTableWidgetItem(k.expiresAt.toString(Qt::ISODate)));
    keyTable_->setItem(i, 5, new QTableWidgetItem(k.active ? "Yes" : "No"));
  }
}

void QuantumSecurityPlugin::rebuildRandomTable() {
  if (!randomTable_) return;
  randomTable_->setRowCount(randoms_.size());
  for (int i = 0; i < randoms_.size(); ++i) {
    const auto &r = randoms_[i];
    randomTable_->setItem(i, 0, new QTableWidgetItem(r.id));
    randomTable_->setItem(i, 1, new QTableWidgetItem(QString::number(r.bitLength)));
    randomTable_->setItem(i, 2, new QTableWidgetItem(r.entropy));
    randomTable_->setItem(i, 3, new QTableWidgetItem(r.generatedAt.toString(Qt::ISODate)));
    randomTable_->setItem(i, 4, new QTableWidgetItem(r.source));
  }
}

void QuantumSecurityPlugin::rebuildEncryptionTable() {
  if (!encryptionTable_) return;
  encryptionTable_->setRowCount(encryptions_.size());
  for (int i = 0; i < encryptions_.size(); ++i) {
    const auto &e = encryptions_[i];
    encryptionTable_->setItem(i, 0, new QTableWidgetItem(e.id));
    encryptionTable_->setItem(i, 1, new QTableWidgetItem(e.algorithm));
    encryptionTable_->setItem(i, 2, new QTableWidgetItem(e.inputHash));
    encryptionTable_->setItem(i, 3, new QTableWidgetItem(e.outputHash));
    encryptionTable_->setItem(i, 4, new QTableWidgetItem(e.timestamp.toString(Qt::ISODate)));
    encryptionTable_->setItem(i, 5, new QTableWidgetItem(e.success ? "Yes" : "No"));
  }
}

void QuantumSecurityPlugin::rebuildSignatureTable() {
  if (!signatureTable_) return;
  signatureTable_->setRowCount(signatures_.size());
  for (int i = 0; i < signatures_.size(); ++i) {
    const auto &s = signatures_[i];
    signatureTable_->setItem(i, 0, new QTableWidgetItem(s.id));
    signatureTable_->setItem(i, 1, new QTableWidgetItem(s.signer));
    signatureTable_->setItem(i, 2, new QTableWidgetItem(s.messageHash));
    signatureTable_->setItem(i, 3, new QTableWidgetItem(s.signature));
    signatureTable_->setItem(i, 4, new QTableWidgetItem(s.timestamp.toString(Qt::ISODate)));
    signatureTable_->setItem(i, 5, new QTableWidgetItem(s.verified ? "Yes" : "No"));
  }
}
