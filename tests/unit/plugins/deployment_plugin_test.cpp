// TestDeploymentPlugin — Tests for DeploymentPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Target add/remove and status updates
//   - Package add/remove
//   - Deploy and rollback operations
//   - Deployment history and records
//   - Export deployment log
//   - Signal emissions

// DeploymentPlugin unit tests.

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QtTest/QtTest>

#include "plugins/deployment/DeploymentPlugin.h"

class TestDeploymentPlugin : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    // Verify plugin id, display names, order, and visibility
    void identity();
    // Verify main widget is created
    void widgetNotNull();
    // Verify deployment targets tree has items
    void deploymentTargets();
    // Verify deployment packages table has correct column count
    void deploymentPackages();
    // Verify deployment history table has correct column count
    void deploymentHistory();
    // Verify deployment status text edit is read-only
    void deploymentStatus();
    // Verify adding and removing targets updates count correctly
    void addAndRemoveTargets();
    // Verify target status update emits signal
    void updateTargetStatus();
    // Verify adding and removing packages updates count correctly
    void addAndRemovePackages();
    // Verify deploy fails closed without deployment backend acknowledgement
    void deploy();
    // Verify rollback does not synthesize completion without a successful deploy
    void rollback();
    // Verify manual deployment record can be added
    void deploymentRecords();
    // Verify clear history removes all records and table rows
    void clearHistory();
    // Verify export creates a log file
    void exportLog();
    // Verify all expected signals are emitted on operations
    void signalEmissions();
    // Verify source does not mint success without backend acknowledgement
    void sourceDoesNotMintSyntheticDeploymentSuccess();

private:
    DeploymentPlugin* plugin_ = nullptr;
};

void TestDeploymentPlugin::initTestCase() {
    plugin_ = new DeploymentPlugin(this);
}

void TestDeploymentPlugin::cleanupTestCase() {
    delete plugin_;
    plugin_ = nullptr;
}

void TestDeploymentPlugin::identity() {
    QCOMPARE(plugin_->id(), QString("deployment"));
    QCOMPARE(plugin_->displayName(), QString("Deployment"));
    QCOMPARE(plugin_->displayNameZh(), QString("部署"));
    QCOMPARE(plugin_->defaultOrder(), 260);
    QVERIFY(!plugin_->visible());
}

void TestDeploymentPlugin::widgetNotNull() {
    QVERIFY(plugin_->widget() != nullptr);
}

void TestDeploymentPlugin::deploymentTargets() {
    QVERIFY(plugin_->deploymentTargets() != nullptr);
    QVERIFY(plugin_->deploymentTargets()->topLevelItemCount() > 0);
}

void TestDeploymentPlugin::deploymentPackages() {
    QVERIFY(plugin_->deploymentPackages() != nullptr);
    QCOMPARE(plugin_->deploymentPackages()->columnCount(), 4);
}

void TestDeploymentPlugin::deploymentHistory() {
    QVERIFY(plugin_->deploymentHistory() != nullptr);
    QCOMPARE(plugin_->deploymentHistory()->columnCount(), 5);
}

void TestDeploymentPlugin::deploymentStatus() {
    QVERIFY(plugin_->deploymentStatus() != nullptr);
    QVERIFY(plugin_->deploymentStatus()->isReadOnly());
}

void TestDeploymentPlugin::addAndRemoveTargets() {
    plugin_->clearTargets();
    QCOMPARE(plugin_->targetCount(), 0);

    plugin_->addTarget("TestTarget", "192.168.1.100", "remote");
    QCOMPARE(plugin_->targetCount(), 1);

    plugin_->addTarget("LocalTarget", "localhost", "local");
    QCOMPARE(plugin_->targetCount(), 2);

    plugin_->removeTarget("target_1");
    QCOMPARE(plugin_->targetCount(), 1);

    plugin_->removeTarget("nonexistent");
    QCOMPARE(plugin_->targetCount(), 1);

    plugin_->removeTarget("target_2");
    QCOMPARE(plugin_->targetCount(), 0);
}

