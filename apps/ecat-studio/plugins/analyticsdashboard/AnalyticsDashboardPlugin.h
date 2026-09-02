#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QPushButton;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class AnalyticsDashboardPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit AnalyticsDashboardPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    struct AnalyticsMetric {
        QString name;
        double value;
        double min;
        double max;
        double avg;
        int samples;
    };

    struct AnalyticsTrend {
        QString metric;
        QVector<double> values;
        QVector<QDateTime> timestamps;
        double slope;
        QString direction;
    };

    struct AnalyticsReport {
        QString title;
        QDateTime generated;
        QString summary;
        QVector<AnalyticsMetric> metrics;
    };

    struct AnalyticsFilter {
        QString field;
        QString operator_;
        QString value;
        bool active;
    };

    void addMetric(const AnalyticsMetric& metric);
    void updateMetric(int index, double value);
    int metricCount() const;
    QVector<AnalyticsMetric> metrics() const;

    void addTrend(const AnalyticsTrend& trend);
    int trendCount() const;
    QVector<AnalyticsTrend> trends() const;

    void addReport(const AnalyticsReport& report);
    int reportCount() const;
    QVector<AnalyticsReport> reports() const;

    void addFilter(const AnalyticsFilter& filter);
    void removeFilter(int index);
    void toggleFilter(int index);
    int filterCount() const;
    QVector<AnalyticsFilter> filters() const;

    QTableWidget* metricsTable() const;
    QTableWidget* trendsTable() const;
    QTableWidget* reportsTable() const;
    QTableWidget* filtersTable() const;
    QTextEdit* reportView() const;
    QLabel* statusLabel() const;

    QString exportReport() const;

signals:
    void metricUpdated(const QString& name, double value);
    void trendAdded(const QString& metric);
    void reportGenerated(const QString& title);
    void filterToggled(int index, bool active);

public slots:
    void refresh();

private:
    void buildUi();
    void rebuildMetricsTable();
    void rebuildTrendsTable();
    void rebuildReportsTable();
    void rebuildFiltersTable();
    void rebuildReportView();

    QWidget* containerWidget_ = nullptr;
    QTabWidget* tabs_ = nullptr;
    QTableWidget* metricsTable_ = nullptr;
    QTableWidget* trendsTable_ = nullptr;
    QTableWidget* reportsTable_ = nullptr;
    QTableWidget* filtersTable_ = nullptr;
    QTextEdit* reportView_ = nullptr;
    QPushButton* refreshBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    QVector<AnalyticsMetric> metrics_;
    QVector<AnalyticsTrend> trends_;
    QVector<AnalyticsReport> reports_;
    QVector<AnalyticsFilter> filters_;
};
