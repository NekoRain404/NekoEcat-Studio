#pragma once

// WorkflowDocumentationBrowserService — browses and manages workflow documentation.
//
// Provides document tree navigation, bookmarking, search, and history
// tracking for workflow documentation.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QDateTime>

struct WfDocNode {
  QString id;
  QString parentId;
  QString title;
  QString content;
  int level = 0;
  int childCount = 0;
};

struct WfDocBookmark {
  QString id;
  QString nodeId;
  QString title;
  QString note;
  QDateTime createdAt;
};

struct WfDocSearchResult {
  QString nodeId;
  QString title;
  QString excerpt;
  double relevance = 0.0;
};

class WorkflowDocumentationBrowserService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowDocumentationBrowserService(QObject *parent = nullptr);

  QString addNode(const QString &parentId, const QString &title,
                  const QString &content);
  bool removeNode(const QString &nodeId);
  WfDocNode node(const QString &nodeId) const;
  QVector<WfDocNode> childNodes(const QString &parentId) const;
  int nodeCount() const;

  QVector<WfDocSearchResult> search(const QString &query);

  QString addBookmark(const QString &nodeId, const QString &title,
                      const QString &note = {});
  bool removeBookmark(const QString &bookmarkId);
  QVector<WfDocBookmark> allBookmarks() const;
  int bookmarkCount() const;

signals:
  void nodeAdded(const QString &nodeId);
  void nodeRemoved(const QString &nodeId);
  void bookmarkAdded(const QString &bookmarkId);
  void bookmarkRemoved(const QString &bookmarkId);
  void searchCompleted(const QString &query, int resultCount);

private:
  QMap<QString, WfDocNode> nodes_;
  QVector<WfDocBookmark> bookmarks_;
  int nextNodeId_ = 1;
  int nextBookmarkId_ = 1;
};
