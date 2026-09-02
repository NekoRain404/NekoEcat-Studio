#include "CalibrationPlugin.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

CalibrationPlugin::CalibrationPlugin(QObject* parent) {
    if (parent)
        setParent(parent);
    buildUi();
}

QString CalibrationPlugin::id() const {
    return "calibration";
}
QString CalibrationPlugin::displayName() const {
    return "Calibration";
}
QString CalibrationPlugin::displayNameZh() const {
    return QStringLiteral("校准");
}
QIcon CalibrationPlugin::icon() const {
    return QIcon::fromTheme("preferences-system");
}
int CalibrationPlugin::defaultOrder() const {
    return 210;
}
bool CalibrationPlugin::visible() const {
    return false;
}

void CalibrationPlugin::activate() {}
void CalibrationPlugin::deactivate() {}

QWidget* CalibrationPlugin::widget() {
    return containerWidget_;
}

CalibrationPlugin::WizardStep CalibrationPlugin::currentStep() const {
    return currentStep_;
}
CalibrationPlugin::CalibrationType CalibrationPlugin::calibrationType() const {
    return calType_;
}
void CalibrationPlugin::setCalibrationType(CalibrationType type) {
    calType_ = type;
    typeCombo_->setCurrentIndex(static_cast<int>(type));
}

bool CalibrationPlugin::isCalibrating() const {
    return calibrating_;
}
int CalibrationPlugin::collectedSamples() const {
    return collectedSamples_;
}
int CalibrationPlugin::requiredSamples() const {
    return requiredSamples_;
}
void CalibrationPlugin::setRequiredSamples(int count) {
    requiredSamples_ = count;
    sampleCountSpin_->setValue(count);
}

double CalibrationPlugin::offsetResult() const {
    return offsetResult_;
}
double CalibrationPlugin::gainResult() const {
    return gainResult_;
}
double CalibrationPlugin::linearityError() const {
    return linearityError_;
}

QTableWidget* CalibrationPlugin::dataTable() const {
    return dataTable_;
}
QTableWidget* CalibrationPlugin::historyTable() const {
    return historyTable_;
}
QTextEdit* CalibrationPlugin::resultsView() const {
    return resultsView_;
}

void CalibrationPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* mainLayout = new QVBoxLayout(containerWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* toolbar = new QWidget;
    auto* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(4, 2, 4, 2);

    prevBtn_ = new QPushButton(tr("Previous"));
    nextBtn_ = new QPushButton(tr("Next"));
    startBtn_ = new QPushButton(tr("Start Calibration"));
    stopBtn_ = new QPushButton(tr("Stop"));
    collectBtn_ = new QPushButton(tr("Collect Sample"));
    analyzeBtn_ = new QPushButton(tr("Analyze"));
    resetBtn_ = new QPushButton(tr("Reset"));
    exportDataBtn_ = new QPushButton(tr("Export Data"));
    exportHistoryBtn_ = new QPushButton(tr("Export History"));

    stepLabel_ = new QLabel(tr("Step 1/5: Select Device"));
    stepLabel_->setStyleSheet("font-weight: bold; padding: 0 8px;");
    progressLabel_ = new QLabel;
    progressLabel_->setStyleSheet("padding: 0 8px;");

    toolbarLayout->addWidget(prevBtn_);
    toolbarLayout->addWidget(nextBtn_);
    toolbarLayout->addWidget(startBtn_);
    toolbarLayout->addWidget(stopBtn_);
    toolbarLayout->addWidget(collectBtn_);
    toolbarLayout->addWidget(analyzeBtn_);
    toolbarLayout->addWidget(resetBtn_);
    toolbarLayout->addWidget(exportDataBtn_);
    toolbarLayout->addWidget(exportHistoryBtn_);
    toolbarLayout->addWidget(stepLabel_);
    toolbarLayout->addWidget(progressLabel_);
    toolbarLayout->addStretch();
    mainLayout->addWidget(toolbar);

    stack_ = new QStackedWidget;
    stack_->addWidget(buildSelectPage());
    stack_->addWidget(buildConfigurePage());
    stack_->addWidget(buildCollectingPage());
    stack_->addWidget(buildAnalyzePage());
    stack_->addWidget(buildResultsPage());
    mainLayout->addWidget(stack_);

    updateButtons();

    connect(prevBtn_, &QPushButton::clicked, this, &CalibrationPlugin::prevStep);
    connect(nextBtn_, &QPushButton::clicked, this, &CalibrationPlugin::nextStep);
    connect(startBtn_, &QPushButton::clicked, this, &CalibrationPlugin::startCalibration);
    connect(stopBtn_, &QPushButton::clicked, this, &CalibrationPlugin::stopCalibration);
    connect(collectBtn_, &QPushButton::clicked, this, &CalibrationPlugin::collectSample);
    connect(analyzeBtn_, &QPushButton::clicked, this, &CalibrationPlugin::analyzeResults);
    connect(resetBtn_, &QPushButton::clicked, this, &CalibrationPlugin::resetWizard);
    connect(exportDataBtn_, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Calibration Data"),
                                                    "calibration_data.csv", tr("CSV Files (*.csv)"));
        if (!path.isEmpty())
            exportCalibrationData(path);
    });
    connect(exportHistoryBtn_, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getSaveFileName(containerWidget_, tr("Export Calibration History"),
                                                    "calibration_history.csv", tr("CSV Files (*.csv)"));
        if (!path.isEmpty())
            exportHistory(path);
    });
}

