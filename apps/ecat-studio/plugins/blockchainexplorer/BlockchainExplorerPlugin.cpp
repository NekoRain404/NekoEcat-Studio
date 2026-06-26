#include "BlockchainExplorerPlugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

BlockchainExplorerPlugin::BlockchainExplorerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString BlockchainExplorerPlugin::id() const { return "blockchainexplorer"; }
QString BlockchainExplorerPlugin::displayName() const { return "Blockchain Explorer"; }
QString BlockchainExplorerPlugin::displayNameZh() const { return "区块链浏览器"; }
int BlockchainExplorerPlugin::defaultOrder() const { return 375; }
bool BlockchainExplorerPlugin::visible() const { return true; }

void BlockchainExplorerPlugin::activate() {}
void BlockchainExplorerPlugin::deactivate() {}

QWidget *BlockchainExplorerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void BlockchainExplorerPlugin::addTransaction(const Transaction &tx) {
  transactions_.append(tx);
  filteredTransactions_ = transactions_;
  rebuildTransactionTable();
  emit transactionAdded(tx.hash);
}

void BlockchainExplorerPlugin::removeTransaction(int index) {
  if (index >= 0 && index < transactions_.size()) {
    transactions_.removeAt(index);
    filteredTransactions_ = transactions_;
    rebuildTransactionTable();
  }
}

int BlockchainExplorerPlugin::transactionCount() const { return transactions_.size(); }

void BlockchainExplorerPlugin::addBlock(const BlockInfo &block) {
  blocks_.append(block);
  rebuildBlockTable();
  emit blockAdded(block.number);
}

void BlockchainExplorerPlugin::removeBlock(int index) {
  if (index >= 0 && index < blocks_.size()) {
    blocks_.removeAt(index);
    rebuildBlockTable();
  }
}

int BlockchainExplorerPlugin::blockCount() const { return blocks_.size(); }

void BlockchainExplorerPlugin::addSmartContract(const SmartContract &contract) {
  contracts_.append(contract);
  rebuildContractTable();
}

void BlockchainExplorerPlugin::removeSmartContract(int index) {
  if (index >= 0 && index < contracts_.size()) {
    contracts_.removeAt(index);
    rebuildContractTable();
  }
}

int BlockchainExplorerPlugin::smartContractCount() const { return contracts_.size(); }

void BlockchainExplorerPlugin::addSupplyChainEntry(const SupplyChainEntry &entry) {
  supplyChain_.append(entry);
  rebuildSupplyChainTable();
  emit supplyChainUpdated(entry.itemId);
}

void BlockchainExplorerPlugin::removeSupplyChainEntry(int index) {
  if (index >= 0 && index < supplyChain_.size()) {
    supplyChain_.removeAt(index);
    rebuildSupplyChainTable();
  }
}

int BlockchainExplorerPlugin::supplyChainEntryCount() const { return supplyChain_.size(); }

void BlockchainExplorerPlugin::filterTransactions(const QString &query) {
  if (query.isEmpty()) {
    filteredTransactions_ = transactions_;
  } else {
    filteredTransactions_.clear();
    for (const auto &tx : transactions_) {
      if (tx.hash.contains(query, Qt::CaseInsensitive) ||
          tx.from.contains(query, Qt::CaseInsensitive) ||
          tx.to.contains(query, Qt::CaseInsensitive) ||
          tx.status.contains(query, Qt::CaseInsensitive)) {
        filteredTransactions_.append(tx);
      }
    }
  }
  rebuildTransactionTable();
  if (statusLabel_) statusLabel_->setText(tr("%1 transactions").arg(filteredTransactions_.size()));
}

