#pragma once

/// @brief Workspace plugin for advanced error analysis and diagnostics.
///
/// @details The Advanced Error Analysis workspace provides deep error analysis
/// capabilities including error timeline visualization, error classification,
/// error correlation analysis, and error prediction.
///
/// Features:
///   - **Error timeline**: Visual timeline of errors with severity-coded markers.
///   - **Error classification**: Errors classified by type and severity.
///   - **Error correlation**: Correlation analysis between error types.
///   - **Error prediction**: Predictive error analysis based on patterns.
///   - **Root cause analysis**: Automated root cause identification.
///   - **Export**: Export error analysis reports.
///
/// @par Constructor
///   AdvancedErrorAnalysisPlugin(ServiceContainer *container, QObject *parent = nullptr)
///
/// @par Plugin Identity
///   - id: "erroranalysis"
///   - defaultOrder: 34
///   - visible: always true
///
/// @see WorkspacePlugin, AdvancedErrorAnalysisService, ErrorTimelineWidget,
///      ErrorCorrelationWidget

#include "plugins/WorkspacePlugin.h"

class AdvancedErrorAnalysisService;
class ErrorTimelineWidget;
class ErrorCorrelationWidget;
class QTableWidget;
class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;

class AdvancedErrorAnalysisPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit AdvancedErrorAnalysisPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTableWidget *errorTable() const;
  ErrorTimelineWidget *timelineWidget() const;
  ErrorCorrelationWidget *correlationWidget() const;
  QLabel *summaryLabel() const;

  void runAnalysis();
  void exportReport(QWidget *parentWidget);
  bool exportReportToFile(const QString &path);

private:
  void buildUi();
  void populateTestData();

  AdvancedErrorAnalysisService *analysisService_ = nullptr;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *errorTable_ = nullptr;
  ErrorTimelineWidget *timelineWidget_ = nullptr;
  ErrorCorrelationWidget *correlationWidget_ = nullptr;
  QLineEdit *filter_ = nullptr;
  QComboBox *severityFilter_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
  QPushButton *analyzeBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *clearBtn_ = nullptr;
};
