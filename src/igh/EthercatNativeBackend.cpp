// Native IgH ecrt API backend for EtherCAT operations.
#include "EthercatNativeBackend.h"

#ifdef HAVE_IGH

#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>

#include <cstring>

MasterGuard::MasterGuard(std::mutex &mutex, ec_master_t *&cachedMaster,
                         int &cachedMasterIndex, int index)
    : lock_(mutex), cachedMaster_(cachedMaster),
      cachedMasterIndex_(cachedMasterIndex) {
    if (cachedMaster_ && cachedMasterIndex_ == index) {
        master_ = cachedMaster_;
        return;
    }
    if (cachedMaster_) {
        ecrt_release_master(cachedMaster_);
        cachedMaster_ = nullptr;
        cachedMasterIndex_ = -1;
    }
    master_ = ecrt_open_master(index);
    if (!master_) {
        return;
    }
    if (ecrt_master_reserve(master_) != 0) {
        ecrt_release_master(master_);
        master_ = nullptr;
        return;
    }
    cachedMaster_ = master_;
    cachedMasterIndex_ = index;
}

MasterGuard::~MasterGuard() {
    if (master_ && master_ != cachedMaster_) {
        ecrt_release_master(master_);
    }
}

MasterGuard::MasterGuard(MasterGuard &&other) noexcept
    : lock_(std::move(other.lock_)), master_(other.master_),
      cachedMaster_(other.cachedMaster_),
      cachedMasterIndex_(other.cachedMasterIndex_) {
    other.master_ = nullptr;
}

MasterGuard &MasterGuard::operator=(MasterGuard &&other) noexcept {
    if (this != &other) {
        if (master_ && master_ != cachedMaster_) {
            ecrt_release_master(master_);
        }
        lock_ = std::move(other.lock_);
        master_ = other.master_;
        cachedMaster_ = other.cachedMaster_;
        cachedMasterIndex_ = other.cachedMasterIndex_;
        other.master_ = nullptr;
    }
    return *this;
}

MasterGuard EthercatNativeBackend::acquireMaster(const QString &master) const {
    bool ok = false;
    int index = master.isEmpty() ? 0 : master.toInt(&ok);
    if (!ok) {
        index = 0;
    }
    return MasterGuard(masterMutex_, cachedMaster_, cachedMasterIndex_, index);
}

// Task 3 helpers

QString EthercatNativeBackend::alStateToString(uint8_t state) {
    switch (state) {
    case 1:  return QStringLiteral("INIT");
    case 2:  return QStringLiteral("PREOP");
    case 4:  return QStringLiteral("SAFEOP");
    case 8:  return QStringLiteral("OP");
    default: return QString::number(state);
    }
}

QString EthercatNativeBackend::slaveFlagsToString(const ec_slave_info_t &info) {
    QString flags;
    if (info.error_flag) {
        flags += QStringLiteral("E");
    }
    for (int i = 0; i < EC_MAX_PORTS; ++i) {
        if (info.ports[i].desc != EC_PORT_NOT_IMPLEMENTED) {
            flags += info.ports[i].link.link_up ? QStringLiteral("L") : QStringLiteral("-");
        }
    }
    return flags;
}

SlaveInfo EthercatNativeBackend::convertSlaveInfo(
    const ec_slave_info_t &info) const {
    SlaveInfo slave;
    slave.position = info.position;
    slave.state = alStateToString(info.al_state);
    slave.flags = slaveFlagsToString(info);
    slave.name = QString::fromLatin1(info.name);
    slave.rawLine = QString("%1  %2  %3  %4")
                        .arg(info.position)
                        .arg(info.alias)
                        .arg(slave.state, slave.flags, slave.name);
    return slave;
}

#endif // HAVE_IGH

EthercatNativeBackend::EthercatNativeBackend(QObject *parent)
    : QObject(parent) {}

EthercatNativeBackend::~EthercatNativeBackend() {
#ifdef HAVE_IGH
    if (cachedMaster_) {
        ecrt_release_master(cachedMaster_);
        cachedMaster_ = nullptr;
        cachedMasterIndex_ = -1;
    }
#endif
}

// Task 3: Master status and slave scanning

