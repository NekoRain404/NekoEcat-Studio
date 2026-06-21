#include "WorkflowCertificationManagerService.h"

WorkflowCertificationManagerService::WorkflowCertificationManagerService(QObject *parent)
    : QObject(parent)
{
}

QString WorkflowCertificationManagerService::addRequirement(
    const QString &name, const QString &standard, const QDateTime &expiry)
{
    WfCertRequirement r;
    r.id = QStringLiteral("cm-%1").arg(nextId_++);
    r.name = name;
    r.standard = standard;
    r.status = QStringLiteral("pending");
    r.createdAt = QDateTime::currentDateTime();
    r.expiry = expiry.isValid() ? expiry : r.createdAt.addYears(1);
    requirements_.append(r);
    emit requirementAdded(r.id);
    return r.id;
}

bool WorkflowCertificationManagerService::removeRequirement(const QString &reqId)
{
    for (int i = 0; i < requirements_.size(); ++i) {
        if (requirements_[i].id == reqId) {
            requirements_.removeAt(i);
            emit requirementRemoved(reqId);
            return true;
        }
    }
    return false;
}

bool WorkflowCertificationManagerService::updateStatus(const QString &reqId,
                                                        const QString &status)
{
    for (auto &r : requirements_) {
        if (r.id == reqId) {
            r.status = status;
            emit statusUpdated(reqId, status);
            return true;
        }
    }
    return false;
}

WfCertRequirement WorkflowCertificationManagerService::requirement(
    const QString &reqId) const
{
    for (const auto &r : requirements_) {
        if (r.id == reqId)
            return r;
    }
    return {};
}

QVector<WfCertRequirement> WorkflowCertificationManagerService::allRequirements() const
{
    return requirements_;
}

int WorkflowCertificationManagerService::requirementCount() const
{
    return requirements_.size();
}

QVector<WfCertRequirement> WorkflowCertificationManagerService::requirementsByStandard(
    const QString &standard) const
{
    QVector<WfCertRequirement> result;
    for (const auto &r : requirements_) {
        if (r.standard == standard)
            result.append(r);
    }
    return result;
}

bool WorkflowCertificationManagerService::renewRequirement(
    const QString &reqId, const QDateTime &newExpiry)
{
    for (auto &r : requirements_) {
        if (r.id == reqId) {
            r.expiry = newExpiry;
            r.status = QStringLiteral("renewed");
            emit requirementRenewed(reqId);
            return true;
        }
    }
    return false;
}

int WorkflowCertificationManagerService::pendingCount() const
{
    int count = 0;
    for (const auto &r : requirements_) {
        if (r.status == QStringLiteral("pending"))
            ++count;
    }
    return count;
}
