#pragma once

// WorkflowOptimizerPlugin — workspace plugin for workflow optimization,
// suggestions, performance metrics, and execution history.
//
// Default order: 385

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>

class WorkflowAnalyticsService;
class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;
class QTableWidget;
class QTextEdit;

class WorkflowOptimizerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit WorkflowOptimizerPlugin(WorkflowAnalyticsService* analytics, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    void addWorkflow(const QString& workflowId, const QString& name);
    void removeWorkflow(const QString& workflowId);
    int workflowCount() const;

    void addSuggestion(const QString& workflowId, const QString& priority, const QString& suggestion);
    int suggestionCount() const;

    void addExecutionRecord(const QString& workflowId, const QString& status, double durationMs);
    int executionHistoryCount() const;

    bool exportReport(const QString& filePath);

signals:
    void workflowSelected(const QString& workflowId);
    void optimizationRequested(const QString& workflowId);
    void reportExported(const QString& filePath);

private:
    void buildUi();
    void refreshWorkflowList();
    void refreshSuggestions();
    void refreshMetrics();

    WorkflowAnalyticsService* analytics_;
    QWidget* containerWidget_ = nullptr;
    QComboBox* workflowSelector_ = nullptr;
    QListWidget* workflowList_ = nullptr;
    QTableWidget* suggestionsTable_ = nullptr;
    QTableWidget* metricsTable_ = nullptr;
    QTableWidget* historyTable_ = nullptr;
    QTextEdit* reportPreview_ = nullptr;
    QPushButton* optimizeButton_ = nullptr;
    QPushButton* exportButton_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    struct WorkflowEntry {
        QString id;
        QString name;
        QString status;
    };
    QVector<WorkflowEntry> workflows_;

    struct SuggestionEntry {
        QString workflowId;
        QString priority;
        QString text;
    };
    QVector<SuggestionEntry> suggestions_;

    struct HistoryEntry {
        QString workflowId;
        QString status;
        double durationMs = 0.0;
        QDateTime timestamp;
    };
    QVector<HistoryEntry> history_;
};
