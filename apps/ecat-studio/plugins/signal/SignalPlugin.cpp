// SignalPlugin — implementation.  See header for interface documentation.
#include "SignalPlugin.h"
#include "services/SignalService.h"
#include "SignalChartWidget.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

SignalPlugin::SignalPlugin(SignalService* service, QObject* parent) : service_(service) {
    if (parent)
        setParent(parent);
    buildUi();

    // When channel data changes, refresh the chart.
    connect(service_, &SignalService::channelDataUpdated, this, &SignalPlugin::refreshChart);
    connect(service_, &SignalService::channelDataUpdated, this, &SignalPlugin::updateStatsOverlay);
    connect(service_, &SignalService::channelAdded, this, [this](int) { refreshChart(); });
    connect(service_, &SignalService::channelRemoved, this, [this](int) { refreshChart(); });
}

// ── Identity ──────────────────────────────────────────────────────────
QString SignalPlugin::id() const {
    return "signal";
}
QString SignalPlugin::displayName() const {
    return "Signal Analyzer";
}
QString SignalPlugin::displayNameZh() const {
    return QStringLiteral("信号分析");
}
int SignalPlugin::defaultOrder() const {
    return 70;
}
bool SignalPlugin::visible() const {
    return true;
}

QWidget* SignalPlugin::widget() {
    return container_;
}

// ── UI construction ───────────────────────────────────────────────────
void SignalPlugin::buildUi() {
    container_ = new QWidget;
    auto* root = new QHBoxLayout(container_);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Left panel: channel list + controls ──
    auto* leftPanel = new QWidget;
    leftPanel->setFixedWidth(200);
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(4, 4, 4, 4);

    auto* channelLabel = new QLabel(tr("Channels"));
    leftLayout->addWidget(channelLabel);

    channelList_ = new QListWidget;
    leftLayout->addWidget(channelList_);

    auto* btnRow = new QWidget;
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 0, 0, 0);

    auto* addBtn = new QPushButton(tr("+"));
    addBtn->setToolTip(tr("Add channel"));
    auto* removeBtn = new QPushButton(tr("-"));
    removeBtn->setToolTip(tr("Remove selected channel"));
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(removeBtn);

    leftLayout->addWidget(btnRow);

    // Window size selector.
    auto* winLabel = new QLabel(tr("Window"));
    leftLayout->addWidget(winLabel);

    windowSizeCombo_ = new QComboBox;
    windowSizeCombo_->addItems({"100", "500", "1000", "5000", "10000"});
    windowSizeCombo_->setCurrentIndex(1); // default 500
    leftLayout->addWidget(windowSizeCombo_);

    leftLayout->addStretch();

    root->addWidget(leftPanel);

    // ── Center: chart + stats ──
    auto* centerPanel = new QWidget;
    auto* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);

    chart_ = new SignalChartWidget;
    centerLayout->addWidget(chart_, 1);

    // Stats bar.
    statsLabel_ = new QLabel;
    statsLabel_->setFixedHeight(24);
    statsLabel_->setObjectName("signalStatsLabel");
    centerLayout->addWidget(statsLabel_);

    root->addWidget(centerPanel, 1);

    // ── Signal connections ──
    connect(addBtn, &QPushButton::clicked, this, &SignalPlugin::showAddChannelDialog);
    connect(removeBtn, &QPushButton::clicked, this, &SignalPlugin::removeSelectedChannel);
    connect(windowSizeCombo_, &QComboBox::currentIndexChanged, this, [this](int idx) {
        const int points = windowSizeCombo_->itemText(idx).toInt();
        chart_->setVisiblePoints(points);
        refreshChart();
    });
}

// ── Channel management UI ─────────────────────────────────────────────

void SignalPlugin::showAddChannelDialog() {
    QDialog dlg(container_);
    dlg.setWindowTitle(tr("Add Signal Channel"));

    auto* form = new QFormLayout(&dlg);

    auto* nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText(tr("e.g. actual_position"));
    form->addRow(tr("Name"), nameEdit);

    auto* slaveSpin = new QSpinBox;
    slaveSpin->setRange(0, 255);
    form->addRow(tr("Slave"), slaveSpin);

    auto* idxEdit = new QLineEdit;
    idxEdit->setPlaceholderText(tr("e.g. 0x6064"));
    form->addRow(tr("Index"), idxEdit);

    auto* subEdit = new QLineEdit;
    subEdit->setText("0");
    form->addRow(tr("SubIndex"), subEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        const QString name = nameEdit->text().trimmed();
        if (name.isEmpty())
            return;
        const int id =
            service_->addChannel(name, slaveSpin->value(), idxEdit->text().trimmed(), subEdit->text().trimmed());
        channelList_->addItem(QStringLiteral("%1 [%2]").arg(name).arg(id));
        refreshChart();
    }
}

void SignalPlugin::removeSelectedChannel() {
    const int row = channelList_->currentRow();
    if (row < 0)
        return;

    const auto chs = service_->channels();
    if (row >= chs.size())
        return;

    service_->removeChannel(chs[row].id);
    delete channelList_->takeItem(row);
    refreshChart();
}

// ── Chart refresh ─────────────────────────────────────────────────────

void SignalPlugin::refreshChart() {
    const auto chs = service_->channels();
    QVector<SignalChartWidget::ChannelData> chartData;
    chartData.reserve(chs.size());

    for (int i = 0; i < chs.size(); ++i) {
        SignalChartWidget::ChannelData cd;
        cd.name = chs[i].name;
        cd.color = SignalChartWidget::kColors[i % SignalChartWidget::kColorCount];
        cd.values = chs[i].values;
        chartData.append(cd);
    }
    chart_->setChannelData(chartData);
}

// ── Stats overlay ─────────────────────────────────────────────────────

void SignalPlugin::updateStatsOverlay(int channelId) {
    Q_UNUSED(channelId);
    // Build stats string for all channels.
    const auto chs = service_->channels();
    QStringList parts;
    for (const auto& ch : chs) {
        const ChannelStats s = service_->stats(ch.id);
        if (ch.values.isEmpty())
            continue;
        parts.append(QStringLiteral("%1: min=%2 max=%3 avg=%4 σ=%5")
                         .arg(ch.name)
                         .arg(s.min, 0, 'g', 4)
                         .arg(s.max, 0, 'g', 4)
                         .arg(s.avg, 0, 'g', 4)
                         .arg(s.stddev, 0, 'g', 4));
    }
    statsLabel_->setText(parts.join(QStringLiteral("  |  ")));
}
