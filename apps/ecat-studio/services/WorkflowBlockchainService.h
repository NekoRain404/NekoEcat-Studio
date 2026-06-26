#pragma once
// WorkflowBlockchainService -- request facade for blockchain audit trails,
// smart contracts, decentralized verification, and supply-chain tracking.
// No blockchain node/backend is wired yet, so blockchain mutations fail closed
// instead of generating synthetic transactions, confirmations, or contract
// execution events. Supply-chain entries remain local draft records only.

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QHash>
#include <QByteArray>

enum class WfTransactionStatus { Pending, Confirmed, Rejected, Expired };

struct WfTransaction {
  QString transactionId;
  QDateTime timestamp;
  QString sender;
  QString receiver;
  QByteArray data;
  QByteArray signature;
  int blockNumber = 0;
  WfTransactionStatus status = WfTransactionStatus::Pending;
};

struct WfSmartContract {
  QString contractId;
  QString name;
  QByteArray bytecode;
  QVariantMap parameters;
  bool active = false;
};

struct WfSupplyChainEntry {
  QString stepId;
  QString location;
  QDateTime timestamp;
  QString handler;
  QString status;
};

struct WfSupplyChain {
  QString productId;
  QVector<WfSupplyChainEntry> entries;
  bool verified = false;
};

class WorkflowBlockchainService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowBlockchainService(QObject *parent = nullptr);

  bool recordTransaction(const WfTransaction &transaction);
  bool verifyTransaction(const QString &transactionId);
  bool executeSmartContract(const WfSmartContract &contract);
  WfSupplyChain trackSupplyChain(const QString &productId);
  WfTransaction transaction(const QString &transactionId) const;
  QVector<WfTransaction> allTransactions() const;
  void addSupplyChainEntry(const QString &productId, const WfSupplyChainEntry &entry);

signals:
  void transactionRecorded(const WfTransaction &transaction);
  void smartContractExecuted(const WfSmartContract &contract);
  void verificationCompleted(const QString &transactionId, bool valid);

private:
  QHash<QString, WfTransaction> transactions_;
  QHash<QString, WfSmartContract> contracts_;
  QHash<QString, WfSupplyChain> supplyChains_;
  int nextBlock_ = 1;
  static constexpr int kMaxTransactions = 10000;
};
