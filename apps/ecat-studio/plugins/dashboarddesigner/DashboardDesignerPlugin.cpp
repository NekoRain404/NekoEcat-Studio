#include "DashboardDesignerPlugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

DashboardDesignerPlugin::DashboardDesignerPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString DashboardDesignerPlugin::id() const { return "dashboarddesigner"; }
QString DashboardDesignerPlugin::displayName() const { return "Dashboard Designer"; }
QString DashboardDesignerPlugin::displayNameZh() const { return "仪表盘设计器"; }
int DashboardDesignerPlugin::defaultOrder() const { return 235; }
bool DashboardDesignerPlugin::visible() const { return false; }

void DashboardDesignerPlugin::activate() {}
void DashboardDesignerPlugin::deactivate() {}

QWidget *DashboardDesignerPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void DashboardDesignerPlugin::addWidget(const QString &type) {
  DashboardWidget w;
  w.id = "widget_" + QString::number(nextId_++);
  w.type = type;
  w.label = type;
  w.x = 0;
  w.y = 0;
  w.width = 100;
  w.height = 100;
  widgets_.append(w);
  rebuildCanvas();
  if (statusLabel_)
    statusLabel_->setText(tr("Widgets: %1").arg(widgets_.size()));
}

void DashboardDesignerPlugin::removeWidget(int index) {
  if (index < 0 || index >= widgets_.size()) return;
  widgets_.removeAt(index);
  if (selectedIndex_ == index)
    selectedIndex_ = -1;
  else if (selectedIndex_ > index)
    --selectedIndex_;
  rebuildCanvas();
  rebuildPropertyEditor();
  if (statusLabel_)
    statusLabel_->setText(tr("Widgets: %1").arg(widgets_.size()));
}

void DashboardDesignerPlugin::selectWidget(int index) {
  if (index < 0 || index >= widgets_.size()) return;
  selectedIndex_ = index;
  rebuildPropertyEditor();
  emit widgetSelected(index);
}

void DashboardDesignerPlugin::updateWidgetProperty(int index, const QString &key, const QString &value) {
  if (index < 0 || index >= widgets_.size()) return;
  if (key == "label")
    widgets_[index].label = value;
  else if (key == "x")
    widgets_[index].x = value.toInt();
  else if (key == "y")
    widgets_[index].y = value.toInt();
  else if (key == "width")
    widgets_[index].width = value.toInt();
  else if (key == "height")
    widgets_[index].height = value.toInt();
  else
    widgets_[index].properties[key] = value;
  rebuildCanvas();
  emit widgetModified(index);
}

int DashboardDesignerPlugin::widgetCount() const { return widgets_.size(); }

int DashboardDesignerPlugin::selectedWidget() const { return selectedIndex_; }

QString DashboardDesignerPlugin::exportConfiguration() const {
  QJsonArray arr;
  for (const auto &w : widgets_) {
    QJsonObject obj;
    obj["id"] = w.id;
    obj["type"] = w.type;
    obj["label"] = w.label;
    obj["x"] = w.x;
    obj["y"] = w.y;
    obj["width"] = w.width;
    obj["height"] = w.height;
    QJsonObject props;
    for (auto it = w.properties.begin(); it != w.properties.end(); ++it)
      props[it.key()] = it.value();
    obj["properties"] = props;
    arr.append(obj);
  }
  return QJsonDocument(arr).toJson(QJsonDocument::Indented);
}

void DashboardDesignerPlugin::importConfiguration(const QString &json) {
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  if (!doc.isArray()) return;
  widgets_.clear();
  selectedIndex_ = -1;
  nextId_ = 1;
  const QJsonArray arr = doc.array();
  for (const auto &val : arr) {
    QJsonObject obj = val.toObject();
    DashboardWidget w;
    w.id = obj["id"].toString();
    w.type = obj["type"].toString();
    w.label = obj["label"].toString();
    w.x = obj["x"].toInt();
    w.y = obj["y"].toInt();
    w.width = obj["width"].toInt();
    w.height = obj["height"].toInt();
    QJsonObject props = obj["properties"].toObject();
    for (auto it = props.begin(); it != props.end(); ++it)
      w.properties[it.key()] = it.value().toString();
    widgets_.append(w);
    int num = w.id.mid(7).toInt();
    if (num >= nextId_) nextId_ = num + 1;
  }
  rebuildCanvas();
  rebuildPropertyEditor();
  if (statusLabel_)
    statusLabel_->setText(tr("Widgets: %1").arg(widgets_.size()));
}

void DashboardDesignerPlugin::setPreviewMode(bool preview) {
  previewMode_ = preview;
  if (modeTabs_) modeTabs_->setCurrentIndex(preview ? 1 : 0);
}

