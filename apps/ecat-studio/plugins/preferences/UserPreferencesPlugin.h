#pragma once

#include "plugins/WorkspacePlugin.h"

class QTreeWidget;
class QStackedWidget;
class QPushButton;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QLabel;

struct PreferenceItem {
  QString key;
  QString name;
  QString description;
  QVariant defaultValue;
  QVariant currentValue;
  QString type;
  QStringList options;
};

struct PreferenceCategory {
  QString name;
  QVector<PreferenceItem> items;
};

class UserPreferencesPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit UserPreferencesPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  void applyPreferences();
  void resetPreferences();
  void exportPreferences();
  void importPreferences();
  void resetToDefaults();

private:
  void buildUi();
  void populateDefaults();
  void buildPreferenceTree();
  QWidget *createEditor(const PreferenceItem &item, int catIdx, int itemIdx);
  void onCategoryChanged(int index);

  QWidget *container_ = nullptr;
  QTreeWidget *tree_ = nullptr;
  QStackedWidget *editorStack_ = nullptr;
  QPushButton *applyBtn_ = nullptr;
  QPushButton *resetBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *defaultsBtn_ = nullptr;
  QLabel *descriptionLabel_ = nullptr;
  QVector<PreferenceCategory> categories_;
};
