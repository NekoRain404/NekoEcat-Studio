#pragma once

// ConfigurationEditorPlugin — configuration editing workspace for EtherCAT.

#include "plugins/WorkspacePlugin.h"

#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;

class ConfigurationEditorPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ConfigurationEditorPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct ConfigEntry {
    QString category;
    QString key;
    QString value;
    QString description;
  };

  struct ValidationError {
    QString key;
    QString message;
  };

  int configCount() const;
  void addConfig(const ConfigEntry &entry);
  void removeConfig(int index);
  void updateConfig(int index, const QString &value);
  void selectConfig(int index);
  int selectedConfig() const;

  void validate();
  int errorCount() const;

  void exportConfig(const QString &path);
  void importConfig(const QString &path);

  QTreeWidget *configTree() const;
  QTextEdit *configEditor() const;
  QTextEdit *configPreview() const;
  QTableWidget *validationTable() const;
  QLabel *statusLabel() const;

signals:
  void configSelected(int index);
  void configChanged(int index);
  void validationCompleted(int errorCount);

public slots:
  void refreshPreview();

private:
  void buildUi();
  void rebuildConfigTree();
  void rebuildValidationTable();

  QWidget *containerWidget_ = nullptr;
  QTreeWidget *configTree_ = nullptr;
  QTextEdit *configEditor_ = nullptr;
  QTextEdit *configPreview_ = nullptr;
  QTableWidget *validationTable_ = nullptr;
  QLineEdit *searchEdit_ = nullptr;
  QPushButton *searchBtn_ = nullptr;
  QPushButton *addBtn_ = nullptr;
  QPushButton *removeBtn_ = nullptr;
  QPushButton *validateBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<ConfigEntry> configs_;
  QVector<ValidationError> errors_;
  int selectedIndex_ = -1;
};
