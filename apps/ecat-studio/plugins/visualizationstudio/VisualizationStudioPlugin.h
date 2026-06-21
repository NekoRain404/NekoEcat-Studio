#pragma once

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;
class QTextEdit;
class QTreeWidget;

class VisualizationStudioPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit VisualizationStudioPlugin(QObject *parent = nullptr);

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
  QListWidget *dataSources() const;
  QTreeWidget *chartTypes() const;
  QTextEdit *exportOptions() const;

  void addDataSource(const QString &name);
  void removeDataSource(const QString &name);
  void clearDataSources();
  int dataSourceCount() const;

  void addChartType(const QString &category, const QString &name);
  void removeChartType(const QString &name);
  void clearChartTypes();
  int chartTypeCount() const;

  void setPreviewText(const QString &text);
  QString previewText() const;

  bool exportVisualization(const QString &filePath, const QString &format);
  bool importConfig(const QString &filePath);

signals:
  void dataSourceAdded(const QString &name);
  void dataSourceRemoved(const QString &name);
  void chartTypeAdded(const QString &name);
  void chartTypeRemoved(const QString &name);
  void exportRequested();
  void previewUpdated();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QWidget *canvas_ = nullptr;
  QListWidget *dataSources_ = nullptr;
  QTreeWidget *chartTypes_ = nullptr;
  QTextEdit *exportOptions_ = nullptr;
  QTextEdit *previewPane_ = nullptr;
  QComboBox *exportFormat_ = nullptr;
  QPushButton *exportButton_ = nullptr;
  QPushButton *importButton_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
