#include "ScriptLibraryPlugin.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

ScriptLibraryPlugin::ScriptLibraryPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString ScriptLibraryPlugin::id() const { return "scriptlibrary"; }
QString ScriptLibraryPlugin::displayName() const { return "Script Library"; }
QString ScriptLibraryPlugin::displayNameZh() const { return QStringLiteral("脚本库"); }
QIcon ScriptLibraryPlugin::icon() const { return QIcon::fromTheme("text-x-script"); }
int ScriptLibraryPlugin::defaultOrder() const { return 200; }
bool ScriptLibraryPlugin::visible() const { return true; }

void ScriptLibraryPlugin::activate() {}
void ScriptLibraryPlugin::deactivate() {}

QWidget *ScriptLibraryPlugin::widget() { return containerWidget_; }
QTreeWidget *ScriptLibraryPlugin::scriptTree() const { return scriptTree_; }
QPlainTextEdit *ScriptLibraryPlugin::scriptEditor() const { return scriptEditor_; }
QTextEdit *ScriptLibraryPlugin::outputConsole() const { return outputConsole_; }
QTextEdit *ScriptLibraryPlugin::docViewer() const { return docViewer_; }

void ScriptLibraryPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *topRow = new QHBoxLayout;
  searchInput_ = new QLineEdit;
  searchInput_->setPlaceholderText(tr("Search scripts..."));
  searchInput_->setClearButtonEnabled(true);
  topRow->addWidget(searchInput_);

  runBtn_ = new QPushButton(tr("Run"));
  topRow->addWidget(runBtn_);
  stopBtn_ = new QPushButton(tr("Stop"));
  stopBtn_->setEnabled(false);
  topRow->addWidget(stopBtn_);
  importBtn_ = new QPushButton(tr("Import"));
  topRow->addWidget(importBtn_);
  exportBtn_ = new QPushButton(tr("Export"));
  topRow->addWidget(exportBtn_);

  statusLabel_ = new QLabel(tr("Ready"));
  topRow->addWidget(statusLabel_);

  mainLayout->addLayout(topRow);

  auto *splitter = new QSplitter(Qt::Horizontal);

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *treeLabel = new QLabel(tr("Script Categories"));
  leftLayout->addWidget(treeLabel);

  scriptTree_ = new QTreeWidget;
  scriptTree_->setHeaderLabel(tr("Scripts"));
  auto *builtinCategory = new QTreeWidgetItem(scriptTree_, {tr("Built-in")});
  new QTreeWidgetItem(builtinCategory, {tr("Read SDO")});
  new QTreeWidgetItem(builtinCategory, {tr("Write SDO")});
  new QTreeWidgetItem(builtinCategory, {tr("Scan Network")});
  auto *customCategory = new QTreeWidgetItem(scriptTree_, {tr("Custom")});
  new QTreeWidgetItem(customCategory, {tr("My Script 1")});
  scriptTree_->expandAll();
  leftLayout->addWidget(scriptTree_);

  splitter->addWidget(leftPanel);

  auto *centerPanel = new QWidget;
  auto *centerLayout = new QVBoxLayout(centerPanel);
  centerLayout->setContentsMargins(4, 4, 4, 4);

  auto *editorLabel = new QLabel(tr("Script Editor"));
  centerLayout->addWidget(editorLabel);

  scriptEditor_ = new QPlainTextEdit;
  scriptEditor_->setPlaceholderText(tr("# Write your script here..."));
  scriptEditor_->setLineWrapMode(QPlainTextEdit::NoWrap);
  centerLayout->addWidget(scriptEditor_);

  auto *outputLabel = new QLabel(tr("Output"));
  centerLayout->addWidget(outputLabel);

  outputConsole_ = new QTextEdit;
  outputConsole_->setReadOnly(true);
  outputConsole_->setMaximumHeight(150);
  outputConsole_->setStyleSheet("background-color: #1e1e1e; color: #d4d4d4; font-family: monospace;");
  centerLayout->addWidget(outputConsole_);

  splitter->addWidget(centerPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *docLabel = new QLabel(tr("Documentation"));
  rightLayout->addWidget(docLabel);

  docViewer_ = new QTextEdit;
  docViewer_->setReadOnly(true);
  docViewer_->setPlaceholderText(tr("Select a script to view documentation..."));
  rightLayout->addWidget(docViewer_);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 3);
  splitter->setStretchFactor(2, 1);

  mainLayout->addWidget(splitter);

  connect(runBtn_, &QPushButton::clicked, this, &ScriptLibraryPlugin::runRequested);
  connect(stopBtn_, &QPushButton::clicked, this, [this]() {
    statusLabel_->setText(tr("Stopped"));
    runBtn_->setEnabled(true);
    stopBtn_->setEnabled(false);
  });
  connect(importBtn_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getOpenFileName(containerWidget_, tr("Import Script"), QString(), "Scripts (*.py *.lua *.js);;All (*)");
    if (!path.isEmpty()) importScript(path);
  });
  connect(exportBtn_, &QPushButton::clicked, this, [this]() {
    QTreeWidgetItem *item = scriptTree_->currentItem();
    if (item && item->parent()) {
      QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Script"), item->text(0) + ".py", "Scripts (*.py);;All (*)");
      if (!path.isEmpty()) exportScript(path, item->text(0));
    }
  });
  connect(scriptTree_, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem *current, QTreeWidgetItem *) {
    if (current && current->parent()) {
      emit scriptSelected(current->text(0));
    }
  });
  connect(searchInput_, &QLineEdit::textChanged, this, [this](const QString &text) {
    for (int i = 0; i < scriptTree_->topLevelItemCount(); ++i) {
      auto *category = scriptTree_->topLevelItem(i);
      bool categoryVisible = false;
      for (int j = 0; j < category->childCount(); ++j) {
        auto *child = category->child(j);
        bool match = text.isEmpty() || child->text(0).contains(text, Qt::CaseInsensitive);
        child->setHidden(!match);
        if (match) categoryVisible = true;
      }
      category->setHidden(!categoryVisible && !text.isEmpty());
    }
  });
}

