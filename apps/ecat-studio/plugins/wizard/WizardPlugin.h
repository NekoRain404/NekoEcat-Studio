#pragma once

// WizardPlugin — guided wizard workspace for EtherCAT commissioning workflows.

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class WizardPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit WizardPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    struct WizardEntry {
        QString id;
        QString name;
        QString category;
        QString description;
        int stepCount;
    };

    struct WizardStep {
        QString title;
        QString instruction;
        QString tip;
    };

    struct WizardHistoryEntry {
        QString wizardId;
        QString wizardName;
        QDateTime completedAt;
        bool success;
    };

    void addWizard(const WizardEntry& entry);
    int wizardCount() const;

    void setWizardSteps(const QString& wizardId, const QVector<WizardStep>& steps);
    int currentStep() const;
    int totalSteps() const;
    bool isRunning() const;

    void startWizard(const QString& wizardId);
    void nextStep();
    void previousStep();
    void finishWizard(bool success);
    void cancelWizard();

    int historyCount() const;
    QListWidget* wizardList() const;
    QTableWidget* stepTable() const;
    QTableWidget* historyTable() const;

signals:
    void wizardStarted(const QString& wizardId);
    void wizardFinished(const QString& wizardId, bool success);
    void stepChanged(int step);

public slots:
    bool exportHistory(const QString& path);

private:
    void buildUi();
    void updateStepView();
    void rebuildHistoryTable();
    int findWizardIndex(const QString& wizardId) const;

    QWidget* containerWidget_ = nullptr;
    QListWidget* wizardList_ = nullptr;
    QTableWidget* stepTable_ = nullptr;
    QTableWidget* historyTable_ = nullptr;
    QTextEdit* instructionView_ = nullptr;
    QLabel* stepLabel_ = nullptr;
    QPushButton* startBtn_ = nullptr;
    QPushButton* nextBtn_ = nullptr;
    QPushButton* prevBtn_ = nullptr;
    QPushButton* finishBtn_ = nullptr;
    QPushButton* cancelBtn_ = nullptr;
    QTabWidget* tabs_ = nullptr;

    QVector<WizardEntry> wizards_;
    QVector<WizardStep> currentSteps_;
    QVector<WizardHistoryEntry> history_;
    QString runningWizardId_;
    int currentStep_ = 0;
    bool running_ = false;
};
