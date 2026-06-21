#pragma once

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTextEdit;
class QTreeWidget;

class DiagramPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DiagramPlugin(QObject *parent = nullptr);

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
  QListWidget *shapeLibrary() const;
  QTextEdit *propertyEditor() const;

  void addShape(const QString &category, const QString &name);
  void removeShape(const QString &name);
  void clearShapes();
  int shapeCount() const;

  void setZoom(double factor);
  double zoom() const;

  void setPropertyText(const QString &text);
  QString propertyText() const;

  bool exportToJson(const QString &filePath);
  bool importFromJson(const QString &filePath);

signals:
  void shapeAdded(const QString &name);
  void shapeRemoved(const QString &name);
  void zoomChanged(double factor);
  void exportRequested();

private:
  void buildUi();

  QWidget *containerWidget_ = nullptr;
  QWidget *canvas_ = nullptr;
  QListWidget *shapeLibrary_ = nullptr;
  QTextEdit *propertyEditor_ = nullptr;
  QComboBox *exportFormat_ = nullptr;
  QPushButton *exportButton_ = nullptr;
  QPushButton *importButton_ = nullptr;
  QLineEdit *zoomInput_ = nullptr;
  QLabel *zoomLabel_ = nullptr;
  QTreeWidget *shapeTree_ = nullptr;
  double zoomFactor_ = 1.0;
};
