#include "ProjectPlanningService.h"

// ProjectPlanningService.cpp — Manages milestones, timelines, and resource planning
//
// Implementation notes:
//   - Milestones support dependencies and priority-based ordering
//   - Resource plans validated for percentage-based allocation totals
//   - Timeline objects group milestones with date ranges

ProjectPlanningService::ProjectPlanningService(QObject *parent)
    : QObject(parent)
{
}

PlannedMilestone ProjectPlanningService::createMilestone(const MilestoneConfig &config)
{
    if (config.name.trimmed().isEmpty())
        return {};

    PlannedMilestone m;
    m.id = nextMilestoneId_++;
    m.name = config.name;
    m.description = config.description;
    m.deadline = config.deadline;
    m.deliverables = config.deliverables;
    m.criteria = config.criteria;
    m.dependencies = config.dependencies;
    m.priority = config.priority;
    m.completed = false;
    m.createdAt = QDateTime::currentDateTime();

    milestones_.insert(m.id, m);
    emit milestoneCreated(m);
    return m;
}

Timeline ProjectPlanningService::createTimeline(const TimelineConfig &config)
{
    if (config.name.trimmed().isEmpty())
        return {};
    if (config.startDate.isValid() && config.endDate.isValid() && config.endDate < config.startDate)
        return {};
    for (int milestoneId : config.milestoneIds) {
        if (!milestones_.contains(milestoneId))
            return {};
    }

    Timeline t;
    t.id = nextTimelineId_++;
    t.name = config.name;
    t.startDate = config.startDate;
    t.endDate = config.endDate;
    t.milestoneIds = config.milestoneIds;
    t.description = config.description;
    t.createdAt = QDateTime::currentDateTime();

    timelines_.insert(t.id, t);
    emit timelineCreated(t);
    return t;
}

bool ProjectPlanningService::planResources(const ResourcePlan &plan)
{
    if (plan.projectId <= 0)
        return false;
    if (plan.resourceNames.size() != plan.allocationPercent.size())
        return false;

    int total = 0;
    for (int pct : plan.allocationPercent) {
        if (pct <= 0 || pct > 100)
            return false;
        total += pct;
    }

    if (total > 100)
        return false;

    resourcePlans_.insert(plan.projectId, plan);
    return true;
}

RiskReport ProjectPlanningService::assessRisks(const RiskAssessment &assessment)
{
    RiskReport report;
    report.projectId = assessment.projectId;
    report.risks = assessment.risks;
    report.generatedAt = QDateTime::currentDateTime();

    for (const auto &risk : assessment.risks) {
        switch (risk.level) {
        case RiskLevel::High:
        case RiskLevel::Critical:
            report.highRiskCount++;
            break;
        case RiskLevel::Medium:
            report.mediumRiskCount++;
            break;
        case RiskLevel::Low:
            report.lowRiskCount++;
            break;
        }

        double score = risk.probability * risk.impact;
        report.overallRiskScore += score;
    }

    if (!assessment.risks.isEmpty())
        report.overallRiskScore /= assessment.risks.size();

    return report;
}

PlannedMilestone ProjectPlanningService::milestone(int milestoneId) const
{
    auto it = milestones_.find(milestoneId);
    if (it != milestones_.end())
        return *it;
    return {};
}

QVector<PlannedMilestone> ProjectPlanningService::allMilestones() const
{
    QVector<PlannedMilestone> result;
    result.reserve(milestones_.size());
    for (const auto &m : milestones_)
        result.append(m);
    return result;
}

Timeline ProjectPlanningService::timeline(int timelineId) const
{
    auto it = timelines_.find(timelineId);
    if (it != timelines_.end())
        return *it;
    return {};
}

QVector<Timeline> ProjectPlanningService::allTimelines() const
{
    QVector<Timeline> result;
    result.reserve(timelines_.size());
    for (const auto &t : timelines_)
        result.append(t);
    return result;
}

int ProjectPlanningService::milestoneCount() const
{
    return milestones_.size();
}

int ProjectPlanningService::timelineCount() const
{
    return timelines_.size();
}

bool ProjectPlanningService::completeMilestone(int milestoneId)
{
    auto it = milestones_.find(milestoneId);
    if (it == milestones_.end())
        return false;

    it->completed = true;
    return true;
}

bool ProjectPlanningService::updateMilestonePriority(int milestoneId, MilestonePriority priority)
{
    auto it = milestones_.find(milestoneId);
    if (it == milestones_.end())
        return false;

    it->priority = priority;
    return true;
}
