#include "EsiDeviceMatcher.h"
#include "services/EsiService.h"
#include "EsiParser.h"

EsiDeviceMatcher::EsiDeviceMatcher(EsiService *esiService, QObject *parent)
    : QObject(parent), esiService_(esiService) {}

EsiDeviceMatcher::EsiDeviceMatcher(EsiParser *esiParser, QObject *parent)
    : QObject(parent), esiParser_(esiParser) {}

EsiDeviceMatcher::MatchResult EsiDeviceMatcher::matchDevice(
    int vendorId, int productCode, int revisionNo) {
    EsiDeviceInfo esi;
    if (esiService_) {
        esi = esiService_->matchDevice(vendorId, productCode);
    } else if (esiParser_) {
        esi = esiParser_->matchDevice(vendorId, productCode);
    }
    return compareDevices(esi, vendorId, productCode, revisionNo);
}

EsiDeviceMatcher::MatchReport EsiDeviceMatcher::generateReport(
    const QVector<QPair<int, int>> &connectedDevices) {
    MatchReport report;
    report.totalDevices = connectedDevices.size();

    for (const auto &dev : connectedDevices) {
        MatchResult result = matchDevice(dev.first, dev.second);
        report.results.append(result);
        if (result.matched)
            ++report.matchedDevices;
        else
            ++report.unmatchedDevices;
    }

    return report;
}

bool EsiDeviceMatcher::hasMatch(int vendorId, int productCode) const {
    if (esiService_) {
        EsiDeviceInfo esi = esiService_->matchDevice(vendorId, productCode);
        return esi.vendorId != 0;
    }
    if (esiParser_) {
        EsiDeviceInfo esi = esiParser_->matchDevice(vendorId, productCode);
        return esi.vendorId != 0;
    }
    return false;
}

EsiDeviceInfo EsiDeviceMatcher::matchedDeviceInfo(int vendorId, int productCode) const {
    if (esiService_)
        return esiService_->matchDevice(vendorId, productCode);
    if (esiParser_)
        return esiParser_->matchDevice(vendorId, productCode);
    return EsiDeviceInfo();
}

EsiDeviceMatcher::MatchResult EsiDeviceMatcher::compareDevices(
    const EsiDeviceInfo &esi, int vendorId, int productCode, int revisionNo) {
    MatchResult result;
    result.vendorId = vendorId;
    result.productCode = productCode;
    result.revisionNo = revisionNo;

    if (esi.vendorId == 0) {
        result.matched = false;
        result.matchLevel = "none";
        result.differences << "No ESI entry found for this device";
        return result;
    }

    result.matched = true;
    result.esiDeviceId = esi.deviceId;
    result.esiDeviceName = esi.name;

    if (esi.vendorId != vendorId) {
        result.differences << QStringLiteral("Vendor ID mismatch: ESI=0x%1, Device=0x%2")
            .arg(esi.vendorId, 8, 16, QChar('0'))
            .arg(vendorId, 8, 16, QChar('0'));
    }
    if (esi.productCode != productCode) {
        result.differences << QStringLiteral("Product code mismatch: ESI=0x%1, Device=0x%2")
            .arg(esi.productCode, 8, 16, QChar('0'))
            .arg(productCode, 8, 16, QChar('0'));
    }
    if (revisionNo != 0 && esi.revisionNo != revisionNo) {
        result.differences << QStringLiteral("Revision mismatch: ESI=0x%1, Device=0x%2")
            .arg(esi.revisionNo, 8, 16, QChar('0'))
            .arg(revisionNo, 8, 16, QChar('0'));
    }

    if (result.differences.isEmpty())
        result.matchLevel = "exact";
    else
        result.matchLevel = "partial";

    if (result.matched && result.differences.isEmpty())
        emit matchFound(esi.name, vendorId, productCode);

    return result;
}
