#include "SessionPlugin.h"
#include "infra/EcatClient.h"
#include "services/ServiceContainer.h"

#include <QColor>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

SessionPlugin::SessionPlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();

    auto* client = container_->client();
    connect(client, &EcatClient::connectionStateChanged, this, [this](ConnectionState state) {
        QStringList headers = {tr("Area"), tr("Status"), tr("Detail"), tr("Next")};
        QList<QStringList> rows;
        QString stateText;
        QColor color;
        switch (state) {
            case ConnectionState::Connected:
                stateText = tr("Connected");
                color = QColor(Qt::darkGreen);
                break;
            case ConnectionState::Connecting:
                stateText = tr("Connecting...");
                color = QColor(Qt::darkYellow);
                break;
            case ConnectionState::Disconnected:
                stateText = tr("Disconnected");
                color = QColor(Qt::red);
                break;
            case ConnectionState::Reconnecting:
                stateText = tr("Reconnecting...");
                color = QColor(Qt::darkYellow);
                break;
        }
        rows << QStringList{tr("Connection"), stateText, QString(), QString()};
        updateSessionBrief(headers, rows, {color});
    });

    connect(client, &EcatClient::connected, this, [this]() { onConnectionChanged(true); });

    connect(client, &EcatClient::disconnected, this, [this]() { onConnectionChanged(false); });
}

// ── Identity ──────────────────────────────────────────────────────────
QString SessionPlugin::id() const {
    return "session";
}
QString SessionPlugin::displayName() const {
    return "Session";
}
QString SessionPlugin::displayNameZh() const {
    return QStringLiteral("\u4f1a\u8bdd");
}
int SessionPlugin::defaultOrder() const {
    return 80;
}
bool SessionPlugin::visible() const {
    return true;
}

QIcon SessionPlugin::icon() const {
    return QIcon::fromTheme("system-users");
}

void SessionPlugin::activate() {}
void SessionPlugin::deactivate() {}
void SessionPlugin::onSettingsChanged(const AppSettings&) {}
void SessionPlugin::onConnectionChanged(bool) {}

QWidget* SessionPlugin::widget() {
    return containerWidget_;
}

QTableWidget* SessionPlugin::sessionBriefTable() const {
    return table_;
}
QPushButton* SessionPlugin::sessionBriefCopyButton() const {
    return copyButton_;
}
QLabel* SessionPlugin::sessionBriefSummaryLabel() const {
    return summaryLabel_;
}

// ── UI construction ───────────────────────────────────────────────────
void SessionPlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    table_ = new QTableWidget;
    table_->setColumnCount(4);
    table_->setHorizontalHeaderLabels({tr("Area"), tr("Status"), tr("Evidence"), tr("Next")});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(table_);

    auto* buttonRow = new QHBoxLayout;
    copyButton_ = new QPushButton(tr("Copy Row"));
    copyButton_->setEnabled(false);
    buttonRow->addWidget(copyButton_);

    summaryLabel_ = new QLabel;
    buttonRow->addWidget(summaryLabel_);
    layout->addLayout(buttonRow);

    connect(table_, &QTableWidget::cellActivated, this,
            [this](int row, int /*column*/) { emit sessionBriefRowActivated(row); });
    connect(table_, &QTableWidget::currentCellChanged, this,
            [this](int row, int /*col*/, int /*prevRow*/, int /*prevCol*/) { updateCopyButtonState(); });
    connect(copyButton_, &QPushButton::clicked, this, [this]() {
        const int row = table_->currentRow();
        if (row >= 0 && row < table_->rowCount()) {
            emit sessionBriefRowCopyRequested(row);
        }
    });
}

// ── Table population ──────────────────────────────────────────────────
void SessionPlugin::updateSessionBrief(const QStringList& headers, const QList<QStringList>& rows,
                                       const QList<QColor>& rowColors) {
    table_->setUpdatesEnabled(false);
    table_->clear();
    table_->setColumnCount(headers.size());
    table_->setHorizontalHeaderLabels(headers);
    table_->setRowCount(rows.size());

    for (int r = 0; r < rows.size(); ++r) {
        const QStringList& cols = rows[r];
        for (int c = 0; c < cols.size() && c < headers.size(); ++c) {
            table_->setItem(r, c, new QTableWidgetItem(cols[c]));
        }
        if (r < rowColors.size() && rowColors[r].isValid()) {
            for (int c = 0; c < table_->columnCount(); ++c) {
                if (auto* item = table_->item(r, c)) {
                    item->setForeground(rowColors[r]);
                }
            }
        }
    }
    table_->resizeColumnsToContents();
    table_->setUpdatesEnabled(true);

    if (summaryLabel_) {
        summaryLabel_->setText(tr("%1 rows").arg(table_->rowCount()));
    }
    updateCopyButtonState();
}

// ── Copy button state ─────────────────────────────────────────────────
void SessionPlugin::updateCopyButtonState() {
    if (!copyButton_ || !table_)
        return;
    const int row = table_->currentRow();
    const bool hasRow = row >= 0 && row < table_->rowCount();
    copyButton_->setEnabled(hasRow);
    if (!hasRow) {
        copyButton_->setText(tr("Copy Row"));
        return;
    }
    const QString area = currentRowArea();
    copyButton_->setText(area.isEmpty() ? tr("Copy Row") : tr("Copy: %1").arg(area));
}

QString SessionPlugin::currentRowArea() const {
    if (!table_)
        return {};
    const int row = table_->currentRow();
    if (row < 0 || row >= table_->rowCount())
        return {};
    if (auto* item = table_->item(row, 0))
        return item->text().trimmed();
    return {};
}
