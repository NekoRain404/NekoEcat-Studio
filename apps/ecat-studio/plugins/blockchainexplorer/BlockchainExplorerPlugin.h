#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QTableWidget;

class BlockchainExplorerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit BlockchainExplorerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct Transaction {
    QString hash;
    QString from;
    QString to;
    double amount;
    QDateTime timestamp;
    QString status;
  };

  struct BlockInfo {
    int number;
    QString hash;
    QString previousHash;
    int transactionCount;
    QDateTime timestamp;
    QString miner;
  };

  struct SmartContract {
    QString address;
    QString name;
    QString owner;
    QDateTime deployedAt;
    int callCount;
    bool active;
  };

  struct SupplyChainEntry {
    QString itemId;
    QString stage;
    QString location;
    QDateTime timestamp;
    QString handler;
    QString status;
  };

  void addTransaction(const Transaction &tx);
  void removeTransaction(int index);
  int transactionCount() const;

  void addBlock(const BlockInfo &block);
  void removeBlock(int index);
  int blockCount() const;

  void addSmartContract(const SmartContract &contract);
  void removeSmartContract(int index);
  int smartContractCount() const;

  void addSupplyChainEntry(const SupplyChainEntry &entry);
  void removeSupplyChainEntry(int index);
  int supplyChainEntryCount() const;

  void filterTransactions(const QString &query);
  QString exportData() const;

  QTabWidget *tabs() const;
  QTableWidget *transactionTable() const;
  QTableWidget *blockTable() const;
  QTableWidget *contractTable() const;
  QTableWidget *supplyChainTable() const;
  QLabel *statusLabel() const;

signals:
  void transactionAdded(const QString &hash);
  void blockAdded(int number);
  void contractExecuted(const QString &address);
  void supplyChainUpdated(const QString &itemId);

private:
  void buildUi();
  void rebuildTransactionTable();
  void rebuildBlockTable();
  void rebuildContractTable();
  void rebuildSupplyChainTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QTableWidget *transactionTable_ = nullptr;
  QTableWidget *blockTable_ = nullptr;
  QTableWidget *contractTable_ = nullptr;
  QTableWidget *supplyChainTable_ = nullptr;
  QLineEdit *filterEdit_ = nullptr;
  QPushButton *addTxBtn_ = nullptr;
  QPushButton *removeTxBtn_ = nullptr;
  QPushButton *addBlockBtn_ = nullptr;
  QPushButton *removeBlockBtn_ = nullptr;
  QPushButton *addContractBtn_ = nullptr;
  QPushButton *removeContractBtn_ = nullptr;
  QPushButton *executeContractBtn_ = nullptr;
  QPushButton *addSupplyBtn_ = nullptr;
  QPushButton *removeSupplyBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<Transaction> transactions_;
  QVector<Transaction> filteredTransactions_;
  QVector<BlockInfo> blocks_;
  QVector<SmartContract> contracts_;
  QVector<SupplyChainEntry> supplyChain_;
};