QString BlockchainExplorerPlugin::exportData() const {
  QJsonObject root;

  QJsonArray txArr;
  for (const auto &tx : transactions_) {
    QJsonObject obj;
    obj["hash"] = tx.hash;
    obj["from"] = tx.from;
    obj["to"] = tx.to;
    obj["amount"] = tx.amount;
    obj["timestamp"] = tx.timestamp.toString(Qt::ISODate);
    obj["status"] = tx.status;
    txArr.append(obj);
  }
  root["transactions"] = txArr;

  QJsonArray blockArr;
  for (const auto &b : blocks_) {
    QJsonObject obj;
    obj["number"] = b.number;
    obj["hash"] = b.hash;
    obj["previousHash"] = b.previousHash;
    obj["transactionCount"] = b.transactionCount;
    obj["timestamp"] = b.timestamp.toString(Qt::ISODate);
    obj["miner"] = b.miner;
    blockArr.append(obj);
  }
  root["blocks"] = blockArr;

  QJsonArray contractArr;
  for (const auto &c : contracts_) {
    QJsonObject obj;
    obj["address"] = c.address;
    obj["name"] = c.name;
    obj["owner"] = c.owner;
    obj["deployedAt"] = c.deployedAt.toString(Qt::ISODate);
    obj["callCount"] = c.callCount;
    obj["active"] = c.active;
    contractArr.append(obj);
  }
  root["smartContracts"] = contractArr;

  QJsonArray supplyArr;
  for (const auto &s : supplyChain_) {
    QJsonObject obj;
    obj["itemId"] = s.itemId;
    obj["stage"] = s.stage;
    obj["location"] = s.location;
    obj["timestamp"] = s.timestamp.toString(Qt::ISODate);
    obj["handler"] = s.handler;
    obj["status"] = s.status;
    supplyArr.append(obj);
  }
  root["supplyChain"] = supplyArr;

  return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QTabWidget *BlockchainExplorerPlugin::tabs() const { return tabs_; }
QTableWidget *BlockchainExplorerPlugin::transactionTable() const { return transactionTable_; }
QTableWidget *BlockchainExplorerPlugin::blockTable() const { return blockTable_; }
QTableWidget *BlockchainExplorerPlugin::contractTable() const { return contractTable_; }
QTableWidget *BlockchainExplorerPlugin::supplyChainTable() const { return supplyChainTable_; }
QLabel *BlockchainExplorerPlugin::statusLabel() const { return statusLabel_; }

void BlockchainExplorerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  tabs_ = new QTabWidget;

  auto *txTab = new QWidget;
  auto *txLayout = new QVBoxLayout(txTab);
  auto *filterRow = new QWidget;
  auto *filterLayout = new QHBoxLayout(filterRow);
  filterEdit_ = new QLineEdit;
  filterEdit_->setPlaceholderText("Filter transactions...");
  filterLayout->addWidget(filterEdit_);
  txLayout->addWidget(filterRow);
  transactionTable_ = new QTableWidget;
  transactionTable_->setColumnCount(6);
  transactionTable_->setHorizontalHeaderLabels({"Hash", "From", "To", "Amount", "Time", "Status"});
  transactionTable_->horizontalHeader()->setStretchLastSection(true);
  txLayout->addWidget(transactionTable_);
  auto *txBtnRow = new QWidget;
  auto *txBtnLayout = new QHBoxLayout(txBtnRow);
  addTxBtn_ = new QPushButton("Add");
  removeTxBtn_ = new QPushButton("Remove");
  txBtnLayout->addWidget(addTxBtn_);
  txBtnLayout->addWidget(removeTxBtn_);
  txLayout->addWidget(txBtnRow);
  tabs_->addTab(txTab, "Transactions");

  auto *blockTab = new QWidget;
  auto *blockLayout = new QVBoxLayout(blockTab);
  blockTable_ = new QTableWidget;
  blockTable_->setColumnCount(6);
  blockTable_->setHorizontalHeaderLabels({"Number", "Hash", "Prev Hash", "TX Count", "Time", "Miner"});
  blockTable_->horizontalHeader()->setStretchLastSection(true);
  blockLayout->addWidget(blockTable_);
  auto *blockBtnRow = new QWidget;
  auto *blockBtnLayout = new QHBoxLayout(blockBtnRow);
  addBlockBtn_ = new QPushButton("Add");
  removeBlockBtn_ = new QPushButton("Remove");
  blockBtnLayout->addWidget(addBlockBtn_);
  blockBtnLayout->addWidget(removeBlockBtn_);
  blockLayout->addWidget(blockBtnRow);
  tabs_->addTab(blockTab, "Block Details");

  auto *contractTab = new QWidget;
  auto *contractLayout = new QVBoxLayout(contractTab);
  contractTable_ = new QTableWidget;
  contractTable_->setColumnCount(6);
  contractTable_->setHorizontalHeaderLabels({"Address", "Name", "Owner", "Deployed", "Calls", "Active"});
  contractTable_->horizontalHeader()->setStretchLastSection(true);
  contractLayout->addWidget(contractTable_);
  auto *contractBtnRow = new QWidget;
  auto *contractBtnLayout = new QHBoxLayout(contractBtnRow);
  addContractBtn_ = new QPushButton("Add");
  removeContractBtn_ = new QPushButton("Remove");
  executeContractBtn_ = new QPushButton("Execute");
  contractBtnLayout->addWidget(addContractBtn_);
  contractBtnLayout->addWidget(removeContractBtn_);
  contractBtnLayout->addWidget(executeContractBtn_);
  contractLayout->addWidget(contractBtnRow);
  tabs_->addTab(contractTab, "Smart Contracts");

  auto *supplyTab = new QWidget;
  auto *supplyLayout = new QVBoxLayout(supplyTab);
  supplyChainTable_ = new QTableWidget;
  supplyChainTable_->setColumnCount(6);
  supplyChainTable_->setHorizontalHeaderLabels({"Item ID", "Stage", "Location", "Time", "Handler", "Status"});
  supplyChainTable_->horizontalHeader()->setStretchLastSection(true);
  supplyLayout->addWidget(supplyChainTable_);
  auto *supplyBtnRow = new QWidget;
  auto *supplyBtnLayout = new QHBoxLayout(supplyBtnRow);
  addSupplyBtn_ = new QPushButton("Add");
  removeSupplyBtn_ = new QPushButton("Remove");
  supplyBtnLayout->addWidget(addSupplyBtn_);
  supplyBtnLayout->addWidget(removeSupplyBtn_);
  supplyLayout->addWidget(supplyBtnRow);
  tabs_->addTab(supplyTab, "Supply Chain");

  mainLayout->addWidget(tabs_);

  exportBtn_ = new QPushButton("Export Blockchain Data");
  mainLayout->addWidget(exportBtn_);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  connect(addTxBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Transaction creation requires a blockchain backend"));
  });
  connect(removeTxBtn_, &QPushButton::clicked, this, [this]() {
    int row = transactionTable_->currentRow();
    if (row >= 0) removeTransaction(row);
  });
  connect(filterEdit_, &QLineEdit::textChanged, this, [this](const QString &text) {
    filterTransactions(text);
  });
  connect(addBlockBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Block creation requires a blockchain backend"));
  });
  connect(removeBlockBtn_, &QPushButton::clicked, this, [this]() {
    int row = blockTable_->currentRow();
    if (row >= 0) removeBlock(row);
  });
  connect(addContractBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Smart contract creation requires a blockchain backend"));
  });
  connect(removeContractBtn_, &QPushButton::clicked, this, [this]() {
    int row = contractTable_->currentRow();
    if (row >= 0) removeSmartContract(row);
  });
  connect(executeContractBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Smart contract execution requires a blockchain backend"));
  });
  connect(addSupplyBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Supply-chain entry creation requires a blockchain backend"));
  });
  connect(removeSupplyBtn_, &QPushButton::clicked, this, [this]() {
    int row = supplyChainTable_->currentRow();
    if (row >= 0) removeSupplyChainEntry(row);
  });
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    exportData();
  });
}

