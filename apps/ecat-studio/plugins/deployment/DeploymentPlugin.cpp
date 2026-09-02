#include "DeploymentPlugin.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

DeploymentPlugin::DeploymentPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString DeploymentPlugin::id() const {
    return "deployment";
}
QString DeploymentPlugin::displayName() const {
    return "Deployment";
}
QString DeploymentPlugin::displayNameZh() const {
    return QStringLiteral("部署");
}
QIcon DeploymentPlugin::icon() const {
    return QIcon::fromTheme("system-software-install");
}
int DeploymentPlugin::defaultOrder() const {
    return 260;
}
bool DeploymentPlugin::visible() const {
    return false;
}

void DeploymentPlugin::activate() {}
void DeploymentPlugin::deactivate() {}

QWidget* DeploymentPlugin::widget() {
    return containerWidget_;
}
QTreeWidget* DeploymentPlugin::deploymentTargets() const {
    return deploymentTargets_;
}
QTableWidget* DeploymentPlugin::deploymentPackages() const {
    return deploymentPackages_;
}
QTableWidget* DeploymentPlugin::deploymentHistory() const {
    return deploymentHistory_;
}
QTextEdit* DeploymentPlugin::deploymentStatus() const {
    return deploymentStatus_;
}

void DeploymentPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QHBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter;

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    auto* targetsLabel = new QLabel(tr("Deployment Targets"));
    leftLayout->addWidget(targetsLabel);

    deploymentTargets_ = new QTreeWidget;
    deploymentTargets_->setHeaderLabels({tr("Name"), tr("Status")});

    auto* localCategory = new QTreeWidgetItem(deploymentTargets_, {tr("Local"), tr("")});
    new QTreeWidgetItem(localCategory, {tr("localhost"), tr("Online")});

    auto* remoteCategory = new QTreeWidgetItem(deploymentTargets_, {tr("Remote"), tr("")});
    new QTreeWidgetItem(remoteCategory, {tr("ecat-dev-01"), tr("Online")});
    new QTreeWidgetItem(remoteCategory, {tr("ecat-dev-02"), tr("Offline")});

    deploymentTargets_->expandAll();
    leftLayout->addWidget(deploymentTargets_);

    auto* addTargetRow = new QHBoxLayout;
    auto* targetNameInput = new QLineEdit;
    targetNameInput->setPlaceholderText(tr("Target name..."));
    addTargetRow->addWidget(targetNameInput);
    auto* addTargetButton = new QPushButton(tr("Add"));
    addTargetRow->addWidget(addTargetButton);
    leftLayout->addLayout(addTargetRow);

    splitter->addWidget(leftPanel);

    auto* centerPanel = new QWidget;
    auto* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(4, 4, 4, 4);

    auto* packagesLabel = new QLabel(tr("Deployment Packages"));
    centerLayout->addWidget(packagesLabel);

    deploymentPackages_ = new QTableWidget(0, 4);
    deploymentPackages_->setHorizontalHeaderLabels({tr("Name"), tr("Version"), tr("Size"), tr("Created")});
    deploymentPackages_->horizontalHeader()->setStretchLastSection(true);
    centerLayout->addWidget(deploymentPackages_);

    auto* historyLabel = new QLabel(tr("Deployment History"));
    centerLayout->addWidget(historyLabel);

    deploymentHistory_ = new QTableWidget(0, 5);
    deploymentHistory_->setHorizontalHeaderLabels(
        {tr("Target"), tr("Package"), tr("Version"), tr("Status"), tr("Time")});
    deploymentHistory_->horizontalHeader()->setStretchLastSection(true);
    centerLayout->addWidget(deploymentHistory_);

    splitter->addWidget(centerPanel);

    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(4, 4, 4, 4);

    auto* statusTitle = new QLabel(tr("Deployment Status"));
    rightLayout->addWidget(statusTitle);

    deploymentStatus_ = new QTextEdit;
    deploymentStatus_->setPlaceholderText(tr("Select a deployment to view details..."));
    deploymentStatus_->setReadOnly(true);
    rightLayout->addWidget(deploymentStatus_);

    statusLabel_ = new QLabel(tr("Status: Ready"));
    rightLayout->addWidget(statusLabel_);

    auto* buttonRow = new QHBoxLayout;
    deployButton_ = new QPushButton(tr("Deploy"));
    buttonRow->addWidget(deployButton_);
    rollbackButton_ = new QPushButton(tr("Rollback"));
    buttonRow->addWidget(rollbackButton_);
    refreshButton_ = new QPushButton(tr("Refresh"));
    buttonRow->addWidget(refreshButton_);
    rightLayout->addLayout(buttonRow);

    auto* exportRow = new QHBoxLayout;
    exportButton_ = new QPushButton(tr("Export Log"));
    exportRow->addWidget(exportButton_);
    clearButton_ = new QPushButton(tr("Clear History"));
    exportRow->addWidget(clearButton_);
    rightLayout->addLayout(exportRow);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    splitter->setStretchFactor(2, 1);

    mainLayout->addWidget(splitter);

    connect(deployButton_, &QPushButton::clicked, this, [this]() {
        if (!targets_.isEmpty() && !packages_.isEmpty()) {
            deploy(targets_.first().id, packages_.first().id);
        }
    });

    connect(rollbackButton_, &QPushButton::clicked, this, [this]() {
        if (!records_.isEmpty()) {
            rollback(records_.last().id);
        }
    });

    connect(refreshButton_, &QPushButton::clicked, this, &DeploymentPlugin::updateStatusText);
    connect(exportButton_, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Log"), QString(), "JSON (*.json)");
        if (!path.isEmpty())
            exportDeploymentLog(path);
    });
    connect(clearButton_, &QPushButton::clicked, this, &DeploymentPlugin::clearHistory);
}

