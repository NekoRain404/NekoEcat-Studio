#include "TestSuitePlugin.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

TestSuitePlugin::TestSuitePlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString TestSuitePlugin::id() const {
    return "testsuite";
}
QString TestSuitePlugin::displayName() const {
    return "Test Suite";
}
QString TestSuitePlugin::displayNameZh() const {
    return QStringLiteral("测试套件");
}
QIcon TestSuitePlugin::icon() const {
    return QIcon::fromTheme("system-run");
}
int TestSuitePlugin::defaultOrder() const {
    return 255;
}
bool TestSuitePlugin::visible() const {
    return false;
}

void TestSuitePlugin::activate() {}
void TestSuitePlugin::deactivate() {}

QWidget* TestSuitePlugin::widget() {
    return containerWidget_;
}
QTreeWidget* TestSuitePlugin::testList() const {
    return testList_;
}
QTableWidget* TestSuitePlugin::testResults() const {
    return testResults_;
}
QTextEdit* TestSuitePlugin::testReport() const {
    return testReport_;
}
QLabel* TestSuitePlugin::runnerStatus() const {
    return runnerStatus_;
}

void TestSuitePlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QHBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter;

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    auto* listLabel = new QLabel(tr("Test List"));
    leftLayout->addWidget(listLabel);

    searchInput_ = new QLineEdit;
    searchInput_->setPlaceholderText(tr("Search tests..."));
    leftLayout->addWidget(searchInput_);

    testList_ = new QTreeWidget;
    testList_->setHeaderLabel(tr("Tests"));

    auto* ethercatCategory = new QTreeWidgetItem(testList_, {tr("EtherCAT Tests")});
    new QTreeWidgetItem(ethercatCategory, {tr("Connection Test")});
    new QTreeWidgetItem(ethercatCategory, {tr("SDO Read/Write Test")});
    new QTreeWidgetItem(ethercatCategory, {tr("PDO Exchange Test")});
    new QTreeWidgetItem(ethercatCategory, {tr("DC Sync Test")});

    auto* systemCategory = new QTreeWidgetItem(testList_, {tr("System Tests")});
    new QTreeWidgetItem(systemCategory, {tr("Master Startup Test")});
    new QTreeWidgetItem(systemCategory, {tr("Slave Discovery Test")});
    new QTreeWidgetItem(systemCategory, {tr("Topology Validation Test")});

    auto* performanceCategory = new QTreeWidgetItem(testList_, {tr("Performance Tests")});
    new QTreeWidgetItem(performanceCategory, {tr("Cycle Time Test")});
    new QTreeWidgetItem(performanceCategory, {tr("Jitter Measurement Test")});
    new QTreeWidgetItem(performanceCategory, {tr("Throughput Test")});

    testList_->expandAll();
    leftLayout->addWidget(testList_);

    splitter->addWidget(leftPanel);

    auto* centerPanel = new QWidget;
    auto* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(4, 4, 4, 4);

    auto* resultsLabel = new QLabel(tr("Test Results"));
    centerLayout->addWidget(resultsLabel);

    testResults_ = new QTableWidget(0, 4);
    testResults_->setHorizontalHeaderLabels({tr("Test"), tr("Status"), tr("Result"), tr("Duration")});
    testResults_->horizontalHeader()->setStretchLastSection(true);
    centerLayout->addWidget(testResults_);

    splitter->addWidget(centerPanel);

    auto* rightPanel = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(4, 4, 4, 4);

    auto* reportLabel = new QLabel(tr("Test Report"));
    rightLayout->addWidget(reportLabel);

    testReport_ = new QTextEdit;
    testReport_->setPlaceholderText(tr("Run tests to generate report..."));
    testReport_->setReadOnly(true);
    rightLayout->addWidget(testReport_);

    runnerStatus_ = new QLabel(tr("Status: Ready"));
    rightLayout->addWidget(runnerStatus_);

    statsLabel_ = new QLabel(tr("Passed: 0 | Failed: 0 | Skipped: 0"));
    rightLayout->addWidget(statsLabel_);

    auto* buttonRow = new QHBoxLayout;
    runAllButton_ = new QPushButton(tr("Run All"));
    buttonRow->addWidget(runAllButton_);
    runSelectedButton_ = new QPushButton(tr("Run Selected"));
    buttonRow->addWidget(runSelectedButton_);
    stopButton_ = new QPushButton(tr("Stop"));
    stopButton_->setEnabled(false);
    buttonRow->addWidget(stopButton_);
    rightLayout->addLayout(buttonRow);

    auto* reportRow = new QHBoxLayout;
    reportButton_ = new QPushButton(tr("Generate Report"));
    reportRow->addWidget(reportButton_);
    exportButton_ = new QPushButton(tr("Export"));
    reportRow->addWidget(exportButton_);
    rightLayout->addLayout(reportRow);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    splitter->setStretchFactor(2, 1);

    mainLayout->addWidget(splitter);

    connect(runAllButton_, &QPushButton::clicked, this, &TestSuitePlugin::runAllTests);
    connect(stopButton_, &QPushButton::clicked, this, &TestSuitePlugin::stopTests);
    connect(reportButton_, &QPushButton::clicked, this, &TestSuitePlugin::generateReport);
    connect(exportButton_, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Report"), QString(),
                                                    "JSON (*.json);;Text (*.txt)");
        if (!path.isEmpty())
            exportReport(path);
    });

    connect(searchInput_, &QLineEdit::textChanged, this, [this](const QString& text) {
        for (int i = 0; i < testList_->topLevelItemCount(); ++i) {
            auto* category = testList_->topLevelItem(i);
            bool categoryVisible = false;
            for (int j = 0; j < category->childCount(); ++j) {
                auto* child = category->child(j);
                bool match = text.isEmpty() || child->text(0).contains(text, Qt::CaseInsensitive);
                child->setHidden(!match);
                if (match)
                    categoryVisible = true;
            }
            category->setHidden(!categoryVisible && !text.isEmpty());
        }
    });
}

