#include "ProjectPlugin.h"
#include "services/ProjectManagerService.h"
#include "services/ConfigurationService.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>

ProjectPlugin::ProjectPlugin(ProjectManagerService *projectService,
                             ConfigurationService *configService,
                             QObject *parent)
    : WorkspacePlugin()
    , projectService_(projectService)
    , configService_(configService)
{
    if (parent) setParent(parent);
    buildUi();

    connect(projectService_, &ProjectManagerService::projectOpened,
            this, [this](const QString &name) {
                projectLabel_->setText(name);
                refreshProjectTree();
            });
    connect(projectService_, &ProjectManagerService::projectSaved,
            this, [this](const QString &name) {
                projectLabel_->setText(name);
            });
    connect(projectService_, &ProjectManagerService::projectClosed,
            this, [this]() {
                projectLabel_->setText("Untitled");
                refreshProjectTree();
            });
}

QWidget *ProjectPlugin::widget() {
    return container_;
}

void ProjectPlugin::activate() {
    refreshProjectTree();
}

void ProjectPlugin::deactivate() {
}

void ProjectPlugin::buildUi() {
    container_ = new QWidget();
    auto *mainLayout = new QHBoxLayout(container_);

    auto *leftPanel = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftPanel);

    projectLabel_ = new QLabel("Untitled");
    projectLabel_->setStyleSheet("font-weight: bold; font-size: 14px;");
    leftLayout->addWidget(projectLabel_);

    auto *btnLayout = new QHBoxLayout();
    newBtn_ = new QPushButton("New");
    openBtn_ = new QPushButton("Open");
    saveBtn_ = new QPushButton("Save");
    saveAsBtn_ = new QPushButton("Save As");
    exportBtn_ = new QPushButton("Export");
    importBtn_ = new QPushButton("Import");
    btnLayout->addWidget(newBtn_);
    btnLayout->addWidget(openBtn_);
    btnLayout->addWidget(saveBtn_);
    btnLayout->addWidget(saveAsBtn_);
    btnLayout->addWidget(exportBtn_);
    btnLayout->addWidget(importBtn_);
    leftLayout->addLayout(btnLayout);

    tree_ = new QTreeWidget();
    tree_->setHeaderLabel("Project Sections");
    leftLayout->addWidget(tree_);

    mainLayout->addWidget(leftPanel, 1);

    configStack_ = new QStackedWidget();

    auto *overviewPage = new QWidget();
    auto *overviewLayout = new QFormLayout(overviewPage);
    projectNameEdit_ = new QLineEdit();
    overviewLayout->addRow("Project Name:", projectNameEdit_);
    auto *descEdit = new QPlainTextEdit();
    descEdit->setMaximumHeight(80);
    overviewLayout->addRow("Description:", descEdit);
    auto *versionEdit = new QLineEdit("1.0.0");
    overviewLayout->addRow("Version:", versionEdit);
    configStack_->addWidget(overviewPage);

    auto *masterPage = new QWidget();
    auto *masterLayout = new QFormLayout(masterPage);
    auto *adapterCombo = new QComboBox();
    masterLayout->addRow("Adapter:", adapterCombo);
    auto *cycleSpin = new QSpinBox();
    cycleSpin->setRange(100, 100000);
    cycleSpin->setValue(1000);
    cycleSpin->setSuffix(" us");
    masterLayout->addRow("Cycle Time:", cycleSpin);
    auto *dcCheck = new QCheckBox("Enable Distributed Clocks");
    dcCheck->setChecked(true);
    masterLayout->addRow(dcCheck);
    configStack_->addWidget(masterPage);

    auto *timingPage = new QWidget();
    auto *timingLayout = new QFormLayout(timingPage);
    auto *tcSpin = new QSpinBox();
    tcSpin->setRange(100, 100000);
    tcSpin->setValue(1000);
    tcSpin->setSuffix(" us");
    timingLayout->addRow("Cycle Time:", tcSpin);
    auto *sync0Spin = new QSpinBox();
    sync0Spin->setRange(0, 100000);
    timingLayout->addRow("Sync0 Shift:", sync0Spin);
    auto *sync1Spin = new QSpinBox();
    sync1Spin->setRange(0, 100000);
    timingLayout->addRow("Sync1 Shift:", sync1Spin);
    configStack_->addWidget(timingPage);

    auto *networkPage = new QWidget();
    auto *networkLayout = new QFormLayout(networkPage);
    networkLayout->addRow("IP Address:", new QLineEdit());
    networkLayout->addRow("Subnet Mask:", new QLineEdit());
    networkLayout->addRow("Gateway:", new QLineEdit());
    networkLayout->addRow("DNS:", new QLineEdit());
    configStack_->addWidget(networkPage);

    auto *safetyPage = new QWidget();
    auto *safetyLayout = new QFormLayout(safetyPage);
    auto *wdSpin = new QSpinBox();
    wdSpin->setRange(100, 60000);
    wdSpin->setValue(5000);
    wdSpin->setSuffix(" ms");
    safetyLayout->addRow("Watchdog Timeout:", wdSpin);
    auto *errCombo = new QComboBox();
    errCombo->addItems({"safeop", "preop", "none"});
    safetyLayout->addRow("Error Behavior:", errCombo);
    auto *autoRecCheck = new QCheckBox("Auto Recover");
    safetyLayout->addRow(autoRecCheck);
    configStack_->addWidget(safetyPage);

    mainLayout->addWidget(configStack_, 2);

    refreshProjectTree();

    connect(newBtn_, &QPushButton::clicked, this, &ProjectPlugin::onNewProject);
    connect(openBtn_, &QPushButton::clicked, this, &ProjectPlugin::onOpenProject);
    connect(saveBtn_, &QPushButton::clicked, this, &ProjectPlugin::onSaveProject);
    connect(saveAsBtn_, &QPushButton::clicked, this, &ProjectPlugin::onSaveProjectAs);
    connect(exportBtn_, &QPushButton::clicked, this, &ProjectPlugin::onExportProject);
    connect(importBtn_, &QPushButton::clicked, this, &ProjectPlugin::onImportProject);
    connect(tree_, &QTreeWidget::itemSelectionChanged,
            this, &ProjectPlugin::onTreeSelectionChanged);
}

