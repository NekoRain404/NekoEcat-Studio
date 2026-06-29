#include "TemplatePlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>

TemplatePlugin::TemplatePlugin(QObject *parent) {
  if (parent) setParent(parent);
  auto now = QDateTime::currentDateTime();
  templates_ = {
      {"default_network", "Default Network", "Network",
       "master {\n  name: \"ecat0\"\n  mode: \"DC\"\n}\n\nslave 0 {\n  vendor: 0x00000001\n  product: 0x00000001\n}\n",
       now, now},
      {"servo_drive", "Servo Drive", "Drive",
       "slave {\n  type: \"CoE\"\n  pdo_mapping {\n    rxpdo: 0x1600\n    txpdo: 0x1a00\n  }\n  dc_sync: true\n}\n",
       now, now},
      {"io_terminal", "I/O Terminal", "I/O",
       "slave {\n  type: \"simple\"\n  do_count: 4\n  di_count: 4\n  ai_count: 2\n}\n",
       now, now},
  };
  buildUi();
}

QString TemplatePlugin::id() const { return "template"; }
QString TemplatePlugin::displayName() const { return "Templates"; }
QString TemplatePlugin::displayNameZh() const { return "模板"; }
int TemplatePlugin::defaultOrder() const { return 225; }
bool TemplatePlugin::visible() const { return false; }

void TemplatePlugin::activate() {}
void TemplatePlugin::deactivate() {}

QWidget *TemplatePlugin::widget() {
  if (!containerWidget_) buildUi();
  return containerWidget_;
}

void TemplatePlugin::addTemplate(const TemplateEntry &entry) {
  templates_.append(entry);
  rebuildTemplateTable();
}

void TemplatePlugin::removeTemplate(int index) {
  if (index >= 0 && index < templates_.size()) {
    templates_.removeAt(index);
    if (selectedIndex_ == index) selectedIndex_ = -1;
    else if (selectedIndex_ > index) --selectedIndex_;
    rebuildTemplateTable();
  }
}

void TemplatePlugin::updateTemplate(int index, const QString &content) {
  if (index >= 0 && index < templates_.size()) {
    templates_[index].content = content;
    templates_[index].modifiedAt = QDateTime::currentDateTime();
    emit templateModified(index);
  }
}

int TemplatePlugin::templateCount() const { return templates_.size(); }

void TemplatePlugin::selectTemplate(int index) {
  if (index < 0 || index >= templates_.size()) return;
  selectedIndex_ = index;
  if (editor_) editor_->setText(templates_[index].content);
  refreshPreview();
  emit templateSelected(index);
}

int TemplatePlugin::selectedTemplate() const { return selectedIndex_; }

void TemplatePlugin::search(const QString &query) {
  searchResults_.clear();
  for (int i = 0; i < templates_.size(); ++i) {
    const auto &t = templates_[i];
    if (t.name.contains(query, Qt::CaseInsensitive) ||
        t.category.contains(query, Qt::CaseInsensitive) ||
        t.content.contains(query, Qt::CaseInsensitive)) {
      searchResults_.append(i);
    }
  }
  rebuildSearchResults();
  if (statusLabel_) statusLabel_->setText(tr("%1 results").arg(searchResults_.size()));
}

int TemplatePlugin::searchResultCount() const { return searchResults_.size(); }

bool TemplatePlugin::importTemplate(const QString &path) {
  if (path.isEmpty()) return false;
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

  TemplateEntry t;
  t.id = QFileInfo(path).baseName();
  t.name = t.id;
  t.category = "Imported";
  t.content = QString::fromUtf8(f.readAll());
  t.createdAt = QDateTime::currentDateTime();
  t.modifiedAt = t.createdAt;
  templates_.append(t);
  rebuildTemplateTable();
  return true;
}

bool TemplatePlugin::exportTemplate(const QString &path) {
  if (selectedIndex_ < 0 || selectedIndex_ >= templates_.size()) return false;
  if (path.isEmpty()) return false;
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&f);
  out << templates_[selectedIndex_].content;
  return out.status() == QTextStream::Ok && f.flush();
}

bool TemplatePlugin::exportAllTemplates(const QString &path) {
  if (path.isEmpty()) return false;
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&f);
  for (const auto &t : templates_) {
    out << "--- " << t.name << " [" << t.category << "] ---\n";
    out << t.content << "\n\n";
  }
  return out.status() == QTextStream::Ok && f.flush();
}

