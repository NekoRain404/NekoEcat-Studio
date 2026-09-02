#include "ResourceManagementService.h"

// ResourceManagementService.cpp — Allocates, tracks, and optimizes resource utilization
//
// Implementation notes:
//   - Resources carry type, capacity, skills, and cost-per-hour metadata
//   - Utilization tracked as currentLoad vs capacity percentage
//   - Optimization analyzes usage patterns and generates recommendations

ResourceManagementService::ResourceManagementService(QObject* parent) : QObject(parent) {}

Resource ResourceManagementService::allocateResource(const ResourceConfig& config) {
    if (config.name.trimmed().isEmpty() || config.capacity <= 0 || config.costPerHour < 0.0)
        return {};

    Resource r;
    r.id = nextResourceId_++;
    r.name = config.name;
    r.type = config.type;
    r.capacity = config.capacity;
    r.available = config.available;
    r.costPerHour = config.costPerHour;
    r.location = config.location;
    r.skills = config.skills;
    r.status = config.available ? ResourceStatus::Available : ResourceStatus::Unavailable;
    r.currentLoad = 0;
    r.createdAt = QDateTime::currentDateTime();
    r.updatedAt = r.createdAt;

    resources_.insert(r.id, r);
    emit resourceAllocated(r);
    return r;
}

ResourceStatusInfo ResourceManagementService::trackResource(int resourceId) const {
    ResourceStatusInfo info;
    auto it = resources_.find(resourceId);
    if (it == resources_.end())
        return info;

    const Resource& r = *it;
    info.resourceId = r.id;
    info.status = r.status;
    info.currentLoad = r.currentLoad;
    info.capacity = r.capacity;
    if (r.capacity > 0)
        info.utilizationPercent = (r.currentLoad * 100) / r.capacity;
    info.lastUpdated = r.updatedAt;
    return info;
}

ResourceOptimizationResult ResourceManagementService::optimizeResources() const {
    ResourceOptimizationResult result;
    result.generatedAt = QDateTime::currentDateTime();

    for (const auto& r : resources_) {
        if (r.status == ResourceStatus::Unavailable)
            continue;

        int utilization = 0;
        if (r.capacity > 0)
            utilization = (r.currentLoad * 100) / r.capacity;

        if (utilization < 30 && r.costPerHour > 0.0) {
            ResourceOptimizationSuggestion s;
            s.resourceId = r.id;
            s.resourceName = r.name;
            s.suggestion = QStringLiteral("Underutilized (%1%%) — consider reassigning or releasing").arg(utilization);
            s.potentialSaving = static_cast<int>(r.costPerHour * 0.5);
            result.suggestions.append(s);
            result.totalPotentialSaving += s.potentialSaving;
        } else if (utilization > 90) {
            ResourceOptimizationSuggestion s;
            s.resourceId = r.id;
            s.resourceName = r.name;
            s.suggestion = QStringLiteral("Overloaded (%1%%) — consider adding capacity").arg(utilization);
            s.potentialSaving = 0;
            result.suggestions.append(s);
        }
    }

    return result;
}

ResourceReport ResourceManagementService::generateResourceReport() const {
    ResourceReport report;
    report.generatedAt = QDateTime::currentDateTime();
    report.totalResources = resources_.size();

    for (const auto& r : resources_) {
        switch (r.status) {
            case ResourceStatus::Available:
                report.availableResources++;
                break;
            case ResourceStatus::Allocated:
                report.allocatedResources++;
                break;
            case ResourceStatus::Maintenance:
                report.maintenanceResources++;
                break;
            default:
                break;
        }
        report.totalCostPerHour += r.costPerHour;
        report.resources.append(r);
    }

    for (const auto& allocs : allocations_) {
        for (const auto& a : allocs)
            report.allocations.append(a);
    }

    return report;
}

bool ResourceManagementService::updateResourceStatus(int resourceId, ResourceStatus status) {
    auto it = resources_.find(resourceId);
    if (it == resources_.end())
        return false;

    it->status = status;
    it->updatedAt = QDateTime::currentDateTime();
    emit resourceUpdated(trackResource(resourceId));
    return true;
}

bool ResourceManagementService::allocateToProject(int resourceId, int projectId, int percent) {
    auto it = resources_.find(resourceId);
    if (it == resources_.end())
        return false;
    if (projectId <= 0 || percent <= 0 || percent > 100)
        return false;

    ResourceAllocationEntry entry;
    entry.resourceId = resourceId;
    entry.projectId = projectId;
    entry.allocationPercent = percent;
    entry.startDate = QDateTime::currentDateTime();
    entry.active = true;

    allocations_[resourceId].append(entry);
    it->currentLoad += percent;
    if (it->currentLoad > it->capacity)
        it->currentLoad = it->capacity;
    it->status = ResourceStatus::Allocated;
    it->updatedAt = QDateTime::currentDateTime();

    emit resourceUpdated(trackResource(resourceId));
    return true;
}

bool ResourceManagementService::releaseFromProject(int resourceId, int projectId) {
    auto it = resources_.find(resourceId);
    if (it == resources_.end())
        return false;

    auto allocIt = allocations_.find(resourceId);
    if (allocIt == allocations_.end())
        return false;

    for (auto& entry : *allocIt) {
        if (entry.projectId == projectId && entry.active) {
            entry.active = false;
            it->currentLoad -= entry.allocationPercent;
            if (it->currentLoad < 0)
                it->currentLoad = 0;
            it->updatedAt = QDateTime::currentDateTime();

            bool hasActive = false;
            for (const auto& e : *allocIt) {
                if (e.active) {
                    hasActive = true;
                    break;
                }
            }
            if (!hasActive)
                it->status = ResourceStatus::Available;

            emit resourceUpdated(trackResource(resourceId));
            return true;
        }
    }
    return false;
}

bool ResourceManagementService::updateLoad(int resourceId, int load) {
    auto it = resources_.find(resourceId);
    if (it == resources_.end())
        return false;
    if (load < 0 || load > it->capacity)
        return false;

    it->currentLoad = load;
    it->updatedAt = QDateTime::currentDateTime();
    emit resourceUpdated(trackResource(resourceId));
    return true;
}

Resource ResourceManagementService::resource(int resourceId) const {
    auto it = resources_.find(resourceId);
    if (it != resources_.end())
        return *it;
    return {};
}

QVector<Resource> ResourceManagementService::allResources() const {
    QVector<Resource> result;
    result.reserve(resources_.size());
    for (const auto& r : resources_)
        result.append(r);
    return result;
}

QVector<Resource> ResourceManagementService::resourcesByType(ResourceType type) const {
    QVector<Resource> result;
    for (const auto& r : resources_) {
        if (r.type == type)
            result.append(r);
    }
    return result;
}

QVector<Resource> ResourceManagementService::availableResources() const {
    QVector<Resource> result;
    for (const auto& r : resources_) {
        if (r.status == ResourceStatus::Available)
            result.append(r);
    }
    return result;
}

QVector<ResourceAllocationEntry> ResourceManagementService::resourceAllocations(int resourceId) const {
    auto it = allocations_.find(resourceId);
    if (it != allocations_.end())
        return *it;
    return {};
}

int ResourceManagementService::resourceCount() const {
    return resources_.size();
}
