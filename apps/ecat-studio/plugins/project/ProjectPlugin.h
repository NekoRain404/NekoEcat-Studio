#pragma once

// ProjectPlugin — workspace plugin for project management and configuration.
// Provides a tree-based project navigator, configuration pages for master/slave/
// network/timing/safety settings, and project import/export capabilities.
// Uses ProjectManagerService for lifecycle and ConfigurationService for settings.

#include "plugins/WorkspacePlugin.h"

class ProjectManagerService;
class ConfigurationService;
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
class QPushButton;
class QLineEdit;
class QPlainTextEdit;
class QStackedWidget;
class QWidget;

class ProjectPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit ProjectPlugin(ProjectManagerService* projectService, ConfigurationService* configService,
                           QObject* parent = nullptr);

    QString id() const override { return "project"; }
    QString displayName() const override { return "Project"; }
    QString displayNameZh() const override { return "工程"; }
    int defaultOrder() const override { return 115; }
    bool visible() const override { return true; }
    QWidget* widget() override;

    void activate() override;
    void deactivate() override;

private:
    void buildUi();
    void refreshProjectTree();
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onSaveProjectAs();
    void onExportProject();
    void onImportProject();
    void onTreeSelectionChanged();
    void showConfigPage(int index);

    ProjectManagerService* projectService_;
    ConfigurationService* configService_;
    QWidget* container_ = nullptr;
    QTreeWidget* tree_ = nullptr;
    QStackedWidget* configStack_ = nullptr;
    QLabel* projectLabel_ = nullptr;
    QLineEdit* projectNameEdit_ = nullptr;
    QPushButton* newBtn_ = nullptr;
    QPushButton* openBtn_ = nullptr;
    QPushButton* saveBtn_ = nullptr;
    QPushButton* saveAsBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QPushButton* importBtn_ = nullptr;
};
