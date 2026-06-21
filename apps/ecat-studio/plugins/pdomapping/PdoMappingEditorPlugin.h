#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QVector>

class QComboBox;
class QLabel;
class QPushButton;
class QSplitter;
class QTableWidget;
class QTabWidget;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;

class PdoMappingCanvas;
class PdoMappingService;
class PdoMappingValidator;

class PdoMappingEditorPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit PdoMappingEditorPlugin(PdoMappingService *pdoService, QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  void setSlavePosition(int position);
  int slavePosition() const;

  void addPdoEntry(int smIndex, const QString &index, const QString &subIndex,
                   const QString &name, const QString &dataType, int bitSize,
                   bool isOutput);
  void removePdoEntry(int smIndex, int entryIndex);
  int pdoEntryCount(int smIndex) const;

  void validateMapping();
  bool hasErrors() const;
  int errorCount() const;

  bool exportMapping(const QString &filePath);
  bool importMapping(const QString &filePath);

  PdoMappingCanvas *canvas() const;
  QTreeWidget *pdoTree() const;
  QTableWidget *propertyTable() const;
  QTextEdit *validationPanel() const;

signals:
  void mappingChanged(int position);
  void validationCompleted(bool valid, int errorCount);

private:
  void buildUi();
  void rebuildPdoTree();
  void rebuildPropertyTable();
  void updateValidationDisplay();
  void populateSampleData();

  PdoMappingService *pdoService_ = nullptr;
  QWidget *containerWidget_ = nullptr;

  QSplitter *mainSplitter_ = nullptr;
  QTreeWidget *pdoTree_ = nullptr;
  PdoMappingCanvas *canvas_ = nullptr;
  QTableWidget *propertyTable_ = nullptr;
  QTextEdit *validationPanel_ = nullptr;

  QComboBox *slaveCombo_ = nullptr;
  QPushButton *validateBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QPushButton *importBtn_ = nullptr;
  QPushButton *addEntryBtn_ = nullptr;
  QPushButton *removeEntryBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  int slavePosition_ = 0;
  bool hasErrors_ = false;
  int errorCount_ = 0;
};
