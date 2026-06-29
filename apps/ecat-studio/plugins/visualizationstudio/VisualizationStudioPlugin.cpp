#include "VisualizationStudioPlugin.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

VisualizationStudioPlugin::VisualizationStudioPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString VisualizationStudioPlugin::id() const { return "visualizationstudio"; }
QString VisualizationStudioPlugin::displayName() const { return "Visualization Studio"; }
QString VisualizationStudioPlugin::displayNameZh() const { return QStringLiteral("可视化工作室"); }
QIcon VisualizationStudioPlugin::icon() const { return QIcon::fromTheme("preferences-desktop-display"); }
int VisualizationStudioPlugin::defaultOrder() const { return 340; }
bool VisualizationStudioPlugin::visible() const { return false; }

void VisualizationStudioPlugin::activate() {}
void VisualizationStudioPlugin::deactivate() {}

QWidget *VisualizationStudioPlugin::widget() { return containerWidget_; }
QWidget *VisualizationStudioPlugin::canvas() const { return canvas_; }
QListWidget *VisualizationStudioPlugin::dataSources() const { return dataSources_; }
QTreeWidget *VisualizationStudioPlugin::chartTypes() const { return chartTypes_; }
QTextEdit *VisualizationStudioPlugin::exportOptions() const { return exportOptions_; }

void VisualizationStudioPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *dsLabel = new QLabel(tr("Data Sources"));
  leftLayout->addWidget(dsLabel);

  dataSources_ = new QListWidget;
  dataSources_->setDragEnabled(true);
  leftLayout->addWidget(dataSources_);

  auto *dsButtonRow = new QHBoxLayout;
  auto *addDsBtn = new QPushButton(tr("Add Source"));
  dsButtonRow->addWidget(addDsBtn);
  auto *removeDsBtn = new QPushButton(tr("Remove"));
  dsButtonRow->addWidget(removeDsBtn);
  dsButtonRow->addStretch();
  leftLayout->addLayout(dsButtonRow);

  splitter->addWidget(leftPanel);

  auto *centerPanel = new QWidget;
  auto *centerLayout = new QVBoxLayout(centerPanel);
  centerLayout->setContentsMargins(4, 4, 4, 4);

  canvas_ = new QWidget;
  canvas_->setMinimumSize(400, 300);
  canvas_->setStyleSheet("background-color: white; border: 1px solid #ccc;");
  centerLayout->addWidget(canvas_, 1);

  statusLabel_ = new QLabel(tr("Ready"));
  centerLayout->addWidget(statusLabel_);

  splitter->addWidget(centerPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *chartLabel = new QLabel(tr("Chart Types"));
  rightLayout->addWidget(chartLabel);

  chartTypes_ = new QTreeWidget;
  chartTypes_->setHeaderLabel(tr("Charts"));
  auto *lineCategory = new QTreeWidgetItem(chartTypes_, {tr("Line Charts")});
  new QTreeWidgetItem(lineCategory, {tr("Single Line")});
  new QTreeWidgetItem(lineCategory, {tr("Multi Line")});
  auto *barCategory = new QTreeWidgetItem(chartTypes_, {tr("Bar Charts")});
  new QTreeWidgetItem(barCategory, {tr("Vertical Bar")});
  new QTreeWidgetItem(barCategory, {tr("Horizontal Bar")});
  auto *scatterCategory = new QTreeWidgetItem(chartTypes_, {tr("Scatter")});
  new QTreeWidgetItem(scatterCategory, {tr("XY Scatter")});
  new QTreeWidgetItem(scatterCategory, {tr("Bubble")});
  chartTypes_->expandAll();
  rightLayout->addWidget(chartTypes_);

  auto *exportLabel = new QLabel(tr("Export Options"));
  rightLayout->addWidget(exportLabel);

  exportOptions_ = new QTextEdit;
  exportOptions_->setPlaceholderText(tr("Configure export settings..."));
  exportOptions_->setMaximumHeight(100);
  rightLayout->addWidget(exportOptions_);

  auto *previewLabel = new QLabel(tr("Real-time Preview"));
  rightLayout->addWidget(previewLabel);

  previewPane_ = new QTextEdit;
  previewPane_->setReadOnly(true);
  previewPane_->setPlaceholderText(tr("Visualization preview will appear here..."));
  previewPane_->setMaximumHeight(150);
  rightLayout->addWidget(previewPane_);

  auto *exportRow = new QHBoxLayout;
  exportFormat_ = new QComboBox;
  exportFormat_->addItem("PNG");
  exportFormat_->addItem("SVG");
  exportFormat_->addItem("PDF");
  exportFormat_->addItem("CSV");
  exportRow->addWidget(exportFormat_);
  exportButton_ = new QPushButton(tr("Export"));
  exportRow->addWidget(exportButton_);
  importButton_ = new QPushButton(tr("Import Config"));
  exportRow->addWidget(importButton_);
  rightLayout->addLayout(exportRow);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 3);
  splitter->setStretchFactor(2, 1);

  mainLayout->addWidget(splitter);

  connect(addDsBtn, &QPushButton::clicked, this, [this]() {
    addDataSource(tr("New Source %1").arg(dataSources_->count() + 1));
  });
  connect(removeDsBtn, &QPushButton::clicked, this, [this]() {
    auto *item = dataSources_->currentItem();
    if (item) removeDataSource(item->text());
  });
  connect(exportButton_, &QPushButton::clicked, this, &VisualizationStudioPlugin::exportRequested);
  connect(importButton_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getOpenFileName(containerWidget_, tr("Import Config"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) importConfig(path);
  });
}

