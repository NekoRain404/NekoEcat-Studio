#pragma once

// AutomationPlugin — workspace plugin for JavaScript script automation.
//
// Features:
//   - JavaScript script editor with syntax highlighting
//   - Script management (save, delete, select from list)
//   - Output console for script execution results
//   - Run/Stop execution controls
//   - Template insertion for common automation patterns
//   - Integration with ScriptingService for SDO, topology, and state operations
//   - Conditional compilation: requires ECAT_SCRIPTING_ENABLED flag
//
// UI Description:
//   The automation workspace has a split layout: script list on the left,
//   code editor on the top right, and output console on the bottom right.
//   Control buttons (Run, Stop, Save, Delete, Template) are in a toolbar.
//   Scripts can interact with EtherCAT services via the ScriptingService API.
//
// Constructor Pattern: Fine-grained injection (ScriptingService)
// Default Order: 120
// Conditional: Only available when ECAT_SCRIPTING_ENABLED is defined

#include "plugins/WorkspacePlugin.h"

#ifdef ECAT_SCRIPTING_ENABLED
    #include <QJSValue>
#endif

class QPlainTextEdit;
class QListWidget;
class QPushButton;
class ScriptingService;

class AutomationPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit AutomationPlugin(ScriptingService* scriptingService, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

private slots:
    void runCurrentScript();
    void stopScript();
    void saveCurrentScript();
    void deleteSelectedScript();
    void onScriptSelected(int index);
    void onScriptStarted(const QString& name);
#ifdef ECAT_SCRIPTING_ENABLED
    void onScriptCompleted(const QString& name, const QJSValue& result);
#else
    void onScriptCompleted(const QString& name, const QVariant& result);
#endif
    void onScriptError(const QString& name, const QString& error);
    void onLogMessage(const QString& message);
    void insertTemplate();

private:
    void buildUi();
    void refreshScriptList();

    ScriptingService* scriptingService_;
    QWidget* container_ = nullptr;
    QPlainTextEdit* editor_ = nullptr;
    QListWidget* scriptList_ = nullptr;
    QPlainTextEdit* console_ = nullptr;
    QPushButton* runBtn_ = nullptr;
    QPushButton* stopBtn_ = nullptr;
    QPushButton* saveBtn_ = nullptr;
    QPushButton* deleteBtn_ = nullptr;
    QPushButton* templateBtn_ = nullptr;
    bool running_ = false;
};
