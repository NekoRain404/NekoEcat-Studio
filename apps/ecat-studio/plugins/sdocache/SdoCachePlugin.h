#pragma once

// SdoCachePlugin — workspace plugin for SDO cache monitoring and management.

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QComboBox;
class QSpinBox;
class SdoCacheService;

class SdoCachePlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit SdoCachePlugin(SdoCacheService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

private:
    void buildUi();
    void updateStats();
    void updateEntries();

    SdoCacheService* service_;
    QWidget* containerWidget_ = nullptr;
    QLabel* hitsLabel_ = nullptr;
    QLabel* missesLabel_ = nullptr;
    QLabel* hitRateLabel_ = nullptr;
    QLabel* sizeLabel_ = nullptr;
    QTableWidget* entriesTable_ = nullptr;
    QPushButton* clearBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QPushButton* importBtn_ = nullptr;
    QSpinBox* maxSizeSpin_ = nullptr;
    QSpinBox* ttlSpin_ = nullptr;
    QComboBox* evictionCombo_ = nullptr;
};