QTableWidget *TemplatePlugin::templateTable() const { return templateTable_; }
QTableWidget *TemplatePlugin::searchResultsTable() const { return searchResultsTable_; }
QTextEdit *TemplatePlugin::editor() const { return editor_; }
QTextEdit *TemplatePlugin::preview() const { return preview_; }
QLabel *TemplatePlugin::statusLabel() const { return statusLabel_; }

void TemplatePlugin::refreshPreview() {
  if (!preview_ || !editor_) return;
  preview_->setText(editor_->toPlainText());
}

void TemplatePlugin::buildUi() {
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

  templateTable_ = new QTableWidget;
  templateTable_->setColumnCount(3);
  templateTable_->setHorizontalHeaderLabels({"Name", "Category", "Modified"});
  leftLayout->addWidget(templateTable_);

  auto *btnRow = new QWidget;
  auto *btnLayout = new QHBoxLayout(btnRow);
  addBtn_ = new QPushButton("Add");
  removeBtn_ = new QPushButton("Remove");
  importBtn_ = new QPushButton("Import");
  exportBtn_ = new QPushButton("Export");
  exportAllBtn_ = new QPushButton("Export All");
  btnLayout->addWidget(addBtn_);
  btnLayout->addWidget(removeBtn_);
  btnLayout->addWidget(importBtn_);
  btnLayout->addWidget(exportBtn_);
  btnLayout->addWidget(exportAllBtn_);
  leftLayout->addWidget(btnRow);

  splitter->addWidget(leftPanel);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);

  tabs_ = new QTabWidget;
  editor_ = new QTextEdit;
  tabs_->addTab(editor_, "Editor");
  preview_ = new QTextEdit;
  preview_->setReadOnly(true);
  tabs_->addTab(preview_, "Preview");

  searchResultsTable_ = new QTableWidget;
  searchResultsTable_->setColumnCount(3);
  searchResultsTable_->setHorizontalHeaderLabels({"Name", "Category", "Match"});
  tabs_->addTab(searchResultsTable_, "Search Results");

  rightLayout->addWidget(tabs_);

  statusLabel_ = new QLabel("Ready");
  rightLayout->addWidget(statusLabel_);

  splitter->addWidget(rightPanel);
  mainLayout->addWidget(splitter);

  rebuildTemplateTable();

  connect(searchBtn_, &QPushButton::clicked, this, [this]() {
    search(searchEdit_->text());
  });
  connect(addBtn_, &QPushButton::clicked, this, [this]() {
    TemplateEntry t;
    t.id = "new_" + QString::number(templates_.size());
    t.name = "New Template";
    t.category = "Custom";
    t.content = "";
    t.createdAt = QDateTime::currentDateTime();
    t.modifiedAt = t.createdAt;
    templates_.append(t);
    rebuildTemplateTable();
  });
  connect(removeBtn_, &QPushButton::clicked, this, [this]() {
    int row = templateTable_->currentRow();
    if (row >= 0) removeTemplate(row);
  });
  connect(templateTable_, &QTableWidget::cellClicked, this, [this](int row, int) {
    selectTemplate(row);
  });
}

void TemplatePlugin::rebuildTemplateTable() {
  if (!templateTable_) return;
  templateTable_->setRowCount(templates_.size());
  for (int i = 0; i < templates_.size(); ++i) {
    const auto &t = templates_[i];
    templateTable_->setItem(i, 0, new QTableWidgetItem(t.name));
    templateTable_->setItem(i, 1, new QTableWidgetItem(t.category));
    templateTable_->setItem(i, 2, new QTableWidgetItem(t.modifiedAt.toString(Qt::ISODate)));
  }
}

void TemplatePlugin::rebuildSearchResults() {
  if (!searchResultsTable_) return;
  searchResultsTable_->setRowCount(searchResults_.size());
  for (int i = 0; i < searchResults_.size(); ++i) {
    const auto &t = templates_[searchResults_[i]];
    searchResultsTable_->setItem(i, 0, new QTableWidgetItem(t.name));
    searchResultsTable_->setItem(i, 1, new QTableWidgetItem(t.category));
    searchResultsTable_->setItem(i, 2, new QTableWidgetItem(t.content.left(80)));
  }
}
