#pragma once
// EtherCATBlockchainService -- request facade for immutable audit trails,
// smart contracts, decentralized verification, and supply chain tracking.
// No blockchain backend is wired yet, so transaction, verification, and smart
// contract requests fail closed instead of generating synthetic confirmations.

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QHash>
#include <QByteArray>

enum class TransactionStatus { Pending, Confirmed, Rejected, Expired };

struct Transaction {
  QString transactionId;
  QDateTime timestamp;
  QString sender;
  QString receiver;
  QByteArray data;
  QByteArray signature;
  int blockNumber = 0;
  TransactionStatus status = TransactionStatus::Pending;
};

struct SmartContract {
  QString contractId;
  QString name;
  QByteArray bytecode;
  QVariantMap parameters;
  bool active = false;
};

struct SupplyChainEntry {
  QString stepId;
  QString location;
  QDateTime timestamp;
  QString handler;
  QString status;
};

struct SupplyChain {
  QString productId;
  QVector<SupplyChainEntry> entries;
  bool verified = false;
};

class EtherCATBlockchainService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATBlockchainService(QObject *parent = nullptr);

  bool recordTransaction(const Transaction &transaction);
  bool verifyTransaction(const QString &transactionId);
  bool executeSmartContract(const SmartContract &contract);
  SupplyChain trackSupplyChain(const QString &productId);
  Transaction transaction(const QString &transactionId) const;
  QVector<Transaction> allTransactions() const;
  void addSupplyChainEntry(const QString &productId, const SupplyChainEntry &entry);

signals:
  void transactionRecorded(const Transaction &transaction);
  void smartContractExecuted(const SmartContract &contract);
  void verificationCompleted(const QString &transactionId, bool valid);

private:
  QHash<QString, Transaction> transactions_;
  QHash<QString, SmartContract> contracts_;
  QHash<QString, SupplyChain> supplyChains_;
  int nextBlock_ = 1;
  static constexpr int kMaxTransactions = 10000;
};
