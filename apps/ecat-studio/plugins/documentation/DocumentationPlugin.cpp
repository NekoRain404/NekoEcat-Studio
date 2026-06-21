#include "DocumentationPlugin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

DocumentationPlugin::DocumentationPlugin(QObject *parent) {
  if (parent) setParent(parent);
  documents_ = {
      {"quickstart", "Quick Start Guide", "Getting Started",
       "Welcome to NekoEcat Studio. This guide helps you get started.", "start,guide"},
      {"installation", "Installation", "Getting Started",
       "System requirements and installation steps.", "install,setup"},
      {"protocol", "EtherCAT Protocol", "Reference",
       "EtherCAT is a real-time industrial Ethernet protocol.", "ethercat,protocol"},
      {"od_ref", "Object Dictionary", "Reference",
       "The Object Dictionary contains all configurable parameters.", "od,object"},
      {"pdo_ref", "PDO Mapping", "Reference",
       "Process Data Objects define real-time data exchange.", "pdo,mapping"},
      {"sdo_api", "SDO Service API", "API Reference",
       "The SDO Service provides methods for reading and writing SDOs.", "sdo,api"},
      {"plugin_api", "Plugin API", "API Reference",
       "The Plugin API allows extending NekoEcat Studio.", "plugin,api"},
  };
  buildUi();
}

QString DocumentationPlugin::id() const { return "documentation"; }
QString DocumentationPlugin::displayName() const { return "Documentation"; }
QString DocumentationPlugin::displayNameZh() const { return "文档"; }

QIcon DocumentationPlugin::icon() const { return QIcon(); }

int DocumentationPlugin::defaultOrder() const { return 215; }
bool DocumentationPlugin::visible() const { return true; }

void DocumentationPlugin::activate() {}
void DocumentationPlugin::deactivate() {}

QWidget *DocumentationPlugin::widget() {
  if (!containerWidget_) {
    buildUi();
  }
  return containerWidget_;
}

void DocumentationPlugin::addDocument(const DocEntry &entry) {
  documents_.append(entry);
  buildDocTree();
}

int DocumentationPlugin::documentCount() const { return documents_.size(); }

void DocumentationPlugin::addBookmark(const QString &docId,
                                       const QString &title,
                                       const QString &category) {
  Bookmark bm;
  bm.docId = docId;
  bm.title = title;
  bm.category = category;
  bm.addedAt = QDateTime::currentDateTime();
  bookmarks_.append(bm);
  if (bookmarkTable_) {
    int row = bookmarkTable_->rowCount();
    bookmarkTable_->insertRow(row);
    bookmarkTable_->setItem(row, 0, new QTableWidgetItem(title));
    bookmarkTable_->setItem(row, 1, new QTableWidgetItem(category));
    bookmarkTable_->setItem(row, 2, new QTableWidgetItem(bm.addedAt.toString(Qt::ISODate)));
  }
  emit bookmarkAdded(docId);
}

void DocumentationPlugin::removeBookmark(int index) {
  if (index >= 0 && index < bookmarks_.size()) {
    bookmarks_.removeAt(index);
  }
}

int DocumentationPlugin::bookmarkCount() const { return bookmarks_.size(); }

void DocumentationPlugin::addAnnotation(const QString &docId,
                                         const QString &text) {
  Annotation ann;
  ann.docId = docId;
  ann.text = text;
  ann.createdAt = QDateTime::currentDateTime();
  annotations_.append(ann);
  if (annotationTable_) {
    int row = annotationTable_->rowCount();
    annotationTable_->insertRow(row);
    annotationTable_->setItem(row, 0, new QTableWidgetItem(docId));
    annotationTable_->setItem(row, 1, new QTableWidgetItem(text));
    annotationTable_->setItem(row, 2, new QTableWidgetItem(ann.createdAt.toString(Qt::ISODate)));
  }
}

void DocumentationPlugin::removeAnnotation(int index) {
  if (index >= 0 && index < annotations_.size()) {
    annotations_.removeAt(index);
  }
}

int DocumentationPlugin::annotationCount() const {
  return annotations_.size();
}

