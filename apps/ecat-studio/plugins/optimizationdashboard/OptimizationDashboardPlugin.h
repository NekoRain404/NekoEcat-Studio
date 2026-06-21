#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QPushButton;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class OptimizationDashboardPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit OptimizationDashboardPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct OptimizationMetric {
    QString name;
    double value;
    double target;
    double improvement;
  };

  struct OptimizationHistoryEntry {
    QDateTime timestamp;
    QString action;
    QString result;
    double improvement;
  };

  struct OptimizationRecommendation {
    QString title;
    QString description;
    QString priority;
    QString category;
  };

  struct OptimizationAction {
    QString name;
    QString description;
    bool executed;
    QString result;
  };

  void addMetric(const OptimizationMetric &metric);
  void updateMetric(int index, double value);
  int metricCount() const;
  QVector<OptimizationMetric> metrics() const;

  void addHistoryEntry(const OptimizationHistoryEntry &entry);
  int historyCount() const;
  QVector<OptimizationHistoryEntry> history() const;

  void addRecommendation(const OptimizationRecommendation &rec);
  void removeRecommendation(int index);
  int recommendationCount() const;
  QVector<OptimizationRecommendation> recommendations() const;

  void addAction(const OptimizationAction &action);
  void executeAction(int index);
  int actionCount() const;
  QVector<OptimizationAction> actions() const;

  QTableWidget *metricsTable() const;
  QTableWidget *historyTable() const;
  QTableWidget *recommendationsTable() const;
  QTableWidget *actionsTable() const;
  QTextEdit *reportView() const;
  QLabel *statusLabel() const;

  QString exportReport() const;

signals:
  void metricUpdated(const QString &name, double value);
  void actionExecuted(const QString &name, const QString &result);
  void recommendationAdded(const QString &title);

public slots:
  void refresh();

private:
  void buildUi();
  void rebuildMetricsTable();
  void rebuildHistoryTable();
  void rebuildRecommendationsTable();
  void rebuildActionsTable();
  void rebuildReportView();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QTableWidget *metricsTable_ = nullptr;
  QTableWidget *historyTable_ = nullptr;
  QTableWidget *recommendationsTable_ = nullptr;
  QTableWidget *actionsTable_ = nullptr;
  QTextEdit *reportView_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<OptimizationMetric> metrics_;
  QVector<OptimizationHistoryEntry> history_;
  QVector<OptimizationRecommendation> recommendations_;
  QVector<OptimizationAction> actions_;
};
