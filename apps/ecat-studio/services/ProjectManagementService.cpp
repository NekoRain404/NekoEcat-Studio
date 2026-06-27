#include "ProjectManagementService.h"

// ProjectManagementService.cpp — CRUD operations for projects, milestones, and deliverables
//
// Implementation notes:
//   - Auto-incrementing IDs for projects, milestones, and deliverables
//   - Status tracking aggregates milestone/deliverable completion
//   - Supports scope-based project filtering and progress reporting

namespace {
bool isValidResourceAllocation(const ResourceAllocation &resource)
{
    return !resource.resourceName.trimmed().isEmpty()
           && resource.allocationPercent > 0
           && resource.allocationPercent <= 100
           && resource.costPerHour >= 0.0;
}

bool isValidMilestone(const Milestone &milestone)
{
    return !milestone.name.trimmed().isEmpty();
}

bool isValidDeliverable(const Deliverable &deliverable)
{
    return !deliverable.name.trimmed().isEmpty();
}
}

ProjectManagementService::ProjectManagementService(QObject *parent)
    : QObject(parent)
{
}

Project ProjectManagementService::createProject(const ProjectConfig &config)
{
    if (config.name.trimmed().isEmpty())
        return {};
    if (config.startDate.isValid() && config.endDate.isValid() && config.endDate < config.startDate)
        return {};
    for (const auto &resource : config.resources) {
        if (!isValidResourceAllocation(resource))
            return {};
    }
    for (const auto &milestone : config.milestones) {
        if (!isValidMilestone(milestone))
            return {};
    }
    for (const auto &deliverable : config.deliverables) {
        if (!isValidDeliverable(deliverable))
            return {};
    }

    Project p;
    p.id = nextProjectId_++;
    p.name = config.name;
    p.description = config.description;
    p.scope = config.scope;
    p.startDate = config.startDate;
    p.endDate = config.endDate;
    p.status = ProjectStatus::NotStarted;
    p.resources = config.resources;
    p.createdAt = QDateTime::currentDateTime();
    p.updatedAt = p.createdAt;

    for (auto m : config.milestones) {
        m.id = nextMilestoneId_++;
        p.milestones.append(m);
    }
    for (auto d : config.deliverables) {
        d.id = nextDeliverableId_++;
        p.deliverables.append(d);
    }

    projects_.insert(p.id, p);
    emit projectCreated(p);
    return p;
}

ProjectStatusInfo ProjectManagementService::trackProject(int projectId) const
{
    ProjectStatusInfo info;
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return info;

    const Project &p = *it;
    info.projectId = p.id;
    info.status = p.status;
    info.totalMilestones = p.milestones.size();
    info.totalDeliverables = p.deliverables.size();

    for (const auto &m : p.milestones) {
        if (m.completed)
            info.completedMilestones++;
    }
    for (const auto &d : p.deliverables) {
        if (d.delivered)
            info.deliveredItems++;
    }

    int totalItems = info.totalMilestones + info.totalDeliverables;
    if (totalItems > 0)
        info.completionPercent = ((info.completedMilestones + info.deliveredItems) * 100) / totalItems;

    info.lastUpdated = p.updatedAt;
    return info;
}

ProjectReport ProjectManagementService::generateReport(int projectId) const
{
    ProjectReport report;
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return report;

    const Project &p = *it;
    report.projectId = p.id;
    report.projectName = p.name;
    report.status = p.status;
    report.milestones = p.milestones;
    report.deliverables = p.deliverables;
    report.generatedAt = QDateTime::currentDateTime();

    auto histIt = history_.find(projectId);
    if (histIt != history_.end())
        report.recentActivity = *histIt;

    ProjectStatusInfo info = trackProject(projectId);
    report.completionPercent = info.completionPercent;

    return report;
}

bool ProjectManagementService::collaborate(int projectId, const CollaborationEntry &collab)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;
    if (collab.user.trimmed().isEmpty() || collab.action.trimmed().isEmpty())
        return false;

    CollaborationEntry entry = collab;
    if (!entry.timestamp.isValid())
        entry.timestamp = QDateTime::currentDateTime();

    history_[projectId].append(entry);
    if (history_[projectId].size() > kMaxHistory)
        history_[projectId].removeFirst();

    it->updatedAt = QDateTime::currentDateTime();
    emit projectUpdated(trackProject(projectId));
    return true;
}

bool ProjectManagementService::updateProjectStatus(int projectId, ProjectStatus status)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;

    it->status = status;
    it->updatedAt = QDateTime::currentDateTime();
    emit projectUpdated(trackProject(projectId));
    return true;
}

bool ProjectManagementService::addMilestone(int projectId, const Milestone &milestone)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;
    if (!isValidMilestone(milestone))
        return false;

    Milestone m = milestone;
    m.id = nextMilestoneId_++;
    it->milestones.append(m);
    it->updatedAt = QDateTime::currentDateTime();
    emit projectUpdated(trackProject(projectId));
    return true;
}

bool ProjectManagementService::completeMilestone(int projectId, int milestoneId)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;

    for (auto &m : it->milestones) {
        if (m.id == milestoneId) {
            m.completed = true;
            m.status = MilestoneStatus::Completed;
            it->updatedAt = QDateTime::currentDateTime();
            emit projectUpdated(trackProject(projectId));
            return true;
        }
    }
    return false;
}

bool ProjectManagementService::deliverItem(int projectId, int deliverableId)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;

    for (auto &d : it->deliverables) {
        if (d.id == deliverableId) {
            d.delivered = true;
            it->updatedAt = QDateTime::currentDateTime();
            emit projectUpdated(trackProject(projectId));
            return true;
        }
    }
    return false;
}

Project ProjectManagementService::project(int projectId) const
{
    auto it = projects_.find(projectId);
    if (it != projects_.end())
        return *it;
    return {};
}

QVector<Project> ProjectManagementService::allProjects() const
{
    QVector<Project> result;
    result.reserve(projects_.size());
    for (const auto &p : projects_)
        result.append(p);
    return result;
}

int ProjectManagementService::projectCount() const
{
    return projects_.size();
}

QVector<CollaborationEntry> ProjectManagementService::projectHistory(int projectId) const
{
    auto it = history_.find(projectId);
    if (it != history_.end())
        return *it;
    return {};
}
