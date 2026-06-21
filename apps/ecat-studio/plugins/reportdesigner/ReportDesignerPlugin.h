#pragma once

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;
class QTextEdit;
class QTreeWidget;

class ReportDesignerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ReportDesignerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  QWidget *layoutEditor() const;
  QListWidget *templates() const;
  QTreeWidget *dataBindings() const;
  QTextEdit *previewPane() const;

  void addTemplate(const QString &name);
  void removeTemplate(const QString &name);
  void clearTemplates();
  int templateCount() const;

  void addDataBinding(const QString &category, const QString &field);
  void removeDataBinding(const QString &field);
  void clearDataBindings();
  int dataBindingCount() const;

  void setPreviewText(const QString &text);
  QString previewText() const;

  bool exportReport(const QString &filePath, const QString &format);
  bool importTemplate(const QString &filePath);

signals:
  void templateAdded(const QString &name);
  void templateRemoved(const QString &name);
  void dataBindingAdded(const QString &field);
  void dataBindingRemoved(const QString &field);
  void exportRequested();
  void previewUpdated();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QWidget *layoutEditor_ = nullptr;
  QListWidget *templates_ = nullptr;
  QTreeWidget *dataBindings_ = nullptr;
  QTextEdit *previewPane_ = nullptr;
  QComboBox *exportFormat_ = nullptr;
  QPushButton *exportButton_ = nullptr;
  QPushButton *importButton_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
