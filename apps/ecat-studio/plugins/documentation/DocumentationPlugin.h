#pragma once

// DocumentationPlugin — workspace plugin for browsing, searching, bookmarking,
// and annotating EtherCAT documentation with full-text search and export.

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;

class DocumentationPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DocumentationPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct DocEntry {
    QString id;
    QString title;
    QString category;
    QString content;
    QString tags;
  };

  struct Bookmark {
    QString docId;
    QString title;
    QString category;
    QDateTime addedAt;
  };

  struct Annotation {
    QString docId;
    QString text;
    QDateTime createdAt;
  };

  void addDocument(const DocEntry &entry);
  int documentCount() const;

  void addBookmark(const QString &docId, const QString &title, const QString &category);
  void removeBookmark(int index);
  int bookmarkCount() const;

  void addAnnotation(const QString &docId, const QString &text);
  void removeAnnotation(int index);
  int annotationCount() const;

  void search(const QString &query);
  int searchResultCount() const;

  QTreeWidget *docTree() const;
  QTextEdit *contentView() const;
  QTableWidget *searchResultsTable() const;
  QTableWidget *bookmarkTable() const;
  QTableWidget *annotationTable() const;

signals:
  void documentSelected(const QString &docId);
  void bookmarkAdded(const QString &docId);
  void searchCompleted(int resultCount);

public slots:
  void selectDocument(const QString &docId);
  bool exportDocumentation(const QString &path);
  bool exportBookmarks(const QString &path);
  bool exportAnnotations(const QString &path);

private:
  void buildUi();
  void buildDocTree();
  void updateContentView();
  int findDocIndex(const QString &docId) const;

  QWidget *containerWidget_ = nullptr;
  QTreeWidget *docTree_ = nullptr;
  QTextEdit *contentView_ = nullptr;
  QLineEdit *searchEdit_ = nullptr;
  QTableWidget *searchResultsTable_ = nullptr;
  QTableWidget *bookmarkTable_ = nullptr;
  QTableWidget *annotationTable_ = nullptr;
  QTabWidget *sideTabs_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QPushButton *searchBtn_ = nullptr;
  QPushButton *bookmarkBtn_ = nullptr;
  QPushButton *annotateBtn_ = nullptr;
  QPushButton *exportDocBtn_ = nullptr;
  QPushButton *exportBmBtn_ = nullptr;
  QPushButton *exportAnBtn_ = nullptr;

  QVector<DocEntry> documents_;
  QVector<Bookmark> bookmarks_;
  QVector<Annotation> annotations_;
  QVector<int> searchResults_;
  QString selectedDocId_;
};
