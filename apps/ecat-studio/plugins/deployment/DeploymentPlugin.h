#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QTreeWidget;

struct DeploymentTarget {
  QString id;
  QString name;
  QString address;
  QString type;
  QString status;
};

struct DeploymentPackage {
  QString id;
  QString name;
  QString version;
  QString description;
  qint64 sizeBytes;
  QString createdAt;
};

struct DeploymentRecord {
  QString id;
  QString targetName;
  QString packageName;
  QString version;
  QString status;
  QString timestamp;
  QString log;
};

class DeploymentPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DeploymentPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTreeWidget *deploymentTargets() const;
  QTableWidget *deploymentPackages() const;
  QTableWidget *deploymentHistory() const;
  QTextEdit *deploymentStatus() const;

  void addTarget(const QString &name, const QString &address, const QString &type);
  void removeTarget(const QString &targetId);
  void clearTargets();
  void updateTargetStatus(const QString &targetId, const QString &status);
  int targetCount() const;

  void addPackage(const QString &name, const QString &version, const QString &description = QString());
  void removePackage(const QString &packageId);
  void clearPackages();
  int packageCount() const;

  void deploy(const QString &targetId, const QString &packageId);
  void rollback(const QString &deploymentId);

  void addDeploymentRecord(const DeploymentRecord &record);
  int deploymentHistoryCount() const;

  bool exportDeploymentLog(const QString &filePath);
  void clearHistory();

signals:
  void targetAdded(const QString &targetId, const QString &name);
  void targetRemoved(const QString &targetId);
  void targetStatusChanged(const QString &targetId, const QString &status);
  void packageAdded(const QString &packageId, const QString &name);
  void packageRemoved(const QString &packageId);
  void deploymentStarted(const QString &targetId, const QString &packageId);
  void deploymentFinished(const QString &deploymentId, const QString &status);
  void rollbackRequested(const QString &deploymentId);

private:
  void buildUi();
  void updateStatusText();

  QWidget *containerWidget_ = nullptr;
  QTreeWidget *deploymentTargets_ = nullptr;
  QTableWidget *deploymentPackages_ = nullptr;
  QTableWidget *deploymentHistory_ = nullptr;
  QTextEdit *deploymentStatus_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QPushButton *deployButton_ = nullptr;
  QPushButton *rollbackButton_ = nullptr;
  QPushButton *refreshButton_ = nullptr;
  QPushButton *exportButton_ = nullptr;
  QPushButton *clearButton_ = nullptr;
  QVector<DeploymentTarget> targets_;
  QVector<DeploymentPackage> packages_;
  QVector<DeploymentRecord> records_;
  int nextTargetId_ = 1;
  int nextPackageId_ = 1;
  int nextRecordId_ = 1;
};
