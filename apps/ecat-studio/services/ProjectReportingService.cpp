#include "ProjectReportingService.h"

// ProjectReportingService.cpp — Generates project status, progress, and quality reports
//
// Implementation notes:
//   - Builds sectioned text reports with completion percentages
//   - Tracks quality scores and resource utilization metrics
//   - Supports multi-format report generation via ProjectDocReport

ProjectReportingService::ProjectReportingService(QObject *parent)
    : QObject(parent)
{
}

bool ProjectReportingService::addProject(const ReportProjectData &data)
{
    if (projects_.contains(data.projectId))
        return false;

    projects_.insert(data.projectId, data);
    return true;
}

ProjectDocReport ProjectReportingService::buildBaseReport(const ReportProjectData &d, const QString &type)
{
    ProjectDocReport r;
    r.title = QStringLiteral("%1 Report - %2").arg(type, d.projectName);
    r.timestamp = QDateTime::currentDateTime();
    r.format = ProjectReportFormat::Text;
    return r;
}

ProjectDocReport ProjectReportingService::generateStatusReport(int projectId)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return {};

    const ReportProjectData &d = *it;
    ProjectDocReport r = buildBaseReport(d, "Status");

    double completion = 0.0;
    if (d.totalTasks > 0)
        completion = (static_cast<double>(d.completedTasks) * 100.0) / d.totalTasks;

    r.summary = QStringLiteral("Project '%1' is %2%% complete with %3 of %4 tasks done.")
        .arg(d.projectName).arg(completion, 0, 'f', 1)
        .arg(d.completedTasks).arg(d.totalTasks);

    ProjectReportSection overview;
    overview.title = "Overview";
    overview.content = QStringLiteral("Project ID: %1\nCompletion: %2%%\nQuality Score: %3")
        .arg(d.projectId).arg(completion, 0, 'f', 1).arg(d.qualityScore, 0, 'f', 1);
    r.sections.append(overview);

    if (!d.statusNotes.isEmpty()) {
        ProjectReportSection notes;
        notes.title = "Status Notes";
        notes.items = d.statusNotes;
        r.sections.append(notes);
    }

    if (d.totalCost > d.budgetedCost * 0.9)
        r.recommendations.append("Budget is approaching limits — review remaining expenditures.");
    if (completion < 50.0 && d.totalTasks > 0)
        r.recommendations.append("Project is less than 50% complete — consider reviewing timeline.");

    if (reports_.size() < kMaxReports)
        reports_.append(r);
    emit reportGenerated(r);
    return r;
}

ProjectDocReport ProjectReportingService::generateProgressReport(int projectId)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return {};

    const ReportProjectData &d = *it;
    ProjectDocReport r = buildBaseReport(d, "Progress");

    double completion = 0.0;
    if (d.totalTasks > 0)
        completion = (static_cast<double>(d.completedTasks) * 100.0) / d.totalTasks;

    double hourUsage = 0.0;
    if (d.budgetedHours > 0)
        hourUsage = (d.totalHours * 100.0) / d.budgetedHours;

    r.summary = QStringLiteral("Progress: %1%% tasks complete, %2%% hours used.")
        .arg(completion, 0, 'f', 1).arg(hourUsage, 0, 'f', 1);

    ProjectReportSection taskSection;
    taskSection.title = "Task Progress";
    taskSection.content = QStringLiteral("Completed: %1 / %2\nPercentage: %3%%")
        .arg(d.completedTasks).arg(d.totalTasks).arg(completion, 0, 'f', 1);
    r.sections.append(taskSection);

    ProjectReportSection hourSection;
    hourSection.title = "Time Progress";
    hourSection.content = QStringLiteral("Hours Used: %1 / %2\nPercentage: %3%%")
        .arg(d.totalHours, 0, 'f', 1).arg(d.budgetedHours, 0, 'f', 1).arg(hourUsage, 0, 'f', 1);
    r.sections.append(hourSection);

    ProjectReportChart chart;
    chart.title = "Completion Breakdown";
    chart.type = "pie";
    chart.data["completed"] = d.completedTasks;
    chart.data["remaining"] = d.totalTasks - d.completedTasks;
    r.charts.append(chart);

    if (hourUsage > completion + 20.0)
        r.recommendations.append("Hours used exceed task completion — review time allocation.");

    if (reports_.size() < kMaxReports)
        reports_.append(r);
    emit reportGenerated(r);
    return r;
}

