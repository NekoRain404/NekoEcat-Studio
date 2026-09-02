#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTextEdit;
class QTreeWidget;

class DocumentationBrowserPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit DocumentationBrowserPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    QTreeWidget* docTree() const;
    QTextEdit* contentViewer() const;
    QLineEdit* searchBox() const;
    QListWidget* bookmarks() const;

    void addDocument(const QString& category, const QString& title);
    void removeDocument(const QString& title);
    void clearDocuments();
    int documentCount() const;

    void addBookmark(const QString& title);
    void removeBookmark(const QString& title);
    void clearBookmarks();
    int bookmarkCount() const;

    void setContentText(const QString& text);
    QString contentText() const;

    void setSearchQuery(const QString& query);
    QString searchQuery() const;

    bool exportDocumentation(const QString& filePath, const QString& format);

signals:
    void documentAdded(const QString& title);
    void documentRemoved(const QString& title);
    void bookmarkAdded(const QString& title);
    void bookmarkRemoved(const QString& title);
    void searchTriggered(const QString& query);
    void exportRequested();

private:
    void buildUi();

    QWidget* containerWidget_ = nullptr;
    QTreeWidget* docTree_ = nullptr;
    QTextEdit* contentViewer_ = nullptr;
    QLineEdit* searchBox_ = nullptr;
    QListWidget* bookmarks_ = nullptr;
    QPushButton* searchButton_ = nullptr;
    QPushButton* exportButton_ = nullptr;
    QPushButton* bookmarkButton_ = nullptr;
    QLabel* statusLabel_ = nullptr;
};
