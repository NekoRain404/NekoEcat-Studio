#pragma once

// RedundancyService — manages EtherCAT cable redundancy.
//
// Provides redundancy status monitoring, failover/failback control,
// and event history tracking. Communicates with the ecatd daemon
// via EcatClient for runtime redundancy operations.
//
// Usage:
//   RedundancyService redundancy(client);
//   redundancy.queryStatus();         // Emits statusReceived()
//   redundancy.enableRedundancy();    // Emits commandResult()
//   redundancy.failover();            // Emits commandResult()
//   redundancy.queryHistory();        // Emits historyReceived()
//
// Thread safety:
//   All methods must be called from the main (GUI) thread.

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QVector>

class EcatClient;

// Redundancy state enumeration.
enum class RedundancyState {
    SinglePath, // Only primary path active
    DualPath,   // Both paths active
    Failover,   // Failover to secondary path
    Error       // Redundancy error
};

// Path state enumeration.
enum class PathState {
    Active,  // Path is active
    Standby, // Path is in standby
    Failed,  // Path has failed
    Unknown  // Path state unknown
};

// Redundancy path structure.
struct RedundancyPath {
    int pathId = 0;                       // Path ID
    PathState state = PathState::Unknown; // Path state
    int slaveCount = 0;                   // Number of slaves on path
    QDateTime lastCheck;                  // Last health check timestamp
    bool isHealthy = false;               // Whether path is healthy
};

// Redundancy event structure.
struct RedundancyEvent {
    int pathId = 0;                                          // Path ID
    RedundancyState fromState = RedundancyState::SinglePath; // Previous state
    RedundancyState toState = RedundancyState::SinglePath;   // New state
    QDateTime timestamp;                                     // Event timestamp
    bool success = false;                                    // Whether event succeeded
    QString reason;                                          // Event reason
};

class RedundancyService : public QObject {
    Q_OBJECT
public:
    explicit RedundancyService(EcatClient* client, QObject* parent = nullptr);

    // Set the primary path configuration (offline draft).
    void setPrimaryPath(int slaveCount);

    // Set the secondary path configuration (offline draft).
    void setSecondaryPath(int slaveCount);

    // Query redundancy status from daemon.
    // Emits statusReceived() with the result.
    void queryStatus();

    // Enable redundancy (runtime operation).
    // Emits commandResult() with the result.
    bool enableRedundancy();

    // Disable redundancy (runtime operation).
    // Emits commandResult() with the result.
    bool disableRedundancy();

    // Perform failover to secondary path.
    // Emits commandResult() with the result.
    bool failover();

    // Perform failback to primary path.
    // Emits commandResult() with the result.
    bool failback();

    // Query redundancy event history from daemon.
    // Emits historyReceived() with the result.
    void queryHistory(int limit = 100);

    // Get the current redundancy state (cached from last query).
    RedundancyState currentState() const;

    // Get the primary path information (cached from last query).
    RedundancyPath primaryPath() const;

    // Get the secondary path information (cached from last query).
    RedundancyPath secondaryPath() const;

    // Get redundancy event history (cached from last query).
    QVector<RedundancyEvent> redundancyHistory() const;

    // Check if redundancy is enabled (cached from last query).
    bool isRedundant() const;

signals:
    // Emitted when redundancy status query completes.
    void statusReceived(const QJsonObject& data);

    // Emitted when a redundancy command completes.
    void commandResult(const QString& command, bool success, const QString& message);

    // Emitted when history query completes.
    void historyReceived(const QJsonObject& data);

    // Emitted when redundancy state changes.
    void redundancyStateChanged(RedundancyState state);

    // Emitted when failover occurs.
    void failoverOccurred(int fromPath, int toPath);

    // Emitted when a path state changes.
    void pathStateChanged(int pathId, PathState state);

    // Emitted on error.
    void error(const QString& message);

private:
    EcatClient* client_;
    RedundancyState state_ = RedundancyState::SinglePath;
    RedundancyPath primaryPath_;
    RedundancyPath secondaryPath_;
    QVector<RedundancyEvent> history_;
    static constexpr int kMaxHistory = 500;
};