QWidget* CalibrationPlugin::buildSelectPage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 8, 8, 8);

    layout->addWidget(new QLabel(tr("Select Device for Calibration")));
    deviceCombo_ = new QComboBox;
    deviceCombo_->addItems(
        {tr("Slave 0 (Auto-detect)"), tr("Slave 1"), tr("Slave 2"), tr("Slave 3"), tr("Custom Address...")});
    layout->addWidget(deviceCombo_);

    layout->addWidget(new QLabel(tr("Calibration Type")));
    typeCombo_ = new QComboBox;
    typeCombo_->addItems({tr("Offset"), tr("Gain"), tr("Linearity"), tr("Full Calibration")});
    typeCombo_->setCurrentIndex(3);
    layout->addWidget(typeCombo_);

    layout->addStretch();
    return page;
}

QWidget* CalibrationPlugin::buildConfigurePage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 8, 8, 8);

    layout->addWidget(new QLabel(tr("Configure Calibration Parameters")));

    auto* sampleRow = new QHBoxLayout;
    sampleRow->addWidget(new QLabel(tr("Required Samples:")));
    sampleCountSpin_ = new QSpinBox;
    sampleCountSpin_->setRange(1, 1000);
    sampleCountSpin_->setValue(50);
    sampleRow->addWidget(sampleCountSpin_);
    layout->addLayout(sampleRow);

    settingsTable_ = new QTableWidget;
    settingsTable_->setColumnCount(2);
    settingsTable_->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    settingsTable_->horizontalHeader()->setStretchLastSection(true);
    settingsTable_->setRowCount(4);
    settingsTable_->setItem(0, 0, new QTableWidgetItem(tr("Min Value")));
    settingsTable_->setItem(0, 1, new QTableWidgetItem("0"));
    settingsTable_->setItem(1, 0, new QTableWidgetItem(tr("Max Value")));
    settingsTable_->setItem(1, 1, new QTableWidgetItem("65535"));
    settingsTable_->setItem(2, 0, new QTableWidgetItem(tr("Tolerance (%)")));
    settingsTable_->setItem(2, 1, new QTableWidgetItem("1.0"));
    settingsTable_->setItem(3, 0, new QTableWidgetItem(tr("Auto-increment")));
    settingsTable_->setItem(3, 1, new QTableWidgetItem("Yes"));
    layout->addWidget(settingsTable_);

    layout->addStretch();
    return page;
}

QWidget* CalibrationPlugin::buildCollectingPage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 8, 8, 8);

    layout->addWidget(new QLabel(tr("Collecting Calibration Data")));

    dataTable_ = new QTableWidget;
    dataTable_->setColumnCount(4);
    dataTable_->setHorizontalHeaderLabels({tr("Sample #"), tr("Raw Value"), tr("Reference Value"), tr("Error (%)")});
    dataTable_->horizontalHeader()->setStretchLastSection(true);
    dataTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dataTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(dataTable_);

    return page;
}

QWidget* CalibrationPlugin::buildAnalyzePage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 8, 8, 8);

    layout->addWidget(new QLabel(tr("Calibration Analysis")));
    layout->addWidget(new QLabel(tr("Click 'Analyze' to process collected data.")));

    resultsView_ = new QTextEdit;
    resultsView_->setReadOnly(true);
    layout->addWidget(resultsView_);

    return page;
}

