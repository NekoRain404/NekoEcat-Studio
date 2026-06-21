#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class QTextEdit;
class QTreeWidget;

class ScriptLibraryPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ScriptLibraryPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTreeWidget *scriptTree() const;
  QPlainTextEdit *scriptEditor() const;
  QTextEdit *outputConsole() const;
  QTextEdit *docViewer() const;

  void addScript(const QString &category, const QString &name, const QString &content);
  void removeScript(const QString &name);
  void clearScripts();
  int scriptCount() const;

  void setCurrentScript(const QString &content);
  QString currentScript() const;

  void appendOutput(const QString &text);
  void clearOutput();
  QString output() const;

  void setDocumentation(const QString &text);
  QString documentation() const;

  bool exportScript(const QString &filePath, const QString &name);
  bool importScript(const QString &filePath);

signals:
  void scriptSelected(const QString &name);
  void runRequested();
  void scriptAdded(const QString &name);
  void scriptRemoved(const QString &name);

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QTreeWidget *scriptTree_ = nullptr;
  QPlainTextEdit *scriptEditor_ = nullptr;
  QTextEdit *outputConsole_ = nullptr;
  QTextEdit *docViewer_ = nullptr;
  QLineEdit *searchInput_ = nullptr;
  QPushButton *runBtn_ = nullptr;
  QPushButton *stopBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
