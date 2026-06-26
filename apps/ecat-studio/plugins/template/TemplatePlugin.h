#pragma once

// TemplatePlugin — template management workspace for EtherCAT configurations.

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

class TemplatePlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit TemplatePlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct TemplateEntry {
    QString id;
    QString name;
    QString category;
    QString content;
    QDateTime createdAt;
    QDateTime modifiedAt;
  };

  void addTemplate(const TemplateEntry &entry);
  void removeTemplate(int index);
  void updateTemplate(int index, const QString &content);
  int templateCount() const;

  void selectTemplate(int index);
  int selectedTemplate() const;

  void search(const QString &query);
  int searchResultCount() const;

  void importTemplate(const QString &path);
  bool exportTemplate(const QString &path);
  bool exportAllTemplates(const QString &path);

  QTableWidget *templateTable() const;
  QTableWidget *searchResultsTable() const;
  QTextEdit *editor() const;
  QTextEdit *preview() const;
  QLabel *statusLabel() const;

signals:
  void templateSelected(int index);
  void templateModified(int index);

public slots:
  void refreshPreview();

private:
  void buildUi();
  void rebuildTemplateTable();
  void rebuildSearchResults();

  QWidget *containerWidget_ = nullptr;
  QTableWidget *templateTable_ = nullptr;
  QTableWidget *searchResultsTable_ = nullptr;
  QTextEdit *editor_ = nullptr;
  QTextEdit *preview_ = nullptr;
  QLineEdit *searchEdit_ = nullptr;
  QPushButton *searchBtn_ = nullptr;
  QPushButton *addBtn_ = nullptr;
  QPushButton *removeBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *exportAllBtn_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<TemplateEntry> templates_;
  QVector<int> searchResults_;
  int selectedIndex_ = -1;
};
