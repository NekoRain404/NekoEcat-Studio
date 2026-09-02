#pragma once

#include <QObject>
#include <QString>
#include <QVector>

class EsiService;
class EsiParser;
struct EsiDeviceInfo;

class EsiDeviceMatcher : public QObject {
    Q_OBJECT
public:
    struct MatchResult {
        bool matched = false;
        QString esiDeviceId;
        QString esiDeviceName;
        int vendorId = 0;
        int productCode = 0;
        int revisionNo = 0;
        QString matchLevel;
        QStringList differences;
    };

    struct MatchReport {
        int totalDevices = 0;
        int matchedDevices = 0;
        int unmatchedDevices = 0;
        QVector<MatchResult> results;
    };

    explicit EsiDeviceMatcher(EsiService* esiService, QObject* parent = nullptr);
    explicit EsiDeviceMatcher(EsiParser* esiParser, QObject* parent = nullptr);

    MatchResult matchDevice(int vendorId, int productCode, int revisionNo = 0);
    MatchReport generateReport(const QVector<QPair<int, int>>& connectedDevices);

    bool hasMatch(int vendorId, int productCode) const;
    EsiDeviceInfo matchedDeviceInfo(int vendorId, int productCode) const;

signals:
    void matchFound(const QString& deviceName, int vendorId, int productCode);

private:
    MatchResult compareDevices(const EsiDeviceInfo& esi, int vendorId, int productCode, int revisionNo);

    EsiService* esiService_ = nullptr;
    EsiParser* esiParser_ = nullptr;
};