void ScriptLibraryPlugin::addScript(const QString &category, const QString &name, const QString &content) {
  QTreeWidgetItem *catItem = nullptr;
  for (int i = 0; i < scriptTree_->topLevelItemCount(); ++i) {
    if (scriptTree_->topLevelItem(i)->text(0) == category) {
      catItem = scriptTree_->topLevelItem(i);
      break;
    }
  }
  if (!catItem) {
    catItem = new QTreeWidgetItem(scriptTree_, {category});
  }
  auto *scriptItem = new QTreeWidgetItem(catItem, {name});
  scriptItem->setData(0, Qt::UserRole, content);
  scriptTree_->expandItem(catItem);
  emit scriptAdded(name);
}

void ScriptLibraryPlugin::removeScript(const QString &name) {
  for (int i = 0; i < scriptTree_->topLevelItemCount(); ++i) {
    auto *category = scriptTree_->topLevelItem(i);
    for (int j = 0; j < category->childCount(); ++j) {
      if (category->child(j)->text(0) == name) {
        delete category->takeChild(j);
        emit scriptRemoved(name);
        return;
      }
    }
  }
}

void ScriptLibraryPlugin::clearScripts() {
  scriptTree_->clear();
}

int ScriptLibraryPlugin::scriptCount() const {
  int count = 0;
  for (int i = 0; i < scriptTree_->topLevelItemCount(); ++i) {
    count += scriptTree_->topLevelItem(i)->childCount();
  }
  return count;
}

void ScriptLibraryPlugin::setCurrentScript(const QString &content) {
  scriptEditor_->setPlainText(content);
}

QString ScriptLibraryPlugin::currentScript() const {
  return scriptEditor_->toPlainText();
}

void ScriptLibraryPlugin::appendOutput(const QString &text) {
  outputConsole_->append(text);
}

void ScriptLibraryPlugin::clearOutput() {
  outputConsole_->clear();
}

QString ScriptLibraryPlugin::output() const {
  return outputConsole_->toPlainText();
}

void ScriptLibraryPlugin::setDocumentation(const QString &text) {
  docViewer_->setHtml(text);
}

QString ScriptLibraryPlugin::documentation() const {
  return docViewer_->toPlainText();
}

bool ScriptLibraryPlugin::exportScript(const QString &filePath, const QString &name) {
  Q_UNUSED(name);
  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
  file.write(scriptEditor_->toPlainText().toUtf8());
  return true;
}

bool ScriptLibraryPlugin::importScript(const QString &filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
  QString content = QString::fromUtf8(file.readAll());
  QFileInfo fi(filePath);
  addScript(tr("Imported"), fi.baseName(), content);
  return true;
}
