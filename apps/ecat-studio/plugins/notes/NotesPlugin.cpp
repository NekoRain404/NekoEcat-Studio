#include "NotesPlugin.h"
#include <QPlainTextEdit>
#include <QVBoxLayout>

NotesPlugin::NotesPlugin(QObject *parent) : WorkspacePlugin() {
  if (parent) setParent(parent);
  container_ = new QWidget;
  auto *layout = new QVBoxLayout(container_);
  layout->setContentsMargins(0, 0, 0, 0);
  editor_ = new QPlainTextEdit;
  editor_->setPlaceholderText(tr("Enter project notes here..."));
  layout->addWidget(editor_);
}

QString NotesPlugin::id() const { return "notes"; }
QString NotesPlugin::displayName() const { return "Notes"; }
QString NotesPlugin::displayNameZh() const { return "备注"; }
QWidget *NotesPlugin::widget() { return container_; }
int NotesPlugin::defaultOrder() const { return 100; }
bool NotesPlugin::visible() const { return true; }

QString NotesPlugin::notesText() const { return editor_->toPlainText(); }
void NotesPlugin::setNotesText(const QString &text) { editor_->setPlainText(text); }
