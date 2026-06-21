#include "DomainService.h"

// DomainService.cpp — EtherCAT domain creation and PDO entry registration
//
// Implementation notes:
//   - Each domain tracks its PDO entries, data size, and working counter
//   - processDomain() allocates a zeroed data buffer sized to registered entries
//   - Simple in-memory store; no persistence or real hardware I/O

DomainService::DomainService(QObject *parent) : QObject(parent) {}

int DomainService::createDomain() {
  int idx = nextIndex_++;
  DomainInfo info;
  info.domainIndex = idx;
  infos_[idx] = info;
  data_[idx] = QByteArray();
  return idx;
}

// Registers a PDO entry (position:index:subindex) into the given domain
bool DomainService::registerPdoEntry(int domain, int position, int index,
                                     int subIndex) {
  auto it = infos_.find(domain);
  if (it == infos_.end()) {
    emit error(QStringLiteral("Domain %1 does not exist").arg(domain));
    return false;
  }
  if (position < 0 || index <= 0) {
    emit error(QStringLiteral("Invalid PDO entry parameters"));
    return false;
  }

  PdoEntry entry;
  entry.position = position;
  entry.index = index;
  entry.subIndex = subIndex;
  entries_[domain].append(entry);

  it->pdoEntryCount = entries_[domain].size();
  it->dataSize += 4;
  return true;
}

// Allocates a zeroed data buffer for the domain and emits domainProcessed
bool DomainService::processDomain(int domain) {
  auto it = infos_.find(domain);
  if (it == infos_.end()) return false;

  it->workingCounter = it->pdoEntryCount;
  QByteArray d(it->dataSize, 0);
  data_[domain] = d;
  emit domainProcessed(domain, d);
  return true;
}

QByteArray DomainService::domainData(int domain) const {
  return data_.value(domain);
}

DomainInfo DomainService::domainInfo(int domain) const {
  return infos_.value(domain);
}

QVector<int> DomainService::domains() const {
  return infos_.keys().toVector();
}
