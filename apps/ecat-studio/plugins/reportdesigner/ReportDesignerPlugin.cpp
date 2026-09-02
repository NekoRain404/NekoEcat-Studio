#include "ReportDesignerPlugin.h"

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

ReportDesignerPlugin::ReportDesignerPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString ReportDesignerPlugin::id() const {
    return "reportdesigner";
}
QString ReportDesignerPlugin::displayName() const {
    return "Report Designer";
}
QString ReportDesignerPlugin::displayNameZh() const {
    return QStringLiteral("报告设计器");
}
QIcon ReportDesignerPlugin::icon() const {
    return QIcon::fromTheme("x-office-document");
}
int ReportDesignerPlugin::defaultOrder() const {
    return 345;
}
bool ReportDesignerPlugin::visible() const {
    return false;
}

void ReportDesignerPlugin::activate() {}
void ReportDesignerPlugin::deactivate() {}

QWidget* ReportDesignerPlugin::widget() {
    return containerWidget_;
}
QWidget* ReportDesignerPlugin::layoutEditor() const {
    return layoutEditor_;
}
QListWidget* ReportDesignerPlugin::templates() const {
    return templates_;
}
QTreeWidget* ReportDesignerPlugin::dataBindings() const {
    return dataBindings_;
}
QTextEdit* ReportDesignerPlugin::previewPane() const {
    return previewPane_;
}

void ReportDesignerPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QHBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter;

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    auto* tmplLabel = new QLabel(tr("Report Templates"));
    leftLayout->addWidget(tmplLabel);

    templates_ = new QListWidget;
    templates_->setDragEnabled(true);
    templates_->addItem(tr("Blank Report"));
    templates_->addItem(tr("Commissioning Summary"));
    templates_->addItem(tr("Diagnostics Report"));
    templates_->addItem(tr("Network Topology"));
    templates_->addItem(tr("Performance Analysis"));
    leftLayout->addWidget(templates_);

    auto* tmplButtonRow = new QHBoxLayout;
    auto* addTmplBtn = new QPushButton(tr("Add Template"));
    tmplButtonRow->addWidget(addTmplBtn);
    auto* removeTmplBtn = new QPushButton(tr("Remove"));
    tmplButtonRow->addWidget(removeTmplBtn);
    tmplButtonRow->addStretch();
    leftLayout->addLayout(tmplButtonRow);

    splitter->addWidget(leftPanel);

    auto* centerPanel = new QWidget;
    auto* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(4, 4, 4, 4);

    auto* layoutLabel = new QLabel(tr("Layout Editor"));
    centerLayout->addWidget(layoutLabel);

    layoutEditor_ = new QWidget;
    layoutEditor_->setMinimumSize(400, 300);
    layoutEditor_->setStyleSheet("background-color: white; border: 1px solid #ccc;");
    centerLayout->addWidget(layoutEditor_, 1);

    statusLabel_ = new QLabel(tr("Ready"));
    centerLayout->addWidget(statusLabel_);

    splitter->addWidget(centerPanel);

    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(4, 4, 4, 4);

    auto* bindLabel = new QLabel(tr("Data Bindings"));
    rightLayout->addWidget(bindLabel);

    dataBindings_ = new QTreeWidget;
    dataBindings_->setHeaderLabel(tr("Fields"));
    auto* slaveCategory = new QTreeWidgetItem(dataBindings_, {tr("Slave Data")});
    new QTreeWidgetItem(slaveCategory, {tr("Slave Name")});
    new QTreeWidgetItem(slaveCategory, {tr("Slave State")});
    new QTreeWidgetItem(slaveCategory, {tr("Vendor ID")});
    auto* networkCategory = new QTreeWidgetItem(dataBindings_, {tr("Network")});
    new QTreeWidgetItem(networkCategory, {tr("Topology")});
    new QTreeWidgetItem(networkCategory, {tr("Bus Load")});
    auto* diagCategory = new QTreeWidgetItem(dataBindings_, {tr("Diagnostics")});
    new QTreeWidgetItem(diagCategory, {tr("AL Status Code")});
    new QTreeWidgetItem(diagCategory, {tr("Error Count")});
    dataBindings_->expandAll();
    rightLayout->addWidget(dataBindings_);

    auto* bindButtonRow = new QHBoxLayout;
    auto* addBindBtn = new QPushButton(tr("Add Field"));
    bindButtonRow->addWidget(addBindBtn);
    auto* removeBindBtn = new QPushButton(tr("Remove"));
    bindButtonRow->addWidget(removeBindBtn);
    bindButtonRow->addStretch();
    rightLayout->addLayout(bindButtonRow);

    auto* previewLabel = new QLabel(tr("Preview"));
    rightLayout->addWidget(previewLabel);

    previewPane_ = new QTextEdit;
    previewPane_->setReadOnly(true);
    previewPane_->setPlaceholderText(tr("Report preview will appear here..."));
    previewPane_->setMaximumHeight(200);
    rightLayout->addWidget(previewPane_);

    auto* exportRow = new QHBoxLayout;
    exportFormat_ = new QComboBox;
    exportFormat_->addItem("PDF");
    exportFormat_->addItem("HTML");
    exportFormat_->addItem("Markdown");
    exportRow->addWidget(exportFormat_);
    exportButton_ = new QPushButton(tr("Export"));
    exportRow->addWidget(exportButton_);
    importButton_ = new QPushButton(tr("Import Template"));
    exportRow->addWidget(importButton_);
    rightLayout->addLayout(exportRow);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setStretchFactor(2, 1);

    mainLayout->addWidget(splitter);

    connect(addTmplBtn, &QPushButton::clicked, this,
            [this]() { addTemplate(tr("Custom Template %1").arg(templates_->count() + 1)); });
    connect(removeTmplBtn, &QPushButton::clicked, this, [this]() {
        auto* item = templates_->currentItem();
        if (item)
            removeTemplate(item->text());
    });
    connect(addBindBtn, &QPushButton::clicked, this,
            [this]() { addDataBinding(tr("Custom"), tr("Field %1").arg(dataBindingCount() + 1)); });
    connect(removeBindBtn, &QPushButton::clicked, this, [this]() {
        auto* item = dataBindings_->currentItem();
        if (item && !item->parent())
            return;
        if (item)
            removeDataBinding(item->text(0));
    });
    connect(exportButton_, &QPushButton::clicked, this, &ReportDesignerPlugin::exportRequested);
    connect(importButton_, &QPushButton::clicked, this, [this]() {
        QString path =
            QFileDialog::getOpenFileName(containerWidget_, tr("Import Template"), QString(), "JSON (*.json)");
        if (!path.isEmpty())
            importTemplate(path);
    });
}

