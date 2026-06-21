#pragma once

// WorkflowEdgeService — edge computing, processing, analytics,
// and storage for workflow data.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>
#include <QByteArray>

struct WfEdgeData {
  QByteArray data;
  qint64 timestamp = 0;
  QString source;
  int priority = 0;
  int size = 0;
  QString format;
  bool compressed = false;
};

struct WfEdgeResult {
  bool success = false;
  QByteArray output;
  double processingTime = 0.0;
  QString error;
};

struct WfEdgeAnalysis {
  bool success = false;
  double mean = 0.0;
  double variance = 0.0;
  double min = 0.0;
  double max = 0.0;
  int sampleCount = 0;
  QString pattern;
};

class WorkflowEdgeService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowEdgeService(QObject *parent = nullptr);

  WfEdgeResult processAtEdge(const WfEdgeData &data);
  WfEdgeAnalysis analyzeAtEdge(const WfEdgeData &data);
  bool storeAtEdge(const WfEdgeData &data);
  bool syncFromEdge();

  int storedCount() const { return storedCount_; }

signals:
  void edgeProcessed(const WfEdgeResult &result);
  void edgeAnalyzed(const WfEdgeAnalysis &analysis);

private:
  int storedCount_ = 0;
};