void TestDeploymentPlugin::updateTargetStatus() {
    plugin_->clearTargets();
    plugin_->clearPackages();
    plugin_->clearHistory();
    plugin_->addTarget("StatusTarget", "10.0.0.1", "remote");

    QSignalSpy statusSpy(plugin_, &DeploymentPlugin::targetStatusChanged);
    plugin_->updateTargetStatus("target_1", "Offline");
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.at(0).at(1).toString(), QString("Offline"));

    plugin_->removeTarget("target_1");
}

void TestDeploymentPlugin::addAndRemovePackages() {
    plugin_->clearPackages();
    QCOMPARE(plugin_->packageCount(), 0);

    plugin_->addPackage("firmware", "1.0.0", "Initial release");
    QCOMPARE(plugin_->packageCount(), 1);

    plugin_->addPackage("firmware", "1.1.0", "Bug fixes");
    QCOMPARE(plugin_->packageCount(), 2);

    QCOMPARE(plugin_->deploymentPackages()->rowCount(), 2);

    plugin_->removePackage("pkg_1");
    QCOMPARE(plugin_->packageCount(), 1);

    plugin_->removePackage("nonexistent");
    QCOMPARE(plugin_->packageCount(), 1);

    plugin_->removePackage("pkg_2");
    QCOMPARE(plugin_->packageCount(), 0);
}

void TestDeploymentPlugin::deploy() {
    plugin_->clearTargets();
    plugin_->clearPackages();
    plugin_->clearHistory();
    plugin_->addTarget("DeployTarget", "192.168.1.50", "remote");
    plugin_->addPackage("ecat-firmware", "2.0.0");

    QSignalSpy startSpy(plugin_, &DeploymentPlugin::deploymentStarted);
    QSignalSpy finishSpy(plugin_, &DeploymentPlugin::deploymentFinished);

    plugin_->deploy("target_1", "pkg_1");

    QCOMPARE(startSpy.count(), 1);
    QCOMPARE(finishSpy.count(), 0);
    QCOMPARE(plugin_->deploymentHistoryCount(), 1);
    QCOMPARE(plugin_->deploymentHistory()->rowCount(), 1);
    QCOMPARE(plugin_->deploymentHistory()->item(0, 3)->text(), QString("Rejected"));

    plugin_->removeTarget("target_1");
    plugin_->removePackage("pkg_1");
    plugin_->clearHistory();
}

void TestDeploymentPlugin::rollback() {
    plugin_->clearTargets();
    plugin_->clearPackages();
    plugin_->clearHistory();
    plugin_->addTarget("RollbackTarget", "192.168.1.60", "remote");
    plugin_->addPackage("ecat-pkg", "1.0.0");
    plugin_->deploy("target_1", "pkg_1");

    QSignalSpy rollbackSpy(plugin_, &DeploymentPlugin::rollbackRequested);
    plugin_->rollback("deploy_1");

    QCOMPARE(rollbackSpy.count(), 0);
    QCOMPARE(plugin_->deploymentHistoryCount(), 1);
    QCOMPARE(plugin_->deploymentHistory()->item(0, 3)->text(), QString("Rejected"));

    plugin_->removeTarget("target_1");
    plugin_->removePackage("pkg_1");
    plugin_->clearHistory();
}

void TestDeploymentPlugin::deploymentRecords() {
    plugin_->clearHistory();
    QCOMPARE(plugin_->deploymentHistoryCount(), 0);

    DeploymentRecord record;
    record.id = "manual_1";
    record.targetName = "test-target";
    record.packageName = "test-pkg";
    record.version = "1.0.0";
    record.status = "Rejected";
    record.timestamp = "2025-01-01T00:00:00";
    record.log = "Manual test deployment";

    plugin_->addDeploymentRecord(record);
    QCOMPARE(plugin_->deploymentHistoryCount(), 1);

    plugin_->clearHistory();
}