ProjectDocReport ProjectReportingService::generatePerformanceReport(int projectId)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return {};

    const ReportProjectData &d = *it;
    ProjectDocReport r = buildBaseReport(d, "Performance");

    double efficiency = 0.0;
    if (d.totalHours > 0 && d.totalTasks > 0) {
        double tasksPerHour = static_cast<double>(d.completedTasks) / d.totalHours;
        double expectedRate = static_cast<double>(d.totalTasks) / d.budgetedHours;
        if (expectedRate > 0)
            efficiency = (tasksPerHour / expectedRate) * 100.0;
    }

    r.summary = QStringLiteral("Performance efficiency: %1%%. Quality score: %2.")
        .arg(efficiency, 0, 'f', 1).arg(d.qualityScore, 0, 'f', 1);

    ProjectReportSection perfSection;
    perfSection.title = "Performance Metrics";
    perfSection.content = QStringLiteral("Efficiency: %1%%\nQuality Score: %2\nTasks/Hour: %3")
        .arg(efficiency, 0, 'f', 1).arg(d.qualityScore, 0, 'f', 1)
        .arg(d.totalHours > 0 ? static_cast<double>(d.completedTasks) / d.totalHours : 0.0, 0, 'f', 2);
    r.sections.append(perfSection);

    ProjectReportChart chart;
    chart.title = "Performance Indicators";
    chart.type = "bar";
    chart.data["efficiency"] = efficiency;
    chart.data["quality"] = d.qualityScore;
    r.charts.append(chart);

    if (efficiency < 80.0)
        r.recommendations.append("Performance efficiency is below 80% — review workflow and resource allocation.");
    if (d.qualityScore < 90.0)
        r.recommendations.append("Quality score is below target — review quality assurance processes.");

    if (reports_.size() < kMaxReports)
        reports_.append(r);
    emit reportGenerated(r);
    return r;
}

ProjectDocReport ProjectReportingService::generateFinancialReport(int projectId)
{
    auto it = projects_.find(projectId);
    if (it == projects_.end())
        return {};

    const ReportProjectData &d = *it;
    ProjectDocReport r = buildBaseReport(d, "Financial");

    double budgetUsage = 0.0;
    if (d.budgetedCost > 0)
        budgetUsage = (d.totalCost * 100.0) / d.budgetedCost;

    double remaining = d.budgetedCost - d.totalCost;

    r.summary = QStringLiteral("Budget usage: %1%%. Total spent: $%2 of $%3 budget.")
        .arg(budgetUsage, 0, 'f', 1).arg(d.totalCost, 0, 'f', 2).arg(d.budgetedCost, 0, 'f', 2);

    ProjectReportSection finSection;
    finSection.title = "Financial Summary";
    finSection.content = QStringLiteral("Total Cost: $%1\nBudget: $%2\nRemaining: $%3\nUsage: %4%%")
        .arg(d.totalCost, 0, 'f', 2).arg(d.budgetedCost, 0, 'f', 2)
        .arg(remaining, 0, 'f', 2).arg(budgetUsage, 0, 'f', 1);
    r.sections.append(finSection);

    ProjectReportTable table;
    table.title = "Cost Breakdown";
    table.headers = {"Category", "Amount"};
    QStringList row1 = {"Spent", QStringLiteral("$%1").arg(d.totalCost, 0, 'f', 2)};
    QStringList row2 = {"Remaining", QStringLiteral("$%1").arg(remaining, 0, 'f', 2)};
    QStringList row3 = {"Budget", QStringLiteral("$%1").arg(d.budgetedCost, 0, 'f', 2)};
    table.rows = {row1, row2, row3};
    r.tables.append(table);

    if (budgetUsage > 90.0)
        r.recommendations.append("Budget usage exceeds 90% — implement cost controls immediately.");
    if (remaining < 0)
        r.recommendations.append("Project is over budget — review and reallocate resources.");

    if (reports_.size() < kMaxReports)
        reports_.append(r);
    emit reportGenerated(r);
    return r;
}

QVector<ProjectDocReport> ProjectReportingService::allReports() const
{
    return reports_;
}

int ProjectReportingService::reportCount() const
{
    return reports_.size();
}
