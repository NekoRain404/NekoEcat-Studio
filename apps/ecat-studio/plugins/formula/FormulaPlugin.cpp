#include "FormulaPlugin.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

FormulaPlugin::FormulaPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString FormulaPlugin::id() const {
    return "formula";
}
QString FormulaPlugin::displayName() const {
    return "Formula Editor";
}
QString FormulaPlugin::displayNameZh() const {
    return QStringLiteral("公式编辑器");
}
QIcon FormulaPlugin::icon() const {
    return QIcon::fromTheme("accessories-calculator");
}
int FormulaPlugin::defaultOrder() const {
    return 195;
}
bool FormulaPlugin::visible() const {
    return false;
}

void FormulaPlugin::activate() {}
void FormulaPlugin::deactivate() {}

QWidget* FormulaPlugin::widget() {
    return containerWidget_;
}
QTextEdit* FormulaPlugin::formulaEditor() const {
    return formulaEditor_;
}
QTableWidget* FormulaPlugin::variableTable() const {
    return variableTable_;
}
QLabel* FormulaPlugin::resultLabel() const {
    return resultLabel_;
}
QListWidget* FormulaPlugin::historyList() const {
    return historyList_;
}

void FormulaPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter(Qt::Horizontal);

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    auto* formulaLabel = new QLabel(tr("Formula"));
    leftLayout->addWidget(formulaLabel);

    formulaEditor_ = new QTextEdit;
    formulaEditor_->setPlaceholderText(tr("Enter formula, e.g. x * 2 + y"));
    formulaEditor_->setMaximumHeight(120);
    leftLayout->addWidget(formulaEditor_);

    auto* btnRow = new QHBoxLayout;
    evaluateBtn_ = new QPushButton(tr("Evaluate"));
    btnRow->addWidget(evaluateBtn_);
    clearBtn_ = new QPushButton(tr("Clear"));
    btnRow->addWidget(clearBtn_);
    leftLayout->addLayout(btnRow);

    auto* resultRow = new QHBoxLayout;
    auto* resultTitle = new QLabel(tr("Result:"));
    resultRow->addWidget(resultTitle);
    resultLabel_ = new QLabel;
    resultLabel_->setStyleSheet("font-weight: bold; font-size: 14px;");
    resultLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    resultRow->addWidget(resultLabel_);
    resultRow->addStretch();
    leftLayout->addLayout(resultRow);

    auto* varLabel = new QLabel(tr("Variables"));
    leftLayout->addWidget(varLabel);

    variableTable_ = new QTableWidget;
    variableTable_->setColumnCount(2);
    variableTable_->setHorizontalHeaderLabels({tr("Name"), tr("Value")});
    variableTable_->horizontalHeader()->setStretchLastSection(true);
    variableTable_->setEditTriggers(QAbstractItemView::DoubleClicked);
    leftLayout->addWidget(variableTable_);

    auto* varBtnRow = new QHBoxLayout;
    addVarBtn_ = new QPushButton(tr("Add Variable"));
    varBtnRow->addWidget(addVarBtn_);
    removeVarBtn_ = new QPushButton(tr("Remove Variable"));
    varBtnRow->addWidget(removeVarBtn_);
    leftLayout->addLayout(varBtnRow);

    splitter->addWidget(leftPanel);

    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(4, 4, 4, 4);

    auto* historyLabel = new QLabel(tr("History"));
    rightLayout->addWidget(historyLabel);

    historyList_ = new QListWidget;
    rightLayout->addWidget(historyList_);

    auto* clearHistoryBtn = new QPushButton(tr("Clear History"));
    rightLayout->addWidget(clearHistoryBtn);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    connect(evaluateBtn_, &QPushButton::clicked, this, &FormulaPlugin::evaluateRequested);
    connect(clearBtn_, &QPushButton::clicked, this, [this]() {
        formulaEditor_->clear();
        resultLabel_->clear();
    });
    connect(addVarBtn_, &QPushButton::clicked, this, [this]() {
        int row = variableTable_->rowCount();
        variableTable_->insertRow(row);
        variableTable_->setItem(row, 0, new QTableWidgetItem(tr("var%1").arg(row + 1)));
        variableTable_->setItem(row, 1, new QTableWidgetItem("0"));
    });
    connect(removeVarBtn_, &QPushButton::clicked, this, [this]() {
        int row = variableTable_->currentRow();
        if (row >= 0)
            variableTable_->removeRow(row);
    });
    connect(clearHistoryBtn, &QPushButton::clicked, this, &FormulaPlugin::clearHistory);
    connect(formulaEditor_, &QTextEdit::textChanged, this,
            [this]() { emit formulaChanged(formulaEditor_->toPlainText()); });
    connect(variableTable_, &QTableWidget::cellChanged, this, [this](int row, int col) {
        if (col == 1) {
            auto* nameItem = variableTable_->item(row, 0);
            auto* valItem = variableTable_->item(row, 1);
            if (nameItem && valItem) {
                emit variableChanged(nameItem->text(), valItem->text());
            }
        }
    });
}

void FormulaPlugin::setFormula(const QString& formula) {
    formulaEditor_->setPlainText(formula);
}

QString FormulaPlugin::formula() const {
    return formulaEditor_->toPlainText();
}

void FormulaPlugin::setResult(const QString& result) {
    resultLabel_->setText(result);
}

QString FormulaPlugin::result() const {
    return resultLabel_->text();
}

void FormulaPlugin::addVariable(const QString& name, const QString& value) {
    int row = variableTable_->rowCount();
    variableTable_->insertRow(row);
    variableTable_->setItem(row, 0, new QTableWidgetItem(name));
    variableTable_->setItem(row, 1, new QTableWidgetItem(value));
}

void FormulaPlugin::removeVariable(const QString& name) {
    for (int i = 0; i < variableTable_->rowCount(); ++i) {
        auto* item = variableTable_->item(i, 0);
        if (item && item->text() == name) {
            variableTable_->removeRow(i);
            return;
        }
    }
}

void FormulaPlugin::clearVariables() {
    variableTable_->setRowCount(0);
}

int FormulaPlugin::variableCount() const {
    return variableTable_->rowCount();
}

void FormulaPlugin::addHistoryEntry(const QString& entry) {
    historyList_->addItem(entry);
}

void FormulaPlugin::clearHistory() {
    historyList_->clear();
}

int FormulaPlugin::historyCount() const {
    return historyList_->count();
}

bool FormulaPlugin::validateFormula(const QString& formula) const {
    if (formula.trimmed().isEmpty())
        return false;
    int parenDepth = 0;
    for (const QChar& ch : formula) {
        if (ch == '(')
            ++parenDepth;
        if (ch == ')')
            --parenDepth;
        if (parenDepth < 0)
            return false;
    }
    return parenDepth == 0;
}
