#include "WorkflowBlockchainService.h"

WorkflowBlockchainService::WorkflowBlockchainService(QObject *parent)
    : QObject(parent) {}

bool WorkflowBlockchainService::recordTransaction(const WfTransaction &transaction) {
  Q_UNUSED(transaction);
  return false;
}

bool WorkflowBlockchainService::verifyTransaction(const QString &transactionId) {
  Q_UNUSED(transactionId);
  return false;
}

bool WorkflowBlockchainService::executeSmartContract(const WfSmartContract &contract) {
  Q_UNUSED(contract);
  return false;
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
