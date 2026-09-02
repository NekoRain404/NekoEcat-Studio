#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;

struct DeploymentMgrTarget {
    QString id;
    QString name;
    QString address;
    QString config;
    QString status;
};

struct DeploymentMgrPackage {
    QString id;
    QString name;
    QString version;
    QString description;
    QString createdAt;
};

struct DeploymentMgrRecord {
    QString id;
    QString targetName;
    QString packageName;
    QString version;
    QString status;
    QString timestamp;
    QString log;
};

class DeploymentManagerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit DeploymentManagerPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    QTableWidget* targetTable() const;
    QTableWidget* packageTable() const;
    QTableWidget* historyTable() const;
    QTextEdit* statusLog() const;
    QLabel* statusLabel() const;

    int targetCount() const;
    int packageCount() const;
    int historyCount() const;

    void addTarget(const DeploymentMgrTarget& target);
    void removeTarget(int index);
    void updateTargetStatus(int index, const QString& status);

    void addPackage(const DeploymentMgrPackage& pkg);
    void removePackage(int index);

    void deploy(int targetIndex, int packageIndex);
    void rollback(int historyIndex);
    void clearHistory();

    bool exportLog(const QString& filePath);

signals:
    void targetAdded(const QString& targetId, const QString& name);
    void targetRemoved(const QString& targetId);
    void targetStatusChanged(const QString& targetId, const QString& status);
    void packageAdded(const QString& packageId, const QString& name);
    void packageRemoved(const QString& packageId);
    void deploymentStarted(const QString& targetId, const QString& packageId);
    void deploymentFinished(const QString& recordId, const QString& status);
    void rollbackRequested(const QString& recordId);

private:
    void buildUi();
    void refreshStatus();

    QWidget* container_ = nullptr;
    QTableWidget* targetTable_ = nullptr;
    QTableWidget* packageTable_ = nullptr;
    QTableWidget* historyTable_ = nullptr;
    QTextEdit* statusLog_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QPushButton* deployBtn_ = nullptr;
    QPushButton* rollbackBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QPushButton* clearBtn_ = nullptr;

    QVector<DeploymentMgrTarget> targets_;
    QVector<DeploymentMgrPackage> packages_;
    QVector<DeploymentMgrRecord> records_;
    int nextId_ = 1;
};
