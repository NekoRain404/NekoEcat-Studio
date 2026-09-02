// SoEPlugin — Servo over EtherCAT (SoE) workspace plugin implementation.

#include "SoEPlugin.h"

#include "infra/EcatClient.h"
#include "services/EventBus.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

SoEPlugin::SoEPlugin(EcatClient* client, EventBus* eventBus, QObject* parent) : client_(client), eventBus_(eventBus) {
    if (parent)
        setParent(parent);
    connect(client_, &EcatClient::soeReadResult, this, &SoEPlugin::onReadResult);
    connect(client_, &EcatClient::soeWriteResult, this, &SoEPlugin::onWriteResult);
    connect(eventBus_, &EventBus::topologyChanged, this, &SoEPlugin::onTopologyChanged);
}

QString SoEPlugin::id() const {
    return "soe";
}
QString SoEPlugin::displayName() const {
    return "SoE Drive";
}
QString SoEPlugin::displayNameZh() const {
    return QStringLiteral("伺服驱动");
}
int SoEPlugin::defaultOrder() const {
    return 156;
}
bool SoEPlugin::visible() const {
    return true;
}

QWidget* SoEPlugin::widget() {
    if (!container_)
        buildUi();
    return container_;
}

void SoEPlugin::buildUi() {
    container_ = new QWidget();
    auto* layout = new QVBoxLayout(container_);

    auto* form = new QFormLayout;

    slaveCombo_ = new QComboBox;
    form->addRow(tr("Slave:"), slaveCombo_);

    driveSpin_ = new QSpinBox;
    driveSpin_->setRange(0, 7);
    form->addRow(tr("Drive (0-7):"), driveSpin_);

    idnEdit_ = new QLineEdit;
    idnEdit_->setPlaceholderText(tr("P-0-0150 or S-0-1000 or 0x0150"));
    form->addRow(tr("IDN:"), idnEdit_);

    typeCombo_ = new QComboBox;
    typeCombo_->addItems({"", "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64", "float",
                          "double", "string", "octet_string"});
    form->addRow(tr("Type:"), typeCombo_);

    valueEdit_ = new QLineEdit;
    valueEdit_->setPlaceholderText(tr("value (for write)"));
    form->addRow(tr("Value:"), valueEdit_);

    layout->addLayout(form);

    auto* btnRow = new QHBoxLayout;
    readBtn_ = new QPushButton(tr("Read IDN"));
    writeBtn_ = new QPushButton(tr("Write IDN"));
    connect(readBtn_, &QPushButton::clicked, this, &SoEPlugin::doRead);
    connect(writeBtn_, &QPushButton::clicked, this, &SoEPlugin::doWrite);
    btnRow->addWidget(readBtn_);
    btnRow->addWidget(writeBtn_);
    btnRow->addStretch();
    layout->addLayout(btnRow);

    log_ = new QPlainTextEdit;
    log_->setReadOnly(true);
    log_->setMaximumBlockCount(500);
    layout->addWidget(log_, 1);
}

void SoEPlugin::onTopologyChanged(const QVector<SlaveInfo>& slaves) {
    if (!slaveCombo_)
        return;
    const int prev = slaveCombo_->currentData().isValid() ? slaveCombo_->currentData().toInt() : -1;
    slaveCombo_->clear();
    for (const auto& s : slaves) {
        slaveCombo_->addItem(QString("%1: %2").arg(s.position).arg(s.name), s.position);
    }
    if (prev >= 0) {
        const int idx = slaveCombo_->findData(prev);
        if (idx >= 0)
            slaveCombo_->setCurrentIndex(idx);
    }
}

int SoEPlugin::selectedPosition() const {
    if (!slaveCombo_ || slaveCombo_->currentData().isNull())
        return -1;
    return slaveCombo_->currentData().toInt();
}

void SoEPlugin::doRead() {
    const int pos = selectedPosition();
    if (pos < 0) {
        log_->appendPlainText(tr("Select a slave first."));
        return;
    }
    const QString idn = idnEdit_->text().trimmed();
    if (idn.isEmpty()) {
        log_->appendPlainText(tr("Enter an IDN."));
        return;
    }
    client_->soeRead(pos, idn, driveSpin_->value(), typeCombo_->currentText());
    log_->appendPlainText(tr("Reading IDN %1 from slave %2 (drive %3)...").arg(idn).arg(pos).arg(driveSpin_->value()));
}

void SoEPlugin::doWrite() {
    const int pos = selectedPosition();
    if (pos < 0) {
        log_->appendPlainText(tr("Select a slave first."));
        return;
    }
    const QString idn = idnEdit_->text().trimmed();
    const QString value = valueEdit_->text();
    if (idn.isEmpty() || value.isEmpty()) {
        log_->appendPlainText(tr("Enter both IDN and value."));
        return;
    }
    client_->soeWrite(pos, idn, value, driveSpin_->value(), typeCombo_->currentText());
    log_->appendPlainText(
        tr("Writing IDN %1 = %2 to slave %3 (drive %4)...").arg(idn, value).arg(pos).arg(driveSpin_->value()));
}

void SoEPlugin::onReadResult(int position, const QString& idn, const QString& value) {
    log_->appendPlainText(tr("Slave %1 IDN %2 = %3").arg(position).arg(idn, value));
    valueEdit_->setText(value);
}

void SoEPlugin::onWriteResult(int position, const QString& idn) {
    log_->appendPlainText(tr("Slave %1 IDN %2 written OK").arg(position).arg(idn));
}
