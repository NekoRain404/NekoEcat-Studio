#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;

class DigitalTwinStudioPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DigitalTwinStudioPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTableWidget *nodeTable() const;
  QTableWidget *connectionTable() const;
  QTextEdit *snapshotView() const;

  void addNode(const QString &name, const QString &type);
  void removeNode(const QString &name);
  int nodeCount() const;
  void clearNodes();

  void addConnectionEntry(const QString &source, const QString &target);
  void clearConnections();
  int connectionEntryCount() const;

  void takeSnapshot();
  int snapshotCount() const;
  void clearSnapshots();

  bool exportReport(const QString &filePath, const QString &format);

signals:
  void nodeAdded(const QString &name);
  void nodeRemovedSignal(const QString &name);
  void connectionEntryAdded(const QString &source, const QString &target);
  void snapshotTaken();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QTableWidget *nodeTable_ = nullptr;
  QTableWidget *connectionTable_ = nullptr;
  QTextEdit *snapshotView_ = nullptr;
  QPushButton *addNodeBtn_ = nullptr;
  QPushButton *removeNodeBtn_ = nullptr;
  QPushButton *addConnBtn_ = nullptr;
  QPushButton *snapshotBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  int snapshotCount_ = 0;
};
