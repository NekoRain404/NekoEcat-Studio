#include "DocumentationBrowserPlugin.h"

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

DocumentationBrowserPlugin::DocumentationBrowserPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString DocumentationBrowserPlugin::id() const {
    return "documentationbrowser";
}
QString DocumentationBrowserPlugin::displayName() const {
    return "Documentation Browser";
}
QString DocumentationBrowserPlugin::displayNameZh() const {
    return QStringLiteral("文档浏览器");
}
QIcon DocumentationBrowserPlugin::icon() const {
    return QIcon::fromTheme("help-browser");
}
int DocumentationBrowserPlugin::defaultOrder() const {
    return 350;
}
bool DocumentationBrowserPlugin::visible() const {
    return false;
}

void DocumentationBrowserPlugin::activate() {}
void DocumentationBrowserPlugin::deactivate() {}

QWidget* DocumentationBrowserPlugin::widget() {
    return containerWidget_;
}
QTreeWidget* DocumentationBrowserPlugin::docTree() const {
    return docTree_;
}
QTextEdit* DocumentationBrowserPlugin::contentViewer() const {
    return contentViewer_;
}
QLineEdit* DocumentationBrowserPlugin::searchBox() const {
    return searchBox_;
}
QListWidget* DocumentationBrowserPlugin::bookmarks() const {
    return bookmarks_;
}

void DocumentationBrowserPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QHBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter;

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    auto* searchRow = new QHBoxLayout;
    searchBox_ = new QLineEdit;
    searchBox_->setPlaceholderText(tr("Search documentation..."));
    searchRow->addWidget(searchBox_);
    searchButton_ = new QPushButton(tr("Search"));
    searchRow->addWidget(searchButton_);
    leftLayout->addLayout(searchRow);

    auto* treeLabel = new QLabel(tr("Documentation"));
    leftLayout->addWidget(treeLabel);

    docTree_ = new QTreeWidget;
    docTree_->setHeaderLabel(tr("Topics"));
    auto* gettingStarted = new QTreeWidgetItem(docTree_, {tr("Getting Started")});
    new QTreeWidgetItem(gettingStarted, {tr("Installation")});
    new QTreeWidgetItem(gettingStarted, {tr("Quick Start Guide")});
    new QTreeWidgetItem(gettingStarted, {tr("Configuration")});
    auto* ethercatRef = new QTreeWidgetItem(docTree_, {tr("EtherCAT Reference")});
    new QTreeWidgetItem(ethercatRef, {tr("Protocol Overview")});
    new QTreeWidgetItem(ethercatRef, {tr("Slave Configuration")});
    new QTreeWidgetItem(ethercatRef, {tr("Distributed Clocks")});
    new QTreeWidgetItem(ethercatRef, {tr("CoE / FoE / EoE")});
    auto* apiDocs = new QTreeWidgetItem(docTree_, {tr("API Documentation")});
    new QTreeWidgetItem(apiDocs, {tr("ecatd Commands")});
    new QTreeWidgetItem(apiDocs, {tr("JSON Protocol")});
    new QTreeWidgetItem(apiDocs, {tr("Service Interfaces")});
    auto* tutorials = new QTreeWidgetItem(docTree_, {tr("Tutorials")});
    new QTreeWidgetItem(tutorials, {tr("First Commissioning")});
    new QTreeWidgetItem(tutorials, {tr("Troubleshooting")});
    new QTreeWidgetItem(tutorials, {tr("Advanced Configuration")});
    docTree_->expandAll();
    leftLayout->addWidget(docTree_);

    splitter->addWidget(leftPanel);

    auto* centerPanel = new QWidget;
    auto* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(4, 4, 4, 4);

    auto* contentLabel = new QLabel(tr("Content Viewer"));
    centerLayout->addWidget(contentLabel);

    contentViewer_ = new QTextEdit;
    contentViewer_->setReadOnly(true);
    contentViewer_->setPlaceholderText(tr("Select a topic to view its documentation..."));
    centerLayout->addWidget(contentViewer_, 1);

    statusLabel_ = new QLabel(tr("Ready"));
    centerLayout->addWidget(statusLabel_);

    splitter->addWidget(centerPanel);

    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(4, 4, 4, 4);

    auto* bookmarkLabel = new QLabel(tr("Bookmarks"));
    rightLayout->addWidget(bookmarkLabel);

    bookmarks_ = new QListWidget;
    rightLayout->addWidget(bookmarks_);

    auto* bookmarkButtonRow = new QHBoxLayout;
    bookmarkButton_ = new QPushButton(tr("Add Bookmark"));
    bookmarkButtonRow->addWidget(bookmarkButton_);
    auto* removeBookmarkBtn = new QPushButton(tr("Remove"));
    bookmarkButtonRow->addWidget(removeBookmarkBtn);
    bookmarkButtonRow->addStretch();
    rightLayout->addLayout(bookmarkButtonRow);

    auto* exportRow = new QHBoxLayout;
    exportButton_ = new QPushButton(tr("Export Docs"));
    exportRow->addWidget(exportButton_);
    auto* clearBookmarksBtn = new QPushButton(tr("Clear Bookmarks"));
    exportRow->addWidget(clearBookmarksBtn);
    rightLayout->addLayout(exportRow);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setStretchFactor(2, 1);

    mainLayout->addWidget(splitter);

    connect(searchButton_, &QPushButton::clicked, this, [this]() {
        emit searchTriggered(searchBox_->text());
        statusLabel_->setText(tr("Search: %1").arg(searchBox_->text()));
    });
    connect(searchBox_, &QLineEdit::returnPressed, this, [this]() {
        emit searchTriggered(searchBox_->text());
        statusLabel_->setText(tr("Search: %1").arg(searchBox_->text()));
    });
    connect(bookmarkButton_, &QPushButton::clicked, this, [this]() {
        auto* item = docTree_->currentItem();
        if (item)
            addBookmark(item->text(0));
    });
    connect(removeBookmarkBtn, &QPushButton::clicked, this, [this]() {
        auto* item = bookmarks_->currentItem();
        if (item)
            removeBookmark(item->text());
    });
    connect(exportButton_, &QPushButton::clicked, this, &DocumentationBrowserPlugin::exportRequested);
    connect(clearBookmarksBtn, &QPushButton::clicked, this, &DocumentationBrowserPlugin::clearBookmarks);
}