QString EthercatNativeBackend::masterText(const QString &master,
                                          QString *error) const {
#ifdef HAVE_IGH
    auto guard = acquireMaster(master);
    if (!guard.get()) {
        if (error) *error = "Failed to open EtherCAT master";
        return {};
    }

    ec_master_info_t info;
    if (ecrt_master(guard.get(), &info) != 0) {
        if (error) *error = "ecrt_master() failed";
        return {};
    }

    QString text;
    QTextStream ts(&text);
    ts << "Slaves: " << info.slave_count << "\n";
    ts << "Link: " << (info.link_up ? "UP" : "DOWN") << "\n";
    ts << "Scan busy: " << (info.scan_busy ? "yes" : "no") << "\n";
    ts << "App time: " << info.app_time << "\n";
    return text;
#else
    Q_UNUSED(master)
    if (error) *error = "IgH support not compiled";
    return {};
#endif
}

QVector<SlaveInfo> EthercatNativeBackend::scanSlaves(const QString &master,
                                                     QString *error) const {
#ifdef HAVE_IGH
    auto guard = acquireMaster(master);
    if (!guard.get()) {
        if (error) *error = "Failed to open EtherCAT master";
        return {};
    }

    ec_master_info_t info;
    if (ecrt_master(guard.get(), &info) != 0) {
        if (error) *error = "ecrt_master() failed";
        return {};
    }

    QVector<SlaveInfo> slaves;
    slaves.reserve(info.slave_count);
    for (unsigned int i = 0; i < info.slave_count; ++i) {
        ec_slave_info_t si;
        if (ecrt_master_get_slave(guard.get(), static_cast<uint16_t>(i),
                                  &si) == 0) {
            slaves.append(convertSlaveInfo(si));
        }
    }
    return slaves;
#else
    Q_UNUSED(master)
    if (error) *error = "IgH support not compiled";
    return {};
#endif
}

// Task 8: Slave info

QString EthercatNativeBackend::slaveInfo(const QString &master, int position,
                                         QString *error) const {
#ifdef HAVE_IGH
    auto guard = acquireMaster(master);
    if (!guard.get()) {
        if (error) *error = "Failed to open EtherCAT master";
        return {};
    }

    ec_slave_info_t si;
    if (ecrt_master_get_slave(guard.get(), static_cast<uint16_t>(position),
                              &si) != 0) {
        if (error) *error = "ecrt_master_get_slave() failed";
        return {};
    }

    QString text;
    QTextStream ts(&text);
    ts << "Position: " << si.position << "\n";
    ts << "Vendor ID: 0x" << QString::number(si.vendor_id, 16) << "\n";
    ts << "Product Code: 0x" << QString::number(si.product_code, 16) << "\n";
    ts << "Revision: 0x" << QString::number(si.revision_number, 16) << "\n";
    ts << "Serial: 0x" << QString::number(si.serial_number, 16) << "\n";
    ts << "Alias: " << si.alias << "\n";
    ts << "AL State: " << alStateToString(si.al_state) << "\n";
    ts << "Error flag: " << si.error_flag << "\n";
    ts << "Sync managers: " << si.sync_count << "\n";
    ts << "SDO count: " << si.sdo_count << "\n";
    ts << "Name: " << QString::fromLatin1(si.name) << "\n";
    ts << "Current on EBus: " << si.current_on_ebus << " mA\n";
    for (int p = 0; p < EC_MAX_PORTS; ++p) {
        const auto &port = si.ports[p];
        if (port.desc == EC_PORT_NOT_IMPLEMENTED) continue;
        ts << "Port " << p << ": ";
        switch (port.desc) {
        case EC_PORT_NOT_CONFIGURED: ts << "not-configured"; break;
        case EC_PORT_EBUS: ts << "EBus"; break;
        case EC_PORT_MII: ts << "MII"; break;
        default: ts << "unknown"; break;
        }
        ts << " link=" << (port.link.link_up ? "up" : "down")
           << " loop=" << (port.link.loop_closed ? "closed" : "open")
           << " signal=" << (port.link.signal_detected ? "yes" : "no")
           << "\n";
    }
    return text;
#else
    Q_UNUSED(master)
    Q_UNUSED(position)
    if (error) *error = "IgH support not compiled";
    return {};
#endif
}

// Task 8: ESI XML — not available via ecrt API

QString EthercatNativeBackend::slaveXml(const QString &master, int position,
                                        QString *error) const {
#ifdef HAVE_IGH
    // Native ecrt API does not expose ESI XML.
    // Fall back to CLI for this operation.
    lastFallback_ = true;
    lastFallbackReason_ = "ecrt API does not expose ESI XML";
    return cliFallback_.slaveXml(master, position, error);
#else
    Q_UNUSED(master)
    Q_UNUSED(position)
    if (error) *error = "IgH support not compiled";
    return {};
#endif
}

// Task 7: PDO dictionary