void TestDeploymentPlugin::clearHistory() {
    plugin_->clearTargets();
    plugin_->clearPackages();
    plugin_->clearHistory();
    plugin_->addTarget("ClearTarget", "192.168.1.70", "remote");
    plugin_->addPackage("clear-pkg", "1.0.0");
    plugin_->deploy("target_1", "pkg_1");
    QCOMPARE(plugin_->deploymentHistory()->item(0, 3)->text(), QString("Rejected"));

    QCOMPARE(plugin_->deploymentHistoryCount(), 1);

    plugin_->clearHistory();
    QCOMPARE(plugin_->deploymentHistoryCount(), 0);
    QCOMPARE(plugin_->deploymentHistory()->rowCount(), 0);

    plugin_->removeTarget("target_1");
    plugin_->removePackage("pkg_1");
}

void TestDeploymentPlugin::exportLog() {
    plugin_->clearTargets();
    plugin_->clearPackages();
    plugin_->clearHistory();
    plugin_->addTarget("ExportTarget", "192.168.1.80", "remote");
    plugin_->addPackage("export-pkg", "1.0.0");
    plugin_->deploy("target_1", "pkg_1");
    QCOMPARE(plugin_->deploymentHistory()->item(0, 3)->text(), QString("Rejected"));

    QString tmpPath = QDir::tempPath() + "/deployment_log.json";
    QVERIFY(plugin_->exportDeploymentLog(tmpPath));
    QVERIFY(QFile::exists(tmpPath));

    QFile::remove(tmpPath);
    plugin_->removeTarget("target_1");
    plugin_->removePackage("pkg_1");
    plugin_->clearHistory();
}

void TestDeploymentPlugin::signalEmissions() {
    plugin_->clearTargets();
    plugin_->clearPackages();
    plugin_->clearHistory();
    QSignalSpy targetAddSpy(plugin_, &DeploymentPlugin::targetAdded);
    QSignalSpy targetRemoveSpy(plugin_, &DeploymentPlugin::targetRemoved);
    QSignalSpy pkgAddSpy(plugin_, &DeploymentPlugin::packageAdded);
    QSignalSpy pkgRemoveSpy(plugin_, &DeploymentPlugin::packageRemoved);
    QSignalSpy deploySpy(plugin_, &DeploymentPlugin::deploymentStarted);
    QSignalSpy finishSpy(plugin_, &DeploymentPlugin::deploymentFinished);

    plugin_->addTarget("SignalTarget", "192.168.1.90", "remote");
    QCOMPARE(targetAddSpy.count(), 1);
    QCOMPARE(targetAddSpy.at(0).at(1).toString(), QString("SignalTarget"));

    plugin_->addPackage("signal-pkg", "1.0.0");
    QCOMPARE(pkgAddSpy.count(), 1);
    QCOMPARE(pkgAddSpy.at(0).at(1).toString(), QString("signal-pkg"));

    plugin_->deploy("target_1", "pkg_1");
    QCOMPARE(deploySpy.count(), 1);
    QCOMPARE(finishSpy.count(), 0);

    plugin_->removeTarget("target_1");
    QCOMPARE(targetRemoveSpy.count(), 1);

    plugin_->removePackage("pkg_1");
    QCOMPARE(pkgRemoveSpy.count(), 1);

    plugin_->clearHistory();
}

void TestDeploymentPlugin::sourceDoesNotMintSyntheticDeploymentSuccess() {
    QFile file(QStringLiteral(SOURCE_ROOT "/apps/ecat-studio/plugins/deployment/DeploymentPlugin.cpp"));
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text), qPrintable(file.errorString()));
    const QString source = QString::fromUtf8(file.readAll());

    QVERIFY2(!source.contains(QStringLiteral("record.status = \"Success\"")),
             "Deployment UI must not mark deployments successful without backend acknowledgement");
    QVERIFY2(!source.contains(QStringLiteral("completed successfully")),
             "Deployment UI must not log synthetic successful completion");
    QVERIFY2(!source.contains(QStringLiteral("record.status = \"Rolled Back\"")),
             "Deployment UI must not mark rollback complete without backend acknowledgement");
}

QTEST_MAIN(TestDeploymentPlugin)
#include "deployment_plugin_test.moc"
