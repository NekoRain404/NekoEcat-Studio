#pragma once

// ConfigurationService — manages EtherCAT master, slave, network, timing,
// and safety configuration with validation and persistence.
//
// Provides structured config objects (MasterConfig, SlaveConfig, NetworkConfig,
// TimingConfig, SafetyConfig) with JSON serialization. Supports load/save from
// file, apply-to-daemon, and validation with error/warning reporting.
//
// This service provides comprehensive configuration management for the
// EtherCAT system. It handles:
//   - Master configuration (adapter, cycle time, DC settings)
//   - Slave configuration (position, name, vendor/product, SDOs)
//   - Network configuration (IP, subnet, gateway, DNS)
//   - Timing configuration (cycle time, sync shifts)
//   - Safety configuration (watchdog, error behavior, recovery)
//   - Configuration validation with error/warning reporting
//   - Configuration persistence (load/save from JSON files)
//   - Configuration application to daemon
//
// Usage:
//   ServiceContainer *container = ...;
//   ConfigurationService *config = container->configuration();
//   config->loadConfiguration("/path/to/config.json");
//   MasterConfig &master = config->masterConfig();
//   master.adapter = "eth0";
//   master.cycleTimeUs = 1000;
//   ConfigValidationResult result = config->validateConfiguration();
//   if (result.valid) {
//     config->applyConfiguration();
//     config->saveConfiguration("/path/to/config.json");
//   }
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Configuration
//   operations are synchronous and block the calling thread.
//
// Performance:
//   - Configuration load/save is O(n) where n is JSON file size
//   - Validation is O(n) where n is number of config items
//   - Configuration application is O(1) for local state, O(n) for daemon

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

// Master configuration structure.
struct MasterConfig {
    QString adapter;                                      // Network adapter name
    int cycleTimeUs = 1000;                               // Cycle time in microseconds
    bool distributedClocks = true;                        // Enable distributed clocks
    int dcSync0Shift = 0;                                 // DC SYNC0 shift in microseconds
    QJsonObject toJson() const;                           // Serialize to JSON
    static MasterConfig fromJson(const QJsonObject& obj); // Deserialize from JSON
};

// Slave configuration structure.
struct SlaveConfig {
    int position = -1;                                   // Slave position on the bus
    QString name;                                        // Slave name
    int vendorId = 0;                                    // Vendor ID
    int productCode = 0;                                 // Product code
    QJsonArray sdos;                                     // SDO configurations
    QJsonObject toJson() const;                          // Serialize to JSON
    static SlaveConfig fromJson(const QJsonObject& obj); // Deserialize from JSON
};

// Network configuration structure.
struct NetworkConfig {
    QString ipAddress;                                     // IP address
    QString subnetMask;                                    // Subnet mask
    QString gateway;                                       // Gateway address
    QString dns;                                           // DNS server address
    QJsonObject toJson() const;                            // Serialize to JSON
    static NetworkConfig fromJson(const QJsonObject& obj); // Deserialize from JSON
};

// Timing configuration structure.
struct TimingConfig {
    int cycleTimeUs = 1000;                               // Cycle time in microseconds
    int sync0Shift = 0;                                   // SYNC0 shift in microseconds
    int sync1Shift = 0;                                   // SYNC1 shift in microseconds
    bool sync0Enabled = true;                             // Enable SYNC0
    bool sync1Enabled = false;                            // Enable SYNC1
    QJsonObject toJson() const;                           // Serialize to JSON
    static TimingConfig fromJson(const QJsonObject& obj); // Deserialize from JSON
};

// Safety configuration structure.
struct SafetyConfig {
    int watchdogTimeoutMs = 5000;                         // Watchdog timeout in milliseconds
    QString errorBehavior = "safeop";                     // Error behavior (safeop, preop, init)
    bool autoRecover = false;                             // Enable automatic recovery
    int maxRetries = 3;                                   // Maximum retry attempts
    QJsonObject toJson() const;                           // Serialize to JSON
    static SafetyConfig fromJson(const QJsonObject& obj); // Deserialize from JSON
};

// Configuration validation result.
struct ConfigValidationResult {
    bool valid = true;    // Whether configuration is valid
    QStringList errors;   // Validation errors
    QStringList warnings; // Validation warnings
};

class ConfigurationService : public QObject {
    Q_OBJECT
public:
    explicit ConfigurationService(QObject* parent = nullptr);

    // Load configuration from a JSON file.
    // @param filePath  Path to the configuration file
    // @return true if load was successful
    bool loadConfiguration(const QString& filePath);

    // Save configuration to a JSON file.
    // @param filePath  Path to save the configuration
    // @return true if save was successful
    bool saveConfiguration(const QString& filePath);

    // Apply configuration to the daemon.
    // @return true if application was successful
    bool applyConfiguration();

    // Validate the current configuration.
    // @return ConfigValidationResult with errors and warnings
    ConfigValidationResult validateConfiguration() const;

    // Reset configuration to default values.
    void resetToDefaults();

    // Get mutable reference to master configuration.
    MasterConfig& masterConfig();

    // Get const reference to master configuration.
    const MasterConfig& masterConfig() const;

    // Get mutable reference to slave configurations.
    QVector<SlaveConfig>& slaveConfigs();

    // Get const reference to slave configurations.
    const QVector<SlaveConfig>& slaveConfigs() const;

    // Get mutable reference to network configuration.
    NetworkConfig& networkConfig();

    // Get const reference to network configuration.
    const NetworkConfig& networkConfig() const;

    // Get mutable reference to timing configuration.
    TimingConfig& timingConfig();

    // Get const reference to timing configuration.
    const TimingConfig& timingConfig() const;

    // Get mutable reference to safety configuration.
    SafetyConfig& safetyConfig();

    // Get const reference to safety configuration.
    const SafetyConfig& safetyConfig() const;

signals:
    // Emitted when configuration changes.
    void configurationChanged();

private:
    MasterConfig master_;         // Master configuration
    QVector<SlaveConfig> slaves_; // Slave configurations
    NetworkConfig network_;       // Network configuration
    TimingConfig timing_;         // Timing configuration
    SafetyConfig safety_;         // Safety configuration
};