QString EthercatNativeBackend::pdos(const QString &master, int position,
                                    QString *error) const {
#ifdef HAVE_IGH
    auto guard = acquireMaster(master);
    if (!guard.get()) {
        if (error) *error = "Failed to open EtherCAT master";
        return {};
    }

    ec_slave_info_t si;
    if (ecrt_master_get_slave(guard.get(), static_cast<uint16_t>(position),
                              &si) != 0) {
        if (error) *error = "ecrt_master_get_slave() failed";
        return {};
    }

    QString text;
    QTextStream ts(&text);

    for (uint8_t sm = 0; sm < si.sync_count; ++sm) {
        ec_sync_info_t sync;
        if (ecrt_master_get_sync_manager(guard.get(),
                static_cast<uint16_t>(position), sm, &sync) != 0) {
            continue;
        }

        const char *dirStr = "???";
        switch (sync.dir) {
        case EC_DIR_OUTPUT: dirStr = "OUTPUT"; break;
        case EC_DIR_INPUT:  dirStr = "INPUT"; break;
        default: break;
        }

        ts << "SM" << sm << " dir=" << dirStr
           << " wd=" << sync.watchdog_mode << "\n";

        for (unsigned int p = 0; ; ++p) {
            ec_pdo_info_t pdo;
            if (ecrt_master_get_pdo(guard.get(),
                    static_cast<uint16_t>(position), sm,
                    static_cast<uint16_t>(p), &pdo) != 0) {
                break;
            }
            ts << "  PDO 0x" << QString::number(pdo.index, 16)
               << " (" << pdo.n_entries << " entries)\n";

            for (unsigned int e = 0; e < pdo.n_entries; ++e) {
                ec_pdo_entry_info_t entry;
                if (ecrt_master_get_pdo_entry(guard.get(),
                        static_cast<uint16_t>(position), sm,
                        static_cast<uint16_t>(p),
                        static_cast<uint16_t>(e), &entry) != 0) {
                    break;
                }
                ts << "    0x" << QString::number(entry.index, 16)
                   << ":" << entry.subindex
                   << " (" << entry.bit_length << " bit)\n";
            }
        }
    }
    return text;
#else
    Q_UNUSED(master)
    Q_UNUSED(position)
    if (error) *error = "IgH support not compiled";
    return {};
#endif
}

// Task 7: SDO dictionary — ecrt does not support enumeration

QString EthercatNativeBackend::sdos(const QString &master, int position,
                                    QString *error) const {
#ifdef HAVE_IGH
    // Native ecrt API does not support SDO dictionary enumeration.
    // Fall back to CLI for this operation.
    lastFallback_ = true;
    lastFallbackReason_ = "ecrt API does not support SDO dictionary enumeration";
    return cliFallback_.sdos(master, position, error);
#else
    Q_UNUSED(master)
    Q_UNUSED(position)
    if (error) *error = "IgH support not compiled";
    return {};
#endif
}

// Task 4: SDO upload (read)

