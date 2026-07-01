#include "HardwareVerificationPlugin.h"
#include "DeviceVerificationWidget.h"
#include "NetworkVerificationWidget.h"
#include "services/HardwareVerificationService.h"

#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

HardwareVerificationPlugin::HardwareVerificationPlugin(
    HardwareVerificationService *service, QObject *parent)
    : service_(service) {
  if (parent) setParent(parent);
  buildUi();

  connect(service_, &HardwareVerificationService::verificationCompleted, this,
          [this](const VerificationResult &result) {
            emit verificationCompleted(result.verificationName, result.passed,
                                       result.failed);
          });
}

QString HardwareVerificationPlugin::id() const {
  return QStringLiteral("hardwareverification");
}

QString HardwareVerificationPlugin::displayName() const {
  return QStringLiteral("Hardware Verification");
}

QString HardwareVerificationPlugin::displayNameZh() const {
  return QStringLiteral("硬件验证");
}

QIcon HardwareVerificationPlugin::icon() const {
  return QIcon::fromTheme("dialog-ok-apply");
}

QWidget *HardwareVerificationPlugin::widget() { return containerWidget_; }

int HardwareVerificationPlugin::defaultOrder() const { return 36; }

bool HardwareVerificationPlugin::visible() const { return false; }

void HardwareVerificationPlugin::activate() {}

void HardwareVerificationPlugin::deactivate() {}

void HardwareVerificationPlugin::onConnectionChanged(bool connected) {
  if (deviceWidget_ && !connected) deviceWidget_->clear();
  if (networkWidget_ && !connected) networkWidget_->clear();
}

HardwareVerificationService *
HardwareVerificationPlugin::verificationService() const {
  return service_;
}

void HardwareVerificationPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(0, 0, 0, 0);

  tabWidget_ = new QTabWidget;

  deviceWidget_ = new DeviceVerificationWidget(service_);
  tabWidget_->addTab(deviceWidget_, tr("Device Verification"));

  networkWidget_ = new NetworkVerificationWidget(service_);
  tabWidget_->addTab(networkWidget_, tr("Network Verification"));

  auto *timingPage = new QWidget;
  auto *timingLayout = new QVBoxLayout(timingPage);
  auto *timingControlRow = new QHBoxLayout;
  auto *timingRunButton = new QPushButton(tr("Run Timing Verification"));
  timingControlRow->addWidget(timingRunButton);
  timingControlRow->addStretch();
  timingLayout->addLayout(timingControlRow);
  timingLabel_ = new QLabel(tr("Click to run timing verification tests."));
  timingLayout->addWidget(timingLabel_);
  timingLayout->addStretch();
  tabWidget_->addTab(timingPage, tr("Timing Verification"));

  connect(timingRunButton, &QPushButton::clicked, this,
          &HardwareVerificationPlugin::updateTimingTab);

  auto *compliancePage = new QWidget;
  auto *complianceLayout = new QVBoxLayout(compliancePage);
  auto *complianceControlRow = new QHBoxLayout;
  auto *complianceRunButton =
      new QPushButton(tr("Run Compliance Verification"));
  complianceControlRow->addWidget(complianceRunButton);
  complianceControlRow->addStretch();
  complianceLayout->addLayout(complianceControlRow);
  complianceLabel_ =
      new QLabel(tr("Click to run compliance verification tests."));
  complianceLayout->addWidget(complianceLabel_);
  complianceLayout->addStretch();
  tabWidget_->addTab(compliancePage, tr("Compliance Verification"));

  connect(complianceRunButton, &QPushButton::clicked, this,
          &HardwareVerificationPlugin::updateComplianceTab);

  layout->addWidget(tabWidget_);
}

void HardwareVerificationPlugin::updateTimingTab() {
  auto result = service_->verifyTiming();
  QStringList lines;
  for (const auto &t : result.tests) {
    const QString status =
        t.skipped ? tr("SKIP") : (t.passed ? tr("PASS") : tr("FAIL"));
    lines << QStringLiteral("%1: %2 (%3 ms) - %4")
                 .arg(t.testName, status)
                 .arg(t.durationMs)
                 .arg(t.details);
  }
  lines << QStringLiteral("\n%1: %2 passed, %3 failed, %4 skipped (%5 ms)")
               .arg(tr("Summary"))
               .arg(result.passed)
               .arg(result.failed)
               .arg(result.skipped)
               .arg(result.totalDurationMs);
  timingLabel_->setText(lines.join('\n'));
  emit verificationCompleted(result.verificationName, result.passed,
                             result.failed);
}

void HardwareVerificationPlugin::updateComplianceTab() {
  auto result = service_->verifyCompliance();
  QStringList lines;
  for (const auto &t : result.tests) {
    const QString status =
        t.skipped ? tr("SKIP") : (t.passed ? tr("PASS") : tr("FAIL"));
    lines << QStringLiteral("%1: %2 (%3 ms) - %4")
                 .arg(t.testName, status)
                 .arg(t.durationMs)
                 .arg(t.details);
  }
  lines << QStringLiteral("\n%1: %2 passed, %3 failed, %4 skipped (%5 ms)")
               .arg(tr("Summary"))
               .arg(result.passed)
               .arg(result.failed)
               .arg(result.skipped)
               .arg(result.totalDurationMs);
  complianceLabel_->setText(lines.join('\n'));
  emit verificationCompleted(result.verificationName, result.passed,
                             result.failed);
}