QWidget* CalibrationPlugin::buildResultsPage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 8, 8, 8);

    layout->addWidget(new QLabel(tr("Calibration Results")));

    auto* resultsTable = new QTableWidget;
    resultsTable->setColumnCount(2);
    resultsTable->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    resultsTable->horizontalHeader()->setStretchLastSection(true);
    resultsTable->setRowCount(4);
    resultsTable->setItem(0, 0, new QTableWidgetItem(tr("Offset")));
    resultsTable->setItem(0, 1, new QTableWidgetItem("0.0"));
    resultsTable->setItem(1, 0, new QTableWidgetItem(tr("Gain")));
    resultsTable->setItem(1, 1, new QTableWidgetItem("1.0"));
    resultsTable->setItem(2, 0, new QTableWidgetItem(tr("Linearity Error (%)")));
    resultsTable->setItem(2, 1, new QTableWidgetItem("0.0"));
    resultsTable->setItem(3, 0, new QTableWidgetItem(tr("Status")));
    resultsTable->setItem(3, 1, new QTableWidgetItem(tr("Not Calibrated")));
    resultsTable->setObjectName("resultsTable");
    layout->addWidget(resultsTable);

    layout->addWidget(new QLabel(tr("Calibration History")));
    historyTable_ = new QTableWidget;
    historyTable_->setColumnCount(6);
    historyTable_->setHorizontalHeaderLabels(
        {tr("Time"), tr("Device"), tr("Type"), tr("Offset"), tr("Gain"), tr("Linearity (%)")});
    historyTable_->horizontalHeader()->setStretchLastSection(true);
    historyTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(historyTable_);

    return page;
}

void CalibrationPlugin::nextStep() {
    int s = static_cast<int>(currentStep_);
    if (s < static_cast<int>(WizardStep::Complete)) {
        currentStep_ = static_cast<WizardStep>(s + 1);
        stack_->setCurrentIndex(s + 1);
        updateStepDisplay();
        updateButtons();
        emit stepChanged(currentStep_);
    }
}

void CalibrationPlugin::prevStep() {
    int s = static_cast<int>(currentStep_);
    if (s > 0) {
        currentStep_ = static_cast<WizardStep>(s - 1);
        stack_->setCurrentIndex(s - 1);
        updateStepDisplay();
        updateButtons();
        emit stepChanged(currentStep_);
    }
}

void CalibrationPlugin::startCalibration() {
    calibrating_ = true;
    collectedSamples_ = 0;
    sampleValues_.clear();
    requiredSamples_ = sampleCountSpin_->value();
    dataTable_->setRowCount(0);
    currentStep_ = WizardStep::Collecting;
    stack_->setCurrentIndex(2);
    updateStepDisplay();
    updateButtons();
    emit stepChanged(currentStep_);
}

void CalibrationPlugin::stopCalibration() {
    calibrating_ = false;
    updateButtons();
}

void CalibrationPlugin::collectSample() {
    if (!calibrating_ || collectedSamples_ >= requiredSamples_)
        return;

    progressLabel_->setText(tr("Calibration sample collection requires a live device backend"));
}

void CalibrationPlugin::analyzeResults() {
    if (sampleValues_.isEmpty())
        return;

    double sum = 0.0;
    for (double v : sampleValues_)
        sum += v;
    double mean = sum / sampleValues_.size();

    offsetResult_ = mean - 1000.0;
    gainResult_ = 1.0 + (mean - 1000.0) / 10000.0;

    double maxErr = 0.0;
    for (int i = 0; i < sampleValues_.size(); ++i) {
        double expected = 1000.0 + i * 10.0;
        double err = qAbs(sampleValues_[i] - expected) / expected * 100.0;
        if (err > maxErr)
            maxErr = err;
    }
    linearityError_ = maxErr;

    resultsView_->clear();
    resultsView_->append(tr("=== Calibration Analysis ==="));
    resultsView_->append(tr("Samples: %1").arg(sampleValues_.size()));
    resultsView_->append(tr("Mean Value: %1").arg(mean, 0, 'f', 4));
    resultsView_->append(tr("Offset: %1").arg(offsetResult_, 0, 'f', 4));
    resultsView_->append(tr("Gain Factor: %1").arg(gainResult_, 0, 'f', 6));
    resultsView_->append(tr("Max Linearity Error: %1%").arg(linearityError_, 0, 'f', 3));
    resultsView_->append(tr("Status: %1").arg(linearityError_ < 2.0 ? tr("PASS") : tr("FAIL")));

    auto* rt = stack_->widget(4)->findChild<QTableWidget*>("resultsTable");
    if (rt) {
        rt->item(0, 1)->setText(QString::number(offsetResult_, 'f', 4));
        rt->item(1, 1)->setText(QString::number(gainResult_, 'f', 6));
        rt->item(2, 1)->setText(QString::number(linearityError_, 'f', 3));
        rt->item(3, 1)->setText(linearityError_ < 2.0 ? tr("PASS") : tr("FAIL"));
    }

    nextStep();
}