QString EthercatNativeBackend::upload(const QString &master, int position,
                                      const QString &index,
                                      const QString &subIndex,
                                      const QString &type,
                                      QString *error) const {
#ifdef HAVE_IGH
    auto guard = acquireMaster(master);
    if (!guard.get()) {
        if (error) *error = "Failed to open EtherCAT master";
        return {};
    }

    bool ok = false;
    uint16_t idx = index.toUShort(&ok, 16);
    if (!ok) {
        if (error) *error = "Invalid SDO index: " + index;
        return {};
    }
    uint8_t sub = static_cast<uint8_t>(subIndex.toUShort(&ok, 16));
    if (!ok) {
        if (error) *error = "Invalid SDO subindex: " + subIndex;
        return {};
    }

    uint8_t buf[MAX_SDO_DATA_SIZE];
    size_t resultSize = 0;
    uint32_t abortCode = 0;
    int ret = ecrt_master_sdo_upload(guard.get(),
                                     static_cast<uint16_t>(position),
                                     idx, sub, buf, sizeof(buf),
                                     &resultSize, &abortCode);
    if (ret != 0) {
        if (error) {
            *error = QString("SDO upload failed (ret=%1, abort=0x%2)")
                         .arg(ret)
                         .arg(abortCode, 8, 16, QChar('0'));
        }
        return {};
    }

    if (resultSize == 0) return {};

    QString typeLower = type.trimmed().toLower();
    if (typeLower == "uint8" || typeLower == "u8") {
        if (resultSize >= 1) return QString::number(buf[0]);
    } else if (typeLower == "uint16" || typeLower == "u16") {
        if (resultSize >= 2) {
            uint16_t val;
            memcpy(&val, buf, sizeof(val));
            return QString::number(val);
        }
    } else if (typeLower == "uint32" || typeLower == "u32") {
        if (resultSize >= 4) {
            uint32_t val;
            memcpy(&val, buf, sizeof(val));
            return QString::number(val);
        }
    } else if (typeLower == "int8" || typeLower == "s8") {
        if (resultSize >= 1) return QString::number(static_cast<int8_t>(buf[0]));
    } else if (typeLower == "int16" || typeLower == "s16") {
        if (resultSize >= 2) {
            int16_t val;
            memcpy(&val, buf, sizeof(val));
            return QString::number(val);
        }
    } else if (typeLower == "int32" || typeLower == "s32") {
        if (resultSize >= 4) {
            int32_t val;
            memcpy(&val, buf, sizeof(val));
            return QString::number(val);
        }
    } else if (typeLower == "string" || typeLower == "visible_string") {
        return QString::fromLatin1(reinterpret_cast<const char *>(buf),
                                   static_cast<int>(resultSize));
    }

    // Default: hex dump
    QString hex;
    for (size_t i = 0; i < resultSize; ++i) {
        hex += QString::number(buf[i], 16).rightJustified(2, '0');
    }
    return hex;
#else
    Q_UNUSED(master)
    Q_UNUSED(position)
    Q_UNUSED(index)
    Q_UNUSED(subIndex)
    Q_UNUSED(type)
    if (error) *error = "IgH support not compiled";
    return {};
#endif
}

// Task 5: SDO download (write)

bool EthercatNativeBackend::download(const QString &master, int position,
                                     const QString &index,
                                     const QString &subIndex,
                                     const QString &value, const QString &type,
                                     QString *error) const {
#ifdef HAVE_IGH
    auto guard = acquireMaster(master);
    if (!guard.get()) {
        if (error) *error = "Failed to open EtherCAT master";
        return false;
    }

    bool ok = false;
    uint16_t idx = index.toUShort(&ok, 16);
    if (!ok) {
        if (error) *error = "Invalid SDO index: " + index;
        return false;
    }
    uint8_t sub = static_cast<uint8_t>(subIndex.toUShort(&ok, 16));
    if (!ok) {
        if (error) *error = "Invalid SDO subindex: " + subIndex;
        return false;
    }

    uint8_t data[MAX_SDO_DATA_SIZE];
    size_t dataSize = 0;
    QString typeLower = type.trimmed().toLower();

    if (typeLower == "uint8" || typeLower == "u8") {
        uint8_t val = static_cast<uint8_t>(value.toUInt(&ok));
        if (!ok) { if (error) *error = "Invalid uint8 value"; return false; }
        memcpy(data, &val, sizeof(val));
        dataSize = sizeof(val);
    } else if (typeLower == "uint16" || typeLower == "u16") {
        uint16_t val = static_cast<uint16_t>(value.toUInt(&ok));
        if (!ok) { if (error) *error = "Invalid uint16 value"; return false; }
        memcpy(data, &val, sizeof(val));
        dataSize = sizeof(val);
    } else if (typeLower == "uint32" || typeLower == "u32") {
        uint32_t val = value.toUInt(&ok);
        if (!ok) { if (error) *error = "Invalid uint32 value"; return false; }
        memcpy(data, &val, sizeof(val));
        dataSize = sizeof(val);
    } else if (typeLower == "int8" || typeLower == "s8") {
        int8_t val = static_cast<int8_t>(value.toInt(&ok));
        if (!ok) { if (error) *error = "Invalid int8 value"; return false; }
        memcpy(data, &val, sizeof(val));
        dataSize = sizeof(val);
    } else if (typeLower == "int16" || typeLower == "s16") {
        int16_t val = static_cast<int16_t>(value.toInt(&ok));
        if (!ok) { if (error) *error = "Invalid int16 value"; return false; }
        memcpy(data, &val, sizeof(val));
        dataSize = sizeof(val);
    } else if (typeLower == "int32" || typeLower == "s32") {
        int32_t val = value.toInt(&ok);
        if (!ok) { if (error) *error = "Invalid int32 value"; return false; }
        memcpy(data, &val, sizeof(val));
        dataSize = sizeof(val);
    } else if (typeLower == "string" || typeLower == "visible_string") {
        QByteArray ba = value.toLatin1();
        dataSize = static_cast<size_t>(ba.size());
        if (dataSize > MAX_SDO_DATA_SIZE) dataSize = MAX_SDO_DATA_SIZE;
        memcpy(data, ba.constData(), dataSize);
    } else {
        // Hex string fallback
        QString hex = value.trimmed();
        if (hex.startsWith("0x") || hex.startsWith("0X")) hex = hex.mid(2);
        QByteArray ba = QByteArray::fromHex(hex.toLatin1());
        dataSize = static_cast<size_t>(ba.size());
        if (dataSize > MAX_SDO_DATA_SIZE) dataSize = MAX_SDO_DATA_SIZE;
        memcpy(data, ba.constData(), dataSize);
    }

    uint32_t abortCode = 0;
    int ret = ecrt_master_sdo_download(guard.get(),
                                       static_cast<uint16_t>(position),
                                       idx, sub, data, dataSize, &abortCode);
    if (ret != 0) {
        if (error) {
            *error = QString("SDO download failed (ret=%1, abort=0x%2)")
                         .arg(ret)
                         .arg(abortCode, 8, 16, QChar('0'));
        }
        return false;
    }
    return true;
#else
    Q_UNUSED(master)
    Q_UNUSED(position)
    Q_UNUSED(index)
    Q_UNUSED(subIndex)
    Q_UNUSED(value)
    Q_UNUSED(type)
    if (error) *error = "IgH support not compiled";
    return false;
#endif
}

