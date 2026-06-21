#include "WorkflowBlockchainService.h"

WorkflowBlockchainService::WorkflowBlockchainService(QObject *parent)
    : QObject(parent) {}

bool WorkflowBlockchainService::recordTransaction(const WfTransaction &transaction) {
  WfTransaction tx = transaction;
  if (tx.transactionId.isEmpty())
    tx.transactionId = QString("WFTX_%1_%2").arg(nextBlock_).arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
  tx.timestamp = QDateTime::currentDateTime();
  tx.blockNumber = nextBlock_++;
  tx.status = WfTransactionStatus::Confirmed;
  transactions_[tx.transactionId] = tx;
  if (transactions_.size() > kMaxTransactions) {
    auto oldest = transactions_.constBegin().key();
    transactions_.remove(oldest);
  }
  emit transactionRecorded(tx);
  return true;
}

bool WorkflowBlockchainService::verifyTransaction(const QString &transactionId) {
  bool valid = transactions_.contains(transactionId);
  emit verificationCompleted(transactionId, valid);
  return valid;
}

bool WorkflowBlockchainService::executeSmartContract(const WfSmartContract &contract) {
  WfSmartContract sc = contract;
  sc.active = true;
  contracts_[contract.contractId] = sc;
  emit smartContractExecuted(sc);
  return true;
}

WfSupplyChain WorkflowBlockchainService::trackSupplyChain(const QString &productId) {
  return supplyChains_.value(productId);
}

WfTransaction WorkflowBlockchainService::transaction(const QString &transactionId) const {
  return transactions_.value(transactionId);
}

QVector<WfTransaction> WorkflowBlockchainService::allTransactions() const {
  QVector<WfTransaction> result;
  for (auto it = transactions_.constBegin(); it != transactions_.constEnd(); ++it)
    result.append(it.value());
  return result;
}

void WorkflowBlockchainService::addSupplyChainEntry(const QString &productId,
                                                      const WfSupplyChainEntry &entry) {
  supplyChains_[productId].productId = productId;
  supplyChains_[productId].entries.append(entry);
}
