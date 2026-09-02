#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QTreeWidget;

struct TestCase {
    QString id;
    QString name;
    QString category;
    QString description;
    QString status;
    QString result;
    int durationMs;
};

class TestSuitePlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit TestSuitePlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    QTreeWidget* testList() const;
    QTableWidget* testResults() const;
    QTextEdit* testReport() const;
    QLabel* runnerStatus() const;

    void addTest(const QString& category, const QString& name, const QString& description = QString());
    void removeTest(const QString& testId);
    void clearTests();
    int testCount() const;

    void runAllTests();
    void runTest(const QString& testId);
    void stopTests();

    void updateTestResult(const QString& testId, const QString& status, const QString& result, int durationMs = 0);

    void generateReport();
    bool exportReport(const QString& filePath);

    int passedCount() const;
    int failedCount() const;
    int skippedCount() const;

signals:
    void testAdded(const QString& testId, const QString& name);
    void testRemoved(const QString& testId);
    void testStarted(const QString& testId);
    void testFinished(const QString& testId, const QString& status);
    void allTestsFinished();
    void reportGenerated();

private:
    void buildUi();
    void updateStatsLabel();

    QWidget* containerWidget_ = nullptr;
    QTreeWidget* testList_ = nullptr;
    QTableWidget* testResults_ = nullptr;
    QTextEdit* testReport_ = nullptr;
    QLabel* runnerStatus_ = nullptr;
    QLabel* statsLabel_ = nullptr;
    QPushButton* runAllButton_ = nullptr;
    QPushButton* runSelectedButton_ = nullptr;
    QPushButton* stopButton_ = nullptr;
    QPushButton* reportButton_ = nullptr;
    QPushButton* exportButton_ = nullptr;
    QLineEdit* searchInput_ = nullptr;
    QVector<TestCase> tests_;
    int nextTestId_ = 1;
    bool running_ = false;
};
