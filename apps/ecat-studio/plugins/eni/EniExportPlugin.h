#pragma once

// EniExportPlugin — generates and exports EtherCAT Network Information (ENI) XML.
//
// Builds an ENI document from the current scanned topology using EniGenerator
// (in ecat_core). Lets the user set master name and cycle time, preview the
// generated XML, and save it to disk. The resulting file can be imported into
// TwinCAT and other EtherCAT masters.

#include "plugins/WorkspacePlugin.h"
#include "EthercatTypes.h"

class EcatClient;
class EventBus;
class QLineEdit;
class QSpinBox;
class QPlainTextEdit;
class QPushButton;
class QLabel;

class EniExportPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit EniExportPlugin(EcatClient *client, EventBus *eventBus, QObject *parent = nullptr);

    QString id() const override;            // "eni"
    QString displayName() const override;   // "ENI Export"
    QString displayNameZh() const override; // "ENI 导出"
    QWidget *widget() override;
    int defaultOrder() const override;      // 157
    bool visible() const override;

    // Generate ENI XML from the given slaves (exposed for testing).
    QString generateEni(const QVector<SlaveInfo> &slaves) const;

private slots:
    void onTopologyChanged(const QVector<SlaveInfo> &slaves);
    void doGenerate();
    void doSave();

private:
    void buildUi();

    EcatClient *client_;
    EventBus *eventBus_;
    QWidget *container_ = nullptr;
    QVector<SlaveInfo> slaves_;

    QLineEdit *masterNameEdit_ = nullptr;
    QSpinBox *cycleTimeSpin_ = nullptr;
    QLabel *slaveCountLabel_ = nullptr;
    QPlainTextEdit *preview_ = nullptr;
    QPushButton *generateBtn_ = nullptr;
    QPushButton *saveBtn_ = nullptr;
};