void DocumentationPlugin::search(const QString &query) {
  searchResults_.clear();
  searchResultsTable_->setRowCount(0);
  for (int i = 0; i < documents_.size(); ++i) {
    const auto &doc = documents_[i];
    if (doc.title.contains(query, Qt::CaseInsensitive) ||
        doc.content.contains(query, Qt::CaseInsensitive) ||
        doc.tags.contains(query, Qt::CaseInsensitive)) {
      searchResults_.append(i);
      int row = searchResultsTable_->rowCount();
      searchResultsTable_->insertRow(row);
      searchResultsTable_->setItem(row, 0, new QTableWidgetItem(doc.title));
      searchResultsTable_->setItem(row, 1, new QTableWidgetItem(doc.category));
      searchResultsTable_->setItem(row, 2, new QTableWidgetItem(doc.tags));
    }
  }
  if (statusLabel_) statusLabel_->setText(tr("%1 results").arg(searchResults_.size()));
  emit searchCompleted(searchResults_.size());
}

int DocumentationPlugin::searchResultCount() const {
  return searchResults_.size();
}

QTreeWidget *DocumentationPlugin::docTree() const { return docTree_; }
QTextEdit *DocumentationPlugin::contentView() const { return contentView_; }
QTableWidget *DocumentationPlugin::searchResultsTable() const {
  return searchResultsTable_;
}
QTableWidget *DocumentationPlugin::bookmarkTable() const {
  return bookmarkTable_;
}
QTableWidget *DocumentationPlugin::annotationTable() const {
  return annotationTable_;
}

void DocumentationPlugin::selectDocument(const QString &docId) {
  selectedDocId_ = docId;
  updateContentView();
  emit documentSelected(docId);
}

void DocumentationPlugin::exportDocumentation(const QString &path) {
  QFile f(path);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    for (const auto &doc : documents_) {
      out << doc.title << "\n" << doc.content << "\n\n";
    }
  }
}

void DocumentationPlugin::exportBookmarks(const QString &path) {
  QFile f(path);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    for (const auto &bm : bookmarks_) {
      out << bm.docId << "," << bm.title << "," << bm.category << "\n";
    }
  }
}

void DocumentationPlugin::exportAnnotations(const QString &path) {
  QFile f(path);
  if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&f);
    for (const auto &ann : annotations_) {
      out << ann.docId << "," << ann.text << "\n";
    }
  }
}

void DocumentationPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *mainLayout = new QHBoxLayout(containerWidget_);
  auto *splitter = new QSplitter;

  docTree_ = new QTreeWidget;
  docTree_->setHeaderLabel("Documents");
  splitter->addWidget(docTree_);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);

  contentView_ = new QTextEdit;
  contentView_->setReadOnly(true);
  rightLayout->addWidget(contentView_);

  auto *searchPanel = new QWidget;
  auto *searchLayout = new QHBoxLayout(searchPanel);
  searchEdit_ = new QLineEdit;
  searchBtn_ = new QPushButton("Search");
  searchLayout->addWidget(searchEdit_);
  searchLayout->addWidget(searchBtn_);
  rightLayout->addWidget(searchPanel);

  sideTabs_ = new QTabWidget;
  searchResultsTable_ = new QTableWidget;
  searchResultsTable_->setColumnCount(3);
  searchResultsTable_->setHorizontalHeaderLabels({"Title", "Category", "Tags"});
  sideTabs_->addTab(searchResultsTable_, "Search Results");

  bookmarkTable_ = new QTableWidget;
  bookmarkTable_->setColumnCount(3);
  bookmarkTable_->setHorizontalHeaderLabels({"Title", "Category", "Added"});
  sideTabs_->addTab(bookmarkTable_, "Bookmarks");

  annotationTable_ = new QTableWidget;
  annotationTable_->setColumnCount(3);
  annotationTable_->setHorizontalHeaderLabels({"Doc ID", "Text", "Created"});
  sideTabs_->addTab(annotationTable_, "Annotations");

  rightLayout->addWidget(sideTabs_);

  splitter->addWidget(rightPanel);
  mainLayout->addWidget(splitter);

  buildDocTree();
}

void DocumentationPlugin::buildDocTree() {
  if (!docTree_) return;
  docTree_->clear();
  for (const auto &doc : documents_) {
    auto *item = new QTreeWidgetItem(docTree_);
    item->setText(0, doc.title);
    item->setData(0, Qt::UserRole, doc.id);
  }
}

void DocumentationPlugin::updateContentView() {
  if (!contentView_) return;
  int idx = findDocIndex(selectedDocId_);
  if (idx >= 0) {
    contentView_->setText(documents_[idx].content);
  }
}

int DocumentationPlugin::findDocIndex(const QString &docId) const {
  for (int i = 0; i < documents_.size(); i++) {
    if (documents_[i].id == docId) return i;
  }
  return -1;
}
