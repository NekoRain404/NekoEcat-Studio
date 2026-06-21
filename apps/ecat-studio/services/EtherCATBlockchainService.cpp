#include "EtherCATBlockchainService.h"

// EtherCATBlockchainService.cpp — Blockchain ledger for transaction tracking and smart contracts
//
// Implementation notes:
//   - In-memory transaction store with auto-eviction at kMaxTransactions
//   - Auto-generates transaction IDs and block numbers via nextBlock_ counter
//   - Supports supply chain tracking and smart contract execution

EtherCATBlockchainService::EtherCATBlockchainService(QObject *parent)
    : QObject(parent) {}

bool EtherCATBlockchainService::recordTransaction(const Transaction &transaction) {
  Transaction tx = transaction;
  if (tx.transactionId.isEmpty())
    tx.transactionId = QString("TX_%1_%2").arg(nextBlock_).arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
  tx.timestamp = QDateTime::currentDateTime();
  tx.blockNumber = nextBlock_++;
  tx.status = TransactionStatus::Confirmed;
  transactions_[tx.transactionId] = tx;
  if (transactions_.size() > kMaxTransactions) {
    auto oldest = transactions_.constBegin().key();
    transactions_.remove(oldest);
  }
  emit transactionRecorded(tx);
  return true;
}

bool EtherCATBlockchainService::verifyTransaction(const QString &transactionId) {
  bool valid = transactions_.contains(transactionId);
  emit verificationCompleted(transactionId, valid);
  return valid;
}

bool EtherCATBlockchainService::executeSmartContract(const SmartContract &contract) {
  SmartContract sc = contract;
  sc.active = true;
  contracts_[contract.contractId] = sc;
  emit smartContractExecuted(sc);
  return true;
}

SupplyChain EtherCATBlockchainService::trackSupplyChain(const QString &productId) {
  return supplyChains_.value(productId);
}

Transaction EtherCATBlockchainService::transaction(const QString &transactionId) const {
  return transactions_.value(transactionId);
}

QVector<Transaction> EtherCATBlockchainService::allTransactions() const {
  QVector<Transaction> result;
  for (auto it = transactions_.constBegin(); it != transactions_.constEnd(); ++it)
    result.append(it.value());
  return result;
}

void EtherCATBlockchainService::addSupplyChainEntry(const QString &productId,
                                                      const SupplyChainEntry &entry) {
  supplyChains_[productId].productId = productId;
  supplyChains_[productId].entries.append(entry);
}
