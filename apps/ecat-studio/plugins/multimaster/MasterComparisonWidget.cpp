#include "MasterComparisonWidget.h"
#include "services/MultiMasterService.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

MasterComparisonWidget::MasterComparisonWidget(QWidget *parent)
    : QWidget(parent) {
  buildUi();
}

void MasterComparisonWidget::buildUi() {
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(8, 8, 8, 8);
  mainLayout->setSpacing(8);

  auto *selectorLayout = new QHBoxLayout;
  selectorLayout->setSpacing(12);

  selectorLayout->addWidget(new QLabel(tr("Left Master:")));
  leftCombo_ = new QComboBox;
  selectorLayout->addWidget(leftCombo_);

  selectorLayout->addWidget(new QLabel(tr("Right Master:")));
  rightCombo_ = new QComboBox;
  selectorLayout->addWidget(rightCombo_);

  mergeBtn_ = new QPushButton(tr("Merge Left → Right"));
  selectorLayout->addWidget(mergeBtn_);

  selectorLayout->addStretch();
  mainLayout->addLayout(selectorLayout);

  auto *tablesLayout = new QHBoxLayout;
  tablesLayout->setSpacing(8);

  leftTable_ = new QTableWidget;
  leftTable_->setColumnCount(2);
  leftTable_->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  leftTable_->horizontalHeader()->setStretchLastSection(true);
  leftTable_->verticalHeader()->setVisible(false);
  leftTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  leftTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tablesLayout->addWidget(leftTable_);

  rightTable_ = new QTableWidget;
  rightTable_->setColumnCount(2);
  rightTable_->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  rightTable_->horizontalHeader()->setStretchLastSection(true);
  rightTable_->verticalHeader()->setVisible(false);
  rightTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  rightTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tablesLayout->addWidget(rightTable_);

  mainLayout->addLayout(tablesLayout, 2);

  auto *diffHeader = new QHBoxLayout;
  diffHeader->addWidget(new QLabel(tr("Differences:")));
  diffCountLabel_ = new QLabel(tr("0 differences"));
  diffCountLabel_->setStyleSheet("font-weight: bold; color: #ff9800;");
  diffHeader->addWidget(diffCountLabel_);
  diffHeader->addStretch();
  mainLayout->addLayout(diffHeader);

  diffTable_ = new QTableWidget;
  diffTable_->setColumnCount(4);
  diffTable_->setHorizontalHeaderLabels(
      {tr("Property"), tr("Left Value"), tr("Right Value"), tr("Status")});
  diffTable_->horizontalHeader()->setStretchLastSection(true);
  diffTable_->verticalHeader()->setVisible(false);
  diffTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  diffTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  diffTable_->setAlternatingRowColors(true);
  mainLayout->addWidget(diffTable_, 1);

  connect(mergeBtn_, &QPushButton::clicked, this, [this]() {
    if (leftMasterId_ >= 0 && rightMasterId_ >= 0) {
      emit mergeRequested(leftMasterId_, rightMasterId_);
    }
  });
}

void MasterComparisonWidget::setLeftMaster(const MmMasterInfo &info,
                                           const MmMasterStatus &status) {
  leftMasterId_ = info.masterId;
  leftTable_->setRowCount(7);

  auto setRow = [&](int row, const QString &prop, const QString &val) {
    leftTable_->setItem(row, 0, new QTableWidgetItem(prop));
    leftTable_->setItem(row, 1, new QTableWidgetItem(val));
  };

  setRow(0, tr("Master ID"), QString::number(info.masterId));
  setRow(1, tr("Adapter"), info.adapterName);
  setRow(2, tr("Slave Count"), QString::number(info.slaveCount));
  setRow(3, tr("State"), QString::number(static_cast<int>(info.state)));
  setRow(4, tr("IP Address"), info.ipAddress);
  setRow(5, tr("MAC Address"), info.macAddress);
  setRow(6, tr("Error Count"), QString::number(status.errorCount));

  updateDiffHighlight();
}

void MasterComparisonWidget::setRightMaster(const MmMasterInfo &info,
                                            const MmMasterStatus &status) {
  rightMasterId_ = info.masterId;
  rightTable_->setRowCount(7);

  auto setRow = [&](int row, const QString &prop, const QString &val) {
    rightTable_->setItem(row, 0, new QTableWidgetItem(prop));
    rightTable_->setItem(row, 1, new QTableWidgetItem(val));
  };

  setRow(0, tr("Master ID"), QString::number(info.masterId));
  setRow(1, tr("Adapter"), info.adapterName);
  setRow(2, tr("Slave Count"), QString::number(info.slaveCount));
  setRow(3, tr("State"), QString::number(static_cast<int>(info.state)));
  setRow(4, tr("IP Address"), info.ipAddress);
  setRow(5, tr("MAC Address"), info.macAddress);
  setRow(6, tr("Error Count"), QString::number(status.errorCount));

  updateDiffHighlight();
}

void MasterComparisonWidget::clearComparison() {
  leftTable_->setRowCount(0);
  rightTable_->setRowCount(0);
  diffTable_->setRowCount(0);
  diffCountLabel_->setText(tr("0 differences"));
  leftMasterId_ = -1;
  rightMasterId_ = -1;
}

int MasterComparisonWidget::differenceCount() const {
  return diffTable_->rowCount();
}

void MasterComparisonWidget::updateDiffHighlight() {
  if (leftTable_->rowCount() == 0 || rightTable_->rowCount() == 0) return;

  int diffCount = 0;
  diffTable_->setRowCount(0);

  int rows = qMin(leftTable_->rowCount(), rightTable_->rowCount());
  for (int i = 0; i < rows; ++i) {
    QString leftVal = leftTable_->item(i, 1) ? leftTable_->item(i, 1)->text() : "";
    QString rightVal = rightTable_->item(i, 1) ? rightTable_->item(i, 1)->text() : "";
    QString prop = leftTable_->item(i, 0) ? leftTable_->item(i, 0)->text() : "";

    if (leftVal != rightVal) {
      int row = diffTable_->rowCount();
      diffTable_->insertRow(row);
      diffTable_->setItem(row, 0, new QTableWidgetItem(prop));
      diffTable_->setItem(row, 1, new QTableWidgetItem(leftVal));
      diffTable_->setItem(row, 2, new QTableWidgetItem(rightVal));

      auto *statusItem = new QTableWidgetItem(tr("Different"));
      statusItem->setBackground(QColor("#fff3e0"));
      diffTable_->setItem(row, 3, statusItem);
      diffCount++;
    }
  }

  diffCountLabel_->setText(tr("%1 differences").arg(diffCount));
  if (diffCount > 0) {
    diffCountLabel_->setStyleSheet("font-weight: bold; color: #ff9800;");
  } else {
    diffCountLabel_->setStyleSheet("font-weight: bold; color: #4caf50;");
  }
}
