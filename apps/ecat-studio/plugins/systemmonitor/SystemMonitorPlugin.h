#pragma once

// SystemMonitorPlugin — system resource monitoring workspace.

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class SystemMonitorPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit SystemMonitorPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct UsageSample {
    QDateTime timestamp;
    double value;
  };

  struct AlertRule {
    QString metric;
    double threshold;
    QString condition;
    QString message;
    bool triggered;
  };

  void updateCpuUsage(double percent);
  void updateMemoryUsage(double percent);
  void updateDiskUsage(double percent);
  void updateNetworkUsage(double mbps);

  double cpuUsage() const;
  double memoryUsage() const;
  double diskUsage() const;
  double networkUsage() const;

  QVector<UsageSample> cpuHistory() const;
  QVector<UsageSample> memoryHistory() const;
  QVector<UsageSample> diskHistory() const;
  QVector<UsageSample> networkHistory() const;

  void addAlert(const AlertRule &rule);
  void removeAlert(int index);
  int alertCount() const;
  int triggeredAlertCount() const;
  void checkAlerts();

  QTableWidget *overviewTable() const;
  QTableWidget *alertTable() const;
  QTextEdit *historyView() const;
  QLabel *statusLabel() const;

signals:
  void usageUpdated(const QString &metric, double value);
  void alertTriggered(const QString &metric, double value, double threshold);

public slots:
  void refresh();

private:
  void buildUi();
  void rebuildOverviewTable();
  void rebuildAlertTable();
  void rebuildHistoryView();
  void recordSample(QVector<UsageSample> &history, double value);

  QWidget *containerWidget_ = nullptr;
  QTableWidget *overviewTable_ = nullptr;
  QTableWidget *alertTable_ = nullptr;
  QTextEdit *historyView_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;
  QPushButton *addAlertBtn_ = nullptr;
  QPushButton *removeAlertBtn_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  double cpuUsage_ = 0.0;
  double memoryUsage_ = 0.0;
  double diskUsage_ = 0.0;
  double networkUsage_ = 0.0;

  QVector<UsageSample> cpuHistory_;
  QVector<UsageSample> memoryHistory_;
  QVector<UsageSample> diskHistory_;
  QVector<UsageSample> networkHistory_;
  QVector<AlertRule> alerts_;
  static constexpr int kMaxHistorySize = 100;
};
