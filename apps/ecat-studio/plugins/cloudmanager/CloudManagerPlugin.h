#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QListWidget;
class QProgressBar;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QTreeWidget;

class CloudManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit CloudManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTableWidget *connectionTable() const;
  QProgressBar *syncProgress() const;
  QTreeWidget *backupHistory() const;
  QTextEdit *monitoringView() const;

  void addConnection(const QString &name, const QString &endpoint);
  void removeConnection(const QString &name);
  void clearConnections();
  int connectionCount() const;

  void setSyncProgress(int percent);
  int syncProgressValue() const;

  void addBackupEntry(const QString &timestamp, const QString &status);
  void clearBackupHistory();
  int backupCount() const;

  void setMonitoringText(const QString &text);
  QString monitoringText() const;

  bool exportCloudReport(const QString &filePath, const QString &format);

signals:
  void connectionAdded(const QString &name);
  void connectionRemoved(const QString &name);
  void syncProgressChanged(int percent);
  void backupAdded(const QString &timestamp);
  void exportRequested();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QTableWidget *connectionTable_ = nullptr;
  QProgressBar *syncProgress_ = nullptr;
  QTreeWidget *backupHistory_ = nullptr;
  QTextEdit *monitoringView_ = nullptr;
  QPushButton *addConnectionBtn_ = nullptr;
  QPushButton *removeConnectionBtn_ = nullptr;
  QPushButton *syncBtn_ = nullptr;
  QPushButton *backupBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
