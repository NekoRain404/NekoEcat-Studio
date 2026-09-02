#pragma once

// SoEPlugin — Servo over EtherCAT (SoE) workspace plugin.
//
// Provides a UI for reading and writing servo drive IDN parameters via the
// SoE mailbox protocol (IEC 61800-7-304). Supports both string-form IDNs
// (S-x-yyyy / P-x-yyyy) and numeric IDNs, with selectable data types.

#include "EthercatTypes.h"
#include "plugins/WorkspacePlugin.h"

class EcatClient;
class EventBus;
class QTableWidget;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QPushButton;
class QLabel;
class QPlainTextEdit;

class SoEPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit SoEPlugin(EcatClient* client, EventBus* eventBus, QObject* parent = nullptr);

    QString id() const override;            // "soe"
    QString displayName() const override;   // "SoE Drive"
    QString displayNameZh() const override; // "伺服驱动"
    QWidget* widget() override;
    int defaultOrder() const override; // 156
    bool visible() const override;

private slots:
    void onTopologyChanged(const QVector<SlaveInfo>& slaves);
    void onReadResult(int position, const QString& idn, const QString& value);
    void onWriteResult(int position, const QString& idn);
    void doRead();
    void doWrite();

private:
    void buildUi();
    int selectedPosition() const;

    EcatClient* client_;
    EventBus* eventBus_;
    QWidget* container_ = nullptr;

    QComboBox* slaveCombo_ = nullptr;
    QSpinBox* driveSpin_ = nullptr;
    QLineEdit* idnEdit_ = nullptr;
    QComboBox* typeCombo_ = nullptr;
    QLineEdit* valueEdit_ = nullptr;
    QPushButton* readBtn_ = nullptr;
    QPushButton* writeBtn_ = nullptr;
    QPlainTextEdit* log_ = nullptr;
};
