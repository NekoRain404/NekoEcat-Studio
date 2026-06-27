#include "DomainService.h"

// DomainService.cpp — EtherCAT domain draft creation and PDO entry registration
//
// Implementation notes:
//   - Each domain tracks its PDO entries, data size, and working counter
//   - processDomain() fails closed until wired to a live EtherCAT backend
//   - Simple in-memory draft store; no persistence or real hardware I/O

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
  if (position < 0 || index <= 0 || index > 0xFFFF || subIndex < 0 || subIndex > 0xFF) {
    emit error(QStringLiteral("Invalid PDO entry parameters"));
    return false;
  }
  for (const auto &existing : entries_[domain]) {
    if (existing.position == position && existing.index == index
        && existing.subIndex == subIndex) {
      emit error(QStringLiteral("PDO entry already registered"));
      return false;
    }
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

// Process-data exchange requires a live EtherCAT backend. Do not synthesize
// zeroed domain data because callers may treat that as real PDO exchange.
bool DomainService::processDomain(int domain) {
  auto it = infos_.find(domain);
  if (it == infos_.end()) {
    emit error(QStringLiteral("Domain %1 does not exist").arg(domain));
    return false;
  }

  emit error(QStringLiteral("Domain processing requires a connected EtherCAT backend"));
  return false;
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
