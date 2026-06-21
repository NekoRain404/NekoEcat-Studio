#pragma once

// EtherCATDocumentationService — generates and searches API, user,
// developer, and system documentation.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>
#include <QDateTime>

class EcatClient;
class EventBus;

struct DocSection {
  QString title;
  QString content;
  int level = 1;
};

struct Documentation {
  QString title;
  QString content;
  QVector<DocSection> sections;
  QString format;
  QString version;
  QString author;
  QDateTime timestamp;
};

struct SearchResult {
  QString title;
  QString excerpt;
  QString source;
  double relevance = 0.0;
};

class EtherCATDocumentationService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATDocumentationService(EventBus *bus, EcatClient *client,
                                        QObject *parent = nullptr);

  Documentation generateApiDocumentation();
  Documentation generateUserDocumentation();
  Documentation generateDeveloperDocumentation();
  Documentation generateSystemDocumentation();
  QVector<SearchResult> searchDocumentation(const QString &query);

signals:
  void documentationGenerated(const Documentation &docs);

private:
  Documentation makeDoc(const QString &title, const QString &content,
                        const QVector<DocSection> &sections,
                        const QString &author);

  EventBus *bus_;
  EcatClient *client_;
};
