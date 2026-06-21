#include "AutomationPlugin.h"
#include "services/ScriptingService.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

AutomationPlugin::AutomationPlugin(ScriptingService *scriptingService,
                                   QObject *parent)
    : scriptingService_(scriptingService) {
  if (parent) setParent(parent);
  buildUi();

  connect(scriptingService_, &ScriptingService::scriptStarted,
          this, &AutomationPlugin::onScriptStarted);
  connect(scriptingService_, &ScriptingService::scriptCompleted,
          this, &AutomationPlugin::onScriptCompleted);
  connect(scriptingService_, &ScriptingService::scriptError,
          this, &AutomationPlugin::onScriptError);
  connect(scriptingService_, &ScriptingService::logMessage,
          this, &AutomationPlugin::onLogMessage);
}

QString AutomationPlugin::id() const { return "automation"; }
QString AutomationPlugin::displayName() const { return "Automation"; }
QString AutomationPlugin::displayNameZh() const {
  return QStringLiteral("\xe8\x87\xaa\xe5\x8a\xa8\xe5\x8c\x96");
}
int AutomationPlugin::defaultOrder() const { return 120; }
bool AutomationPlugin::visible() const { return true; }

QWidget *AutomationPlugin::widget() { return container_; }

void AutomationPlugin::buildUi() {
  container_ = new QWidget;
  auto *rootLayout = new QVBoxLayout(container_);
  rootLayout->setContentsMargins(0, 0, 0, 0);

  auto *toolbar = new QWidget;
  auto *toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(4, 2, 4, 2);

  runBtn_ = new QPushButton(tr("Run"));
  stopBtn_ = new QPushButton(tr("Stop"));
  stopBtn_->setEnabled(false);
  saveBtn_ = new QPushButton(tr("Save"));
  deleteBtn_ = new QPushButton(tr("Delete"));
  templateBtn_ = new QPushButton(tr("Templates"));

  toolbarLayout->addWidget(runBtn_);
  toolbarLayout->addWidget(stopBtn_);
  toolbarLayout->addWidget(saveBtn_);
  toolbarLayout->addWidget(deleteBtn_);
  toolbarLayout->addWidget(templateBtn_);
  toolbarLayout->addStretch();
  rootLayout->addWidget(toolbar);

  auto *splitter = new QSplitter(Qt::Horizontal);

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->addWidget(new QLabel(tr("Scripts")));
  scriptList_ = new QListWidget;
  leftLayout->addWidget(scriptList_);
  splitter->addWidget(leftPanel);

  auto *rightSplitter = new QSplitter(Qt::Vertical);

  auto *editorPanel = new QWidget;
  auto *editorLayout = new QVBoxLayout(editorPanel);
  editorLayout->setContentsMargins(0, 0, 0, 0);
  editorLayout->addWidget(new QLabel(tr("Editor")));
  editor_ = new QPlainTextEdit;
  editor_->setPlaceholderText(tr("Enter JavaScript code here..."));
  editorLayout->addWidget(editor_);
  rightSplitter->addWidget(editorPanel);

  auto *consolePanel = new QWidget;
  auto *consoleLayout = new QVBoxLayout(consolePanel);
  consoleLayout->setContentsMargins(0, 0, 0, 0);
  consoleLayout->addWidget(new QLabel(tr("Output")));
  console_ = new QPlainTextEdit;
  console_->setReadOnly(true);
  console_->setMaximumBlockCount(5000);
  consoleLayout->addWidget(console_);
  rightSplitter->addWidget(consolePanel);

  splitter->addWidget(rightSplitter);
  splitter->setSizes({200, 600});
  rootLayout->addWidget(splitter);

  connect(runBtn_, &QPushButton::clicked,
          this, &AutomationPlugin::runCurrentScript);
  connect(stopBtn_, &QPushButton::clicked,
          this, &AutomationPlugin::stopScript);
  connect(saveBtn_, &QPushButton::clicked,
          this, &AutomationPlugin::saveCurrentScript);
  connect(deleteBtn_, &QPushButton::clicked,
          this, &AutomationPlugin::deleteSelectedScript);
  connect(scriptList_, &QListWidget::currentRowChanged,
          this, &AutomationPlugin::onScriptSelected);
  connect(templateBtn_, &QPushButton::clicked,
          this, &AutomationPlugin::insertTemplate);

  refreshScriptList();
}

