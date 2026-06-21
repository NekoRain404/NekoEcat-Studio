#include "WorkflowDocumentationBrowserService.h"

WorkflowDocumentationBrowserService::WorkflowDocumentationBrowserService(QObject *parent)
    : QObject(parent) {}

QString WorkflowDocumentationBrowserService::addNode(const QString &parentId,
                                                      const QString &title,
                                                      const QString &content) {
  QString id = QString("node_%1").arg(nextNodeId_++);
  WfDocNode n;
  n.id = id;
  n.parentId = parentId;
  n.title = title;
  n.content = content;
  n.level = 0;
  if (nodes_.contains(parentId)) {
    n.level = nodes_[parentId].level + 1;
    nodes_[parentId].childCount++;
  }
  nodes_[id] = n;
  emit nodeAdded(id);
  return id;
}

bool WorkflowDocumentationBrowserService::removeNode(const QString &nodeId) {
  if (!nodes_.contains(nodeId))
    return false;
  QString pid = nodes_[nodeId].parentId;
  if (nodes_.contains(pid) && nodes_[pid].childCount > 0)
    nodes_[pid].childCount--;
  QList<QString> childIds;
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
    if (it.value().parentId == nodeId)
      childIds << it.key();
  }
  for (const auto &cid : childIds)
    removeNode(cid);
  QList<int> bmToRemove;
  for (int i = 0; i < bookmarks_.size(); ++i) {
    if (bookmarks_[i].nodeId == nodeId)
      bmToRemove << i;
  }
  for (int i = bmToRemove.size() - 1; i >= 0; --i)
    bookmarks_.removeAt(bmToRemove[i]);
  nodes_.remove(nodeId);
  emit nodeRemoved(nodeId);
  return true;
}

WfDocNode WorkflowDocumentationBrowserService::node(const QString &nodeId) const {
  return nodes_.value(nodeId, WfDocNode{});
}

QVector<WfDocNode> WorkflowDocumentationBrowserService::childNodes(
    const QString &parentId) const {
  QVector<WfDocNode> result;
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
    if (it.value().parentId == parentId)
      result << it.value();
  }
  return result;
}

int WorkflowDocumentationBrowserService::nodeCount() const {
  return nodes_.size();
}

QVector<WfDocSearchResult> WorkflowDocumentationBrowserService::search(
    const QString &query) {
  QVector<WfDocSearchResult> results;
  QString q = query.toLower();
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
    const WfDocNode &n = it.value();
    double relevance = 0.0;
    if (n.title.toLower().contains(q)) {
      relevance = 1.0;
    } else if (n.content.toLower().contains(q)) {
      relevance = 0.5;
    }
    if (relevance > 0.0) {
      WfDocSearchResult r;
      r.nodeId = n.id;
      r.title = n.title;
      r.excerpt = n.content.left(200);
      r.relevance = relevance;
      results << r;
    }
  }
  emit searchCompleted(query, results.size());
  return results;
}

QString WorkflowDocumentationBrowserService::addBookmark(const QString &nodeId,
                                                          const QString &title,
                                                          const QString &note) {
  QString id = QString("bm_%1").arg(nextBookmarkId_++);
  WfDocBookmark bm;
  bm.id = id;
  bm.nodeId = nodeId;
  bm.title = title;
  bm.note = note;
  bm.createdAt = QDateTime::currentDateTime();
  bookmarks_ << bm;
  emit bookmarkAdded(id);
  return id;
}

bool WorkflowDocumentationBrowserService::removeBookmark(const QString &bookmarkId) {
  for (int i = 0; i < bookmarks_.size(); ++i) {
    if (bookmarks_[i].id == bookmarkId) {
      bookmarks_.removeAt(i);
      emit bookmarkRemoved(bookmarkId);
      return true;
    }
  }
  return false;
}

QVector<WfDocBookmark> WorkflowDocumentationBrowserService::allBookmarks() const {
  return bookmarks_;
}

int WorkflowDocumentationBrowserService::bookmarkCount() const {
  return bookmarks_.size();
}
