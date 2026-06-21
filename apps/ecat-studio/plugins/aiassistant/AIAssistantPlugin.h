#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QTreeWidget;

class AIAssistantPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit AIAssistantPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTableWidget *predictionsTable() const;
  QTreeWidget *anomaliesTree() const;
  QTextEdit *optimizationsView() const;
  QTableWidget *patternsTable() const;

  void addPrediction(const QString &metric, double confidence, const QString &value);
  void removePrediction(const QString &metric);
  void clearPredictions();
  int predictionCount() const;

  void addAnomaly(const QString &source, const QString &severity, const QString &description);
  void clearAnomalies();
  int anomalyCount() const;

  void setOptimizationsText(const QString &text);
  QString optimizationsText() const;

  void addPattern(const QString &name, const QString &frequency, const QString &confidence);
  void clearPatterns();
  int patternCount() const;

  bool exportAIReport(const QString &filePath, const QString &format);

signals:
  void predictionAdded(const QString &metric);
  void anomalyDetected(const QString &source);
  void optimizationsUpdated();
  void patternRecognized(const QString &name);
  void exportRequested();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QTableWidget *predictionsTable_ = nullptr;
  QTreeWidget *anomaliesTree_ = nullptr;
  QTextEdit *optimizationsView_ = nullptr;
  QTableWidget *patternsTable_ = nullptr;
  QPushButton *analyzeBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
