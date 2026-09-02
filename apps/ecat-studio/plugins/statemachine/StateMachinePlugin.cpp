#include "StateMachinePlugin.h"
#include "services/ServiceContainer.h"
#include "services/StateMachineService.h"

#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

StateMachinePlugin::StateMachinePlugin(ServiceContainer* container, QObject* parent) : container_(container) {
    if (parent)
        setParent(parent);
    buildUi();

    // Live data: connect to StateMachineService for auto-updates
    auto* sm = container_->stateMachine();
    if (sm) {
        connect(sm, &StateMachineService::stateChanged, this,
                [this](int pos, int state) { updateStateCell(pos, state); });
        connect(sm, &StateMachineService::stateTransitionFailed, this,
                [this](int pos, int from, int to, const QString& reason) {
                    setDetail(QString(tr("Slave %1: %2 → %3 failed — %4")).arg(pos).arg(from).arg(to).arg(reason),
                              kWarningColor);
                });
    }
}

// ── Identity ──────────────────────────────────────────────────────────
QString StateMachinePlugin::id() const {
    return "statemachine";
}
QString StateMachinePlugin::displayName() const {
    return "State Machine";
}
QString StateMachinePlugin::displayNameZh() const {
    return QStringLiteral("状态机");
}
QIcon StateMachinePlugin::icon() const {
    return QIcon::fromTheme("media-seek-forward");
}
int StateMachinePlugin::defaultOrder() const {
    return 60;
}
bool StateMachinePlugin::visible() const {
    return true;
}

void StateMachinePlugin::activate() {}
void StateMachinePlugin::deactivate() {}
void StateMachinePlugin::onSettingsChanged(const AppSettings&) {}
void StateMachinePlugin::onConnectionChanged(bool) {}

QWidget* StateMachinePlugin::widget() {
    return containerWidget_;
}

// ── UI construction ───────────────────────────────────────────────────
void StateMachinePlugin::buildUi() {
    containerWidget_ = new QWidget;
    auto* layout = new QVBoxLayout(containerWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    table_ = new QTableWidget;
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(table_);

    summaryLabel_ = new QLabel;
    summaryLabel_->setWordWrap(true);
    layout->addWidget(summaryLabel_);

    detailLabel_ = new QLabel;
    detailLabel_->setWordWrap(true);
    layout->addWidget(detailLabel_);
}

// ── Table population ──────────────────────────────────────────────────
void StateMachinePlugin::setRows(const QStringList& headers, const QList<QStringList>& rows) {
    table_->setColumnCount(headers.size());
    table_->setHorizontalHeaderLabels(headers);
    table_->setRowCount(rows.size());

    for (int r = 0; r < rows.size(); ++r) {
        const QStringList& cols = rows[r];
        for (int c = 0; c < cols.size() && c < headers.size(); ++c) {
            table_->setItem(r, c, new QTableWidgetItem(cols[c]));
        }
    }
    table_->resizeColumnsToContents();
}

// ── Live state update ──────────────────────────────────────────────────
void StateMachinePlugin::updateStateCell(int position, int state) {
    // Find the row for this slave position and update the state column.
    for (int r = 0; r < table_->rowCount(); ++r) {
        auto* item = table_->item(r, 0);
        if (item && item->text().toInt() == position) {
            if (table_->columnCount() > 1)
                table_->item(r, 1)->setText(stateToString(state));
            // Highlight updated row briefly
            for (int c = 0; c < table_->columnCount(); ++c) {
                auto* cell = table_->item(r, c);
                if (cell)
                    cell->setBackground(QColor(kInfoColor).lighter(180));
            }
            QTimer::singleShot(500, this, [this, r]() {
                for (int c = 0; c < table_->columnCount(); ++c) {
                    auto* cell = table_->item(r, c);
                    if (cell)
                        cell->setBackground(QColor(Qt::transparent));
                }
            });
            break;
        }
    }
}

QString StateMachinePlugin::stateToString(int state) const {
    switch (state) {
        case 1:
            return tr("INIT");
        case 2:
            return tr("PREOP");
        case 4:
            return tr("SAFEOP");
        case 8:
            return tr("OP");
        default:
            return tr("UNKNOWN");
    }
}

// ── Summary label ─────────────────────────────────────────────────────
void StateMachinePlugin::setSummary(const QString& text, const QString& severity) {
    summaryLabel_->setText(text);
    if (!severity.isEmpty()) {
        summaryLabel_->setProperty("severity", severity);
    }
}

void StateMachinePlugin::setSummaryToolTip(const QString& tip) {
    summaryLabel_->setToolTip(tip);
}

// ── Detail label ──────────────────────────────────────────────────────
void StateMachinePlugin::setDetail(const QString& text, const QString& severity) {
    detailLabel_->setText(text);
    if (!severity.isEmpty()) {
        detailLabel_->setProperty("severity", severity);
    }
}

void StateMachinePlugin::setDetailToolTip(const QString& tip) {
    detailLabel_->setToolTip(tip);
}

// ── Selection ─────────────────────────────────────────────────────────
int StateMachinePlugin::currentRow() const {
    return table_->currentRow();
}

void StateMachinePlugin::setCurrentCell(int row, int column) {
    table_->setCurrentCell(row, column);
}

int StateMachinePlugin::rowCount() const {
    return table_->rowCount();
}

bool StateMachinePlugin::isRowHidden(int row) const {
    return table_->isRowHidden(row);
}

void StateMachinePlugin::resizeColumnsToContents() {
    table_->resizeColumnsToContents();
}

// ── Accessors ─────────────────────────────────────────────────────────
QTableWidget* StateMachinePlugin::table() const {
    return table_;
}
QLabel* StateMachinePlugin::summaryLabel() const {
    return summaryLabel_;
}
QLabel* StateMachinePlugin::detailLabel() const {
    return detailLabel_;
}
