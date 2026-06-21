#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QTreeWidget;

class EdgeComputingPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit EdgeComputingPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTableWidget *deviceTable() const;
  QTreeWidget *processingJobs() const;
  QTextEdit *analyticsView() const;
  QTableWidget *storageTable() const;

  void addDevice(const QString &name, const QString &type, const QString &status);
  void removeDevice(const QString &name);
  void clearDevices();
  int deviceCount() const;

  void addProcessingJob(const QString &jobId, const QString &description);
  void removeProcessingJob(const QString &jobId);
  void clearProcessingJobs();
  int processingJobCount() const;

  void setAnalyticsText(const QString &text);
  QString analyticsText() const;

  void addStorageEntry(const QString &name, const QString &size, const QString &usage);
  void clearStorageEntries();
  int storageEntryCount() const;

  bool exportEdgeReport(const QString &filePath, const QString &format);

signals:
  void deviceAdded(const QString &name);
  void deviceRemoved(const QString &name);
  void processingJobAdded(const QString &jobId);
  void analyticsUpdated();
  void exportRequested();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QTableWidget *deviceTable_ = nullptr;
  QTreeWidget *processingJobs_ = nullptr;
  QTextEdit *analyticsView_ = nullptr;
  QTableWidget *storageTable_ = nullptr;
  QPushButton *addDeviceBtn_ = nullptr;
  QPushButton *removeDeviceBtn_ = nullptr;
  QPushButton *addJobBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
