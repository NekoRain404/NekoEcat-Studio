#pragma once
// AdapterHandler — discovers and manages network adapters for EtherCAT.
// Reads /sys/class/net/ for PCI device info and uses ethtool for link status.

#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QString>

// Describes a single network interface found on the host.
struct NetworkAdapter {
    QString name;        // e.g., "eth0"
    QString mac;         // MAC address in AA:BB:CC:DD:EE:FF format
    QString driver;      // kernel driver, e.g., "r8169"
    QString pciSlot;     // PCI slot name from sysfs
    bool linkUp = false; // whether the link is currently up
    bool isEthercat = false; // true if this is the current IgH master adapter
};

// Enumerates host network adapters and allows the user to select which
// NIC the IgH EtherCAT Master should bind to.
class AdapterHandler {
public:
  // List all available network adapters with their properties.
  QJsonObject handleList(const QString &id, const QJsonObject &params);

  // Set the adapter used by IgH EtherCAT Master by writing to /etc/ethercat.conf.
  QJsonObject handleSet(const QString &id, const QJsonObject &params);

private:
  // Enumerate network adapters from /sys/class/net/.
  QVector<NetworkAdapter> enumerateAdapters() const;

  // Read a single sysfs attribute file, trimmed.
  QString readSysfs(const QString &path) const;

  // Run a shell command and return its stdout.
  QString runCommand(const QString &cmd) const;

  // Read the current master0_device value from /etc/ethercat.conf.
  QString currentAdapter() const;
};
