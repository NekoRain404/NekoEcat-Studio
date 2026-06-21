#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;

struct UpdateEntry {
  QString id;
  QString name;
  QString currentVersion;
  QString availableVersion;
  QString description;
  QString status;
};

struct UpdateRecord {
  QString id;
  QString name;
  QString fromVersion;
  QString toVersion;
  QString status;
  QString timestamp;
  QString log;
};

class UpdateManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit UpdateManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QTableWidget *availableTable() const;
  QTableWidget *historyTable() const;
  QTextEdit *settingsPanel() const;
  QTextEdit *statusPanel() const;
  QLabel *statusLabel() const;

  int availableCount() const;
  int historyCount() const;

  void addAvailableUpdate(const UpdateEntry &entry);
  void removeAvailableUpdate(int index);
  void applyUpdate(int index);
  void rollbackUpdate(int historyIndex);
  void clearHistory();

  bool exportUpdateLog(const QString &filePath);

signals:
  void updateAvailable(const QString &updateId, const QString &name);
  void updateApplied(const QString &recordId, const QString &status);
  void rollbackRequested(const QString &recordId);

private:
  void buildUi();
  void refreshStatus();

  QWidget *container_ = nullptr;
  QTableWidget *availableTable_ = nullptr;
  QTableWidget *historyTable_ = nullptr;
  QTextEdit *settingsPanel_ = nullptr;
  QTextEdit *statusPanel_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QPushButton *applyBtn_ = nullptr;
  QPushButton *rollbackBtn_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;

  QVector<UpdateEntry> available_;
  QVector<UpdateRecord> history_;
  int nextId_ = 1;
};
