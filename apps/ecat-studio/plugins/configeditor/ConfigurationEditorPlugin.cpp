#include "ConfigurationEditorPlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

ConfigurationEditorPlugin::ConfigurationEditorPlugin(QObject *parent) {
  if (parent) setParent(parent);
  configs_ = {
      {"Network", "master.name", "ecat0", "EtherCAT master interface name"},
      {"Network", "master.mode", "DC", "Distributed Clock mode"},
      {"Slave", "slave.0.vendor", "0x00000001", "Vendor ID"},
      {"Slave", "slave.0.product", "0x00000001", "Product Code"},
      {"Timing", "dc.sync0_cycle", "1000", "DC Sync0 cycle time in us"},
      {"Timing", "dc.sync0_shift", "0", "DC Sync0 shift in ns"},
  };
  buildUi();
}

QString ConfigurationEditorPlugin::id() const { return "configeditor"; }
QString ConfigurationEditorPlugin::displayName() const { return "Configuration Editor"; }
QString ConfigurationEditorPlugin::displayNameZh() const { return "配置编辑器"; }
int ConfigurationEditorPlugin::defaultOrder() const { return 265; }
bool ConfigurationEditorPlugin::visible() const { return false; }

void ConfigurationEditorPlugin::activate() {}
void ConfigurationEditorPlugin::deactivate() {}

QWidget *ConfigurationEditorPlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

int ConfigurationEditorPlugin::configCount() const { return configs_.size(); }

void ConfigurationEditorPlugin::addConfig(const ConfigEntry &entry) {
  configs_.append(entry);
  rebuildConfigTree();
}

void ConfigurationEditorPlugin::removeConfig(int index) {
  if (index >= 0 && index < configs_.size()) {
    configs_.removeAt(index);
    if (selectedIndex_ == index) selectedIndex_ = -1;
    else if (selectedIndex_ > index) --selectedIndex_;
    rebuildConfigTree();
  }
}

void ConfigurationEditorPlugin::updateConfig(int index, const QString &value) {
  if (index >= 0 && index < configs_.size()) {
    configs_[index].value = value;
    emit configChanged(index);
  }
}

void ConfigurationEditorPlugin::selectConfig(int index) {
  if (index < 0 || index >= configs_.size()) return;
  selectedIndex_ = index;
  if (configEditor_) {
    configEditor_->setText(configs_[index].key + " = " + configs_[index].value);
  }
  refreshPreview();
  emit configSelected(index);
}

int ConfigurationEditorPlugin::selectedConfig() const { return selectedIndex_; }

void ConfigurationEditorPlugin::validate() {
  errors_.clear();
  for (int i = 0; i < configs_.size(); ++i) {
    const auto &c = configs_[i];
    if (c.value.isEmpty()) {
      errors_.append({c.key, "Value is empty"});
    }
    if (c.key.isEmpty()) {
      errors_.append({c.key, "Key is empty"});
    }
  }
  rebuildValidationTable();
  if (statusLabel_) statusLabel_->setText(QString("Validation: %1 errors").arg(errors_.size()));
  emit validationCompleted(errors_.size());
}

int ConfigurationEditorPlugin::errorCount() const { return errors_.size(); }

bool ConfigurationEditorPlugin::exportConfig(const QString &path) {
  if (path.isEmpty()) return false;
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&f);
  for (const auto &c : configs_) {
    out << c.category << "." << c.key << " = " << c.value << "\n";
  }
  return out.status() == QTextStream::Ok && f.flush();
}

bool ConfigurationEditorPlugin::importConfig(const QString &path) {
  if (path.isEmpty()) return false;
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

  const int initialCount = configs_.size();
  QTextStream in(&f);
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    if (line.isEmpty() || line.startsWith('#')) continue;
    int eqIdx = line.indexOf('=');
    if (eqIdx > 0) {
      QString key = line.left(eqIdx).trimmed();
      QString value = line.mid(eqIdx + 1).trimmed();
      QStringList parts = key.split('.');
      QString category = parts.isEmpty() ? "General" : parts.first();
      configs_.append({category, key, value, "Imported"});
    }
  }
  if (configs_.size() == initialCount) return false;
  rebuildConfigTree();
  return true;
}

QTreeWidget *ConfigurationEditorPlugin::configTree() const { return configTree_; }
QTextEdit *ConfigurationEditorPlugin::configEditor() const { return configEditor_; }
QTextEdit *ConfigurationEditorPlugin::configPreview() const { return configPreview_; }
QTableWidget *ConfigurationEditorPlugin::validationTable() const { return validationTable_; }
QLabel *ConfigurationEditorPlugin::statusLabel() const { return statusLabel_; }