void CalibrationPlugin::resetWizard() {
    currentStep_ = WizardStep::SelectDevice;
    calibrating_ = false;
    collectedSamples_ = 0;
    sampleValues_.clear();
    offsetResult_ = 0.0;
    gainResult_ = 1.0;
    linearityError_ = 0.0;
    dataTable_->setRowCount(0);
    if (resultsView_)
        resultsView_->clear();
    progressLabel_->clear();
    stack_->setCurrentIndex(0);
    updateStepDisplay();
    updateButtons();
    emit stepChanged(currentStep_);
}

void CalibrationPlugin::addHistoryEntry(const QString& device, CalibrationType type, double offset, double gain,
                                        double linearity) {
    int row = historyTable_->rowCount();
    historyTable_->insertRow(row);
    historyTable_->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString(Qt::ISODate)));
    historyTable_->setItem(row, 1, new QTableWidgetItem(device));
    QString typeStr;
    switch (type) {
        case CalibrationType::Offset:
            typeStr = "Offset";
            break;
        case CalibrationType::Gain:
            typeStr = "Gain";
            break;
        case CalibrationType::Linearity:
            typeStr = "Linearity";
            break;
        case CalibrationType::Full:
            typeStr = "Full";
            break;
    }
    historyTable_->setItem(row, 2, new QTableWidgetItem(typeStr));
    historyTable_->setItem(row, 3, new QTableWidgetItem(QString::number(offset, 'f', 4)));
    historyTable_->setItem(row, 4, new QTableWidgetItem(QString::number(gain, 'f', 6)));
    historyTable_->setItem(row, 5, new QTableWidgetItem(QString::number(linearity, 'f', 3)));
}

int CalibrationPlugin::historyCount() const {
    return historyTable_->rowCount();
}

void CalibrationPlugin::updateStepDisplay() {
    static const char* stepNames[] = {"Select Device", "Configure", "Collecting", "Analyzing", "Results", "Complete"};
    int s = static_cast<int>(currentStep_);
    stepLabel_->setText(tr("Step %1/5: %2").arg(s + 1).arg(tr(stepNames[s])));
}

void CalibrationPlugin::updateButtons() {
    int s = static_cast<int>(currentStep_);
    prevBtn_->setEnabled(s > 0 && !calibrating_);
    nextBtn_->setEnabled(s < static_cast<int>(WizardStep::Results) && !calibrating_);
    startBtn_->setVisible(s == static_cast<int>(WizardStep::Configure));
    stopBtn_->setVisible(calibrating_);
    collectBtn_->setVisible(s == static_cast<int>(WizardStep::Collecting));
    collectBtn_->setEnabled(calibrating_ && collectedSamples_ < requiredSamples_);
    analyzeBtn_->setVisible(s == static_cast<int>(WizardStep::Analyzing));
    resetBtn_->setVisible(s >= static_cast<int>(WizardStep::Results));
}

bool CalibrationPlugin::exportCalibrationData(const QString& path) {
    if (path.isEmpty())
        return false;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    out << "Sample,Raw Value,Reference Value,Error (%)\n";
    for (int r = 0; r < dataTable_->rowCount(); ++r) {
        QStringList cols;
        for (int c = 0; c < 4; ++c) {
            auto* item = dataTable_->item(r, c);
            cols << (item ? item->text() : "");
        }
        out << cols.join(",") << "\n";
    }
    return out.status() == QTextStream::Ok && file.flush();
}

bool CalibrationPlugin::exportHistory(const QString& path) {
    if (path.isEmpty())
        return false;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    out << "Time,Device,Type,Offset,Gain,Linearity (%)\n";
    for (int r = 0; r < historyTable_->rowCount(); ++r) {
        QStringList cols;
        for (int c = 0; c < 6; ++c) {
            auto* item = historyTable_->item(r, c);
            cols << (item ? item->text() : "");
        }
        out << cols.join(",") << "\n";
    }
    return out.status() == QTextStream::Ok && file.flush();
}
