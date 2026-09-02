#pragma once

#include "plugins/WorkspacePlugin.h"

class QTabWidget;
class QLabel;
class HardwareVerificationService;
class DeviceVerificationWidget;
class NetworkVerificationWidget;

class HardwareVerificationPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit HardwareVerificationPlugin(HardwareVerificationService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;
    void onConnectionChanged(bool connected) override;

    HardwareVerificationService* verificationService() const;

signals:
    void verificationCompleted(const QString& category, int passed, int failed);

private:
    void buildUi();
    void updateTimingTab();
    void updateComplianceTab();

    HardwareVerificationService* service_ = nullptr;
    QWidget* containerWidget_ = nullptr;
    QTabWidget* tabWidget_ = nullptr;
    DeviceVerificationWidget* deviceWidget_ = nullptr;
    NetworkVerificationWidget* networkWidget_ = nullptr;
    QLabel* timingLabel_ = nullptr;
    QLabel* complianceLabel_ = nullptr;
};
