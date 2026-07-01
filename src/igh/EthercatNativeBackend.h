#pragma once

// Native IgH ecrt API backend for EtherCAT operations.
// Replaces CLI shell-outs with direct ecrt calls for 10-100x performance.

#include "EthercatTypes.h"
#include "EcatService.h"
#include "EthercatCliBackend.h"

#include <QObject>
#include <QJsonArray>
#include <QString>

#ifdef HAVE_IGH
#include <ecrt.h>
#endif

#include <mutex>

#ifdef HAVE_IGH
class MasterGuard {
public:
    explicit MasterGuard(std::mutex &mutex, ec_master_t *&cachedMaster,
                         int &cachedMasterIndex, int index);
    ~MasterGuard();

    MasterGuard(const MasterGuard &) = delete;
    MasterGuard &operator=(const MasterGuard &) = delete;
    MasterGuard(MasterGuard &&other) noexcept;
    MasterGuard &operator=(MasterGuard &&other) noexcept;

    ec_master_t *get() const { return master_; }

private:
    std::unique_lock<std::mutex> lock_;
    ec_master_t *master_ = nullptr;
    ec_master_t *&cachedMaster_;
    int &cachedMasterIndex_;
};
#endif

class EthercatNativeBackend : public QObject, public EcatService {
    Q_OBJECT

public:
    explicit EthercatNativeBackend(QObject *parent = nullptr);
    ~EthercatNativeBackend() override;

    QString masterText(const QString &master, QString *error = nullptr) const override;
    QVector<SlaveInfo> scanSlaves(const QString &master, QString *error = nullptr) const override;
    QString slaveInfo(const QString &master, int position, QString *error = nullptr) const override;
    QString slaveXml(const QString &master, int position, QString *error = nullptr) const override;
    QString pdos(const QString &master, int position, QString *error = nullptr) const override;
    QString sdos(const QString &master, int position, QString *error = nullptr) const override;
    QString upload(const QString &master, int position, const QString &index,
                   const QString &subIndex, const QString &type = QString(),
                   QString *error = nullptr) const override;
    bool download(const QString &master, int position, const QString &index,
                  const QString &subIndex, const QString &value,
                  const QString &type, QString *error = nullptr) const override;
    bool setState(const QString &master, int position, const QString &state,
                  QString *error = nullptr) const override;
    bool setAllStates(const QString &master, const QString &state,
                      QString *error = nullptr) const override;
    bool rescan(const QString &master, QString *error = nullptr) const override;
    QJsonArray hostDiagnostics(QString *error = nullptr) const override;

    // TODO: Replace with backendType() enum or dynamic_cast at call sites.
    // Keeping temporarily for backward compatibility.
    bool isNative() const override { return true; }

    // CLI fallback tracking — ecrt API has gaps vs the CLI tool.
    bool lastOperationWasFallback() const override { return lastFallback_; }
    QString lastFallbackReason() const override { return lastFallbackReason_; }

private:
#ifdef HAVE_IGH
    MasterGuard acquireMaster(const QString &master) const;
    static QString alStateToString(uint8_t state);
    static QString slaveFlagsToString(const ec_slave_info_t &info);
    SlaveInfo convertSlaveInfo(const ec_slave_info_t &info) const;

    mutable std::mutex masterMutex_;
    mutable ec_master_t *cachedMaster_ = nullptr;
    mutable int cachedMasterIndex_ = -1;
#endif

    mutable EthercatCliBackend cliFallback_;

    // CLI fallback tracking flags (mutable because all operations are const).
    mutable bool lastFallback_ = false;
    mutable QString lastFallbackReason_;

    static constexpr int SDO_TIMEOUT_MS = 5000;
    static constexpr size_t MAX_SDO_DATA_SIZE = 4096;
};