void DocumentationBrowserPlugin::addDocument(const QString& category, const QString& title) {
    for (int i = 0; i < docTree_->topLevelItemCount(); ++i) {
        auto* cat = docTree_->topLevelItem(i);
        if (cat->text(0) == category) {
            new QTreeWidgetItem(cat, {title});
            emit documentAdded(title);
            return;
        }
    }
    auto* cat = new QTreeWidgetItem(docTree_, {category});
    new QTreeWidgetItem(cat, {title});
    emit documentAdded(title);
}

void DocumentationBrowserPlugin::removeDocument(const QString& title) {
    for (int i = 0; i < docTree_->topLevelItemCount(); ++i) {
        auto* cat = docTree_->topLevelItem(i);
        for (int j = 0; j < cat->childCount(); ++j) {
            if (cat->child(j)->text(0) == title) {
                delete cat->takeChild(j);
                emit documentRemoved(title);
                return;
            }
        }
    }
}

void DocumentationBrowserPlugin::clearDocuments() {
    docTree_->clear();
}
int DocumentationBrowserPlugin::documentCount() const {
    int count = 0;
    for (int i = 0; i < docTree_->topLevelItemCount(); ++i) {
        count += docTree_->topLevelItem(i)->childCount();
    }
    return count;
}

void DocumentationBrowserPlugin::addBookmark(const QString& title) {
    for (int i = 0; i < bookmarks_->count(); ++i) {
        if (bookmarks_->item(i)->text() == title)
            return;
    }
    bookmarks_->addItem(title);
    statusLabel_->setText(tr("Bookmarked: %1").arg(title));
    emit bookmarkAdded(title);
}

void DocumentationBrowserPlugin::removeBookmark(const QString& title) {
    for (int i = 0; i < bookmarks_->count(); ++i) {
        if (bookmarks_->item(i)->text() == title) {
            delete bookmarks_->takeItem(i);
            emit bookmarkRemoved(title);
            return;
        }
    }
}

void DocumentationBrowserPlugin::clearBookmarks() {
    bookmarks_->clear();
}
int DocumentationBrowserPlugin::bookmarkCount() const {
    return bookmarks_->count();
}

void DocumentationBrowserPlugin::setContentText(const QString& text) {
    contentViewer_->setPlainText(text);
}

QString DocumentationBrowserPlugin::contentText() const {
    return contentViewer_->toPlainText();
}

void DocumentationBrowserPlugin::setSearchQuery(const QString& query) {
    searchBox_->setText(query);
}

QString DocumentationBrowserPlugin::searchQuery() const {
    return searchBox_->text();
}

bool DocumentationBrowserPlugin::exportDocumentation(const QString& filePath, const QString& format) {
    if (filePath.isEmpty())
        return false;

    QJsonObject root;
    root["version"] = 1;
    root["format"] = format;
    root["content"] = contentViewer_->toPlainText();
    QJsonArray bm;
    for (int i = 0; i < bookmarks_->count(); ++i) {
        bm.append(bookmarks_->item(i)->text());
    }
    root["bookmarks"] = bm;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    const QByteArray bytes = QJsonDocument(root).toJson();
    return file.write(bytes) == bytes.size() && file.flush();
}