bool DashboardDesignerPlugin::isPreviewMode() const { return previewMode_; }

QListWidget *DashboardDesignerPlugin::widgetPalette() const { return widgetPalette_; }
QTableWidget *DashboardDesignerPlugin::propertyEditor() const { return propertyEditor_; }
QTabWidget *DashboardDesignerPlugin::modeTabs() const { return modeTabs_; }
QLabel *DashboardDesignerPlugin::statusLabel() const { return statusLabel_; }

void DashboardDesignerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QVBoxLayout(containerWidget_);

  modeTabs_ = new QTabWidget;
  auto *designTab = new QWidget;
  auto *designLayout = new QHBoxLayout(designTab);

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->addWidget(new QLabel("Widgets"));
  widgetPalette_ = new QListWidget;
  widgetPalette_->addItems({"Gauge", "Chart", "Table", "LED", "Bar Graph", "Text Display"});
  leftLayout->addWidget(widgetPalette_);
  leftPanel->setMaximumWidth(160);
  designLayout->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);

  canvas_ = new QWidget;
  canvas_->setMinimumSize(400, 300);
  canvas_->setStyleSheet("background-color: #2b2b2b; border: 1px solid #555;");
  rightLayout->addWidget(canvas_, 1);

  propertyEditor_ = new QTableWidget;
  propertyEditor_->setColumnCount(2);
  propertyEditor_->setHorizontalHeaderLabels({"Property", "Value"});
  propertyEditor_->horizontalHeader()->setStretchLastSection(true);
  propertyEditor_->setMaximumHeight(180);
  rightLayout->addWidget(propertyEditor_);

  designLayout->addWidget(rightPanel, 1);

  auto *previewTab = new QWidget;
  auto *previewLayout = new QVBoxLayout(previewTab);
  auto *previewArea = new QWidget;
  previewArea->setMinimumSize(400, 300);
  previewArea->setStyleSheet("background-color: #1e1e1e; border: 1px solid #555;");
  previewLayout->addWidget(previewArea);

  modeTabs_->addTab(designTab, "Design");
  modeTabs_->addTab(previewTab, "Preview");

  mainLayout->addWidget(modeTabs_, 1);

  statusLabel_ = new QLabel("Ready");
  mainLayout->addWidget(statusLabel_);

  connect(widgetPalette_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
    if (item) addWidget(item->text().toLower().replace(" ", ""));
  });
  connect(propertyEditor_, &QTableWidget::cellChanged, this, [this](int row, int col) {
    if (selectedIndex_ < 0 || col != 1) return;
    auto *keyItem = propertyEditor_->item(row, 0);
    auto *valItem = propertyEditor_->item(row, 1);
    if (keyItem && valItem)
      updateWidgetProperty(selectedIndex_, keyItem->text(), valItem->text());
  });
}

void DashboardDesignerPlugin::rebuildPropertyEditor() {
  if (!propertyEditor_) return;
  propertyEditor_->blockSignals(true);
  propertyEditor_->setRowCount(0);
  if (selectedIndex_ < 0 || selectedIndex_ >= widgets_.size()) {
    propertyEditor_->blockSignals(false);
    return;
  }
  const auto &w = widgets_[selectedIndex_];
  auto addRow = [&](const QString &key, const QString &val) {
    int r = propertyEditor_->rowCount();
    propertyEditor_->insertRow(r);
    propertyEditor_->setItem(r, 0, new QTableWidgetItem(key));
    propertyEditor_->setItem(r, 1, new QTableWidgetItem(val));
  };
  addRow("id", w.id);
  addRow("type", w.type);
  addRow("label", w.label);
  addRow("x", QString::number(w.x));
  addRow("y", QString::number(w.y));
  addRow("width", QString::number(w.width));
  addRow("height", QString::number(w.height));
  for (auto it = w.properties.begin(); it != w.properties.end(); ++it)
    addRow(it.key(), it.value());
  propertyEditor_->blockSignals(false);
}

void DashboardDesignerPlugin::rebuildCanvas() {
  if (!canvas_) return;
  for (auto *child : canvas_->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly))
    child->deleteLater();
  auto *layout = canvas_->layout();
  if (!layout) {
    layout = new QVBoxLayout(canvas_);
    layout->setContentsMargins(0, 0, 0, 0);
  }
  for (int i = 0; i < widgets_.size(); ++i) {
    const auto &w = widgets_[i];
    auto *label = new QLabel(w.label + " (" + w.type + ")");
    label->setStyleSheet("color: #aaa; padding: 4px; border: 1px dashed #666; margin: 2px;");
    label->setProperty("widgetIndex", i);
    layout->addWidget(label);
  }
}