void TestSuitePlugin::addTest(const QString& category, const QString& name, const QString& description) {
    TestCase test;
    test.id = QString("test_%1").arg(nextTestId_++);
    test.name = name;
    test.category = category;
    test.description = description;
    test.status = "Pending";
    test.result = "";
    test.durationMs = 0;
    tests_.append(test);

    int row = testResults_->rowCount();
    testResults_->insertRow(row);
    testResults_->setItem(row, 0, new QTableWidgetItem(name));
    testResults_->setItem(row, 1, new QTableWidgetItem(tr("Pending")));
    testResults_->setItem(row, 2, new QTableWidgetItem("-"));
    testResults_->setItem(row, 3, new QTableWidgetItem("-"));

    emit testAdded(test.id, name);
}

void TestSuitePlugin::removeTest(const QString& testId) {
    for (int i = 0; i < tests_.size(); ++i) {
        if (tests_[i].id == testId) {
            tests_.removeAt(i);
            testResults_->removeRow(i);
            emit testRemoved(testId);
            return;
        }
    }
}

void TestSuitePlugin::clearTests() {
    tests_.clear();
    testResults_->setRowCount(0);
    nextTestId_ = 1;
}

int TestSuitePlugin::testCount() const {
    return tests_.size();
}

void TestSuitePlugin::runAllTests() {
    running_ = true;
    runAllButton_->setEnabled(false);
    stopButton_->setEnabled(true);
    runnerStatus_->setText(tr("Status: Running..."));

    for (int i = 0; i < tests_.size(); ++i) {
        if (!running_)
            break;
        tests_[i].status = "Running";
        testResults_->item(i, 1)->setText(tr("Running"));
        emit testStarted(tests_[i].id);
    }

    if (running_) {
        for (int i = 0; i < tests_.size(); ++i) {
            tests_[i].status = "Skipped";
            tests_[i].result = tr("No test backend configured");
            tests_[i].durationMs = 0;
            testResults_->item(i, 1)->setText(tr("Skipped"));
            testResults_->item(i, 2)->setText(tests_[i].result);
            testResults_->item(i, 3)->setText("-");
        }
    }

    running_ = false;
    runAllButton_->setEnabled(true);
    stopButton_->setEnabled(false);
    runnerStatus_->setText(tr("Status: Finished"));
    updateStatsLabel();
    emit allTestsFinished();
}

