#include "DiagramPlugin.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

DiagramPlugin::DiagramPlugin(QObject *parent) {
  if (parent) setParent(parent);
  buildUi();
}

QString DiagramPlugin::id() const { return "diagram"; }
QString DiagramPlugin::displayName() const { return "Diagram Editor"; }
QString DiagramPlugin::displayNameZh() const { return QStringLiteral("图表编辑器"); }
QIcon DiagramPlugin::icon() const { return QIcon::fromTheme("x-office-drawing"); }
int DiagramPlugin::defaultOrder() const { return 190; }
bool DiagramPlugin::visible() const { return true; }

void DiagramPlugin::activate() {}
void DiagramPlugin::deactivate() {}

QWidget *DiagramPlugin::widget() { return containerWidget_; }
QWidget *DiagramPlugin::canvas() const { return canvas_; }
QListWidget *DiagramPlugin::shapeLibrary() const { return shapeLibrary_; }
QTextEdit *DiagramPlugin::propertyEditor() const { return propertyEditor_; }

void DiagramPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(4, 4, 4, 4);

  auto *shapeLabel = new QLabel(tr("Shape Library"));
  leftLayout->addWidget(shapeLabel);

  shapeTree_ = new QTreeWidget;
  shapeTree_->setHeaderLabel(tr("Shapes"));
  auto *basicCategory = new QTreeWidgetItem(shapeTree_, {tr("Basic Shapes")});
  new QTreeWidgetItem(basicCategory, {tr("Rectangle")});
  new QTreeWidgetItem(basicCategory, {tr("Circle")});
  new QTreeWidgetItem(basicCategory, {tr("Line")});
  new QTreeWidgetItem(basicCategory, {tr("Arrow")});
  auto *ethercatCategory = new QTreeWidgetItem(shapeTree_, {tr("EtherCAT")});
  new QTreeWidgetItem(ethercatCategory, {tr("Slave Node")});
  new QTreeWidgetItem(ethercatCategory, {tr("Master Node")});
  new QTreeWidgetItem(ethercatCategory, {tr("Bus Segment")});
  shapeTree_->expandAll();
  leftLayout->addWidget(shapeTree_);

  shapeLibrary_ = new QListWidget;
  shapeLibrary_->setDragEnabled(true);
  leftLayout->addWidget(shapeLibrary_);

  splitter->addWidget(leftPanel);

  canvas_ = new QWidget;
  canvas_->setMinimumSize(400, 300);
  canvas_->setStyleSheet("background-color: white; border: 1px solid #ccc;");
  splitter->addWidget(canvas_);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(4, 4, 4, 4);

  auto *propLabel = new QLabel(tr("Properties"));
  rightLayout->addWidget(propLabel);

  propertyEditor_ = new QTextEdit;
  propertyEditor_->setPlaceholderText(tr("Select a shape to edit properties..."));
  rightLayout->addWidget(propertyEditor_);

  auto *zoomRow = new QHBoxLayout;
  zoomLabel_ = new QLabel(tr("Zoom:"));
  zoomRow->addWidget(zoomLabel_);
  zoomInput_ = new QLineEdit;
  zoomInput_->setText("100%");
  zoomInput_->setMaximumWidth(80);
  zoomRow->addWidget(zoomInput_);
  auto *zoomInBtn = new QPushButton(tr("+"));
  zoomInBtn->setMaximumWidth(30);
  zoomRow->addWidget(zoomInBtn);
  auto *zoomOutBtn = new QPushButton(tr("-"));
  zoomOutBtn->setMaximumWidth(30);
  zoomRow->addWidget(zoomOutBtn);
  zoomRow->addStretch();
  rightLayout->addLayout(zoomRow);

  auto *exportRow = new QHBoxLayout;
  exportFormat_ = new QComboBox;
  exportFormat_->addItem("PNG");
  exportFormat_->addItem("SVG");
  exportFormat_->addItem("PDF");
  exportRow->addWidget(exportFormat_);
  exportButton_ = new QPushButton(tr("Export"));
  exportRow->addWidget(exportButton_);
  importButton_ = new QPushButton(tr("Import JSON"));
  exportRow->addWidget(importButton_);
  rightLayout->addLayout(exportRow);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 3);
  splitter->setStretchFactor(2, 1);

  mainLayout->addWidget(splitter);

  connect(zoomInBtn, &QPushButton::clicked, this, [this]() {
    setZoom(zoomFactor_ * 1.25);
  });
  connect(zoomOutBtn, &QPushButton::clicked, this, [this]() {
    setZoom(zoomFactor_ / 1.25);
  });
  connect(zoomInput_, &QLineEdit::editingFinished, this, [this]() {
    QString text = zoomInput_->text();
    text.remove('%');
    bool ok = false;
    double val = text.toDouble(&ok);
    if (ok && val > 0) setZoom(val / 100.0);
  });
  connect(exportButton_, &QPushButton::clicked, this, &DiagramPlugin::exportRequested);
  connect(importButton_, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getOpenFileName(containerWidget_, tr("Import Diagram"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) importFromJson(path);
  });
}

void DiagramPlugin::addShape(const QString &category, const QString &name) {
  shapeLibrary_->addItem(category + ": " + name);
  emit shapeAdded(name);
}

void DiagramPlugin::removeShape(const QString &name) {
  for (int i = 0; i < shapeLibrary_->count(); ++i) {
    if (shapeLibrary_->item(i)->text().endsWith(name)) {
      delete shapeLibrary_->takeItem(i);
      emit shapeRemoved(name);
      return;
    }
  }
}

void DiagramPlugin::clearShapes() { shapeLibrary_->clear(); }
int DiagramPlugin::shapeCount() const { return shapeLibrary_->count(); }

void DiagramPlugin::setZoom(double factor) {
  zoomFactor_ = factor;
  zoomInput_->setText(QString("%1%").arg(static_cast<int>(factor * 100)));
  emit zoomChanged(factor);
}

double DiagramPlugin::zoom() const { return zoomFactor_; }

void DiagramPlugin::setPropertyText(const QString &text) {
  propertyEditor_->setPlainText(text);
}

QString DiagramPlugin::propertyText() const {
  return propertyEditor_->toPlainText();
}

bool DiagramPlugin::exportToJson(const QString &filePath) {
  QJsonObject root;
  root["version"] = 1;
  root["zoom"] = zoomFactor_;
  QJsonArray shapes;
  for (int i = 0; i < shapeLibrary_->count(); ++i) {
    shapes.append(shapeLibrary_->item(i)->text());
  }
  root["shapes"] = shapes;
  root["properties"] = propertyEditor_->toPlainText();

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) return false;
  file.write(QJsonDocument(root).toJson());
  return true;
}

bool DiagramPlugin::importFromJson(const QString &filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) return false;
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (doc.isNull()) return false;

  QJsonObject root = doc.object();
  if (root.contains("zoom")) setZoom(root["zoom"].toDouble());
  if (root.contains("shapes")) {
    shapeLibrary_->clear();
    for (const auto &s : root["shapes"].toArray()) {
      shapeLibrary_->addItem(s.toString());
    }
  }
  if (root.contains("properties")) {
    propertyEditor_->setPlainText(root["properties"].toString());
  }
  return true;
}
