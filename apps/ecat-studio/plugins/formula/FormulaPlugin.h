#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTableWidget;
class QTextEdit;

class FormulaPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit FormulaPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    QTextEdit* formulaEditor() const;
    QTableWidget* variableTable() const;
    QLabel* resultLabel() const;
    QListWidget* historyList() const;

    void setFormula(const QString& formula);
    QString formula() const;

    void setResult(const QString& result);
    QString result() const;

    void addVariable(const QString& name, const QString& value);
    void removeVariable(const QString& name);
    void clearVariables();
    int variableCount() const;

    void addHistoryEntry(const QString& entry);
    void clearHistory();
    int historyCount() const;

    bool validateFormula(const QString& formula) const;

signals:
    void formulaChanged(const QString& formula);
    void evaluateRequested();
    void variableChanged(const QString& name, const QString& value);

private:
    void buildUi();

    QWidget* containerWidget_ = nullptr;
    QTextEdit* formulaEditor_ = nullptr;
    QTableWidget* variableTable_ = nullptr;
    QLabel* resultLabel_ = nullptr;
    QListWidget* historyList_ = nullptr;
    QPushButton* evaluateBtn_ = nullptr;
    QPushButton* clearBtn_ = nullptr;
    QPushButton* addVarBtn_ = nullptr;
    QPushButton* removeVarBtn_ = nullptr;
};