void TestSuitePlugin::runTest(const QString& testId) {
    for (int i = 0; i < tests_.size(); ++i) {
        if (tests_[i].id == testId) {
            tests_[i].status = "Running";
            testResults_->item(i, 1)->setText(tr("Running"));
            emit testStarted(testId);

            tests_[i].status = "Blocked";
            tests_[i].result = tr("No test backend configured");
            tests_[i].durationMs = 0;
            testResults_->item(i, 1)->setText(tr("Blocked"));
            testResults_->item(i, 2)->setText(tests_[i].result);
            testResults_->item(i, 3)->setText("-");
            updateStatsLabel();
            return;
        }
    }
}

void TestSuitePlugin::stopTests() {
    running_ = false;
    runnerStatus_->setText(tr("Status: Stopped"));
}

void TestSuitePlugin::updateTestResult(const QString& testId, const QString& status, const QString& result,
                                       int durationMs) {
    for (int i = 0; i < tests_.size(); ++i) {
        if (tests_[i].id == testId) {
            tests_[i].status = status;
            tests_[i].result = result;
            tests_[i].durationMs = durationMs;
            testResults_->item(i, 1)->setText(status);
            testResults_->item(i, 2)->setText(result);
            testResults_->item(i, 3)->setText(durationMs > 0 ? QString("%1 ms").arg(durationMs) : "-");
            updateStatsLabel();
            return;
        }
    }
}

void TestSuitePlugin::generateReport() {
    QString report;
    report += tr("=== Test Suite Report ===\n\n");
    report += tr("Total Tests: %1\n").arg(tests_.size());
    report += tr("Passed: %1\n").arg(passedCount());
    report += tr("Failed: %1\n").arg(failedCount());
    report += tr("Skipped: %1\n\n").arg(skippedCount());

    for (const auto& test : tests_) {
        report += QString("[%1] %2 - %3\n").arg(test.status, test.name, test.result);
    }

    testReport_->setPlainText(report);
    emit reportGenerated();
}

bool TestSuitePlugin::exportReport(const QString& filePath) {
    if (filePath.isEmpty())
        return false;

    QJsonObject root;
    root["version"] = 1;
    root["totalTests"] = tests_.size();
    root["passed"] = passedCount();
    root["failed"] = failedCount();
    root["skipped"] = skippedCount();

    QJsonArray testsArray;
    for (const auto& test : tests_) {
        QJsonObject testObj;
        testObj["id"] = test.id;
        testObj["name"] = test.name;
        testObj["category"] = test.category;
        testObj["status"] = test.status;
        testObj["result"] = test.result;
        testObj["durationMs"] = test.durationMs;
        testsArray.append(testObj);
    }
    root["tests"] = testsArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    const QByteArray bytes = QJsonDocument(root).toJson();
    return file.write(bytes) == bytes.size() && file.flush();
}

int TestSuitePlugin::passedCount() const {
    int count = 0;
    for (const auto& t : tests_)
        if (t.status == "Passed")
            count++;
    return count;
}

int TestSuitePlugin::failedCount() const {
    int count = 0;
    for (const auto& t : tests_)
        if (t.status == "Failed")
            count++;
    return count;
}

int TestSuitePlugin::skippedCount() const {
    int count = 0;
    for (const auto& t : tests_)
        if (t.status == "Skipped")
            count++;
    return count;
}

void TestSuitePlugin::updateStatsLabel() {
    statsLabel_->setText(
        tr("Passed: %1 | Failed: %2 | Skipped: %3").arg(passedCount()).arg(failedCount()).arg(skippedCount()));
}
