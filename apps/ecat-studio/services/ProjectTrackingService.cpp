#include "ProjectTrackingService.h"

// ProjectTrackingService.cpp — Tracks project progress, time, and task status
//
// Implementation notes:
//   - Progress calculated from completed vs total task counts
//   - Status thresholds: >=80% OnTrack, >=50% AtRisk, else Behind
//   - Time tracking accumulates hours per project with budget comparison

ProjectTrackingService::ProjectTrackingService(QObject *parent)
    : QObject(parent)
{
}

bool ProjectTrackingService::addProject(const ProjectTrackData &data)
{
    if (projects_.contains(data.projectId))
        return false;

    projects_.insert(data.projectId, data);
    return true;
}

ProgressStatus ProjectTrackingService::trackProgress(int projectId) const
{
    ProgressStatus status;
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return status;

    const ProjectTrackData &d = *it;
    status.projectId = d.projectId;
    status.completedTasks = d.completedTasks;
    status.totalTasks = d.totalTasks;
    status.lastUpdated = QDateTime::currentDateTime();

    if (d.totalTasks > 0) {
        status.percentage = (static_cast<double>(d.completedTasks) * 100.0) / d.totalTasks;
        status.progress = static_cast<int>(status.percentage);
    }

    if (d.completedTasks >= d.totalTasks && d.totalTasks > 0)
        status.status = ProjectTrackStatus::Completed;
    else if (status.percentage >= 80.0)
        status.status = ProjectTrackStatus::OnTrack;
    else if (status.percentage >= 50.0)
        status.status = ProjectTrackStatus::AtRisk;
    else
        status.status = ProjectTrackStatus::Behind;

    return status;
}

TimeReport ProjectTrackingService::trackTime(int projectId) const
{
    TimeReport report;
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return report;

    const ProjectTrackData &d = *it;
    report.projectId = d.projectId;
    report.budgetedHours = d.budgetedHours;
    report.entries = d.timeEntries;
    report.generatedAt = QDateTime::currentDateTime();

    for (const auto &entry : d.timeEntries)
        report.totalHours += entry.hours;

    report.remainingHours = d.budgetedHours - report.totalHours;
    if (report.remainingHours < 0)
        report.remainingHours = 0;

    return report;
}

CostReport ProjectTrackingService::trackCost(int projectId) const
{
    CostReport report;
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return report;

    const ProjectTrackData &d = *it;
    report.projectId = d.projectId;
    report.budgetedCost = d.budgetedCost;
    report.entries = d.costEntries;
    report.generatedAt = QDateTime::currentDateTime();

    for (const auto &entry : d.costEntries)
        report.totalCost += entry.amount;

    report.remainingBudget = d.budgetedCost - report.totalCost;
    if (report.remainingBudget < 0)
        report.remainingBudget = 0;

    return report;
}

QualityReport ProjectTrackingService::trackQuality(int projectId) const
{
    QualityReport report;
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return report;

    const ProjectTrackData &d = *it;
    report.projectId = d.projectId;
    report.metrics = d.qualityMetrics;
    report.generatedAt = QDateTime::currentDateTime();

    double totalScore = 0.0;
    int count = 0;
    for (const auto &m : d.qualityMetrics) {
        if (m.target > 0) {
            totalScore += (m.value / m.target) * 100.0;
            count++;
        }
    }

    if (count > 0)
        report.overallScore = totalScore / count;

    return report;
}

bool ProjectTrackingService::logTime(int projectId, const TimeEntry &entry)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;

    it->timeEntries.append(entry);
    return true;
}

bool ProjectTrackingService::logCost(int projectId, const CostEntry &entry)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;

    it->costEntries.append(entry);
    return true;
}

bool ProjectTrackingService::updateTaskCompletion(int projectId, int completedTasks)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;

    it->completedTasks = completedTasks;
    emit progressUpdated(trackProgress(projectId));
    return true;
}

bool ProjectTrackingService::updateQualityMetric(int projectId, const QualityMetric &metric)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return false;

    for (auto &m : it->qualityMetrics) {
        if (m.name == metric.name) {
            m = metric;
            m.meetsTarget = (m.value >= m.target);
            return true;
        }
    }

    QualityMetric newMetric = metric;
    newMetric.meetsTarget = (newMetric.value >= newMetric.target);
    it->qualityMetrics.append(newMetric);
    return true;
}

int ProjectTrackingService::projectCount() const
{
    return projects_.size();
}
