#pragma once

// EtherCATEdgeService -- local in-memory processing, analytics, and storage
// helpers for EtherCAT data. No edge-node/backend sync path is wired yet, so
// syncFromEdge() fails closed instead of reporting synthetic sync success.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>
#include <QByteArray>

struct EdgeData {
  QByteArray data;
  qint64 timestamp = 0;
  QString source;
  int priority = 0;
  int size = 0;
  QString format;
  bool compressed = false;
};

struct EdgeResult {
  bool success = false;
  QByteArray output;
  double processingTime = 0.0;
  QString error;
};

struct EdgeAnalysis {
  bool success = false;
  double mean = 0.0;
  double variance = 0.0;
  double min = 0.0;
  double max = 0.0;
  int sampleCount = 0;
  QString pattern;
};

class EtherCATEdgeService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATEdgeService(QObject *parent = nullptr);

  EdgeResult processAtEdge(const EdgeData &data);
  EdgeAnalysis analyzeAtEdge(const EdgeData &data);
  bool storeAtEdge(const EdgeData &data);
  bool syncFromEdge();

  int storedCount() const { return storedCount_; }

signals:
  void edgeProcessed(const EdgeResult &result);
  void edgeAnalyzed(const EdgeAnalysis &analysis);

private:
  int storedCount_ = 0;
};
