#include "EtherCATBlockchainService.h"

EtherCATBlockchainService::EtherCATBlockchainService(QObject *parent)
    : QObject(parent) {}

bool EtherCATBlockchainService::recordTransaction(const Transaction &transaction) {
  Q_UNUSED(transaction);
  return false;
}

bool EtherCATBlockchainService::verifyTransaction(const QString &transactionId) {
  Q_UNUSED(transactionId);
  return false;
}

bool EtherCATBlockchainService::executeSmartContract(const SmartContract &contract) {
  Q_UNUSED(contract);
  return false;
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
