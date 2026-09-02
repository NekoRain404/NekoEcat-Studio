// EniExportPlugin — generates and exports ENI XML implementation.

#include "EniExportPlugin.h"

#include "EniGenerator.h"
#include "infra/EcatClient.h"
#include "services/EventBus.h"

#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextStream>
#include <QVBoxLayout>

EniExportPlugin::EniExportPlugin(EcatClient* client, EventBus* eventBus, QObject* parent)
    : client_(client), eventBus_(eventBus) {
    if (parent)
        setParent(parent);
    connect(eventBus_, &EventBus::topologyChanged, this, &EniExportPlugin::onTopologyChanged);
}

QString EniExportPlugin::id() const {
    return "eni";
}
QString EniExportPlugin::displayName() const {
    return "ENI Export";
}
QString EniExportPlugin::displayNameZh() const {
    return QStringLiteral("ENI 导出");
}
int EniExportPlugin::defaultOrder() const {
    return 157;
}
bool EniExportPlugin::visible() const {
    return true;
}

QWidget* EniExportPlugin::widget() {
    if (!container_)
        buildUi();
    return container_;
}

void EniExportPlugin::buildUi() {
    container_ = new QWidget();
    auto* layout = new QVBoxLayout(container_);

    auto* form = new QFormLayout;
    masterNameEdit_ = new QLineEdit("NekoEcat Master");
    form->addRow(tr("Master Name:"), masterNameEdit_);

    cycleTimeSpin_ = new QSpinBox;
    cycleTimeSpin_->setRange(100, 100000);
    cycleTimeSpin_->setValue(1000);
    cycleTimeSpin_->setSuffix(" µs");
    form->addRow(tr("Cycle Time:"), cycleTimeSpin_);

    slaveCountLabel_ = new QLabel(tr("0 slaves (scan the bus first)"));
    form->addRow(tr("Topology:"), slaveCountLabel_);
    layout->addLayout(form);

    auto* btnRow = new QHBoxLayout;
    generateBtn_ = new QPushButton(tr("Generate ENI"));
    saveBtn_ = new QPushButton(tr("Save to File..."));
    saveBtn_->setEnabled(false);
    connect(generateBtn_, &QPushButton::clicked, this, &EniExportPlugin::doGenerate);
    connect(saveBtn_, &QPushButton::clicked, this, &EniExportPlugin::doSave);
    btnRow->addWidget(generateBtn_);
    btnRow->addWidget(saveBtn_);
    btnRow->addStretch();
    layout->addLayout(btnRow);

    preview_ = new QPlainTextEdit;
    preview_->setReadOnly(true);
    preview_->setLineWrapMode(QPlainTextEdit::NoWrap);
    QFont mono("monospace");
    mono.setStyleHint(QFont::TypeWriter);
    preview_->setFont(mono);
    layout->addWidget(preview_, 1);
}

void EniExportPlugin::onTopologyChanged(const QVector<SlaveInfo>& slaves) {
    slaves_ = slaves;
    if (slaveCountLabel_) {
        slaveCountLabel_->setText(tr("%1 slaves detected").arg(slaves.size()));
    }
}

QString EniExportPlugin::generateEni(const QVector<SlaveInfo>& slaves) const {
    EniGenerator gen;
    gen.setMasterName(masterNameEdit_ ? masterNameEdit_->text() : "EtherCAT Master");
    gen.setCycleTimeUs(cycleTimeSpin_ ? cycleTimeSpin_->value() : 1000);

    for (const auto& s : slaves) {
        EniSlaveConfig cfg;
        cfg.position = s.position;
        cfg.name = s.name;
        // VendorId/ProductCode are not available from a basic scan; they remain
        // 0 unless enriched via per-slave detail queries. The ENI is still valid.
        gen.addSlave(cfg);
    }
    return gen.generate();
}

void EniExportPlugin::doGenerate() {
    if (slaves_.isEmpty()) {
        QMessageBox::information(container_, tr("No Topology"),
                                 tr("No slaves detected. Scan the bus before generating ENI."));
        return;
    }
    const QString xml = generateEni(slaves_);
    preview_->setPlainText(xml);
    saveBtn_->setEnabled(true);
}

void EniExportPlugin::doSave() {
    const QString xml = preview_->toPlainText();
    if (xml.isEmpty())
        return;

    const QString path = QFileDialog::getSaveFileName(container_, tr("Save ENI File"), "network.xml",
                                                      tr("ENI XML files (*.xml);;All files (*)"));
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(container_, tr("Save Failed"), tr("Could not write to %1").arg(path));
        return;
    }
    QTextStream out(&file);
    out << xml;
    file.close();

    QMessageBox::information(container_, tr("ENI Saved"), tr("ENI file saved to %1").arg(path));
}
