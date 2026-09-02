#pragma once
// NotesPlugin — free-text project notes workspace.
// First proof-of-concept plugin demonstrating the WorkspacePlugin interface.
#include "plugins/WorkspacePlugin.h"

class QPlainTextEdit;
struct AppSettings;

class NotesPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit NotesPlugin(QObject* parent = nullptr);

    // WorkspacePlugin interface
    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    // Project data
    QString notesText() const;
    void setNotesText(const QString& text);

private:
    QPlainTextEdit* editor_ = nullptr;
    QWidget* container_ = nullptr;
};
