#pragma once

// TopologyService — manages bus scanning, slave info, and topology baseline.

#include <QObject>
#include <QVector>
#include "EthercatTypes.h"

class EcatClient;

class TopologyService : public QObject {
  Q_OBJECT
public:
  explicit TopologyService(EcatClient *client, QObject *parent = nullptr);

  void scan();
  void rescan();
  QVector<SlaveInfo> currentSlaves() const;
  QVector<SlaveInfo> baselineSlaves() const;
  void captureBaseline();
  void clearBaseline();
  bool baselineCaptured() const;

signals:
  void scanComplete(const QVector<SlaveInfo> &slaves);
  void baselineChanged();

private:
  EcatClient *client_;
  QVector<SlaveInfo> slaves_;
  QVector<SlaveInfo> baseline_;
  bool baselineCaptured_ = false;
};