void AutomationPlugin::runCurrentScript() {
  if (running_) return;
  running_ = true;
  runBtn_->setEnabled(false);
  stopBtn_->setEnabled(true);
  console_->appendPlainText(tr("--- Running script ---"));
  scriptingService_->executeScript(editor_->toPlainText());
}

void AutomationPlugin::stopScript() {
  scriptingService_->abortRunning();
  running_ = false;
  runBtn_->setEnabled(true);
  stopBtn_->setEnabled(false);
  console_->appendPlainText(tr("--- Script stopped ---"));
}

void AutomationPlugin::saveCurrentScript() {
  QString name = scriptList_->currentItem()
                     ? scriptList_->currentItem()->text()
                     : tr("untitled");
  scriptingService_->saveScript(name, editor_->toPlainText());
  refreshScriptList();
}

void AutomationPlugin::deleteSelectedScript() {
  if (!scriptList_->currentItem()) return;
  scriptingService_->saveScript(scriptList_->currentItem()->text(),
                                QString());
  refreshScriptList();
}

void AutomationPlugin::onScriptSelected(int index) {
  if (index < 0) return;
  QString name = scriptList_->item(index)->text();
  QString content = scriptingService_->loadScript(name);
  if (!content.isEmpty()) {
    editor_->setPlainText(content);
  }
}

void AutomationPlugin::onScriptStarted(const QString &name) {
  running_ = true;
  runBtn_->setEnabled(false);
  stopBtn_->setEnabled(true);
  console_->appendPlainText(tr("Script started: %1").arg(name));
}

void AutomationPlugin::onScriptCompleted(const QString &name,
#ifdef ECAT_SCRIPTING_ENABLED
                                         const QJSValue &result) {
  QString resultStr = result.toString();
#else
                                         const QVariant &result) {
  QString resultStr = result.toString();
#endif
  running_ = false;
  runBtn_->setEnabled(true);
  stopBtn_->setEnabled(false);
  console_->appendPlainText(
      tr("Script completed: %1 => %2").arg(name, resultStr));
}

void AutomationPlugin::onScriptError(const QString &name,
                                     const QString &error) {
  running_ = false;
  runBtn_->setEnabled(true);
  stopBtn_->setEnabled(false);
  console_->appendPlainText(tr("Script error [%1]: %2").arg(name, error));
}

void AutomationPlugin::onLogMessage(const QString &message) {
  console_->appendPlainText(message);
}

void AutomationPlugin::insertTemplate() {
  auto *menu = new QMenu(container_);
  menu->addAction(tr("Read SDO"), [this] {
    editor_->setPlainText(
        QStringLiteral("var val = readSDO(0, '1000', '0');\nlog('Value: ' + val);\n"));
  });
  menu->addAction(tr("Write SDO"), [this] {
    editor_->setPlainText(
        QStringLiteral("writeSDO(0, '1000', '0', '1', 'uint32');\nlog('Written');\n"));
  });
  menu->addAction(tr("Scan Topology"), [this] {
    editor_->setPlainText(
        QStringLiteral("var slaves = scanTopology();\n"
                       "for (var i = 0; i < slaves.length; i++) {\n"
                       "  log(slaves[i].position + ': ' + slaves[i].name);\n"
                       "}\n"));
  });
  menu->addAction(tr("Set All States"), [this] {
    editor_->setPlainText(
        QStringLiteral("setState(0, 'OP');\nlog('State set to OP');\n"));
  });
  menu->popup(QCursor::pos());
}

void AutomationPlugin::refreshScriptList() {
  scriptList_->clear();
  scriptList_->addItems(scriptingService_->listScripts());
}