// Task 6: State management — ecrt does not support direct AL state transitions

bool EthercatNativeBackend::setState(const QString &master, int position,
                                     const QString &state,
                                     QString *error) const {
#ifdef HAVE_IGH
    // Native ecrt API doesn't support AL state transitions.
    // Fall back to CLI for this operation.
    lastFallback_ = true;
    lastFallbackReason_ = "ecrt API does not support AL state transitions";
    return cliFallback_.setState(master, position, state, error);
#else
    Q_UNUSED(master)
    Q_UNUSED(position)
    Q_UNUSED(state)
    if (error) *error = "IgH support not compiled";
    return false;
#endif
}

bool EthercatNativeBackend::setAllStates(const QString &master,
                                         const QString &state,
                                         QString *error) const {
#ifdef HAVE_IGH
    // Native ecrt API doesn't support AL state transitions.
    // Fall back to CLI for this operation.
    lastFallback_ = true;
    lastFallbackReason_ = "ecrt API does not support AL state transitions";
    return cliFallback_.setAllStates(master, state, error);
#else
    Q_UNUSED(master)
    Q_UNUSED(state)
    if (error) *error = "IgH support not compiled";
    return false;
#endif
}

// Task 9: Rescan — ecrt does not have a direct rescan API

bool EthercatNativeBackend::rescan(const QString &master,
                                   QString *error) const {
#ifdef HAVE_IGH
    // Native ecrt API doesn't have a direct rescan API.
    // Fall back to CLI for this operation.
    lastFallback_ = true;
    lastFallbackReason_ = "ecrt API does not have a rescan API";
    return cliFallback_.rescan(master, error);
#else
    Q_UNUSED(master)
    if (error) *error = "IgH support not compiled";
    return false;
#endif
}

// Task 9: Host diagnostics — check if the master can be opened

QJsonArray EthercatNativeBackend::hostDiagnostics(QString *error) const {
    QJsonArray checks;
#ifdef HAVE_IGH
    {
        ec_master_t *m = ecrt_open_master(0);
        if (m) {
            ecrt_release_master(m);
            checks.append(QJsonObject{
                {"level", "Info"},
                {"source", "Native API"},
                {"message", "ecrt_open_master(0) succeeded"},
                {"hint", "Master is accessible via the native ecrt API."},
            });
        } else {
            checks.append(QJsonObject{
                {"level", "Error"},
                {"source", "Native API"},
                {"message", "ecrt_open_master(0) failed"},
                {"hint", "Check /dev/EtherCAT0 permissions and ec_master module."},
            });
        }
    }
#else
    Q_UNUSED(error)
    checks.append(QJsonObject{
        {"level", "Warning"},
        {"source", "Native API"},
        {"message", "IgH ecrt support is not compiled in"},
        {"hint", "Install IgH EtherCAT Master headers and rebuild."},
    });
#endif

    // Append host-level diagnostics from the CLI backend (kernel version,
    // module status, config, NIC, device node, DKMS, ethercatctl).
    const QJsonArray cliChecks = cliFallback_.hostDiagnostics(error);
    for (const auto &item : cliChecks) {
        checks.append(item);
    }

    return checks;
}
