#pragma once

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QTreeWidget;

struct WorkflowNode {
  QString id;
  QString name;
  QString category;
  QString description;
  int x;
  int y;
};

struct WorkflowConnection {
  QString fromNodeId;
  QString toNodeId;
  QString label;
};

class WorkflowDesignerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit WorkflowDesignerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QWidget *canvas() const;
  QTreeWidget *nodePalette() const;
  QTextEdit *propertyEditor() const;
  QTableWidget *executionMonitor() const;

  void addNode(const QString &category, const QString &name);
  void removeNode(const QString &nodeId);
  void clearNodes();
  int nodeCount() const;

  void addConnection(const QString &fromId, const QString &toId, const QString &label = QString());
  void removeConnection(int index);
  int connectionCount() const;

  void updateNodeStatus(const QString &nodeId, const QString &status);
  void setExecutionStatus(const QString &status);
  QString executionStatus() const;

  bool exportWorkflow(const QString &filePath);
  bool importWorkflow(const QString &filePath);

signals:
  void nodeAdded(const QString &nodeId, const QString &name);
  void nodeRemoved(const QString &nodeId);
  void connectionAdded(const QString &fromId, const QString &toId);
  void connectionRemoved(int index);
  void executionStatusChanged(const QString &status);
  void exportRequested();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QWidget *canvas_ = nullptr;
  QTreeWidget *nodePalette_ = nullptr;
  QTextEdit *propertyEditor_ = nullptr;
  QTableWidget *executionMonitor_ = nullptr;
  QLineEdit *nodeNameInput_ = nullptr;
  QPushButton *addNodeButton_ = nullptr;
  QPushButton *removeNodeButton_ = nullptr;
  QPushButton *clearButton_ = nullptr;
  QPushButton *exportButton_ = nullptr;
  QPushButton *importButton_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QVector<WorkflowNode> nodes_;
  QVector<WorkflowConnection> connections_;
  int nextNodeId_ = 1;
  QString executionStatus_;
};
