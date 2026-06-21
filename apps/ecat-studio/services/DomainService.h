#pragma once

// DomainService — EtherCAT domain management for process data.
//
// Provides domain creation, PDO entry registration, domain processing,
// and data access. Manages the mapping between PDO entries and process
// data memory regions.
//
// This service provides EtherCAT domain management capabilities. It handles:
//   - Domain creation and management
//   - PDO entry registration within domains
//   - Domain processing (exchange of process data)
//   - Domain data access and retrieval
//   - Domain information tracking
//
// Usage:
//   DomainService domain;
//   int domIdx = domain.createDomain();
//   domain.registerPdoEntry(domIdx, 0, 0x6000, 0x01);
//   domain.processDomain(domIdx);
//   QByteArray data = domain.domainData(domIdx);
//   DomainInfo info = domain.domainInfo(domIdx);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Domain
//   processing is synchronous and blocks the calling thread.
//
// Performance:
//   - Domain creation is O(1)
//   - PDO registration is O(1)
//   - Domain processing is O(n) where n is number of PDO entries
//   - Data access is O(1)

#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QHash>

// Domain information structure.
struct DomainInfo {
  int domainIndex = -1;     // Domain index
  int pdoEntryCount = 0;    // Number of PDO entries in domain
  int dataSize = 0;         // Data size in bytes
  int workingCounter = 0;   // Working counter value
};

// PDO entry structure.
struct PdoEntry {
  int position = 0;    // Slave position
  int index = 0;       // PDO index
  int subIndex = 0;    // PDO subindex
};

class DomainService : public QObject {
  Q_OBJECT
public:
  explicit DomainService(QObject *parent = nullptr);

  // Create a new domain.
  // @return Domain index
  int createDomain();

  // Register a PDO entry in a domain.
  // @param domain    Domain index
  // @param position  Slave position
  // @param index     PDO index
  // @param subIndex  PDO subindex
  // @return true if registration was successful
  bool registerPdoEntry(int domain, int position, int index, int subIndex);

  // Process a domain (exchange process data).
  // @param domain  Domain index
  // @return true if processing was successful
  bool processDomain(int domain);

  // Get the data from a domain.
  // @param domain  Domain index
  // @return Domain data as QByteArray
  QByteArray domainData(int domain) const;

  // Get information about a domain.
  // @param domain  Domain index
  // @return DomainInfo structure
  DomainInfo domainInfo(int domain) const;

  // Get all domain indices.
  // @return Vector of domain indices
  QVector<int> domains() const;

signals:
  // Emitted when a domain is processed.
  // @param domain  Domain index
  // @param data    Processed data
  void domainProcessed(int domain, const QByteArray &data);

  // Emitted when an error occurs.
  // @param message  Human-readable error message
  void error(const QString &message);

private:
  int nextIndex_ = 0;                          // Next domain index
  QHash<int, DomainInfo> infos_;               // Domain information
  QHash<int, QVector<PdoEntry>> entries_;      // PDO entries per domain
  QHash<int, QByteArray> data_;                // Domain data
};
