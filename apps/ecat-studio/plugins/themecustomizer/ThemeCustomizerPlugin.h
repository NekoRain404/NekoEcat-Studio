#pragma once

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QPushButton;
class QTableWidget;
class QLineEdit;
class QPlainTextEdit;
class QLabel;

class ThemeCustomizerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ThemeCustomizerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  void saveTheme();
  void loadTheme();
  void exportTheme();
  void importTheme();
  void resetToDefaults();
  void applyPreview();

private:
  void buildUi();
  void populateColorTable();
  void populateFontCombo();
  void updatePreview();

  QWidget *container_ = nullptr;
  QComboBox *themeSelector_ = nullptr;
  QTableWidget *colorTable_ = nullptr;
  QComboBox *fontCombo_ = nullptr;
  QComboBox *fontSizeCombo_ = nullptr;
  QPlainTextEdit *previewPane_ = nullptr;
  QLineEdit *themeNameEdit_ = nullptr;
  QPushButton *applyBtn_ = nullptr;
  QPushButton *saveBtn_ = nullptr;
  QPushButton *loadBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *resetBtn_ = nullptr;
  QLabel *previewLabel_ = nullptr;
};