void BlockchainExplorerPlugin::rebuildTransactionTable() {
  if (!transactionTable_) return;
  transactionTable_->setRowCount(filteredTransactions_.size());
  for (int i = 0; i < filteredTransactions_.size(); ++i) {
    const auto &tx = filteredTransactions_[i];
    transactionTable_->setItem(i, 0, new QTableWidgetItem(tx.hash));
    transactionTable_->setItem(i, 1, new QTableWidgetItem(tx.from));
    transactionTable_->setItem(i, 2, new QTableWidgetItem(tx.to));
    transactionTable_->setItem(i, 3, new QTableWidgetItem(QString::number(tx.amount)));
    transactionTable_->setItem(i, 4, new QTableWidgetItem(tx.timestamp.toString(Qt::ISODate)));
    transactionTable_->setItem(i, 5, new QTableWidgetItem(tx.status));
  }
}

void BlockchainExplorerPlugin::rebuildBlockTable() {
  if (!blockTable_) return;
  blockTable_->setRowCount(blocks_.size());
  for (int i = 0; i < blocks_.size(); ++i) {
    const auto &b = blocks_[i];
    blockTable_->setItem(i, 0, new QTableWidgetItem(QString::number(b.number)));
    blockTable_->setItem(i, 1, new QTableWidgetItem(b.hash));
    blockTable_->setItem(i, 2, new QTableWidgetItem(b.previousHash));
    blockTable_->setItem(i, 3, new QTableWidgetItem(QString::number(b.transactionCount)));
    blockTable_->setItem(i, 4, new QTableWidgetItem(b.timestamp.toString(Qt::ISODate)));
    blockTable_->setItem(i, 5, new QTableWidgetItem(b.miner));
  }
}

void BlockchainExplorerPlugin::rebuildContractTable() {
  if (!contractTable_) return;
  contractTable_->setRowCount(contracts_.size());
  for (int i = 0; i < contracts_.size(); ++i) {
    const auto &c = contracts_[i];
    contractTable_->setItem(i, 0, new QTableWidgetItem(c.address));
    contractTable_->setItem(i, 1, new QTableWidgetItem(c.name));
    contractTable_->setItem(i, 2, new QTableWidgetItem(c.owner));
    contractTable_->setItem(i, 3, new QTableWidgetItem(c.deployedAt.toString(Qt::ISODate)));
    contractTable_->setItem(i, 4, new QTableWidgetItem(QString::number(c.callCount)));
    contractTable_->setItem(i, 5, new QTableWidgetItem(c.active ? "Yes" : "No"));
  }
}

void BlockchainExplorerPlugin::rebuildSupplyChainTable() {
  if (!supplyChainTable_) return;
  supplyChainTable_->setRowCount(supplyChain_.size());
  for (int i = 0; i < supplyChain_.size(); ++i) {
    const auto &s = supplyChain_[i];
    supplyChainTable_->setItem(i, 0, new QTableWidgetItem(s.itemId));
    supplyChainTable_->setItem(i, 1, new QTableWidgetItem(s.stage));
    supplyChainTable_->setItem(i, 2, new QTableWidgetItem(s.location));
    supplyChainTable_->setItem(i, 3, new QTableWidgetItem(s.timestamp.toString(Qt::ISODate)));
    supplyChainTable_->setItem(i, 4, new QTableWidgetItem(s.handler));
    supplyChainTable_->setItem(i, 5, new QTableWidgetItem(s.status));
  }
}