void ReportDesignerPlugin::addTemplate(const QString& name) {
    templates_->addItem(name);
    statusLabel_->setText(tr("Added template: %1").arg(name));
    emit templateAdded(name);
}

void ReportDesignerPlugin::removeTemplate(const QString& name) {
    for (int i = 0; i < templates_->count(); ++i) {
        if (templates_->item(i)->text() == name) {
            delete templates_->takeItem(i);
            statusLabel_->setText(tr("Removed template: %1").arg(name));
            emit templateRemoved(name);
            return;
        }
    }
}

void ReportDesignerPlugin::clearTemplates() {
    templates_->clear();
}
int ReportDesignerPlugin::templateCount() const {
    return templates_->count();
}

void ReportDesignerPlugin::addDataBinding(const QString& category, const QString& field) {
    for (int i = 0; i < dataBindings_->topLevelItemCount(); ++i) {
        auto* cat = dataBindings_->topLevelItem(i);
        if (cat->text(0) == category) {
            new QTreeWidgetItem(cat, {field});
            emit dataBindingAdded(field);
            return;
        }
    }
    auto* cat = new QTreeWidgetItem(dataBindings_, {category});
    new QTreeWidgetItem(cat, {field});
    emit dataBindingAdded(field);
}

void ReportDesignerPlugin::removeDataBinding(const QString& field) {
    for (int i = 0; i < dataBindings_->topLevelItemCount(); ++i) {
        auto* cat = dataBindings_->topLevelItem(i);
        for (int j = 0; j < cat->childCount(); ++j) {
            if (cat->child(j)->text(0) == field) {
                delete cat->takeChild(j);
                emit dataBindingRemoved(field);
                return;
            }
        }
    }
}

void ReportDesignerPlugin::clearDataBindings() {
    dataBindings_->clear();
}
int ReportDesignerPlugin::dataBindingCount() const {
    int count = 0;
    for (int i = 0; i < dataBindings_->topLevelItemCount(); ++i) {
        count += dataBindings_->topLevelItem(i)->childCount();
    }
    return count;
}

void ReportDesignerPlugin::setPreviewText(const QString& text) {
    previewPane_->setPlainText(text);
    emit previewUpdated();
}

QString ReportDesignerPlugin::previewText() const {
    return previewPane_->toPlainText();
}

bool ReportDesignerPlugin::exportReport(const QString& filePath, const QString& format) {
    if (filePath.isEmpty())
        return false;

    QJsonObject root;
    root["version"] = 1;
    root["format"] = format;
    QJsonArray tmpls;
    for (int i = 0; i < templates_->count(); ++i) {
        tmpls.append(templates_->item(i)->text());
    }
    root["templates"] = tmpls;
    root["preview"] = previewPane_->toPlainText();

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    const QByteArray bytes = QJsonDocument(root).toJson();
    return file.write(bytes) == bytes.size() && file.flush();
}

bool ReportDesignerPlugin::importTemplate(const QString& filePath) {
    if (filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject root = doc.object();
    if (!root.value("templates").isArray())
        return false;

    QStringList importedTemplates;
    for (const auto& t : root["templates"].toArray()) {
        if (!t.isString())
            return false;
        const QString name = t.toString().trimmed();
        if (name.isEmpty())
            return false;
        importedTemplates.append(name);
    }
    if (importedTemplates.isEmpty())
        return false;

    const QString importedPreview = root.contains("preview") ? root["preview"].toString() : QString();

    templates_->clear();
    for (const auto& name : importedTemplates)
        templates_->addItem(name);

    if (root.contains("preview")) {
        previewPane_->setPlainText(importedPreview);
    }
    statusLabel_->setText(tr("Template imported"));
    return true;
}