void ProjectPlugin::refreshProjectTree() {
    tree_->clear();

    auto *overview = new QTreeWidgetItem(tree_, {"Overview"});
    overview->setData(0, Qt::UserRole, 0);

    auto *master = new QTreeWidgetItem(tree_, {"Master Configuration"});
    master->setData(0, Qt::UserRole, 1);

    auto *timing = new QTreeWidgetItem(tree_, {"Timing Configuration"});
    timing->setData(0, Qt::UserRole, 2);

    auto *network = new QTreeWidgetItem(tree_, {"Network Configuration"});
    network->setData(0, Qt::UserRole, 3);

    auto *safety = new QTreeWidgetItem(tree_, {"Safety Configuration"});
    safety->setData(0, Qt::UserRole, 4);

    auto *slaves = new QTreeWidgetItem(tree_, {"Slave Configurations"});
    slaves->setData(0, Qt::UserRole, -1);
    for (const auto &sc : configService_->slaveConfigs()) {
        auto *item = new QTreeWidgetItem(slaves,
            {QString("%1: %2").arg(sc.position).arg(sc.name)});
    }

    auto *sdo = new QTreeWidgetItem(tree_, {"SDO Configurations"});
    sdo->setData(0, Qt::UserRole, -1);

    auto *watch = new QTreeWidgetItem(tree_, {"Watch List"});
    watch->setData(0, Qt::UserRole, -1);

    auto *startup = new QTreeWidgetItem(tree_, {"Startup SDO List"});
    startup->setData(0, Qt::UserRole, -1);

    auto *io = new QTreeWidgetItem(tree_, {"I/O Variables"});
    io->setData(0, Qt::UserRole, -1);

    auto *notes = new QTreeWidgetItem(tree_, {"Notes"});
    notes->setData(0, Qt::UserRole, -1);

    tree_->expandAll();
}

void ProjectPlugin::onNewProject() {
    projectService_->createProject(projectNameEdit_->text().isEmpty()
                                       ? "Untitled"
                                       : projectNameEdit_->text());
}

void ProjectPlugin::onOpenProject() {
    QString path = QFileDialog::getOpenFileName(
        container_, "Open Project", QString(),
        "Ecat Project (*.ecat.json);;All Files (*)");
    if (!path.isEmpty())
        projectService_->openProject(path);
}

void ProjectPlugin::onSaveProject() {
    if (projectService_->projectPath().isEmpty()) {
        onSaveProjectAs();
        return;
    }
    projectService_->saveProject();
}

void ProjectPlugin::onSaveProjectAs() {
    QString path = QFileDialog::getSaveFileName(
        container_, "Save Project As", QString(),
        "Ecat Project (*.ecat.json);;All Files (*)");
    if (!path.isEmpty())
        projectService_->saveProjectAs(path);
}

void ProjectPlugin::onExportProject() {
    QString path = QFileDialog::getSaveFileName(
        container_, "Export Project", QString(),
        "Ecat Project (*.ecat.json);;All Files (*)");
    if (!path.isEmpty())
        projectService_->exportProject(path);
}

void ProjectPlugin::onImportProject() {
    QString path = QFileDialog::getOpenFileName(
        container_, "Import Project", QString(),
        "Ecat Project (*.ecat.json);;All Files (*)");
    if (!path.isEmpty())
        projectService_->importProject(path);
}

void ProjectPlugin::onTreeSelectionChanged() {
    auto items = tree_->selectedItems();
    if (items.isEmpty())
        return;
    int page = items.first()->data(0, Qt::UserRole).toInt();
    if (page >= 0)
        showConfigPage(page);
}

void ProjectPlugin::showConfigPage(int index) {
    if (index >= 0 && index < configStack_->count())
        configStack_->setCurrentIndex(index);
}
