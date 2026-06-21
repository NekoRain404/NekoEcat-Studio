#pragma once

// ErrorCorrelationWidget — displays correlation matrix, pattern detection,
// root cause analysis results, and recommendations.

#include <QWidget>
#include <QVector>
#include <QString>

struct CorrelationDisplayEntry {
  QString typeA;
  QString typeB;
  double value = 0.0;
  QString relationship;
};

struct RootCauseDisplay {
  QString errorType;
  QString rootCause;
  double confidence = 0.0;
  QStringList factors;
  QStringList actions;
};

class QLabel;
class QTableWidget;
class QTextEdit;

class ErrorCorrelationWidget : public QWidget {
  Q_OBJECT
public:
  explicit ErrorCorrelationWidget(QWidget *parent = nullptr);

  void setCorrelationData(const QVector<CorrelationDisplayEntry> &entries);
  void setRootCause(const RootCauseDisplay &rca);
  void setRecommendations(const QStringList &recommendations);
  void clear();

  QTableWidget *correlationTable() const;
  QTextEdit *rootCauseText() const;
  QTextEdit *recommendationsText() const;

private:
  void buildUi();

  QTableWidget *correlationTable_ = nullptr;
  QTextEdit *rootCauseText_ = nullptr;
  QTextEdit *recommendationsText_ = nullptr;
  QLabel *summaryLabel_ = nullptr;
};