void ConfigurationEditorPlugin::refreshPreview() {
  if (!configPreview_) return;
  QString preview;
  for (const auto &c : configs_) {
    preview += c.key + " = " + c.value + "\n";
  }
  configPreview_->setText(preview);
}

void ConfigurationEditorPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  auto *splitter = new QSplitter;

  auto *leftPanel = new QWidget;
  auto *leftLayout = new QVBoxLayout(leftPanel);

  auto *searchRow = new QWidget;
  auto *searchLayout = new QHBoxLayout(searchRow);
  searchEdit_ = new QLineEdit;
  searchBtn_ = new QPushButton("Search");
  searchLayout->addWidget(searchEdit_);
  searchLayout->addWidget(searchBtn_);
  leftLayout->addWidget(searchRow);

  configTree_ = new QTreeWidget;
  configTree_->setHeaderLabels({"Key", "Value"});
  leftLayout->addWidget(configTree_);

  auto *btnRow = new QWidget;
  auto *btnLayout = new QHBoxLayout(btnRow);
  addBtn_ = new QPushButton("Add");
  removeBtn_ = new QPushButton("Remove");
  validateBtn_ = new QPushButton("Validate");
  importBtn_ = new QPushButton("Import");
  exportBtn_ = new QPushButton("Export");
  btnLayout->addWidget(addBtn_);
  btnLayout->addWidget(removeBtn_);
  btnLayout->addWidget(validateBtn_);
  btnLayout->addWidget(importBtn_);
  btnLayout->addWidget(exportBtn_);
  leftLayout->addWidget(btnRow);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);

  tabs_ = new QTabWidget;
  configEditor_ = new QTextEdit;
  tabs_->addTab(configEditor_, "Editor");
  configPreview_ = new QTextEdit;
  configPreview_->setReadOnly(true);
  tabs_->addTab(configPreview_, "Preview");
  validationTable_ = new QTableWidget;
  validationTable_->setColumnCount(2);
  validationTable_->setHorizontalHeaderLabels({"Key", "Error"});
  tabs_->addTab(validationTable_, "Validation");

  rightLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  rightLayout->addWidget(statusLabel_);

  splitter->addWidget(rightPanel);
  mainLayout->addWidget(splitter);

  rebuildConfigTree();

  connect(searchBtn_, &QPushButton::clicked, this, [this]() {
    QString query = searchEdit_->text();
    for (int i = 0; i < configTree_->topLevelItemCount(); ++i) {
      auto *item = configTree_->topLevelItem(i);
      bool match = item->text(0).contains(query, Qt::CaseInsensitive) ||
                   item->text(1).contains(query, Qt::CaseInsensitive);
      item->setHidden(!match && !query.isEmpty());
    }
  });
  connect(addBtn_, &QPushButton::clicked, this, [this]() {
    addConfig({"General", "new_key", "", "New configuration entry"});
  });
  connect(removeBtn_, &QPushButton::clicked, this, [this]() {
    if (selectedIndex_ >= 0) removeConfig(selectedIndex_);
  });
  connect(validateBtn_, &QPushButton::clicked, this, [this]() {
    validate();
  });
  connect(configTree_, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem *item, int) {
    for (int i = 0; i < configs_.size(); ++i) {
      if (configs_[i].key == item->text(0)) {
        selectConfig(i);
        break;
      }
    }
  });
}

void ConfigurationEditorPlugin::rebuildConfigTree() {
  if (!configTree_) return;
  configTree_->clear();
  QMap<QString, QTreeWidgetItem *> categories;
  for (const auto &c : configs_) {
    if (!categories.contains(c.category)) {
      auto *catItem = new QTreeWidgetItem(configTree_);
      catItem->setText(0, c.category);
      catItem->setExpanded(true);
      categories[c.category] = catItem;
    }
    auto *item = new QTreeWidgetItem(categories[c.category]);
    item->setText(0, c.key);
    item->setText(1, c.value);
  }
}

void ConfigurationEditorPlugin::rebuildValidationTable() {
  if (!validationTable_) return;
  validationTable_->setRowCount(errors_.size());
  for (int i = 0; i < errors_.size(); ++i) {
    validationTable_->setItem(i, 0, new QTableWidgetItem(errors_[i].key));
    validationTable_->setItem(i, 1, new QTableWidgetItem(errors_[i].message));
  }
}