void DeploymentPlugin::addTarget(const QString& name, const QString& address, const QString& type) {
    DeploymentTarget target;
    target.id = QString("target_%1").arg(nextTargetId_++);
    target.name = name;
    target.address = address;
    target.type = type;
    target.status = "Online";
    targets_.append(target);

    auto* item = new QTreeWidgetItem(deploymentTargets_->topLevelItem(0), {name, tr("Online")});
    Q_UNUSED(item);

    emit targetAdded(target.id, name);
}

void DeploymentPlugin::removeTarget(const QString& targetId) {
    for (int i = 0; i < targets_.size(); ++i) {
        if (targets_[i].id == targetId) {
            targets_.removeAt(i);
            emit targetRemoved(targetId);
            return;
        }
    }
}

void DeploymentPlugin::clearTargets() {
    targets_.clear();
    nextTargetId_ = 1;
}

void DeploymentPlugin::updateTargetStatus(const QString& targetId, const QString& status) {
    for (int i = 0; i < targets_.size(); ++i) {
        if (targets_[i].id == targetId) {
            targets_[i].status = status;
            emit targetStatusChanged(targetId, status);
            return;
        }
    }
}

int DeploymentPlugin::targetCount() const {
    return targets_.size();
}

void DeploymentPlugin::addPackage(const QString& name, const QString& version, const QString& description) {
    DeploymentPackage pkg;
    pkg.id = QString("pkg_%1").arg(nextPackageId_++);
    pkg.name = name;
    pkg.version = version;
    pkg.description = description;
    pkg.sizeBytes = 0;
    pkg.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    packages_.append(pkg);

    int row = deploymentPackages_->rowCount();
    deploymentPackages_->insertRow(row);
    deploymentPackages_->setItem(row, 0, new QTableWidgetItem(name));
    deploymentPackages_->setItem(row, 1, new QTableWidgetItem(version));
    deploymentPackages_->setItem(row, 2, new QTableWidgetItem("-"));
    deploymentPackages_->setItem(row, 3, new QTableWidgetItem(pkg.createdAt));

    emit packageAdded(pkg.id, name);
}

void DeploymentPlugin::removePackage(const QString& packageId) {
    for (int i = 0; i < packages_.size(); ++i) {
        if (packages_[i].id == packageId) {
            packages_.removeAt(i);
            deploymentPackages_->removeRow(i);
            emit packageRemoved(packageId);
            return;
        }
    }
}

void DeploymentPlugin::clearPackages() {
    packages_.clear();
    deploymentPackages_->setRowCount(0);
    nextPackageId_ = 1;
}

int DeploymentPlugin::packageCount() const {
    return packages_.size();
}

void DeploymentPlugin::deploy(const QString& targetId, const QString& packageId) {
    DeploymentRecord record;
    record.id = QString("deploy_%1").arg(nextRecordId_++);

    for (const auto& t : targets_) {
        if (t.id == targetId) {
            record.targetName = t.name;
            break;
        }
    }
    for (const auto& p : packages_) {
        if (p.id == packageId) {
            record.packageName = p.name;
            record.version = p.version;
            break;
        }
    }

    record.status = "Rejected";
    record.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    record.log = tr("Deployment of %1 v%2 to %3 requires a deployment backend acknowledgement.")
                     .arg(record.packageName, record.version, record.targetName);

    emit deploymentStarted(targetId, packageId);
    addDeploymentRecord(record);
}

void DeploymentPlugin::rollback(const QString& deploymentId) {
    for (const auto& r : records_) {
        if (r.id == deploymentId && r.status == QStringLiteral("Success")) {
            emit rollbackRequested(deploymentId);
            return;
        }
    }
}

void DeploymentPlugin::addDeploymentRecord(const DeploymentRecord& record) {
    records_.append(record);

    int row = deploymentHistory_->rowCount();
    deploymentHistory_->insertRow(row);
    deploymentHistory_->setItem(row, 0, new QTableWidgetItem(record.targetName));
    deploymentHistory_->setItem(row, 1, new QTableWidgetItem(record.packageName));
    deploymentHistory_->setItem(row, 2, new QTableWidgetItem(record.version));
    deploymentHistory_->setItem(row, 3, new QTableWidgetItem(record.status));
    deploymentHistory_->setItem(row, 4, new QTableWidgetItem(record.timestamp));
}

int DeploymentPlugin::deploymentHistoryCount() const {
    return records_.size();
}

bool DeploymentPlugin::exportDeploymentLog(const QString& filePath) {
    QJsonObject root;
    root["version"] = 1;
    root["totalDeployments"] = records_.size();

    QJsonArray recordsArray;
    for (const auto& record : records_) {
        QJsonObject recordObj;
        recordObj["id"] = record.id;
        recordObj["targetName"] = record.targetName;
        recordObj["packageName"] = record.packageName;
        recordObj["version"] = record.version;
        recordObj["status"] = record.status;
        recordObj["timestamp"] = record.timestamp;
        recordObj["log"] = record.log;
        recordsArray.append(recordObj);
    }
    root["deployments"] = recordsArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(QJsonDocument(root).toJson());
    return true;
}

void DeploymentPlugin::clearHistory() {
    records_.clear();
    deploymentHistory_->setRowCount(0);
    updateStatusText();
}

void DeploymentPlugin::updateStatusText() {
    QString status;
    status += tr("Targets: %1\n").arg(targets_.size());
    status += tr("Packages: %1\n").arg(packages_.size());
    status += tr("Deployments: %1\n").arg(records_.size());
    deploymentStatus_->setPlainText(status);
    statusLabel_->setText(tr("Status: Ready"));
}
