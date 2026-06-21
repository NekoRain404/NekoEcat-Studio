#pragma once

// WatchService — manages the Watch list, periodic polling, and drift detection.
//
// This service provides SDO value monitoring capabilities for the Watch workspace.
// It handles:
//   - Adding/removing SDO entries to the watch list
//   - Periodic polling of watched SDO values
//   - Drift detection by comparing current vs previous values
//   - Batch refresh of all watched entries
//
// Usage:
//   ServiceContainer *container = ...;
//   WatchService *watch = container->watch();
//   watch->addEntry(0, "0x6000", "0x01", "UINT8");
//   watch->refreshAll();  // Poll all entries
//   int count = watch->entryCount();
//   const WatchEntry &entry = watch->entryAt(0);
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - Batch refresh sends all SDO reads in a single batch request
//   - Drift detection uses simple string comparison (O(n) per entry)
//   - Entry count is limited by available memory

#include <QObject>
#include <QString>
#include <QVector>

class EcatClient;

// Represents a single SDO entry in the watch list.
struct WatchEntry {
  int position = -1;      // Slave position on the bus
  QString index;          // SDO index in hex format (e.g., "0x6000")
  QString subIndex;       // SDO subindex in hex format (e.g., "0x01")
  QString type;           // Data type (e.g., "UINT8", "INT16")
  QString value;          // Current value (populated after refresh)
  QString previousValue;  // Previous value (for drift detection)
  bool changed = false;   // Whether value changed since last refresh
};

class WatchService : public QObject {
  Q_OBJECT
public:
  explicit WatchService(EcatClient *client, QObject *parent = nullptr);

  // Add an SDO entry to the watch list.
  // @param position  Slave position on the bus (0-based)
  // @param index     SDO index in hex format (e.g., "0x6000")
  // @param subIndex  SDO subindex in hex format (e.g., "0x01")
  // @param type      Data type (optional, auto-detected if empty)
  void addEntry(int position, const QString &index, const QString &subIndex, const QString &type = QString());

  // Remove an SDO entry from the watch list.
  // @param position  Slave position on the bus (0-based)
  // @param index     SDO index in hex format
  // @param subIndex  SDO subindex in hex format
  void removeEntry(int position, const QString &index, const QString &subIndex);

  // Refresh all entries in the watch list by polling the daemon.
  // Emits entryUpdated() for each entry and refreshComplete() when done.
  void refreshAll();

  // Get the number of entries in the watch list.
  // @return Number of entries
  int entryCount() const;

  // Get a specific entry by index.
  // @param i  Index in the entry list (0-based)
  // @return Reference to the WatchEntry
  const WatchEntry &entryAt(int i) const;

signals:
  // Emitted when a single entry is updated during refresh.
  // @param row    Row index in the entry list
  // @param entry  The updated WatchEntry
  void entryUpdated(int row, const WatchEntry &entry);

  // Emitted when all entries have been refreshed.
  // @param requested  Number of entries requested
  // @param succeeded  Number of entries successfully refreshed
  void refreshComplete(int requested, int succeeded);

private:
  EcatClient *client_;           // TCP client to ecatd daemon
  QVector<WatchEntry> entries_;  // List of watched SDO entries
};
