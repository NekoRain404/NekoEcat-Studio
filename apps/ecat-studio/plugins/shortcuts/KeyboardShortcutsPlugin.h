#pragma once

#include "plugins/WorkspacePlugin.h"

class QTableWidget;
class QPushButton;
class QLineEdit;
class QComboBox;

struct ShortcutEntry {
  QString action;
  QString defaultKey;
  QString currentKey;
  QString category;
};

class KeyboardShortcutsPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit KeyboardShortcutsPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  void addShortcut();
  void editShortcut();
  void deleteShortcut();
  void searchShortcuts(const QString &text);
  void exportShortcuts();
  void importShortcuts();
  void resetToDefaults();

  int shortcutCount() const;
  QString shortcutKey(int index) const;

private:
  void buildUi();
  void populateDefaults();
  void refreshTable();

  QWidget *container_ = nullptr;
  QTableWidget *table_ = nullptr;
  QLineEdit *searchEdit_ = nullptr;
  QComboBox *categoryFilter_ = nullptr;
  QPushButton *addBtn_ = nullptr;
  QPushButton *editBtn_ = nullptr;
  QPushButton *deleteBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *resetBtn_ = nullptr;
  QVector<ShortcutEntry> shortcuts_;
};