void VisualizationStudioPlugin::addDataSource(const QString &name) {
  dataSources_->addItem(name);
  statusLabel_->setText(tr("Added data source: %1").arg(name));
  emit dataSourceAdded(name);
}

void VisualizationStudioPlugin::removeDataSource(const QString &name) {
  for (int i = 0; i < dataSources_->count(); ++i) {
    if (dataSources_->item(i)->text() == name) {
      delete dataSources_->takeItem(i);
      statusLabel_->setText(tr("Removed data source: %1").arg(name));
      emit dataSourceRemoved(name);
      return;
    }
  }
}

void VisualizationStudioPlugin::clearDataSources() { dataSources_->clear(); }
int VisualizationStudioPlugin::dataSourceCount() const { return dataSources_->count(); }

void VisualizationStudioPlugin::addChartType(const QString &category, const QString &name) {
  for (int i = 0; i < chartTypes_->topLevelItemCount(); ++i) {
    auto *cat = chartTypes_->topLevelItem(i);
    if (cat->text(0) == category) {
      new QTreeWidgetItem(cat, {name});
      emit chartTypeAdded(name);
      return;
    }
  }
  auto *cat = new QTreeWidgetItem(chartTypes_, {category});
  new QTreeWidgetItem(cat, {name});
  emit chartTypeAdded(name);
}

void VisualizationStudioPlugin::removeChartType(const QString &name) {
  for (int i = 0; i < chartTypes_->topLevelItemCount(); ++i) {
    auto *cat = chartTypes_->topLevelItem(i);
    for (int j = 0; j < cat->childCount(); ++j) {
      if (cat->child(j)->text(0) == name) {
        delete cat->takeChild(j);
        emit chartTypeRemoved(name);
        return;
      }
    }
  }
}

void VisualizationStudioPlugin::clearChartTypes() { chartTypes_->clear(); }
int VisualizationStudioPlugin::chartTypeCount() const {
  int count = 0;
  for (int i = 0; i < chartTypes_->topLevelItemCount(); ++i) {
    count += chartTypes_->topLevelItem(i)->childCount();
  }
  return count;
}

void VisualizationStudioPlugin::setPreviewText(const QString &text) {
  previewPane_->setPlainText(text);
  emit previewUpdated();
}

QString VisualizationStudioPlugin::previewText() const {
  return previewPane_->toPlainText();
}

bool VisualizationStudioPlugin::exportVisualization(const QString &filePath, const QString &format) {
  if (filePath.isEmpty()) return false;

  QJsonObject root;
  root["version"] = 1;
  root["format"] = format;
  QJsonArray sources;
  for (int i = 0; i < dataSources_->count(); ++i) {
    sources.append(dataSources_->item(i)->text());
  }
  root["dataSources"] = sources;
  root["exportOptions"] = exportOptions_->toPlainText();
  root["preview"] = previewPane_->toPlainText();

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  const QByteArray bytes = QJsonDocument(root).toJson();
  return file.write(bytes) == bytes.size() && file.flush();
}

bool VisualizationStudioPlugin::importConfig(const QString &filePath) {
  if (filePath.isEmpty()) return false;

  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) return false;
  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) return false;

  QJsonObject root = doc.object();
  if (!root.value("dataSources").isArray()) return false;

  QStringList importedSources;
  for (const auto &s : root["dataSources"].toArray()) {
    if (!s.isString()) return false;
    const QString source = s.toString().trimmed();
    if (source.isEmpty()) return false;
    importedSources.append(source);
  }
  if (importedSources.isEmpty()) return false;

  const QString importedExportOptions =
      root.contains("exportOptions") ? root["exportOptions"].toString() : QString();
  const QString importedPreview =
      root.contains("preview") ? root["preview"].toString() : QString();

  dataSources_->clear();
  for (const auto &source : importedSources)
    dataSources_->addItem(source);

  if (root.contains("exportOptions")) {
    exportOptions_->setPlainText(importedExportOptions);
  }
  if (root.contains("preview")) {
    previewPane_->setPlainText(importedPreview);
  }
  statusLabel_->setText(tr("Configuration imported"));
  return true;
}
