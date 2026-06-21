#pragma once

// WorkflowIntegrationService — connects workflows with external systems
// including CI/CD pipelines, issue trackers, communication platforms,
// and documentation systems.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QJsonObject>

struct CIConfig {
  QString server;
  QString token;
  QString project;
  QString pipelineId;
  QJsonObject parameters;
};

struct IssueTrackerConfig {
  QString server;
  QString token;
  QString project;
  QString issueType;
  QJsonObject customFields;
};

struct CommunicationConfig {
  QString server;
  QString channel;
  QString token;
  QString botName;
  QJsonObject settings;
};

struct DocumentationConfig {
  QString server;
  QString space;
  QString token;
  QString parentId;
  QJsonObject metadata;
};

class WorkflowIntegrationService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowIntegrationService(QObject *parent = nullptr);

  bool integrateWithCI(const CIConfig &config);
  bool integrateWithIssueTracker(const IssueTrackerConfig &config);
  bool integrateWithCommunication(const CommunicationConfig &config);
  bool integrateWithDocumentation(const DocumentationConfig &config);

  CIConfig ciConfig() const { return ciConfig_; }
  IssueTrackerConfig issueTrackerConfig() const { return issueTrackerConfig_; }
  CommunicationConfig communicationConfig() const { return commConfig_; }
  DocumentationConfig documentationConfig() const { return docConfig_; }

signals:
  void integrationConnected(const QString &system);
  void integrationDisconnected(const QString &system);
  void integrationError(const QString &system, const QString &error);

private:
  bool validateServer(const QString &server) const;
  bool validateToken(const QString &token) const;

  CIConfig ciConfig_;
  IssueTrackerConfig issueTrackerConfig_;
  CommunicationConfig commConfig_;
  DocumentationConfig docConfig_;
  bool ciConnected_ = false;
  bool issueTrackerConnected_ = false;
  bool commConnected_ = false;
  bool docConnected_ = false;
};
